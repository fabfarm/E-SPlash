#ifndef TIME_HANDLER_H
#define TIME_HANDLER_H

#include "Config.h"

void printCompilationTimestamp();
void printRtcDateTime(const RtcDateTime &dt);
void syncInternalRtcWithExternal(const RtcDateTime &dt);
void initializeRtc();
void printRtcDateTimeInLoop();
void initializeInternetTimeSync();
void syncWithInternetTime();
void handleInternetTimeSync();
void updateTimezone();

#endif // WIFIHANDLER_H