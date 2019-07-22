/*
 * CallbackFunctions.h
 *
 * Created: 15.03.2017 20:27:03
 *  Author: 123
 */ 


#ifndef CALLBACKFUNCTIONS_H_
#define CALLBACKFUNCTIONS_H_

#include "button.h"
#include "settings.h"
#include "MicroMenu.h"


#include <stdint.h>

#include <avr/pgmspace.h>

#define TEXTVALUESIZE	18

typedef enum
{
	GENERIC = 0,
	PROGRAMNUMBER,
	SHOW_OFFSET_BY_ONE,//this value should be displayed as val+1
	CC_WITH_OFF_VAL//show 0-127 as number and 128 as "NO"
} ValueType;

typedef enum
{
	NoAction = 0,
	ValueIncrement,
	ValueDecrement,
	ValueNext,
	ValuePrevious,
	ValueSaveAndExit,
	ValueRestoreAndExit
} ActionType;

/*Контекст выполнения текущей функции*/
typedef struct context
{
	void *value;
	uint16_t currentValue;
	uint16_t prevValue;
	uint32_t valueAddr;
	ValueType type;
	PGM_P text[TEXTVALUESIZE];
	uint8_t min;
	uint16_t max;
	uint8_t position;
	ActionType action;
	void (*ExecuteFunction)(void);
}FunctionContext;

Menu_Item_t *ButtonParent;
Menu_Item_t *Parent;
Menu_Item_t	*NextTypeSibling;
Menu_Item_t	*PrevTypeSibling;

extern FunctionContext CurrentFunctionContext;
uint8_t CurrentButtonUnderSetupIndex;
uint8_t CurrentBankIndex;
uint8_t BankNumberTemp;

void Menu_SettingsSelectCallback(void);
void Menu_SystemSettingsEnterCallback(void);
void Menu_MIDIchannelSelectCallback(void);

void Menu_MainScreenSelectCallback(void);
void Menu_MainScreenRedrawCallback(void);

void Menu_SystemSetupEnterCallback(void);
void Menu_Prg_ch_modeSelectCallback(void);
void Bnk_Sel_modeSelectCallBack(void);
void Show_pr_nameSelectCallBack(void);
void Target_deviceSelectCallback(void);
void USB_baudrateSelectCallback(void);
void Menu_BnkSwOnBoardSelectCallback(void);
void MIDI_thru_mapEnterCallback(void);
void Menu_MIDI_out_mapSelectCallback(void);
void Menu_USB_out_mapSelectCallback(void);
void Menu_Bank_maxSelectCallback(void);

void Menu_Restore_defaultsSelectCallback(void);
void Menu_Restore_defaultsEnterCallback(void);

void Screen_brightnessSelectCallback(void);
void Screen_contrastSelectCallback(void);
void Menu_PwmControllableEnterCallback(void);

void Menu_Exp_P_typeSelectCallback(void);
//void Menu_Exp_P_typeSelectCallback(void);
void Menu_BUT_hold_time_typeSelectCallback(void);
void Menu_Tap_displaySelectCallback(void);
void Menu_Tap_typeSelectCallback(void);
void Menu_Display_typeSelectCallback(void);
void Menu_Tuner_scheme_SelectCallback(void);
void Menu_pedal_brightness_SelectCallback(void);
void Menu_TAP_SelectCallback(void);

void Menu_ExpPedalCC_SelectCallback(void);
void Menu_ExpPedalClb_SelectCallback(void);
void Menu_ExpPedalClb_EnterCallback(void);
void Menu_ExpPedalClb_RedrawCallback(void);
/*void Menu_ExpP1CC_SelectCallback(void);
void Menu_ExpP2CC_SelectCallback(void);
void Menu_Exp_OnBrd_SelectCallback(void);*/
void Menu_Send_tuner_CC_SelectCallback(void);
void Menu_Button_EnterCallback(void);

void Menu_Button1_SelectCallback(void);

void Menu_Type_change_SelectCallback(void);
void Menu_Type_change_EnterCallback(void);
void Menu_Message1_SelectCallback(void);
void Menu_Message2_SelectCallback(void);
void Menu_Message3_SelectCallback(void);
void Menu_Message4_SelectCallback(void);
void Menu_Message_EnterCallback(void);
void Menu_Channel_SelectCallback(void);
void Menu_Number_SelectCallback(void);
void Menu_Buttons_setup_EnterCallback(void);
void Menu_RLY_SelectCallback(void);
void Menu_RLY_EnterCallback(void);
void Menu_RLY1_SelectCallback(void);
void Menu_RLY2_SelectCallback(void);
void Menu_RLY3_SelectCallback(void);
void Menu_RLY4_SelectCallback(void);

void Menu_ccNumber_SelectCallback(void);
void Menu_ccNumber_EnterCallback(void);
void Menu_ccFreezeNumber_SelectCallback(void);
void Menu_ccFreezeNumber_EnterCallback(void);

void Menu_ccValue_SelectCallback(void);
void Menu_ccOnValue_SelectCallback(void);
void Menu_ccOffValue_SelectCallback(void);

void Menu_iaState_SelectCallback(void);
void Menu_iaState_EnterCallback(void);
void Menu_autoSendState_SelectCallback(void);
void Menu_autoSendState_EnterCallback(void);
void Menu_ctrlNumber_SelectCallback(void);
void Menu_ctrlMsbNumber_SelectCallback(void);
void Menu_ctrlLsbNumber_SelectCallback(void);
void Menu_paramOnValue_SelectCallback(void);

void Menu_paramMsbOnValue_SelectCallback(void);
void Menu_paramLsbOnValue_SelectCallback(void);
void Menu_paramOffValue_SelectCallback(void);
void Menu_paramMsbOffValue_SelectCallback(void);
void Menu_paramLsbOffValue_SelectCallback(void);
void Menu_noteNumber_SelectCallback(void);
void Menu_noteNumber_EnterCallback(void);
void Menu_noteVelocity_SelectCallback(void);
void Menu_noteVelocity_EnterCallback(void);
void Menu_bankNumber_SelectCallback(void);
void Menu_bankNumber_EnterCallback(void);
void Menu_ctrlLsbNumber_EnterCallback(void);
void Menu_SelectSaveBank_SelectCallback(void);
void Menu_LoadBank_SelectCallback(void);
void Menu_BankName_SelectCallback(void);
void Menu_CopyBank_SelectCallback(void);
void Menu_SrcBank_SelectCallback(void);
void Menu_DstBank_SelectCallback(void);

void DisplayUpdateCurrentValue(uint16_t value, uint8_t position);
void DisplayUpdateBitStringValue(uint16_t value, uint8_t position);
void ClearCurrentContext(void);
void CommonExecuteFunction(void);
void MainScreenExecuteFunction(void);
void ChangeMessageFunction(void);
void ChangeRLYFunction(void);
void ChangeiaStateFunction(void);
void UpdateiaStateIndexNumber(uint8_t number);
void BankSaveExecuteFunction(void);
void BankRenameExecuteFunction(void);
void BankCopyExecuteFunction(void);
void BankLoadExecuteFunction(void);

void Menu_NavigationProcessButtonEvent(ButtonEvent *ButtonEvent);
void DisplayUpdateHeader(const char* Text);

//void LoadBank(uint8_t BankNumber, BankSettings *bank);



#endif /* CALLBACKFUNCTIONS_H_ */