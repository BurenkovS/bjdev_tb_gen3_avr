/*
 * TaskManager.c
 *	Manage some specific delayed tasks, i.e. relay off after a certain time
 *
 * Created: 23.08.2017 0:47:11
 *  Author: Sergey
 */ 

#include "TaskManager.h"
#include "RuntimeEnvironment.h"
#include "settings.h"

static uint32_t relaysSwitchOffTime[RELAYS_NUM] = {0,0,0,0};
void setTaskRelayOff(uint8_t relayNum)
{
	relaysSwitchOffTime[relayNum] = getTicks() + RELAY_MOMETRARY_CONTACT_CLOSURE_TIME;
}

static uint32_t messageOffTime = 0;
void setTaskMessageOff()
{
	messageOffTime = getTicks() + POPUP_MESSAGE_TIME;
}

static uint32_t switchBetweenPresetNameAndBankNameTime = 0;
void setTaskSwitchBetweenPresetNameAndBankName()
{
	switchBetweenPresetNameAndBankNameTime = getTicks() + PRESET_BANK_NAME_TOGGLE_TIME;
}

static uint32_t disconnectKpaTime = 0;
void setTaskDisconnectKpa()
{
	disconnectKpaTime = getTicks() + DISCONNECT_KPA_TIME;
}

static uint32_t tapOffTime = 0;
void setTaskTapOff()
{
	tapOffTime = getTicks() + TAP_OFF_TIME;
}


void taskManagerLoop()
{
	uint8_t i;
	uint32_t currentTime = getTicks();
	for (i = 0; i < RELAYS_NUM; ++i)
	{
		if (currentTime >= relaysSwitchOffTime[i] && relaysSwitchOffTime[i] != 0)
		{
			setRelay(i, 0);
			relaysSwitchOffTime[i] = 0;//If time == 0, it mean no switch off requared for this relay
		}
	}
	
	if(currentTime >= messageOffTime && messageOffTime!= 0)
	{
		messageOffTime = 0;
		updateRequests.updateScreenRq_ = 1;
	}
	
	if(currentTime >= switchBetweenPresetNameAndBankNameTime && switchBetweenPresetNameAndBankNameTime!= 0)
	{
		if(global.Show_pr_name == PRESET_AND_BANK)//check is settings not change and need to show in turn the preset and the bank names.
		{
			switchBetweenPresetNameAndBankNameTime = getTicks() + PRESET_BANK_NAME_TOGGLE_TIME;//auto refresh
			runtimeEnvironment.isTimeToShowPresetName_ = !runtimeEnvironment.isTimeToShowPresetName_;//switch between preset name and bank name on main screen
		}
		else//stop auto refresh and show only bank or only preset corresponding actual settings
		{
			runtimeEnvironment.isTimeToShowPresetName_ = (global.Show_pr_name == ONLY_BANK) ? false : true;
			switchBetweenPresetNameAndBankNameTime = 0;
		}
		setEventByMask(EVENT_MASK_MAIN_SCREEN_NAME_CHANGE);
		updateRequests.updateScreenRq_ = 1;
	}
	
	if(currentTime >= disconnectKpaTime && disconnectKpaTime != 0)
	{
		runtimeEnvironment.isKpaConnected_ = false;
		disconnectKpaTime = 0;
	}
	
	if(currentTime >= tapOffTime && tapOffTime != 0)
	{
		runtimeEnvironment.tapState_ = false;
		tapOffTime = 0;
		if(global.tapDisplayType == ON_SCREEN)
		{
			setEventByMask(EVENT_MASK_TAP_CHANGE);
			++updateRequests.updateScreenRq_;
		}
		else
		{
			++updateRequests.updatePedalLedsRq_;
			++updateRequests.updateLedsRq_;
		}
	}
}


