/*
 * Maintenance.c
 *
 * Created: 29.08.2017 1:01:22
 *  Author: Sergey
 */

/*
 * Cycle increment value form 0 to max_val (0,1,2,...,127,0)
 * return 1 if border crossing occur
*/

#include "Maintenance.h"

uint8_t cycleIncUcahrVal(uint8_t* param, uint8_t maxVal)
{
    if (*param < maxVal)
    {
        *param = *param + 1;
        return 0;
    }
    else
    {
        *param = 0;
        return 1;
    }
}

/*
 * Cycle decrement value form max_val to 0 (127, 126, ... , 0, 127)
 * return 1 if border crossing occur
*/
uint8_t cycleDecUcahrVal(uint8_t* param, uint8_t maxVal)
{
    if  (*param > 0)
    {    
        *param = *param - 1;
        return 0;
    }
    else
    {
        *param = maxVal;
        return 1;
    }
}

void strcpyLimits(const char* src, char* dst, char max_size)
{
	char i;
	for (i = 0; *(src + i) != '\0'; ++i)
	{
		*(dst + i) = *(src + i);
		if(i == max_size)
		break;
	}
	*(dst + i) = '\0';
}
 
