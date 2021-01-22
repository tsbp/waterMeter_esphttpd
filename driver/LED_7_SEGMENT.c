/*
 * LED_7_SEGMENT.c
 *
 *  Created on: 29 вер. 2016
 *      Author: Voodoo
 */
//==============================================================================
#include "ets_sys.h"
#include "osapi.h"
//#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
//#include "driver/uart.h"
//#include "driver/gpio16.h"
#include "LED_7_SEGMENT.h"
#include "spi_interface.h"
#include <esp8266.h>
//======================================================================
ETSTimer display_timer;
static void  display_loop(void *arg);

#define DISPLAY_PERIOD		(5) // in msec
#define user_procTaskPrio        0

//static void  display_loop(os_event_t *events);

//==============================================================================
uint8 blink = 0;
uint8 disp[3] = {13, 14, 15};
//==============================================================================
uint8 digit[] = {DIG_0, DIG_1, DIG_2, DIG_3, DIG_4,
				 DIG_5, DIG_6, DIG_7, DIG_8, DIG_9,
				 DIG__, DIG_F, DIG_n, DIG_A, DIG_L,
				 DIG_r, DIG_S, DIG_t, DIG_b};
//==============================================================================
#define HSPI_LED7_CS_PIN (1 << 15)//(8) // GPIO15
//==============================================================================
//PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
//
////	gpio_write(HSPI_CS_PIN, 1);
//	gpio_output_set(HSPI_CS_PIN, 0, HSPI_CS_PIN, 0);
//	os_printf("hspi_init end \r\n");
void ICACHE_FLASH_ATTR indicatorInit(void) {

//	set_gpio_mode(HSPI_LED7_CS_PIN, GPIO_PULLUP, GPIO_OUTPUT);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_GPIO15);

	//gpio_write(HSPI_LED7_CS_PIN, 1);
	gpio_output_set(HSPI_LED7_CS_PIN, 0, HSPI_LED7_CS_PIN, 0);

	// Start display timer
		os_timer_disarm(&display_timer);
		os_timer_setfn(&display_timer,  display_loop, NULL);
		os_timer_arm(&display_timer, DISPLAY_PERIOD, true);

}
//==============================================================================
void ICACHE_FLASH_ATTR serialSendData(uint32_t aWord)
{
//	//os_printf("%04x\r\n", lcdWord.word);
//	hspi_send_uint16((uint16)aWord);
//	hspi_wait_ready();

//	gpio_write(HSPI_LED7_CS_PIN, 0);
	gpio_output_set(0, HSPI_LED7_CS_PIN, 0, HSPI_LED7_CS_PIN);

	SpiData spiData;
	spiData.cmd = 0;
	spiData.cmdLen = 0;
	spiData.addr = 0;
	spiData.addrLen = 0;
	spiData.data = (uint32_t*) &aWord;
	spiData.dataLen = 2; //32;
	SPIMasterSendData(SpiNum_HSPI, &spiData);

	//gpio_write(HSPI_LED7_CS_PIN, 1);
	gpio_output_set(HSPI_LED7_CS_PIN, 0, HSPI_LED7_CS_PIN, 0);

}
//==============================================================================
void display(uint8 *aDisp, uint8 aBlink)
{
	static int blink = 100;
	static int pos = 0;
	pos++ ;
	if(pos >= 3) pos = 0;

	if (blink) blink--;
	else blink = 200;

	if (aBlink & (1 << pos))
	{
		if (blink > 100) serialSendData(((~digit[aDisp[pos]]) << 8) + (1 << (pos + 1)));
		else  serialSendData(((~digit[10]) << 8) + (1 << (pos + 1)));
	}
	else			serialSendData(((~digit[aDisp[pos]]) << 8) + (1 << (pos + 1)));
}

//======================================================================
static void  display_loop(void *arg)
{
	display(disp, blink);
}
//======================================================================
void copyScreen(uint8 *aInBuf, uint8 *aOutBuf)
{
	int i;
	for(i = 0; i < 3; i++) aInBuf[i] = aOutBuf[i];
}
//==============================================================================
void ICACHE_FLASH_ATTR indicatorUpdate(unsigned long pulseCntr) {

	unsigned long tmp = pulseCntr % 1000;

	disp[2] = tmp % 10;
	tmp /= 10;
	disp[1] = tmp % 10;
	tmp /= 10;
	disp[0] = tmp % 10;
}
