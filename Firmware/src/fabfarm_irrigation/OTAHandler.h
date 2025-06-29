#ifndef OTA_HANDLER_H
#define OTA_HANDLER_H

#include "Config.h"
#include <ArduinoOTA.h>
#include <ESPmDNS.h>

// OTA Configuration
extern const char* ota_hostname;
extern const char* ota_password;

// OTA Functions
void initializeOTA();
void handleOTA();
void setupOTACallbacks();
bool isOTAActive();

#endif // OTA_HANDLER_H
