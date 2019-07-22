/*
 * BJ Devices Travel Box series midi controller library
 * @file	realy.c
 * 
 * @brief	Relays functions
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2017
 */

#include "portio.h"
#include "pinout.h"
#include <stdint.h>
#include <avr/pgmspace.h>


void initRelay()
{
	RELAY_1_DDR |= (1 << RELAY_1_PNUM);
	RELAY_2_DDR |= (1 << RELAY_2_PNUM);
	RELAY_3_DDR |= (1 << RELAY_3_PNUM);
	RELAY_4_DDR |= (1 << RELAY_4_PNUM);
}

void setRelayClose(uint8_t num)
{
	switch(num)
	{
		case 0:
			RELAY_1_PORT |= (1 << RELAY_1_PNUM);
			break;

		case 1:
			RELAY_2_PORT |= (1 << RELAY_2_PNUM);
		break;

		case 2:
			RELAY_3_PORT |= (1 << RELAY_3_PNUM);
		break;
		
		case 3 :	
			RELAY_4_PORT |= (1 << RELAY_4_PNUM);
		break;
		
		default: break;
	}
}

void setRelayOpen(uint8_t num)
{
	switch(num)
	{
		case 0:
			RELAY_1_PORT &= ~(1 << RELAY_1_PNUM);
		break;

		case 1:
			RELAY_2_PORT &= ~(1 << RELAY_2_PNUM);
		break;

		case 2:
			RELAY_3_PORT &= ~(1 << RELAY_3_PNUM);
		break;
		
		case 3 :
			RELAY_4_PORT &= ~(1 << RELAY_4_PNUM);
		break;
		
		default: break;
	}
}

void setRelay(uint8_t num, uint8_t val)
{
	if(val)
		setRelayClose(num);
	else
		setRelayOpen(num);
}
