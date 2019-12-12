/*
 * ExpressionProcess.c
 *
 * Created: 24.10.2017 23:42:07
 *  Author: Sergey
 */ 
#include "ExpressionProcess.h"
#include "settings.h"
#include "RuntimeEnvironment.h"
#include "MidiHelpers.h"
#include "bjdevlib_tb.h"
#include "log.h"

#include <avr/pgmspace.h>

static const uint8_t tableLog10[128] PROGMEM = {0,9,18,29,36,42,47,51,55,58,60,63,65,67,69,71,73,74,76,77,79,80,81,82,83,84,85,86,87,88,89,90,91,
												92,92,93,94,95,95,96,97,97,98,99,99,100,100,101,101,102,103,103,104,104,105,105,106,106,106,107,
												107,108,108,109,109,109,110,110,111,111,111,112,112,112,113,113,114,114,114,115,115,115,116,116,
												116,116,117,117,117,118,118,118,119,119,119,119,120,120,120,120,121,121,121,122,122,122,122,123,
												123,123,123,123,124,124,124,124,125,125,125,125,126,126,126,126,126,127,127,127};

static const uint8_t tableAntilog10[128] PROGMEM = {0,0,0,1,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,4,5,5,5,5,6,6,6,7,7,7,7,8,8,8,8,9,9,9,10,10,10,11,11,11,
													11,12,12,12,13,13,13,14,14,15,15,15,16,16,16,17,17,18,18,18,19,19,20,20,21,21,21,22,22,23,23,24,24,
													25,26,26,27,27,28,28,29,30,30,31,32,32,33,34,35,35,36,37,38,39,40,41,42,43,44,45,46,47,48,50,51,53,
													54,56,58,50,62,64,67,69,72,76,80,85,91,98,109,118,127};

static uint8_t prevPedalsPositionInRange[TOTAL_EXPRESSIONS_PEDALS];
static uint8_t pedalsRange[TOTAL_EXPRESSIONS_PEDALS];

uint8_t leadToRange(uint8_t pedalNum, uint8_t pedalPosition);
uint8_t convertAccordingPedalType(uint8_t pedalPosInRange, uint8_t pedalNum);

void expressionProcessSetInitPositionsAndRange()
{
	uint8_t i;
	for (i = 0; i < TOTAL_EXPRESSIONS_PEDALS; ++i)
	{
		prevPedalsPositionInRange[i] = convertAccordingPedalType(leadToRange(i, expGetPedalPosition(i)), i);
		pedalsRange[i] = global.pHighPos[i] - global.pLowPos[i]; 
		runtimeEnvironment.expPedalPos_[i] = prevPedalsPositionInRange[i];
	}
}

uint8_t leadToRange(uint8_t pedalNum, uint8_t pedalPosition)
{
	uint8_t ret;
	uint8_t pedalPositionNorm = pedalPosition >> 1;//need to get pedal in range 0 to 127 
	if(pedalPositionNorm <= global.pLowPos[pedalNum])
		ret = 0;
	else if(pedalPositionNorm >= global.pHighPos[pedalNum])
		ret = 127;
	else
		ret = (((uint16_t)(pedalPositionNorm - global.pLowPos[pedalNum])) << 7 ) / pedalsRange[pedalNum];
	
	return ret;
}

uint8_t convertAccordingPedalType(uint8_t pedalPosInRange, uint8_t pedalNum)
{
	uint8_t ret;
	switch(global.expPtype[pedalNum])
	{
		case LOGARITHMIC:
			ret = pgm_read_byte(&(tableLog10[pedalPosInRange]));
			break;

		case BACK_LOGARITHMIC:
			ret = pgm_read_byte(&(tableAntilog10[pedalPosInRange]));
			break;
			
		default://LINEAL
			ret = pedalPosInRange;
			break;
	}
	return ret;
}


void singlePedalProcess(uint8_t pedalNum)
{
	uint8_t valToSend = convertAccordingPedalType(leadToRange(pedalNum, expGetPedalPosition(pedalNum)), pedalNum);
	uint8_t pedalCc;
	//LOG(SEV_TRACE,"%s %d", __FUNCTION__, pedalNum);
	if(valToSend != prevPedalsPositionInRange[pedalNum])
	{
		//LOG(SEV_TRACE,"%s", __FUNCTION__, pedalNum);
		if(pedalNum == TOTAL_EXPRESSIONS_PEDALS-1)
			pedalCc = (runtimeEnvironment.currentIaState_[FOOT_BUTTONS_NUM-1] == IA_STATE_OFF || bank.pedalsCc[TOTAL_EXPRESSIONS_PEDALS] == 128) ? bank.pedalsCc[pedalNum] : bank.pedalsCc[pedalNum+1];
		else
			pedalCc = bank.pedalsCc[pedalNum]; 
		
		midiSendControlChange(pedalCc, valToSend, global.midiChanNum);
		prevPedalsPositionInRange[pedalNum] = valToSend;
		runtimeEnvironment.expPedalPos_[pedalNum] = valToSend;
		updateRequests.updateScreenRq_ = 1;
		setEventByMask(EVENT_MASK_EXP_CHANGE);
	}
}

void expressionProcess()
{
	uint8_t i;
	
	if(global.bnkSwOnBoard == NO_EXT_PEDAL)
		i = 0;
	else
		i = 1;//pass out expression pedal 1 if BS-2 is using
		
	for( ; i < TOTAL_EXPRESSIONS_PEDALS; ++i)
		singlePedalProcess(i);
}

