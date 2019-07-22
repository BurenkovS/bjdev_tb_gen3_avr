/*
 * AxeFxMidiEventsProcess.c
 *
 * Created: 29.08.2017 16:48:17
 *  Author: Sergey
 */ 

#include "AxeFxMidiEventsProcess.h"
#include "settings.h"
#include "bjdevlib_tb.h"
#include "MidiHelpers.h"
#include "RuntimeEnvironment.h"
#include "PedalLedProcess.h"
#include "TaskManager.h"
#include "lcd_tb.h"

#include <avr/pgmspace.h>
#include <stdint.h>

uint8_t isAxefxMessage(uint8_t* message)
{
    if(*message == 0x00 && *(message + 1) == 0x01 && *(message + 2) == 0x74)//AxeFx Manufacturer ID
        return 1;
    else
        return 0;
}

const uint8_t convAxe2GMajorNote[12] PROGMEM = {0x9,0xA,0xB,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8};

const uint8_t convAxe2GMajorTune[99] PROGMEM =	{ 16,16,16,16,16,16,16,16,16,
											  17,17,17,17,17,17,17,17,
											  18,18,18,18,18,18,
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
											  29,29,29,29,29,29,
											  30,30,30,30,30,30,30,
											  31,31,31,31,31,31,31,31,31};


uint8_t parseAxefxTuner(const uint8_t *sys_ex_data, uint8_t* note, uint8_t* tune)
{
    if (*(sys_ex_data + AXEFX_FCODE_OFFSET) != AXEFX_TUNER_FUNCTION_ID)//Check message is tuner info
        return 0;

    if(*(sys_ex_data + AXEFX_NOTE_OFFSET) > 0xb || *(sys_ex_data + AXEFX_TUNE_OFFSET) < 14 || *(sys_ex_data + AXEFX_TUNE_OFFSET) > 112)//Check valid tuner parameters
        return 0;

    *note = pgm_read_byte(&(convAxe2GMajorNote[sys_ex_data[AXEFX_NOTE_OFFSET]]));//Convert to G-major note representation
    *tune = pgm_read_byte(&(convAxe2GMajorTune[sys_ex_data[AXEFX_TUNE_OFFSET] - 14]));//Convert to G-major tune representation
    return 1;
}


uint8_t parseAxefxIaState(  const uint8_t sys_ex_length
        ,const uint8_t* sys_ex_data
        ,const uint8_t axe_ver)
{
    uint8_t i;
    uint8_t j;
    uint8_t CC_num_from_AXE;
    uint8_t MS_nibble;
    uint8_t LS_nibble;
    uint8_t ms_nible_index;
    uint8_t ls_nible_index;
    uint8_t ia_state_index;
    uint8_t nums_of_blocks;

    if (*(sys_ex_data + AXEFX_FCODE_OFFSET) != IA_STATE_FUNCTION_ID)//Check message is IA state info
        return 0;

    nums_of_blocks = (sys_ex_length - IA_STATE_INFO_OFFSET)/IA_STATE_BLOCK_SIZE;

    if(axe_ver == AXEFX_MODEL_CODE || axe_ver == AXEFX_ULTRA_MODEL_CODE)//If Standard or Ultra
    {
        for (j = 0; j < nums_of_blocks; j++)
        {
            ms_nible_index = IA_STATE_BLOCK_SIZE*j + IA_STATE_INFO_OFFSET + IA_STATE_AXEFX1_MSB_CC_NUM_OFFSET_INSIDE_BLOCK;
            ls_nible_index = IA_STATE_BLOCK_SIZE*j + IA_STATE_INFO_OFFSET + IA_STATE_AXEFX1_LSB_CC_NUM_OFFSET_INSIDE_BLOCK;

            MS_nibble = *(sys_ex_data + ms_nible_index);//Get CC number
            LS_nibble = *(sys_ex_data + ls_nible_index);//
            CC_num_from_AXE = LS_nibble | (MS_nibble << 4);

            for (i = 0; i < FOOT_BUTTONS_NUM; i++)//find cc in
            {
				if (bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber == CC_num_from_AXE)//
                {
                    ia_state_index = IA_STATE_BLOCK_SIZE*j + IA_STATE_INFO_OFFSET + IA_STATE_AXEFX1_VAL_OFFSET_INSIDE_BLOCK;

					if (*(sys_ex_data + ia_state_index) == 0x00)
                        runtimeEnvironment.currentIaState_[i] = IA_STATE_OFF;//Fx is off
                    else
                       runtimeEnvironment.currentIaState_[i] = IA_STATE_ON;
                }
            }
        }
    }
    else //If gen 2 devices
    {
        for (j = 0; j < nums_of_blocks; j++)
        {
            ms_nible_index = IA_STATE_BLOCK_SIZE*j + IA_STATE_INFO_OFFSET + IA_STATE_AXEFX2_MSB_CC_NUM_OFFSET_INSIDE_BLOCK;
            ls_nible_index = IA_STATE_BLOCK_SIZE*j + IA_STATE_INFO_OFFSET + IA_STATE_AXEFX2_LSB_CC_NUM_OFFSET_INSIDE_BLOCK;

            MS_nibble = *(sys_ex_data + ms_nible_index);//Get CC number
            LS_nibble = *(sys_ex_data + ls_nible_index);//

            CC_num_from_AXE = ((LS_nibble >> 1) | (MS_nibble << 6)) & 0x7F;


            for (i = 0; i <= FOOT_BUTTONS_NUM - 1; i++)//find cc in
            {
                if (bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber == CC_num_from_AXE)//
                {
                    ia_state_index = IA_STATE_BLOCK_SIZE*j + IA_STATE_INFO_OFFSET + IA_STATE_AXEFX2_VAL_OFFSET_INSIDE_BLOCK;

                    if (*(sys_ex_data + ia_state_index) == 0x00 || *(sys_ex_data + ia_state_index) == 0x02)
                       runtimeEnvironment.currentIaState_[i] = IA_STATE_OFF;//Fx if in bypass. also ignore IA state for X layout
                    else
                        runtimeEnvironment.currentIaState_[i] = IA_STATE_ON;
                }
            }
        }
    }
    return 1;
}

void requestAxefxInfo(char rq_type)
{   
    uint8_t checksumm = 0;
	uint8_t model = runtimeEnvironment.currentAxeFxModel_;
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //FIND OUT NEWEST SOURCE TO WORKING WITH AXEFX ON
    //http://sourceforge.net/p/mtroll/mTroll/ci/default/tree/
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    //Message format:
    //0xF0 sysex start
    //0x00 Manf. ID byte0
    //0x01 Manf. ID byte1
    //0x74 Manf. ID byte2
    //0x01 Model #
    //0x0f Get Preset Name ID
    //0xF7 sysex end

    uart0PutChar(0xF0);
    uart0PutChar(0x00);
    uart0PutChar(0x01);
    uart0PutChar(0x74);
    uart0PutChar(model);//standard or ultra model or axe2 of axe2xl
    uart0PutChar(rq_type);
    checksumm = (0xF0^0x00^0x01^0x74^model^rq_type) & 0x7F;
    //send two extra bytes if GET_AXE_FIRMWARE request
    //(see http://wiki.fractalaudio.com/index.php?title=Axe-Fx_SysEx_Documentation#MIDI_GET_FIRMWARE_VERSION)
    if (rq_type == FIRMWARE_FUNCTION_ID)
    {
        if (model == 3)//If axe2
        {
            //Checksum
            //F0 00 01 74 03 0F F7
            //We would have to XOR all the byte values from the starting 'F0' to the '0F' which is the second last byte:
            //0xF0 ^ 0x00 ^ 0x01 ^ 0x74 ^ 0x03 ^ 0x0F = 0x89
            //Then, we would need to strip the leftmost bit from the result (by ANDing it to 0x7F):
            //0x89 && 0x7F = 0x09
            //And, we add this byte (actually, a septet now) to the end of the SYSEX string, BEFORE the terminating F7:
            uart0PutChar(0x0A);//Why 2 bytes?? fucking fractal!!
            uart0PutChar(0x04);
        } 

        else//If axe 1
        {
            uart0PutChar(0x00);
            uart0PutChar(0x00);
        }
    }
    uart0PutChar(checksumm);
    uart0PutChar(0xF7);
}

uint8_t parceAxefxModelFromSysex(const uint8_t *sys_ex_data, uint8_t* model)
{
    *model = *(sys_ex_data + AXEFX_MODEL_INFO_OSFFSET);
    return 1;
}

uint8_t parceAxefxPresetName(const uint8_t* message, char* name)
{
    uint8_t i;

    if (*(message + AXEFX_FCODE_OFFSET) != PR_NAME_FUNCTION_ID)
        return 0;
        
    for (i = AXEFX_PRESET_NAME_OFFSET; *(message + i) != '\0'; ++i)
    {
        *(name + i - AXEFX_PRESET_NAME_OFFSET) = *(message + i);
            
        if(i >= AXEFX_PRESET_NAME_OFFSET + MAX_AXEFX_STRING_LENGTH)
        break;
    }
    *(name + i - AXEFX_PRESET_NAME_OFFSET) = '\0';
    return 1;
}

uint8_t isAxefxTap(const uint8_t* message)
{
    if (*(message + AXEFX_FCODE_OFFSET) != AXEFX_TAP_EVENT_ID)
        return 0;
    else
        return 1;
}

uint8_t isAxefxInTargetSettings(uint8_t* model)
{
    uint8_t ret = 1;
    
    switch(global.targetDevice)
    {
        case TARGET_DEVICE_AXE_ST:
            *model = AXEFX_MODEL_CODE;
            break;

        case TARGET_DEVICE_AXE_ULTRA:
            *model = AXEFX_ULTRA_MODEL_CODE;
            break;

        case TARGET_DEVICE_AXEII:
            *model = AXEFX_2_MODEL_CODE;
            break;

        case TARGET_DEVICE_AXEII_XL:
            *model = AXEFX_2_XL_MODEL_CODE;
            break;
            
        case TARGET_DEVICE_AXEII_XL_PLUS:
            *model = AXEFX_2_XL_PLUS_MODEL_CODE;
            break;
            
        /*case TARGET_DEVICE_AX8:
            *model = AX8_MODEL_CODE;
            break;*/
            
        default: ret = 0;
    }
    return ret;
}

void handleMidiEventAxeFx(uint8_t in_MessType
        ,uint8_t* midiMessage
        ,uint8_t sys_ex_length)
{
    uint8_t* sys_ex_data = midiMessage + 1;//just remove status byte 
    if (in_MessType == MIDI_SYSEX_START && global.targetDevice != TARGET_DEVICE_OFF && isAxefxMessage(sys_ex_data))//if SysEx is enabled
    {
        if (runtimeEnvironment.isAxeFxConnected_ == 0 && global.targetDevice == TARGET_DEVICE_AUTO)//If axe not connected  and auto detection enabled 
        {
            if(parceAxefxModelFromSysex(sys_ex_data, &runtimeEnvironment.currentAxeFxModel_))
            {
                runtimeEnvironment.isAxeFxConnected_ = true;
                LCDClear();
                switch ((uint8_t)runtimeEnvironment.currentAxeFxModel_)
                {
                    case AXEFX_MODEL_CODE:
						LCDWriteString("AxeFX Standard");
                        break;
                                
                    case AXEFX_ULTRA_MODEL_CODE:
                        LCDWriteString("AxeFX Ultra");
                        break; 
                                
                    case AXEFX_2_MODEL_CODE:
                        LCDWriteString("AxeFX II");
                        break;
                                
                    case AXEFX_2_XL_MODEL_CODE:
                        LCDWriteString("AxeFX II XL");
                        break;
                            
                    case AXEFX_2_XL_PLUS_MODEL_CODE:
                        LCDWriteString("AxeFX II XL+");
                        break;
                            
                    case AX8_MODEL_CODE:
                        LCDWriteString("AX8");  
                        break;
                                    
                    default: break;
                }
				setTaskMessageOff();

                /* 
                 * Send request for IA states.
                 * After controller receive answer from AxeFx,
                 * it will send preset name request.
                 * So in this case preset will shown on screen after power ON(if axe fx is active)
                 */
                requestAxefxInfo(IA_STATE_FUNCTION_ID);
            }
        }
        //If AxeFx connected
        else
        {
            if (parseAxefxTuner(sys_ex_data, &runtimeEnvironment.tunerNote_, &runtimeEnvironment.tunerTune_))
            {
				//LCDWriteStringXY(0,0,"parseAxefxTuner");  
				drawPedalTuner(runtimeEnvironment.tunerTune_);
                if(runtimeEnvironment.isTimeToShowTuner_ == true)
				{
				   setEventByMask(EVENT_MASK_TUNER_CHANGE);
				   updateRequests.updateScreenRq_ = 1;
				}
            }

            if(parseAxefxIaState(sys_ex_length, sys_ex_data, runtimeEnvironment.currentAxeFxModel_))
            {
                if (global.Show_pr_name != 1)//If preset name display required
                    requestAxefxInfo(PR_NAME_FUNCTION_ID);//Send request after received IA states

                updateRequests.updateLedsRq_ = 1;

            }

            if (global.Show_pr_name != ONLY_BANK)//If preset name display required
            {
                if (parceAxefxPresetName(sys_ex_data, runtimeEnvironment.vendorPresetName_))
                {
                    updateRequests.updateScreenRq_ = 1;
                    runtimeEnvironment.isTimeToShowPresetName_ = true;
					if(global.Show_pr_name == PRESET_AND_BANK)
						setTaskSwitchBetweenPresetNameAndBankName();
                }
            }

            if (isAxefxTap(sys_ex_data))
                runtimeEnvironment.showTapRq_ = 1;
        }
    }
}
