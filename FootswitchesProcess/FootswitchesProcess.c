/*
 * FootswitchesProcess.c
 * Processing of foot button pressing
 * Created: 08.08.2017 0:03:37
 *  Author: Sergey
 */ 

#include "FootswitchesProcess.h"
#include "settings.h"
#include "bjdevlib_tb.h"
#include "RuntimeEnvironment.h"
#include "MidiHelpers.h"
#include "TaskManager.h"
#include "Maintenance.h"
#include "AxeFxMidiEventsProcess.h"

//Same procedure for all button types, there need to change preset 
void selectNewPresetCommonPart(uint8_t buttonNum)
{
	//Send bank select message before preset number
	uint8_t midiChannelToSend;
	midiChannelToSend = bank.buttonContext[buttonNum].presetChangeContext.midiChannelNumbers[0];
	if(midiChannelToSend == MIDI_CHANNEL_PRIMARY)
		midiChannelToSend = global.midiChanNum;
			
	//Send bank select message before preset number
	if(midiChannelToSend != MIDI_CHANNEL_NOT_ACTIVE)
	{
		sendPcWithOptionalBs(bank.buttonContext[buttonNum].presetChangeContext.programsNumbers[0]
						,bank.buttonContext[buttonNum].presetChangeContext.banksNumbers[0]
						//,bank.buttonContext[buttonNum].presetChangeContext.midiChannelNumbers[0]
						,midiChannelToSend
						,global.bankSelectMessType
						,global.useBankSelectMess);
	}
	
	if (runtimeEnvironment.isAxeFxConnected_ == true)//Если акс подключен то отправим запрос на состояние эффектов
	{
		requestAxefxInfo(IA_STATE_FUNCTION_ID);
		runtimeEnvironment.isTimeToShowPresetName_ = 0;//и будем показывать имя банка, пока не придет имя пресета
	}
}

static uint8_t tapMomentaryLock = 0;
void sendTapTempo(ButtonEvent *buttonEvent)
{
	if(buttonEvent->actionType_ != BUTTON_PUSH && buttonEvent->actionType_ != BUTTON_RELEASE && buttonEvent->actionType_ != BUTTON_RELEASE_AFTER_HOLD)
		return;
	
	if(bank.tapCc == CC_OFF_VALUE)
		return;
		 
	switch(global.tapType)
	{
		case TAP_CONST_VALUE : 
			if(buttonEvent->actionType_ == BUTTON_PUSH)
			{
				runtimeEnvironment.currentTapState_ = IA_STATE_OFF;
				midiSendControlChange(bank.tapCc, runtimeEnvironment.currentTapState_, global.midiChanNum);
			}
			break;
			
		case TAP_MOMETRARY :
			if(buttonEvent->actionType_ == BUTTON_PUSH)//push
			{
				runtimeEnvironment.currentTapState_ = IA_STATE_ON;
				tapMomentaryLock = 1;
			}
			else//release
			{
				runtimeEnvironment.currentTapState_ = IA_STATE_OFF;
			}
			if(tapMomentaryLock)
				midiSendControlChange(bank.tapCc, runtimeEnvironment.currentTapState_, global.midiChanNum);
				
			break;
			
		case TAP_TOGGLE :
			if(buttonEvent->actionType_ == BUTTON_PUSH)
			{
				if(runtimeEnvironment.currentTapState_ == IA_STATE_OFF)
					runtimeEnvironment.currentTapState_ = IA_STATE_ON;
				else
					runtimeEnvironment.currentTapState_ = IA_STATE_OFF;	
					
				midiSendControlChange(bank.tapCc, runtimeEnvironment.currentTapState_, global.midiChanNum); 		
			}
			break;
	}

}

void relayActionCommon(uint8_t relayRequared, bool invert)
{
	uint8_t relayNum;
	uint8_t i;
	for (i = 0; i < RELAYS_NUM; i++)
	{
		relayNum = RELAYS_NUM-1-i;//TODO reverse relay positions in byte and remove this shit
		
		switch(((relayRequared >> (i<<1)) & 0x3))//get 2 bit relay setting
		{
			case RELAY_MASK_CLOSE :
				if(!invert)
					setRelayClose(relayNum);
				else
					setRelayOpen(relayNum);
				break;
			
			case RELAY_MASK_OPEN :
				if(invert)
					setRelayClose(relayNum);
				else
					setRelayOpen(relayNum);
				break;
			
				case RELAY_MASK_MOMENTARY :
					setRelayClose(relayNum);
					setTaskRelayOff(relayNum);
					break;
			
			default: break;
		}
	}
}

void presetChangeProcess(ButtonEvent *buttonEvent)
{
	uint8_t buttonNum = buttonEvent->buttonNum_;
	if(runtimeEnvironment.activePresetButtonNumber_ == buttonNum)//If push on active preset button - send TAP message
	{
		sendTapTempo(buttonEvent);
		return;
	}
	
	if(buttonEvent->actionType_ != BUTTON_PUSH)//Send PC only on button push
		return;
		
	uint8_t i;
	runtimeEnvironment.activePresetButtonNumber_ = buttonNum;
	runtimeEnvironment.activePresetNumber_ = bank.buttonContext[buttonNum].presetChangeContext.programsNumbers[0];
	
	tapMomentaryLock = 0;
	selectNewPresetCommonPart(buttonNum);
	
	//Send 3 other program changes
	uint8_t midiChannelToSend;
	for(i = 1; i < MAX_PROGRAMS_TO_SEND; ++i)//First PC already sent
	{
		midiChannelToSend = bank.buttonContext[buttonNum].presetChangeContext.midiChannelNumbers[i];
		if(midiChannelToSend >= MIDI_CHANNEL_PRIMARY)
			midiChannelToSend = global.midiChanNum;

		if(bank.buttonContext[buttonNum].presetChangeContext.midiChannelNumbers[i] != MIDI_CHANNEL_NOT_ACTIVE)//This mean no message send in this position
		{
			sendPcWithOptionalBs(bank.buttonContext[buttonNum].presetChangeContext.programsNumbers[i]
							,bank.buttonContext[buttonNum].presetChangeContext.banksNumbers[i]
							,bank.buttonContext[buttonNum].presetChangeContext.midiChannelNumbers[i]
							,global.bankSelectMessType
							,global.useBankSelectMess);
		}
	}
	
	uint16_t iaState = bank.buttonContext[buttonNum].presetChangeContext.iaState;
	uint8_t valToSend;
	for(i = 0; i < FOOT_BUTTONS_NUM; ++i)
	{
		//set IA states according new preset
		runtimeEnvironment.currentIaState_[i] = ((iaState >> i) & 0x01) ? IA_STATE_ON : IA_STATE_OFF; 
		//Some of CC_tg buttons should send its new value to midi out
		if(bank.buttonType[i] == CC_TOGGLE && bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.autoSendState != 0)
		{
			valToSend = runtimeEnvironment.currentIaState_[i] == IA_STATE_ON ? bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue : bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.paramLsbOffValue;
			midiSendControlChange(bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber, valToSend, global.midiChanNum);
		}
	}

	//set relays
	uint8_t currenRelays = bank.buttonContext[buttonNum].relays;
	relayActionCommon(currenRelays, false);
/*	runtimeEnvironment.isTimeToShowTuner_ = false;//Turn off tuner after preset change*/
	updateRequests.updateLedsRq_ = 1;
	updateRequests.updatePedalLedsRq_ = 1;
	updateRequests.updateScreenRq_ = 1;
}

void ccToggleProcess(ButtonEvent* buttonEvent)
{
//	if(buttonEvent->actionType_ != BUTTON_PUSH)//only on button push
//		return;
	
	uint8_t valToSend = 0;
	uint8_t buttonNum = buttonEvent->buttonNum_;
	
	if(buttonEvent->actionType_ == BUTTON_PUSH)
	{
		if(runtimeEnvironment.currentIaState_[buttonNum] == IA_STATE_ON)
		{
			runtimeEnvironment.currentIaState_[buttonNum] = IA_STATE_OFF;
			valToSend = bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramLsbOffValue;
		}
		else
		{
			runtimeEnvironment.currentIaState_[buttonNum] = IA_STATE_ON;
			valToSend = bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue;
		}
	
		uint8_t ccNumToSend = bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber;
		if(ccNumToSend != CC_OFF_VALUE)//if user not set "OFF" value here
			midiSendControlChange(ccNumToSend, valToSend, global.midiChanNum);
	
		//set relays
		uint8_t currenRelays = bank.buttonContext[buttonNum].relays;
		bool invert = runtimeEnvironment.currentIaState_[buttonNum] == IA_STATE_OFF ? true : false;
		relayActionCommon(currenRelays, invert);
		updateRequests.updateLedsRq_ = true;
	
		//if button is under the pedal - update pedal leds
		if(buttonEvent->buttonNum_ == FOOT_BUTTONS_NUM-1)
			updateRequests.updatePedalLedsRq_ = true;
	}
	else if(buttonEvent->actionType_ == BUTTON_HOLDON || buttonEvent->actionType_ == BUTTON_RELEASE_AFTER_HOLD)
	{
		uint8_t ccNumToSend = bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlMsbFreezeNumber;
		if(ccNumToSend != CC_OFF_VALUE)
		{
			valToSend = (buttonEvent->actionType_ == BUTTON_HOLDON) ? bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue : bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramLsbOffValue;
			midiSendControlChange(ccNumToSend, valToSend, global.midiChanNum);
		}
	}
	else return;
}

void ccMomentaryProcess(ButtonEvent* buttonEvent)
{
	uint8_t buttonNum = buttonEvent->buttonNum_;
	uint8_t ccNumToSend = bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber;

	if(buttonEvent->actionType_ == BUTTON_PUSH)
	{
		if(ccNumToSend != CC_OFF_VALUE)//if user not set "OFF" value here
		{
			midiSendControlChange(bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber
							,bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue
							,global.midiChanNum);
		}
							
		uint8_t currenRelays = bank.buttonContext[buttonNum].relays;
		relayActionCommon(currenRelays, false);//Relay acts only on button push event if button type is Momentary CC
	}
	else if(buttonEvent->actionType_ == BUTTON_RELEASE || buttonEvent->actionType_ == BUTTON_RELEASE_AFTER_HOLD)
	{
		if(ccNumToSend != CC_OFF_VALUE)//if user not set "OFF" value here
		{
			midiSendControlChange(bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber
					,bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramLsbOffValue
					,global.midiChanNum);
		}
	}
}

void ccConstValProcess(ButtonEvent* buttonEvent)
{
	if(buttonEvent->actionType_ != BUTTON_PUSH)//only on button push
		return;
	
	uint8_t buttonNum = buttonEvent->buttonNum_;
	
	if(runtimeEnvironment.lastCcConstButton_ == buttonNum)//If push on active preset button - send TAP message
	{
		sendTapTempo(buttonEvent);
		return;
	}
		
	runtimeEnvironment.lastCcConstButton_ = buttonNum;
	uint8_t ccNumToSend = bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber;

	if(ccNumToSend != CC_OFF_VALUE)//if user not set "OFF" value here		
	{
		midiSendControlChange(bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber
				,bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue
				,global.midiChanNum);
	}
				
	uint8_t currenRelays = bank.buttonContext[buttonNum].relays;
	relayActionCommon(currenRelays, false);

	updateRequests.updateLedsRq_ = true;
	if(runtimeEnvironment.isAxeFx3Connected_ || runtimeEnvironment.isAxeFxConnected_)
	{
		runtimeEnvironment.axeSceneChanged_ = true;
	}
}

void nrpnToggleProcess(ButtonEvent* buttonEvent)
{
	if(buttonEvent->actionType_ != BUTTON_PUSH)//only on button push
		return;
	
	uint8_t valToSendLsb;
	uint8_t valToSendMsb;
	uint8_t buttonNum = buttonEvent->buttonNum_;
	runtimeEnvironment.lastCcConstButton_ = buttonNum;
	if(runtimeEnvironment.currentIaState_[buttonNum] == IA_STATE_ON)
	{
		runtimeEnvironment.currentIaState_[buttonNum] = IA_STATE_OFF;
		valToSendMsb = bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramMsbOffValue;
		valToSendLsb = bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramLsbOffValue;
	}
	else
	{
		runtimeEnvironment.currentIaState_[buttonNum] = IA_STATE_ON;
		valToSendMsb = bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramMsbOnValue;
		valToSendLsb = bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue;
	}
	
	midiSendNrpn(bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlMsbFreezeNumber
				,bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber
				,valToSendMsb
				,valToSendLsb
				,global.midiChanNum);
				
	updateRequests.updateLedsRq_ = true;	
}

void nrpnMomentaryProcess(ButtonEvent* buttonEvent)
{
	uint8_t buttonNum = buttonEvent->buttonNum_;
	if(buttonEvent->actionType_ == BUTTON_PUSH)
	{
		midiSendNrpn(bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlMsbFreezeNumber
			,bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber
			,bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramMsbOnValue
			,bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue
			,global.midiChanNum);
	}
	else if(buttonEvent->actionType_ == BUTTON_RELEASE || buttonEvent->actionType_ == BUTTON_RELEASE_AFTER_HOLD)
	{
		midiSendNrpn(bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlMsbFreezeNumber
			,bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber
			,bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramMsbOffValue
			,bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramLsbOffValue
			,global.midiChanNum);
	}	
}

void nrpnConstValProcess(ButtonEvent* buttonEvent)
{
	if(buttonEvent->actionType_ != BUTTON_PUSH)//only on button push
	return;
	
	uint8_t buttonNum = buttonEvent->buttonNum_;

	midiSendNrpn(bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlMsbFreezeNumber
				,bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber
				,bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramMsbOnValue
				,bank.buttonContext[buttonNum].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue
				,global.midiChanNum);
}

void noteProcess(bool isOn, ButtonEvent *buttonEvent)
{
	if(buttonEvent->actionType_ != BUTTON_PUSH)//only on button push
		return;
	
	uint8_t buttonNum = buttonEvent->buttonNum_;	
	midiSendNote(true
			,bank.buttonContext[buttonNum].commonContext.noteOnConstContext_.noteNumber
			,bank.buttonContext[buttonNum].commonContext.noteOnConstContext_.noteVelocity
			,global.midiChanNum);
}

void presetUpProcess(ButtonEvent* buttonEvent)
{
	if(buttonEvent->actionType_ != BUTTON_PUSH)//only on button push
		return;
		
	if (cycleIncUcahrVal(&runtimeEnvironment.activePresetNumber_, MAX_PROGRAM_CHANGE_MESSAGE_VALUE))
	{
		if(global.useBankSelectMess == USE_BANK_SELECT)//Also need to increment BS message value here
			cycleIncUcahrVal(&runtimeEnvironment.activeBankNumber_, MAX_BANK_SELECT_MESSAGE_VALUE);
	}
	
	sendPcWithOptionalBs(runtimeEnvironment.activePresetNumber_
		,runtimeEnvironment.activeBankNumber_
		,global.midiChanNum
		,global.bankSelectMessType
		,global.useBankSelectMess);

	
	if (runtimeEnvironment.isAxeFxConnected_ == true)//Если акс подключен то отправим запрос на состояние эффектов
	{
		requestAxefxInfo(IA_STATE_FUNCTION_ID);
		runtimeEnvironment.isTimeToShowPresetName_ = 0;//и будем показывать имя банка, пока не придет имя пресета
	}
	updateRequests.updateScreenRq_++;
		
}

void presetDownProcess(ButtonEvent* buttonEvent)
{
	if(buttonEvent->actionType_ != BUTTON_PUSH)//only on button push
	return;
	
	if (cycleDecUcahrVal(&runtimeEnvironment.activePresetNumber_, MAX_PROGRAM_CHANGE_MESSAGE_VALUE))
	{
		if(global.useBankSelectMess == USE_BANK_SELECT)//Also need to increment BS message value here
			cycleDecUcahrVal(&runtimeEnvironment.activeBankNumber_, MAX_BANK_SELECT_MESSAGE_VALUE);
	}
	
	sendPcWithOptionalBs(runtimeEnvironment.activePresetNumber_
			,runtimeEnvironment.activeBankNumber_
			,global.midiChanNum
			,global.bankSelectMessType
			,global.useBankSelectMess);

	if (runtimeEnvironment.isAxeFxConnected_ == true)//Если акс подключен то отправим запрос на состояние эффектов
	{
		requestAxefxInfo(IA_STATE_FUNCTION_ID);
		runtimeEnvironment.isTimeToShowPresetName_ = 0;//и будем показывать имя банка, пока не придет имя пресета
	}
	updateRequests.updateScreenRq_++;	
}

void bankChangeProcess(ButtonEvent* buttonEvent, ButtonType buttonType)
{
	if(buttonEvent->actionType_ != BUTTON_PUSH)//only on button push
	return;
	
	if(buttonType == BANK_UP)
	{
		if(runtimeEnvironment.activeBankNumber_ < global.maxBankNumber - 1)
			++runtimeEnvironment.activeBankNumber_;
		else
			runtimeEnvironment.activeBankNumber_ = 0;
	}
	else if(buttonType == BANK_DOWN)
	{
		if(runtimeEnvironment.activeBankNumber_ > 0)
			--runtimeEnvironment.activeBankNumber_;
		else
			runtimeEnvironment.activeBankNumber_ = global.maxBankNumber;		
	}
	else//BANK_TO
	{
		uint8_t buttonNum = buttonEvent->buttonNum_;
		if (bank.buttonContext[buttonNum].bankNumber <= global.maxBankNumber - 1)
		{
			runtimeEnvironment.activeBankNumber_ = bank.buttonContext[buttonNum].bankNumber;
		}
			
	}

	LoadBank(runtimeEnvironment.activeBankNumber_, &bank);
	updateRequests.updateScreenRq_ = true;			
}

void tunerOnOffProcess()
{
	runtimeEnvironment.isTimeToShowTuner_ = !runtimeEnvironment.isTimeToShowTuner_;
	updateRequests.updateScreenRq_ = true;
	setEventByMask(EVENT_MASK_TUNER_CHANGE);
	if(bank.tunerCc != CC_OFF_VALUE)
	{
		uint8_t valToSend = runtimeEnvironment.isTimeToShowTuner_ ? IA_STATE_ON : IA_STATE_OFF;
		midiSendControlChange(bank.tunerCc, valToSend, global.midiChanNum);
	}
}


void footswitchesProcess(ButtonEvent *buttonEvent)
{
	if(buttonEvent->buttonNum_ >= FOOT_BUTTONS_NUM)
		return;
		
	ButtonType buttonType = bank.buttonType[buttonEvent->buttonNum_];
	switch(buttonType)
	{
		case PRESET_CHANGE :
			presetChangeProcess(buttonEvent);
		break;
		
		case CC_TOGGLE :
			ccToggleProcess(buttonEvent); 
		break;
		
		case CC_MOMENTARY :
			ccMomentaryProcess(buttonEvent); 
		break;
		
		case CC_CONST_VAL :
			ccConstValProcess(buttonEvent); 
		break;
		
		case NRPN_TOGGLE :
			nrpnToggleProcess(buttonEvent);
		break;
		
		case NRPN_MOMENTARY :
			nrpnMomentaryProcess(buttonEvent); 
		break;
		
		case NRPN_CONST_VAL :
			nrpnConstValProcess(buttonEvent);
		break;
		
		case NOTE_ON : 
			noteProcess(true, buttonEvent);
		break;

		case NOTE_OFF :
			noteProcess(false, buttonEvent);	
		break;
		
		case PRESET_UP :
			presetUpProcess(buttonEvent); 
		break;
		
		case PRESET_DOWN :
			presetDownProcess(buttonEvent);  
		break;
		
		case BANK_UP :
		case BANK_DOWN :
		case BANK_TO :
			bankChangeProcess(buttonEvent, buttonType);
		break;
		
		default: break;
	}
	//toggle tuner on any button hold, exept CCT with freeze
	if(buttonEvent->actionType_ == BUTTON_HOLDON)
	{
		if(!(buttonType == CC_TOGGLE && bank.buttonContext[buttonEvent->buttonNum_].commonContext.contolAndNrpnChangeContext_.ctrlMsbFreezeNumber != CC_OFF_VALUE))
			tunerOnOffProcess();
	}
}
