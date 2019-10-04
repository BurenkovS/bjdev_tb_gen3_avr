/*
 * MenuItems.c
 *
 * Created: 19.03.2017 11:08:56
 *  Author: 123
 */ 

#include "CallbackFunctions.h"
#include "MenuItems.h"
#include "PwmControllableFeatures.h"//Get some defines here

MenuPosition CurrentMenuPosition;

MENU_ITEM(Main_screen, NULL_MENU, NULL_MENU, NULL_MENU, Banks , Menu_MainScreenSelectCallback, NULL, Menu_MainScreenRedrawCallback, "", 0);
		MENU_ITEM(KeyLoadBank, NULL_MENU, NULL_MENU, Main_screen, NULL_MENU, Menu_LoadBank_SelectCallback, NULL, NULL, "LOAD BANK      ", 0);
#if defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)
		MENU_ITEM(Banks, System_Setup, Pedal_view, Main_screen, Save_bank, Menu_SettingsSelectCallback, Menu_SystemSetupEnterCallback, NULL, "BANKS          ", 0);
#else
		MENU_ITEM(Banks, System_Setup, Exp_Tap_Tune, Main_screen, Save_bank, Menu_SettingsSelectCallback, Menu_SystemSetupEnterCallback, NULL, "BANKS          ", 0);
#endif
			MENU_ITEM(Save_bank, Load_bank, Rename_bank, Banks, SelectSaveBank, Menu_SettingsSelectCallback, Menu_SystemSetupEnterCallback, NULL, "SAVE BANK      ", 0);
				MENU_ITEM(SelectSaveBank, NULL_MENU, NULL_MENU, Save_bank, NULL_MENU, Menu_SelectSaveBank_SelectCallback, NULL, NULL, "SELECT BANK", 0);
			MENU_ITEM(Load_bank, Copy_bank, Save_bank, Banks, SelectLoadBank, Menu_SettingsSelectCallback, Menu_SystemSetupEnterCallback, NULL, "LOAD BANK      ", 0);
				MENU_ITEM(SelectLoadBank, NULL_MENU, NULL_MENU, Load_bank, NULL_MENU, Menu_LoadBank_SelectCallback, NULL, NULL, "", 0);
			MENU_ITEM(Copy_bank, Rename_bank, Load_bank, Banks, SelectCopyBank, Menu_SettingsSelectCallback, Menu_SystemSetupEnterCallback, NULL, "COPY BANK      ", 0);
				MENU_ITEM(SelectCopyBank, NULL_MENU, NULL_MENU, NULL_MENU, SrcBank, Menu_CopyBank_SelectCallback, NULL, NULL, "", 0);
					MENU_ITEM(SrcBank, DstBank, DstBank, Copy_bank, NULL_MENU, Menu_SrcBank_SelectCallback, NULL, NULL, "", 0);
					MENU_ITEM(DstBank, SrcBank, SrcBank, Copy_bank, NULL_MENU, Menu_DstBank_SelectCallback, NULL, NULL, "", 0);
			MENU_ITEM(Rename_bank, Save_bank, Copy_bank, Banks, BankName, Menu_SettingsSelectCallback, Menu_SystemSetupEnterCallback, NULL, "RENAME BANK    ", 0);
				MENU_ITEM(BankName, NULL_MENU, NULL_MENU, Rename_bank, NULL_MENU, Menu_BankName_SelectCallback, NULL, NULL, "BANK NAME  ", 0);
		MENU_ITEM(System_Setup, Buttons_setup, Banks, Main_screen, MIDI_channel, Menu_SettingsSelectCallback, Menu_SystemSetupEnterCallback, NULL, "SYSTEM SETUP   ", 0);
// by peter			MENU_ITEM(MIDI_channel, Prg_ch_mode, Restore_defaults/*Screen_contrast*/, System_Setup, NULL_MENU, Menu_MIDIchannelSelectCallback, Menu_SystemSettingsEnterCallback, NULL, "MIDI CHANNEL   ", 0);
// peters changing
MENU_ITEM(MIDI_channel, Prg_ch_mode, Send_Dump/*Screen_contrast*/, System_Setup, NULL_MENU, Menu_MIDIchannelSelectCallback, Menu_SystemSettingsEnterCallback, NULL, "MIDI CHANNEL   ", 0);
			MENU_ITEM(Prg_ch_mode, Bnk_Sel_mode, MIDI_channel, System_Setup, NULL_MENU, Menu_Prg_ch_modeSelectCallback, Menu_SystemSettingsEnterCallback, NULL, "PRG. CH. MODE  ", 0);
			MENU_ITEM(Bnk_Sel_mode, Show_pr_name, Prg_ch_mode, System_Setup, NULL_MENU, Bnk_Sel_modeSelectCallBack, Menu_SystemSettingsEnterCallback, NULL, "BANK SEL. MODE ", 0);
			MENU_ITEM(Show_pr_name, Target_device, Bnk_Sel_mode, System_Setup, NULL_MENU, Show_pr_nameSelectCallBack, Menu_SystemSettingsEnterCallback, NULL, "SHOW PR. NAME  ", 0);
			MENU_ITEM(Target_device, USB_baudrate, Show_pr_name, System_Setup, NULL_MENU, Target_deviceSelectCallback, Menu_SystemSettingsEnterCallback, NULL, "TARGET DEVICE  ", 0);
			MENU_ITEM(USB_baudrate, Bank_sw_mode, Target_device, System_Setup, NULL_MENU, USB_baudrateSelectCallback, Menu_SystemSettingsEnterCallback, NULL, "USB BAUDRATE   ", 0);
			MENU_ITEM(Bank_sw_mode, MIDI_thru_map, USB_baudrate, System_Setup, NULL_MENU, Menu_BnkSwOnBoardSelectCallback, Menu_SystemSettingsEnterCallback, NULL, "BANK SW. MODE  ", 0);
			MENU_ITEM(MIDI_thru_map, Bank_max, Bank_sw_mode, System_Setup, MIDI_out, Menu_SettingsSelectCallback, MIDI_thru_mapEnterCallback, NULL, "MIDI THRU MAP  ", 0);
				MENU_ITEM(MIDI_out, USB_out, USB_out, MIDI_thru_map, NULL_MENU, Menu_MIDI_out_mapSelectCallback, Menu_SystemSettingsEnterCallback, NULL, "MIDI IN        ", 0);
				MENU_ITEM(USB_out, MIDI_out, MIDI_out, MIDI_thru_map, NULL_MENU, Menu_USB_out_mapSelectCallback, Menu_SystemSettingsEnterCallback, NULL, "USB IN         ", 0);
			MENU_ITEM(Bank_max, Restore_defaults, MIDI_thru_map, System_Setup, NULL_MENU, Menu_Bank_maxSelectCallback, Menu_SystemSettingsEnterCallback, NULL, "BANK MAX       ", 0);
// by peter			MENU_ITEM(Restore_defaults, MIDI_channel/*Screen_brightness*/, Bank_max, System_Setup, NULL_MENU, Menu_Restore_defaultsSelectCallback, Menu_Restore_defaultsEnterCallback, NULL, "RESTORE DEFAULT", 0);
// peters changing
MENU_ITEM(Restore_defaults, Send_Dump/*Screen_brightness*/, Bank_max, System_Setup, NULL_MENU, Menu_Restore_defaultsSelectCallback, Menu_Restore_defaultsEnterCallback, NULL, "RESTORE DEFAULT", 0);
//peters
MENU_ITEM(Send_Dump, MIDI_channel, Restore_defaults, System_Setup, NULL_MENU, Menu_SystemDumpSelectCallback, Menu_SystemDumpEnterCallback, NULL, "SEND DUMP      ", 0);
//MENU_ITEM(Send_Dump, MIDI_channel, Restore_defaults, System_Setup, Dumping, Menu_SettingsSelectCallback, Menu_SystemDump, NULL, "SEND DUMP      ", 0);
//MENU_ITEM(Dumping, NULL_MENU, NULL_MENU, Send_Dump, NULL_MENU, Menu_Dumping, NULL, NULL, "DUMPING...     ", 0);

			//TODO uncomment in new PCB
			//MENU_ITEM(Screen_brightness, Screen_contrast, Bank_max, System_Setup, NULL_MENU, Screen_brightnessSelectCallback, Menu_PwmControllableEnterCallback, NULL, "SCRN BACKLIGHT ", SCREEN_BRIGHTNESS_PWM_NUM);
			//MENU_ITEM(Screen_contrast, MIDI_channel, Screen_brightness, System_Setup, NULL_MENU, Screen_contrastSelectCallback, Menu_PwmControllableEnterCallback, NULL, "SCRN CONTRAST ", SCREEN_CONTRAST_PWM_NUM);
		
		MENU_ITEM(Buttons_setup, Exp_Tap_Tune, System_Setup, Main_screen, Button1, Menu_SettingsSelectCallback, Menu_Buttons_setup_EnterCallback, NULL, "BUTTONS SETUP  ", 0);
			#ifdef TB_12_DEVICE
			MENU_ITEM(Button1, Button2, Button12, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 1 SETUP ", 0);
			MENU_ITEM(Button2, Button3, Button1, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 2 SETUP ", 1);
			MENU_ITEM(Button3, Button4, Button2, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 3 SETUP ", 2);
			MENU_ITEM(Button4, Button5, Button3, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 4 SETUP ", 3);
			MENU_ITEM(Button5, Button6, Button4, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 5 SETUP ", 4);
			MENU_ITEM(Button6, Button7, Button5, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 6 SETUP ", 5);
			MENU_ITEM(Button7, Button8, Button6, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 7 SETUP ", 6);
			MENU_ITEM(Button8, Button9, Button7, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 8 SETUP ", 7);
			MENU_ITEM(Button9, Button10, Button8, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 9 SETUP ", 8);
			MENU_ITEM(Button10, Button11, Button9, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 10 SETUP", 9);
			MENU_ITEM(Button11, Button12, Button10, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 11 SETUP", 10);
			MENU_ITEM(Button12, Button1, Button11, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 12 SETUP", 11);
			#elif defined TB_11P_DEVICE
			MENU_ITEM(Button1, Button2, Button11, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 1 SETUP ", 0);
			MENU_ITEM(Button2, Button3, Button1, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 2 SETUP ", 1);
			MENU_ITEM(Button3, Button4, Button2, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 3 SETUP ", 2);
			MENU_ITEM(Button4, Button5, Button3, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 4 SETUP ", 3);
			MENU_ITEM(Button5, Button6, Button4, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 5 SETUP ", 4);
			MENU_ITEM(Button6, Button7, Button5, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 6 SETUP ", 5);
			MENU_ITEM(Button7, Button8, Button6, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 7 SETUP ", 6);
			MENU_ITEM(Button8, Button9, Button7, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 8 SETUP ", 7);
			MENU_ITEM(Button9, Button10, Button8, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 9 SETUP ", 8);
			MENU_ITEM(Button10, Button11, Button9, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 10 SETUP", 9);
			MENU_ITEM(Button11, Button1, Button10, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 11 SETUP", 10);
			#elif defined TB_8_DEVICE
			MENU_ITEM(Button1, Button2, Button8, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 1 SETUP ", 0);
			MENU_ITEM(Button2, Button3, Button1, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 2 SETUP ", 1);
			MENU_ITEM(Button3, Button4, Button2, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 3 SETUP ", 2);
			MENU_ITEM(Button4, Button5, Button3, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 4 SETUP ", 3);
			MENU_ITEM(Button5, Button6, Button4, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 5 SETUP ", 4);
			MENU_ITEM(Button6, Button7, Button5, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 6 SETUP ", 5);
			MENU_ITEM(Button7, Button8, Button6, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 7 SETUP ", 6);
			MENU_ITEM(Button8, Button1, Button7, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 8 SETUP ", 7);
			#elif defined TB_6P_DEVICE
			MENU_ITEM(Button1, Button2, Button6, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 1 SETUP ", 0);
			MENU_ITEM(Button2, Button3, Button1, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 2 SETUP ", 1);
			MENU_ITEM(Button3, Button4, Button2, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 3 SETUP ", 2);
			MENU_ITEM(Button4, Button5, Button3, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 4 SETUP ", 3);
			MENU_ITEM(Button5, Button6, Button4, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 5 SETUP ", 4);
			MENU_ITEM(Button6, Button1, Button5, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 6 SETUP ", 5);
			#elif defined TB_5_DEVICE
			MENU_ITEM(Button1, Button2, Button5, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 1 SETUP ", 0);
			MENU_ITEM(Button2, Button3, Button1, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 2 SETUP ", 1);
			MENU_ITEM(Button3, Button4, Button2, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 3 SETUP ", 2);
			MENU_ITEM(Button4, Button5, Button3, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 4 SETUP ", 3);
			MENU_ITEM(Button5, Button1, Button4, Buttons_setup, NULL_MENU, Menu_Button1_SelectCallback, Menu_Button_EnterCallback, NULL, "BUTTON 5 SETUP ", 4);
			#endif
				MENU_ITEM(Type, NULL_MENU, NULL_MENU, NULL_MENU, NULL_MENU, Menu_Type_change_SelectCallback, Menu_Type_change_EnterCallback, NULL, "", 0);
				//PC
				MENU_ITEM(Message1, Message2, Type, NULL_MENU, Channel, Menu_Message1_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
				MENU_ITEM(Message2, Message3, Message1, NULL_MENU, Channel, Menu_Message2_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
				MENU_ITEM(Message3, Message4, Message2, NULL_MENU, Channel, Menu_Message3_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
				MENU_ITEM(Message4, PC_RLY, Message3, NULL_MENU, Channel, Menu_Message4_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
					MENU_ITEM(Channel, Number, Number, NULL_MENU, NULL_MENU, Menu_Channel_SelectCallback, NULL, NULL, "", 0);
					MENU_ITEM(Number, Channel, Channel, NULL_MENU, NULL_MENU, Menu_Number_SelectCallback, NULL, NULL, "", 0);
				MENU_ITEM(PC_RLY, iaState, Message4, NULL_MENU, RLY1, Menu_RLY_SelectCallback, Menu_RLY_EnterCallback, NULL, "", 0);
					MENU_ITEM(RLY1, RLY2, RLY4, NULL_MENU, NULL_MENU, Menu_RLY1_SelectCallback, NULL, NULL, "", 0);
					MENU_ITEM(RLY2, RLY3, RLY1, NULL_MENU, NULL_MENU, Menu_RLY2_SelectCallback, NULL, NULL, "", 0);
					MENU_ITEM(RLY3, RLY4, RLY2, NULL_MENU, NULL_MENU, Menu_RLY3_SelectCallback, NULL, NULL, "", 0);
					MENU_ITEM(RLY4, RLY1, RLY3, NULL_MENU, NULL_MENU, Menu_RLY4_SelectCallback, NULL, NULL, "", 0);
				MENU_ITEM(iaState, Type, PC_RLY, NULL_MENU, NULL_MENU, Menu_iaState_SelectCallback, Menu_iaState_EnterCallback, NULL, "", 0);
				//CCT
				MENU_ITEM(CCT_ccNumber, CCT_ccFreezeNumber, Type, NULL_MENU, NULL_MENU, Menu_ccNumber_SelectCallback, Menu_ccNumber_EnterCallback, NULL, "", 0);
				MENU_ITEM(CCT_ccFreezeNumber, CCT_autoSendState, CCT_ccNumber, NULL_MENU, NULL_MENU, Menu_ccFreezeNumber_SelectCallback, Menu_ccFreezeNumber_EnterCallback, NULL, "", 0);
				MENU_ITEM(CCT_autoSendState, CCT_ccValue, CCT_ccFreezeNumber, NULL_MENU, NULL_MENU, Menu_autoSendState_SelectCallback, Menu_autoSendState_EnterCallback, NULL, "", 0);
				MENU_ITEM(CCT_ccValue, CCT_RLY, CCT_autoSendState, NULL_MENU, CCT_ccOnValue, Menu_ccValue_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
					MENU_ITEM(CCT_ccOnValue, CCT_ccOffValue, CCT_ccOffValue, NULL_MENU, NULL_MENU, Menu_ccOnValue_SelectCallback, NULL, NULL, "", 0);
					MENU_ITEM(CCT_ccOffValue, CCT_ccOnValue, CCT_ccOnValue, NULL_MENU, NULL_MENU, Menu_ccOffValue_SelectCallback, NULL, NULL, "", 0);
				MENU_ITEM(CCT_RLY, Type, CCT_ccValue, NULL_MENU, RLY1, Menu_RLY_SelectCallback, Menu_RLY_EnterCallback, NULL, "", 0);
								
				//CCM
				MENU_ITEM(CCM_ctrlLsbNumber, CCM_ccValue, Type, NULL_MENU, NULL_MENU, Menu_ccNumber_SelectCallback, Menu_ccNumber_EnterCallback, NULL, "", 0);
				MENU_ITEM(CCM_ccValue, CCM_RLY, CCM_ctrlLsbNumber, NULL_MENU, CCT_ccOnValue, Menu_ccValue_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
				MENU_ITEM(CCM_RLY, Type, CCM_ccValue, NULL_MENU, RLY1, Menu_RLY_SelectCallback, Menu_RLY_EnterCallback, NULL, "", 0);
				//CCC
				MENU_ITEM(CCC_ctrlLsbNumber, CCC_ccValue, Type, NULL_MENU, NULL_MENU, Menu_ccNumber_SelectCallback, Menu_ccNumber_EnterCallback, NULL, "", 0);
				MENU_ITEM(CCC_ccValue, CCC_RLY, CCC_ctrlLsbNumber, NULL_MENU, CCT_ccOnValue, Menu_ccValue_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
				MENU_ITEM(CCC_RLY, Type, CCC_ccValue, NULL_MENU, RLY1, Menu_RLY_SelectCallback, Menu_RLY_EnterCallback, NULL, "", 0);
				//NRPN_TOGGLE
				MENU_ITEM(NRPN_TOGGLE_ctrlNumber, NRPN_TOGGLE_paramOnValue, Type, NULL_MENU, NRPN_TOGGLE_ctrlMsbNumber, Menu_ctrlNumber_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
					MENU_ITEM(NRPN_TOGGLE_ctrlMsbNumber, NRPN_TOGGLE_ctrlLsbNumber, NRPN_TOGGLE_ctrlLsbNumber, NULL_MENU, NULL_MENU, Menu_ctrlMsbNumber_SelectCallback, NULL, NULL, "", 0);
					MENU_ITEM(NRPN_TOGGLE_ctrlLsbNumber, NRPN_TOGGLE_ctrlMsbNumber, NRPN_TOGGLE_ctrlMsbNumber, NULL_MENU, NULL_MENU, Menu_ctrlLsbNumber_SelectCallback, NULL, NULL, "", 0);
				MENU_ITEM(NRPN_TOGGLE_paramOnValue, NRPN_TOGGLE_paramOffValue, NRPN_TOGGLE_ctrlNumber, NULL_MENU, NRPN_TOGGLE_paramMsbOnValue, Menu_paramOnValue_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
					MENU_ITEM(NRPN_TOGGLE_paramMsbOnValue, NRPN_TOGGLE_paramLsbOnValue, NRPN_TOGGLE_paramLsbOnValue, NULL_MENU, NULL_MENU, Menu_paramMsbOnValue_SelectCallback, NULL, NULL, "", 0);
					MENU_ITEM(NRPN_TOGGLE_paramLsbOnValue, NRPN_TOGGLE_paramMsbOnValue, NRPN_TOGGLE_paramMsbOnValue, NULL_MENU, NULL_MENU, Menu_paramLsbOnValue_SelectCallback, NULL, NULL, "", 0);
				MENU_ITEM(NRPN_TOGGLE_paramOffValue, NRPN_TOGGLE_autoSendState, NRPN_TOGGLE_paramOnValue, NULL_MENU, NRPN_TOGGLE_paramMsbOffValue, Menu_paramOffValue_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
					MENU_ITEM(NRPN_TOGGLE_paramMsbOffValue, NRPN_TOGGLE_paramLsbOffValue, NRPN_TOGGLE_paramLsbOffValue, NULL_MENU, NULL_MENU, Menu_paramMsbOffValue_SelectCallback, NULL, NULL, "", 0);
					MENU_ITEM(NRPN_TOGGLE_paramLsbOffValue, NRPN_TOGGLE_paramMsbOffValue, NRPN_TOGGLE_paramMsbOffValue, NULL_MENU, NULL_MENU, Menu_paramLsbOffValue_SelectCallback, NULL, NULL, "", 0);
				//MENU_ITEM(NRPN_initState, NRPN_autoSendState, paramOffValue, NULL_MENU, NULL_MENU, Menu_initState_SelectCallback, Menu_initState_EnterCallback, NULL, "", 0);
				MENU_ITEM(NRPN_TOGGLE_autoSendState, NRPN_TOGGLE_RLY, NRPN_TOGGLE_paramOffValue, NULL_MENU, NULL_MENU, Menu_autoSendState_SelectCallback, Menu_autoSendState_EnterCallback, NULL, "", 0);
				MENU_ITEM(NRPN_TOGGLE_RLY, Type, NRPN_TOGGLE_autoSendState, NULL_MENU, RLY1, Menu_RLY_SelectCallback, Menu_RLY_EnterCallback, NULL, "", 0);
				//NRPN_MOMENTARY
				MENU_ITEM(NRPN_MOMENTARY_ctrlNumber, NRPN_MOMENTARY_paramOnValue, Type, NULL_MENU, NRPN_TOGGLE_ctrlMsbNumber, Menu_ctrlNumber_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
				MENU_ITEM(NRPN_MOMENTARY_paramOnValue, NRPN_MOMENTARY_paramOffValue, NRPN_MOMENTARY_ctrlNumber, NULL_MENU, NRPN_TOGGLE_paramMsbOnValue, Menu_paramOnValue_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
				MENU_ITEM(NRPN_MOMENTARY_paramOffValue, NRPN_MOMENTARY_RLY, NRPN_MOMENTARY_paramOnValue, NULL_MENU, NRPN_TOGGLE_paramOffValue, Menu_paramOffValue_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
				MENU_ITEM(NRPN_MOMENTARY_RLY, Type, NRPN_MOMENTARY_paramOffValue, NULL_MENU, RLY1, Menu_RLY_SelectCallback, Menu_RLY_EnterCallback, NULL, "", 0);
				//NRPN_CONST_VAL
				MENU_ITEM(NRPN_CONST_ctrlNumber, NRPN_CONST_paramOnValue, Type, NULL_MENU, NRPN_TOGGLE_ctrlMsbNumber, Menu_ctrlNumber_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
				MENU_ITEM(NRPN_CONST_paramOnValue, /*NRPN_CONST_autoSendState*/NRPN_CONST_RLY, NRPN_CONST_ctrlNumber, NULL_MENU, NRPN_TOGGLE_paramMsbOnValue, Menu_paramOnValue_SelectCallback, Menu_Message_EnterCallback, NULL, "", 0);
				//MENU_ITEM(NRPN_CONST_autoSendState, NRPN_CONST_RLY, NRPN_CONST_paramOnValue, NULL_MENU, NULL_MENU, Menu_autoSendState_SelectCallback, Menu_autoSendState_EnterCallback, NULL, "", 0);
				MENU_ITEM(NRPN_CONST_RLY, Type, /*NRPN_CONST_autoSendState*/NRPN_CONST_paramOnValue, NULL_MENU, RLY1, Menu_RLY_SelectCallback, Menu_RLY_EnterCallback, NULL, "", 0);
				//NOTE_ON NOTE_OFF
				MENU_ITEM(noteNumber, noteVelocity, Type, NULL_MENU, NULL_MENU, Menu_noteNumber_SelectCallback, Menu_noteNumber_EnterCallback, NULL, "", 0);
				MENU_ITEM(noteVelocity, Type, noteNumber, NULL_MENU, NULL_MENU, Menu_noteVelocity_SelectCallback, Menu_noteVelocity_EnterCallback, NULL, "", 0);
				//BANK_TO
				MENU_ITEM(bankNumber, Type, Type, NULL_MENU, NULL_MENU, Menu_bankNumber_SelectCallback, Menu_bankNumber_EnterCallback, NULL, "", 0);
#if defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)
		MENU_ITEM(Exp_Tap_Tune, Pedal_view, Buttons_setup, Main_screen, TAP, Menu_SettingsSelectCallback, Menu_SystemSetupEnterCallback, NULL, "EXP&TAP&TUNE   ", 0);
#else
		MENU_ITEM(Exp_Tap_Tune, Banks, Buttons_setup, Main_screen, TAP, Menu_SettingsSelectCallback, Menu_SystemSetupEnterCallback, NULL, "EXP&TAP&TUNE   ", 0);
#endif
			MENU_ITEM(TAP, Tap_display, BUT_hold_time, Exp_Tap_Tune, NULL_MENU, Menu_TAP_SelectCallback, Menu_SystemSettingsEnterCallback, NULL, "TAP CC         ", 0);
			MENU_ITEM(Tap_display, Tap_type, TAP, Exp_Tap_Tune, NULL_MENU, Menu_Tap_displaySelectCallback, Menu_SystemSettingsEnterCallback, NULL, "TAP DISPLAY    ", 0);
			MENU_ITEM(Tap_type, Exp_P1_CC, Tap_display, Exp_Tap_Tune, NULL_MENU, Menu_Tap_typeSelectCallback, Menu_SystemSettingsEnterCallback, NULL, "TAP TYPE       ", 0);
			MENU_ITEM(Exp_P1_CC, Exp_P1_type, Tap_type, Exp_Tap_Tune, NULL_MENU, Menu_ExpPedalCC_SelectCallback, Menu_SystemSettingsEnterCallback, NULL, "EXP. P1 CC     ", 0);
			MENU_ITEM(Exp_P1_type, Exp_P1_clb, Exp_P1_CC, Exp_Tap_Tune, NULL_MENU, Menu_Exp_P_typeSelectCallback, Menu_SystemSettingsEnterCallback, NULL, "EXP. P1 TYPE   ", 0);
			MENU_ITEM(Exp_P1_clb, Exp_P2_CC, Exp_P1_type, Exp_Tap_Tune, NULL_MENU, Menu_ExpPedalClb_SelectCallback, Menu_ExpPedalClb_EnterCallback, Menu_ExpPedalClb_RedrawCallback, "EXP. P1 CLB    ", 0);
			MENU_ITEM(Exp_P2_CC, Exp_P2_type, Exp_P1_clb, Exp_Tap_Tune, NULL_MENU, Menu_ExpPedalCC_SelectCallback, Menu_SystemSettingsEnterCallback, NULL, "EXP. P2 CC     ", 1);
			MENU_ITEM(Exp_P2_type, Exp_P2_clb, Exp_P2_CC, Exp_Tap_Tune, NULL_MENU, Menu_Exp_P_typeSelectCallback, Menu_SystemSettingsEnterCallback, NULL, "EXP. P2 TYPE   ", 1);
#if defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)
			MENU_ITEM(Exp_P2_clb, Exp_OnBrd_CC, Exp_P2_type, Exp_Tap_Tune, NULL_MENU, Menu_ExpPedalClb_SelectCallback, Menu_ExpPedalClb_EnterCallback, Menu_ExpPedalClb_RedrawCallback, "EXP. P2 CLB    ", 1);
			MENU_ITEM(Exp_OnBrd_CC, Exp_OnBrd_Alt_CC, Exp_P2_clb, Exp_Tap_Tune, NULL_MENU, Menu_ExpPedalCC_SelectCallback, Menu_SystemSettingsEnterCallback, NULL, "EXP. ONBRD     ", 2);
			MENU_ITEM(Exp_OnBrd_Alt_CC, Exp_OnBrd_clb, Exp_OnBrd_CC, Exp_Tap_Tune, NULL_MENU, Menu_ExpPedalCC_SelectCallback, Menu_SystemSettingsEnterCallback, NULL, "EXP. ONBRD ALT ", 3);

			//Onboard pedal have no Type menu
			MENU_ITEM(Exp_OnBrd_clb, Send_tuner_CC, Exp_OnBrd_Alt_CC, Exp_Tap_Tune, NULL_MENU, Menu_ExpPedalClb_SelectCallback, Menu_ExpPedalClb_EnterCallback, Menu_ExpPedalClb_RedrawCallback, "EXP. ONBRD CLB ", 2);
			MENU_ITEM(Send_tuner_CC, BUT_hold_time, Exp_OnBrd_clb, Exp_Tap_Tune, NULL_MENU, Menu_Send_tuner_CC_SelectCallback, Menu_SystemSettingsEnterCallback, NULL, "SEND TUNER CC  ", 0);
#else
			MENU_ITEM(Exp_P2_clb, Send_tuner_CC, Exp_P2_type, Exp_Tap_Tune, NULL_MENU, Menu_ExpPedalClb_SelectCallback, Menu_ExpPedalClb_EnterCallback, Menu_ExpPedalClb_RedrawCallback, "EXP. P2 CLB    ", 1);
			MENU_ITEM(Send_tuner_CC, BUT_hold_time, Exp_P2_clb, Exp_Tap_Tune, NULL_MENU, Menu_Send_tuner_CC_SelectCallback, Menu_SystemSettingsEnterCallback, NULL, "SEND TUNER CC  ", 0);
#endif
			MENU_ITEM(BUT_hold_time, TAP, Send_tuner_CC, Exp_Tap_Tune, NULL_MENU, Menu_BUT_hold_time_typeSelectCallback, Menu_SystemSettingsEnterCallback, NULL, "BUT HOLD TIME  ", 0);
#if defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)
		MENU_ITEM(Pedal_view, Banks, Exp_Tap_Tune, Main_screen, Display_type, Menu_SettingsSelectCallback, Menu_SystemSetupEnterCallback, NULL, "PEDAL VIEW     ", 0);
			MENU_ITEM(Display_type, Tuner_scheme, Tuner_scheme/*Pedal_brightness*/, Pedal_view, NULL_MENU, Menu_Display_typeSelectCallback, Menu_SystemSettingsEnterCallback, NULL, "DISPLAY TYPE   ", 0);
			MENU_ITEM(Tuner_scheme, Display_type/*Pedal_brightness*/, Display_type, Pedal_view, NULL_MENU, Menu_Tuner_scheme_SelectCallback, Menu_SystemSettingsEnterCallback, NULL, "TUNER SCHEME   ", 0);
			//
			//MENU_ITEM(Pedal_brightness, Display_type, Tuner_scheme, Pedal_view, NULL_MENU, Menu_pedal_brightness_SelectCallback, Menu_PwmControllableEnterCallback, NULL, "BRIGHTNESS   ", PEDAL_BRIGHTNESS_PWM_NUM);
			//
#endif
