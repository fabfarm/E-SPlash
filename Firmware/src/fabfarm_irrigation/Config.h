#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESP32Time.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <Wire.h>
#include <RtcDS3231.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

#include "WebServerManager.h"
#include "WiFiHandler.h"
#include "JsonHandler.h"
#include "RelayHandler.h"
#include "TimeHandler.h"
#include "Board_Config.h"
#include "storageHandler.h"

extern WiFiMulti wifiMulti;

#define BOARD_CONFIG configs[1]

extern int pumpPinNumber;
extern const char *wifi_network_hostname;
extern const char *soft_ap_ssid;
extern const char *soft_ap_password;

#define ds_3231
#define TTL_Logic_Low

#define countof(a) (sizeof(a) / sizeof(a[0]))

#ifdef ds_3231
extern RtcDS3231<TwoWire> Rtc;
extern int sdaPin;
extern int sclPin;
#endif
#ifdef ds_1302
extern ThreeWire myWire;
extern RtcDS1302<ThreeWire> Rtc;
#endif
#ifdef TTL_Logic_Low
#define ON LOW
#define OFF HIGH
#else
#define ON HIGH
#define OFF LOW
#endif

extern ESP32Time rtc;
extern AsyncWebServer server;
extern JsonDocument doc;
extern boolean isScheduleMode;

#endif // CONFIG_H