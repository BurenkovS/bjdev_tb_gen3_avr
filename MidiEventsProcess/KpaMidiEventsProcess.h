/*
 * KpaMidiEventsProcess.h
 *
 * Created: 29.08.2017 17:00:06
 *  Author: Sergey
 */ 


#ifndef KPAMIDIEVENTSPROCESS_H_
#define KPAMIDIEVENTSPROCESS_H_

#include <stdint.h>

//Beacone flags
/*?Flags?: Several bits that represent the Boolean value for specific options for the command or set. 
    Bit 0 ?INIT?: If the set should be initially sent after enabling the bidirectional mode (1)
    Bit 1 ?SYSEX?: If the set should send NRPN CC (0) or use SYSEX (1)2
    Bit 2 ?ECHO?: If set, parameters are sent back even if they were set via MIDI IN (1)
    Bit 3 ?NOFE?: if set, the KPA will stop sending 0xFE when protocol is initiated
    Bit 4 ?NOCTR?: if set, the KPA will not send the ?sensing? command(see Table 2)
    Bit 5 ?TUNEMODE?: if set, the Tuning information is only sent in Tuner Mode, otherwise it?s being sent all the time.
    Bit 6 *reserved* set to 0
*/
#define KPA_INIT        0x01
#define KPA_SYSEX       0x02
#define KPA_ECHO        0x04
#define KPA_NOFE        0x08
#define KPA_NOCTR       0x10
#define KPA_TUNEMODE    0x20

//function code
#define SINGLE_CHANGE 0x01 //Single Parameter Change
#define STRING 0x03 //String Parameter
#define EX_SINGLE_CHANGE 0x06//Extended Parameter Change
#define EX_STRING 0x07// Extended String Parameter Change

//Parameters numbers
#define NOTE 0x7D54 //Current note
#define TUNING 0x7C0F //Current tuning
#define RIG_NAME 0x0001 // Rig Name
#define KPA_PERFOMANCE_NAME 0x00010000 //Performance Name. Warning!! Parameter does not converted from 8 to 7 bit representation!!
#define KPA_TAP_EVENT 0x7C00 //Tap event
#define MAIN_MODE 0x7F7D //Selected Main Mode (0=Browse/1=Perform)


//Generic values offsets for Single Parameter Change
#define SINGLE_CHANGE_LSB_OFFSET 10
#define SINGLE_CHANGE_MSB_OFFSET 9
#define EX_SINGLE_CHANGE_LSB_OFFSET 16

//Maintenance
#define FCODE_OFFSET 5 //function code offset in SysEx message
#define STRING_OFFSET 9 //string payload offset in SysEx message(function code STRING 0x03)
#define EX_STRING_OFFSET 12 //
#define IA_STATE_VALUE_OFFSET 10 //IA state value (on/off) offset (function code SINGLE_CHANGE 0x01)
#define NOTE_OFFSET 10 //note offset in tuner note sysex 
#define H_TUNE_OFFSET 9 //high tune offset in tuner tune sysex
#define MAX_KPA_STRING_LENGTH 15//Max string length acceptable from KPA.
#define MAX_KPA_SLOTS 5//number of slots in KPA.

//CC numbers for dedicated IA
#define CC_STOMP_A      17 //Stomp A
#define CC_STOMP_B      18 //Stomp B
#define CC_STOMP_C      19 //Stomp C
#define CC_STOMP_D      20 //Stomp D
#define CC_STOMP_X      22 //Effect X
#define CC_MOD          24 //Effect Mod
#define CC_DELAY        26 //Delay
#define CC_DELAY_SPOL   27 //Delay (with spillover)
#define CC_REVERB       28 //Reverb
#define CC_REVERB_SPOL  29 //Reverb (with spillover)


//NRPN parameters(IA state) numbers for dedicated IA
#define NRPN_STOMP_A        0x3203 //Stomp A
#define NRPN_STOMP_B        0x3303 //Stomp B
#define NRPN_STOMP_C        0x3403 //Stomp C
#define NRPN_STOMP_D        0x3503 //Stomp D
#define NRPN_STOMP_X        0x3803 //Effect X
#define NRPN_MOD            0x3A03 //Effect Mod
#define NRPN_DELAY          0x4A02 //Delay
#define NRPN_DELAY_SPOL     0x4A02 //Delay (with spillover)

//#define NRPN_REVERB         0x4B02 //Reverb
//#define NRPN_REVERB_SPOL    0x4B02 //Reverb (with spillover)
//Reverb ID change in version 5.7
#define NRPN_REVERB         0x3D03 //Reverb
#define NRPN_REVERB_SPOL    0x3D03 //Reverb (with spillover)

/*
* Send string "BJ Devices board" to shown on KPA
*/
//void send_name_to_kpa();

/*
* Send beacon message
* flags - beacon flags. See beacon description
* time - repeat time. See beacon description
*/
//void send_beacon_to_kpa(const uint8_t flags, const uint8_t time);


/*
* is_kpa_message
* check current message was sent by KPA
* char* message - input parameter. Pointer on SysEx message
* return 1 if parsing success, 0 otherwise.
*/
//uint8_t is_kpa_message(const char* message);

/*
* parce_name_for_display
* parce rig name, slots names, current active slot and current mode
* and fill "name" parameter by rig name if mode == browse
* and active slot name if mode == Perform
* char* message - input parameter. Pointer on SysEx message
* char* name - output parameter. Result string will placed in this pointer
* return 1 if parsing success, 0 otherwise.
*/
//uint8_t parce_name_for_display(const char* message, char* name);

/*
* parce_kpa_tuner
* char* message - input parameter. Pointer on SysEx message
* note - output parameter. Note number is in G-major format
* tune - output parameter. Tuning is in G-major format
* return 1 if parsing success, 0 otherwise.
*/ 
//uint8_t parce_kpa_tuner(const char* message, uint8_t* note, uint8_t* tune);

/*
* update_kpa_ia_state
* if input sysex is state of StompA,B,C,D,X, MOD, delay or reverb
* and if some button have according IA numbers
* update state of this Fx
* 
* char* message - input parameter. Pointer on SysEx message
* char* ia_nums - input parameter. CC numbers of all buttons(Fx's)
* char* ia_state - output parameter. Current sttae of all Fx's
* return 1 if parsing success, 0 otherwise.
*/ 
//uint8_t update_kpa_ia_state(const char* message);

/*
* is_kpa_tap
* check current message is Tap event
* char* message - input parameter. Pointer on SysEx message
* return 1 if massage is tap, 0 otherwise.
*/
//uint8_t is_kpa_tap(const char* message);

/*
 * Process MIDI events for KPA
 */
void handleMidiEventKpa(uint8_t in_MessType ,uint8_t* midiMessage);


#endif /* KPAMIDIEVENTSPROCESS_H_ */