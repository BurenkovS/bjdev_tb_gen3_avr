/*
 * tlc59731.h
 *
 * Created: 20.11.2018 23:46:31
 *  Author: Sergey
 */ 


#ifndef TLC59731_H_
#define TLC59731_H_

void tlc59731Init();
void tlc59731SendPwmValues(uint8_t pwm1, uint8_t pwm2, uint8_t pwm3);

#endif /* TLC59731_H_ */