/*
 * MidiHelpers.c
 *
 * Created: 11.08.2017 22:43:03
 *  Author: Sergey
 */ 

#include "MidiHelpers.h"
#include "settings.h"

void sendBankSelectMessage(uint16_t bankNum, uint8_t ChNum, BankSelectMessType bsType)
{
	switch(bsType)
	{
		case MSB:
			midiSendControlChange(BANK_SEL_CONTROLLER_MSB, (bankNum & 0xFF), ChNum);
			break;

		case LSB:
			midiSendControlChange(BANK_SEL_CONTROLLER_LSB, (bankNum & 0xFF), ChNum);
			break;

		case MSB_AND_LSB:
			midiSendControlChange(BANK_SEL_CONTROLLER_MSB, ((bankNum >> 8) & 0xFF), ChNum);
			midiSendControlChange(BANK_SEL_CONTROLLER_LSB, (bankNum & 0xFF), ChNum);
			break;
	
		default : break;
	}
}

void sendPcWithOptionalBs(uint8_t progNum, uint16_t bankNum, uint8_t ChNum, BankSelectMessType bsType, UseBankSelectMess useBankSelectMess)
{
	if (useBankSelectMess == USE_BANK_SELECT)
		sendBankSelectMessage(bankNum, ChNum, bsType);

	midiSendProgramChange(progNum,ChNum);
}