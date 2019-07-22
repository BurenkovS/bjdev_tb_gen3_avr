/*
 * TunerOnLcd.c
 *
 * Created: 14.12.2017 23:03:20
 *  Author: Sergey
 */ 
#include "lcd_tb.h"

void drawTunerOnLcd(uint8_t y_position, uint8_t note, uint8_t tune)
{
	char i;
	LCDGotoXY(0, y_position);//��������� � ������ ������, ������������ �������������
	if (tune == 0)// ���� �������� tune ����� 0 ������ ����� �� ���� ����������� ����������
		LCDWriteString("       ");//��������� ����� � ��������� ����
	else
	{
		LCDGotoXY(0, y_position);//��������� � ������ ������, ������������ �������������
		for (i = 0; i < 6; i++)//������ ���������� �����
		{
			if (i < (tune - 16))
			LCDData(' ');
			else LCDData(255);
		}
		//fine
		if(tune == 23 || tune == 24)//center
			LCDData('>');
		else if (6 < (tune - 16))
			LCDData(' ');
		else
			LCDData(255);
	}
	switch (note) //������ ����
	{
		case 0xb:
			LCDWriteString("B ");
			break;

		case 0xa:
			LCDWriteString("Bb");
			break;
		case 0x9:
			LCDWriteString("A ");
			break;
		case 0x8:
			LCDWriteString("Ab");
			break;
		case 0x7:
			LCDWriteString("G ");
			break;
		case 0x6:
			LCDWriteString("Gb");
			break;
		case 0x5:
			LCDWriteString("F ");
			break;
		case 0x4:
			LCDWriteString("E ");
			break;
		case 0x3:
			LCDWriteString("Eb");
			break;
		case 0x2:
			LCDWriteString("D ");
			break;
		case 0x1:
			LCDWriteString("Db");
			break;
		case 0x0:
			LCDWriteString("C ");
			break;
	}

	//fine
	if(tune == 23 || tune == 24)
		LCDData('<');
	else if (0 < (tune - 24))
		LCDData(255);
	else
		LCDData(' ');

	for (i = 1; i < 7; i++)//������ ���������� ������
	{
		if (i < (tune - 24))
			LCDData(255);
		else
			LCDData(' ');
	}
}
