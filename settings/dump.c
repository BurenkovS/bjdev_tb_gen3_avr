/*
* dump.c
*
* Created: 15.05.2019 10:03:49
*  Author: peter
*/
#include <string.h>
#include <stddef.h>
#include "dump.h"
#include "midi.h"
#include "eeprom_wrapper.h"
#include "RuntimeEnvironment.h"

// byte[in]	- input byte
// pData[out]	- encoded sequence 2 bytes
static inline
bool encodeByte( uint8_t byte, uint8_t * pData){
	if ( NULL == pData ) return(false);
	*pData		= (byte >> 4 ) & 0x0F;
	*(pData+1)	= byte & 0x0F;
	return( true );
}

// pData[in] - sequence 2 bytes
// byte[out] - decoded byte
static inline
bool decodeByte( uint8_t * pData, uint8_t * byte ){
	uint8_t _data[2] = {0,0};

	if ( NULL == pData )	return(false);
	if ( NULL == byte)		return(false);

	for ( uint8_t i=0; i<2; i++ ){
		if ( pData[i] > 0x7F ) return(false);
		_data[i] = pData[i];
	}
	*byte = ((_data[0]<<4)&0xF0)|(_data[1]&0x0F);
	return(true);
}

#if 0
static inline
bool decode14bits( uint8_t * pData, uint16_t * word ){
	uint16_t _data[2] = {0,0};

	if ( NULL == pData )	return(false);
	if ( NULL == word)		return(false);

	for ( uint8_t i=0; i<2; i++ ){
		if ( pData[i] > 0x7F ) return(false);
		_data[i] = pData[i];
	}
	*word = ((_data[0]<<7)&0x3F80)|(_data[1]&0x007F);
	return(true);
}
#endif

/*
uint8_t bnkNum;//Текущий номер банка. В настройках он меняется когда выбираем банк для сохранения/загрузки( Banks )
uint8_t midiChanNum;//System Setup -> MIDI channel. Диапазон от 0 до 15, на экране отображается от 1 до 16
UseBankSelectMess useBankSelectMess;//System Setup -> Prg. ch. mode
BankSelectMessType bankSelectMessType;//System Setup -> Bnk. Sel mode
BnkSwOnBoard bnkSwOnBoard;//System Setup -> Bank sw. mode
ShowPresetBank Show_pr_name;//System Setup -> Show pr. name
TargetDevice targetDevice;//System Setup -> Target device
UsbBaudrate usbBaudrate;//System Setup -> USB baudrate
InputThrough inputThrough[TOTAL_MIDI_INTERFACES]; //System Setup ->MIDI thru map
uint8_t maxBankNumber;//System Setup ->Max. bank
uint8_t screenBrightness;//System Setup -> Screen brightness. отображаем от 1 до 10
uint8_t screenContrast;//System Setup ->Screen contrast. от 0 до 255
ExpPedalType expPtype[3];//Exp&Tap&Tune -> Exp. P1(2) type.
HoldTime buttonHoldTime;//Exp&Tap&Tune -> BUT hold time. Диапазон от 1 до 15, в меню отобржажается как 100ms, 200ms, ..., 1500ms
TapDisplayType tapDisplayType;//Exp&Tap&Tune -> Tap display
TapType tapType;//Exp&Tap&Tune -> Tap type
PedalLedView pedalLedView;//Pedal view -> Display type
PedalTunerScheme pedalTunerScheme;//Pedal view -> Tuner scheme. Тут пока только два значения 0 и 1. Отобразить можно как Scheme 1 и  Scheme 2
uint8_t pedalBrightness;//Pedal view -> BRIGHTNESS. Яркость светодидов педали. отображаем от 1 до 10
//This setting are not mapped into GUI, but is stores in global settings during pedals calibration
uint8_t pLowPos[3];
uint8_t pHighPos[3];
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///									G L O B A L S		S E T T I N G S
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stddef.h>
#define member_size(type, member) sizeof(((type *)0)->member)

// IDs globals settings

#define GS_ID_bnkNum				0
#define GS_ID_midiChanNum			1
#define GS_ID_useBankSelectMess		2
#define GS_ID_bankSelectMessType	3
#define GS_ID_bnkSwOnBoard			4
#define GS_ID_showPresetBank		5
#define GS_ID_targetDevice			6
#define GS_ID_usbBaudrate			7
#define GS_ID_inputThrough			8
#define GS_ID_maxBankNumber			9
#define GS_ID_screenBrightness		10
#define GS_ID_screenContrast		11
#define GS_ID_expPedalType			12
#define GS_ID_buttonHoldTime		13
#define GS_ID_tapDisplayType		14
#define GS_ID_tapType				15
#define GS_ID_pedalLedView			16
#define GS_ID_pedalTunerScheme		17
#define GS_ID_pedalBrightness		18
#define GS_ID_pedalsCalibrationLo	19
#define GS_ID_pedalsCalibrationHi	20

//static inline
bool getDataAndLenghtGS(uint8_t paramID, uint16_t* pDataLen, uint32_t* pDataOffset ){

	if ( NULL == pDataLen )		return(false);
	if ( NULL == pDataOffset )	return(false);

	switch ( paramID ){
		case GS_ID_bnkNum:
		*pDataLen		= member_size(GlobalSettings, bnkNum);
		*pDataOffset	= offsetof(GlobalSettings, bnkNum);
		break;
		case GS_ID_midiChanNum:
		*pDataLen		= member_size(GlobalSettings, midiChanNum);
		*pDataOffset	= offsetof(GlobalSettings, midiChanNum);
		break;
		case GS_ID_useBankSelectMess:
		*pDataLen		= member_size(GlobalSettings, useBankSelectMess);
		*pDataOffset	= offsetof(GlobalSettings, useBankSelectMess);
		break;
		case GS_ID_bankSelectMessType:
		*pDataLen		= member_size(GlobalSettings, bankSelectMessType);
		*pDataOffset	= offsetof(GlobalSettings, bankSelectMessType);
		break;
		case GS_ID_bnkSwOnBoard:
		*pDataLen		= member_size(GlobalSettings, bnkSwOnBoard);
		*pDataOffset	= offsetof(GlobalSettings, bnkSwOnBoard);
		break;
		case GS_ID_showPresetBank:
		*pDataLen		= member_size(GlobalSettings, Show_pr_name);
		*pDataOffset	= offsetof(GlobalSettings, Show_pr_name);
		break;
		case GS_ID_targetDevice:
		*pDataLen		= member_size(GlobalSettings, targetDevice);
		*pDataOffset	= offsetof(GlobalSettings, targetDevice);
		break;
		case GS_ID_usbBaudrate:
		*pDataLen		= member_size(GlobalSettings, usbBaudrate);
		*pDataOffset	= offsetof(GlobalSettings, usbBaudrate);
		break;
		case GS_ID_inputThrough:
		*pDataLen		= member_size(GlobalSettings, inputThrough);
		*pDataOffset	= offsetof(GlobalSettings, inputThrough);
		break;
		case GS_ID_maxBankNumber:
		*pDataLen		= member_size(GlobalSettings, maxBankNumber);
		*pDataOffset	= offsetof(GlobalSettings, maxBankNumber);
		break;
		case GS_ID_screenBrightness:
		*pDataLen		= member_size(GlobalSettings, screenBrightness);
		*pDataOffset	= offsetof(GlobalSettings, screenBrightness);
		break;
		case GS_ID_screenContrast:
		*pDataLen		= member_size(GlobalSettings, screenContrast);
		*pDataOffset	= offsetof(GlobalSettings, screenContrast);
		break;
		case GS_ID_expPedalType:
		*pDataLen		= member_size(GlobalSettings, expPtype);
		*pDataOffset	= offsetof(GlobalSettings, expPtype);
		break;
		case GS_ID_buttonHoldTime:
		*pDataLen		= member_size(GlobalSettings, buttonHoldTime);
		*pDataOffset	= offsetof(GlobalSettings, buttonHoldTime);
		break;
		case GS_ID_tapDisplayType:
		*pDataLen		= member_size(GlobalSettings, tapDisplayType);
		*pDataOffset	= offsetof(GlobalSettings, tapDisplayType);
		break;
		case GS_ID_tapType:
		*pDataLen		= member_size(GlobalSettings, tapType);
		*pDataOffset	= offsetof(GlobalSettings, tapType);
		break;
		case GS_ID_pedalLedView:
		*pDataLen		= member_size(GlobalSettings, pedalLedView);
		*pDataOffset	= offsetof(GlobalSettings, pedalLedView);
		break;
		case GS_ID_pedalTunerScheme:
		*pDataLen		= member_size(GlobalSettings, pedalTunerScheme);
		*pDataOffset	= offsetof(GlobalSettings, pedalTunerScheme);
		break;
		case GS_ID_pedalBrightness:
		*pDataLen		= member_size(GlobalSettings, pedalBrightness);
		*pDataOffset	= offsetof(GlobalSettings, pedalBrightness);
		break;
		case GS_ID_pedalsCalibrationLo:
		*pDataLen		= member_size(GlobalSettings, pLowPos);
		*pDataOffset	= offsetof(GlobalSettings, pLowPos);
		break;
		case GS_ID_pedalsCalibrationHi:
		*pDataLen		= member_size(GlobalSettings, pHighPos);
		*pDataOffset	= offsetof(GlobalSettings, pHighPos);
		break;
		default:
		return(false);
	}
	return(true);
}

static inline
void save_GS_param( const uint8_t paramID, const GlobalSettings * pGlobals ){
	if ( NULL == pGlobals ) return;
	/*  № по порядку
	0 - номер устройства в сети
	1 - код модели устройства от производителя
	2 - признак глобальной настройки = 0
	3 - paramID
	4 - encoded data
	*/
	uint16_t	midiMsgLen	= 4;	//midi msg length, init value = header size
	uint16_t	DataLen		= 0;	//input data length in bytes
	uint32_t	DataOffset	= 0;	//offset data address in bytes

	if ( false==getDataAndLenghtGS( paramID, &DataLen, &DataOffset )) return;

	midiMsgLen += DataLen * 2;

	uint8_t midiMsg[midiMsgLen];
	memset(midiMsg, 0, (size_t)midiMsgLen);

	uint16_t offset = 0;

	midiMsg[offset++] = NETWORK_NUMBER;
	midiMsg[offset++] = MODEL_NUMBER;
	midiMsg[offset++] = GLOBALS_MSG;
	midiMsg[offset++] = paramID;

	for( uint16_t i=0; i < DataLen; i++ ){
		uint8_t byte = *((uint8_t*)pGlobals + DataOffset + i);
		if (false == encodeByte( byte, &midiMsg[offset] )) return;
		offset+=2;
	}

	midiSendSysExManfId( (uint32_t)MANUFACTURER_ID, midiMsgLen, midiMsg );
}

static inline
bool load_GS_param( const uint8_t paramID, uint8_t * pData, uint16_t length, const GlobalSettings * pGlobals ){
	typedef enum {
		GLOB_SETS = 0,
		GS_ID,
		DATA
	} State_t;

	if ( NULL == pData ) return(false);
	if ( NULL == pGlobals) return(false);

	if (4 > length) return (false);

	// берем адрес данных
	uint16_t	DataLen		= 0; //input data length in bytes
	uint32_t	DataOffset	= 0; //input data address

	if (false==getDataAndLenghtGS( paramID, &DataLen, &DataOffset )) return(false);

	uint16_t		counter = 0;
	State_t			state	= GLOB_SETS;
	bool			_do_	= true;

	while(1) {
		switch( state ){
			case GLOB_SETS:
			if ( GLOBALS_MSG != pData[counter] ) return (false);
			state = GS_ID;
			break;
			case GS_ID:
			if ( paramID != pData[counter] ) return (false);
			state = DATA;
			break;
			case DATA:
			_do_ = false;
			break;

		}
		if ( false == _do_ ) break;
		counter++;
		if ( counter >= length ) return (false);
	}

	// декодирование
	for ( uint16_t i=0; i<DataLen; i++ ){
		uint8_t byte = 0;
		if (!decodeByte( &pData[counter], &byte )) return(false);
		*((uint8_t*)pGlobals + DataOffset + i) = byte;
		counter+=2;
	}

	// write to eeprom
	//if (checkEepromHeader()) {
	WriteEEPROM((uint8_t*)pGlobals + DataOffset, GlobalSettings_ADDR + DataOffset, DataLen);
	//}
	//LOG(SEV_TRACE, "%s tapDisplayType=%d", __FUNCTION__, tapDisplayType );
	return (true);
}

/*
typedef struct
{
CtrlChangeNum tapCc;
CtrlChangeNum pedalsCc[4];//0,1 - external pedals, 2 - onbpard pedal, 2 - onboard pedal alternate number
CtrlChangeNum tunerCc;

ButtonType buttonType[FOOT_BUTTONS_NUM];
ButtonContext buttonContext[FOOT_BUTTONS_NUM];

char BankName[BANK_NAME_NMAX_SIZE];
} BankSettings;
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///									B A N K			S E T T I N G S
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// IDs banks settings
/*
Значения ParamID от 0 до (N<FOOT_BUTTONS_NUM)
соответствуют номеру контекста кнопки для данного банка
*/

#define BS_ID_tapCC					123
#define BS_ID_pedalsCC				124
#define BS_ID_tunerCC				125
#define BS_ID_buttonType			126
#define BS_ID_bankName				127

bool getDataAndLenghtBS(uint8_t paramID, uint16_t* pDataLen, uint32_t* pDataOffset ){
	if ( paramID < FOOT_BUTTONS_NUM ){
		// передается контекс кнопки с № paramID = 0 ... (N <FOOT_BUTTONS_NUM)
		*pDataLen		= member_size(BankSettings, buttonContext[paramID]);
		*pDataOffset	= offsetof(BankSettings, buttonContext[paramID]);
		return(true);
	}
	switch ( paramID ){
		case BS_ID_tapCC:
		*pDataLen		= member_size(BankSettings, tapCc);
		*pDataOffset	= offsetof(BankSettings, tapCc);
		break;
		case BS_ID_pedalsCC:
		*pDataLen		= member_size(BankSettings, pedalsCc);
		*pDataOffset	= offsetof(BankSettings, pedalsCc);
		break;
		case BS_ID_tunerCC:
		*pDataLen		= member_size(BankSettings, tunerCc);
		*pDataOffset	= offsetof(BankSettings, tunerCc);
		break;
		case BS_ID_buttonType:
		*pDataLen		= member_size(BankSettings, buttonType);
		*pDataOffset	= offsetof(BankSettings, buttonType);
		break;
		case BS_ID_bankName:
		*pDataLen		= member_size(BankSettings, BankName);
		*pDataOffset	= offsetof(BankSettings, BankName);
		break;
		default:
		return(false);
	}
	return(true);
}

static inline
void save_BS_param( const uint8_t paramID,  const uint8_t bankNumber, const BankSettings * pBank ){
	if ( NULL == pBank ) return;
	if ( 127 < bankNumber ) return;

	/*  № по порядку
	0 - номер устройства в сети
	1 - код модели устройства от производителя
	2 - признак настройки банка = 1
	3 - номер банка bankNumber
	4 - ID= BS_ID_...
	... data
	*/

	uint16_t	midiMsgLen		= 5;	//midi msg length, init value = header size
	uint16_t	DataLen			= 0;	//input data length in bytes
	uint32_t	DataOffset		= 0;	//data address offset

	if (false==getDataAndLenghtBS( paramID, &DataLen, &DataOffset )) return;

	midiMsgLen += DataLen * 2;

	uint8_t midiMsg[midiMsgLen];
	memset(midiMsg, 0, (size_t)midiMsgLen);

	uint16_t offset = 0;

	midiMsg[offset++] = NETWORK_NUMBER;
	midiMsg[offset++] = MODEL_NUMBER;
	midiMsg[offset++] = BANKS_MSG;
	midiMsg[offset++] = bankNumber;
	midiMsg[offset++] = paramID;

	for( uint16_t i=0; i < DataLen; i++ ){
		uint8_t byte = *((uint8_t*)pBank + DataOffset + i);
		if (false == encodeByte( byte, &midiMsg[offset] )) return;
		offset+=2;
	}

	midiSendSysExManfId( (uint32_t)MANUFACTURER_ID, midiMsgLen, midiMsg );
}

static inline
bool load_BS_param( const uint8_t paramID, uint8_t * pData, uint16_t length, GlobalSettings * pGlobals  ){
	typedef enum {
		BANK_SETS = 0,
		BANK_NUMBER,
		BS_ID,
		DATA
	} State_t;

	if ( NULL == pData )	return(false);
	if ( NULL == pGlobals)	return(false);
	if (5 > length)			return (false);

	uint16_t		counter = 0;
	State_t			state	= BANK_SETS;
	bool			_do_	= true;
	uint8_t			bankNumber	= 0;

	while(1) {
		switch( state ){
			case BANK_SETS:
			if ( BANKS_MSG != pData[counter] ) return (false);
			state = BANK_NUMBER;
			break;
			case BANK_NUMBER:
			if ( 0x7F < pData[counter] ) return (false);
			state = BS_ID;
			bankNumber = pData[counter];
			if ( bankNumber > 127 ) return (false);
			if ( bankNumber > pGlobals->maxBankNumber ) return (false);
			break;
			case BS_ID:
			if ( paramID != pData[counter] ) return (false);
			state = DATA;
			break;
			case DATA:
			_do_ = false;
			break;

		}
		if ( false == _do_ ) break;
		counter++;
		if ( counter >= length ) return (false);
	}

	// берем адрес данных
	uint16_t	DataLen			= 0;	//data length in bytes
	uint32_t	DataOffset		= 0;	//data address offset

	if (false==getDataAndLenghtBS( paramID, &DataLen, &DataOffset )) return(false);

	uint8_t buffer[DataLen];
	memset( buffer, 0, (size_t)DataLen);

	// декодирование
	for ( uint16_t i=0; i<DataLen; i++ ){
		uint8_t byte = 0;
		if (!decodeByte( &pData[counter], &byte )) return(false);
		buffer[i] = byte;
		counter+=2;
	}

	if( bankNumber == runtimeEnvironment.activeBankNumber_ ){
		memcpy( (uint8_t*)((uint8_t*)(&bank) + DataOffset), buffer,  DataLen );
	}

	WriteEEPROM(buffer,	BankSettings_ADDR(bankNumber) + DataOffset, DataLen);
	return (true);
}

static inline
void send_ACK(){
	// отправляем ACK
	/*  № по порядку
	0 - номер устройства в сети
	1 - код модели устройства от производителя
	2 - признак ACK
	*/

	uint16_t midiMsgLen = 3;	//midi ACK length, init value = header size

	uint8_t midiMsg[midiMsgLen];
	memset(midiMsg, 0, (size_t)midiMsgLen);

	uint16_t offset = 0;

	midiMsg[offset++] = NETWORK_NUMBER;
	midiMsg[offset++] = MODEL_NUMBER;
	midiMsg[offset++] = ACK_MSG;

	midiSendSysExManfId( (uint32_t)MANUFACTURER_ID, midiMsgLen, midiMsg );
}

static inline
void send_EOT(){
	// отправляем EOT
	/*  № по порядку
	0 - номер устройства в сети
	1 - код модели устройства от производителя
	2 - признак EOT
	*/

	uint16_t midiMsgLen = 3;	//midi EOT length, init value = header size

	uint8_t midiMsg[midiMsgLen];
	memset(midiMsg, 0, (size_t)midiMsgLen);

	uint16_t offset = 0;

	midiMsg[offset++] = NETWORK_NUMBER;
	midiMsg[offset++] = MODEL_NUMBER;
	midiMsg[offset++] = EOT_MSG;

	midiSendSysExManfId( (uint32_t)MANUFACTURER_ID, midiMsgLen, midiMsg );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///									D U M P		A L L	S E T T I N G S
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void save_All(){

	uint8_t BanksCount;
	//Check external EEPROM is present
	if(eepromPresentStartupCheck()){
		BanksCount = ((TOTAL_EEPROM_SIZE_BYTES - sizeof(FirmwareVersionInfoInEeprom) - sizeof(GlobalSettings))/sizeof(BankSettings));
		if(BanksCount > 128)
		BanksCount = 128;
	}
	else{
		BanksCount = 1;//((INTERNAL_EEPROM_SIZE_BYTES - sizeof(FirmwareVersionInfoInEeprom) - sizeof(GlobalSettings))/sizeof(BankSettings));
	}

	//if (checkEepromHeader()) {
	GlobalSettings _globals;
	ReadEEPROM((uint8_t*)&_globals, GlobalSettings_ADDR, sizeof(GlobalSettings));
	if(_globals.maxBankNumber > BanksCount)//crutch
	_globals.maxBankNumber = BanksCount;

	save_GS_param( GS_ID_bnkNum,				&_globals);
	save_GS_param( GS_ID_midiChanNum,			&_globals);
	save_GS_param( GS_ID_useBankSelectMess,		&_globals);
	save_GS_param( GS_ID_bankSelectMessType,	&_globals);
	save_GS_param( GS_ID_bnkSwOnBoard,			&_globals);
	save_GS_param( GS_ID_showPresetBank,		&_globals);
	save_GS_param( GS_ID_targetDevice,			&_globals);
	save_GS_param( GS_ID_usbBaudrate,			&_globals);
	save_GS_param( GS_ID_inputThrough,			&_globals);
	save_GS_param( GS_ID_maxBankNumber,			&_globals);
	save_GS_param( GS_ID_screenBrightness,		&_globals);
	save_GS_param( GS_ID_screenContrast,		&_globals);
	save_GS_param( GS_ID_expPedalType,			&_globals);
	save_GS_param( GS_ID_buttonHoldTime,		&_globals);
	save_GS_param( GS_ID_tapDisplayType,		&_globals);
	save_GS_param( GS_ID_tapType,				&_globals);
	save_GS_param( GS_ID_pedalLedView,			&_globals);
	save_GS_param( GS_ID_pedalTunerScheme,		&_globals);
	save_GS_param( GS_ID_pedalBrightness,		&_globals);
	save_GS_param( GS_ID_pedalsCalibrationLo,	&_globals);
	save_GS_param( GS_ID_pedalsCalibrationHi,	&_globals);

	for( uint8_t i=0; i < BanksCount; i++ ){
		BankSettings _bank;
		ReadEEPROM((uint8_t*)&_bank, BankSettings_ADDR(i), sizeof(BankSettings));

		// сохраняем контекс кнопки № 0 ... N
		for(uint8_t btnNum=0; btnNum<FOOT_BUTTONS_NUM; btnNum++) save_BS_param( btnNum, i, &_bank );

		save_BS_param( BS_ID_tapCC,		i, &_bank );
		save_BS_param( BS_ID_pedalsCC,	i, &_bank );
		save_BS_param( BS_ID_tunerCC,	i, &_bank );
		save_BS_param( BS_ID_buttonType,i, &_bank );
		save_BS_param( BS_ID_bankName,	i, &_bank );
	}
	send_EOT();
	//}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///					H A N D L E		S Y S E X 	S E T T I N G S		M E S S A G E S
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool handleMidiSysExSettings( uint8_t midiMsgType, uint8_t * midiMsg, uint16_t midiMsgLength, uint16_t midiBuffLength ){
	LOG(SEV_TRACE,"%s", __FUNCTION__);
	typedef enum {
		START_SYSEX = 0,
		MANUFACTURER_ID_0,
		MANUFACTURER_ID_1,
		MANUFACTURER_ID_2,
		NETWORK_NUM,
		MODEL_NUM,
		PAYLOAD
	} SysExState_t;

	if (
	( 0 == midiMsgLength )
	||
	( 0 == midiBuffLength )
	||
	( midiBuffLength < midiMsgLength )
	||
	( /*MIDI_SYSEX_END*/MIDI_SYSEX_START != midiMsgType )
	)
	return (false);

	bool			_do_				= true;				// true - do loop while, false - exit from loop
	uint8_t			byte				= 0;
	uint16_t		counter				= 0;
	uint16_t		startPayloadPos		= 0;
	uint16_t		lengthPayload		= 0;
	SysExState_t	state				= START_SYSEX;
	uint32_t		manfId				= MANUFACTURER_ID;

	while ( 1 ){
		switch (state){
			case START_SYSEX:
				if ( MIDI_SYSEX_START != midiMsg[counter] )
					return (false);
				else
					state = MANUFACTURER_ID_0;
				break;
			case MANUFACTURER_ID_0:
				byte = (manfId >> 16) & 0x7F;
				if ( byte != midiMsg[counter] )
					return (false);
				else
					state = MANUFACTURER_ID_1;
				break;
			case MANUFACTURER_ID_1:
				byte = (manfId >> 8) & 0x7F;
				if ( byte != midiMsg[counter] )
					return (false);
				else
					state = MANUFACTURER_ID_2;
				break;
			case MANUFACTURER_ID_2:
				byte = manfId & 0x7F;
				if ( byte != midiMsg[counter] )
					return (false);
				else
					state = NETWORK_NUM;
				break;
			case NETWORK_NUM:
				if ( NETWORK_NUMBER != midiMsg[counter] )
					return (false);
				else
					state = MODEL_NUM;
				break;
			case MODEL_NUM:
				if ( MODEL_NUMBER != midiMsg[counter] )
					return (false);
				else
					state = PAYLOAD;
				break;
			case PAYLOAD:
				if ( MIDI_SYSEX_END == midiMsg[counter] )
					_do_ = false;
				else {
					if ( 0 == startPayloadPos ) startPayloadPos = counter;
					lengthPayload++;
				}
				break;
		}
		if ( false == _do_ ) break;
		counter++;
		if ( counter >= midiMsgLength ) return(false);
	}

	if (0 == lengthPayload) return(false);

	// decode globals messages

	if ( GLOBALS_MSG == midiMsg[startPayloadPos] ){
		if (load_GS_param( GS_ID_bnkNum,				&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_bnkNum");
			return (true);
		}
		if (load_GS_param( GS_ID_midiChanNum,			&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_midiChanNum");
			return (true);
		}
		if (load_GS_param( GS_ID_useBankSelectMess,		&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_useBankSelectMess");
			return (true);
		}
		if (load_GS_param( GS_ID_bankSelectMessType,	&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_bankSelectMessType");
			return (true);
		}
		if (load_GS_param( GS_ID_bnkSwOnBoard,			&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_bnkSwOnBoard");
			return (true);
		}
		if (load_GS_param( GS_ID_showPresetBank,		&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_showPresetBank");
			return (true);
		}
		if (load_GS_param( GS_ID_targetDevice,			&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_targetDevice");
			return (true);
		}
		if (load_GS_param( GS_ID_usbBaudrate,			&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_usbBaudrate");
			return (true);
		}
		if (load_GS_param( GS_ID_inputThrough,			&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_inputThrough");
			return (true);
		}
		if (load_GS_param( GS_ID_maxBankNumber,			&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_maxBankNumber");
			return (true);
		}
		if (load_GS_param( GS_ID_screenBrightness,		&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_screenBrightness");
			return (true);
		}
		if (load_GS_param( GS_ID_screenContrast,		&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_screenContrast");
			return (true);
		}
		if (load_GS_param( GS_ID_expPedalType,			&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_expPedalType");
			return (true);
		}
		if (load_GS_param( GS_ID_buttonHoldTime,		&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_buttonHoldTime");
			return (true);
		}
		if (load_GS_param( GS_ID_tapDisplayType,		&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_tapDisplayType");
			return (true);
		}
		if (load_GS_param( GS_ID_tapType,				&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_tapType");
			return (true);
		}
		if (load_GS_param( GS_ID_pedalLedView,			&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_pedalLedView");
			return (true);
		}
		if (load_GS_param( GS_ID_pedalTunerScheme,		&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_pedalTunerScheme");
			return (true);
		}
		if (load_GS_param( GS_ID_pedalBrightness,		&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_pedalBrightness");
			return (true);
		}
		if (load_GS_param( GS_ID_pedalsCalibrationLo,	&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_pedalsCalibrationLo");
			return (true);
		}
		if (load_GS_param( GS_ID_pedalsCalibrationHi,	&midiMsg[startPayloadPos], lengthPayload,  &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "GS_ID_pedalsCalibrationHi");
			return (true);
		}
	}

	// decode banks messages
	if ( BANKS_MSG == midiMsg[startPayloadPos] ){
		// пытаемся декодировать контекст кнопки
		for(uint8_t btnNum=0; btnNum<FOOT_BUTTONS_NUM; btnNum++){
			if (load_BS_param( btnNum,	&midiMsg[startPayloadPos], lengthPayload, &global )){
				LOG(SEV_TRACE,"%s %s", __FUNCTION__, "BS_ID_ButtonCotext");
				return (true);
			}
		}
		if (load_BS_param( BS_ID_tapCC,			&midiMsg[startPayloadPos], lengthPayload, &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "BS_ID_tapCC");
			return (true);
		}
		if (load_BS_param( BS_ID_pedalsCC,		&midiMsg[startPayloadPos], lengthPayload, &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "BS_ID_pedalsCC");
			return (true);
		}
		if (load_BS_param( BS_ID_tunerCC,		&midiMsg[startPayloadPos], lengthPayload, &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "BS_ID_tunerCC");
			return (true);
		}
		if (load_BS_param( BS_ID_buttonType,	&midiMsg[startPayloadPos], lengthPayload, &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "BS_ID_buttonType");
			return (true);
		}
		if (load_BS_param( BS_ID_bankName,		&midiMsg[startPayloadPos], lengthPayload, &global )){
			LOG(SEV_TRACE,"%s %s", __FUNCTION__, "BS_ID_bankName");
			return (true);
		}
	}
	return (false);
}

bool handleMidiSysEx( uint8_t midiMsgType, uint8_t * midiMsg, uint16_t midiMsgLength, uint16_t midiBuffLength ){
	LOG(SEV_TRACE,"%s", __FUNCTION__);
	bool res = handleMidiSysExSettings( midiMsgType, midiMsg, midiMsgLength, midiBuffLength );
	send_ACK();
	return(res);
}
