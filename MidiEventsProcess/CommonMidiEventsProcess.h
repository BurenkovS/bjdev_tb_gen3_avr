/*
 * CommonMidiEventsProcess.h
 *
 * Created: 29.08.2017 16:47:27
 *  Author: Sergey
 */ 


#ifndef COMMONMIDIEVENTSPROCESS_H_
#define COMMONMIDIEVENTSPROCESS_H_

#include <stdint.h>

void handleMidiEventCommonMidiDevices(uint8_t in_MessType
		,uint8_t in_ChanNum
		,uint8_t* midiMessage);



#endif /* COMMONMIDIEVENTSPROCESS_H_ */