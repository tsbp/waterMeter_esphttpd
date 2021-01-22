/*
 * pulseCounter.c
 *
 *  Created on: 7 сент. 2020 г.
 *      Author: Voodoo
 */
//#include "user_interface.h"
//#include "gpio16.h"
//#include "osapi.h"

#include <esp8266.h>
#include "ramtron.h"
#include "LED_7_SEGMENT.h"
#include "pulseCounter.h"
#include "sntp.h"

//==============================================================================
static void cntr_intr_callback();
//======================= GPIO init function ======================================
//uint8_t button_pin = 2;
#define CNTR_PIN (4)
//=====================================================================
//unsigned long pulseCntr = 0;
sPERIOD_USEGE periodUsage;

#define BOUNCE_PERIOD		(100) // in msec
ETSTimer cBounce_timer;
static void bounceDelay_callback(void *arg);
//==============================================================================
void ICACHE_FLASH_ATTR pulseCounterInit(void) {
//	GPIO_INT_TYPE gpio_type;
//	gpio_type = GPIO_PIN_INTR_NEGEDGE;
//	//==== button pin ====
//	if (set_gpio_mode(button_pin, GPIO_PULLUP, GPIO_INT)) {
//		if (gpio_intr_init(button_pin, gpio_type))
//			gpio_intr_attach(button_intr_callback);
//		//ets_uart_printf("button init!!!\r\n");
//	}

	gpio_init();
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, 0);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO4_U);
	//gpio_output_set(0, 0, (1 << LED), 0);
	gpio_output_set(0, 0, 0, (1 << CNTR_PIN));
	ETS_GPIO_INTR_DISABLE();
	ETS_GPIO_INTR_ATTACH(cntr_intr_callback, NULL);
	gpio_pin_intr_state_set(GPIO_ID_PIN(4), GPIO_PIN_INTR_NEGEDGE);
	ETS_GPIO_INTR_ENABLE();

	os_printf("input init!!!\r\n");

	sntp_initialize();
}

//======================= GPIO interrupt callback =======================================================
static void cntr_intr_callback() {


	uint32_t gpio_st = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	ETS_GPIO_INTR_DISABLE();
	if (gpio_st & BIT(4)) {
		os_printf("pulse!!!\r\n");
		os_timer_disarm(&cBounce_timer);
		os_timer_setfn(&cBounce_timer, (os_timer_func_t *) bounceDelay_callback, NULL);
		os_timer_arm(&cBounce_timer, BOUNCE_PERIOD, false);
	}
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_st);
	ETS_GPIO_INTR_ENABLE();

}
//==============================================================================
void ICACHE_FLASH_ATTR showUsage(uint8_t *period, uint8_t len) {
	char tmp[32][4];
	int i;
	for (i = 0; i < len; i++) {

		os_sprintf(tmp[i], "%d", period[i]);
		os_printf("%s;", tmp[i]);
	}
	os_printf("\n");
}
//==============================================================================
void ICACHE_FLASH_ATTR bounceDelay_callback(void *arg) {

	if ((gpio_input_get() & (1 << 4)) == 0)/*(gpio_read(button_pin) == 0)*/{

		periodUsage.totalCounter++; // common counter

		sntp_get_stamp();
		// daily counter reset
		if (periodUsage.currentDay != dateTime.date && dateTime.currYear != 1970) {
			periodUsage.currentDay = dateTime.date;
			periodUsage.dayCounter[periodUsage.currentDay] = 0;
		}
		periodUsage.dayCounter[periodUsage.currentDay]++;

		// monthly counter reset
		if (periodUsage.currentMonth != dateTime.month  && dateTime.currYear != 1970) {
			periodUsage.currentMonth = dateTime.month;
			periodUsage.monthCounter[periodUsage.currentMonth] = 0;
		}
		periodUsage.monthCounter[periodUsage.currentMonth]++;

		os_printf("write \r\n");
		WriteMem(1, 0, periodUsage.byte, sizeof(sPERIOD_USEGE));
		indicatorUpdate(periodUsage.dayCounter[periodUsage.currentDay]);

		showUsage(periodUsage.dayCounter, 32);
//		showUsage(periodUsage.monthCounter, 13);

//		char days[32][4];
//		int i;
//		for(i = 0; i < 32; i++) {
//
//			os_sprintf(days[i], "%d", periodUsage.dayCounter[i]);
//			os_printf("%s;", days[i]);
//		}
//		os_printf("\n");
	}
}
//================== SNTP ==================================================
void ICACHE_FLASH_ATTR sntp_initialize(void)
{
	ip_addr_t *addr = (ip_addr_t *)os_zalloc(sizeof(ip_addr_t));
	sntp_setservername(0, "us.pool.ntp.org"); // set server 0 by domain name
	sntp_setservername(1, "ntp.sjtu.edu.cn"); // set server 1 by domain name
//	ipaddr_aton("210.72.145.44", addr);
//	sntp_setserver(2, addr); // set server 2 by IP address
	sntp_init();
	os_free(addr);

	sntp_set_timezone(3);
}

//==========================================================================
sDATE_TIME dateTime;
//================== SNTP ==================================================
void ICACHE_FLASH_ATTR sntp_get_stamp(void)
{
	dateTime = unixTimeToHumanReadable(sntp_get_current_timestamp());
	os_printf("%d:%d:%d %d.%d.%d, \n",
			dateTime.hours, dateTime.minutes, dateTime.seconds,
			dateTime.date, dateTime.month, dateTime.currYear);
}

//==========================================================================
void litrToMeter3(uint32 litr, char *out) {
	//char out[30];
	char fract[] = "000";

	int tmpi = litr % 1000;
	fract[2] = tmpi % 10 + '0';
	tmpi /= 10;
	fract[1] = tmpi % 10 + '0';
	tmpi /= 10;
	fract[0] = tmpi % 10 + '0';
	tmpi /= 10;
	os_sprintf(out, "%d.%s", litr / 1000, fract);
	//return out;
}
//==========================================================================
sDATE_TIME unixTimeToHumanReadable(uint32 seconds)
{

    // Save the time in Human
    // readable format
    //char ans[64] = "";

    // Number of days in month
    // in normal year
    int daysOfMonth[] = { 31, 28, 31, 30, 31, 30,
                          31, 31, 30, 31, 30, 31 };

    long int currYear, daysTillNow, extraTime,
        extraDays, index, date, month, hours,
        minutes, secondss, flag = 0;

    // Calculate total days unix time T
    daysTillNow = seconds / (24 * 60 * 60);
    extraTime = seconds % (24 * 60 * 60);
    currYear = 1970;

    // Calculating currrent year
    while (daysTillNow >= 365) {
        if (currYear % 400 == 0
            || (currYear % 4 == 0
                && currYear % 100 != 0)) {
            daysTillNow -= 366;
        }
        else {
            daysTillNow -= 365;
        }
        currYear += 1;
    }

    // Updating extradays because it
    // will give days till previous day
    // and we have include current day
    extraDays = daysTillNow + 1;

    if (currYear % 400 == 0
        || (currYear % 4 == 0
            && currYear % 100 != 0))
        flag = 1;

    // Calculating MONTH and DATE
    month = 0, index = 0;
    if (flag == 1) {
        while (true) {

            if (index == 1) {
                if (extraDays - 29 < 0)
                    break;
                month += 1;
                extraDays -= 29;
            }
            else {
                if (extraDays
                        - daysOfMonth[index]
                    < 0) {
                    break;
                }
                month += 1;
                extraDays -= daysOfMonth[index];
            }
            index += 1;
        }
    }
    else {
        while (true) {

            if (extraDays
                    - daysOfMonth[index]
                < 0) {
                break;
            }
            month += 1;
            extraDays -= daysOfMonth[index];
            index += 1;
        }
    }

    // Current Month
    if (extraDays > 0) {
        month += 1;
        date = extraDays;
    }
    else {
        if (month == 2 && flag == 1)
            date = 29;
        else {
            date = daysOfMonth[month - 1];
        }
    }

    // Calculating HH:MM:YYYY
    hours = extraTime / 3600;
    minutes = (extraTime % 3600) / 60;
    secondss = (extraTime % 3600) % 60;
    sDATE_TIME dateTime;
    dateTime.date = date;
    dateTime.month = (month);
    dateTime.currYear = (currYear);
    dateTime.hours = (hours);
    dateTime.minutes = (minutes);
    dateTime.seconds = (secondss);

    // Return the time
    return dateTime;
}

