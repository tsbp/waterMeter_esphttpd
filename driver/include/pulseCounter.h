/*
 * pulseCounter.h
 *
 *  Created on: 8 сент. 2020 г.
 *      Author: Voodoo
 */

#ifndef DRIVER_INCLUDE_PULSECOUNTER_H_
#define DRIVER_INCLUDE_PULSECOUNTER_H_

//unsigned long pulseCntr;
//==============================================================================
typedef union __attribute__ ((__packed__))
{
	uint8_t byte[64];
	struct __attribute__ ((__packed__))
	{
		uint8_t currentDay;
		uint8_t currentMonth;
		uint8_t dayCounter[31 + 1];
		uint16_t monthCounter[12 + 1];
		uint32_t totalCounter;
	};
} sPERIOD_USEGE;
extern sPERIOD_USEGE periodUsage;

//===============================================================
typedef struct {
	int date;
    int month;
    int currYear;
    int hours;
    int minutes;
    int seconds;
}sDATE_TIME;
//===============================================================
extern sDATE_TIME dateTime;
//===============================================================

sDATE_TIME unixTimeToHumanReadable(uint32 seconds);
void ICACHE_FLASH_ATTR pulseCounterInit(void);
void ICACHE_FLASH_ATTR sntp_initialize(void);
void ICACHE_FLASH_ATTR sntp_get_stamp(void);
void litrToMeter3(uint32 litr, char *out);

#endif /* DRIVER_INCLUDE_PULSECOUNTER_H_ */
