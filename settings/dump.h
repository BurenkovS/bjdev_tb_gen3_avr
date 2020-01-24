/*
 * dump.h
 *
 * Created: 15.05.2019 10:03:01
 *  Author: peter
 */ 


#ifndef DUMP_H_
#define DUMP_H_
#include <stdint.h>
#include <stdbool.h>
#include "settings.h"

#define MANUFACTURER_ID (0x007F7F)
#define NETWORK_NUMBER	(0x00)
#define MODEL_NUMBER	(0x00)

// типы сообщений
#define GLOBALS_MSG		(0x00)
#define BANKS_MSG		(0x01)
#define ACK_MSG			(0x02)
#define EOT_MSG			(0x03)
#define UNKNOWN_MSG     (0xFF)

/**************************************************************/
/// dump all settings
void save_All();

/**************************************************************/
/// read SysEx message from network, parse and apply to settings
///\param midiMsgType[in]		- message type, must be MIDI_SYSEX_END 
///\param midiMsg[in]			- midi message
///\param midiMsgLength[in]		- midi message length
///\param midiBuffLength[in]	- midi buffer length
///\return true if decoding is successful, else - false 
bool handleMidiSysExSettings( uint8_t midiMsgType, uint8_t * midiMsg, uint16_t midiMsgLength, uint16_t midiBuffLength);

#endif /* DUMP_H_ */