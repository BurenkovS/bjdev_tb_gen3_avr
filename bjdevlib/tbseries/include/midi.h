/*
 * BJ Devices Travel Box series midi controller library
 * @file	midi.h
 * 
 * @brief	Send and receive midi messages. Running status is not supported in this version,
 *			running status messages will ignored!!				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */ 


#ifndef midi_h_
#define midi_h_

#include <stdint.h>
#include <stdbool.h>

/*
 * @brief	UART and maintenance initialization 
 */
void initMidi();

/*
 * @brief	Send program change midi message
 * @param	progNum - midi program number
 * @param	chanNum - midi channel  
 */
void midiSendProgramChange(uint8_t progNum, uint8_t chanNum);

/*
 * @brief	Send control change midi message
 * @param	ctrlNum - midi continuous controller number
 * @param	val - controller value
 * @param	chanNum - midi channel   
 */
void midiSendControlChange(uint8_t ctrlNum, uint8_t val, uint8_t chanNum);

/*
 * @brief	Send NRPN message
 * @param	ctrlMsb - midi NRPN controller number MSB
 * @param	ctrlLsb - midi NRPN controller number LSB
 * @param	valMsb - NRPN controller value MSB
 * @param	valLsb - NRPN controller value LSB
 * @param	chanNum - midi channel   
 */
void midiSendNrpn(uint8_t ctrlMsb,uint8_t ctrlLsb, uint8_t valMsb, uint8_t valLsb, uint8_t chanNum);

/*
 * @brief	Send NOTE ON and NOTE OFF message
 * @param	isOn - if TRUE - send note on, else note off
 * @param	noteNum - note number
 * @param	noteVel - note velocity
 * @param	chanNum - midi channel   
 */
void midiSendNote(bool isOn, uint8_t noteNum, uint8_t noteVel, uint8_t chanNum);

/*
 * @brief	Send system exclusive midi message.  
 *			Start byte (F0 hex) and an end byte (F7 hex) will automatically included and should not passed in payload 
 * @param	length - data length
 * @param	data - payload  
 */
void midiSendSysEx(uint16_t length, uint8_t* data);

/*
 * @brief	Send system exclusive midi message with manufacturer id.  
 *			Start byte (F0 hex) and an end byte (F7 hex) will automatically included and should not passed in payload 
 * @param	manfId - 3 byte format manufacturer id
 *			Most significant byte [31:25] will ignored
 *			[24:16] - manufacturer id byte 1
 *			[15:8] - manufacturer id byte 2
 *			[7:0] - manufacturer id byte 3
 * @param	length - data length without manufacturer id
 * @param	data - payload  without manufacturer id
 */
void midiSendSysExManfId(uint32_t manfId, uint16_t length, uint8_t* data);

/*
 * @brief	Read UART buffer and check midi messages. 
 *			Should be invoked in main loop. Running status is not supported in this version,
 *			running status messages will ignored!!
 * @return	true if valid midi message received
 */
bool midiRead(uint8_t interfaceNum, uint8_t* midiBuffer, uint8_t* midiMessageType, uint16_t* midiCmdLength);

/*
 * @brief	Register callback will invoked if control change message received
 */
void midiRegisterControlChangeCallback(void (*callback)(uint8_t channel, uint8_t ccNum, uint8_t ccVal));

/*
 * @brief	Register callback will invoked if program change message received
 */
void midiRegisterProgramChangeCallback(void (*callback)(uint8_t channel, uint8_t program));

/*
 * @brief	Register callback will invoked if system exclusive message received
 */
void midiRegisterSysExCallback(void (*callback)(uint16_t length));

/*
 * @brief	Register callback will invoked if active sense message received
 */
void midiRegisterActiveSenseCallback(void (*callback)(void));

/*
 * @return	Midi channel number in last input midi message
*/
uint8_t midiGetChannelNumber(uint8_t* midiBuffer); 

/*
 * @return	Midi message type
*/
uint8_t midiGetMessageType(uint8_t* midiBuffer); 

/*
 * @return	Midi program number in last input program change midi message
*/
uint8_t midiGetProgramNumber(uint8_t* midiBuffer);

/*
 * @return	Midi continuous controller number in last input midi control change message
*/
uint8_t midiGetControllerNumber(uint8_t* midiBuffer);

/*
 * @return	Midi continuous controller value in last input midi control change message
*/
uint8_t midiGetControllerValue(uint8_t* midiBuffer);

/*
 * @return	System exclusive payload length from any SysEx message
*/
uint16_t midiGetSysExLength(uint8_t* sysEx);

/*
 * @return	System exclusive payload length from last SysEx message
*/
//uint16_t midiGetLastSysExLength(uint8_t* midiBuffer);

/*
 * @return	Pointer to system exclusive payload data
*/
uint8_t* midiGetLastSysExData(uint8_t* midiBuffer);

/*
 * @return	3 byte format manufacturer id from SysEx message. 
 * 			Most significant byte in return value is always 0,
 *			[24:16] - manufacturer id byte 1
 *			[15:8] - manufacturer id byte 2
 *			[7:0] - manufacturer id byte 3
 */
uint32_t midiGetSysExManufacturerId(uint8_t* sysEx);

// midi events
#define MIDI_NOTE_OFF       0x80
#define MIDI_NOTE_ON        0x90
#define MIDI_POLY_TOUCH     0xA0
#define MIDI_CONTROL_CHANGE 0xB0
#define MIDI_PROGRAM_CHANGE 0xC0
#define MIDI_CHANNEL_TOUCH  0xD0
#define MIDI_PITCH_BEND     0xE0

// midi system events
#define MIDI_SYSEX_START        0xF0
#define MIDI_SYSEX_END          0xF7
#define MIDI_MTC                0xF1
#define MIDI_SPP                0xF2
#define MIDI_SONG_SEL           0xF3
#define MIDI_TUNE_REQ           0xF6
#define MIDI_CLOCK              0xF8
#define MIDI_SYNC               0xF9
#define MIDI_START              0xFA
#define MIDI_STOP               0xFC
#define MIDI_CONT               0xFB
#define MIDI_SENSE              0xFE
#define MIDI_RESET              0xFF
// unknown event
#define MIDI_UNKNOWN            0x00


#define SYSEX_END		0xF7 //End of System exclusive message

//midi buffer size in bytes
#define MIDI_BUFFER_SIZE	256

#define TOTAL_INTERFACES 2
#define MIDI_INTERFACE 0 //Define interface numbers
#define USB_INTERFACE 1

//NRPN defines
#define NRPN_CONTROLLER_MSB 99
#define NRPN_CONTROLLER_LSB 98
#define NRPN_VALUE_MSB 6
#define NRPN_VALUE_LSB 38
 

#endif /* midi_h_ */