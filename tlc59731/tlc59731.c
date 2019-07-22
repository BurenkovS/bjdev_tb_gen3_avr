/*
 * tlc59731.c
 *
 * Created: 20.11.2018 23:46:20
 *  Author: Sergey
 */ 

#include "pinout.h"

#include "tlc59731.h"
#include <avr/io.h>      // Defines pins, ports, etc
#include <util/delay.h>  // Functions to waste time

// timing is everything here
#define PWM_DELAY     1u // width a pulse on the wire (read: 1 unit!), period is 2 units
#define PWM_T_CYCLE_0 4u // time to wait for the cycle to complete (before the next bit can be sent) if a 0 was send (4 units)
#define PWM_T_CYCLE_1 1u // time to wait for the cycle to complete (before the next bit can be sent) if a 1 was send (1 unit)

void setHigh()
{
	PWM_IC_PORT |= (1<<PWM_IC_PIN);  // set the pin bit for port
}

void setLow()
{
	PWM_IC_PORT &= ~(1<<PWM_IC_PIN); // clear the pin bit for port
}

void sendBit(uint8_t bit)
{
	// start next cycle
	setHigh();
	_delay_us(PWM_DELAY); // the pulse on the wire does not really have a width of 1us, since the write operation needs time too
	setLow();
	_delay_us(PWM_DELAY);
	
	if(bit)
	{   // write "1" data bit
		setHigh();
		_delay_us(PWM_DELAY); // Bit 29 is a 1
		setLow();
		_delay_us(PWM_T_CYCLE_1);
	}
	else
	{   // for "0" data bit simply let timeout the cycle
		_delay_us(PWM_T_CYCLE_0);
	}
}

void sendByte(uint8_t byte)
{
	sendBit(byte & (1<<7));
	sendBit(byte & (1<<6));
	sendBit(byte & (1<<5));
	sendBit(byte & (1<<4));
	sendBit(byte & (1<<3));
	sendBit(byte & (1<<2));
	sendBit(byte & (1<<1));
	sendBit(byte & (1<<0));
}


void tlc59731SendPwmValues(uint8_t pwm1, uint8_t pwm2, uint8_t pwm3)
{
	 // establish the cycle time,
	 // by writing the 1st SID Rising Edge (see data sheet)
	 setHigh();
	 _delay_us(PWM_DELAY);
	 setLow();
	 _delay_us(PWM_DELAY);
	 _delay_us(PWM_T_CYCLE_0);
	 
	 sendByte(0x3A);
	 sendByte(pwm1);
	 sendByte(pwm2);
	 sendByte(pwm3);
}


void tlc59731Init()
{
	PWM_IC_DDR |= (1<<PWM_IC_PIN); // set port pin to output
}

