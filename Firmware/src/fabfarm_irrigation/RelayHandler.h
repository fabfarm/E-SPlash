#ifndef RELAYHANDLER_H
#define RELAYHANDLER_H

#include "Config.h"

bool isWithinTimeslot(int startTimeInMinutes, int duration, JsonArray days);
unsigned long getNextScheduleEventTime();
void switchDevice(int pinNumber, bool state, const char *deviceName);
void switchDeviceQuiet(int pinNumber, bool state, const char *deviceName, bool &lastState, bool &hasChanged);
bool handleDevicesWithStateTracking(JsonArray devices, bool (*shouldEnableDevice)(JsonObject &));
bool shouldEnableDeviceInScheduleMode(JsonObject &device);
void scheduleMode();
void manualMode();
bool shouldEnableDeviceInManualMode(JsonObject &device);
void disableAllDevices();

#endif // RELAYHANDLER_H
