#ifndef TIME_HANDLER_H
#define TIME_HANDLER_H

#include "Config.h"

void printCompileTime();
void printDateTime(const RtcDateTime &dt);
void updateInternalRTC(const RtcDateTime &dt);
void setupRTC();
void testRtcOnLoop();

#endif // WIFIHANDLER_H