/*
 * CallbackFunctions.c
 *
 * Created: 15.03.2017 20:27:49
 *  Author: 123
 */ 

#include "lcd_tb.h"
#include "CallbackFunctions.h"
#include "MenuItems.h"
#include "settings.h"
#include "eeprom_wrapper.h"
#include "RuntimeEnvironment.h"
#include "log.h"
#include "TunerOnLcd.h"
#include "MidiHelpers.h"
#include "ExpressionProcess.h"
#include "PwmControllableFeatures.h"

#include <string.h>

static volatile uint8_t ProgramIndex;

/*Строковые константы для отображения на дисплее*/
const char UseBankSelectMess_str1[] PROGMEM = "USE SELECT     ";
const char UseBankSelectMess_str2[] PROGMEM = "DONT USE SELECT";
PGM_P const UseBankSelectMess_value[] PROGMEM = {UseBankSelectMess_str1, UseBankSelectMess_str2};
	
const char BankSelectMessType_str1[] PROGMEM = "MSB        ";
const char BankSelectMessType_str2[] PROGMEM = "LSB        ";
const char BankSelectMessType_str3[] PROGMEM = "MSB AND LSB";
	
const char ShowPresetBank_str1[] PROGMEM = "ONLY PRESET    ";
const char ShowPresetBank_str2[] PROGMEM = "ONLY BANK      ";
const char ShowPresetBank_str3[] PROGMEM = "PRESET AND BANK";
	
const char targetDevice_str_auto[] PROGMEM =		"AUTO         ";
const char targetDevice_str_off[] PROGMEM =			"OFF          ";
const char targetDevice_str_axe_st[] PROGMEM =		"AXE ST       ";
const char targetDevice_str_axe_ultra[] PROGMEM =	"AXE ULTRA    ";
const char targetDevice_str_axeii[] PROGMEM =		"AXEII        ";
const char targetDevice_str_axeii_xl[] PROGMEM =	"AXEII XL     ";
const char targetDevice_str_kpa[] PROGMEM =			"KPA          ";
const char targetDevice_str_g_major[] PROGMEM =		"G MAJOR      ";
const char targetDevice_str_axeii_plus[] PROGMEM =	"AXEII XL PLUS";
const char targetDevice_str_ax8[] PROGMEM =			"AX8          ";
const char targetDevice_str_axeiii[] PROGMEM =		"AXEIII       ";
											
const char USB_baudrate_str1[] PROGMEM = "MIDI BAUD ";
const char USB_baudrate_str2[] PROGMEM = "57600 BAUD";

const char Restore_defaults_str1[] PROGMEM = "CONFIRM";
	
const char BnkSwOnBoard_str1[] PROGMEM = "NO EXT. PEDAL";
const char BnkSwOnBoard_str2[] PROGMEM = "EXT. PEDAL   ";
	
const char InputThrough_str1[] PROGMEM = "NONE          ";
const char InputThrough_str2[] PROGMEM = "MIDI OUT      ";
const char InputThrough_str3[] PROGMEM = "USB OUT       ";
const char InputThrough_str4[] PROGMEM = "MIDI & USB OUT";
	
const char ExpPedalType_str1[] PROGMEM = "LINEAL  ";
const char ExpPedalType_str2[] PROGMEM = "LOG     ";
const char ExpPedalType_str3[] PROGMEM = "BACK LOG";
	
const char HoldTime_str1[] PROGMEM =	"100 ms ";
const char HoldTime_str2[] PROGMEM =	"2";
const char HoldTime_str3[] PROGMEM =	"3";
const char HoldTime_str4[] PROGMEM =	"4";
const char HoldTime_str5[] PROGMEM =	"5";
const char HoldTime_str6[] PROGMEM =	"6";
const char HoldTime_str7[] PROGMEM =	"7";
const char HoldTime_str8[] PROGMEM =	"8";
const char HoldTime_str9[] PROGMEM =	"900 ms ";
const char HoldTime_str10[] PROGMEM =	"1000 ms";
const char HoldTime_str11[] PROGMEM =	"11";
const char HoldTime_str12[] PROGMEM =	"12";
const char HoldTime_str13[] PROGMEM =	"13";
const char HoldTime_str14[] PROGMEM =	"14";
const char HoldTime_str15[] PROGMEM =	"1500 ms";
										
const char TapDisplayType_str1[] PROGMEM = "NOT SHOW TAP ";
const char TapDisplayType_str2[] PROGMEM = "ON SCREEN    ";
const char TapDisplayType_str3[] PROGMEM = "ON ACT PRESET";
const char TapDisplayType_str4[] PROGMEM = "LED1         ";
const char TapDisplayType_str5[] PROGMEM = "LED2";
const char TapDisplayType_str6[] PROGMEM = "LED3";
const char TapDisplayType_str7[] PROGMEM = "LED4";
const char TapDisplayType_str8[] PROGMEM = "LED5          ";
const char TapDisplayType_str9[] PROGMEM = "LED6          ";
const char TapDisplayType_str10[] PROGMEM = "LED7";
const char TapDisplayType_str11[] PROGMEM = "LED8        ";
const char TapDisplayType_str12[] PROGMEM = "LED9";
const char TapDisplayType_str13[] PROGMEM = "LED10";
const char TapDisplayType_str14[] PROGMEM = "LED11       ";
const char TapDisplayType_str15[] PROGMEM = "LED12       ";
												
const char TapType_str1[] PROGMEM = "TOGGLE     ";
const char TapType_str2[] PROGMEM = "MOMETRARY  ";
const char TapType_str3[] PROGMEM = "CONST VALUE";
	
const char PedalLedView_str1[] PROGMEM = "MODE BUTTON";
const char PedalLedView_str2[] PROGMEM = "MODE TUNER ";
const char PedalLedView_str3[] PROGMEM = "MODE DUAL  ";
	
const char PedalTunerScheme_str1[] PROGMEM = "Scheme 1";
const char PedalTunerScheme_str2[] PROGMEM = "Scheme 2";
	
const char ButtonType_str1[] PROGMEM =  "PC      ";
const char ButtonType_str2[] PROGMEM =  "CCT";
const char ButtonType_str3[] PROGMEM =  "CCM";
const char ButtonType_str4[] PROGMEM =  "CCC   ";
const char ButtonType_str5[] PROGMEM =  "NRPN_T";
const char ButtonType_str6[] PROGMEM =  "NRPN_M";
const char ButtonType_str7[] PROGMEM =  "NRPN_C ";
const char ButtonType_str8[] PROGMEM =  "NOTE ON ";
const char ButtonType_str9[] PROGMEM =  "NOTE OFF ";
const char ButtonType_str10[] PROGMEM = "PRESET UP  ";
const char ButtonType_str11[] PROGMEM = "PRESET DOWN";
const char ButtonType_str12[] PROGMEM = "BANK UP    ";
const char ButtonType_str13[] PROGMEM = "BANK DOWN  ";
const char ButtonType_str14[] PROGMEM = "BANK TO  ";
PGM_P const ButtonTypes[] PROGMEM = {ButtonType_str1, ButtonType_str2, ButtonType_str3, ButtonType_str4, ButtonType_str5, ButtonType_str6, ButtonType_str7
									,ButtonType_str8, ButtonType_str9, ButtonType_str10, ButtonType_str11, ButtonType_str12, ButtonType_str13, ButtonType_str14};
										  
const char Channel_str1[] PROGMEM = " P";
const char Channel_str2[] PROGMEM = " N";

const char Message1_str[] PROGMEM = "Mes1:C    N    ";
const char Message2_str[] PROGMEM = "Mes2:";
const char Message3_str[] PROGMEM = "Mes3:";
const char Message4_str[] PROGMEM = "Mes4:C    N    ";

const char Type_change_str[] PROGMEM = "Typ:           ";
const char Clear_str[] PROGMEM = "                ";
const char ClearCurrentValue_str[] PROGMEM = "    ";

const char RLYmsg_str[] PROGMEM = "RLY 1  2  3  4";
const char RLY_str1[] PROGMEM = "N";
const char RLY_str2[] PROGMEM = "C";
const char RLY_str3[] PROGMEM = "O";
const char RLY_str4[] PROGMEM = "M";

const char ccNumber_str[] PROGMEM =			"CC num:        ";
const char ccFreezeNumber_str[] PROGMEM =	"CC fr num:    ";

const char ON_str[] PROGMEM = "ON ";
const char OFF_str[] PROGMEM = "OFF";

const char initState_str[] PROGMEM = "Init state:  ";

const char autoSendState_str[] PROGMEM = "Auto send:     ";

const char ccValue_str[] PROGMEM = "ON     OFF    ";

const char ctrlNumber_str[] PROGMEM = "#99     #98    ";

const char paramValue_str[] PROGMEM = "#6     #38     ";

const char noteNumber_str[] PROGMEM = "Note number:";

const char Velocity_str[] PROGMEM =	"Velocity:      ";

const char bankNumber_str[] PROGMEM = "Bank:      ";

const char CopyBankSrcDst_str[] PROGMEM = "SRC:    DST:";

const char Main_screen_str[] PROGMEM = "PR:";

const char expPedalCalibrationPosition_str[] PROGMEM = "Position:      ";

//forward decalration
void ExitCurrentExecuteFunction(void);
/*Контекст выполнения текущей функции*/
FunctionContext CurrentFunctionContext;

void setSibling(Menu_Item_t	*Next, Menu_Item_t	*Previous)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	NextTypeSibling = Next;
	PrevTypeSibling = Previous;
}

/*Установка соседник элементов меню для элемента Type*/
void setSiblingMenu(ButtonType type)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	if (type == PRESET_CHANGE)
	{
		setSibling(&Message1, &iaState);
	}
	else if (type == CC_TOGGLE)
	{
		setSibling(&CCT_ccNumber, &CCT_RLY);
	}
	else if (type == CC_MOMENTARY)
	{
		setSibling(&CCM_ctrlLsbNumber, &CCM_RLY);
	}
	else if (type == CC_CONST_VAL)
	{
		setSibling(&CCC_ctrlLsbNumber, &CCC_RLY);
	}
	else if (type == NRPN_TOGGLE)
	{
		setSibling(&NRPN_TOGGLE_ctrlNumber, &NRPN_TOGGLE_RLY);
	}
	else if (type == NRPN_MOMENTARY)
	{
		setSibling(&NRPN_MOMENTARY_ctrlNumber, &NRPN_MOMENTARY_RLY);
	}
	else if (type == NRPN_CONST_VAL)
	{
		setSibling(&NRPN_CONST_ctrlNumber, &NRPN_CONST_RLY);
	}
	else if ((type == NOTE_ON) || (type == NOTE_OFF))
	{
		setSibling(&noteNumber, &noteVelocity);
	}
	else if (type == BANK_TO)
	{
		setSibling(&bankNumber, &bankNumber);
	}
	else
	{
		setSibling(NULL, NULL);
	}
}

void setIndicator(uint8_t indicator, uint8_t line)
{
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	LCDGotoXY(15, line);
	LCDData(indicator);
}

void setCursor(uint8_t position)
{
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	setIndicator(' ', 1);
	
	LCDGotoXY(position, 1);
	LcdShowCursor();
}

void DisplayUpdateFooter(const char* Text)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	char text[17];
	
	LCDWriteStringXY(0, 1, strcpy_P(text, Text));
}

void DisplayUpdateHeader(const char* Text)
{
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	char text[17];
	
	LCDWriteStringXY(0, 0, strcpy_P(text, Text));
}

/*Обновление текущего значения для элементов меню System Setup*/
void Display_UpdateSystemSetupValue(FunctionContext *CurrentFunctionContext)
{
	LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentMenuPosition = MenuSystemSetupInside;
	
	CurrentFunctionContext->ExecuteFunction = CommonExecuteFunction;
	setCursor(CurrentFunctionContext->position);
}

void Menu_SettingsSelectCallback(void)
{
	LOG(SEV_TRACE,"%s", __FUNCTION__);	
	ClearCurrentContext();
	
	setIndicator(0x00, 0);
	
	CurrentMenuPosition = MenuSettings;	
	
	NextTypeSibling = NULL;
	PrevTypeSibling = NULL;
	
	DisplayUpdateFooter(Clear_str);
}

void Menu_MainScreenRedrawCallback(void)
{
	if(updateRequests.eventFlags_ !=0) //some special update need
	{
		if(checkEventByMask(EVENT_MASK_MAIN_SCREEN_NAME_CHANGE))
		{
			LCDWriteStringXY(1, 0, "               ");//Clear string
			if((runtimeEnvironment.isTimeToShowPresetName_ && global.Show_pr_name == PRESET_AND_BANK) || global.Show_pr_name == ONLY_PRESET)
				LCDWriteStringXY(1, 0, runtimeEnvironment.vendorPresetName_);
			else
				LCDWriteStringXY(1, 0, bank.BankName);
		}
	
		if(checkEventByMask(EVENT_MASK_TUNER_CHANGE))
		{
			if(runtimeEnvironment.isTimeToShowTuner_)
				drawTunerOnLcd(1, runtimeEnvironment.tunerNote_,runtimeEnvironment.tunerTune_);
			else
				Menu_MainScreenSelectCallback();
		}
		
		if(checkEventByMask(EVENT_MASK_TAP_CHANGE))
		{
			if(runtimeEnvironment.tapState_)//draw TAP symbols in last char on each line
			{
				LCDGotoXY(15,0);
				LCDData(CUSTOM_CHAR_TAP);
				LCDGotoXY(15,1);
				LCDData(CUSTOM_CHAR_TAP);
			}
			else
			{
				LCDGotoXY(15,0);
				LCDData(' ');
				LCDGotoXY(15,1);
				LCDData(' ');				
			}
		}
		
		//if(checkEventByMask(EVENT_MASK_EXP_CHANGE)){}//Do nothing
		
		clearEvents();
	}
	else//just to redraw main screen
	{
		Menu_MainScreenSelectCallback();	
	}
}

void Menu_MainScreenSelectCallback()
{
	if(runtimeEnvironment.isTimeToShowTuner_)//tuner displayed on second line of screen, no need to clear it
		LCDWriteStringXY(1, 0, "               ");
	else
		LCDClear();
	
	LCDGotoXY(0,0);	
	LCDData(CUSTOM_CHAR_HOME);
			
	if((runtimeEnvironment.isTimeToShowPresetName_ && global.Show_pr_name == PRESET_AND_BANK) || global.Show_pr_name == ONLY_PRESET)
		LCDWriteStringXY(1, 0, runtimeEnvironment.vendorPresetName_);
	else
		LCDWriteStringXY(1, 0, bank.BankName);
	
	if(!runtimeEnvironment.isTimeToShowTuner_)//tuner displayed on second line of screen
		DisplayUpdateFooter(Main_screen_str);
	
	ClearCurrentContext();
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 127;
	CurrentFunctionContext.position = 3;
	
	CurrentFunctionContext.value = &runtimeEnvironment.activePresetNumber_;
	CurrentFunctionContext.valueAddr = 0xFFFFFFFF;//GlobalSettings_ADDR + offsetof(GlobalSettings, midiChanNum);
	//CurrentFunctionContext.ExecuteFunction = CommonExecuteFunction;
	CurrentFunctionContext.ExecuteFunction = MainScreenExecuteFunction;
	if(!runtimeEnvironment.isTimeToShowTuner_)
		DisplayUpdateCurrentValue(runtimeEnvironment.activePresetNumber_ + 1, CurrentFunctionContext.position);
	
	CurrentMenuPosition = MenuMain;	
}

void Menu_SystemSetupEnterCallback(void)
{
	LOG(SEV_TRACE,"%s", __FUNCTION__);	
	Menu_Navigate(MENU_CHILD);
}

/*Callback функции для вызова из пунктов меню System Settings.
  В данных функциях устанавливается контекст выполнения CurrentFunctionContext.
*/
void Menu_SystemSettingsEnterCallback(void)
{
	LOG(SEV_TRACE,"%s", __FUNCTION__);	
	setIndicator(' ', 0);
	Display_UpdateSystemSetupValue(&CurrentFunctionContext);
}

void Menu_MIDIchannelSelectCallback(void)
{
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();
	
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = MAX_MIDI_CHANNEL_NUMBER;
	CurrentFunctionContext.type = SHOW_OFFSET_BY_ONE;
	CurrentFunctionContext.position = 0;
	
	CurrentFunctionContext.value = &global.midiChanNum;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, midiChanNum);
	
	DisplayUpdateCurrentValue(global.midiChanNum, CurrentFunctionContext.position);
	
}

void Menu_Prg_ch_modeSelectCallback(void)
{
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();
	
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 1;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.text[0] = UseBankSelectMess_str1;
	CurrentFunctionContext.text[1] = UseBankSelectMess_str2;
	CurrentFunctionContext.value = &global.useBankSelectMess;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, useBankSelectMess);
	
	DisplayUpdateCurrentValue(global.useBankSelectMess, CurrentFunctionContext.position);
}

void Bnk_Sel_modeSelectCallBack(void)
{
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();
	
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 2;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.text[0] = BankSelectMessType_str1;
	CurrentFunctionContext.text[1] = BankSelectMessType_str2;
	CurrentFunctionContext.text[2] = BankSelectMessType_str3;
	CurrentFunctionContext.value = &global.bankSelectMessType;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, bankSelectMessType);
	
	DisplayUpdateCurrentValue(global.bankSelectMessType, CurrentFunctionContext.position);
}

void Show_pr_nameSelectCallBack(void)
{
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 2;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.text[0] = ShowPresetBank_str1;
	CurrentFunctionContext.text[1] = ShowPresetBank_str2;
	CurrentFunctionContext.text[2] = ShowPresetBank_str3;
	CurrentFunctionContext.value = &global.Show_pr_name;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, Show_pr_name);
	
	DisplayUpdateCurrentValue(global.Show_pr_name, CurrentFunctionContext.position);
}

void Target_deviceSelectCallback(void)
{
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 10;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.text[TARGET_DEVICE_AUTO] = targetDevice_str_auto;
	CurrentFunctionContext.text[TARGET_DEVICE_OFF] = targetDevice_str_off;
	CurrentFunctionContext.text[TARGET_DEVICE_AXE_ST] = targetDevice_str_axe_st;
	CurrentFunctionContext.text[TARGET_DEVICE_AXE_ULTRA] = targetDevice_str_axe_ultra;
	CurrentFunctionContext.text[TARGET_DEVICE_AX8] = targetDevice_str_ax8;
	CurrentFunctionContext.text[TARGET_DEVICE_AXEII] = targetDevice_str_axeii;
	CurrentFunctionContext.text[TARGET_DEVICE_AXEII_XL] = targetDevice_str_axeii_xl;
	CurrentFunctionContext.text[TARGET_DEVICE_AXEII_XL_PLUS] = targetDevice_str_axeii_plus;
	CurrentFunctionContext.text[TARGET_DEVICE_AXEIII] = targetDevice_str_axeiii;
	CurrentFunctionContext.text[TARGET_DEVICE_KPA] = targetDevice_str_kpa;
	CurrentFunctionContext.text[TARGET_DEVICE_G_MAJOR] = targetDevice_str_g_major;
	CurrentFunctionContext.value = &global.targetDevice;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, targetDevice);
	
	DisplayUpdateCurrentValue(global.targetDevice, CurrentFunctionContext.position);
}

void USB_baudrateSelectCallback(void)
{
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 1;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.text[0] = USB_baudrate_str1;
	CurrentFunctionContext.text[1] = USB_baudrate_str2;
	CurrentFunctionContext.value = &global.usbBaudrate;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, usbBaudrate);
	DisplayUpdateCurrentValue(global.usbBaudrate, CurrentFunctionContext.position);
	//set new baudrate
	initUart1(global.usbBaudrate);

}

void Menu_BnkSwOnBoardSelectCallback(void)
{
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 1;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.text[0] = BnkSwOnBoard_str1;
	CurrentFunctionContext.text[1] = BnkSwOnBoard_str2;
	CurrentFunctionContext.value = &global.bnkSwOnBoard;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, bnkSwOnBoard);
	
	DisplayUpdateCurrentValue(global.bnkSwOnBoard, CurrentFunctionContext.position);
}

void MIDI_thru_mapEnterCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	Menu_Navigate(MENU_CHILD);
}

void Menu_MIDI_out_mapSelectCallback(void)
{	
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 3;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.text[0] = InputThrough_str1;
	CurrentFunctionContext.text[1] = InputThrough_str2;
	CurrentFunctionContext.text[2] = InputThrough_str3;
	CurrentFunctionContext.text[3] = InputThrough_str4;
	CurrentFunctionContext.value = &global.inputThrough[0];
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, inputThrough[0]);
	
	DisplayUpdateCurrentValue(global.inputThrough[0], CurrentFunctionContext.position);
}

void Menu_USB_out_mapSelectCallback(void)
{	
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 3;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.text[0] = InputThrough_str1;
	CurrentFunctionContext.text[1] = InputThrough_str2;
	CurrentFunctionContext.text[2] = InputThrough_str3;
	CurrentFunctionContext.text[3] = InputThrough_str4;
	CurrentFunctionContext.value = &global.inputThrough[1];
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, inputThrough[1]);
	
	DisplayUpdateCurrentValue(global.inputThrough[1], CurrentFunctionContext.position);
}

void Menu_Bank_maxSelectCallback(void)
{	
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 1;
	CurrentFunctionContext.max = BanksCount;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.value = &global.maxBankNumber;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, maxBankNumber);
	
	DisplayUpdateCurrentValue(global.maxBankNumber, CurrentFunctionContext.position);
}

void Restore_defaultsExecuteFunction()
{
	if (CurrentFunctionContext.action == ValueSaveAndExit)
	{
		LCDClear();
		LCDWriteString("Resetting...");
		_delay_ms(500);
		WriteEEPROM("000", FirmwareVersionInfoInEeprom_ADDR, sizeof("000"));//reset version info from EEPROM. If wrong version will detected, all setting becomes default on start
		WDTCR=0x18;
		WDTCR=0x08;		
		while(1){};//wait for watchdog reset
			
		ExitCurrentExecuteFunction();
		return;
	}
	else if (CurrentFunctionContext.action == ValueRestoreAndExit)
	{
		ExitCurrentExecuteFunction();
		return;
	}
	else
		return;
}

void Menu_Restore_defaultsSelectCallback(void)
{
	Menu_SettingsSelectCallback();
	ClearCurrentContext();
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 0;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.text[0] = Restore_defaults_str1;
	//CurrentFunctionContext.ExecuteFunction = Restore_defaultsExecuteFunction;
	DisplayUpdateCurrentValue(0, CurrentFunctionContext.position);
}

void Menu_Restore_defaultsEnterCallback(void)
{
	setIndicator(' ', 0);
	CurrentMenuPosition = MenuSystemSetupInside;
	CurrentFunctionContext.ExecuteFunction = Restore_defaultsExecuteFunction;
	setCursor(CurrentFunctionContext.position);
}



void Menu_PwmControllableExecuteFunction(void)
{
	LOG(SEV_TRACE,"%s", __FUNCTION__);
	uint8_t *value = CurrentFunctionContext.value;
	
	if (CurrentFunctionContext.action == ValueIncrement)
	{
		if (((*value)++) == CurrentFunctionContext.max)
		*value = CurrentFunctionContext.min;
	}
	else if (CurrentFunctionContext.action == ValueDecrement)
	{
		if (((*value)--) == CurrentFunctionContext.min)
		*value = CurrentFunctionContext.max;
	}
	else if (CurrentFunctionContext.action == ValueSaveAndExit)
	{
		ExitCurrentExecuteFunction();
		return;
	}
	else if (CurrentFunctionContext.action == ValueRestoreAndExit)
	{
		ReadEEPROM(value, CurrentFunctionContext.valueAddr, sizeof(uint8_t));
		ExitCurrentExecuteFunction();
		return;
	}
	else
		return;
	
	DisplayUpdateCurrentValue(*value, CurrentFunctionContext.position);
	
	setCursor(CurrentFunctionContext.position);
	
	/*************************/
	uint8_t pwmNum = pgm_read_byte(&(Menu_GetCurrentMenu()->NumericParam));
	uint8_t pwmVal = 0;

	if(pwmNum == SCREEN_BRIGHTNESS_PWM_NUM)
		pwmVal =  global.screenBrightness;
		
	else if(pwmNum == SCREEN_CONTRAST_PWM_NUM)
		pwmVal =  global.screenContrast;
		
	else if(pwmNum == PEDAL_BRIGHTNESS_PWM_NUM)
		pwmVal =  global.pedalBrightness;

	LOG(SEV_TRACE,"%s %d %d, max %d min %d", __FUNCTION__, pwmNum, pwmVal, CurrentFunctionContext.max, CurrentFunctionContext.min);
	setPwmValueWithScale(pwmNum, pwmVal);
}


void Screen_brightnessSelectCallback(void)
{
	LOG(SEV_TRACE,"%s", __FUNCTION__);
	Menu_SettingsSelectCallback();

	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 9;
	CurrentFunctionContext.type = SHOW_OFFSET_BY_ONE;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.value = &global.screenBrightness;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, screenBrightness);
	DisplayUpdateCurrentValue(global.screenBrightness, CurrentFunctionContext.position);
}


void Screen_contrastSelectCallback(void)
{
	LOG(SEV_TRACE,"%s", __FUNCTION__);
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 255;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.value = &global.screenContrast;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, screenContrast);
	
	DisplayUpdateCurrentValue(global.screenContrast, CurrentFunctionContext.position);
}

void Menu_PwmControllableEnterCallback(void)
{
	LOG(SEV_TRACE,"%s", __FUNCTION__);
	setIndicator(' ', 0);
	CurrentMenuPosition = MenuSystemSetupInside;
	CurrentFunctionContext.ExecuteFunction = Menu_PwmControllableExecuteFunction;
	setCursor(CurrentFunctionContext.position);
}


void Menu_Exp_P_typeSelectCallback(void)
{
	////LOG(SEV_TRACE,"%s", __FUNCTION__);
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	uint8_t pedalIndex = pgm_read_byte(&(Menu_GetCurrentMenu()->NumericParam));
	
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 2;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.text[0] = ExpPedalType_str1;
	CurrentFunctionContext.text[1] = ExpPedalType_str2;
	CurrentFunctionContext.text[2] = ExpPedalType_str3;
	CurrentFunctionContext.value = &global.expPtype[pedalIndex];//expP1type;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, expPtype[pedalIndex]);
	
	DisplayUpdateCurrentValue(global.expPtype[pedalIndex], CurrentFunctionContext.position);
}

void Menu_ExpPedalClb_SelectCallback(void)
{
	Menu_SettingsSelectCallback();
	DisplayUpdateFooter(expPedalCalibrationPosition_str);
	uint8_t pedalIndex = pgm_read_byte(&(Menu_GetCurrentMenu()->NumericParam)); 
	uint8_t currentPedalPosition = runtimeEnvironment.expPedalPos_[pedalIndex];// convertAccordingPedalType(leadToRange(pedalIndex, expGetPedalPosition(pedalIndex)), pedalIndex);
	DisplayUpdateCurrentValue(currentPedalPosition, 10);
}

//This is menu with fully custom drawing
void Menu_ExpPedalClb_EnterCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	ButtonEvent buttonEvent;
	buttonEvent.actionType_ = BUTTON_NO_EVENT;
	uint8_t pedalIndex = pgm_read_byte(&(Menu_GetCurrentMenu()->NumericParam));
	uint8_t tmpHi;
	uint8_t tmpLo;
	LCDClear();
	LCDWriteStringXY(0,0, "Set pedal toi");
	LCDWriteStringXY(0,1, "then press OK");
	
	//wait while user push OK or NO button
	while(1)
	{
		buttonEvent = getButtonLastEvent();
		if(buttonEvent.actionType_ == BUTTON_PUSH && buttonEvent.buttonNum_ == KEY_LOAD)//save adc value and go next
		{
			tmpHi = expGetPedalPositionaRaw(pedalIndex)/2;
			break;
		}
		else if(buttonEvent.actionType_ == BUTTON_PUSH && buttonEvent.buttonNum_ == KEY_SETUP)
		{
			Menu_Navigate(Menu_GetCurrentMenu());
			return;
		}
	}
	
	LCDClear();
	LCDWriteStringXY(0,0, "Set pedal heel");
	LCDWriteStringXY(0,1, "then press OK");
		
	
	while(1)
	{
		buttonEvent = getButtonLastEvent();
		if(buttonEvent.actionType_ == BUTTON_PUSH && buttonEvent.buttonNum_ == KEY_LOAD)//save adc value and go next
		{
			tmpLo = expGetPedalPositionaRaw(pedalIndex)/2;
			break;
		}
		else if(buttonEvent.actionType_ == BUTTON_PUSH && buttonEvent.buttonNum_ == KEY_SETUP)
		{
			Menu_Navigate(Menu_GetCurrentMenu());
			return;
		}
	}
	if(tmpHi > tmpLo)
	{
		//save values
		global.pHighPos[pedalIndex] = tmpHi;
		global.pLowPos[pedalIndex] = tmpLo;
		expressionProcessSetInitPositionsAndRange();
		//save to eeprom
		LOG(SEV_TRACE,"%s %d %d", __FUNCTION__, offsetof(GlobalSettings, pHighPos), sizeof(global.pHighPos));
		WriteEEPROM((uint8_t*)global.pHighPos, GlobalSettings_ADDR + offsetof(GlobalSettings, pHighPos), sizeof(global.pHighPos));
		WriteEEPROM((uint8_t*)global.pLowPos, GlobalSettings_ADDR + offsetof(GlobalSettings, pLowPos), sizeof(global.pLowPos));
		LCDClear();
		LCDWriteStringXY(0,0, "Calibration OK");
	}
	else
	{
		LCDClear();
		LCDWriteStringXY(0,0, "Calibration FAIL");		
	}
	_delay_ms(500);
		
	
	Menu_Navigate(Menu_GetCurrentMenu());//redraw current menu
	return;
	//uint8_t currentPedalPosition = convertAccordingPedalType(leadToRange(pedalIndex, expGetPedalPosition(pedalIndex)), pedalIndex);
}

void Menu_ExpPedalClb_RedrawCallback(void)
{
	uint8_t pedalIndex = pgm_read_byte(&(Menu_GetCurrentMenu()->NumericParam));
	LCDGotoXY(10,1);
	if(runtimeEnvironment.expPedalPos_[pedalIndex] < 10)
	{
		LCDWriteString("  ");
		LCDWriteInt(runtimeEnvironment.expPedalPos_[pedalIndex],1);
	}
	else if(runtimeEnvironment.expPedalPos_[pedalIndex] < 100)
	{	
		LCDWriteString(" ");
		LCDWriteInt(runtimeEnvironment.expPedalPos_[pedalIndex],2);
	}
	else
		LCDWriteInt(runtimeEnvironment.expPedalPos_[pedalIndex],3);
}

void Menu_BUT_hold_time_typeSelectCallback(void)
{	
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 14;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.text[0] = HoldTime_str1;
	CurrentFunctionContext.text[1] = HoldTime_str2;
	CurrentFunctionContext.text[2] = HoldTime_str3;
	CurrentFunctionContext.text[3] = HoldTime_str4;
	CurrentFunctionContext.text[4] = HoldTime_str5;
	CurrentFunctionContext.text[5] = HoldTime_str6;
	CurrentFunctionContext.text[6] = HoldTime_str7;
	CurrentFunctionContext.text[7] = HoldTime_str8;
	CurrentFunctionContext.text[8] = HoldTime_str9;
	CurrentFunctionContext.text[9] = HoldTime_str10;
	CurrentFunctionContext.text[10] = HoldTime_str11;
	CurrentFunctionContext.text[11] = HoldTime_str12;
	CurrentFunctionContext.text[12] = HoldTime_str13;
	CurrentFunctionContext.text[13] = HoldTime_str14;
	CurrentFunctionContext.text[14] = HoldTime_str15;
	CurrentFunctionContext.value = &global.buttonHoldTime;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, buttonHoldTime);
	
	DisplayUpdateCurrentValue(global.buttonHoldTime, CurrentFunctionContext.position);
}

void Menu_Tap_displaySelectCallback(void)
{	
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = FOOT_BUTTONS_NUM + 2;
	CurrentFunctionContext.position = 0;	
	CurrentFunctionContext.text[0] = TapDisplayType_str1;
	CurrentFunctionContext.text[1] = TapDisplayType_str2;
	CurrentFunctionContext.text[2] = TapDisplayType_str3;
	CurrentFunctionContext.text[3] = TapDisplayType_str4;
	CurrentFunctionContext.text[4] = TapDisplayType_str5;
	CurrentFunctionContext.text[5] = TapDisplayType_str6;
	CurrentFunctionContext.text[6] = TapDisplayType_str7;
	CurrentFunctionContext.text[7] = TapDisplayType_str8;
	CurrentFunctionContext.text[8] = TapDisplayType_str9;
	CurrentFunctionContext.text[9] = TapDisplayType_str10;
	CurrentFunctionContext.text[10] = TapDisplayType_str11;
	CurrentFunctionContext.text[11] = TapDisplayType_str12;
	CurrentFunctionContext.text[12] = TapDisplayType_str13;
	CurrentFunctionContext.text[13] = TapDisplayType_str14;
	CurrentFunctionContext.text[14] = TapDisplayType_str15;
	CurrentFunctionContext.value = &global.tapDisplayType;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, tapDisplayType);
	
	DisplayUpdateCurrentValue(global.tapDisplayType, CurrentFunctionContext.position);
}

void Menu_Tap_typeSelectCallback(void)
{	
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 2;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.text[0] = TapType_str1;
	CurrentFunctionContext.text[1] = TapType_str2;
	CurrentFunctionContext.text[2] = TapType_str3;
	CurrentFunctionContext.value = &global.tapType;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, tapType);
	
	DisplayUpdateCurrentValue(global.tapType, CurrentFunctionContext.position);
}

void Menu_Display_typeSelectCallback(void)
{	
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 2;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.text[0] = PedalLedView_str1;
	CurrentFunctionContext.text[1] = PedalLedView_str2;
	CurrentFunctionContext.text[2] = PedalLedView_str3;
	CurrentFunctionContext.value = &global.pedalLedView;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, pedalLedView);
	
	DisplayUpdateCurrentValue(global.pedalLedView, CurrentFunctionContext.position);
}

void Menu_pedal_brightness_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 9;
	CurrentFunctionContext.type = SHOW_OFFSET_BY_ONE;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.value = &global.pedalBrightness;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, pedalBrightness);
	
	DisplayUpdateCurrentValue(global.pedalBrightness, CurrentFunctionContext.position);
}

void Menu_Tuner_scheme_SelectCallback(void)
{	
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 1;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.text[0] = PedalTunerScheme_str1;
	CurrentFunctionContext.text[1] = PedalTunerScheme_str2;
	CurrentFunctionContext.value = &global.pedalTunerScheme;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, pedalTunerScheme);
	
	DisplayUpdateCurrentValue(global.pedalTunerScheme, CurrentFunctionContext.position);
}

void Menu_TAP_SelectCallback(void)
{	
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 128;//128 - tap off
	CurrentFunctionContext.type = CC_WITH_OFF_VAL;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.value = &bank.tapCc;
	CurrentFunctionContext.valueAddr = BankSettings_ADDR(runtimeEnvironment.activeBankNumber_) + offsetof(BankSettings, tapCc);
	
	DisplayUpdateCurrentValue(bank.tapCc, CurrentFunctionContext.position);
}

//Using same function for all 3 pedals
void Menu_ExpPedalCC_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	Menu_SettingsSelectCallback();
	
	//Get pedal number from menu parameter
	uint8_t pedalIndex = pgm_read_byte(&(Menu_GetCurrentMenu()->NumericParam));
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	if(pedalIndex == 3)//onboard pedal alternate CC number
		CurrentFunctionContext.max = 128;
	else
		CurrentFunctionContext.max = 127;
	
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.type = CC_WITH_OFF_VAL;
	CurrentFunctionContext.value = &(bank.pedalsCc[pedalIndex]);
	CurrentFunctionContext.valueAddr = BankSettings_ADDR(runtimeEnvironment.activeBankNumber_) + offsetof(BankSettings, pedalsCc[pedalIndex]);
	
	DisplayUpdateCurrentValue(bank.pedalsCc[pedalIndex], CurrentFunctionContext.position);
}

void Menu_Send_tuner_CC_SelectCallback(void)
{	
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	Menu_SettingsSelectCallback();
	
	ClearCurrentContext();

	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 128;//128  - TAP OFF
	CurrentFunctionContext.type = CC_WITH_OFF_VAL;
	CurrentFunctionContext.position = 0;
	CurrentFunctionContext.value = &bank.tunerCc;
	CurrentFunctionContext.valueAddr = BankSettings_ADDR(runtimeEnvironment.activeBankNumber_) + offsetof(BankSettings, tunerCc);
	
	DisplayUpdateCurrentValue(bank.tunerCc, CurrentFunctionContext.position);
}

void Menu_Button_EnterCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentMenuPosition = MenuButtonSetup;
	
	ButtonParent = Menu_GetCurrentMenu();
	
	Menu_Navigate(&Type);
	
	setIndicator(' ', 0);
}

void Menu_Buttons_setup_EnterCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	Menu_Navigate(MENU_CHILD);	
}

void setContextForButtonSetup()
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = BUTTON_TYPE_TOTAL_ELEMENTS-1;
	
	CurrentFunctionContext.position = 4;
	
	uint8_t i;
	for(i = 0; i < BUTTON_TYPE_TOTAL_ELEMENTS; ++i)
		CurrentFunctionContext.text[i] =(PGM_P)pgm_read_word(&(ButtonTypes[i]));

	CurrentFunctionContext.value = &bank.buttonType[CurrentButtonUnderSetupIndex];
	CurrentFunctionContext.currentValue = bank.buttonType[CurrentButtonUnderSetupIndex];
	//CurrentFunctionContext.ValueAddr = 
}

void Menu_Button1_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s %d", __FUNCTION__, pgm_read_byte(&(Menu_GetCurrentMenu()->NumericParam)));
	
	Menu_SettingsSelectCallback();
	CurrentButtonUnderSetupIndex = pgm_read_byte(&(Menu_GetCurrentMenu()->NumericParam));		
	setContextForButtonSetup();
	
	DisplayUpdateFooter(Type_change_str);
	DisplayUpdateCurrentValue(bank.buttonType[CurrentButtonUnderSetupIndex], CurrentFunctionContext.position);
	
}


void Menu_Type_change_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	ClearCurrentContext();
	
	CurrentMenuPosition = MenuButtonSetup;

	setContextForButtonSetup();

	DisplayUpdateFooter(Type_change_str);
	DisplayUpdateCurrentValue(bank.buttonType[CurrentButtonUnderSetupIndex], CurrentFunctionContext.position);
	
	setSiblingMenu(bank.buttonType[CurrentButtonUnderSetupIndex]);
	
	setIndicator(0x00, 1);
}

void Menu_Type_change_EnterCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentMenuPosition = MenuTypeChange;
	
	CurrentFunctionContext.ExecuteFunction = CommonExecuteFunction;
	CurrentFunctionContext.valueAddr = 0xFFFFFFFF;//BankSettings_ADDR(runtimeEnvironment.activeBankNumber_) + offsetof(BankSettings, buttonType[CurrentButtonUnderSetupIndex]);//0xFFFFFFFF;
	
	setSiblingMenu(bank.buttonType[CurrentButtonUnderSetupIndex]);
	
	setCursor(CurrentFunctionContext.position);
}

void cleanField(uint8_t x, uint8_t y, uint8_t lenght)
{
	LCDGotoXY(x, y);
	uint8_t i;
	for(i = 0; i < lenght; ++i)
	LCDData(0x20);
}

void setMessageContext(uint8_t index)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	ClearCurrentContext();
	
	CurrentMenuPosition = MenuButtonSetup;
	
	
	CurrentFunctionContext.text[16] = Channel_str1;
	CurrentFunctionContext.text[17] = Channel_str2;
	CurrentFunctionContext.type = SHOW_OFFSET_BY_ONE;

	ProgramIndex = index;
	
	if (global.useBankSelectMess == USE_BANK_SELECT)
	{
		uint16_t temp_value = bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.programsNumbers[index] + (bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.banksNumbers[index] * 128);
		cleanField(11, 1, 4);
		DisplayUpdateCurrentValue(temp_value, 11);
	}
	else
		DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.programsNumbers[index], 11);
	
	CurrentFunctionContext.type = GENERIC;
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.midiChannelNumbers[index], 6);
	
	setIndicator(0x00, 1);
}

void Menu_Message1_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	setSibling(NULL, NULL);
		
	DisplayUpdateFooter(Message1_str);

	setMessageContext(0);
}

void Menu_Message2_SelectCallback(void)
{	
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	DisplayUpdateFooter(Message2_str);
	
	setMessageContext(1);
}

void Menu_Message3_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	DisplayUpdateFooter(Message3_str);
	
	setMessageContext(2);
}

void Menu_Message4_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	DisplayUpdateFooter(Message4_str);
	
	setMessageContext(3);
}

void Menu_Message_EnterCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentMenuPosition = MenuMsgChange;
	
	Parent = Menu_GetCurrentMenu();
	
	Menu_Navigate(MENU_CHILD);
}

void Menu_Channel_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	ClearCurrentContext();
	
	CurrentFunctionContext.min = 1;
	CurrentFunctionContext.max = 17;
	CurrentFunctionContext.text[16] = Channel_str1;
	CurrentFunctionContext.text[17] = Channel_str2;
	CurrentFunctionContext.position = 6;
	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.midiChannelNumbers[ProgramIndex];
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.midiChannelNumbers[ProgramIndex];
	CurrentFunctionContext.ExecuteFunction = ChangeMessageFunction;
	
	setCursor(CurrentFunctionContext.position);
}

void Menu_Number_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	ClearCurrentContext();
	
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 127;	
	CurrentFunctionContext.position = 11;
	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.programsNumbers[ProgramIndex];
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.programsNumbers[ProgramIndex];
	BankNumberTemp = bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.banksNumbers[ProgramIndex];
	CurrentFunctionContext.type = PROGRAMNUMBER;
	CurrentFunctionContext.ExecuteFunction = ChangeMessageFunction;
	
	setCursor(CurrentFunctionContext.position);
}

#define  RLY1	6//TODO remove this fucking shit!!
#define  RLY2	4
#define  RLY3	2
#define  RLY4	0

uint8_t getRelayStatus(uint8_t index)
{
	////LOG(SEV_TRACE,"%s", __FUNCTION__);
	return (bank.buttonContext[CurrentButtonUnderSetupIndex].relays >> index) & 3;
}

void setRelayStatus(uint8_t index, uint8_t status)
{
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	bank.buttonContext[CurrentButtonUnderSetupIndex].relays = (bank.buttonContext[CurrentButtonUnderSetupIndex].relays & (~(0x03 << index))) | (status << index);
}

void Menu_RLY_SelectCallback(void)
{
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentMenuPosition = MenuButtonSetup;
	
	setSibling(NULL, NULL);
	
	ClearCurrentContext();
	
	DisplayUpdateFooter(RLYmsg_str);
	
	Parent = Menu_GetCurrentMenu();
	
	CurrentFunctionContext.text[0] = RLY_str1;
	CurrentFunctionContext.text[1] = RLY_str2;
	CurrentFunctionContext.text[2] = RLY_str3;
	CurrentFunctionContext.text[3] = RLY_str4;
	
	DisplayUpdateCurrentValue(getRelayStatus(RLY1), 5);
	DisplayUpdateCurrentValue(getRelayStatus(RLY2), 8);
	DisplayUpdateCurrentValue(getRelayStatus(RLY3), 11);
	DisplayUpdateCurrentValue(getRelayStatus(RLY4), 14);
	
	setIndicator(0x00, 1);
}

void Menu_RLY_EnterCallback(void)
{
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentMenuPosition = MenuRLYChange;
	
	Menu_Navigate(MENU_CHILD);
}

void setRLYContext(uint8_t RLY_Number)
{
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 3;
	CurrentFunctionContext.text[0] = RLY_str1;
	CurrentFunctionContext.text[1] = RLY_str2;
	CurrentFunctionContext.text[2] = RLY_str3;
	CurrentFunctionContext.text[3] = RLY_str4;
	
	CurrentFunctionContext.currentValue = getRelayStatus(RLY_Number);
	CurrentFunctionContext.ExecuteFunction = ChangeRLYFunction;
	
	ProgramIndex = RLY_Number;
	
	setCursor(CurrentFunctionContext.position);
}

void Menu_RLY1_SelectCallback(void)
{	
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentFunctionContext.position = 5;
	
	setRLYContext(RLY1);
}

void Menu_RLY2_SelectCallback(void)
{	
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentFunctionContext.position = 8;
	
	setRLYContext(RLY2);
}

void Menu_RLY3_SelectCallback(void)
{
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentFunctionContext.position = 11;
	
	setRLYContext(RLY3);
}

void Menu_RLY4_SelectCallback(void)
{
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentFunctionContext.position = 14;
	
	setRLYContext(RLY4);
}

uint8_t get_iaState(uint8_t index)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	//return (bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.iaState >> (iaState_COUNT - index)) & 1;
	return (bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.iaState >> index) & 1;
}

void set_iaState(uint8_t index, uint8_t status)
{		
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	if (status)
		//bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.iaState |= (1 << (iaState_COUNT - index));//TODO reverse bits for this
		bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.iaState |= (1 << index);
	else
		//bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.iaState &= ~(1 << (iaState_COUNT - index));
		bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.iaState &= ~(1 << index);
}

void Menu_iaState_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	ClearCurrentContext();
	
	setSibling(NULL, NULL);
	
	CurrentMenuPosition = MenuButtonSetup;
	
	DisplayUpdateFooter(Clear_str);
	
	setIndicator(0x00, 1);
	
	uint8_t count;
	
	for (count = 0; count < FOOT_BUTTONS_NUM; count++)
	{
		DisplayUpdateBitStringValue(get_iaState(count), count);
	}
}

void Menu_iaState_EnterCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentMenuPosition = MenuiaStateChange;
	
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.iaState;
	CurrentFunctionContext.ExecuteFunction = ChangeiaStateFunction;
	
	ProgramIndex = 0;
	
	UpdateiaStateIndexNumber(ProgramIndex);
}

void Menu_ccNumber_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	setSibling(NULL, NULL);
	
	ClearCurrentContext();
	
	DisplayUpdateFooter(ccNumber_str);
	CurrentFunctionContext.type = CC_WITH_OFF_VAL;	
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber, 10);
	
	CurrentMenuPosition = MenuButtonSetup;
	setIndicator(0x00, 1);
}

void Menu_ccNumber_EnterCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = CC_OFF_VALUE;
	CurrentFunctionContext.type = CC_WITH_OFF_VAL;
	CurrentFunctionContext.position = 10;
	
	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber;
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber;
	CurrentFunctionContext.valueAddr = 0xFFFFFFFF;//BankSettings_ADDR(runtimeEnvironment.activeBankNumber_) + offsetof(BankSettings, buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber); //0xFFFFFFFF;
	
	CurrentFunctionContext.ExecuteFunction = CommonExecuteFunction;
	
	CurrentMenuPosition = MenuCommonChange;
	
	setCursor(CurrentFunctionContext.position);
}

void Menu_ccFreezeNumber_SelectCallback(void)
{
	setSibling(NULL, NULL);
	
	ClearCurrentContext();
	
	DisplayUpdateFooter(ccFreezeNumber_str);
	CurrentFunctionContext.type = CC_WITH_OFF_VAL;
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlMsbFreezeNumber, 10);
	
	CurrentMenuPosition = MenuButtonSetup;
	setIndicator(0x00, 1);
}

void Menu_ccFreezeNumber_EnterCallback(void)
{
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = CC_OFF_VALUE;
	CurrentFunctionContext.type = CC_WITH_OFF_VAL;
	CurrentFunctionContext.position = 10;
	
	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlMsbFreezeNumber;
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlMsbFreezeNumber;
	CurrentFunctionContext.valueAddr = 0xFFFFFFFF;
	CurrentFunctionContext.ExecuteFunction = CommonExecuteFunction;
	
	CurrentMenuPosition = MenuCommonChange;
	
	setCursor(CurrentFunctionContext.position);
}

void Menu_autoSendState_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	setSibling(NULL, NULL);
	
	ClearCurrentContext();
	
	DisplayUpdateFooter(autoSendState_str);
	
	CurrentFunctionContext.text[0] = OFF_str;
	CurrentFunctionContext.text[1] = ON_str;
	
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.autoSendState, 11);
	
	CurrentMenuPosition = MenuButtonSetup;
	
	setIndicator(0x00, 1);
}

void Menu_autoSendState_EnterCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 1;
	CurrentFunctionContext.position = 11;
	
	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.autoSendState;
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.autoSendState;
	CurrentFunctionContext.valueAddr = 0xFFFFFFFF;
	
	CurrentFunctionContext.ExecuteFunction = CommonExecuteFunction;
	
	CurrentMenuPosition = MenuCommonChange;
	
	setCursor(CurrentFunctionContext.position);
}

void Menu_ccValue_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	setSibling(NULL, NULL);
	
	ClearCurrentContext();
	
	DisplayUpdateFooter(ccValue_str);
	
	CurrentMenuPosition = MenuButtonSetup;
	
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue, 3);
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramLsbOffValue, 12);
	
	setIndicator(0x00, 1);
}

void set_ChangeMessageContext(uint8_t position)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	ClearCurrentContext();
	
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 127;
	CurrentFunctionContext.position = position;
	CurrentFunctionContext.ExecuteFunction = ChangeMessageFunction;
	
	setCursor(CurrentFunctionContext.position);
}

void Menu_ccOnValue_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	set_ChangeMessageContext(3);

	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue;
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue;	
}

void Menu_ccOffValue_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	set_ChangeMessageContext(12);
	
	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramLsbOffValue;
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramLsbOffValue;
}

void Menu_ctrlNumber_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	
	setSibling(NULL, NULL);
	
	ClearCurrentContext();
	
	DisplayUpdateFooter(ctrlNumber_str);
	
	CurrentMenuPosition = MenuButtonSetup;
	
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlMsbFreezeNumber, 4);
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber, 12);
	
	setIndicator(0x00, 1);
}

void Menu_ctrlMsbNumber_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	
	set_ChangeMessageContext(4);
	
	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlMsbFreezeNumber;
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlMsbFreezeNumber;
	CurrentFunctionContext.valueAddr = 0xFFFFFFFF;//BankSettings_ADDR(runtimeEnvironment.activeBankNumber_) + offsetof(BankSettings, buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlMsbNumber); //0xFFFFFFFF;

	
	//setIndicator(0x00, 1);
}

void Menu_ctrlLsbNumber_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	
	set_ChangeMessageContext(12);

	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber;
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber;
	CurrentFunctionContext.valueAddr = 0xFFFFFFFF;//BankSettings_ADDR(runtimeEnvironment.activeBankNumber_) + offsetof(BankSettings, buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.ctrlLsbNumber); //0xFFFFFFFF;
	
	//setIndicator(0x00, 1);
}

void set_paramValue(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	setSibling(NULL, NULL);
	
	ClearCurrentContext();
	
	DisplayUpdateFooter(paramValue_str);
	
	CurrentMenuPosition = MenuButtonSetup;
}

void Menu_paramOnValue_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	
	set_paramValue();
	
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramMsbOnValue, 3);
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue, 11);
	
	//Arrow UP
	LCDGotoXY(14, 1);
	LCDData(2);
		
	setIndicator(0x00, 1);
}

void Menu_paramMsbOnValue_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	
	set_ChangeMessageContext(3);
	
	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramMsbOnValue;
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramMsbOnValue;
	CurrentFunctionContext.valueAddr = 0xFFFFFFFF;//BankSettings_ADDR(runtimeEnvironment.activeBankNumber_) + offsetof(BankSettings, buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramMsbOnValue); //0xFFFFFFFF;
}

void Menu_paramLsbOnValue_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	
	set_ChangeMessageContext(11);

	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue;
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue;
	CurrentFunctionContext.valueAddr = 0xFFFFFFFF;//BankSettings_ADDR(runtimeEnvironment.activeBankNumber_) + offsetof(BankSettings, buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramLsbOnValue); //0xFFFFFFFF;
}

void Menu_paramOffValue_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	
	set_paramValue();
	
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramMsbOffValue, 3);
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramLsbOffValue, 11);
	
	//Arrow down
	LCDGotoXY(14, 1);
	LCDData(3);
	
	setIndicator(0x00, 1);
}

void Menu_paramMsbOffValue_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	
	set_ChangeMessageContext(3);
	
	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramMsbOffValue;
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramMsbOffValue;
	CurrentFunctionContext.valueAddr = 0xFFFFFFFF;//BankSettings_ADDR(runtimeEnvironment.activeBankNumber_) + offsetof(BankSettings, buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramMsbOffValue); //0xFFFFFFFF;
}

void Menu_paramLsbOffValue_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	
	set_ChangeMessageContext(11);
	
	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramLsbOffValue;
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramLsbOffValue;
	CurrentFunctionContext.valueAddr = 0xFFFFFFFF;//BankSettings_ADDR(runtimeEnvironment.activeBankNumber_) + offsetof(BankSettings, buttonContext[CurrentButtonUnderSetupIndex].commonContext.contolAndNrpnChangeContext_.paramLsbOffValue); //0xFFFFFFFF;
}

void Menu_noteNumber_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	setSibling(NULL, NULL);
	
	ClearCurrentContext();
	
	DisplayUpdateFooter(noteNumber_str);
	
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.noteOnConstContext_.noteNumber, 12);
	
	CurrentMenuPosition = MenuButtonSetup;
}

void set_CommonContext(uint8_t position)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = 127;
	CurrentFunctionContext.valueAddr = 0xFFFFFFFF;
	CurrentFunctionContext.position = position;
	CurrentFunctionContext.ExecuteFunction = CommonExecuteFunction;
	
	CurrentMenuPosition = MenuCommonChange;
	
	setCursor(CurrentFunctionContext.position);
}

void Menu_noteNumber_EnterCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	set_CommonContext(12);
	
	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.noteOnConstContext_.noteNumber;
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.noteOnConstContext_.noteNumber;
}

void Menu_noteVelocity_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	setSibling(NULL, NULL);
	
	ClearCurrentContext();
	
	DisplayUpdateFooter(Velocity_str);
	
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.noteOnConstContext_.noteVelocity, 10);
	
	CurrentMenuPosition = MenuButtonSetup;
}

void Menu_noteVelocity_EnterCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	set_CommonContext(10);
	
	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.noteOnConstContext_.noteVelocity;
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].commonContext.noteOnConstContext_.noteVelocity;
}

void Menu_bankNumber_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	setSibling(NULL, NULL);
	
	ClearCurrentContext();
	
	DisplayUpdateFooter(bankNumber_str);
	CurrentFunctionContext.type = SHOW_OFFSET_BY_ONE;
	DisplayUpdateCurrentValue(bank.buttonContext[CurrentButtonUnderSetupIndex].bankNumber, 5);
	
	CurrentMenuPosition = MenuButtonSetup;
}

void Menu_bankNumber_EnterCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	set_CommonContext(5);
	CurrentFunctionContext.type = SHOW_OFFSET_BY_ONE;
	CurrentFunctionContext.value = &bank.buttonContext[CurrentButtonUnderSetupIndex].bankNumber;
	CurrentFunctionContext.currentValue = bank.buttonContext[CurrentButtonUnderSetupIndex].bankNumber;
}

void SelectBankRoutine(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	ClearCurrentContext();
	
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = BanksCount - 1;
	CurrentFunctionContext.position = 0;
	
	CurrentFunctionContext.value = &global.bnkNum;
	CurrentFunctionContext.valueAddr = GlobalSettings_ADDR + offsetof(GlobalSettings, bnkNum);
	
	CurrentMenuPosition = MenuSystemSetupInside;
}

void Menu_SelectSaveBank_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	SelectBankRoutine();
	
	DisplayUpdateCurrentValue(global.bnkNum + 1, CurrentFunctionContext.position);
	
	setIndicator(' ', 0);
	
	CurrentFunctionContext.ExecuteFunction = BankSaveExecuteFunction;
	
	setCursor(CurrentFunctionContext.position);
}

void Menu_LoadBank_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	SelectBankRoutine();
	
	LCDWriteIntXY(11, 0, global.bnkNum + 1, 3);
	LCDWriteStringXY(0, 1, bank.BankName);
	
	setIndicator(' ', 0);
	
	CurrentFunctionContext.ExecuteFunction = BankLoadExecuteFunction;
}

void Menu_BankName_SelectCallback(void)
{	
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	ClearCurrentContext();
	
	CurrentFunctionContext.position = 0;
	
	LCDWriteStringXY(CurrentFunctionContext.position, 1, bank.BankName);	
	
	setIndicator(' ', 0);
	
	CurrentMenuPosition = MenuSystemSetupInside;
	
	CurrentFunctionContext.ExecuteFunction = BankRenameExecuteFunction;
	
	setCursor(CurrentFunctionContext.position);
}

void Menu_CopyBank_SelectCallback(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	DisplayUpdateFooter(CopyBankSrcDst_str);
	
	ClearCurrentContext();
	
	CurrentFunctionContext.min = 0;
	CurrentFunctionContext.max = BanksCount - 1;
	CurrentFunctionContext.currentValue = 0;
	
	CurrentBankIndex = global.bnkNum;
	
	setIndicator(' ', 0);
	
	CurrentMenuPosition = MenuCopyBank;
	
	DisplayUpdateCurrentValue(global.bnkNum + 1, 4);
	DisplayUpdateCurrentValue(1, 12);
	
	Menu_Navigate(MENU_CHILD);
}

void Menu_SrcBank_SelectCallback(void)
{	
	CurrentFunctionContext.position = 4;
	CurrentFunctionContext.ExecuteFunction = BankCopyExecuteFunction;
	CurrentFunctionContext.value = &CurrentBankIndex;
	
	setCursor(CurrentFunctionContext.position);
}

void Menu_DstBank_SelectCallback(void)
{
	CurrentFunctionContext.position = 12;
	CurrentFunctionContext.value = &CurrentFunctionContext.currentValue;
	CurrentFunctionContext.ExecuteFunction = BankCopyExecuteFunction;
	
	setCursor(CurrentFunctionContext.position);
}

/*Обновление текущего значения на дисплее.
  Устанавливает на дисплее текущее значение (uint16 или строковая константа, 
  если таковая установлена)*/
void DisplayUpdateCurrentValue(uint16_t value, uint8_t position)
{	
	//LOG(SEV_TRACE,"%s value %d position %d max %d, min %d", __FUNCTION__, value, position, CurrentFunctionContext.max, CurrentFunctionContext.min);	
	char text[17];
	
	if (/*!!! TODO what is this?? */value < TEXTVALUESIZE/*!!!*/ && CurrentFunctionContext.text[value] != NULL)
	{
		LCDWriteStringXY(position, 1, strcpy_P(text, CurrentFunctionContext.text[value]));	
	} 
	else
	{
		uint8_t field_length;
		uint16_t valueToShow = CurrentFunctionContext.type == SHOW_OFFSET_BY_ONE ? (value + 1) : value;
		if (valueToShow < 100)
		{
			field_length = 2;
		}
		else if (valueToShow < 1000)
		{
			field_length = 3;
		}
		else
		{
			field_length = 4;
		}
		
		uint8_t i;
		//LOG(SEV_TRACE,"field_length %d", field_length);
		LCDGotoXY(position, 1);
		for(i = 0; i < field_length + 1; ++i)
			LCDData(' ');
		
		LCDGotoXY(position, 1);
		if(CurrentFunctionContext.type == CC_WITH_OFF_VAL && value == 128)
			LCDWriteString("NO");
		else
			LCDWriteInt((int)(valueToShow), field_length);
	}
}

void DisplayUpdateBitStringValue(uint16_t value, uint8_t position)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	LCDGotoXY(position, 1);
	LCDWriteInt((int)(value), 1);
}

void ClearCurrentContext(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	memset(&CurrentFunctionContext, 0, sizeof(FunctionContext));
}

void GotoParentMenuItem(Menu_Item_t *parent)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	LcdHideCursor();
	Menu_Navigate(parent);
}

void ExitCurrentExecuteFunction(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	ClearCurrentContext();
	
	GotoParentMenuItem(Menu_GetCurrentMenu());
}

void UpdateiaStateIndexNumber(uint8_t number)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	DisplayUpdateCurrentValue(number + 1/*, 1, 13*/, 13);//TODO changes magic numbers to a named constants
	setCursor(number);
}

/*Обработка изменения значения параметров для пунктов меню iaState*/
void ChangeiaStateFunction(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	uint8_t value = get_iaState(ProgramIndex);
	
	if (CurrentFunctionContext.action == ValueIncrement)
	{
		if ((value++) == 1)
		{
			value = 0;
		}
	}
	else if (CurrentFunctionContext.action == ValueDecrement)
	{
		if ((value--) == 0)
		{
			value = 1;
		}
	}
	else if (CurrentFunctionContext.action == ValueNext)
	{
		if ((ProgramIndex++) == iaState_COUNT)
		{
			ProgramIndex = 0;
		}
		
		UpdateiaStateIndexNumber(ProgramIndex);
		
		return;
	}
	else if (CurrentFunctionContext.action == ValuePrevious)
	{
		if ((ProgramIndex--) == 0)
		{
			ProgramIndex = iaState_COUNT;
		}
		
		UpdateiaStateIndexNumber(ProgramIndex);
		
		return;
	}
	else if (CurrentFunctionContext.action == ValueSaveAndExit)
	{
		GotoParentMenuItem(Menu_GetCurrentMenu());
		
		return;
	}
	else if (CurrentFunctionContext.action == ValueRestoreAndExit)
	{
		bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.iaState = CurrentFunctionContext.currentValue;
		GotoParentMenuItem(Menu_GetCurrentMenu());
		
		return;
	}
	else
		return;
		
	DisplayUpdateBitStringValue(value, ProgramIndex);
	set_iaState(ProgramIndex, value);
	setCursor(ProgramIndex);
}

/*Обработка изменения значения параметров для пунтков меню RLY*/
void ChangeRLYFunction(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	uint8_t value = getRelayStatus(ProgramIndex);

	if (CurrentFunctionContext.action == ValueIncrement)
	{
		if ((value++) == CurrentFunctionContext.max)
		{		
			value = CurrentFunctionContext.min;
		}
	}
	else if (CurrentFunctionContext.action == ValueDecrement)
	{
		if ((value--) == CurrentFunctionContext.min)
		{		
			value = CurrentFunctionContext.max;
		}
	}
	else if (CurrentFunctionContext.action == ValueNext)
	{
		Menu_Navigate(MENU_NEXT);
		
		return;
	}
	else if (CurrentFunctionContext.action == ValuePrevious)
	{
		Menu_Navigate(MENU_PREVIOUS);
		
		return;
	}
	else if (CurrentFunctionContext.action == ValueSaveAndExit)
	{	
		GotoParentMenuItem(Parent);
		
		return;
	}
	else if (CurrentFunctionContext.action == ValueRestoreAndExit)
	{
		setRelayStatus(ProgramIndex, CurrentFunctionContext.currentValue);
		GotoParentMenuItem(Parent);
		
		return;
	}
	else
		return;
	
	DisplayUpdateCurrentValue(value, CurrentFunctionContext.position);
	
	setRelayStatus(ProgramIndex, value);
		
	setCursor(CurrentFunctionContext.position);
}

/*Обработка изменения значения параметров для пунтков меню Message1*/
void ChangeMessageFunction(void)
{
	uint8_t *value = CurrentFunctionContext.value;
	uint16_t temp_value = 0;
	LOG(SEV_TRACE,"%s value %d Action %d", __FUNCTION__, *value, CurrentFunctionContext.action);	
	if (CurrentFunctionContext.action == ValueIncrement)
	{
		if (((*value)++) >= CurrentFunctionContext.max)
		{
			if (CurrentFunctionContext.type == PROGRAMNUMBER &&	global.useBankSelectMess == USE_BANK_SELECT)
			{
				if ((bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.banksNumbers[ProgramIndex]++) == MAX_BANK_SELECT_MESSAGE_VALUE)
					bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.banksNumbers[ProgramIndex] = 0;
			}
			*value = CurrentFunctionContext.min;
		}
	}
	else if (CurrentFunctionContext.action == ValueDecrement)
	{
		if (((*value)--) == CurrentFunctionContext.min)
		{
			if (CurrentFunctionContext.type == PROGRAMNUMBER &&	global.useBankSelectMess == USE_BANK_SELECT)
			{
				if ((bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.banksNumbers[ProgramIndex]--) == 0)
					bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.banksNumbers[ProgramIndex] = MAX_BANK_SELECT_MESSAGE_VALUE;
			}
			
			*value = CurrentFunctionContext.max;
		}
	}
	else if (CurrentFunctionContext.action == ValueNext)
	{
		Menu_Navigate(MENU_NEXT);
		
		return;
	}
	else if (CurrentFunctionContext.action == ValuePrevious)
	{
		Menu_Navigate(MENU_PREVIOUS);
		
		return;
	}
	else if (CurrentFunctionContext.action == ValueSaveAndExit)
	{
		GotoParentMenuItem(Parent);
		
		return;
	}
	else if (CurrentFunctionContext.action == ValueRestoreAndExit)
	{
		*value = CurrentFunctionContext.currentValue;
		
		if (CurrentFunctionContext.type == PROGRAMNUMBER &&
		global.useBankSelectMess == USE_BANK_SELECT)
		{
			bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.banksNumbers[ProgramIndex] = BankNumberTemp;
		}
		
		GotoParentMenuItem(Parent);
		
		return;
	}
	else
		return;
	
	//LOG(SEV_TRACE,"%s Type %d useBankSelectMess %d", __FUNCTION__, CurrentFunctionContext.Type, global.useBankSelectMess);
	if (CurrentFunctionContext.type == PROGRAMNUMBER)
	{		
		if(global.useBankSelectMess == USE_BANK_SELECT)
			temp_value = *value + (bank.buttonContext[CurrentButtonUnderSetupIndex].presetChangeContext.banksNumbers[ProgramIndex] * 128);
		else
			temp_value = *value;
			
		LOG(SEV_TRACE,"%s temp_value %d", __FUNCTION__, temp_value);
		cleanField(CurrentFunctionContext.position, 1, 4);
		DisplayUpdateCurrentValue(temp_value+1, CurrentFunctionContext.position);
	}
	else
		DisplayUpdateCurrentValue(*value, CurrentFunctionContext.position);
		
	setCursor(CurrentFunctionContext.position);
}

/*Change preset number form main screen*/
void MainScreenExecuteFunction(void)
{
	LOG(SEV_TRACE,"%s", __FUNCTION__);	
	uint8_t *value = CurrentFunctionContext.value;
	
	if (CurrentFunctionContext.action == ValueIncrement)
	{
		if (((*value)++) == CurrentFunctionContext.max)
			*value = CurrentFunctionContext.min;
			
		bank.buttonContext[runtimeEnvironment.activePresetButtonNumber_].presetChangeContext.programsNumbers[0] = *value;
		midiSendProgramChange(*value, bank.buttonContext[runtimeEnvironment.activePresetButtonNumber_].presetChangeContext.midiChannelNumbers[0]);
	} 
	else if (CurrentFunctionContext.action == ValueDecrement)
	{
		if (((*value)--) == CurrentFunctionContext.min)
			*value = CurrentFunctionContext.max;
			
		bank.buttonContext[runtimeEnvironment.activePresetButtonNumber_].presetChangeContext.programsNumbers[0] = *value;
		midiSendProgramChange(*value, bank.buttonContext[runtimeEnvironment.activePresetButtonNumber_].presetChangeContext.midiChannelNumbers[0]);
	}
	else if (CurrentFunctionContext.action == ValueSaveAndExit)
	{
		ExitCurrentExecuteFunction();
		return;
	}
	else if (CurrentFunctionContext.action == ValueRestoreAndExit)
	{
		ExitCurrentExecuteFunction();
		return;
	}
	else
		return;
	
	DisplayUpdateCurrentValue(*value + 1, CurrentFunctionContext.position);
	
	setCursor(CurrentFunctionContext.position);
}

/*Обработка изменения значения параметров для обычных данных, таких как System Setup*/
void CommonExecuteFunction(void)
{
	LOG(SEV_TRACE,"%s", __FUNCTION__);	
	uint8_t *value = CurrentFunctionContext.value;
	
	if (CurrentFunctionContext.action == ValueIncrement)
	{
		if (((*value)++) >= CurrentFunctionContext.max)
			*value = CurrentFunctionContext.min;
	} 
	else if (CurrentFunctionContext.action == ValueDecrement)
	{
		if (((*value)--) == CurrentFunctionContext.min)
			*value = CurrentFunctionContext.max;
	}
	else if (CurrentFunctionContext.action == ValueSaveAndExit)
	{
		/* При записи в EEPROM проверяется адрес. Если адрес 0xFFFFFFFF, то запись не
		   производится*/
		WriteEEPROM(value, CurrentFunctionContext.valueAddr, sizeof(uint8_t));
		ExitCurrentExecuteFunction();
		
		return;
	}
	else if (CurrentFunctionContext.action == ValueRestoreAndExit)
	{
		if(CurrentFunctionContext.valueAddr != 0xFFFFFFFF)//restore from EEPROM
			ReadEEPROM(value, CurrentFunctionContext.valueAddr, sizeof(uint8_t));
		else//restore from function context
			*value = CurrentFunctionContext.prevValue;
		
		ExitCurrentExecuteFunction();
		
		return;
	}
	else
		return;
	
	DisplayUpdateCurrentValue(*value, CurrentFunctionContext.position);
	
	setCursor(CurrentFunctionContext.position);
}

/* Обработчик сохранения/загрузки банков*/
void BankSaveExecuteFunction(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	uint8_t *value = CurrentFunctionContext.value;
	
	if (CurrentFunctionContext.action == ValueIncrement)
	{
		if (((*value)++) == CurrentFunctionContext.max)
			*value = CurrentFunctionContext.min;
	}
	else if (CurrentFunctionContext.action == ValueDecrement)
	{
		if (((*value)--) == CurrentFunctionContext.min)
			*value = CurrentFunctionContext.max;
	}
	else if (CurrentFunctionContext.action == ValueSaveAndExit)
	{
		SaveBank(*value, &bank);
		
		ReadEEPROM(value, CurrentFunctionContext.valueAddr, sizeof(uint8_t));
		GotoParentMenuItem(MENU_PARENT);
		
		return;
	}
	else if (CurrentFunctionContext.action == ValueRestoreAndExit)
	{
		ReadEEPROM(value, CurrentFunctionContext.valueAddr, sizeof(uint8_t));
		GotoParentMenuItem(MENU_PARENT);
		
		return;
	}
	else
		return;
	
	DisplayUpdateCurrentValue(*value + 1, CurrentFunctionContext.position);
	
	setCursor(CurrentFunctionContext.position);
}

/* Обработчик сохранения/загрузки банков*/
void BankLoadExecuteFunction(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	BankSettings TempBank;
	
	uint8_t *value = CurrentFunctionContext.value;
	
	if (CurrentFunctionContext.action == ValueIncrement)
	{
		if (((*value)++) == CurrentFunctionContext.max)
		*value = CurrentFunctionContext.min;
	}
	else if (CurrentFunctionContext.action == ValueDecrement)
	{
		if (((*value)--) == CurrentFunctionContext.min)
		*value = CurrentFunctionContext.max;
	}
	else if (CurrentFunctionContext.action == ValueSaveAndExit)
	{
		LoadBank(*value, &bank);
		
		WriteEEPROM(value, CurrentFunctionContext.valueAddr, sizeof(uint8_t));
		GotoParentMenuItem(MENU_PARENT);
		
		return;
	}
	else if (CurrentFunctionContext.action == ValueRestoreAndExit)
	{
		ReadEEPROM(value, CurrentFunctionContext.valueAddr, sizeof(uint8_t));
		GotoParentMenuItem(MENU_PARENT);
		
		return;
	}
	else
		return;
	
	LCDWriteIntXY(11, 0, *value + 1, 3);
	
	ReadEEPROM((uint8_t*)&TempBank, BankSettings_ADDR(*value), sizeof(BankSettings));
	
	DisplayUpdateFooter(Clear_str);
	
	LCDWriteStringXY(0, 1, TempBank.BankName);
}

#define First_symbol          '0'
#define Last_symbol           'z'

/* Обработчик переименования банка*/
void BankRenameExecuteFunction(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	if (CurrentFunctionContext.action == ValueIncrement)
	{
		/*Если достигнута верхняя граница таблицы  символов, добавляем в таблицу символ пробела.
		  Если все символы исчерпаны, они начинаются заново, иначе берем следующий*/
		if (bank.BankName[CurrentFunctionContext.position] == Last_symbol)
			bank.BankName[CurrentFunctionContext.position] = ' ';
		else if (bank.BankName[CurrentFunctionContext.position] == ' ')
			bank.BankName[CurrentFunctionContext.position] = First_symbol;
		else
			bank.BankName[CurrentFunctionContext.position]++; 

	}
	else if (CurrentFunctionContext.action == ValueDecrement)
	{
		/*Если достигнута нижняя граница таблицы  символов, добавляем в таблицу символ пробела.
		  Если все символы исчерпаны, они начинаются заново, иначе берем следующий*/
		if (bank.BankName[CurrentFunctionContext.position] == First_symbol)
			bank.BankName[CurrentFunctionContext.position] = ' ';
		else if (bank.BankName[CurrentFunctionContext.position] == ' ')
			bank.BankName[CurrentFunctionContext.position] = Last_symbol;
		else
			bank.BankName[CurrentFunctionContext.position]--; 
	}
	else if (CurrentFunctionContext.action == ValueNext)
	{
		if (CurrentFunctionContext.position++ == (BANK_NAME_NMAX_SIZE-2))
			CurrentFunctionContext.position = 0;
		
		setCursor(CurrentFunctionContext.position);
		
		return;
	}
	else if (CurrentFunctionContext.action == ValuePrevious)
	{
		if (CurrentFunctionContext.position-- == 0)
			CurrentFunctionContext.position = BANK_NAME_NMAX_SIZE-2;
		
		setCursor(CurrentFunctionContext.position);
				
		return;
	}
	else if (CurrentFunctionContext.action == ValueSaveAndExit)
	{	
		WriteEEPROM((uint8_t*)&bank, BankSettings_ADDR(global.bnkNum), sizeof(BankSettings));
		GotoParentMenuItem(MENU_PARENT);
			
		return;
	}
	else if (CurrentFunctionContext.action == ValueRestoreAndExit)
	{
		ReadEEPROM((uint8_t*)&bank, BankSettings_ADDR(global.bnkNum), sizeof(BankSettings));
		GotoParentMenuItem(MENU_PARENT);
		
		return;
	}
	else
		return;
		
	/* Если мы вылетели за пределы символьной таблицы, возвращаемся в нее.*/
	if ((bank.BankName[CurrentFunctionContext.position] > Last_symbol) || 
		(bank.BankName[CurrentFunctionContext.position] < First_symbol))
		bank.BankName[CurrentFunctionContext.position] = ' ';
		
	LCDData(bank.BankName[CurrentFunctionContext.position]);
	LCDGotoXY(CurrentFunctionContext.position, 1);
}

void BankCopyExecuteFunction(void)
{
	//LOG(SEV_TRACE,"%s", __FUNCTION__);	
	BankSettings TempBank;
	
	uint8_t *value = CurrentFunctionContext.value;

	if (CurrentFunctionContext.action == ValueIncrement)
	{
		if (((*value)++) == CurrentFunctionContext.max)
			*value = CurrentFunctionContext.min;
	}
	else if (CurrentFunctionContext.action == ValueDecrement)
	{
		if (((*value)--) == CurrentFunctionContext.min)
			*value = CurrentFunctionContext.max;
	}
	else if (CurrentFunctionContext.action == ValueNext)
	{
		Menu_Navigate(MENU_NEXT);
		
		return;
	}
	else if (CurrentFunctionContext.action == ValuePrevious)
	{
		Menu_Navigate(MENU_PREVIOUS);
		
		return;
	}
	else if (CurrentFunctionContext.action == ValueSaveAndExit)
	{
		ReadEEPROM((uint8_t*)&TempBank, BankSettings_ADDR(CurrentBankIndex), sizeof(BankSettings));
		WriteEEPROM((uint8_t*)&TempBank, BankSettings_ADDR(CurrentFunctionContext.currentValue), sizeof(BankSettings));
		
		GotoParentMenuItem(MENU_PARENT);
		
		return;
	}
	else if (CurrentFunctionContext.action == ValueRestoreAndExit)
	{
		GotoParentMenuItem(MENU_PARENT);
		
		return;
	}
	else
		return;

	DisplayUpdateCurrentValue((*value) + 1, CurrentFunctionContext.position);
	
	setCursor(CurrentFunctionContext.position);
}

/*Обработка нажатий на клавиши навигации*/
void Menu_NavigationProcessButtonEvent(ButtonEvent *ButtonEvent)
{
	////LOG(SEV_TRACE,"%s", __FUNCTION__);	
	CurrentFunctionContext.action = NoAction;
	
	switch(ButtonEvent->buttonNum_)
	{
		case KEY_INC:
		if (CurrentMenuPosition == MenuSettings		||
			CurrentMenuPosition == MenuSystemSetup	||
			CurrentMenuPosition == MenuButtonSetup)
		{
			if (NextTypeSibling)
			{
				Menu_Navigate(NextTypeSibling);
			}
			else
			{
				Menu_Navigate(MENU_NEXT);
			}
		}
		else
		{
			CurrentFunctionContext.action = ValueNext;
		}
		
		break;
		
		
		case KEY_DEC:
			if (CurrentMenuPosition == MenuSettings		||
				CurrentMenuPosition == MenuSystemSetup	||
				CurrentMenuPosition == MenuButtonSetup)
			{
				if (PrevTypeSibling != NULL)
				Menu_Navigate(PrevTypeSibling);
				else
				Menu_Navigate(MENU_PREVIOUS);
			}
			else
			{
				CurrentFunctionContext.action = ValuePrevious;
			}
			break;
		
		
		case KEY_UP:
			CurrentFunctionContext.action = ValueIncrement;
			break;
		
		case KEY_DOWN:
			CurrentFunctionContext.action = ValueDecrement;
			break;
		
		
		case KEY_LOAD:
			if (CurrentMenuPosition == MenuSettings		||
				CurrentMenuPosition == MenuSystemSetup	||
				CurrentMenuPosition == MenuButtonSetup)
			{
				Menu_EnterCurrentItem();
			}
			else if (CurrentMenuPosition == MenuMain)
			{
				Menu_Navigate(&KeyLoadBank);
			
				break;
			}
			else
			{
				CurrentFunctionContext.action = ValueSaveAndExit;
			}
		
		break;
		
		
		case KEY_SETUP:
			if(CurrentMenuPosition == MenuMain)
			{
				Menu_Navigate(&Banks);
			
				break;
			}
		
			if (CurrentMenuPosition == MenuSettings		||
				CurrentMenuPosition == MenuSystemSetup)
			{
				Menu_Navigate(MENU_PARENT);
			}
			else if (CurrentMenuPosition == MenuButtonSetup)
			{
				Menu_Navigate(ButtonParent);
			}
			else
			{
				CurrentFunctionContext.action = ValueRestoreAndExit;
			}
		
			break;
	}
	
	if (CurrentFunctionContext.ExecuteFunction)
	{
		CurrentFunctionContext.ExecuteFunction();
	}
}
// peters
#include "dump.h"
void Menu_SystemDump(void){
	LCDClear();
	LCDWriteStringXY(0,0,"DUMPING...");
	//_delay_ms( 2000 );
	save_All();
	Menu_Navigate(MENU_CHILD);

}
