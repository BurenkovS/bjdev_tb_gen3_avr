/*
 * settings.c
 *
 * Created: 19.03.2017 13:17:11
 *  Author: 123
 */ 

#include "settings.h"
#include "eeprom_wrapper.h"
#include "FirmwareVersion.h"
#include "RuntimeEnvironment.h"
#include "lcd_tb.h"
#include "MidiHelpers.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "log.h"


//Read eeprom header with firmware version, check this version match current version. If everything is OK  - return true 
bool checkEepromHeader()
{
	FirmwareVersionInfoInEeprom firmwareVersionInfoInEeprom;
	//Check version
	ReadEEPROM((uint8_t*)&firmwareVersionInfoInEeprom, FirmwareVersionInfoInEeprom_ADDR, sizeof(firmwareVersionInfoInEeprom));
	if(0 != strcmp(firmwareVersionInfoInEeprom.version, FIRMWARE_VERSION_STRING))
		return false;
	else
		return true;
}

void setDefaultGlobalSettings()
{
	global.bnkNum = 0;//Текущий номер банка. В настройках он меняется когда выбираем банк для сохранения/загрузки( Banks )
	global.midiChanNum = 0;//System Setup -> MIDI channel. Диапазон от 0 до 15, на экране отображается от 1 до 16
	global.useBankSelectMess = DONT_USE_BANK_SELECT;//System Setup -> Prg. ch. mode
	global.bankSelectMessType = MSB;//System Setup -> Bnk. Sel mode
	global.bnkSwOnBoard = NO_EXT_PEDAL;//System Setup -> Bank sw. mode
	global.Show_pr_name = PRESET_AND_BANK;//System Setup -> Show pr. name
	global.targetDevice = TARGET_DEVICE_AUTO;//System Setup -> Target device
	global.usbBaudrate = MIDI_BAUD;//System Setup -> USB baudrate
	global.inputThrough[0] = IN_TO_NONE; //System Setup ->MIDI thru map
	global.inputThrough[1] = IN_TO_NONE;
	global.maxBankNumber = BanksCount;//System Setup ->Max. bank.  
	global.screenBrightness = 10;//System Setup -> Screen brightness
	global.screenContrast = 255;//System Setup ->Screen contrast
		

	global.buttonHoldTime = ms1500;//Exp&Tap&Tune -> BUT hold time. Диапазон от 1 до 15, в меню отобржажается как 100ms, 200ms, ..., 1500ms
	global.tapDisplayType = ON_SCREEN;//Exp&Tap&Tune -> Tap display
	global.tapType = TAP_CONST_VALUE;//Exp&Tap&Tune -> Tap type
	global.pedalLedView = PEDAL_LED_MODE_DUAL;//Pedal view -> Display type
	global.pedalTunerScheme = Scheme1;
	global.pedalBrightness = 5;
	
	uint8_t i = 0;
	for(i = 0; i < 3; ++i)
	{
		global.pLowPos[i] = 0;
		global.pHighPos[i] = 127;
		global.expPtype[i] = LINEAL;
	}
}

void setDefaultBankSettings()
{
	memset((void*)&bank, 0, sizeof(bank));
	strcpy(bank.BankName, "Unnamed 1");
	bank.pedalsCc[0] = 1; 
	bank.pedalsCc[1] = 7;
	bank.pedalsCc[2] = 11;
	bank.pedalsCc[3] = 128;
	bank.tapCc = 30;
	bank.tunerCc = 31;
	
	uint32_t i;
	for (i = 0; i < FOOT_BUTTONS_NUM; ++i)
	{
		bank.buttonType[i] = PRESET_CHANGE;//All buttons will preset switchers
		bank.buttonContext[i].presetChangeContext.programsNumbers[0] = i;//preset numbers will match button number;
		//Send only first PC message
		bank.buttonContext[i].presetChangeContext.midiChannelNumbers[0] = MIDI_CHANNEL_PRIMARY;
		bank.buttonContext[i].presetChangeContext.midiChannelNumbers[1] = MIDI_CHANNEL_NOT_ACTIVE;
		bank.buttonContext[i].presetChangeContext.midiChannelNumbers[2] = MIDI_CHANNEL_NOT_ACTIVE;
		bank.buttonContext[i].presetChangeContext.midiChannelNumbers[3] = MIDI_CHANNEL_NOT_ACTIVE;
		
		//CC and NRpN context
		bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.autoSendState = 0;
		bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber = 26;	
		bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.ctrlMsbFreezeNumber = 128;
		bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.paramLsbOffValue = 0x00;	
		bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.paramMsbOffValue = 0x00;
		bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue = 127;
		bank.buttonContext[i].commonContext.contolAndNrpnChangeContext_.paramMsbOnValue = 127;
	}
}

void loadDefaults(uint32_t banksCount)
{
	LCDClear();
	LCDWriteStringXY(0,0,"Loading defaults");
	LCDWriteStringXY(0,1,"Bank     of   ");
	LCDWriteIntXY(12,1,banksCount, 3);
	
	setDefaultGlobalSettings();//create default global settings and write to EEPROM
	WriteEEPROM((uint8_t*)&global, GlobalSettings_ADDR, sizeof(global));
		
	setDefaultBankSettings();//Set default bank setting and replicate it to all banks
	uint16_t i;
	char bnkNameWithNum[16];
	for(i = 0; i < banksCount; ++i)
	{
		LCDWriteIntXY(5,1,i+1, 3);

		//Create unique bank name
		sprintf(bnkNameWithNum, "Unnamed %d", i+1);//TODO possible need remove sprintf to reduce code size
		strcpy((char*)&(bank.BankName), bnkNameWithNum);
		WriteEEPROM((uint8_t*)&bank, BankSettings_ADDR(i), sizeof(bank));
	}
}

void startupSettingsCheckAndLoad()
{
	
	//Check external EEPROM is present
	if(eepromPresentStartupCheck())
	{
		BanksCount = ((TOTAL_EEPROM_SIZE_BYTES - sizeof(FirmwareVersionInfoInEeprom) - sizeof(GlobalSettings))/sizeof(BankSettings));
		if(BanksCount > 128) 
			BanksCount = 128;
	}
	else
	{
		BanksCount = 1;//((INTERNAL_EEPROM_SIZE_BYTES - sizeof(FirmwareVersionInfoInEeprom) - sizeof(GlobalSettings))/sizeof(BankSettings));
	}
	
	if (checkEepromHeader())
	{
		//load setting from EEPROM

		ReadEEPROM((uint8_t*)&global, GlobalSettings_ADDR, sizeof(GlobalSettings));
		if(global.maxBankNumber > BanksCount)//crutch
			global.maxBankNumber = BanksCount;
		
		runtimeEnvironment.activeBankNumber_ = global.bnkNum;
		
			
		ReadEEPROM((uint8_t*)&bank, BankSettings_ADDR(runtimeEnvironment.activeBankNumber_), sizeof(BankSettings));
		/*LCDClear();
		LCDWriteStringXY(0,1,"Banks :  ");
		LCDWriteIntXY(8,1,BanksCount, 3);
		_delay_ms(500);*/
		return;
	}
	
	LCDClear();
	//ask to load defaults
	bool loadDef = false;
	LCDWriteStringXY(0,0,"Load defaults?");
	LCDWriteStringXY(0,1,"OK/NO");

	ButtonEvent buttonCmd;
	while(1)//wait ok or no button push
	{
		buttonCmd = getButtonLastEvent();
		if(buttonCmd.actionType_ == BUTTON_PUSH)
		{
			if(buttonCmd.buttonNum_ == KEY_LOAD)
			{
				loadDef = true;
				break;
			}
			else if(buttonCmd.buttonNum_ == KEY_SETUP)
			{
				break;
			}
		}
		
	}


	if(loadDef)
		loadDefaults(BanksCount);
	//Write correct version
	WriteEEPROM((uint8_t*)FIRMWARE_VERSION_STRING, FirmwareVersionInfoInEeprom_ADDR, sizeof(FIRMWARE_VERSION_STRING));
		
	LCDClear();
	LCDWriteStringXY(0,0,"Done!");
	LCDWriteStringXY(0,1,"Banks :  ");
	LCDWriteIntXY(8,1,BanksCount, 3);
	_delay_ms(500);
	LCDClear();
}

void InitializeGlobalSettings(void)
{
	ReadEEPROM((uint8_t*)&global, GlobalSettings_ADDR, sizeof(GlobalSettings));
}

void SaveBank(uint8_t BankNumber, BankSettings *bnk)
{
	WriteEEPROM((uint8_t*)bnk, BankSettings_ADDR(BankNumber), sizeof(BankSettings));
}

void LoadBank(uint8_t BankNumber, BankSettings *bnk)
{
	ReadEEPROM((uint8_t*)bnk, BankSettings_ADDR(BankNumber), sizeof(BankSettings));
	runtimeEnvironment.activeBankNumber_ = BankNumber;
}