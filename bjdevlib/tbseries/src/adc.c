/*
 * BJ Devices Travel Box series midi controller library
 * @file	adc.c
 * 
 * @brief	ADC low level functions				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#include "adc.h"
#include <util/delay.h>
#include <avr/io.h>

#include "log.h"



void initAdc()
{
	ADMUX=ADC_VREF_TYPE & 0xff;
	ADCSRA=0x87;	
}

uint8_t adcRead8MsbBit(uint8_t chanNum)
{
	ADMUX=chanNum | (ADC_VREF_TYPE & 0xff);
	// Delay needed for the stabilization of the ADC input voltage
	_delay_us(10);
	// Start the AD conversion
	ADCSRA|=0x40;
	// Wait for the AD conversion to complete
	while ((ADCSRA & 0x10)==0);
	ADCSRA|=0x10;
	return ADCH;
}

static uint8_t direction[3] = {0,0,0};
static const uint8_t threshold = 1;
static uint8_t lastVal[3] = {0,0,0};
uint8_t hysteresis(uint8_t input, uint8_t channel)
{
	//LOG(SEV_TRACE, "%s input %d channel %d", __FUNCTION__, input, channel);
	if (direction[channel] == 0)        //go down
	{
		if (input > lastVal[channel] + threshold)
		{
			direction[channel] = 1;    //go up
			lastVal[channel] = input;
			return input;
		}
		else if(input < lastVal[channel])
		{
			lastVal[channel] = input;
			return input;
		}
		else
		{
			return lastVal[channel];
		}
	}
	else                //go up
	{
		if (input < lastVal[channel] - threshold)
		{
			direction[channel] = 0;    //go down
			lastVal[channel] = input;
			return input;
		}
		else if(input >= lastVal[channel])
		{
			lastVal[channel] = input;
			return input;
		}
		else
		{
			return lastVal[channel];
		}
	}
}
