#include "settings.h"
#include "bjdevlib_tb.h"
#include "MidiHelpers.h"
#include "RuntimeEnvironment.h"
#include "PedalLedProcess.h"
#include "TaskManager.h"
#include "lcd_tb.h"
#include "GmajorMidiEventsProcess.h"

uint8_t parse_GM_tuner(uint8_t sys_ex_length, uint8_t *sys_ex_data, uint8_t *note, uint8_t *tune)//������� ������� sys_ex ��������� �� ������� ������ � ��� ��������� ������ �� G-Major
//sys_ex_length - ������ ���������� ���������, *sys_ex_data - ��������� �� ������ ������,
// *note - ���� ������� ������� ��������� ����,  *tune - �������� ���� ������������ �������� ����
//���� ������� ���������� ���������� ��������� � ��������� ��� �������� ����� ������������ 1, ����� 0 
{
    uint8_t retval = 0;
    if (sys_ex_length == 10)//���� ������ ��������� 9 ����, �� ���� ����������� ��� ��� �� ��� �� ����
    {
        if(sys_ex_data[8] == 0xF7)//���� ��������� ���� ��������� - 0xF7 ������� ��� ������ ��������� ������
            //� ���������� ���� �� ��� ��������� ���������, �� ��� ����� ������, ���� ��� ����� �������
        {
            retval = 1;
            *note = sys_ex_data[6];//��������� ���� ����
            *tune = sys_ex_data[7];//���� �������� ���� ������������ �������� ����
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
        
