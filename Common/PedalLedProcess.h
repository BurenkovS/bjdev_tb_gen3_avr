/*
 * PedalLedProcess.h
 *
 * Created: 28.11.2017 23:47:41
 *  Author: Sergey
 */ 

#include <stdint.h>
#include "bjdevlib_tb.h"

#ifndef PEDALLEDPROCESS_H_
#define PEDALLEDPROCESS_H_

//define colors for button representation

#define COLOR_LED_OFF       PEDAL_COLOR_B
#define COLOR_LED_PRESET    PEDAL_COLOR_R
#define COLOR_LED_IA        PEDAL_COLOR_G
#define COLOR_LED_SCENE     PEDAL_COLOR_RG

//colors for tuner
#define COLOR_TUNER_BACKGROUND  PEDAL_COLOR_B
#define COLOR_TUNER_TONE        PEDAL_COLOR_R
#define COLOR_TUNER_TONE_FINE   PEDAL_COLOR_G

/*High level functions*/

/*
 * Init pedal shift registers interface
 */
void ledPedalInit();


/*
 * Set three-color led to color
 * Led numbers(top view of pedal):
 *      0       8
 *      1       9
 *      2       10
 *      3       11
 *      4       12
 *      5       13
 *      6       14
 *      7       15 
 * function does NOT perform leds update!!!
 */
void setPedalLedColor(uint8_t ledNum, PedalLedColor color);


/*
 * Set all pedal color
 * function perform leds update if bool send == true
 */
void setAllPedalColor(PedalLedColor color, _Bool send);


/*
 * Set tuner display type. In current version 2 types are supported
 */
//void setPedalTunerType(uint8_t type);

/*
 * Set pedal to any mode
 */
//void setPedalLedMode(uint8_t mode);

/*
 * Draw tuner on the pedal leds
 * Using G-major tune format
 * tune == 16 - lower possible tuner value
 * tune == 31 - higher possible value
 * tune == 23 || tune == 24 - fine tuning
 */
void drawPedalTuner(uint8_t tune);

/*
 * Update pedal leds in button mode
 */
void updatePedalLed(); 
                
/*
 * Pedal action during controller start
 */
void pedalStart();

/*
 * Enable or disable power save mode(only 4 leds is active at the all time) 
 */
//void pedalPowerSaveModeOn();
// void pedalPowerSaveModeOff();

/*
 * Get current pedal led mode
 */
// unsigned char pedalGetLedMode();


#endif /* PEDALLEDPROCESS_H_ */