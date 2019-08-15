/*
 * dump.c
 *
 * Created: 15.05.2019 10:03:49
 *  Author: peter
 */ 
#include <string.h>
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
 
// признаки настройки - глобальный или банка
static const uint8_t GLOBAL_SETTINGS	= 0;
static const uint8_t BANK_SETTINGS		= 1;

/*
	uint8_t bnkNum;//“екущий номер банка. ¬ настройках он мен€етс€ когда выбираем банк дл€ сохранени€/загрузки( Banks )
	uint8_t midiChanNum;//System Setup -> MIDI channel. ƒиапазон от 0 до 15, на экране отображаетс€ от 1 до 16
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
	HoldTime buttonHoldTime;//Exp&Tap&Tune -> BUT hold time. ƒиапазон от 1 до 15, в меню отобржажаетс€ как 100ms, 200ms, ..., 1500ms
	TapDisplayType tapDisplayType;//Exp&Tap&Tune -> Tap display
	TapType tapType;//Exp&Tap&Tune -> Tap type
	PedalLedView pedalLedView;//Pedal view -> Display type
	PedalTunerScheme pedalTunerScheme;//Pedal view -> Tuner scheme. “ут пока только два значени€ 0 и 1. ќтобразить можно как Scheme 1 и  Scheme 2
	uint8_t pedalBrightness;//Pedal view -> BRIGHTNESS. яркость светодидов педали. отображаем от 1 до 10
	//This setting are not mapped into GUI, but is stores in global settings during pedals calibration
	uint8_t pLowPos[3];
	uint8_t pHighPos[3];
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///									G L O B A L S		S E T T I N G S
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
bool getDataAndLenghtGS(uint8_t paramID, uint16_t* pInputDataLen, uint8_t** pInputData, const GlobalSettings * pGlobals ){

	if ( NULL == pInputDataLen ) return(false);
	if ( NULL == pInputData ) return(false);
	if ( NULL == pGlobals ) return(false);

	switch ( paramID ){
		case GS_ID_bnkNum:
			*pInputDataLen	= sizeof(pGlobals->bnkNum);
			*pInputData		= (uint8_t *)&(pGlobals->bnkNum);
			break;
		case GS_ID_midiChanNum:
			*pInputDataLen	= sizeof(pGlobals->midiChanNum);
			*pInputData		= (uint8_t *)&(pGlobals->midiChanNum);
			break;
		case GS_ID_useBankSelectMess:
			*pInputDataLen	= sizeof(pGlobals->useBankSelectMess);
			*pInputData		= (uint8_t *)&(pGlobals->useBankSelectMess);
			break;
		case GS_ID_bankSelectMessType:
			*pInputDataLen	= sizeof(pGlobals->bankSelectMessType);
			*pInputData		= (uint8_t *)&(pGlobals->bankSelectMessType);
			break;
		case GS_ID_bnkSwOnBoard:
			*pInputDataLen	= sizeof(pGlobals->bnkSwOnBoard);
			*pInputData		= (uint8_t *)&(pGlobals->bnkSwOnBoard);
			break;
		case GS_ID_showPresetBank:
			*pInputDataLen	= sizeof(pGlobals->Show_pr_name);
			*pInputData		= (uint8_t *)&(pGlobals->Show_pr_name);
			break;
		case GS_ID_targetDevice:
			*pInputDataLen	= sizeof(pGlobals->targetDevice);
			*pInputData		= (uint8_t *)&(pGlobals->targetDevice);
			break;
		case GS_ID_usbBaudrate:
			*pInputDataLen	= sizeof(pGlobals->usbBaudrate);
			*pInputData		= (uint8_t *)&(pGlobals->usbBaudrate);
			break;
		case GS_ID_inputThrough:
			*pInputDataLen	= sizeof(pGlobals->inputThrough);
			*pInputData		= (uint8_t *)&(pGlobals->inputThrough);
			break;
		case GS_ID_maxBankNumber:
			*pInputDataLen	= sizeof(pGlobals->maxBankNumber);
			*pInputData		= (uint8_t *)&(pGlobals->maxBankNumber);
			break;
		case GS_ID_screenBrightness:
			*pInputDataLen	= sizeof(pGlobals->screenBrightness);
			*pInputData		= (uint8_t *)&(pGlobals->screenBrightness);
			break;
		case GS_ID_screenContrast:
			*pInputDataLen	= sizeof(pGlobals->screenContrast);
			*pInputData		= (uint8_t *)&(pGlobals->screenContrast);
			break;
		case GS_ID_expPedalType:
			*pInputDataLen	= sizeof(pGlobals->expPtype);
			*pInputData		= (uint8_t *)&(pGlobals->expPtype);
			break;
		case GS_ID_buttonHoldTime:
			*pInputDataLen	= sizeof(pGlobals->buttonHoldTime);
			*pInputData		= (uint8_t *)&(pGlobals->buttonHoldTime);
			break;
		case GS_ID_tapDisplayType:
			*pInputDataLen	= sizeof(pGlobals->tapDisplayType);
			*pInputData		= (uint8_t *)&(pGlobals->tapDisplayType);
			break;
		case GS_ID_tapType:
			*pInputDataLen	= sizeof(pGlobals->tapType);
			*pInputData		= (uint8_t *)&(pGlobals->tapType);
			break;
		case GS_ID_pedalLedView:
			*pInputDataLen	= sizeof(pGlobals->pedalLedView);
			*pInputData		= (uint8_t *)&(pGlobals->pedalLedView);
			break;
		case GS_ID_pedalTunerScheme:
			*pInputDataLen	= sizeof(pGlobals->pedalTunerScheme);
			*pInputData		= (uint8_t *)&(pGlobals->pedalTunerScheme);
			break;
		case GS_ID_pedalBrightness:
			*pInputDataLen	= sizeof(pGlobals->pedalBrightness);
			*pInputData		= (uint8_t *)&(pGlobals->pedalBrightness);
			break;
		case GS_ID_pedalsCalibrationLo:
			*pInputDataLen	= sizeof(pGlobals->pLowPos);
			*pInputData		= (uint8_t *)&(pGlobals->pLowPos);
			break;
		case GS_ID_pedalsCalibrationHi:
			*pInputDataLen	= sizeof(pGlobals->pHighPos);
			*pInputData		= (uint8_t *)&(pGlobals->pHighPos);
			break;
		default:
			return(false);
	}
	return(true);
}

static inline
void save_GS_param( const uint8_t paramID, const GlobalSettings * pGlobals ){
	if ( NULL == pGlobals ) return;
	/*  є по пор€дку
		0 - номер устройства в сети
		1 - код модели устройства от производител€
		2 - признак глобальной настройки = 0
		3 - paramID
		4 - encoded data
	*/
	uint16_t	midiMsgLen		= 4;	//midi msg length, init value = header size
	uint16_t	inputDataLen	= 0;	//input data length in bytes
	uint8_t *	pInputData		= NULL;	//input data address

	if ( false==getDataAndLenghtGS( paramID, &inputDataLen, &pInputData, pGlobals )) return;

	midiMsgLen += inputDataLen * 2;

	uint8_t midiMsg[midiMsgLen];
	memset(midiMsg, 0, (size_t)midiMsgLen);

	uint16_t offset = 0;

	midiMsg[offset++] = NETWORK_NUMBER;
	midiMsg[offset++] = MODEL_NUMBER;
	midiMsg[offset++] = GLOBAL_SETTINGS;
	midiMsg[offset++] = paramID;

	for( uint16_t i=0; i < inputDataLen; i++ ){
		uint16_t byte = *(pInputData + i);
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

	if (!(2 > length)) return (false);

	// берем адрес данных
	uint16_t	inputDataLen	= 0;	//input data length in bytes
	uint8_t *	pInputData		= NULL;	//input data address

	if (false==getDataAndLenghtGS( paramID, &inputDataLen, &pInputData, pGlobals )) return(false);

	uint16_t		counter = 0;
	State_t			state	= GLOB_SETS;
	bool			_do_	= true;

	while(1) {
		switch( state ){
			case GLOB_SETS:
				if ( GLOBAL_SETTINGS != pData[counter] ) return (false);
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
	for ( uint16_t i=0; i<inputDataLen; i++ ){
		uint8_t byte = 0;
		if (!decodeByte( &pData[counter], &byte )) return(false);
		*(pInputData + i) = byte;
		counter+=2;
	}

	// write to eeprom
	//if (checkEepromHeader()) {
	WriteEEPROM((uint8_t*)pGlobals, GlobalSettings_ADDR, sizeof(GlobalSettings));
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
#define BS_ID_tapCC					0
#define BS_ID_pedalsCC				1
#define BS_ID_tunerCC				2
#define BS_ID_buttonType			3
#define BS_ID_buttonContext			4
#define BS_ID_bankName				5

bool getDataAndLenghtBS(uint8_t paramID, uint16_t* pInputDataLen, uint8_t** pInputData, const BankSettings * pBank ){
	switch ( paramID ){
		case BS_ID_tapCC:
			*pInputDataLen	= sizeof(pBank->tapCc);
			*pInputData = (uint8_t*)&(pBank->tapCc);
			break;
		case BS_ID_pedalsCC:
			*pInputDataLen	= sizeof(pBank->pedalsCc);
			*pInputData = (uint8_t*)&(pBank->pedalsCc);
			break;
		case BS_ID_tunerCC:
			*pInputDataLen	= sizeof(pBank->tunerCc);
			*pInputData = (uint8_t*)&(pBank->tunerCc);
			break;
		case BS_ID_buttonType:
			*pInputDataLen	= sizeof(pBank->buttonType);
			*pInputData = (uint8_t*)&(pBank->buttonType);
			break;
		case BS_ID_buttonContext:
			*pInputDataLen	= sizeof(pBank->buttonContext);
			*pInputData = (uint8_t*)&(pBank->buttonContext);
			break;
		case BS_ID_bankName:
			*pInputDataLen	= sizeof(pBank->BankName);
			*pInputData = (uint8_t*)&(pBank->BankName);
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

	/*  є по пор€дку
		0 - номер устройства в сети
		1 - код модели устройства от производител€
		2 - признак настройки банка = 1
		3 - номер банка bankNumber
		4 - ID= BS_ID_...
		... data
	*/

	uint16_t	midiMsgLen		= 5;	//midi msg length, init value = header size
	uint16_t	inputDataLen	= 0;	//input data length in bytes
	uint8_t *	pInputData		= NULL;	//input data address

	if (false==getDataAndLenghtBS( paramID, &inputDataLen, &pInputData, pBank )) return;

	midiMsgLen += inputDataLen * 2;

	uint8_t midiMsg[midiMsgLen];
	memset(midiMsg, 0, (size_t)midiMsgLen);

	uint16_t offset = 0;

	midiMsg[offset++] = NETWORK_NUMBER;
	midiMsg[offset++] = MODEL_NUMBER;
	midiMsg[offset++] = BANK_SETTINGS;
	midiMsg[offset++] = bankNumber;
	midiMsg[offset++] = paramID;

	for( uint16_t i=0; i < inputDataLen; i++ ){
		uint8_t byte = *(pInputData + i);
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

	if ( NULL == pData ) return(false);
	if ( NULL == pGlobals) return(false);
	if (!(2 > length)) return (false);

	uint16_t		counter = 0;
	State_t			state	= BANK_SETS;
	bool			_do_	= true;
	uint8_t			bankNumber	= 0;

	while(1) {
		switch( state ){
			case BANK_SETS:
				if ( BANK_SETTINGS != pData[counter] ) return (false);
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

	if( bankNumber == runtimeEnvironment.activeBankNumber_ ){
		BankSettings *	pBank		= &bank;
		uint16_t		_counter	= counter;

		// берем адрес данных
		uint16_t	inputDataLen	= 0;	//input data length in bytes
		uint8_t *	pInputData		= NULL;	//input data address

		if (false==getDataAndLenghtBS( paramID, &inputDataLen, &pInputData, pBank )) return(false);
		// декодирование
		for ( uint16_t i=0; i<inputDataLen; i++ ){
			uint8_t byte = 0;
			if (!decodeByte( &pData[_counter], &byte )) return(false);
			*(pInputData + i) = byte;
			_counter+=2;
		}
	}

	BankSettings bank;
	ReadEEPROM((uint8_t*)&bank,		BankSettings_ADDR(bankNumber), sizeof(BankSettings));
	// берем адрес данных
	uint16_t	inputDataLen	= 0;	//input data length in bytes
	uint8_t *	pInputData		= NULL;	//input data address

	if (false==getDataAndLenghtBS( paramID, &inputDataLen, &pInputData, &bank )) return(false);
	// декодирование
	for ( uint16_t i=0; i<inputDataLen; i++ ){
		uint8_t byte = 0;
		if (!decodeByte( &pData[counter], &byte )) return(false);
		*(pInputData + i) = byte;
		counter+=2;
	}
	WriteEEPROM((uint8_t*)&bank,	BankSettings_ADDR(bankNumber), sizeof(BankSettings));

	return (true);
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
		GlobalSettings globals;
		ReadEEPROM((uint8_t*)&globals, GlobalSettings_ADDR, sizeof(GlobalSettings));
		if(globals.maxBankNumber > BanksCount)//crutch
			globals.maxBankNumber = BanksCount;

		save_GS_param( GS_ID_bnkNum, &globals );
		save_GS_param( GS_ID_midiChanNum, &globals );
		save_GS_param( GS_ID_useBankSelectMess, &globals );
		save_GS_param( GS_ID_bankSelectMessType, &globals);
		save_GS_param( GS_ID_bnkSwOnBoard, &globals);
		save_GS_param( GS_ID_showPresetBank, &globals);
		save_GS_param( GS_ID_targetDevice, &globals);
		save_GS_param( GS_ID_usbBaudrate, &globals);
		save_GS_param( GS_ID_inputThrough, &globals);
		save_GS_param( GS_ID_maxBankNumber, &globals);
		save_GS_param( GS_ID_screenBrightness, &globals);
		save_GS_param( GS_ID_screenContrast, &globals);
		save_GS_param( GS_ID_expPedalType, &globals);
		save_GS_param( GS_ID_buttonHoldTime, &globals);
		save_GS_param( GS_ID_tapDisplayType, &globals);
		save_GS_param( GS_ID_tapType, &globals);
		save_GS_param( GS_ID_pedalLedView, &globals);
		save_GS_param( GS_ID_pedalTunerScheme, &globals);
		save_GS_param( GS_ID_pedalBrightness, &globals);
		save_GS_param( GS_ID_pedalsCalibrationLo, &globals);
		save_GS_param( GS_ID_pedalsCalibrationHi, &globals);

		for( uint8_t i=0; i < BanksCount; i++ ){
			BankSettings bank;
			ReadEEPROM((uint8_t*)&bank, BankSettings_ADDR(i), sizeof(BankSettings));

			save_BS_param( BS_ID_tapCC, i, &bank );
			save_BS_param( BS_ID_pedalsCC, i, &bank );
			save_BS_param( BS_ID_tunerCC, i, &bank );
			save_BS_param( BS_ID_buttonType, i, &bank );
			save_BS_param( BS_ID_buttonContext, i, &bank );
			save_BS_param( BS_ID_bankName, i, &bank );
		}			
	//}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///					H A N D L E		S Y S E X 	S E T T I N G S		M E S S A G E S
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool handleMidiSysExSettings( uint8_t midiMsgType, uint8_t * midiMsg, uint16_t midiMsgLength, uint16_t midiBuffLength ){

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
	// decode globals messages

	if (load_GS_param( GS_ID_bnkNum,				&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_midiChanNum,			&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_useBankSelectMess,		&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_bankSelectMessType,	&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_bnkSwOnBoard,			&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_showPresetBank,		&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_targetDevice,			&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_usbBaudrate,			&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_inputThrough,			&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_maxBankNumber,			&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_screenBrightness,		&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_screenContrast,		&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_expPedalType,			&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_buttonHoldTime,		&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_tapDisplayType,		&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_tapType,				&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_pedalLedView,			&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_pedalTunerScheme,		&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_pedalBrightness,		&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_pedalsCalibrationLo,	&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);
	if (load_GS_param( GS_ID_pedalsCalibrationHi,	&midiMsg[startPayloadPos], lengthPayload,  &global )) return (true);

	// decode banks messages
	//load_BS_param( const uint8_t paramID, uint8_t * pData, uint16_t length, GlobalSettings * pGlobals  )
	if (load_BS_param( BS_ID_tapCC,			&midiMsg[startPayloadPos], lengthPayload, &global )) return (true);
	if (load_BS_param( BS_ID_pedalsCC,		&midiMsg[startPayloadPos], lengthPayload, &global )) return (true);
	if (load_BS_param( BS_ID_tunerCC,		&midiMsg[startPayloadPos], lengthPayload, &global )) return (true);
	if (load_BS_param( BS_ID_buttonType,	&midiMsg[startPayloadPos], lengthPayload, &global )) return (true);
	if (load_BS_param( BS_ID_buttonContext,	&midiMsg[startPayloadPos], lengthPayload, &global )) return (true);
	if (load_BS_param( BS_ID_bankName,		&midiMsg[startPayloadPos], lengthPayload, &global )) return (true);


	return (false);
}