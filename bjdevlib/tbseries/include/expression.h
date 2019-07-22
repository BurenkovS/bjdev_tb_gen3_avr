/*
 * BJ Devices Travel Box series midi controller library
 * @file	expression.h
 * 
 * @brief	Expression pedal functions				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#ifndef expression_h_
#define expression_h_

#include "adc.h"
#include "pinout.h"
#include <stdint.h>

typedef enum PedalNumber
{
	EXP_PEDAL1 = 0
	,EXP_PEDAL2
	,EXP_PEDAL_ONBOARD	
}PedalNumber;

#define EXP_P1_ADC_CHAN EXP_P1_PIN
#define EXP_P2_ADC_CHAN EXP_P2_PIN
#define EXP_P_ONBPAR_ADC_CHAN EXP_P_OB_PIN

/*
 * @brief	Initialization of expression pedals driver 
 */
void initExpression();

/*
 * @brief	Get expression pedal position with hysteresis. Max position is 255, min - 0
 */
uint8_t expGetPedalPosition(PedalNumber pedalNumber);

/*
 * @brief	Get expression pedal position. No hysteresis. Max position is 255, min - 0
 */
uint8_t expGetPedalPositionaRaw(uint8_t pedalNumber);

/*
 * @brief	Register callback will invoked if any pedal change position
 */
void expRegisterPedalChangePositionCallback(void (*callback)(PedalNumber pedalNumber, uint8_t position));

/*
 * @brief	Register callback will invoked if any pedal change position
 */
void expRegisterPedalChangePositionCallback(void (*callback)(PedalNumber pedalNumber, uint8_t position));


/*
 * @brief	REad all pedals positions, and invoke registered callback if any pedals was turn. Should be invoked in main loop
 */
void expProcess();

#endif /* expression_h_ */