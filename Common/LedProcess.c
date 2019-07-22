/*
 * LedProcess.c
 *
 * Created: 09.10.2017 17:09:31
 *  Author: User
 */ 


#include "LedProcess.h"
#include "bjdevlib_tb.h"
#include "settings.h"
#include "RuntimeEnvironment.h"
#include "MidiHelpers.h"

#include <util/delay.h>

/*High level functions*/
void updateLed()
{
	uint8_t i;

	for (i = 0; i < FOOT_BUTTONS_NUM; i++)//
	{
		if(global.tapDisplayType == (i + 1 + ON_ACTIVE_PRESET))//If TAP display active on this led, other led functions will overwrite
		{
			if(runtimeEnvironment.tapState_ == false)
				ledSetColor(i, COLOR_BLACK, false);
			else
				ledSetColor(i, LED_COLOR_ROLE_TAP, false);//Зажигем светодиод
		}
		else
		{
			if((bank.buttonType[i] == CC_TOGGLE || bank.buttonType[i] == NRPN_TOGGLE) && runtimeEnvironment.currentIaState_[i] >= IA_STATE_ON)//If IA state ON and not shift mode
			{
				ledSetColor(i, LED_COLOR_ROLE_IA_STATE, false);//Зажигем светодиод
			}
			
			else if (bank.buttonType[i] == CC_CONST_VAL && runtimeEnvironment.lastCcConstButton_ == i)
			{
				ledSetColor(i, LED_COLOR_ROLE_ACTIVE_SCENE, false);
			}
			
			else if (bank.buttonType[i] == PRESET_CHANGE && runtimeEnvironment.activePresetButtonNumber_ == i)
			{
				if(global.tapDisplayType == ON_ACTIVE_PRESET)//blink with LED_ACTIVE_PRESET color 
				{
					if(runtimeEnvironment.tapState_ == false)
						ledSetColor(i, COLOR_BLACK, false);
					else
						ledSetColor(i, LED_COLOR_ROLE_ACTIVE_PRESET, false);
				}
				else//just show active preset led
					ledSetColor(i, LED_COLOR_ROLE_ACTIVE_PRESET, false);
			}
						
			else
			{
				ledSetColor(i, COLOR_BLACK, false);
			}
		}
	}
	ledSend();
}

void ledCheck(uint8_t num) //Включает и выключает все светодиоды num раз. сделано для проверки при включении
{
	uint8_t i = 0;
	for (i = 0; i < num; i++)
	{
		ledSetColorAll(COLOR_BLACK, true);
		_delay_ms(200);
		ledSetColorAll(LED_COLOR_ROLE_IA_STATE, true);
		_delay_ms(200);
		ledSetColorAll(COLOR_BLACK, true);
		_delay_ms(200);		
		ledSetColorAll(LED_COLOR_ROLE_ACTIVE_PRESET, true);
		_delay_ms(200);
		ledSetColorAll(COLOR_BLACK, true);
	}
}

