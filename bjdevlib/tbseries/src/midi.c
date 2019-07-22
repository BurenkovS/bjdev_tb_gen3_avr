/*
 * BJ Devices Travel Box series midi controller library
 * @file	midi.c
 * 
 * @brief	Send and receive midi messages				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#include "midi.h"
#include "uart.h"

#include <stdint.h>

#define NUM_MIDI_EVENTS 18
static unsigned char midiEventLength[NUM_MIDI_EVENTS][2] = {
	{MIDI_NOTE_OFF,3},
	{MIDI_NOTE_ON,3},
	{MIDI_POLY_TOUCH,3},
	{MIDI_CONTROL_CHANGE,3},
	{MIDI_PROGRAM_CHANGE,2},
	{MIDI_CHANNEL_TOUCH,2},
	{MIDI_PITCH_BEND,3},
	{MIDI_MTC,2},
	{MIDI_SPP,3},
	{MIDI_SONG_SEL,2},
	{MIDI_TUNE_REQ,1},
	{MIDI_CLOCK,1},
	{MIDI_SYNC,1},
	{MIDI_START,1},
	{MIDI_STOP,1},
	{MIDI_CONT,1},
	{MIDI_SENSE,1},
	{MIDI_RESET,1}};

uint8_t getCommandLen(uint8_t cmd)
{
	uint8_t i;
	if ((cmd & 0xF0) != 0xF0)
		cmd = cmd & 0xF0;
		
	for (i = 0; i < NUM_MIDI_EVENTS; i++)
	{
		if (cmd == midiEventLength[i][0])
		return midiEventLength[i][1];
	}
	return 0;
}
	
//static uint8_t midiBuffer[MIDI_BUFFER_SIZE];

void initMidi()
{
	initUart0AsMidi();	
}

void midiSendProgramChange(uint8_t progNum, uint8_t chanNum)
{
	uart0PutChar(MIDI_PROGRAM_CHANGE | (0x0F & chanNum));
	uart0PutChar(0x7F & progNum);

	//Send same command to USB, if LOG disable
#ifndef LOG_ENABLED	
	uart1PutChar(MIDI_PROGRAM_CHANGE | (0x0F & chanNum));
	uart1PutChar(0x7F & progNum);
#endif
}

void midiSendControlChange(uint8_t ctrlNum, uint8_t val, uint8_t chanNum)
{
	uart0PutChar(MIDI_CONTROL_CHANGE | (0x0F & chanNum));
	uart0PutChar(0x7F & ctrlNum);
	uart0PutChar(0x7F & val);
//Send same command to USB, if LOG disable
#ifndef LOG_ENABLED
	uart1PutChar(MIDI_CONTROL_CHANGE | (0x0F & chanNum));
	uart1PutChar(0x7F & ctrlNum);
	uart1PutChar(0x7F & val);
#endif
}

void midiSendNrpn(uint8_t ctrlMsb,uint8_t ctrlLsb, uint8_t valMsb, uint8_t valLsb, uint8_t chanNum)
{
	//NRPNs controllers are specified by CC 99 and 98
	//parameters are specified by CC 6 and 38
	midiSendControlChange(NRPN_CONTROLLER_MSB, ctrlMsb, chanNum);
	midiSendControlChange(NRPN_CONTROLLER_LSB, ctrlLsb, chanNum);
	midiSendControlChange(NRPN_VALUE_MSB, valMsb, chanNum);
	midiSendControlChange(NRPN_VALUE_LSB, valLsb, chanNum);
}

void midiSendNote(bool isOn, uint8_t noteNum, uint8_t noteVel, uint8_t chanNum)
{
	if(isOn)
		uart0PutChar(MIDI_NOTE_OFF | (0x0F & chanNum));
	else
		uart0PutChar(MIDI_NOTE_OFF | (0x0F & chanNum));
		
	uart0PutChar(0x7F & noteNum);
	uart0PutChar(0x7F & noteVel);
	//Send same command to USB, if LOG disable
	#ifndef LOG_ENABLED
	if(isOn)
		uart1PutChar(MIDI_NOTE_ON  | (0x0F & chanNum));
	else
		uart1PutChar(MIDI_NOTE_ON  | (0x0F & chanNum));
	uart1PutChar(0x7F & noteNum);
	uart1PutChar(0x7F & noteVel);
	#endif
}


void midiSendSysEx(uint16_t length, uint8_t* data)
{
	uint32_t i;
	
	uart0PutChar(MIDI_SYSEX_START);
	for (i = 0; i < length; ++i)
		uart0PutChar(*(data + i));	
	
	uart0PutChar(0xF7);
	
#ifndef LOG_ENABLED
	uart1PutChar(MIDI_SYSEX_START);
	for (i = 0; i < length; ++i)
		uart1PutChar(*(data + i));
	
	uart1PutChar(0xF7);
#endif
}

void midiSendSysExManfId(uint32_t manfId, uint16_t length, uint8_t* data)
{
	uint32_t i;
	
	uart0PutChar(MIDI_SYSEX_START);
	
	uart0PutChar((manfId >> 16) & 0x7F);
	uart0PutChar((manfId >> 8) & 0x7F);
	uart0PutChar(manfId & 0x7F);
	
	for (i = 0; i < length; ++i)
	{
		uart0PutChar(*(data + i));
	}
	uart0PutChar(0xF7);
	
#ifndef LOG_ENABLED
	uart1PutChar(MIDI_SYSEX_START);
	
	uart1PutChar((manfId >> 16) & 0x7F);
	uart1PutChar((manfId >> 8) & 0x7F);
	uart1PutChar(manfId & 0x7F);
	
	for (i = 0; i < length; ++i)
	{
		uart1PutChar(*(data + i));
	}
	uart1PutChar(0xF7);
#endif

}

static uint16_t midiInRxCnt[TOTAL_INTERFACES] = {0, 0};
static uint8_t lastStatus[TOTAL_INTERFACES] = {MIDI_UNKNOWN, MIDI_UNKNOWN};
static uint8_t lastCmdLength[TOTAL_INTERFACES] = {0, 0};

_Bool parce(uint8_t data, uint8_t* midiBuffer, uint8_t interfaceNum)
{
    uint8_t statusWoChannel;
	if(midiInRxCnt[interfaceNum] == 0)
	{
		if(!(data & 0x80))
			return 0;//TODO add running status support here

		statusWoChannel = (data < 0xF0) ? (data & 0xF0) : data;//Save all byte in case of realtime message
		switch (statusWoChannel)
		{
			case MIDI_NOTE_OFF :
            case MIDI_NOTE_ON :
            case MIDI_POLY_TOUCH :
            case MIDI_CONTROL_CHANGE :
            case MIDI_PROGRAM_CHANGE :
            case MIDI_CHANNEL_TOUCH :
            case MIDI_PITCH_BEND :
			case MIDI_SYSEX_START :
				midiBuffer[midiInRxCnt[interfaceNum]++] = data;
				lastStatus[interfaceNum] = data & 0xF0;
				return 0;
			    break;

			case MIDI_MTC :
            case MIDI_SPP :
            case MIDI_SONG_SEL :
            case MIDI_TUNE_REQ :
            case MIDI_CLOCK :
            case MIDI_SYNC :
            case MIDI_START :
            case MIDI_STOP :
            case MIDI_CONT :
            case MIDI_SENSE :
            case MIDI_RESET :
				midiBuffer[midiInRxCnt[interfaceNum]] = data;
				lastStatus[interfaceNum] = data;
                lastCmdLength[interfaceNum] = 1;//REALTIME is always 1 byte length
				return 1;
			break;

			default:
				lastStatus[interfaceNum] = MIDI_UNKNOWN;
				return 0;
			break;
		}
	}
	else
	{
		//MSB must be 0 inside the message if it is not end of SysEx message
		if ((data & 0x80) && (data != MIDI_SYSEX_END))
		{
			midiInRxCnt[interfaceNum] = 0;//start waiting new valid message
			lastStatus[interfaceNum] = MIDI_UNKNOWN;
			return 0;
		}

		switch (lastStatus[interfaceNum])
		{
			case MIDI_NOTE_OFF :
            case MIDI_NOTE_ON :
            case MIDI_POLY_TOUCH :
            case MIDI_CONTROL_CHANGE :
            case MIDI_PROGRAM_CHANGE :
            case MIDI_CHANNEL_TOUCH :
            case MIDI_PITCH_BEND :
				midiBuffer[midiInRxCnt[interfaceNum]++] = data;
				if(midiInRxCnt[interfaceNum] == getCommandLen(lastStatus[interfaceNum]))//if end of message reached
				{
                    lastCmdLength[interfaceNum] = midiInRxCnt[interfaceNum];
                    midiInRxCnt[interfaceNum] = 0;
					return 1;
				}
			break;

			case MIDI_SYSEX_START :
				midiBuffer[midiInRxCnt[interfaceNum]++] = data;
				if(data == MIDI_SYSEX_END)//End of sys ex
				{
					lastCmdLength[interfaceNum] = midiInRxCnt[interfaceNum];
					midiInRxCnt[interfaceNum] = 0;
					return 1;
				}
			break;

			default:
			break;
		}
		return 0;
	}
}

bool midiRead(uint8_t interfaceNum, uint8_t* midiBuffer, uint8_t* midiMessageType, uint16_t* midiCmdLength)
{
	uint16_t i;
    uint8_t data;
    volatile unsigned int rx_counter0_barier = (interfaceNum == MIDI_INTERFACE) ? uart0GetRxCounter() : uart1GetRxCounter();
    for(i = 0; i < rx_counter0_barier; i++)
	{
		data = (interfaceNum == MIDI_INTERFACE) ? uart0GetChar() : uart1GetChar();
        if(parce(data, midiBuffer, interfaceNum))
        {
            *midiMessageType = lastStatus[interfaceNum];
            *midiCmdLength = lastCmdLength[interfaceNum];
            return 1;
        }
    }
    return 0;
}


/*uint8_t getMessageLength(uint8_t messageType)
{
	switch(messageType)
	{
		case PC_STATUS :
			return 2;
		break;
		
		case CC_STATUS :
			return 3;
		break;
		
		case ACTIVE_SENSE :
			return 1;
		break;
		
		default:
			return 0;//return 0 if length is unknown
		break;
	}

}
static uint16_t midiInRxCnt = 0;
static uint8_t lastStatus = UNKNOWN_STATUS;
static uint16_t lastSysExLength = 0;*/

/*bool parce(uint8_t data)
{
	if(midiInRxCnt == 0)
	{
		if(!(data & 0x80))
			return false;//TODO add running status support here
		
		uint8_t statusWoChannel = (data < 0xF0) ? (data & 0xF0) : data;//Save all byte in case of realtime message
		switch (statusWoChannel)
		{
			case PC_STATUS :
			case CC_STATUS	:
			case SYSEX_STATUS :
				midiBuffer[midiInRxCnt++] = data;
				lastStatus = data & 0xF0;
				return false;
			break;

			case ACTIVE_SENSE :
				midiBuffer[midiInRxCnt] = data;
				lastStatus = data;
				return true;
			break;
			
			default:
				lastStatus = UNKNOWN_STATUS;
				return false;
			break;
		}
	}
	else
	{
		//MSB must be 0 inside the message if it is not end of SysEx message
		if ((data & 0x80) && (data != SYSEX_END))
		{
			midiInRxCnt = 0;//start waiting new valid message
			lastStatus = UNKNOWN_STATUS;
			return false;
		}
		
		switch (lastStatus)
		{
			case PC_STATUS :
			case CC_STATUS	:
				midiBuffer[midiInRxCnt++] = data;
				if(midiInRxCnt == getMessageLength(lastStatus))//if end of message reached
				{
					midiInRxCnt = 0;
					//lastStatus = UNKNOWN_STATUS;
					return true;
				}
			break;
			
			case SYSEX_STATUS :
				midiBuffer[midiInRxCnt++] = data;
				if(data == SYSEX_END)//End of sys ex
				{
					lastSysExLength = midiInRxCnt;
					midiInRxCnt = 0;
					return true;
				}
			break;

			default:
			break;
		}
		return false;
	}
}*/


//callbacks
/*static void (*ccCallback)(uint8_t, uint8_t, uint8_t);
static void (*pcCallback)(uint8_t, uint8_t);
static void (*sysExCallback)(uint16_t);
static void (*activeSenseCallback)(void);*/

/*void runCallbacks() 
{
	if(ccCallback && lastStatus == MIDI_CONTROL_CHANGE)
		(*ccCallback)(midiGetChannelNumber(), midiGetControllerNumber(), midiGetControllerValue());
		
	if(pcCallback && lastStatus == MIDI_PROGRAM_CHANGE)
		(*pcCallback)(midiGetChannelNumber(), midiGetProgramNumber());
		
	if(sysExCallback && lastStatus == MIDI_SYSEX_START)
		(*sysExCallback)(lastSysExLength);
		
	if(activeSenseCallback && lastStatus == MIDI_SYSEX_START)
		(*activeSenseCallback)();
}*/

/*bool midiRead()
{
	while(!uart0IsBufferEmpty())
	{
		if(parce(uart0GetChar()))
		{
			//runCallbacks();
			return true;
		}
	}
	return false;
}*/

/*void midiRegisterControlChangeCallback(void (*callback)(uint8_t channel, uint8_t ccNum, uint8_t ccVal))
{
	ccCallback = callback;
}

void midiRegisterProgramChangeCallback(void (*callback)(uint8_t channel, uint8_t program))
{
	pcCallback = callback;
}

void midiRegisterSysExCallback(void (*callback)(uint16_t length))
{
	sysExCallback = callback;
}

void midiRegisterActiveSenseCallback(void (*callback)(void))
{
	activeSenseCallback = callback;
}*/


uint8_t midiGetChannelNumber(uint8_t* midiBuffer)
{
	return (midiBuffer[0] & 0x0F);
} 

uint8_t midiGetProgramNumber(uint8_t* midiBuffer)
{
	return midiBuffer[1];
}

uint8_t midiGetControllerNumber(uint8_t* midiBuffer)
{
	return midiBuffer[1];
}

uint8_t midiGetControllerValue(uint8_t* midiBuffer)
{
	return midiBuffer[2];
}

uint16_t midiGetSysExLength(uint8_t* sysEx)
{
	uint16_t length = 0;
	
	while(*(sysEx + length) != SYSEX_END)
		++length;
		
	return length;
}

/*uint16_t midiGetLastSysExLength(uint8_t* midiBuffer)
{
	//return lastSysExLength;
}*/

uint8_t* midiGetLastSysExData(uint8_t* midiBuffer)
{
	return midiBuffer;
}

uint8_t midiGetMessageType(uint8_t* midiBuffer)
{
	return (midiBuffer[0] & 0xF0);
}

uint32_t midiGetSysExManufacturerId(uint8_t* sysEx)
{
	uint32_t ret = 0;
	ret = ((uint32_t)sysEx[1] << 16) | ((uint32_t)sysEx[2] << 8) | sysEx[3]; 
	return ret;
}