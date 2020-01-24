#ifndef GMAJOR_H
#define GMAJOR_H

//TODO Move G-major functionality to separate its own files
#define GMAJOR1 0x1F
///////TC Electonic G-Major1 ���� 7 - ����, 8 - ����������
//////���� ���������� � 0x4 - E, ���������� �� 0x10 �� 0x1F. ����� 0x17 � 0x18
//////������������ �� 3�� ����� - ���� ����������
// 0  1  2  3  4  5  6  7  8  9
//F0 00 20 1F 00 48 70 00 00 F7

void handleMidiEventGMajor(uint8_t in_MessType
		,uint8_t* midiMessage
		,uint8_t sys_ex_length);

#endif //GMAJOR_H


