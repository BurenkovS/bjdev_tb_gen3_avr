/*
 * RuntimeEnvironment.c
 *
 * Created: 12.12.2017 16:21:25
 *  Author: User
 */ 

#include "RuntimeEnvironment.h"
#include <stdint.h>
#include <stdbool.h>

void setEventByMask(uint8_t mask)
{
	updateRequests.eventFlags_ |= mask;
}

void clearEvents()
{
	updateRequests.eventFlags_ = 0;
}

bool checkEventByMask(uint8_t mask)
{
	return (updateRequests.eventFlags_ & mask) ? true : false;
}
