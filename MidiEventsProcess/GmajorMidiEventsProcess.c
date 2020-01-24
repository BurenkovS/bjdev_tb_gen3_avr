#include "settings.h"
#include "bjdevlib_tb.h"
#include "MidiHelpers.h"
#include "RuntimeEnvironment.h"
#include "PedalLedProcess.h"
#include "TaskManager.h"
#include "lcd_tb.h"
#include "GmajorMidiEventsProcess.h"

uint8_t parse_GM_tuner(uint8_t sys_ex_length, uint8_t *sys_ex_data, uint8_t *note, uint8_t *tune)//Функция разбора sys_ex сообщение на предмет поиска в нем сообщений тюнера от G-Major
//sys_ex_length - длинна пришедшего сообщения, *sys_ex_data - указатель на массив данных,
// *note - сюда функция запишет пришедшую ноту,  *tune - смещение тона относительно основной ноты
//Если функция распарсила правильное сообщение и заполнила все выходные данне возвращается 1, иначе 0 
{
    uint8_t retval = 0;
    if (sys_ex_length == 10)//если длинна сообщения 9 байт, то есть вероятность что это то что мы ищем
    {
        if(sys_ex_data[8] == 0xF7)//если последний байт сообщения - 0xF7 считаем что пришло сообщение тюнера
            //в реальности надо бы еще разобрать заголовок, но это потом сделаю, када еще процы добавлю
        {
            retval = 1;
            *note = sys_ex_data[6];//Извлекаем байт ноты
            *tune = sys_ex_data[7];//Байт смещения тона относительно основной ноты
        }
    }
    return retval;
}

void handleMidiEventGMajor(	uint8_t in_MessType
							,uint8_t* midiMessage
							,uint8_t sys_ex_length)
{
    uint8_t sysExModelId;
    uint8_t* sys_ex_data = midiMessage + 1;

    if (in_MessType != MIDI_SYSEX_START)
        return;

    if(global.targetDevice == TARGET_DEVICE_AUTO || global.targetDevice == TARGET_DEVICE_G_MAJOR)// try to handle G-Major tuner   
    {
        sysExModelId = *(midiMessage + 3);
        if (sysExModelId == GMAJOR1)//parse just tuner message from G-major
        {
            if (parse_GM_tuner(sys_ex_length, sys_ex_data, &runtimeEnvironment.tunerNote_, &runtimeEnvironment.tunerTune_))
            {
				drawPedalTuner(runtimeEnvironment.tunerTune_);
                if(runtimeEnvironment.isTimeToShowTuner_ == true)
				{
					setEventByMask(EVENT_MASK_TUNER_CHANGE);
                   	updateRequests.updateScreenRq_ = 1;
				}
            }
        }
    }
}
        
