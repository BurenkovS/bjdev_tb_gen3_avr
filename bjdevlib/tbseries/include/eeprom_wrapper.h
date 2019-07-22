/*
 * eeprom.h
 *
 * Created: 20.03.2017 22:02:42
 *  Author: Катико
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>
#include <stdbool.h>
#include "25LC256.h"

#ifndef INTERNAL_EEPROM_SIZE_BYTES
#define INTERNAL_EEPROM_SIZE_BYTES 2048 
#endif

#ifndef EXTERNAL_EEPROM_SIZE_BYTES
#define EXTERNAL_EEPROM_ADDR_WIDTH 16
#define EXTERNAL_EEPROM_SIZE_BYTES 65536//((uint32_t)(_BV(EXTERNAL_EEPROM_ADDR_WIDTH)))
#endif

#define TOTAL_EEPROM_SIZE_BYTES ((uint32_t)((uint32_t)EXTERNAL_EEPROM_SIZE_BYTES + (uint32_t)INTERNAL_EEPROM_SIZE_BYTES))

//Check external EEPROM is present
//use this only after power up!
bool eepromPresentStartupCheck();
void WriteEEPROM(uint8_t *data, uint32_t address, uint32_t len);
void ReadEEPROM(uint8_t *data, uint32_t address, uint32_t len);
uint32_t memTest();


#endif /* EEPROM_H_ */