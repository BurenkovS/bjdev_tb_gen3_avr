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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///									T R A C E		S E T T I N G S
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
TRACE FUNCTIONS CODE in bits, send as bytes chain in ack message
---------------------------------------------------------------
[7] - allways 0
[6]--------------
[5] Error
[4] number
[3]--------------
[2] 1- success 0-fail
[1]--------------
[0] function ID
*/
/* trace length buffer */
#define TRACE_LENGTH					(90) /* ACK full msg length = 8*/
/* functions IDs */
#define handleMidiSysExSettings_ID		(1)
#define load_GS_param_ID				(2)
#define load_BS_param_ID				(3)
#define load_BS_context_ID				(0)
/* ErrNo for handleMidiSysExSettings */
#define HMS_BAD_FIRST_CHECK				(1)
#define HMS_NO_MIDI_SYSEX_START			(2)
#define HMS_BAD_MANUFACTURER_ID_0		(3)
#define HMS_BAD_MANUFACTURER_ID_1		(4)
#define HMS_BAD_MANUFACTURER_ID_2		(5)
#define HMS_BAD_NETWORK_NUMBER			(6)
#define HMS_BAD_MODEL_NUMBER			(7)
#define HMS_COUNTER_OVERRUN				(8)
#define HMS_NO_PAYLOAD					(9)
#define HMS_NO_DECODE					(10)
/* ErrNo for load_GS_param */
#define LGS_BAD_PDATA					(1)
#define LGS_BAD_PGLOBALS				(2)
#define LGS_BAD_LEN						(3)
#define LGS_BAD_GDL_GS					(4)
#define LGS_BAD_GLOBALS_MSG				(5)
#define LGS_BAD_PARAMS_ID				(6)
#define LGS_COUNTER_OVERRUN				(7)
#define LGS_NO_DECODE_BYTE				(8)
/* ErrNo for load_BS_param */
#define LBS_BAD_PDATA					(1)
#define LBS_BAD_PGLOBALS				(2)
#define LBS_BAD_LEN						(3)
#define LBS_BAD_BANKS_MSG				(4)
#define LBS_BAD_BANK_NUM1				(5)
#define LBS_BAD_BANK_NUM2				(6)
#define LBS_BAD_PARAMS_ID				(7)
#define LBS_COUNTER_OVERRUN				(8)
#define LBS_BAD_GDL_BS					(9)
#define LBS_NO_DECODE_BYTE				(8)
/* ErrNo for load_BS_context */
#define LCX_BAD_PDATA					(1)
#define LCX_BAD_PGLOBALS				(2)
#define LCX_BAD_LEN						(3)
#define LCX_BAD_BANKS_MSG				(4)
#define LCX_BAD_BANK_NUM1				(5)
#define LCX_BAD_BANK_NUM2				(6)
#define LCX_BAD_BTN_NUM1				(7)
#define LCX_BAD_BTN_NUM2				(8)
#define LCX_BAD_CTX_ID					(9)
#define LCX_COUNTER_OVERRUN				(10)
#define LCX_BAD_GDL_BTN					(11)
#define LCX_BAD_GDL_CTX					(12)
#define LCX_NO_DECODE_BYTE				(13)



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
	/*  є по пор€дку
	0 - номер устройства в сети
	1 - код модели устройства от производител€
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
bool load_GS_param( const uint8_t paramID, uint8_t * pData, uint16_t length, const GlobalSettings * pGlobals, uint8_t * trace ){
	typedef enum {
		GLOB_SETS = 0,
		GS_ID,
		DATA
	} State_t;

	uint8_t tcode = load_GS_param_ID; // trace code

	if ( NULL == pData ){
		tcode = tcode | ( LGS_BAD_PDATA << 3 );
		if (trace != NULL) *trace= tcode;
		return(false);
	}
	if ( NULL == pGlobals){
		tcode = tcode | ( LGS_BAD_PGLOBALS << 3 );
		if (trace != NULL) *trace= tcode;
		return(false);
	}

	if (4 > length){
		tcode = tcode | ( LGS_BAD_LEN << 3 );
		if (trace != NULL) *trace= tcode;
		return (false);
	}

	// берем адрес данных
	uint16_t	DataLen		= 0; //input data length in bytes
	uint32_t	DataOffset	= 0; //input data address

	if (false==getDataAndLenghtGS( paramID, &DataLen, &DataOffset )){
		tcode = tcode | ( LGS_BAD_GDL_GS << 3 );
		if (trace != NULL) *trace= tcode;
		return(false);
	}

	uint16_t		counter = 0;
	State_t			state	= GLOB_SETS;
	bool			_do_	= true;

	while(1) {
		switch( state ){
			case GLOB_SETS:
				if ( GLOBALS_MSG != pData[counter] ){
					tcode = tcode | ( LGS_BAD_GLOBALS_MSG << 3 );
					if (trace != NULL) *trace= tcode;
					return (false);
				}
				state = GS_ID;
				break;
			case GS_ID:
				if ( paramID != pData[counter] ){
					tcode = tcode | ( LGS_BAD_PARAMS_ID << 3 );
					if (trace != NULL) *trace= tcode;
					return (false);
				}
				state = DATA;
				break;
			case DATA:
				_do_ = false;
				break;
		}
		if ( false == _do_ ) break;
		counter++;
		if ( counter >= length ){
			tcode = tcode | ( LGS_COUNTER_OVERRUN << 3 );
			if (trace != NULL) *trace= tcode;
			return (false);
		}
	}

	// декодирование
	for ( uint16_t i=0; i<DataLen; i++ ){
		uint8_t byte = 0;
		if (!decodeByte( &pData[counter], &byte )){
			tcode = tcode | ( LGS_NO_DECODE_BYTE << 3 );
			if (trace != NULL) *trace= tcode;
			return(false);
		}
		*((uint8_t*)pGlobals + DataOffset + i) = byte;
		counter+=2;
	}

	// write to eeprom
	WriteEEPROM((uint8_t*)pGlobals + DataOffset, GlobalSettings_ADDR + DataOffset, DataLen);

	//LOG(SEV_TRACE, "%s tapDisplayType=%d", __FUNCTION__, tapDisplayType );
	tcode = tcode | ( 1 << 2 );
	if (trace != NULL) *trace= tcode;
	return (true);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///									B A N K			S E T T I N G S
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// IDs banks settings
/*
«начени€ ParamID от 0 до (N<FOOT_BUTTONS_NUM)
соответствуют номеру контекста кнопки дл€ данного банка
*/

// дл€ контекста
#define BS_ID_CTX_relays				20
#define BS_ID_CTX_presetChangeContext	21
#define BS_ID_CTX_commonContext			22
#define BS_ID_CTX_bankNumber			23
#define BS_ID_CTX_nameAlias				24

// общие
#define BS_ID_selectBankAction			121
#define BS_ID_selectBankActionProgNum	122
#define BS_ID_tapCC						123
#define BS_ID_pedalsCC					124
#define BS_ID_tunerCC					125
#define BS_ID_buttonType				126
#define BS_ID_bankName					127

bool getDataAndLenghtBS(uint8_t paramID, uint16_t* pDataLen, uint32_t* pDataOffset ){
	if ( paramID < FOOT_BUTTONS_NUM ){
		// передаетс€ контекс кнопки с є paramID = 0 ... (N <FOOT_BUTTONS_NUM)
		*pDataLen		= member_size(BankSettings, buttonContext[paramID]);
		*pDataOffset	= offsetof(BankSettings, buttonContext[paramID]);
		return(true);
	}
	switch ( paramID ){
		case BS_ID_CTX_relays:
			*pDataLen		= member_size(ButtonContext, relays);
			*pDataOffset	= offsetof(ButtonContext, relays);
			break;
		case BS_ID_CTX_presetChangeContext:
			*pDataLen		= member_size(ButtonContext, presetChangeContext);
			*pDataOffset	= offsetof(ButtonContext, presetChangeContext);
			break;
		case BS_ID_CTX_commonContext:
			*pDataLen		= member_size(ButtonContext, commonContext);
			*pDataOffset	= offsetof(ButtonContext, commonContext);
			break;
		case BS_ID_CTX_bankNumber:
			*pDataLen		= member_size(ButtonContext, bankNumber);
			*pDataOffset	= offsetof(ButtonContext, bankNumber);
			break;
		case BS_ID_CTX_nameAlias:
			*pDataLen		= member_size(ButtonContext, nameAlias);
			*pDataOffset	= offsetof(ButtonContext, nameAlias);
			break;
		case BS_ID_selectBankAction:
			*pDataLen		= member_size(BankSettings, selectBankAction);
			*pDataOffset	= offsetof(BankSettings, selectBankAction);
			break;
		case BS_ID_selectBankActionProgNum:
			*pDataLen		= member_size(BankSettings, selectBankActionProgNum);
			*pDataOffset	= offsetof(BankSettings, selectBankActionProgNum);
			break;
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
void save_BS_Context( const uint8_t paramCtxID, const uint8_t btnNumber, const uint8_t bankNumber, const BankSettings * pBank ){
	if ( NULL == pBank ) return;
	if ( 127 < bankNumber ) return;

	/*  є по пор€дку
	0 - номер устройства в сети
	1 - код модели устройства от производител€
	2 - признак настройки банка = 1
	3 - номер банка bankNumber
	4 - button Number
	5 - param Context ID
	... data
	*/

	uint16_t	midiMsgLen		= 6;	//midi msg length, init value = header size
	uint16_t	DataLen			= 0;	//input data length in bytes
	uint32_t	DataOffset		= 0;	//data address offset
	uint16_t	DataLenCtx		= 0;	//input data length in bytes
	uint32_t	DataOffsetCtx	= 0;	//data address offset

	if (false==getDataAndLenghtBS( paramCtxID,	&DataLenCtx, &DataOffsetCtx )) return;
	if (false==getDataAndLenghtBS( btnNumber,	&DataLen, &DataOffset )) return;

	DataOffsetCtx+=DataOffset;
	midiMsgLen += DataLenCtx * 2;

	uint8_t midiMsg[midiMsgLen];
	memset(midiMsg, 0, (size_t)midiMsgLen);

	uint16_t offset = 0;

	midiMsg[offset++] = NETWORK_NUMBER;
	midiMsg[offset++] = MODEL_NUMBER;
	midiMsg[offset++] = BANKS_MSG;
	midiMsg[offset++] = bankNumber;
	midiMsg[offset++] = btnNumber;
	midiMsg[offset++] = paramCtxID;

	for( uint16_t i=0; i < DataLenCtx; i++ ){
		uint8_t byte = *((uint8_t*)pBank + DataOffsetCtx + i);
		if (false == encodeByte( byte, &midiMsg[offset] )) return;
		offset+=2;
	}

	midiSendSysExManfId( (uint32_t)MANUFACTURER_ID, midiMsgLen, midiMsg );
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
bool load_BS_context( const uint8_t paramCtxID, const uint8_t btnNumber, uint8_t * pData, uint16_t length, GlobalSettings * pGlobals, uint8_t * trace ){
	/*  є по пор€дку
	0 - номер устройства в сети
	1 - код модели устройства от производител€
	2 - признак настройки банка = 1
	3 - номер банка bankNumber
	4 - button Number
	5 - param Context ID
	... data
	*/

	typedef enum {
		BANK_SETS = 0,
		BANK_NUMBER,
		BTN_NUMBER,
		CTX_ID,
		DATA
	} State_t;

	uint8_t tcode = load_BS_context_ID; // trace code

	if ( NULL == pData ){
		tcode = tcode | (LCX_BAD_PDATA<<3);
		if (NULL!=trace) *trace= tcode;
		return(false);
	}
	if ( NULL == pGlobals){
		tcode = tcode | (LCX_BAD_PGLOBALS<<3);
		if (NULL!=trace) *trace= tcode;
		return(false);
	}
	if (6 > length){
		tcode = tcode | (LCX_BAD_LEN<<3);
		if (NULL!=trace) *trace= tcode;
		return (false);
	}

	uint16_t		counter		= 0;
	State_t			state		= BANK_SETS;
	bool			_do_		= true;
	uint8_t			bankNumber	= 0;

	while(1) {
		switch( state ){
			case BANK_SETS:
				if ( BANKS_MSG != pData[counter] ){
					tcode = tcode | (LCX_BAD_BANKS_MSG<<3);
					if (NULL!=trace) *trace= tcode;
					return (false);
				}
				state = BANK_NUMBER;
				break;
			case BANK_NUMBER:
				if ( 0x7F <  pData[counter] ){
					tcode = tcode | (LCX_BAD_BANK_NUM1<<3);
					if (NULL!=trace) *trace= tcode;
					return (false);
				}
				state = BTN_NUMBER;
				bankNumber = pData[counter];
/*
				if ( bankNumber > pGlobals->maxBankNumber ){
					tcode = tcode | (LCX_BAD_BANK_NUM2<<3);
					*trace= tcode;
					return (false);
				}
*/
				break;
			case BTN_NUMBER:
				if ( !(FOOT_BUTTONS_NUM > pData[counter]) ){
					tcode = tcode | (LCX_BAD_BTN_NUM1<<3);
					if (NULL!=trace) *trace= tcode;
					return (false);
				}
				if ( btnNumber != pData[counter] ){
					tcode = tcode | (LCX_BAD_BTN_NUM2<<3);
					if (NULL!=trace) *trace= tcode;
					return (false);
				}
				state = CTX_ID;
				break;
			case CTX_ID:
				if ( paramCtxID!=pData[counter] ) {
					tcode = tcode | (LCX_BAD_CTX_ID<<3);
					if (NULL!=trace) *trace= tcode;
					return(false);
				}
				state = DATA;
				break;
			case DATA:
				_do_ = false;
				break;
		}
		if ( false == _do_ ) break;
		counter++;
		if ( counter >= length ){
			tcode = tcode | (LCX_COUNTER_OVERRUN<<3);
			if (NULL!=trace) *trace= tcode;
			return (false);
		}
	}

	// берем адрес данных
	uint16_t	DataLen			= 0;	//data length in bytes
	uint32_t	DataOffset		= 0;	//data address offset
	uint16_t	DataLenCtx		= 0;	//input data length in bytes
	uint32_t	DataOffsetCtx	= 0;	//data address offset

	// button context
	if (false==getDataAndLenghtBS( btnNumber, &DataLen, &DataOffset )){
		tcode = tcode | (LCX_BAD_GDL_BTN<<3);
		if (NULL!=trace) *trace= tcode;
		return(false);
	}
	// context member
	if (false==getDataAndLenghtBS( paramCtxID,	&DataLenCtx, &DataOffsetCtx )){
		tcode = tcode | (LCX_BAD_GDL_CTX<<3);
		if (NULL!=trace) *trace= tcode;
		return(false);
	}

	uint8_t buffer[DataLenCtx];
	memset( buffer, 0, (size_t)DataLenCtx);

	// декодирование
	for ( uint16_t i=0; i<DataLenCtx; i++ ){
		uint8_t byte = 0;
		if (!decodeByte( &pData[counter], &byte )){
			tcode = tcode | (LCX_NO_DECODE_BYTE<<3);
			if (NULL!=trace) *trace= tcode;
			return(false);
		}
		buffer[i] = byte;
		counter+=2;
	}

	if( bankNumber == runtimeEnvironment.activeBankNumber_ ){
		memcpy( (uint8_t*)((uint8_t*)(&bank) + DataOffset + DataOffsetCtx), buffer,  DataLenCtx );
	}

	WriteEEPROM(buffer,	BankSettings_ADDR(bankNumber) + DataOffset  + DataOffsetCtx, DataLenCtx);

	tcode = tcode | (1<<2);
	if (NULL!=trace) *trace= tcode;
	return (true);
}


static inline
bool load_BS_param( const uint8_t paramID, uint8_t * pData, uint16_t length, GlobalSettings * pGlobals, uint8_t * trace  ){
	typedef enum {
		BANK_SETS = 0,
		BANK_NUMBER,
		BS_ID,
		DATA
	} State_t;

	uint8_t tcode = load_BS_param_ID; // trace code

	if ( NULL == pData ){
		tcode = tcode | (LBS_BAD_PDATA<<3);
		if (NULL!=trace) *trace= tcode;
		return(false);
	}
	if ( NULL == pGlobals){
		tcode = tcode | (LBS_BAD_PGLOBALS<<3);
		if (NULL!=trace) *trace= tcode;
		return(false);
	}
	if (5 > length){
		tcode = tcode | (LBS_BAD_LEN<<3);
		if (NULL!=trace) *trace= tcode;
		return (false);
	}

	uint16_t		counter = 0;
	State_t			state	= BANK_SETS;
	bool			_do_	= true;
	uint8_t			bankNumber	= 0;

	while(1) {
		switch( state ){
			case BANK_SETS:
				if ( BANKS_MSG != pData[counter] ){
					tcode = tcode | (LBS_BAD_BANKS_MSG<<3);
					if (NULL!=trace) *trace= tcode;
					return (false);
				}
				state = BANK_NUMBER;
				break;
			case BANK_NUMBER:
				if ( 0x7F < pData[counter] ){
					tcode = tcode | (LBS_BAD_BANK_NUM1<<3);
					if (NULL!=trace) *trace= tcode;
					return (false);
				}
				state = BS_ID;
				bankNumber = pData[counter];
/*
				if ( bankNumber > pGlobals->maxBankNumber ){
					tcode = tcode | (LBS_BAD_BANK_NUM2<<3);
					*trace= tcode;
					return (false);
				}
*/
				break;
			case BS_ID:
				if ( paramID != pData[counter] ){
					tcode = tcode | (LBS_BAD_PARAMS_ID<<3);
					if (NULL!=trace) *trace= tcode;
					return (false);
				}
				state = DATA;
				break;
			case DATA:
				_do_ = false;
				break;
		}
		if ( false == _do_ ) break;
		counter++;
		if ( counter >= length ){
			tcode = tcode | (LBS_COUNTER_OVERRUN<<3);
			if (NULL!=trace) *trace= tcode;
			return (false);
		}
	}

	// берем адрес данных
	uint16_t	DataLen			= 0;	//data length in bytes
	uint32_t	DataOffset		= 0;	//data address offset

	if (false==getDataAndLenghtBS( paramID, &DataLen, &DataOffset )){
		tcode = tcode | (LBS_BAD_GDL_BS<<3);
		if (NULL!=trace) *trace= tcode;
		return(false);
	}

	uint8_t buffer[DataLen];
	memset( buffer, 0, (size_t)DataLen);

	// декодирование
	for ( uint16_t i=0; i<DataLen; i++ ){
		uint8_t byte = 0;
		if (!decodeByte( &pData[counter], &byte )){
			tcode = tcode | (LBS_NO_DECODE_BYTE<<3);
			if (NULL!=trace) *trace= tcode;
			return(false);
		}
		buffer[i] = byte;
		counter+=2;
	}

	if( bankNumber == runtimeEnvironment.activeBankNumber_ ){
		memcpy( (uint8_t*)((uint8_t*)(&bank) + DataOffset), buffer,  DataLen );
	}

	WriteEEPROM(buffer,	BankSettings_ADDR(bankNumber) + DataOffset, DataLen);

	tcode = tcode | (1<<2);
	if (NULL!=trace) *trace= tcode;
	return (true);
}

static inline
void send_ACK(){
	// отправл€ем ACK
	/*  є по пор€дку
	0 - номер устройства в сети
	1 - код модели устройства от производител€
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
void send_ACK_TRACE( uint8_t * trace ){
	// отправл€ем ACK
	/*  є по пор€дку
	0 - номер устройства в сети
	1 - код модели устройства от производител€
	2 - признак ACK
	*/

	uint16_t midiMsgLen = 3 + TRACE_LENGTH;	//midi ACK length, init value = header size

	uint8_t midiMsg[midiMsgLen];
	memset(midiMsg, 0, (size_t)midiMsgLen);

	uint16_t offset = 0;

	midiMsg[offset++] = NETWORK_NUMBER;
	midiMsg[offset++] = MODEL_NUMBER;
	midiMsg[offset++] = ACK_MSG;

	if ( NULL != trace ) memcpy( midiMsg + offset, trace, TRACE_LENGTH );

	midiSendSysExManfId( (uint32_t)MANUFACTURER_ID, midiMsgLen, midiMsg );
}


static inline
void send_EOT(){
	// отправл€ем EOT
	/*  є по пор€дку
	0 - номер устройства в сети
	1 - код модели устройства от производител€
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
	GlobalSettings _globals;
	ReadEEPROM((uint8_t*)&_globals, GlobalSettings_ADDR, sizeof(GlobalSettings));

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

	for( uint8_t i=0; i < runtimeEnvironment.totalBanksAvalible_; i++ ){
		BankSettings _bank;
		ReadEEPROM((uint8_t*)&_bank, BankSettings_ADDR(i), sizeof(BankSettings));

		save_BS_param( BS_ID_tapCC,						i, &_bank );
		save_BS_param( BS_ID_pedalsCC,					i, &_bank );
		save_BS_param( BS_ID_tunerCC,					i, &_bank );
		save_BS_param( BS_ID_buttonType,				i, &_bank );
		save_BS_param( BS_ID_bankName,					i, &_bank );
		save_BS_param( BS_ID_selectBankAction,			i, &_bank );
		save_BS_param( BS_ID_selectBankActionProgNum,	i, &_bank );

		// сохран€ем контекс кнопки є 0 ... N
		//for(uint8_t btnNum=0; btnNum<FOOT_BUTTONS_NUM; btnNum++) save_BS_param( btnNum, i, &_bank );
		for(uint8_t btnNum=0; btnNum<FOOT_BUTTONS_NUM; btnNum++){
			save_BS_Context( BS_ID_CTX_relays,				btnNum, i, &_bank );
			save_BS_Context( BS_ID_CTX_bankNumber,			btnNum, i, &_bank );
			save_BS_Context( BS_ID_CTX_nameAlias,			btnNum, i, &_bank );
			save_BS_Context( BS_ID_CTX_commonContext,		btnNum, i, &_bank );
			save_BS_Context( BS_ID_CTX_presetChangeContext,	btnNum, i, &_bank );
		}

	}
	send_EOT();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///					H A N D L E		S Y S E X 	S E T T I N G S		M E S S A G E S
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool handleMidiSysExSettings( uint8_t midiMsgType, uint8_t * midiMsg, uint16_t midiMsgLength, uint16_t midiBuffLength, uint8_t * trace ){
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	typedef enum {
		START_SYSEX = 0,
		MANUFACTURER_ID_0,
		MANUFACTURER_ID_1,
		MANUFACTURER_ID_2,
		NETWORK_NUM,
		MODEL_NUM,
		PAYLOAD
	} SysExState_t;

	uint8_t tcode = handleMidiSysExSettings_ID; // trace code

	if (
	( 0 == midiMsgLength )
	||
	( 0 == midiBuffLength )
	||
	( midiBuffLength < midiMsgLength )
	||
	( /*MIDI_SYSEX_END*/MIDI_SYSEX_START != midiMsgType )
	){
		tcode	= tcode | (HMS_BAD_FIRST_CHECK << 3);
		if (NULL!=trace) *trace	= tcode;
		return (false);
	}

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
				if ( MIDI_SYSEX_START != midiMsg[counter] ){
					tcode	= tcode | (HMS_NO_MIDI_SYSEX_START << 3);
					if (NULL!=trace) *trace	= tcode;
					return (false);
				}
				else
					state = MANUFACTURER_ID_0;
				break;
			case MANUFACTURER_ID_0:
				byte = (manfId >> 16) & 0x7F;
				if ( byte != midiMsg[counter] ){
					tcode	= tcode | (HMS_BAD_MANUFACTURER_ID_0 << 3);
					if (NULL!=trace) *trace	= tcode;
					return (false);
				}
				else
					state = MANUFACTURER_ID_1;
				break;
			case MANUFACTURER_ID_1:
				byte = (manfId >> 8) & 0x7F;
				if ( byte != midiMsg[counter] ){
					tcode	= tcode | (HMS_BAD_MANUFACTURER_ID_1 << 3);
					if (NULL!=trace) *trace	= tcode;
					return (false);
				}
				else
					state = MANUFACTURER_ID_2;
				break;
			case MANUFACTURER_ID_2:
				byte = manfId & 0x7F;
				if ( byte != midiMsg[counter] ){
					tcode	= tcode | (HMS_BAD_MANUFACTURER_ID_2 << 3);
					if (NULL!=trace) *trace	= tcode;
					return (false);
				}
				else
					state = NETWORK_NUM;
				break;
			case NETWORK_NUM:
				if ( NETWORK_NUMBER != midiMsg[counter] ){
					tcode	= tcode | (HMS_BAD_NETWORK_NUMBER << 3);
					if (NULL!=trace) *trace	= tcode;
					return (false);
				}
				else
					state = MODEL_NUM;
				break;
			case MODEL_NUM:
				if ( MODEL_NUMBER != midiMsg[counter] ){
					tcode	= tcode | (HMS_BAD_MODEL_NUMBER << 3);
					if (NULL!=trace) *trace	= tcode;
					return (false);
				}
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
		if ( counter >= midiMsgLength ) { 
			tcode	= tcode | (HMS_COUNTER_OVERRUN << 3);
			if (NULL!=trace) *trace	= tcode;
			return(false); 
		}
	}

	if (0 == lengthPayload){
		tcode	= tcode | (HMS_NO_PAYLOAD << 3);
		if (NULL!=trace) *trace	= tcode;
		return(false);
	}

	// decode globals messages

	if ( GLOBALS_MSG == midiMsg[startPayloadPos] ){
		if (load_GS_param( GS_ID_bnkNum,				&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 1):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_midiChanNum,			&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 2):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_useBankSelectMess,		&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 3):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_bankSelectMessType,	&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 4):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_bnkSwOnBoard,			&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 5):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_showPresetBank,		&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 6):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_targetDevice,			&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 7):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_usbBaudrate,			&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 8):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_inputThrough,			&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 9):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_maxBankNumber,			&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 10):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_screenBrightness,		&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 11):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_screenContrast,		&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 12):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_expPedalType,			&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 13):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_buttonHoldTime,		&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 14):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_tapDisplayType,		&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 15):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_tapType,				&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 16):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_pedalLedView,			&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 17):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_pedalTunerScheme,		&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 18):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_pedalBrightness,		&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 19):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_pedalsCalibrationLo,	&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 20):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_GS_param( GS_ID_pedalsCalibrationHi,	&midiMsg[startPayloadPos], lengthPayload,  &global, (NULL!=trace)?(trace + 21):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
	}

	// decode banks messages
	if ( BANKS_MSG == midiMsg[startPayloadPos] ){
		if (load_BS_param( BS_ID_tapCC,			&midiMsg[startPayloadPos], lengthPayload, &global, (NULL!=trace)?(trace + 22):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_BS_param( BS_ID_pedalsCC,		&midiMsg[startPayloadPos], lengthPayload, &global, (NULL!=trace)?(trace + 23):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_BS_param( BS_ID_tunerCC,		&midiMsg[startPayloadPos], lengthPayload, &global, (NULL!=trace)?(trace + 24):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_BS_param( BS_ID_buttonType,	&midiMsg[startPayloadPos], lengthPayload, &global, (NULL!=trace)?(trace + 25):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_BS_param( BS_ID_bankName,		&midiMsg[startPayloadPos], lengthPayload, &global, (NULL!=trace)?(trace + 26):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_BS_param( BS_ID_selectBankAction,			&midiMsg[startPayloadPos], lengthPayload, &global, (NULL!=trace)?(trace + 27):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		if (load_BS_param( BS_ID_selectBankActionProgNum,	&midiMsg[startPayloadPos], lengthPayload, &global, (NULL!=trace)?(trace + 28):NULL )){
			tcode	= tcode | ( 1 << 2 );
			if (NULL!=trace) *trace	= tcode;
			return (true);
		}
		// пытаемс€ декодировать контекст кнопки
		for(uint8_t btnNum=0; btnNum<FOOT_BUTTONS_NUM; btnNum++){
			if(load_BS_context( BS_ID_CTX_relays, btnNum, &midiMsg[startPayloadPos], lengthPayload, &global, (NULL!=trace)?(trace + 29 + btnNum*5):NULL )){
				tcode	= tcode | ( 1 << 2 );
				if (NULL!=trace) *trace	= tcode;
				return (true);
			}
			if(load_BS_context( BS_ID_CTX_presetChangeContext, btnNum, &midiMsg[startPayloadPos], lengthPayload, &global, (NULL!=trace)?(trace + 30 + btnNum*5):NULL )){
				tcode	= tcode | ( 1 << 2 );
				if (NULL!=trace) *trace	= tcode;
				return (true);
			}
			if(load_BS_context( BS_ID_CTX_commonContext, btnNum, &midiMsg[startPayloadPos], lengthPayload, &global, (NULL!=trace)?(trace + 31 + btnNum*5):NULL )){
				tcode	= tcode | ( 1 << 2 );
				if (NULL!=trace) *trace	= tcode;
				return (true);
			}
			if(load_BS_context( BS_ID_CTX_bankNumber, btnNum, &midiMsg[startPayloadPos], lengthPayload, &global, (NULL!=trace)?(trace + 32 + btnNum*5):NULL )){
				tcode	= tcode | ( 1 << 2 );
				if (NULL!=trace) *trace	= tcode;
				return (true);
			}
			if(load_BS_context( BS_ID_CTX_nameAlias, btnNum, &midiMsg[startPayloadPos], lengthPayload, &global, (NULL!=trace)?(trace + 33 + btnNum*5):NULL )){
				tcode	= tcode | ( 1 << 2 );
				if (NULL!=trace) *trace	= tcode;
				return (true);
			}
		}
	}
	tcode	= tcode | (HMS_NO_DECODE << 3);
	if (NULL!=trace) *trace	= tcode;
	return (false);
}

bool handleMidiSysExConfig( uint8_t midiMsgType, uint8_t * midiMsg, uint16_t midiMsgLength, uint16_t midiBuffLength ){
	//LOG(SEV_TRACE,"%s", __FUNCTION__);
	bool		res = false;
	uint8_t		trace[TRACE_LENGTH];
	uint32_t	manfId = MANUFACTURER_ID;
	if (
		(MIDI_SYSEX_START		== midiMsg[0])
		&&
		(((manfId >> 16) & 0x7F)== midiMsg[1])
		&&
		(((manfId >> 8) & 0x7F)	== midiMsg[2])
		&&
		((manfId & 0x7F)		== midiMsg[3])
		&&
		(NETWORK_NUMBER			== midiMsg[4])
		&&
		(MODEL_NUMBER			== midiMsg[5])
	){
		memset( trace, 0 , TRACE_LENGTH );
		res = handleMidiSysExSettings( midiMsgType, midiMsg, midiMsgLength, midiBuffLength, trace );

		if ( false == res ){
			uint8_t tcode = trace[0];
			if ((handleMidiSysExSettings_ID == (tcode & 0x03)) && (0 == (tcode & 0x04))){
				uint8_t error = (tcode>>3) & 0x0f;
				if ( HMS_BAD_FIRST_CHECK == error ) return(res);
			}
		}
		send_ACK();
		//send_ACK_TRACE( trace );
	}
	return(res);
}
