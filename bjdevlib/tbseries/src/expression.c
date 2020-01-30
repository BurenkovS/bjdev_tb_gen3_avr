/*
 * BJ Devices Travel Box series midi controller library
 * @file	expression.c
 * 
 * @brief	Expression pedal functions				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#include "expression.h"
#include "adc.h"
#include "pinout.h"
#include "portio.h"
#include "settings.h"
#include <avr/pgmspace.h>

const ioPort padalsArray[MAX_PHY_PEDALS] PROGMEM = {
	{ &EXP_P1_PORT, EXP_P1_PIN },
	{ &EXP_P2_PORT, EXP_P2_PIN },
	{ &EXP_P_OB_PORT, EXP_P_OB_PIN }};


static const uint8_t adcChanArray[MAX_PHY_PEDALS] = {EXP_P1_ADC_CHAN, EXP_P2_ADC_CHAN, EXP_P_ONBPAR_ADC_CHAN};
static uint8_t pedalsPrevValue[MAX_PHY_PEDALS] = {0, 0, 0};
	
void initExpression()
{
	uint8_t i;
	ioPort tmpPort;
	
	initAdc();
	
	for(i = 0; i < MAX_PHY_PEDALS; ++i)
	{	
		tmpPort.pin_ = pgm_read_byte(&(padalsArray[i].pin_));
		tmpPort.portReg_ = (volatile uint8_t*)pgm_read_word(&(padalsArray[i].portReg_));
		initInput(&tmpPort, 1);
		//initFilter(adcRead8MsbBit(adcChanArray[i]), i);
		//pedalsPrevValue[i] = (adcRead8MsbBit(adcChanArray[i]) >> 1);
	}
	
}

uint8_t expGetPedalPosition(uint8_t pedalNumber)
{
	return (hysteresis(adcRead8MsbBit(adcChanArray[pedalNumber]), pedalNumber));
}

uint8_t expGetPedalPositionaRaw(uint8_t pedalNumber)
{
	return adcRead8MsbBit(adcChanArray[pedalNumber]);
}

//callback
static void (*posCallback)(PedalNumber, uint8_t);

void expRegisterPedalChangePositionCallback(void (*callback)(PedalNumber pedalNumber, uint8_t position))
{
	posCallback = callback;
}

void expProcess()
{
	if(!posCallback)
		return;

	uint8_t i;
	uint8_t position;
		
	for(i = 0 ; i < MAX_PHY_PEDALS; ++i)
	{
		position = expGetPedalPosition(i);
		if(position != pedalsPrevValue[i])
		{
			pedalsPrevValue[i] = position;
			(*posCallback)((PedalNumber)i, position);
		}
	}
}
