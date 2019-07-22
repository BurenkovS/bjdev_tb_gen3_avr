/*
Custom char set for alphanumeric LCD Module
Created by eXtreme Electronics
LCD Custom Char Builder!

Visit http://www.eXtremeElectronics.co.in
*/

#ifndef __CUSTOMCHAR_H
#define __CUSTOMCHAR_H

unsigned char __cgram[]=
{
0b10000,0b11000,0b11101,0b11011,0b10111,0b00011,0b00001,0b00000,
//0x2,0x6,0xe,0x1e,0xe,0x6,0x2,0x0, //Arrow Left
0x04, 0x04, 0x04, 0x04, 0xFF, 0x0E, 0x04, 0x04, //Arrow Down
0b01110, 0b00100, 0b00100, 0b01110, 0b00000, 0b11111,  0b11111, 0b00000,//ON symbol
0b11111, 0b11111, 0b00000, 0b01110, 0b01010, 0b01010, 0b01110, 0b00000,//OFF symbol
0b00100, 0b01010, 0b10001, 0b01110, 0b01010, 0b01110, 0b00000, 0b00000, //Home
0b11111, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11111,//TAP
//0x01, 0x01, 0x05, 0x05, 0x15, 0x15, 0x15, 0x00, //Char5
0x0E, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, //Char6
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Char7
};
#endif

