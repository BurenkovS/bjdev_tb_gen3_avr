/*
 * AxeFxMidiEventsProcess.h
 *
 * Created: 29.08.2017 16:48:34
 *  Author: Sergey
 */ 


#ifndef AXEFXMIDIEVENTSPROCESS_H_
#define AXEFXMIDIEVENTSPROCESS_H_

#include <stdint.h>

//Check AxeFx model
#define AXEFX_MODEL_INFO_OSFFSET 0x03//Model code offset
#define AXEFX_MODEL_CODE 0x00//AXEFX standard code
#define AXEFX_ULTRA_MODEL_CODE 0x01//AXEFX ultra code
#define AXEFX_2_MODEL_CODE 0x03//AXEFX2 code
#define AXEFX_2_XL_MODEL_CODE 0x06//AXEFX2 XL code
#define AXEFX_2_XL_PLUS_MODEL_CODE 0x07 //AXEFX2 XL plus code
#define AX8_MODEL_CODE 0x08 //AX8 code


//Tuner
#define AXEFX_NOTE_OFFSET 5 //note offset in tuner note sysex
#define AXEFX_TUNE_OFFSET 7 //high tune offset in tuner tune sysex
#define AXEFX_TUNER_FUNCTION_ID 0x0D //Tuner function ID

//IA
#define IA_STATE_INFO_OFFSET 5 //Ofset of begin IA state info blocks
#define IA_STATE_BLOCK_SIZE 0x05 //Size of ia state information block in bytes
#define IA_STATE_AXEFX1_MSB_CC_NUM_OFFSET_INSIDE_BLOCK 3 //High part of CC number offset on IA state block for AXEFX1
#define IA_STATE_AXEFX1_LSB_CC_NUM_OFFSET_INSIDE_BLOCK 2 //Low part of CC number offset on IA state block  for AXEFX1
#define IA_STATE_AXEFX1_VAL_OFFSET_INSIDE_BLOCK 4 //IA state value offset for AXEFX1
#define IA_STATE_AXEFX2_MSB_CC_NUM_OFFSET_INSIDE_BLOCK 2 //High part of CC number offset on IA state block for AXEFX1
#define IA_STATE_AXEFX2_LSB_CC_NUM_OFFSET_INSIDE_BLOCK 1 //Low part of CC number offset on IA state block  for AXEFX1
#define IA_STATE_AXEFX2_VAL_OFFSET_INSIDE_BLOCK 0 //IA state value offset for AXEFX1

//Preset name
#define AXEFX_PRESET_NAME_OFFSET 5 //Axe fx preset name offset
#define MAX_AXEFX_STRING_LENGTH 15 ////Max string length acceptable from AxeFx

//Maintenance
#define AXEFX_FCODE_OFFSET 4 //function ID offset in SysEx message

//Supported function ID
#define IA_STATE_FUNCTION_ID    0x0E //IA state function ID
#define FIRMWARE_FUNCTION_ID    0x08//Get firmware version
#define PR_NAME_FUNCTION_ID     0x0F//Get preset name
#define AXEFX_TAP_EVENT_ID      0x10//Tap event

//Firmware version
#define FIRM_VERSION_MAJOR_INFO_OFFSET 5//Firmware Version major number
#define FIRM_VERSION_MINOR_INFO_OFFSET 6//Firmware Version minor number

void requestAxefxInfo(char rq_type);

/*
 * Process MIDI events for AxeFx
 */
void handleMidiEventAxeFx(uint8_t in_MessType
		,uint8_t* midiMessage
		,uint8_t sys_ex_length);

#endif /* AXEFXMIDIEVENTSPROCESS_H_ */