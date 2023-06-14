#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <DHT.h>
#include <SPIFFS.h>
#include <ESP32Time.h>
// #include <AsyncElegantOTA.h>
#include <WiFi.h>
#include <WiFiMulti.h>

extern WiFiMulti wifiMulti;

#include <Wire.h>
#include <RtcDS3231.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

struct BoardConfig {
  const char* name;
  int pump_pin;
  int voltage_pin;
  int dht_pin;
  int dht_type;
  const char* wifi_hostname;
  const char* ap_ssid;
  const char* ap_password;
  #ifdef static_IP
  IPAddress local_IP;
  #endif
};

extern const BoardConfig configs[];

#define BOARD_CONFIG configs[1]

extern int pumpPinNumber;
extern int batVoltPin;
extern const char* wifi_network_hostname;
extern const char *soft_ap_ssid;
extern const char *soft_ap_password;
#ifdef static_IP
extern IPAddress local_IP;
#endif

#define ds_3231
#define TTL_Logic_Low
#define DHTPIN BOARD_CONFIG.dht_pin
#define DHTTYPE BOARD_CONFIG.dht_type

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

extern IPAddress gateway;
extern IPAddress subnet;
extern IPAddress primaryDNS;
extern IPAddress secondaryDNS;

extern ESP32Time rtc;
extern const char *dataFile;
extern AsyncWebServer server;
extern int jsonSize;
extern DynamicJsonDocument doc;
extern boolean isScheduleMode;

#endif // CONFIG_H