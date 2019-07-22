/*
 * BJ Devices Travel Box series midi controller library
 * @file	realy.c
 * 
 * @brief	Relays functions
 *
 * Software is provided "as is" without express or implied warranty.
 * BJ Devices 2017
 */


#ifndef RELAY_H_
#define RELAY_H_

void initRelay();

/*
 * @brief	Set relay to close state - contacts are closed
 * @param	num -		relay number. from 0 to 3
 */
void setRelayClose(uint8_t num);

/*
 * @brief	Set relay to open state - contacts are open
 * @param	num -		relay number. from 0 to 3
 */
void setRelayOpen(uint8_t num);

/*
 * @brief	Set relay to particular state
 * @param	num -		relay number. from 0 to 3
 * @param	send -		value. 0 - relay open, else close  
 */
void setRelay(uint8_t num, uint8_t val);

#endif /* RELAY_H_ */