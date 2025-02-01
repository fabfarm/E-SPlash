#include "Config.h"

int pumpPinNumber = BOARD_CONFIG.pump_pin;
int batVoltPin = BOARD_CONFIG.voltage_pin;
const char *wifi_network_hostname = BOARD_CONFIG.wifi_hostname;
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

ESP32Time rtc;
AsyncWebServer server(80);
JsonDocument doc;
boolean isScheduleMode = false;

WiFiMulti wifiMulti;
