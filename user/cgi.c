/*
 Some random cgi routines. Used in the LED example and the page that returns the entire
 flash as a binary. Also handles the hit counter on the main page.
 */

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain 
 * this notice you can do whatever you want with this stuff. If we meet some day, 
 * and you think this stuff is worth it, you can buy me a beer in return. 
 * ----------------------------------------------------------------------------
 */

#include <esp8266.h>
#include "cgi.h"
#include "io.h"
#include "pulseCounter.h"
#include "ramtron.h"
#include "LED_7_SEGMENT.h"

//cause I can't be bothered to write an ioGetLed()
static char currLedState = 0;

//Cgi that turns the LED on or off according to the 'led' param in the POST data
int ICACHE_FLASH_ATTR cgiLed(HttpdConnData *connData) {
	int len;
	char buff[1024];

	if (connData->conn == NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	len = httpdFindArg(connData->post->buff, "led", buff, sizeof(buff));
	if (len != 0) {
		currLedState = atoi(buff);
		ioLed(currLedState);
	}

	httpdRedirect(connData, "led.tpl");
	return HTTPD_CGI_DONE;
}

// cgiCValue
int ICACHE_FLASH_ATTR cgiCValue(HttpdConnData *connData) {
	int len;
	char buff[1024];

	httpd_printf("cntvalue.tpl ulr: %s\n", connData->url);

	if (connData->conn == NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}

	len = httpdFindArg(connData->post->buff, "value", buff, sizeof(buff));
	if (len != 0) {
		periodUsage.totalCounter = atoi(buff);
		//WriteMem(1, 0, (uint8*)&pulseCntr, 4);
		WriteMem(1, 0, periodUsage.byte, sizeof(sPERIOD_USEGE));
		indicatorUpdate(periodUsage.totalCounter);
		httpd_printf("new counter value: %d\n", (int )periodUsage.totalCounter);
	}

	httpdRedirect(connData, "cntvalue.tpl");
//	sntp_get_stamp();

	return HTTPD_CGI_DONE;
}

//Template code for the led page.
int ICACHE_FLASH_ATTR tplLed(HttpdConnData *connData, char *token, void **arg) {
	char buff[128];
	if (token == NULL)
		return HTTPD_CGI_DONE;

	os_strcpy(buff, "Unknown");
	if (os_strcmp(token, "ledstate") == 0) {
		if (currLedState) {
			os_strcpy(buff, "on");
		} else {
			os_strcpy(buff, "off");
		}
	}
	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}

//===========================================================================
int ICACHE_FLASH_ATTR monthlyUsage(HttpdConnData *connData, char *token,
		void **arg) {
	char buff[128];
	if (token == NULL)
		return HTTPD_CGI_DONE;

	char _token[] = "month__";
	for (int i = 1; i < 13; i++) {
		_token[5] = i/10 + '0';
		_token[6] = i%10 + '0';
		if (os_strcmp(token, _token) == 0) {
//			os_sprintf(buff, "%d", periodUsage.monthCounter[i]);
			litrToMeter3(periodUsage.monthCounter[i], buff);
		}
	}
	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}
//===========================================================================
int ICACHE_FLASH_ATTR dailyUsage(HttpdConnData *connData, char *token,
		void **arg) {
	char buff[128];
	if (token == NULL)
		return HTTPD_CGI_DONE;

	char _token[] = "day__";
	for (int i = 1; i < 32; i++) {
		_token[3] = i/10 + '0';
		_token[4] = i%10 + '0';
		if (os_strcmp(token, _token) == 0) {
			//os_sprintf(buff, "%d", periodUsage.dayCounter[i]);
			litrToMeter3(periodUsage.dayCounter[i], buff);
		}
	}
	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}

//Template code for the counter on the index page.
int ICACHE_FLASH_ATTR tplCounter(HttpdConnData *connData, char *token,
		void **arg) {
	char buff[128];
	if (token == NULL)
		return HTTPD_CGI_DONE;

	if (os_strcmp(token, "counter") == 0) {
//		os_sprintf(buff, "%d.%s", periodUsage.totalCounter / 1000, tmp);
		litrToMeter3(periodUsage.totalCounter, buff);

//		httpd_printf("tmp: %s\n", tmp);
		indicatorUpdate(periodUsage.dayCounter[periodUsage.currentDay]);
	}
	httpdSend(connData, buff, -1);
	return HTTPD_CGI_DONE;
}

