#ifndef RELAYHANDLER_H
#define RELAYHANDLER_H

#include "Config.h"

void scheduleMode();
void manualMode();
void disableAllRelays();
void switchPump(bool state);
bool isWithinTimeslot(int startTimeInMinutes, int duration);

#endif // RELAYHANDLER_H