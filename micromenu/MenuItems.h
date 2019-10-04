/*
 * MenuItems.h
 *
 * Created: 15.03.2017 20:23:22
 *  Author: 123
 */ 


#ifndef MENUITEMS_H_
#define MENUITEMS_H_

#include "MicroMenu.h"

typedef enum menuposition
{
	MenuMain = 0,
	MenuSettings,
	MenuSystemSetup,
	MenuSystemSetupInside,
	MenuButtonSetup,
	MenuTypeChange,
	MenuMsgChange,
	MenuRLYChange,
	MenuiaStateChange,
	MenuCommonChange,
	MenuCopyBank
} MenuPosition;

extern MenuPosition CurrentMenuPosition;

extern Menu_Item_t MENU_ITEM_STORAGE Main_screen;
extern Menu_Item_t MENU_ITEM_STORAGE Banks;
extern Menu_Item_t MENU_ITEM_STORAGE MIDI_channel;
extern Menu_Item_t MENU_ITEM_STORAGE Prg_ch_mode;
extern Menu_Item_t MENU_ITEM_STORAGE Bnk_Sel_mode;
extern Menu_Item_t MENU_ITEM_STORAGE Show_pr_name;
extern Menu_Item_t MENU_ITEM_STORAGE Target_device;
extern Menu_Item_t MENU_ITEM_STORAGE USB_baudrate;
extern Menu_Item_t MENU_ITEM_STORAGE Bank_sw_mode;
extern Menu_Item_t MENU_ITEM_STORAGE MIDI_out;
extern Menu_Item_t MENU_ITEM_STORAGE USB_out;
extern Menu_Item_t MENU_ITEM_STORAGE Bank_max;
extern Menu_Item_t MENU_ITEM_STORAGE Restore_defaults;
//extern Menu_Item_t MENU_ITEM_STORAGE Screen_brightness;
//extern Menu_Item_t MENU_ITEM_STORAGE Screen_contrast;
extern Menu_Item_t MENU_ITEM_STORAGE Tap_display;
extern Menu_Item_t MENU_ITEM_STORAGE Tap_type;
extern Menu_Item_t MENU_ITEM_STORAGE Exp_P1_type;
extern Menu_Item_t MENU_ITEM_STORAGE Exp_P2_type;
extern Menu_Item_t MENU_ITEM_STORAGE BUT_hold_time;
extern Menu_Item_t MENU_ITEM_STORAGE Display_type;
extern Menu_Item_t MENU_ITEM_STORAGE Tuner_scheme;
//extern Menu_Item_t MENU_ITEM_STORAGE Pedal_brightness;
extern Menu_Item_t MENU_ITEM_STORAGE TAP;
extern Menu_Item_t MENU_ITEM_STORAGE Exp_P1_CC;
extern Menu_Item_t MENU_ITEM_STORAGE Exp_P2_CC;

#if defined (TB_11P_DEVICE) || defined (TB_6P_DEVICE)
	extern Menu_Item_t MENU_ITEM_STORAGE Exp_OnBrd;
#endif

extern Menu_Item_t MENU_ITEM_STORAGE Send_tuner_CC;
extern Menu_Item_t MENU_ITEM_STORAGE Button1;
extern Menu_Item_t MENU_ITEM_STORAGE Type;
extern Menu_Item_t MENU_ITEM_STORAGE Preset_change;
extern Menu_Item_t MENU_ITEM_STORAGE Message1;
extern Menu_Item_t MENU_ITEM_STORAGE PC_RLY;
extern Menu_Item_t MENU_ITEM_STORAGE Channel;
extern Menu_Item_t MENU_ITEM_STORAGE Number;
extern Menu_Item_t MENU_ITEM_STORAGE CCT_ccNumber;
extern Menu_Item_t MENU_ITEM_STORAGE CCT_ccFreezeNumber;
extern Menu_Item_t MENU_ITEM_STORAGE initState;
extern Menu_Item_t MENU_ITEM_STORAGE iaState;
extern Menu_Item_t MENU_ITEM_STORAGE CCT_RLY;
extern Menu_Item_t MENU_ITEM_STORAGE CCM_ctrlLsbNumber;
extern Menu_Item_t MENU_ITEM_STORAGE CCM_RLY;
extern Menu_Item_t MENU_ITEM_STORAGE CCC_ctrlLsbNumber;
extern Menu_Item_t MENU_ITEM_STORAGE CCC_RLY;
extern Menu_Item_t MENU_ITEM_STORAGE NRPN_TOGGLE_ctrlNumber;
extern Menu_Item_t MENU_ITEM_STORAGE NRPN_TOGGLE_RLY;
extern Menu_Item_t MENU_ITEM_STORAGE NRPN_MOMENTARY_ctrlNumber;
extern Menu_Item_t MENU_ITEM_STORAGE NRPN_MOMENTARY_RLY;
extern Menu_Item_t MENU_ITEM_STORAGE NRPN_CONST_ctrlNumber;
extern Menu_Item_t MENU_ITEM_STORAGE NRPN_CONST_RLY;
extern Menu_Item_t MENU_ITEM_STORAGE noteNumber;
extern Menu_Item_t MENU_ITEM_STORAGE noteVelocity;
extern Menu_Item_t MENU_ITEM_STORAGE bankNumber;
extern Menu_Item_t MENU_ITEM_STORAGE KeyLoadBank;

// peters adjunctions
extern Menu_Item_t MENU_ITEM_STORAGE Send_Dump;
extern Menu_Item_t MENU_ITEM_STORAGE Dumping;
#endif /* MENUITEMS_H_ */