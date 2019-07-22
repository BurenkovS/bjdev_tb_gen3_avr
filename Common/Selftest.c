#include "selftest.h"
#include "eeprom_wrapper.h"
#include "settings.h"
#include "lcd_tb.h"
#include "bjdevlib_tb.h"

#include <stdint.h>
#include <stdbool.h>

#if defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)
#   define TOTAL_PEDALS_FOR_SELFTEST 3
#else
#   define TOTAL_PEDALS_FOR_SELFTEST 2
#endif

#define BYTES_TO_MIDI_CHECK_FOR_SELFTEST 128//8192

#define HIGH_LEVEL 250 //Expected ADC level for opened relay
#define LOW_LEVEL  5   //Expected ADC level for closed relay


void waitOkButton()
{
    ButtonEvent buttonCmd;
    buttonCmd = getButtonLastEvent();
    while(!(buttonCmd.actionType_ == BUTTON_PUSH && buttonCmd.buttonNum_ == KEY_LOAD))
    {
        buttonCmd = getButtonLastEvent();
    }
}

void startSelfTest()
{
    uint32_t i;
    uint32_t j;
    uint8_t adcData = 255;
    uint8_t adcDataPrev = 16;
    uint32_t midiErrors = 0;
    uint8_t relay1Status = 0;
    uint8_t relay2Status = 0;
    uint8_t relay3Status = 0;
    uint8_t relay4Status = 0;
    char strBad[4] = "bad";
    char strOk[4] = " ok";
    uint8_t adcNumsForPedals[3] = {EXP_P1_ADC_CHAN, EXP_P2_ADC_CHAN, EXP_P_ONBPAR_ADC_CHAN};
    ButtonEvent buttonCmd;
    buttonCmd.actionType_ = BUTTON_NO_EVENT;

    LCDClear();
    LCDWriteString("Start self test");

	ledSetColorAll(COLOR_BLACK, true);
	
#if defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)
    ledSetPedalColorAll(PEDAL_COLOR_B, true);
#endif

    //all relays off
    for(i = 0; i < RELAYS_NUM; ++i)
        setRelayOpen(i);

    _delay_ms(1000);

    //check all buttons
    for(i = 0; i < FOOT_BUTTONS_NUM+CONF_BUTTONS_NUM; ++i)
    {
        LCDClear();
		LCDWriteStringXY(0,0, "PUSH BUTTON ");
		LCDWriteIntXY(12, 0, i+1, 2);
        while(!(buttonCmd.actionType_ == BUTTON_PUSH && buttonCmd.buttonNum_ == i))
        {
            buttonCmd = getButtonLastEvent();
        }
    }

    //check all LEDS
    LCDClear();
    LCDWriteStringXY(0,0, "LEDs GREEN->OK");
	ledSetColorAll(COLOR_GREEN, true);

    waitOkButton();

    LCDClear();
    LCDWriteStringXY(0,0,"LEDs RED->OK");
	ledSetColorAll(COLOR_RED, true);
    
	waitOkButton();

   ledSetColorAll(COLOR_BLACK, true);

    //Pedal LED test
#if defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)
    LCDClear();
    LCDWriteStringXY(0, 0, "PEDAL RED->OK");
    ledSetPedalColorAll(PEDAL_COLOR_R, 1);
    waitOkButton();

    LCDClear();
    LCDWriteStringXY(0, 0, "PEDAL GREEN->OK");
    ledSetPedalColorAll(PEDAL_COLOR_G, 1);
    waitOkButton();

    LCDClear();
    LCDWriteStringXY(0, 0, "PEDAL BLUE->OK");
    ledSetPedalColorAll(PEDAL_COLOR_B, 1);
    waitOkButton();
    ledSetPedalColorAll(PEDAL_COLOR_NO, 1);
#endif

    //Pedal position test
    for(i = 0; i < TOTAL_PEDALS_FOR_SELFTEST; ++i)
    {
        LCDClear();
		LCDWriteStringXY(0, 0, "PEDAL ");
		LCDWriteIntXY(6, 0, i+1, 3);
        LCDWriteStringXY(0, 9, "PEDAL ");
        while(!(buttonCmd.actionType_ == BUTTON_PUSH && buttonCmd.buttonNum_ == KEY_LOAD))
        {
            buttonCmd = getButtonLastEvent();
            adcData = adcRead8MsbBit(adcNumsForPedals[i]);
            if(adcData != adcDataPrev)
            {
                LCDWriteIntXY(0,1, adcData, 3);
                adcDataPrev = adcData;
                _delay_ms(5);
            }
        }
        buttonCmd.actionType_ = BUTTON_NO_EVENT;
    }

    //MIDI test. Cable should be connected IN to OUT
    //Test run two times
    for(j = 0; j < 2; ++j)//running twice to check both cables 
    {
        LCDClear();
        LCDWriteStringXY(0,0,"Connect MIDI cable");
        LCDWriteStringXY(0,1,"IN to OUT");
        waitOkButton();
        LCDClear();
        LCDWriteStringXY(0,0,"MIDI test start");
        LCDWriteStringXY(0,1,"Sd:     Rc:    ");
        for(i = 0; i < BYTES_TO_MIDI_CHECK_FOR_SELFTEST; ++i)
        {
            uart0PutChar((char)i);
            LCDGotoXY(3,1);
            LCDWriteIntXY(3,1,i,3);
            if (uart0GetChar() != (char)i)
                midiErrors++;
            LCDWriteIntXY(11,1,i,3);
        }
        LCDClear();
        LCDWriteStringXY(0,0,"MIDI test done");
		LCDWriteStringXY(0,1,"Errors : ");
		LCDWriteIntXY(9,1, midiErrors, 3);
        if(midiErrors) while(1){};//Stop test if errors occurred
    }

    //Check relays
    LCDClear();
    LCDWriteStringXY(0,0,"Connect Y cable");
    LCDWriteStringXY(0,1,"SW 1/2<->EXP");
    waitOkButton();
    //SW1/2 opened
    if(adcRead8MsbBit(EXP_P1_ADC_CHAN) < HIGH_LEVEL)
        relay1Status++;
    if(adcRead8MsbBit(EXP_P2_ADC_CHAN) < HIGH_LEVEL)
        relay2Status++;

    //SW1 closed, SW2 opened
	setRelay(0,1);
	setRelay(1,0);
    _delay_ms(10);
    if(adcRead8MsbBit(EXP_P1_ADC_CHAN) > LOW_LEVEL)
        relay1Status++;
    if(adcRead8MsbBit(EXP_P2_ADC_CHAN) < HIGH_LEVEL)
        relay2Status++;

    //SW1 opened, SW2 closed
	setRelay(0,0);
	setRelay(1,1);

    _delay_ms(10);
    if(adcRead8MsbBit(EXP_P1_ADC_CHAN) < HIGH_LEVEL)
        relay1Status++;
    if(adcRead8MsbBit(EXP_P2_ADC_CHAN) > LOW_LEVEL)
        relay2Status++;

    //SW1 closed, SW2 closed
	setRelay(0,1);
	setRelay(1,1);
    _delay_ms(10);
    if(adcRead8MsbBit(EXP_P1_ADC_CHAN) > LOW_LEVEL)
        relay1Status++;
    if(adcRead8MsbBit(EXP_P2_ADC_CHAN) > LOW_LEVEL)
        relay2Status++;

    //Check SW3/4
    LCDClear();    
    LCDWriteStringXY(0,0,"Connect Y cable\n");
    LCDWriteStringXY(0,1,"SW 3/4<->EXP");
    waitOkButton();

    //SW3/4 opened
    if(adcRead8MsbBit(EXP_P1_ADC_CHAN) < HIGH_LEVEL)
        relay3Status++;
    if(adcRead8MsbBit(EXP_P2_ADC_CHAN) < HIGH_LEVEL)
        relay4Status++;

    //SW3 closed, SW4 opened
	setRelay(2,1);
	setRelay(3,0);
    _delay_ms(10);
    if(adcRead8MsbBit(EXP_P1_ADC_CHAN) > LOW_LEVEL)
        relay3Status++;
    if(adcRead8MsbBit(EXP_P2_ADC_CHAN) < HIGH_LEVEL)
        relay4Status++;

    //SW3 opened, SW4 closed
	setRelay(2,0);
	setRelay(3,1);

    _delay_ms(10);
    if(adcRead8MsbBit(EXP_P1_ADC_CHAN) < HIGH_LEVEL)
        relay3Status++;
    if(adcRead8MsbBit(EXP_P2_ADC_CHAN) > LOW_LEVEL)
        relay4Status++;

    //SW3 closed, SW4 closed
	setRelay(2,1);
	setRelay(3,1);

    _delay_ms(10);
    if(adcRead8MsbBit(EXP_P1_ADC_CHAN) > LOW_LEVEL)
        relay3Status++;
    if(adcRead8MsbBit(EXP_P2_ADC_CHAN) > LOW_LEVEL)
        relay4Status++;

    LCDClear();
    LCDWriteStringXY(0,0, "SW1     SW2    ");
    LCDWriteStringXY(4, 0, relay1Status ? strBad : strOk);
	LCDWriteStringXY(12, 0, relay2Status ? strBad : strOk);
	LCDWriteStringXY(0, 1, "SW3     SW4    ");
	LCDWriteStringXY(4, 1, relay3Status ? strBad : strOk);
	LCDWriteStringXY(12, 1, relay4Status ? strBad : strOk);
    if(relay1Status+relay2Status+relay3Status+relay4Status)
		while(1){};//Stop test if errors occurred
			
	//external eeprom test
	for(i = 0; i < EXTERNAL_EEPROM_ADDR_WIDTH; ++i)
		EEPROM_writeByte((1 << i), i);
	
	uint8_t byteFromEeprom;
	for(i = 0; i < EXTERNAL_EEPROM_ADDR_WIDTH; ++i)
	{
		byteFromEeprom = EEPROM_readByte((1 << i));
		if(byteFromEeprom != i)
		{
			LCDClear();
			LCDWriteStringXY(0,0, "EEPROM error");
			while(1){};//Stop test if errors occurred			
		}	
	}

	uint32_t banksCount = ((TOTAL_EEPROM_SIZE_BYTES - sizeof(FirmwareVersionInfoInEeprom) - sizeof(GlobalSettings))/sizeof(BankSettings));
	if(banksCount > 128)
		banksCount = 128;

	loadDefaults(banksCount);
}
