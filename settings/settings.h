/*
 * settings.h
 *
 * Created: 19.03.2017 13:17:26
 *  Author: 123
 */ 


#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "bjdevlib_tb.h"
#include <stdint.h>
#include <avr/pgmspace.h>

/*Общее количество MIDI интерфейсов на контроллере 2. MIDI разъемы и USB*/
#define TOTAL_MIDI_INTERFACES 2

//Relay action type masks. 2 bit mask for 4 relays packed in uint8_t
#define RELAY_MASK_NO_ACTION	0x00
#define RELAY_MASK_CLOSE		0x01
#define RELAY_MASK_OPEN			0x02
#define RELAY_MASK_MOMENTARY	0x03


typedef enum UseBankSelectMess
{
	USE_BANK_SELECT = 0,
	DONT_USE_BANK_SELECT
} UseBankSelectMess;

typedef enum
{
	MSB = 0,
	LSB,
	MSB_AND_LSB
} BankSelectMessType;

typedef enum
{
	NO_EXT_PEDAL = 0,
	EXT_PEDAL_BANK_SWITCH,
	EXT_PEDAL_PRESET_SWITCH,
} ExternalBs2Pedal;

typedef enum ShowPresetBankEnum
{
	ONLY_PRESET = 0,
	ONLY_BANK,
	PRESET_AND_BANK
} ShowPresetBank;

typedef enum
{
	LINEAL = 0,
	LOGARITHMIC,
	BACK_LOGARITHMIC
} ExpPedalType;

typedef enum
{
	TARGET_DEVICE_AUTO = 0,
	TARGET_DEVICE_OFF,
	TARGET_DEVICE_AXE_ST,
	TARGET_DEVICE_AXE_ULTRA,
	TARGET_DEVICE_AX8,
	TARGET_DEVICE_AXEII,
	TARGET_DEVICE_AXEII_XL,
	TARGET_DEVICE_AXEII_XL_PLUS,
	TARGET_DEVICE_AXEIII,
	TARGET_DEVICE_KPA,
	TARGET_DEVICE_G_MAJOR,
} TargetDevice;

typedef enum
{
	MIDI_BAUD = 0,
	CUSTOM_BAUD_57600
} UsbBaudrate;

typedef enum 
{
	NOT_SHOW_TAP = 0,
	ON_SCREEN,
	ON_ACTIVE_PRESET,
	LED1,
	LED2,
	LED3,
	LED4,
	LED5,
	LED6,
	LED7,
	LED8,
	LED9,
	LED10,
	LED11,
	LED12
} TapDisplayType;

typedef enum 
{
	TAP_TOGGLE = 0,
	TAP_MOMETRARY,
	TAP_CONST_VALUE
} TapType;

typedef enum 
{
	IN_TO_NONE = 0,//Don't send input to any out
	IN_TO_MIDI_OUT,//Send input to midi output
	IN_TO_USB_OUT,//Send input to usb output
	IN_TO_MIDI_AND_USB_OUT//Send input both to midi and to usb output
} InputThrough;

typedef enum 
{
	PEDAL_LED_MODE_BUTTON = 0,
	PEDAL_LED_MODE_TUNER,
	PEDAL_LED_MODE_DUAL

} PedalLedView;

typedef enum
{
	ms100 = 0,
	ms200,
	ms300,
	ms400,
	ms500,
	ms600,
	ms700,
	ms800,
	ms900,
	ms1000,
	ms1100,
	ms1200,
	ms1300,
	ms1400,
	ms1500,
	ms1600,
	ms1700,
	ms1800,
	ms1900,
	ms2000
} HoldTime;

typedef enum
{
	Scheme1 = 0,
	Scheme2
} PedalTunerScheme;


/*Save firmware version in eeprom.
* After power up we will compare this version with define in code,
* if versions are differ or crc wrong, set all settings to default and write correct version to eeprom	
*/
typedef struct
{
	char version[8];//version format : "3.0.0pb" or "3.0.0", or "3.0" - both correct
	uint8_t checkSumm;//version[0] + version[1] + ... + version[7]	
}FirmwareVersionInfoInEeprom;
#define FirmwareVersionInfoInEeprom_ADDR 0x00
 
/* Размер структуры глобальных настроек 18 байт*/
typedef struct
{
	//TODO move bnkNum to runtimeEnvironment!! 
	uint8_t bnkNum;//Текущий номер банка. При загрузке контроллера переписываем это значение в runtime runtimeEnvironment. После каждого переключения банка перезаписываем это значение чтобы потом стартануть с него
	uint8_t midiChanNum;//System Setup -> MIDI channel. Диапазон от 0 до 15, на экране отображается от 1 до 16
	UseBankSelectMess useBankSelectMess;//System Setup -> Prg. ch. mode
	BankSelectMessType bankSelectMessType;//System Setup -> Bnk. Sel mode
	ExternalBs2Pedal bnkSwOnBoard;//System Setup -> Bank sw. mode
	ShowPresetBank Show_pr_name;//System Setup -> Show pr. name
	TargetDevice targetDevice;//System Setup -> Target device
	UsbBaudrate usbBaudrate;//System Setup -> USB baudrate
	InputThrough inputThrough[TOTAL_MIDI_INTERFACES]; //System Setup ->MIDI thru map
	uint8_t maxBankNumber;//System Setup ->Max. bank
	uint8_t screenBrightness;//System Setup -> Screen brightness. отображаем от 1 до 10
	uint8_t screenContrast;//System Setup ->Screen contrast. от 0 до 255
	ExpPedalType expPtype[3];//Exp&Tap&Tune -> Exp. P1(2) type.  
	HoldTime buttonHoldTime;//Exp&Tap&Tune -> BUT hold time. Диапазон от 1 до 20, в меню отобржажается как 100ms, 200ms, ..., 2000ms
	TapDisplayType tapDisplayType;//Exp&Tap&Tune -> Tap display
	TapType tapType;//Exp&Tap&Tune -> Tap type
	PedalLedView pedalLedView;//Pedal view -> Display type
	PedalTunerScheme pedalTunerScheme;//Pedal view -> Tuner scheme. Тут пока только два значения 0 и 1. Отобразить можно как Scheme 1 и  Scheme 2
	uint8_t pedalBrightness;//Pedal view -> BRIGHTNESS. Яркость светодидов педали. отображаем от 1 до 10
	//This setting are not mapped into GUI, but is stores in global settings during pedals calibration
	uint8_t pLowPos[3];
	uint8_t pHighPos[3];
}GlobalSettings;
#define GlobalSettings_ADDR (FirmwareVersionInfoInEeprom_ADDR + sizeof(FirmwareVersionInfoInEeprom))

//Это тип номера, который передается в Control Change сообщении.
//Валидные значения от 0 до 127, будем считать что 128 это значение типа "OFF" или "NO"
typedef uint8_t CtrlChangeNum;

/*Типы кнопок*/
typedef enum 
{
	PRESET_CHANGE = 0,
	CC_TOGGLE,
	CC_MOMENTARY,
	CC_CONST_VAL,
	NRPN_TOGGLE,
	NRPN_MOMENTARY,
	NRPN_CONST_VAL,
	NOTE_ON,
	NOTE_OFF,
	PRESET_UP,
	PRESET_DOWN,
	BANK_UP,
	BANK_DOWN,
	BANK_TO,
	BUTTON_TYPE_TOTAL_ELEMENTS
} ButtonType;

#define MAX_PROGRAMS_TO_SEND 4
#define iaState_COUNT	(FOOT_BUTTONS_NUM-1)
//TODO Check what is this
#define MAX_BANK_SELECT_MESSAGE_VALUE 7
typedef struct
{
	uint8_t midiChannelNumbers[MAX_PROGRAMS_TO_SEND];
	uint16_t programsNumbers[MAX_PROGRAMS_TO_SEND];//14 bit??
	uint8_t banksNumbers[MAX_PROGRAMS_TO_SEND];
	uint16_t iaState;//14 bit	
}PresetChangeContext;

#define VENDOR_BLOCK_ID_NONE 255 
typedef struct
{
	uint8_t ctrlMsbFreezeNumber;//MSB num for MRPN message or CC num for 
    uint8_t ctrlLsbNumber;//CC num for CC message
    uint8_t paramMsbOnValue;
    uint8_t paramLsbOnValue;//CC val for CC message
    uint8_t paramMsbOffValue;
    uint8_t paramLsbOffValue;//CC val for CC message
	uint8_t autoSendState;
	uint8_t vendorBlockId;//custom number from vendor SysEx, attach to this button as IA 
} ContolAndNrpnChangeContext;//This is using for all CC and NRPN button types

typedef struct
{
	uint8_t noteNumber;
	uint8_t noteVelocity;
} NoteOnConstContext;

typedef uint8_t BankNumber;

typedef union commonContext
{
	NoteOnConstContext			noteOnConstContext_;
	ContolAndNrpnChangeContext  contolAndNrpnChangeContext_;
	BankNumber					bankNumber;//TODO check same name at ButtonContext
} CommonContext;

#define BUTTON_NAME_MAX_SIZE	17
typedef struct buttonContext
{
	uint8_t					relays;//Used in most type of buttons, so let it be always available 
	PresetChangeContext		presetChangeContext;
	CommonContext			commonContext;
	BankNumber				bankNumber;//for "BANK TO" type
	char					nameAlias[BUTTON_NAME_MAX_SIZE];//some text data
} ButtonContext;

#define BANK_NAME_NMAX_SIZE	13
#define FIRST_EXT_PEDAL 0
#define SECOND_EXT_PEDAL 1

#define ONBOARD_PEDAL 2
#define ONBOARD_PEDAL_ALT 3//same physical pedal as ONBOARD_PEDAL
 
typedef enum
{
	SELECT_BANK_NO_ACTION = 0,
	SELECT_BANK_ACTION_SEND_PC 
} SelectBankAction;

typedef struct
{
	CtrlChangeNum tapCc;
	CtrlChangeNum pedalsCc[MAX_PHY_PEDALS+1];//0,1 - external pedals, 2 - onbpard pedal, 3 - onboard pedal alternate number
	CtrlChangeNum tunerCc;

	ButtonType buttonType[FOOT_BUTTONS_NUM];
	ButtonContext buttonContext[FOOT_BUTTONS_NUM];
	
	char BankName[BANK_NAME_NMAX_SIZE];
	SelectBankAction selectBankAction;
	uint8_t selectBankActionProgNum;
} BankSettings;

/*Адресация банков начинается с 0. Отображается для пользователя с 1*/
#define BankSettings_ADDR(n) (uint32_t)(((uint32_t)GlobalSettings_ADDR + (uint32_t)sizeof(GlobalSettings)) + (uint32_t)((uint32_t)sizeof(BankSettings) * (uint32_t)n))

void InitializeGlobalSettings(void);
void setDefaultGlobalSettings(void);

/*
 * Read eeprom header with firmware version, check this version match current version, check crc.
 * If everything is OK  - return true 
 */
//bool checkEepromHeader(uint8_t* validCheckSumm);

void startupSettingsCheckAndLoad();

/*
 * Save bank to eeprom
 */
void SaveBank(uint8_t BankNumber, BankSettings *bnk);

/*
 * Load bank to eeprom
 */
void LoadBank(uint8_t BankNumber, BankSettings *bnk);

/*
 * Load default global settings and banks to eeprom
 */
void loadDefaults(uint32_t banksCount);


GlobalSettings global;
BankSettings bank;

//uint16_t BanksCount;

#endif /* SETTINGS_H_ */