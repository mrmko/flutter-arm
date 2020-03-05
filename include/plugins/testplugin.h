#ifndef _TEST_PLUGIN_H
#define _TEST_PLUGIN_H

#include <stdio.h>
#include <string.h>

#define TESTPLUGIN_CHANNEL_JSON "flutter-arm/testjson"
#define TESTPLUGIN_CHANNEL_STD "flutter-arm/teststd"
#define TESTPLUGIN_CHANNEL_PING "flutter-arm/ping"

extern int testp_init(void);
extern int testp_deinit(void);

#endif