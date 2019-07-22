/*
 * PwmControllableFeatures.c
 *
 * Created: 27.11.2018 13:10:29
 *  Author: User
 */ 

#include "PwmControllableFeatures.h"
#include "tlc59731.h"
#include "log.h"
#include <avr/pgmspace.h>

uint8_t pwmCurrentValue[3] = {0, 0, 0};
static const uint8_t pwmMaxTable[3] PROGMEM = {SCREEN_BRIGHTNESS_MAX, SCREEN_CONTRAST_MAX, PEDAL_BRIGHTNESS_MAX};
static const uint8_t pwmScaleTable[3] PROGMEM = {SCREEN_BRIGHTNESS_SCALE, SCREEN_CONTRAST_SCALE, PEDAL_BRIGHTNESS_SCALE};
	
void setPwmValue(uint8_t pwmNum,uint8_t pwmValue)
{
	pwmCurrentValue[pwmNum] = pwmValue;
	tlc59731SendPwmValues(pwmCurrentValue[0], pwmCurrentValue[1], pwmCurrentValue[2]);	
}

void setPwmValueWithScale(uint8_t pwmNum,uint8_t pwmValue)
{
	LOG(SEV_TRACE, "%s", __FUNCTION__);
	LOG(SEV_TRACE, "pwmNum %d pwmValue %d", pwmNum, pwmValue);
	if(pwmValue >= pgm_read_byte(&(pwmMaxTable[pwmNum])))
		setPwmValue(pwmNum, 255);
	else
		setPwmValue(pwmNum, pwmValue*pgm_read_byte(&(pwmScaleTable[pwmNum])));
}

