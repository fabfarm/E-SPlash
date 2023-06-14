#include "Config.h"

// Define configuration settings for different boards
const BoardConfig configs[] = {
  {
    "0_point_1_main",               // Board name
    33,                             // Pump pin
    35,                             // Voltage read pin
    21,                             // DHT sensor pin
    DHT11,                          // DHT sensor type
    "irrigationmain",               // WiFi network hostname
    "irrigation_main",              // Soft AP SSID
    ""                            // Soft AP password
  },
  {
    "0_point",                      // Board name
    13,                             // Pump pin
    35,                             // Voltage read pin
    21,                             // DHT sensor pin
    DHT11,                          // DHT sensor type
    "test",                         // WiFi network hostname
    "irrigation_main_prototype",    // Soft AP SSID
    ""                            // Soft AP password
  },
  {
    "_1_point_0",                   // Board name
    13,                             // Pin number for pump
    35,                             // Pin number for voltage reading
    21,                             // Pin number for DHT sensor
    DHT11,                          // DHT sensor type (-1=None, 11=DHT11, 22=DHT22, 21=AM2301)
    "test",                         // Hostname of WiFi network
    "irrigation_test",              // Soft AP SSID
    ""                             // Soft AP password
  },
  {
    "_0_point_greenhouse",           // Board name
    33,                              // Pin number for pump
    35,                              // Pin number for voltage reading
    21,                              // Pin number for DHT sensor
    -1,                              // DHT sensor type (-1=None, 11=DHT11, 22=DHT22, 21=AM2301)
    "greenhousetestboard",           // Hostname of WiFi network
    "irrigation_greenhousetest",     // Soft AP SSID
    ""
  }
};


int pumpPinNumber = BOARD_CONFIG.pump_pin; 
int batVoltPin = BOARD_CONFIG.voltage_pin; 
const char* wifi_network_hostname = BOARD_CONFIG.wifi_hostname; 
const char *soft_ap_ssid = BOARD_CONFIG.ap_ssid; 
const char *soft_ap_password = BOARD_CONFIG.ap_password;

#ifdef ds_3231
RtcDS3231<TwoWire> Rtc(Wire);
int sdaPin = 05;
int sclPin = 17;
#endif

#ifdef ds_1302
ThreeWire myWire(14, 13, 32);
RtcDS1302<ThreeWire> Rtc(myWire);
#endif

IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

ESP32Time rtc;
const char *dataFile = "data.json";
AsyncWebServer server(80);
int jsonSize = 1800;
DynamicJsonDocument doc(jsonSize);
boolean isScheduleMode = false;

WiFiMulti wifiMulti;
