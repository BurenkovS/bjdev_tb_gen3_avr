/*
 * FootswitchesProcess.h
 *
 * Created: 08.08.2017 0:03:48
 *  Author: Sergey
 */ 


#ifndef FOOTSWITCHESPROCESS_H_
#define FOOTSWITCHESPROCESS_H_

#include "bjdevlib_tb.h"

void footswitchesProcess(ButtonEvent *buttonEvent);

void extBs2PedalProcess(ButtonEvent *buttonEvent);

//common function for switch tuner off and on
void tunerOnOffProcess();



#endif /* FOOTSWITCHESPROCESS_H_ */