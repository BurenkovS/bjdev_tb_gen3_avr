/*
 * eeprom.с
 *
 * Created: 20.03.2017 22:03:43
 *  Author: Катико
 */ 
#include "eeprom_wrapper.h"
#include "settings.h"
#include "25LC256.h"
#include "lcd_tb.h"

#include <avr/eeprom.h>
#include <util/delay.h>
#include "log.h"

bool eepromPresentStartupCheck()
{
	EEPROM_writeDisable();
	_delay_ms(1);
	uint8_t eepStat = EEPROM_readStatus();
	if(!(eepStat & _BV(EEPROM_WRITE_ENABLE_LATCH)))
	{
		EEPROM_writeEnable();
		_delay_ms(1);
		eepStat = EEPROM_readStatus();
		if(eepStat & _BV(EEPROM_WRITE_ENABLE_LATCH))
			return true;
		else
			return false;
	}
	else
		return false;
}

void WriteEEPROM(uint8_t *data, uint32_t address, uint32_t len)
{
	//LOG(SEV_TRACE, "Address:%X", (unsigned int)address);
	
	if (address == 0xFFFFFFFF)
		return;
	
	while (len)
	{
		if (address >= INTERNAL_EEPROM_SIZE_BYTES)
		{
			EEPROM_writeByte(address - INTERNAL_EEPROM_SIZE_BYTES, *data);
			//_delay_ms(50);//TODO use this only in proteus!!
		}
		else
			eeprom_write_byte((uint8_t*)(uint16_t)address, *data);
		
		data++;
		len--;
		address++;
	}
}

void ReadEEPROM(uint8_t *data, uint32_t address, uint32_t len)
{
	if (address == 0xFFFFFFFF)
		return;
	
	while (len)
	{
		if (address >= INTERNAL_EEPROM_SIZE_BYTES)
			*data = EEPROM_readByte(address - INTERNAL_EEPROM_SIZE_BYTES);
		else
			*data = eeprom_read_byte((uint8_t*)(uint16_t)address);
		
		data++;
		len--;
		address++;
	}
}

uint32_t memTest()
{
	uint32_t i;
	uint8_t data = 0;
	uint32_t errors = 0;
	for(i = 0; i < EXTERNAL_EEPROM_SIZE_BYTES; ++i)
	{
		data = (uint8_t)i;
		EEPROM_writeByte(i, data);
	}
	
	for(i = 0; i < EXTERNAL_EEPROM_SIZE_BYTES; ++i)
	{
		data = EEPROM_readByte(i);
		if(data != (uint8_t)i)
		{
			++errors;
		}
	}
	return errors;
}