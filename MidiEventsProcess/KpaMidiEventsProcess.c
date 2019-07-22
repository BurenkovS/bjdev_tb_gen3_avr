/*
 * KpaMidiEventsProcess.c
 *
 * Created: 29.08.2017 16:59:31
 *  Author: Sergey
 */ 

#include "KpaMidiEventsProcess.h"
#include "settings.h"
#include "bjdevlib_tb.h"
#include "MidiHelpers.h"
#include "RuntimeEnvironment.h"
#include "lcd_tb.h"
#include "Maintenance.h"
#include "PedalLedProcess.h"
#include "TaskManager.h"


#include <avr/pgmspace.h>


static uint8_t main_mode = 0;//Selected Main Mode (0=Browse/1=Perform)
//static uint8_t enabled_slot = 0;//Number of enabled slot
/*char slot_names[MAX_KPA_STRING_LENGTH*MAX_KPA_SLOTS] = 
        {   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//slots names  */
            
char rig_name[MAX_KPA_STRING_LENGTH] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//rig name
char perfomance_name[MAX_KPA_STRING_LENGTH] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

uint16_t get_parameter_num(const uint8_t* message)
{
    return (*(message + 7) << 8) | *(message + 8);//parameter_number is 7 and 8 for high and low part 
}

uint64_t get_ex_parameter_num(const uint8_t* message)
{
    //Extended parameters has offset 11(LS), 10 9 8 7 (MS), 
    //but we will use only 4 byte as current KPA version always set MSB to 0 
    return ((unsigned long int)*(message + 8) << 24 |   \
            (unsigned long int)*(message + 9) << 16 |   \
            (unsigned long int)*(message + 10) << 8 |   \
            *(message + 11));
}


const uint8_t head[6] PROGMEM = {0xF0, 0x00, 0x20, 0x33, 0x02, 0x7F};
void send_kpa_sysex_head()
{
    uint8_t i = 0;
    for(i = 0; i < sizeof(head); ++i)
        uart0PutChar(pgm_read_byte(&head[i]));
}


const uint8_t message_name_to_kpa[22] PROGMEM = {0x03, 0x00, 0x7F, 0x7F, 'B', 'J', ' ', 'D', 'e', 'v', 'i', 'c', 'e', 's', ' ', 'b', 'o', 'a', 'r', 'd', 0x00, 0xF7};
void send_name_to_kpa()
{
    uint8_t i = 0;
    
    send_kpa_sysex_head();
    
    for(i = 0; i < sizeof(message_name_to_kpa); ++i)
        uart0PutChar(pgm_read_byte(&message_name_to_kpa[i]));
}

    /*“Flags”: Several bits that represent the Boolean value for specific options for the command or set. 
    Bit 0 “INIT”: If the set should be initially sent after enabling the bidirectional mode (1)
    Bit 1 “SYSEX”: If the set should send NRPN CC (0) or use SYSEX (1)2
    Bit 2 “ECHO”: If set, parameters are sent back even if they were set via MIDI IN (1)
    Bit 3 “NOFE”: if set, the KPA will stop sending 0xFE when protocol is initiated
    Bit 4 “NOCTR”: if set, the KPA will not send the “sensing” command(see Table 2)
    Bit 5 “TUNEMODE”: if set, the Tuning information is only sent in Tuner Mode, otherwise it’s being sent all the time.
    Bit 6 *reserved* set to 0
    */
    const uint8_t message_beacon[7] PROGMEM = {0x7E, 0x00, 0x40, 
                             0x02,              //Number of requested set
                             0x00,              //Flags
                             0x00,              //Time Lease. 0 - stop, 1..126 - 2,4,6...252 sec, 127 - inf.
                             0xF7};             //End sysex
                             
void send_beacon_to_kpa(const uint8_t flags, const uint8_t time)
{
    uint8_t i;
    
    send_kpa_sysex_head();
    
    for(i = 0; i < sizeof(message_beacon); ++i)
    {
       if (i == 4)//Flags
            uart0PutChar(flags);  
       else if (i == 5) //Time Lease
            uart0PutChar(time);
       else
            uart0PutChar(pgm_read_byte(&message_beacon[i]));
    }
}

char is_kpa_message(const uint8_t* message)
{
    if(*message == 0x00 && *(message + 1) == 0x20 && *(message + 2) == 0x33)//Access/Kemper Manufacturer ID
        return 1;
    else
        return 0;
}



const uint8_t conv_KPA2GMajor_tune[128] PROGMEM = {16,16,16,16,16,16,16,16,16,16,
                                        17,17,17,17,17,17,17,17,17,
                                        18,18,18,18,18,18,18,18,17,
                                        19,19,19,19,19,19,19,19,
                                        20,20,20,20,20,20,20,20,
                                        21,21,21,21,21,21,21,21,
                                        22,22,22,22,22,22,22,22,
                                        23,23,23,23,//fine
                                        24,24,24,24,//fine
                                        25,25,25,25,25,25,25,25,
                                        26,26,26,26,26,26,26,26,
                                        27,27,27,27,27,27,27,27,
                                        28,28,28,28,28,28,28,27,
                                        29,29,29,29,29,29,29,29,30,
                                        30,30,30,30,30,30,30,30,30,
                                        31,31,31,31,31,31,31,31,31,31};

uint8_t parce_kpa_tuner(const uint8_t* message, uint8_t* note, uint8_t* tune)
{
    unsigned int pm = get_parameter_num(message);

    static uint8_t prev_note = 0;
    static uint8_t prev_tune = 0;
        
    //Tuner is single parameter change function
    if (*(message + FCODE_OFFSET) != SINGLE_CHANGE)
        return 0;
        
    if(pm == NOTE)
    {
        if (*(message + NOTE_OFFSET) != prev_note)//If need to recalculate note
        {
            //Notes nums is the same as G-Major 
            *note = *(message + NOTE_OFFSET) % 12;
            prev_note = *(message + NOTE_OFFSET);
            return 1;
        }
        return 0;
    }
    
    else if (pm == TUNING)
    {
        if (*(message + H_TUNE_OFFSET) != prev_tune)//If need to recalculate note
        {
            *tune = pgm_read_byte(&conv_KPA2GMajor_tune[*(message + H_TUNE_OFFSET)]);
            prev_tune = *(message + H_TUNE_OFFSET);
            return 1;
        }
        return 0;
    }
    else
    {
        return 0;
    }
}

char parce_main_mode(const uint8_t* message)
{
    unsigned int pm = get_parameter_num(message);

    if (*(message + FCODE_OFFSET) != SINGLE_CHANGE)
        return 0;

    if(pm == MAIN_MODE)
    {
        main_mode = *(message + SINGLE_CHANGE_LSB_OFFSET);
        return 1;
    }
    return 0;
}

//TODO Implement some functionality with slots names and numbers
/*char parce_enabled_slot_num(char* message)
{
    unsigned long int pm = get_ex_parameter_num(message);
    char i;

    if (*(message + FCODE_OFFSET) != EX_SINGLE_CHANGE)
        return 0;

    //Enables slot parameners starts from pm == 0x010000
	for (i = 0; i < MAX_KPA_SLOTS; ++i)
    {
        if (((0x010000 | (i & 0xFF)) == pm) && (*(message + EX_SINGLE_CHANGE_LSB_OFFSET) == 0x01))
        {
            enabled_slot = i;
            return 1;
        }
    }
    return 0;
}*/

/*char parce_enabled_slot_name(char* message)
{
    unsigned long int pm = get_ex_parameter_num(message);
    char i;

    if (*(message + FCODE_OFFSET) != EX_STRING)
        return 0;

    //Slots names parameners starts from pm == 0x010001
	for (i = 1; i <= MAX_KPA_SLOTS; ++i)
    {
        if ((0x010000 | (i & 0xFF)) == pm)
        {
            strcpy_limits((message + EX_STRING_OFFSET), (slot_names + (i-1)*MAX_KPA_STRING_LENGTH), MAX_KPA_STRING_LENGTH);
            return 1;
        }
    }
    return 0;
}*/

char parce_kpa_rig_name(const uint8_t* message)
{
    unsigned int pm = get_parameter_num(message);
   
    //Rig Name is string Parameter change function
    if (*(message + FCODE_OFFSET) != STRING)
        return 0;
        
    if (pm == RIG_NAME)
    {
        strcpyLimits((const char*)(message + STRING_OFFSET), rig_name, MAX_KPA_STRING_LENGTH - 1);
        return 1;
    }
    return 0;
}

char parce_kpa_perfomance_name(const uint8_t* message)
{
    unsigned long int pm = get_ex_parameter_num(message);

    if (*(message + FCODE_OFFSET) != EX_STRING)
        return 0;

    if (pm == (unsigned long int)KPA_PERFOMANCE_NAME)
    {
        strcpyLimits((const char*)(message + EX_STRING_OFFSET), perfomance_name, MAX_KPA_STRING_LENGTH - 1);
        return 1;
    }
    return 0;
}

char parce_name_for_display(const uint8_t* message, char* name)
{
    char ret = parce_main_mode(message);
    //ret += parce_enabled_slot_num(message);
    //ret += parce_enabled_slot_name(message);
    ret +=parce_kpa_perfomance_name(message);
    ret += parce_kpa_rig_name(message);
    if(ret)
    {
        if (main_mode)//perfomance mode
        {
           //strcpy_limits((slot_names + enabled_slot*MAX_KPA_STRING_LENGTH), name, MAX_KPA_STRING_LENGTH);//slot name to screen
            strcpyLimits(perfomance_name, name, MAX_KPA_STRING_LENGTH - 1);
        }
        else//browse mode
        {
            strcpyLimits(rig_name, name, MAX_KPA_STRING_LENGTH - 1);
        }
    }
    
    return ret;
}

const uint16_t ia_nrpn_nums[] PROGMEM =   {NRPN_STOMP_A,  NRPN_STOMP_B,   NRPN_STOMP_C,   NRPN_STOMP_D,   NRPN_STOMP_X,  NRPN_MOD,    NRPN_DELAY,     NRPN_DELAY_SPOL,    NRPN_REVERB,    NRPN_REVERB_SPOL};
const uint8_t ia_cc_nums[] PROGMEM =  {CC_STOMP_A,    CC_STOMP_B,     CC_STOMP_C,     CC_STOMP_D,     CC_STOMP_X,    CC_MOD,      CC_DELAY,       CC_DELAY_SPOL,      CC_REVERB,      CC_REVERB_SPOL};
                                    
char update_kpa_ia_state(uint8_t* message)
{
    uint16_t pm = get_parameter_num(message);
    uint8_t val;
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t ret = 0;
    
    //IA state is single parameter change function
    if (*(message + FCODE_OFFSET) != SINGLE_CHANGE)
        return 0;

    for (i = 0; i < sizeof(ia_nrpn_nums)/sizeof(ia_nrpn_nums[0]); ++i)
    {
        if (pm == pgm_read_word(&ia_nrpn_nums[i]))
        {
            for (j = 0; j < FOOT_BUTTONS_NUM; ++j)//loop from 0 to 11 if tb12 and from 0 to 4 if tb-5
            {
                if (bank.buttonContext[j].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber == pgm_read_byte(&ia_cc_nums[i]))
                {   
                    val = *(message + IA_STATE_VALUE_OFFSET) != 0x00 ? IA_STATE_ON : IA_STATE_OFF;  
					runtimeEnvironment.currentIaState_[j] = val;//Âûñòàâèì íóæíûé ýôôåêò â íóæíîå çíà÷åíèå;
                    ret++;
                }
            }
        }
    }
	return ret;
}

char is_kpa_tap(uint8_t* message)
{
    unsigned int pm = get_parameter_num(message);

    //Tap is single parameter change function
    if (*(message + FCODE_OFFSET) != SINGLE_CHANGE)
        return 0;

    //Check parameter num is TAP and TAP value is 1.
    //KPA send two messages per tempo beat - 1 for led on, 0 for led off.
    //Only "led on" message needed, as "led off" time calculates internally
    if (pm == KPA_TAP_EVENT && (*(message + IA_STATE_VALUE_OFFSET) == 0x01))
        return 1;
    else
        return 0;
}

void handleMidiEventKpa(uint8_t in_MessType
        ,uint8_t* midiMessage)
{
	
	uint8_t* sys_ex_data = midiMessage + 1;//just remove status byte 
    if (in_MessType == MIDI_SENSE && (global.targetDevice == TARGET_DEVICE_AUTO || global.targetDevice == TARGET_DEVICE_KPA))
    {
        if (runtimeEnvironment.isKpaConnected_ == false) // ACTIVE_SENSING øëåò òîëüêî KEMPER
        {
            send_name_to_kpa();
            LCDClear();
            LCDWriteString("Kemper Profiler");
            setTaskMessageOff();
            send_beacon_to_kpa(KPA_INIT | KPA_SYSEX | KPA_NOCTR /*| KPA_NOFE*/, 0x7F);
            updateRequests.updateScreenRq_ = 1;
            runtimeEnvironment.isKpaConnected_ = true;
			setTaskDisconnectKpa();
        }
    }
    else if (in_MessType == MIDI_SYSEX_START && global.targetDevice != TARGET_DEVICE_OFF)//if SysEx is enabled
    {
        //KPA
		if(is_kpa_message(sys_ex_data) != 0)
		{
			setTaskDisconnectKpa();
			if (runtimeEnvironment.isKpaConnected_ == true)
			{
				if (parce_kpa_tuner(sys_ex_data, &runtimeEnvironment.tunerNote_, &runtimeEnvironment.tunerTune_))
				{
					drawPedalTuner(runtimeEnvironment.tunerTune_);
					if(runtimeEnvironment.isTimeToShowTuner_ == true)
					{
						setEventByMask(EVENT_MASK_TUNER_CHANGE);
						updateRequests.updateScreenRq_ = 1;
					}
				}
                    
				if (global.Show_pr_name != ONLY_BANK)//If rig name display required
				{
					if (parce_name_for_display(sys_ex_data, runtimeEnvironment.vendorPresetName_))
					{
						updateRequests.updateScreenRq_ = 1;
						runtimeEnvironment.isTimeToShowPresetName_ = 1;
						if(global.Show_pr_name == PRESET_AND_BANK)//if need to show in turn the preset and the bank names.
							setTaskSwitchBetweenPresetNameAndBankName();						
					}
				}
                    
				if (update_kpa_ia_state(sys_ex_data))
					updateRequests.updateLedsRq_ = 1;

				if (is_kpa_tap(sys_ex_data))
					runtimeEnvironment.showTapRq_ = 1;
			}
		}
    }
}

