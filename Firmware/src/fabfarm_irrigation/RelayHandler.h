#ifndef RELAYHANDLER_H
#define RELAYHANDLER_H

#include "Config.h"

bool isWithinTimeslot(int startTimeInMinutes, int duration);
unsigned long getNextScheduleEventTime();
void switchDevice(int pinNumber, bool state, const char *deviceName);
void switchDeviceQuiet(int pinNumber, bool state, const char *deviceName, bool &lastState, bool &hasChanged);
void handleDevices(JsonArray devices, bool (*shouldEnableDevice)(JsonObject &));
bool handleDevicesWithStateTracking(JsonArray devices, bool (*shouldEnableDevice)(JsonObject &));
bool shouldEnableDeviceInScheduleMode(JsonObject &device);
void scheduleMode();
bool shouldEnableDeviceInManualMode(JsonObject &device);
void manualMode();
void disableAllDevices();

#endif // RELAYHANDLER_H