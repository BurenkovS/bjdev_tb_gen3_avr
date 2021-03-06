/*
 * BJ Devices Travel Box series midi controller library
 * @file	uart.c
 * 
 * @brief	USART low level functions				
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2016
 */

#include "uart.h"
#include "log.h"

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

static uint8_t rxBuffer0[RX_BUFFER_SIZE0];

#if RX_BUFFER_SIZE0 <= 256
static volatile uint8_t rxWrIndex0;
static volatile uint8_t rxRdIndex0;
static volatile uint8_t rxCounter0;

#else
static volatile uint16_t rxWrIndex0;
static volatile uint16_t rxRdIndex0;
static volatile uint16_t rxCounter0;

#endif // RX_BUFFER_SIZE0 <= 256

// This flag is set on USART0 Receiver buffer overflow
static bool rxBufferOverflow0;

// USART0 Receiver interrupt service routine
ISR(USART0_RX_vect)
{
	uint8_t status = UCSR0A;
	uint8_t data = UDR0;
	
	if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
	{
		rxBuffer0[rxWrIndex0++]=data;
		
		#if RX_BUFFER_SIZE0 == 256
			// special case for receiver buffer size=256
			if (++rxCounter0 == 0)
			{
				rxBufferOverflow0 = true;
				//while(1){};
			}
		#else

		if (rxWrIndex0 == RX_BUFFER_SIZE0)
			rxWrIndex0 = 0;
			
		if (++rxCounter0 == RX_BUFFER_SIZE0)
		{
			rxCounter0 = 0;
			rxBufferOverflow0 = true;
		}
		#endif //RX_BUFFER_SIZE0 == 256
	}
}

void initUart0AsMidi()
{
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART0 Receiver: On
	// USART0 Transmitter: On
	// USART0 Mode: Asynchronous
	// USART0 Baud Rate: 31250
	UCSR0A=0x00;
	UCSR0B=0x98;
	UCSR0C=0x06;
	UBRR0H=0x00;
	UBRR0L=0x0F;
}

void uart0PutChar(uint8_t data)
{
	while ((UCSR0A & DATA_REGISTER_EMPTY) == 0);
	UDR0 = data;
}

uint8_t uart0GetChar()
{	
	uint8_t data;
	while (rxCounter0 == 0){};
		
	data = rxBuffer0[rxRdIndex0++];
	
	#if RX_BUFFER_SIZE0 != 256
	if (rxRdIndex0 == RX_BUFFER_SIZE0) 
		rxRdIndex0=0;
	#endif
	
	cli();
	--rxCounter0;
	sei();
	return data;
}

bool uart0IsBufferEmpty()
{
	return (rxCounter0 == 0);
}

bool uart0IsBufferOvefflow(bool resetOverflowFlag)
{
	bool ret = rxBufferOverflow0;
	
	if(resetOverflowFlag)
		rxBufferOverflow0 = false;
		
	return ret;
}

//uart 2

static uint8_t rxBuffer1[RX_BUFFER_SIZE1];

#if RX_BUFFER_SIZE1 <= 256
static uint8_t rxWrIndex1;
static uint8_t rxRdIndex1;
static uint8_t rxCounter1;

#else
static uint16_t rxWrIndex1;
static uint16_t rxRdIndex1;
static uint16_t rxCounter1;

#endif // RX_BUFFER_SIZE1 <= 256

// This flag is set on USART0 Receiver buffer overflow
static bool rxBufferOverflow1;

// USART0 Receiver interrupt service routine
ISR(USART1_RX_vect)
{
	uint8_t status = UCSR1A;
	uint8_t data = UDR1;
	
	if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
	{
		rxBuffer1[rxWrIndex1++]=data;
		
		#if RX_BUFFER_SIZE1 == 256
		// special case for receiver buffer size=256
		if (++rxCounter1 == 0)
		rxBufferOverflow1 = true;
		#else

		if (rxWrIndex1 == RX_BUFFER_SIZE1)
		rxWrIndex1 = 0;
		
		if (++rxCounter1 == RX_BUFFER_SIZE1)
		{
			rxCounter1 = 0;
			rxBufferOverflow1 = true;
		}
		#endif //RX_BUFFER_SIZE1 == 256
	}
}

void initUart1(uint32_t baudrate)
{
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART1 Receiver: On
	// USART1 Transmitter: On
	// USART1 Mode: Asynchronous
	
	UCSR1A=0x00;
	UCSR1B=0x98;
	UCSR1C=0x06;
	
	UBRR1L = (baudrate == 0) ? 0x0F : 0x08;
	UBRR1H = 0x00;//val>>8;
}

void uart1PutChar(uint8_t data)
{
	while ((UCSR1A & DATA_REGISTER_EMPTY) == 0);
	UDR1 = data;
}

uint8_t uart1GetChar()
{
	uint8_t data;
	while (rxCounter1==0);
	data = rxBuffer1[rxRdIndex1++];
	
	#if RX_BUFFER_SIZE1 != 256
	if (rxRdIndex1 == RX_BUFFER_SIZE1)
	rxRdIndex1=0;
	#endif
	
	cli();
	--rxCounter1;
	sei();
	return data;
}

void uart1PutString(char* str)
{
	while (0 != *str)
	{
		uart1PutChar(*str);
		str++;
	}
}

void uart1PutStringP(const char* str)
{
	char c;
	while (0 != (c = pgm_read_byte(str)))
	{
		uart1PutChar(c);
		str++;
	}
}

uint16_t uart0GetRxCounter()
{
	return rxCounter0;
};

uint16_t uart1GetRxCounter()
{
	return rxCounter1;
};

