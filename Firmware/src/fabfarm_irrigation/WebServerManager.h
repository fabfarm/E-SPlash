#ifndef WEB_SERVER_MANAGER_H
#define WEB_SERVER_MANAGER_H

#include "Config.h"

void serverHandle();
void handleDataJsonRequest(AsyncWebServerRequest *request);
void handleUpdateDataRequest(AsyncWebServerRequest *request, JsonVariant &json);
void handleModeRequest(AsyncWebServerRequest *request);
void handleRelayRequest(AsyncWebServerRequest *request);
void handleUpdateRelayTimeRequest(AsyncWebServerRequest *request, JsonVariant &json);
void handleAddRelayTimeRequest(AsyncWebServerRequest *request, JsonVariant &json);
void handleDeleteRelayTimeRequest(AsyncWebServerRequest *request);
void handleAddRelayRequest(AsyncWebServerRequest *request, JsonVariant &json);
void handleDeleteRelayRequest(AsyncWebServerRequest *request);

#endif // WEB_SERVER_MANAGER_H