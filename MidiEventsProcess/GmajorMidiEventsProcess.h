#ifndef GMAJOR_H
#define GMAJOR_H

//TODO Move G-major functionality to separate its own files
#define GMAJOR1 0x1F
///////TC Electonic G-Major1 байт 7 - нота, 8 - подстройка
//////Ноты начинаются с 0x4 - E, подстройка от 0x10 до 0x1F. центр 0x17 и 0x18
//////Определяется по 3му байту - коду устройства
// 0  1  2  3  4  5  6  7  8  9
//F0 00 20 1F 00 48 70 00 00 F7

void handleMidiEventGMajor(uint8_t in_MessType
		,uint8_t* midiMessage
		,uint8_t sys_ex_length);

#endif //GMAJOR_H


