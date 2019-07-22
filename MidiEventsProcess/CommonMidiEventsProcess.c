/*
 * ComonMidiEventsProcess.c
 *
 * Created: 29.08.2017 16:47:09
 *  Author: Sergey
 */ 

#include "CommonMidiEventsProcess.h"
#include "settings.h"
#include "RuntimeEnvironment.h"
#include "bjdevlib_tb.h"

void handleMidiEventCommonMidiDevices(uint8_t in_MessType
		,uint8_t in_ChanNum
		,uint8_t* midiMessage)
{
	uint8_t i;
	uint8_t ccNum = midiGetControllerNumber(midiMessage);
	uint8_t ccVal = midiGetControllerValue(midiMessage);
	if (in_MessType == MIDI_CONTROL_CHANGE && in_ChanNum == global.midiChanNum)  //Если пришло CC сообшение по нужному каналу, обработаем его
	{
		for (i = 0; i <= FOOT_BUTTONS_NUM - 1; i++)
		{
			if (bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber == ccNum)//Если номер входящего миди сообщения есть
			{
				runtimeEnvironment.currentIaState_[i] = ccVal;//Выставим нужный эффект в нужное значение
				updateRequests.updateLedsRq_ = 1;//Update IA state LED
			}
		}
	}
}
