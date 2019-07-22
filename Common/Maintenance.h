/*
 * Maintenance.h
 *
 * Created: 29.08.2017 1:03:32
 *  Author: Sergey
 */ 


#ifndef MAINTENANCE_H_
#define MAINTENANCE_H_

#include <stdint.h>
/*
 * Cycle increment value form 0 to max_val (0,1,2,...,127,0)
 * return 1 if border crossing occur
*/
uint8_t cycleIncUcahrVal(uint8_t* param, uint8_t maxVal);


/*
 * Cycle decrement value form max_val to 0 (127, 126, ... , 0, 127)
 * return 1 if border crossing occur
*/
uint8_t cycleDecUcahrVal(uint8_t* param, uint8_t maxVal);

/*
 * Copy string and write 0 to the max position if needed
 */
void strcpyLimits(const char* src, char* dst, char max_size);




#endif /* MAINTENANCE_H_ */