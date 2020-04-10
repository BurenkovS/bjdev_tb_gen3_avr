/*
 * AxeFx3MidiEventsProcess.c
 *
 * Created: 22.11.2018 0:04:10
 *  Author: Sergey
 */ 

#include "AxeFx3MidiEventsProcess.h"
#include "settings.h"
#include "bjdevlib_tb.h"
#include "MidiHelpers.h"
#include "RuntimeEnvironment.h"
#include "PedalLedProcess.h"
#include "TaskManager.h"
#include "lcd_tb.h"

#include <avr/pgmspace.h>
#include <stdint.h>

uint8_t isAxefx3Message(uint8_t* message)
{
    if(*message == 0x00 && *(message + 1) == 0x01 && *(message + 2) == 0x74 && *(message + 3) == AXEFX_3_MODEL_CODE)//AxeFx Manufacturer ID
		return 1;
	else
		return 0;
}

const uint8_t convAxe3ToGMajorNote[12] PROGMEM = {0x9,0xA,0xB,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8}; 

const uint8_t convAxe3ToGMajorTune[128] PROGMEM  = {	16,16,16,16,16,16,16,16,16,16,16,
													16,16,16,16,16,16,16,16,16,16,
													17,17,17,17,17,17,17,17,17,
													18,18,18,18,18,18,18,
													19,19,19,19,19,19,
													20,20,20,20,20,20,
													21,21,21,21,21,21,
													22,22,22,22,22,22,
													23,23,23,//fine
													24,24,24,//fine
													25,25,25,25,25,25,
													26,26,26,26,26,26,
													27,27,27,27,27,27,
													28,28,28,28,28,28,
													29,29,29,29,29,29,29,
													30,30,30,30,30,30,30,30,30,
													31,31,31,31,31,31,31,31,31,31,
													31,31,31,31,31,31,31,31,31,31,31};



uint8_t parseAxefx3Tuner(const uint8_t *sys_ex_data, uint8_t* note, uint8_t* tune)
{
    if (*(sys_ex_data + AXEFX3_FCODE_OFFSET) != AXEFX3_TUNER_FUNCTION_ID)//Check message is tuner info
    return 0;

    if(*(sys_ex_data + AXEFX3_NOTE_OFFSET) > 0xb)//Check valid tuner parameters
    return 0;

    *note = pgm_read_byte(&convAxe3ToGMajorNote[sys_ex_data[AXEFX3_NOTE_OFFSET]]);//Convert to G-major note representation
    *tune = pgm_read_byte(&convAxe3ToGMajorTune[sys_ex_data[AXEFX3_TUNE_OFFSET]]);//Convert to G-major tune representation
    return 1;
}

uint8_t parceAxefx3PresetName(const uint8_t* message, char* name)
{
    uint8_t i;

    if (*(message + AXEFX3_FCODE_OFFSET) != AXEFX3_PATCH_NAME_FUNCTION_ID)
		return 0;

    for (i = AXEFX3_PRESET_NAME_OFFSET; *(message + i) != '\0'; ++i)
    {
	    *(name + i - AXEFX3_PRESET_NAME_OFFSET) = *(message + i);
	    
	    if(i >= AXEFX3_PRESET_NAME_OFFSET + MAX_AXEFX3_STRING_LENGTH)
			break;
    }
    *(name + i - AXEFX3_PRESET_NAME_OFFSET) = '\0';
		return 1;
}

uint8_t isAxefx3Tap(const uint8_t* message)
{
    if (*(message + AXEFX3_FCODE_OFFSET) != AXEFX3_TAP_EVENT_ID)
		return 0;
    else
		return 1;
}

uint8_t parseAxefx3IaState( uint8_t* sys_ex_data
							,uint8_t sys_ex_length)
{
	uint8_t i;
	uint8_t j;
	uint8_t MS_nibble;
	uint8_t LS_nibble;
	uint8_t ms_nible_index;
	uint8_t ls_nible_index;
	uint8_t ia_state_index;
	uint8_t ia_state;
	uint8_t nums_of_blocks;
	uint16_t effectId;

	if (*(sys_ex_data + AXEFX3_FCODE_OFFSET) != AXEFX3_STATUS_DUMP_COMMAND)//Check message is IA state info
		return 0;

	nums_of_blocks = (sys_ex_length - AXEFX3_STATUS_DUMP_OFFSET)/AXEFX3_STATUS_DUMP_SIZE;

	for (j = 0; j < nums_of_blocks; j++)
	{
		ms_nible_index = AXEFX3_STATUS_DUMP_SIZE*j + AXEFX3_STATUS_DUMP_OFFSET + 1;
		ls_nible_index = AXEFX3_STATUS_DUMP_SIZE*j + AXEFX3_STATUS_DUMP_OFFSET + 0;

		MS_nibble = *(sys_ex_data + ms_nible_index);
		LS_nibble = *(sys_ex_data + ls_nible_index);
		
		effectId = (LS_nibble | (MS_nibble << 6)) & 0x7F;

		//get state for this effect
		ia_state_index = AXEFX3_STATUS_DUMP_SIZE*j + AXEFX3_STATUS_DUMP_OFFSET + 2;
		ia_state = *(sys_ex_data + ia_state_index) & 0x01;
			
		for (i = 0; i <= FOOT_BUTTONS_NUM - 1; i++)//find ID in buttons
		{
			if (bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.vendorBlockId == (uint8_t)effectId)//
			{
				if (ia_state != 0)
					runtimeEnvironment.currentIaState_[i] = IA_STATE_OFF;
				else
					runtimeEnvironment.currentIaState_[i] = IA_STATE_ON;
			}
		}
	}
	return 1;
}

//The general format of the sysex commands is:
//F0 00 01 74 10 cc dd dd dd … cs F7
//where cc is the command opcode, dd is a variable number of bytes and cs is the XOR checksum.
//size = number of "dd" 
//data = pointer to dd dd dd...
uint8_t axefx3CommandCommonPart[5]  = {0xF0, 0x00, 0x01, 0x74, 0x10};
void axefx3SendCommand(uint8_t command, uint8_t *data, uint8_t size)
{
	uint8_t i;
	uint8_t summ = 0;
	for(i = 0; i < sizeof(axefx3CommandCommonPart); ++i)
	{
		uart0PutChar(axefx3CommandCommonPart[i]);
		summ ^= axefx3CommandCommonPart[i];
	}
	uart0PutChar(command);
	summ ^= command;

	for(i = 0; i < size; ++i)
	{
		uart0PutChar(data[i]);
		summ ^= data[i];
	}

	uart0PutChar(0x7F & summ);
	uart0PutChar(0xF7);
}

void axefx3QueryPatchName()
{
	uint8_t data[2] = {0x7F, 0x7F};
	axefx3SendCommand(AXEFX3_PATCH_NAME_FUNCTION_ID, data, 2);
}

void axefx3QueryStatusDump()
{
	uint8_t dummy;
	axefx3SendCommand(AXEFX3_STATUS_DUMP_COMMAND, &dummy, 0);
}


void handleMidiEventAxeFx3(uint8_t in_MessType
        ,uint8_t* midiMessage
        ,uint8_t sys_ex_length)
{
    uint8_t* sys_ex_data = midiMessage + 1;//just remove status byte
    if (in_MessType == MIDI_SYSEX_START && global.targetDevice != TARGET_DEVICE_OFF && isAxefx3Message(sys_ex_data))//if SysEx is enabled
    {
	    if (runtimeEnvironment.isAxeFx3Connected_ == 0)//If axe not connected  
		{
			if(global.targetDevice == TARGET_DEVICE_AUTO)//If auto detection enabled, fill all data from sysex
			{
				runtimeEnvironment.isAxeFx3Connected_ = true;
				LCDClear();
				LCDWriteString("AxeFX III");
				setTaskMessageOff();
			}
			else if(global.targetDevice == TARGET_DEVICE_AXEIII)//if device specified in settings, fill from settings
			{
				runtimeEnvironment.isAxeFx3Connected_ = true;
			}
		}
	    //If AxeFx3 connected
	    else
	    {
            if (parseAxefx3Tuner(sys_ex_data, &runtimeEnvironment.tunerNote_, &runtimeEnvironment.tunerTune_))
            {
	            //LCDWriteStringXY(0,0,"parseAxefxTuner");
	            drawPedalTuner(runtimeEnvironment.tunerTune_);
	            if(runtimeEnvironment.isTimeToShowTuner_ == true)
	            {
		            setEventByMask(EVENT_MASK_TUNER_CHANGE);
		            updateRequests.updateScreenRq_ = 1;
	            }
            }
			
			if(parseAxefx3IaState(sys_ex_data, sys_ex_length))
			{
				if (global.Show_pr_name != ONLY_BANK)//If preset name display required
				{
					axefx3QueryPatchName();
					updateRequests.updateLedsRq_ = 1;
				}
			}

            if (global.Show_pr_name != ONLY_BANK)//If preset name display required
            {
	            if (parceAxefx3PresetName(sys_ex_data, runtimeEnvironment.vendorPresetName_))
	            {
		            updateRequests.updateScreenRq_ = 1;
		            runtimeEnvironment.isTimeToShowPresetName_ = true;
		            if(global.Show_pr_name == PRESET_AND_BANK)
						setTaskSwitchBetweenPresetNameAndBankName();
	            }
            }

            if (isAxefx3Tap(sys_ex_data))
				runtimeEnvironment.showTapRq_ = 1;
	    }
    }
}

