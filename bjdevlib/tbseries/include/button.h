/*
 * BJ Devices Travel Box series midi controller library
 * @file	button.h
 * 
 * @brief	Process footswitch buttons and configuration keyboard buttons 
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2015
 */ 


#ifndef button_h_
#define button_h_

#include <stdint.h>

#define KEY_INC		(FOOT_BUTTONS_NUM+0)
#define KEY_DEC		(FOOT_BUTTONS_NUM+1)
#define KEY_UP		(FOOT_BUTTONS_NUM+2)
#define	KEY_DOWN	(FOOT_BUTTONS_NUM+3)
#define KEY_LOAD	(FOOT_BUTTONS_NUM+4)
#define KEY_SETUP	(FOOT_BUTTONS_NUM+5)

typedef enum ButtonActionType//Action type
{
	BUTTON_NO_EVENT = 0,
	BUTTON_PUSH,			//push button
	BUTTON_RELEASE,			//release button
	BUTTON_HOLDON,			//button holds during some time
	BUTTON_REPAEATED_PUSH,	//repeated button push event
	BUTTON_RELEASE_AFTER_HOLD,	//release after hold on
}ButtonActionType;


//keyboard via single ACD input
#define ADC_BUTTONS_NUM 2//total buttons, connected to ADC input

#define ADC_BUT_HIGH 0xD0//Верхняя граница, если с ацп получили значение выше этого, считаем что кнопка нажата
#define ADC_BUT_LOW  0x2f//Нижняя граница, если с ацп получили значение ниже этого, считаем что кнопка нажата



typedef struct
{
	ButtonActionType actionType_;
	uint8_t buttonNum_;				//Button numbers starts from 0 value. Button 0 is marked as "1" on device panel  
}ButtonEvent;

typedef struct
{
	uint8_t adcNum_;
	uint8_t minVal_;
	uint8_t maxVal_;
} AdcButtonsDecs;

/*
 * @brief	Buttons initialization
 */
void initButtons();


/*
 * @brief	Return last button event - button number and action type
 */
ButtonEvent getButtonLastEvent();

/*
 * @brief	Return lastADC  button event - button number and action type
 */
ButtonEvent getAdcButtonLastEvent();

#endif /* button_h_ */