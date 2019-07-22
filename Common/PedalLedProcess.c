/*
 * PedalLedProcess.c
 *
 * Created: 28.11.2017 23:47:28
 *  Author: Sergey
 */ 
#include "PedalLedProcess.h"
#include "bjdevlib_tb.h"
#include "settings.h"
#include "RuntimeEnvironment.h"
#include "MidiHelpers.h"
#include <string.h>
#include <util/delay.h>

//
//static unsigned char pedalLedMode = PEDAL_LED_MODE_BUTTON;
//static unsigned char tunerType = 0;

/*
 * prepare led interface
 */
/*static unsigned char ledBuffer[PEDAL_REGS_NUM];
static unsigned char ledBufferPrev[PEDAL_REGS_NUM];//this previous state of registers. Using to prevent blinking  during redraw


void ledPadalSendData()
{
    unsigned char i;
    for (i = 0; i < PEDAL_REGS_NUM; ++i)
    {
        if (ledBufferPrev[i] != ledBuffer[i])//check data in difference to prevent blinks
        {
            ledPedalSend();
            memcpy(ledBufferPrev, ledBuffer, PEDAL_REGS_NUM);
            break;
        }
    }
}*/

/*void setPedalLedMode(uint8_t mode)
{
    pedalLedMode = mode;
}*/


void updatePedalLed()
{
#if defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)
    uint8_t i = FOOT_BUTTONS_NUM - 1; 
    
    if (global.pedalLedView == PEDAL_LED_MODE_TUNER)
    {
        return;
    }
        
    if(global.pedalLedView == PEDAL_LED_MODE_DUAL && runtimeEnvironment.isTimeToShowTuner_ == true)//if dual mode(button / tuner) enabled, but tuner is active at the moment
    {
        return;
    }
    
    if(global.tapDisplayType == ON_ACTIVE_PRESET + FOOT_BUTTONS_NUM)//If TAP display active on "virtual" led under pedal button
    {
		if(runtimeEnvironment.tapState_ == 0)
            ledSetPedalColorAll(COLOR_LED_OFF, true); 
        else 
            ledSetPedalColorAll(COLOR_LED_IA, true);
    }
    else
    {
        //IA State (CC_tg) and NRPN(the same as for CC_tg)
        if((bank.buttonType[i] == CC_TOGGLE  || bank.buttonType[i] == NRPN_TOGGLE) && runtimeEnvironment.currentIaState_[i] >= IA_STATE_ON)//If IA state ON and not shift mode 
        {
            ledSetPedalColorAll(COLOR_LED_IA, true);
        }
        
        else if (bank.buttonType[i] == CC_CONST_VAL && runtimeEnvironment.lastCcConstButton_ == i)
        {
            ledSetPedalColorAll(COLOR_LED_SCENE, true);
        }

        else if(bank.buttonType[i] == PRESET_CHANGE && runtimeEnvironment.activePresetButtonNumber_ == i)//if button is current preset
        {
            if(global.tapDisplayType == ON_ACTIVE_PRESET)//blink with LED_ACTIVE_PRESET color
            {
				if(runtimeEnvironment.tapState_ == false)
					ledSetPedalColorAll(COLOR_LED_OFF, true);
			}
			else
				ledSetPedalColorAll(COLOR_LED_PRESET, true);

        }
        else 
        {
            ledSetPedalColorAll(COLOR_LED_OFF, true); 
        }
    }
#endif
}

//Pasha style tuner
void tunerType1(uint8_t tune)
{
    unsigned char i;
    unsigned char ledPosition = 0;
    unsigned char fractional = 0;//if need to set two leds

    for (i = 0; i < PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM; ++i)//fill all green(like clear screen)
            ledSetPedalColor(i, COLOR_TUNER_BACKGROUND, false);
            
    if(tune == 23 || tune == 24)//draw center value
    {
        //set center to blue
        ledSetPedalColor(3, COLOR_TUNER_TONE_FINE, false);//from left side of pedal
        ledSetPedalColor(4, COLOR_TUNER_TONE_FINE, false);
        ledSetPedalColor(3 + PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM_IN_LINE, COLOR_TUNER_TONE_FINE, false);//from right side of pedal
        ledSetPedalColor(4 + PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM_IN_LINE, COLOR_TUNER_TONE_FINE, false);
        return;
    }
    else if(tune < 23)
    {
        ledPosition = (tune - 16)/2;
        fractional = (tune - 16)%2;
        //invert enumeration
        ledPosition = PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM_IN_LINE - 1 - ledPosition;
    }
    
    else if(tune > 24)//as we add two value as center position, we should add 1 to led position
    {
        ledPosition = (tune - 17)/2;
        fractional = (tune - 17)%2;
        //invert enumeration
        ledPosition = PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM_IN_LINE - 1 - ledPosition;
    }
    
    ledSetPedalColor(ledPosition, COLOR_TUNER_TONE, false);
    ledSetPedalColor(ledPosition + PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM_IN_LINE, COLOR_TUNER_TONE, false);
    
    if(fractional)
    {
        ledSetPedalColor(ledPosition - 1, COLOR_TUNER_TONE, false);
        ledSetPedalColor(ledPosition - 1 + PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM_IN_LINE, COLOR_TUNER_TONE, false);
    } 
}


/*
 * My tuner style.
 * If tune is low - left side of leds will go up with red color on green background
 * If tune is high - right side of leds will go up with red color on green background
 * If tune is fine - two low leds on both sides was blue
 */
void tunerType2(uint8_t tune)
{
    unsigned char i;
    
    for (i = 0; i < PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM; ++i)//fill all green(like clear screen)
            ledSetPedalColor(i, COLOR_TUNER_BACKGROUND, false);
            
    if(tune == 23 || tune == 24)//draw center value
    {
        ledSetPedalColor(7, COLOR_TUNER_TONE_FINE, false);
        ledSetPedalColor(7 + PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM_IN_LINE, COLOR_TUNER_TONE_FINE, false);//from right side of pedal
    }
    else
    {
        //setPedalLedColor(7 + PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM_IN_LINE, COLOR_TUNER_TONE);//from left side of pedal(leds 0-7)
        if(tune < 23)
        {
            ledSetPedalColor(7, COLOR_TUNER_TONE_FINE, false);
            for (i = 0; i < PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM_IN_LINE-1; ++i)
            {
                if(i >= tune - 16)
                    ledSetPedalColor(i, COLOR_TUNER_TONE, false);
            }
        }
        else
        {
            ledSetPedalColor(7 + PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM_IN_LINE, COLOR_TUNER_TONE_FINE, false);
            for(i = 0; i < PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM_IN_LINE ; ++i)//from right side of pedal(leds 8-15)
            {
                if(i <= (tune - 25))
                    ledSetPedalColor(PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM - 2 - i, COLOR_TUNER_TONE, false);
            }
        }
    }
}

/*
 * G-major tune format
 * tune == 16 - lower possible tuner value
 * tune == 31 - higher possible value
 * tune == 23 || tune == 24 - fine tuning
 */
void drawPedalTuner(uint8_t tune)
{
#if defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)
    if(global.pedalLedView == PEDAL_LED_MODE_TUNER || (global.pedalLedView == PEDAL_LED_MODE_DUAL && runtimeEnvironment.isTimeToShowTuner_ == true))
    {
        if(tune < 16 || tune > 31)//discard invalid values
        {
            ledSetPedalColorAll(COLOR_TUNER_BACKGROUND, true);
            return;
        }

        switch(global.pedalTunerScheme)
        {
            case Scheme1 : //pasha tuner
                tunerType1(tune);
                break;
            case Scheme2 : //my
                tunerType2(tune);
                break;
            default : break;
        }
    }    
    ledPedalSend();  
#endif  
}


/*void setPedalTunerType(uint8_t type)
{
    tunerType = type;
}*/


void pedalStart()
{
    unsigned char i = 0;
    unsigned char ledNum = 0;
    
    ledSetPedalColorAll(PEDAL_COLOR_NO, true);
    _delay_ms(200);
    for (i = 0; i < PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM_IN_LINE; ++i)
    {
        ledNum = PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM_IN_LINE - 1 - i;
        ledSetPedalColor(ledNum, PEDAL_COLOR_RGB, false);
        ledSetPedalColor(ledNum+PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM_IN_LINE, PEDAL_COLOR_RGB, true);
        _delay_ms(120);
    }

}

//static unsigned char pedalLedModeLastState = 0;
/*void pedalPowerSaveModeOn()
{
    unsigned char i;
    pedalLedModeLastState = pedalLedMode;
    pedalLedMode = PEDAL_LED_MODE_POWER_SAVE;
    
    for (i = 0; i < PEDAL_TRICOLOR_VIRTUAL_LEDS_NUM; ++i)
            setPedalLedColor(i, PEDAL_COLOR_NO);//COLOR_TUNER_BACKGROUND
            
    setPedalLedColor(0, COLOR_TUNER_BACKGROUND);
    setPedalLedColor(7, COLOR_TUNER_BACKGROUND);
    setPedalLedColor(8, COLOR_TUNER_BACKGROUND);
    setPedalLedColor(15, COLOR_TUNER_BACKGROUND);
    ledPadalSendData();

}

void pedalPowerSaveModeOff()
{
    pedalLedMode = pedalLedModeLastState;
}

unsigned char pedalGetLedMode()
{
    return pedalLedMode;
}*/
