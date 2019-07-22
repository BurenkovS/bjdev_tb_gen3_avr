/*
 * MidiHelpers.h
 * Add some MIDI functions, which are specific for TB-series project
 * Created: 11.08.2017 22:43:15
 *  Author: Sergey
 */
 
#include "bjdevlib_tb.h"
#include "settings.h"

#ifndef MIDIHELPERS_H_
#define MIDIHELPERS_H_

#define MAX_MIDI_CHANNEL_NUMBER 15
#define MIDI_CHANNEL_PRIMARY (MAX_MIDI_CHANNEL_NUMBER + 1)//If this number set as MIDI channel - channel number will taken from global setings
#define MIDI_CHANNEL_NOT_ACTIVE (MAX_MIDI_CHANNEL_NUMBER + 2)//If this number set as MIDI channel - no midi message will be send
#define MAX_PROGRAM_CHANGE_MESSAGE_VALUE 127
#define CC_OFF_VALUE 128
#define BANK_SEL_CONTROLLER_MSB 0x00
#define BANK_SEL_CONTROLLER_LSB 0x20

#define IA_STATE_ON 0x7F
#define IA_STATE_OFF 0x00
/*
 * Send Bank Select MIDI message depends on global settings
*/
void sendBankSelectMessage(uint16_t bankNum, uint8_t ChNum, BankSelectMessType bsType);

/*
 * Send Program change and when BS, if needed
*/
void sendPcWithOptionalBs(uint8_t progNum, uint16_t bankNum, uint8_t ChNum, BankSelectMessType bsType, UseBankSelectMess useBankSelectMess);





#endif /* MIDIHELPERS_H_ */