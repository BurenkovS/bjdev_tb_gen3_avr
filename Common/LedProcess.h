/*
 * LedProcess.h
 *
 * Created: 09.10.2017 17:09:43
 *  Author: User
 */ 


#ifndef LEDPROCESS_H_
#define LEDPROCESS_H_

#include "bjdevlib_tb.h"
#include <stdint.h>

#define LED_COLOR_ROLE_IA_STATE COLOR_GREEN
#define LED_COLOR_ROLE_ACTIVE_PRESET COLOR_RED
#define LED_COLOR_ROLE_ACTIVE_SCENE COLOR_YELLOW
#define LED_COLOR_ROLE_TAP COLOR_RED

/*
 * Update leds
 */
void updateLed();

void ledCheck(uint8_t num); //blink all onboard LEDs num times

#endif /* LEDPROCESS_H_ */