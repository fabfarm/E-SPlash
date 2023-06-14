#ifndef FABFARM_IRRIGATION_H
#define FABFARM_IRRIGATION_H

#include "config.h"
#include "WiFiHandler.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <DHT.h>
#include <SPIFFS.h>
#include <ESP32Time.h>
// #include <AsyncElegantOTA.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <Wire.h>
#include <RtcDS3231.h>

bool writeDataJson();

void setupSerialMonitor();
void setupWiFi();
void printCompileTime();
void readDataJson();
void setupStorage();
void scheduleMode();
void manualMode();
void disableAllRelays();
void switchPump(bool state);
bool isWithinTimeslot(int startTimeInMinutes, int duration);
float readDHTTemperature();
float readDHTHumidity();
float getBatteryLevel();
void assignLocalTime();
void printDateTime(const RtcDateTime &dt);
void updateInternalRTC(const RtcDateTime &dt);
void setupRTC();
void testRtcOnLoop();
void setupWifi();
void serverHandle();

#endif //FABFARM_IRRIGATION_H