#ifndef CGI_H
#define CGI_H

#include "httpd.h"

int cgiLed(HttpdConnData *connData);
int tplLed(HttpdConnData *connData, char *token, void **arg);
int tplCounter(HttpdConnData *connData, char *token, void **arg);
int cgiCValue(HttpdConnData *connData);
int monthlyUsage(HttpdConnData *connData, char *token,void **arg);
int  dailyUsage(HttpdConnData *connData, char *token, void **arg);

#endif
