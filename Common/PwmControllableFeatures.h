/*
 * PwmControllableFeatures.h
 *
 * Created: 27.11.2018 13:10:38
 *  Author: User
 */ 


#ifndef PWMCONTROLLABLEFEATURES_H_
#define PWMCONTROLLABLEFEATURES_H_

#include <stdint.h>
#define SCREEN_BRIGHTNESS_PWM_NUM 0
#define SCREEN_CONTRAST_PWM_NUM 1
#define PEDAL_BRIGHTNESS_PWM_NUM 2

#define SCREEN_BRIGHTNESS_MAX 9
#define SCREEN_CONTRAST_MAX 255
#define PEDAL_BRIGHTNESS_MAX 9

#define SCREEN_BRIGHTNESS_SCALE (255/SCREEN_BRIGHTNESS_MAX)
#define SCREEN_CONTRAST_SCALE (255/SCREEN_CONTRAST_MAX)
#define PEDAL_BRIGHTNESS_SCALE (255/PEDAL_BRIGHTNESS_MAX)

/*
 * Set single pwm to particular value considering current pwm scaling.
 */
void setPwmValueWithScale(uint8_t pwmNum,uint8_t pwmValue);


#endif /* PWMCONTROLLABLEFEATURES_H_ */