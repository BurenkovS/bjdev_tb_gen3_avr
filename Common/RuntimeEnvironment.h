/*
 * RuntimeEnvironment.h
 * Runtime environment of business logic: statuses, flags, etc. 
 * Created: 11.08.2017 0:32:43
 *  Author: Sergey
 */ 


#ifndef RUNTIMEENVIRONMENT_H_
#define RUNTIMEENVIRONMENT_H_

#include <stdint.h>
#include <stdbool.h>
//#include "bjdevlib/tbseries/vendors/fractal_audio/axefx.h"
#include "axefx.h"
#include "bjdevlib_tb.h"

#define  EVENT_MASK_TUNER_CHANGE 0x01 //tuner massage received, need to redraw it on screen or on pedal
#define  EVENT_MASK_MAIN_SCREEN_NAME_CHANGE 0x02//Change preset name of bank name, or switching between show preset/bank name on main screen
#define  EVENT_MASK_EXP_CHANGE 0x04//Change preset name of bank name, or switching between show preset/bank name on main screen
#define  EVENT_MASK_TAP_CHANGE 0x08//TAP state change

typedef struct RuntimeEnvironment 
{
	uint8_t activePresetButtonNumber_;
	uint8_t activePresetNumber_;//Last sent Program Change message from 1st of total 4 message on PC button push
	uint8_t activeBankNumber_;//Last sent Bank select message from 1st of total 4 message on PC button push
	bool isAxeFxConnected_;
	bool isAxeFx3Connected_;
	AxeFxModelId currentAxeFxModel_;
	bool isKpaConnected_;
	bool isTimeToShowPresetName_;
	bool isTimeToShowTuner_;
	bool showTapRq_;
	bool tapState_;
	char vendorPresetName_[16];
	uint8_t currentIaState_[FOOT_BUTTONS_NUM];//State for CC_toggle of NRPN toggle button
	uint8_t lastCcConstButton_;//CC Const works like scene switcher in AxeFx. 
	uint8_t currentTapState_;//In using TG tap we should remember prev state
	uint8_t tunerNote_;
	uint8_t tunerTune_;
	uint8_t expPedalPos_[3];
}RuntimeEnvironment;

typedef struct UpdateRequests
{
	//LED, Pedal LED and screen update requests
	uint8_t updateLedsRq_;
	uint8_t updatePedalLedsRq_;
	uint8_t updateScreenRq_;
	uint8_t	eventFlags_;//Shows which exactly event occur. See define EVENT_MASK_X 
}UpdateRequests;

RuntimeEnvironment runtimeEnvironment;
UpdateRequests updateRequests;


void setEventByMask(uint8_t mask);
void clearEvents();
bool checkEventByMask(uint8_t mask);

#endif /* RUNTIMEENVIRONMENT_H_ */