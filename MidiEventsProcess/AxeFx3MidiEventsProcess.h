/*
 * AxeFx3MidiEventsProcess.h
 *
 * Created: 22.11.2018 0:04:22
 *  Author: Sergey
 */ 


#ifndef AXEFX3MIDIEVENTSPROCESS_H_
#define AXEFX3MIDIEVENTSPROCESS_H_

#include <stdint.h>

//Check AxeFx model
#define AXEFX_3_MODEL_CODE 0x10//AXFX III code

//Tuner
#define AXEFX3_NOTE_OFFSET 5 //note offset in tuner note sysex
#define AXEFX3_TUNE_OFFSET 7 //high tune offset in tuner tune sysex

//Preset name
#define AXEFX3_PRESET_NAME_OFFSET 7 //Axe fx preset name offset
#define MAX_AXEFX3_STRING_LENGTH 15 //Max string length acceptable from AxeFx3

//Maintenance
#define AXEFX3_FCODE_OFFSET 4 //function ID offset in SysEx message

//Supported function ID
#define AXEFX3_PATCH_NAME_FUNCTION_ID		0x0D
#define AXEFX3_STATUS_DUMP_COMMAND			0x13
#define AXEFX3_TAP_EVENT_ID      0x10//Tap event
#define AXEFX3_TUNER_FUNCTION_ID 0x11 //Tuner function ID


void handleMidiEventAxeFx3(uint8_t in_MessType
,uint8_t* midiMessage
,uint8_t sys_ex_length);

//requests to AxeFxIII
void axefx3QueryPatchName();


#endif /* AXEFX3MIDIEVENTSPROCESS_H_ */