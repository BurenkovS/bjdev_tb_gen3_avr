/*
 * BJ Devices Travel Box series midi controller library
 * @file	bjdevlib_tb.c
 * 
 * @brief	Common include for library
			  
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#include "bjdevlib_tb.h"
#include <avr/interrupt.h>

void initBjDevLib()
{

#ifdef LOG_ENABLED
	initUart1(19200);
	LOG(SEV_INFO, "Log enabled");
#else
	initUart1(31250);
#endif
	initButtons();
	initTimer();
	initMidi();
	initLed();
	initExpression();
	initRelay();
#if defined (TB_6P_DEVICE) || defined(TB_11P_DEVICE)
	initPedalLed();
#endif
	sei();
} 