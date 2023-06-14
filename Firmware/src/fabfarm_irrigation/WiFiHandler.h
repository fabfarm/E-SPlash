#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include "config.h"
// #include <Arduino.h>
// #include <ArduinoJson.h>
// #include <AsyncJson.h>
// #include <DHT.h>
// #include <SPIFFS.h>
// #include <ESP32Time.h>
// // #include <AsyncElegantOTA.h>
// #include <WiFi.h>
// #include <WiFiMulti.h>

#include "Fabfarm_Irrigation.h" 

void startSoftAP();
void startWifi();
void wifiLoop();
void scanWifi();

#endif // WIFIHANDLER_H