#include "Config.h"

int pumpPinNumber = PUMP_PIN;
const char *wifi_network_hostname = WIFI_HOSTNAME;
const char *soft_ap_ssid = AP_SSID;
const char *soft_ap_password = AP_PASSWORD;

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
