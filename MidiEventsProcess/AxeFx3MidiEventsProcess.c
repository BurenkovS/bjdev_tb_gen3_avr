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

uint8_t queryPatchName[10]  = {0xF0, 0x00, 0x01, 0x74, 0x10, AXEFX3_PATCH_NAME_FUNCTION_ID, 0x7F, 0x7F, 0x18, 0xF7};
void axefx3QueryPatchName()
{
	uint8_t i;
	for(i = 0; i < sizeof(queryPatchName); ++i)
		uart0PutChar(pgm_read_byte(&queryPatchName[i]));
}


void handleMidiEventAxeFx3(uint8_t in_MessType
        ,uint8_t* midiMessage
        ,uint8_t sys_ex_length)
{
    uint8_t* sys_ex_data = midiMessage + 1;//just remove status byte
    if (in_MessType == MIDI_SYSEX_START && global.targetDevice != TARGET_DEVICE_OFF && isAxefx3Message(sys_ex_data))//if SysEx is enabled
    {
	    if (runtimeEnvironment.isAxeFx3Connected_ == 0 && global.targetDevice == TARGET_DEVICE_AUTO)//If axe not connected  and auto detection enabled
	    {
            runtimeEnvironment.isAxeFx3Connected_ = true;
            LCDClear();
		    LCDWriteString("AxeFX III");
			setTaskMessageOff();
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

