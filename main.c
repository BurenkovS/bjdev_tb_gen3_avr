#include "bjdevlib_tb.h"
#include "MenuItems.h"
#include "lcd_tb.h"
#include "button.h"
#include "settings.h"
#include "CallbackFunctions.h"
#include "cat25512vi/25LC256.h"
#include "tlc59731/tlc59731.h"
#include "FootswitchesProcess.h"
#include "TaskManager.h"
#include "CommonMidiEventsProcess.h"
#include "AxeFxMidiEventsProcess.h"
#include "AxeFx3MidiEventsProcess.h"
#include "KpaMidiEventsProcess.h"
#include "GmajorMidiEventsProcess.h"
#include "LedProcess.h"
#include "PedalLedProcess.h"
#include "RuntimeEnvironment.h"
#include "ExpressionProcess.h"
#include "Maintenance.h"
#include "CallbackFunctions.h"
#include "FirmwareVersion.h"
#include "PwmControllableFeatures.h"
#include "Selftest.h"
#include "dump.h"

#include <string.h>
#include <avr/io.h>

#define  MAX_MIDI_MESSAGE_LENGTH 128
static uint8_t midiMessageFromMidi[MAX_MIDI_MESSAGE_LENGTH];
static uint8_t midiMessageFromUsb[MAX_MIDI_MESSAGE_LENGTH];
uint8_t inputMessType; 
uint16_t inputMessLength;
uint8_t inputMessChanNum;

int main(void)
{
	initBjDevLib();
	
	initSPI();

	LCDInit(LS_ULINE);
	
	LcdHideCursor();
	
	ButtonEvent lastButtonEvent;
	lastButtonEvent = getButtonLastEvent();
	if(lastButtonEvent.actionType_ == BUTTON_PUSH && lastButtonEvent.buttonNum_ == KEY_LOAD)//enter to self test mode
		startSelfTest();

	ButtonEvent lastAdcButtonEvent;//for bs-2 pedal
	lastAdcButtonEvent = getAdcButtonLastEvent();
		
	startupSettingsCheckAndLoad();
	LoadBank(global.bnkNum, (BankSettings*)&bank);
	initUart1(global.usbBaudrate);

	//Show model and firmware version
	LCDWriteStringXY(0,0, HARDWARE_MODEL_STRING);
	LCDWriteStringXY(0,1, "Version:");
	LCDWriteStringXY(8,1, FIRMWARE_VERSION_STRING);
	_delay_ms(1000);
	
	Menu_SetGenericWriteCallback(DisplayUpdateHeader);
	
	//set brightness and contrast settings
	setPwmValueWithScale(PEDAL_BRIGHTNESS_PWM_NUM, global.pedalBrightness);
	setPwmValueWithScale(SCREEN_BRIGHTNESS_PWM_NUM, global.screenBrightness);
	setPwmValueWithScale(SCREEN_CONTRAST_PWM_NUM, global.screenContrast);

	runtimeEnvironment.activePresetButtonNumber_ = 0;
	runtimeEnvironment.activePresetNumber_ = bank.buttonContext[0].presetChangeContext.programsNumbers[0];
	//Set default preset name to show. This string will replace by first valid sysex message with name
	strcpy((char*)&(runtimeEnvironment.vendorPresetName_), ("No preset name"));
	
	//update LEDs after start sequence 
	updateRequests.updateLedsRq_ = 1;
	updateRequests.updatePedalLedsRq_ = 1;
	
	Menu_Navigate(&Main_screen);
	
	expressionProcessSetInitPositionsAndRange();
	expressionProcess();
	
#if defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)
	pedalStart();
#endif
	
	ledCheck(1);

    while (1) 
    {
		if(midiRead(MIDI_INTERFACE, midiMessageFromMidi, &inputMessType, &inputMessLength))
		{
			inputMessChanNum = midiGetChannelNumber(midiMessageFromMidi);
			handleMidiEventCommonMidiDevices(inputMessType,inputMessChanNum, midiMessageFromMidi);
			handleMidiEventAxeFx(inputMessType,midiMessageFromMidi, inputMessLength);
			handleMidiEventAxeFx3(inputMessType,midiMessageFromMidi, inputMessLength);
			handleMidiEventKpa(inputMessType, midiMessageFromMidi); 
			handleMidiEventGMajor(inputMessType,midiMessageFromMidi, inputMessLength);
			handleMidiSysExSettings( inputMessType, midiMessageFromMidi, inputMessLength, MAX_MIDI_MESSAGE_LENGTH );
		}
					
		lastButtonEvent = getButtonLastEvent();
		
		//turn off tuner on any button push
		if (lastButtonEvent.actionType_ == BUTTON_PUSH)
		{
			if(runtimeEnvironment.isTimeToShowTuner_)
			{
				tunerOnOffProcess();
			}
		}
		
		if(lastButtonEvent.actionType_ == BUTTON_PUSH
			|| (lastButtonEvent.actionType_ == BUTTON_REPAEATED_PUSH && lastButtonEvent.buttonNum_ == KEY_UP)//apply auto push on UP and DOWN buttons
			||(lastButtonEvent.actionType_ == BUTTON_REPAEATED_PUSH && lastButtonEvent.buttonNum_ == KEY_DOWN))
		{
			Menu_NavigationProcessButtonEvent(&lastButtonEvent);
		}
		
		if(runtimeEnvironment.showTapRq_)
		{
			runtimeEnvironment.tapState_ = true;
			if(global.tapDisplayType == ON_SCREEN)
			{
				setEventByMask(EVENT_MASK_TAP_CHANGE);
				++updateRequests.updateScreenRq_;
			}
			
#if defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)

			else if(global.tapDisplayType == ON_ACTIVE_PRESET + FOOT_BUTTONS_NUM)//If led is a pedal led stripes
			{
				++updateRequests.updatePedalLedsRq_;	
			}
			
			else if(global.tapDisplayType == ON_ACTIVE_PRESET && runtimeEnvironment.activePresetButtonNumber_ == (FOOT_BUTTONS_NUM-1))//active preset button is under pedal
			{
				++updateRequests.updatePedalLedsRq_;	
			}
#endif	//defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)
		
			else 
			{
				++updateRequests.updateLedsRq_;
			}
			setTaskTapOff();
			runtimeEnvironment.showTapRq_ = false;
		}
		
		footswitchesProcess(&lastButtonEvent);
		
		lastAdcButtonEvent = getAdcButtonLastEvent();
		extBs2PedalProcess(&lastAdcButtonEvent);
		
		expressionProcess();
		taskManagerLoop();
		
		//process requests
		if(updateRequests.updateScreenRq_)
		{
			Menu_Redraw();
			clearEvents();
			updateRequests.updateScreenRq_ = 0;
		}


		if(updateRequests.updateLedsRq_)
		{
			updateLed();
			updateRequests.updateLedsRq_ = 0;
		}
		//some other vendor specific processes
		axeFxCommonProcess(); 
		
		
#if defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)
		if(updateRequests.updatePedalLedsRq_)
		{
			updatePedalLed();
			updateRequests.updatePedalLedsRq_ = 0;
		}
#endif
    }
}

