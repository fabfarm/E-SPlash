#include "WebServerManager.h"
#include "Config.h"
void serverHandle() {
    server.on("/data.json", HTTP_GET, handleDataJsonRequest);
    AsyncCallbackJsonWebHandler *updateData = new AsyncCallbackJsonWebHandler("/updateData", handleUpdateDataRequest);
    server.on("^\\/mode\\/((manual)|(scheduled))$", HTTP_GET, handleModeRequest);
    server.on("^\\/relay\\/([0-9]+)\\/((on)|(off))$", HTTP_GET, handleRelayRequest);
    AsyncCallbackJsonWebHandler *updateRelayTime = new AsyncCallbackJsonWebHandler("/relay/update-time", handleUpdateRelayTimeRequest);
    AsyncCallbackJsonWebHandler *addRelayTime = new AsyncCallbackJsonWebHandler("/relay/add-time", handleAddRelayTimeRequest);
    server.on("^\\/relay\\/([0-9]+)\\/time\\/([0-9]+)$", HTTP_DELETE, handleDeleteRelayTimeRequest);
    AsyncCallbackJsonWebHandler *addRelay = new AsyncCallbackJsonWebHandler("/relay/add", handleAddRelayRequest);
    server.on("^\\/relay\\/([0-9]+)$", HTTP_DELETE, handleDeleteRelayRequest);

    server.addHandler(updateData);
    server.addHandler(updateRelayTime);
    server.addHandler(addRelayTime);
    server.addHandler(addRelay);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", String(), false);
    });

    server.on("^(\\/[a-zA-Z0-9_.-]*)$", HTTP_GET, [](AsyncWebServerRequest *request) {
        String file = request->pathArg(0);
        Serial.printf("Serving file %s\n\r", file.c_str());
        request->send(SPIFFS, file, String(), false);
    });

    isScheduleMode = doc["data"]["isScheduleMode"];
    if (!isScheduleMode) {
        manualMode();
    }
}


void handleDataJsonRequest(AsyncWebServerRequest *request) {
    Serial.println("/data.json");
    DynamicJsonDocument data = doc;

    data["data"]["currentTime"] = rtc.getTime("%A, %B %d %Y %H:%M");
    data["data"]["temperature"] = readDHTTemperature();
    data["data"]["humidity"] = readDHTHumidity();
    data["data"]["batLevel"] = getBatteryLevel();

    char jsonReply[1800];
    Serial.println("Serialize JSON & return to caller - BEGIN");
    serializeJson(data, jsonReply);
    Serial.println(jsonReply);
    request->send(200, "application/json", jsonReply);
    Serial.println("Serialize JSON & return to caller - COMPLETE");
}

void handleUpdateDataRequest(AsyncWebServerRequest *request, JsonVariant &json) {
    doc = json.as<JsonObject>();

    if (writeDataJson()) {
        request->send(200);
    } else {
        request->send(500);
    }
}

void handleModeRequest(AsyncWebServerRequest *request) {
    isScheduleMode = request->pathArg(0).equals("scheduled");
    doc["data"]["isScheduleMode"].set(isScheduleMode);

    if (writeDataJson()) {
        if (isScheduleMode) {
            scheduleMode();
        } else {
            manualMode();
        }

        request->send(200);
    } else {
        request->send(500);
    }
}

void handleRelayRequest(AsyncWebServerRequest *request) {
    if (isScheduleMode) {
        request->send(405);
    }

    int relayIndex = request->pathArg(0).toInt();
    bool state = request->pathArg(1).equals("on");

    Serial.printf("Relay %d: %d\n\r", relayIndex, state);
    doc["relays"][relayIndex]["isEnabled"].set(state);

    if (writeDataJson()) {
        manualMode();
        request->send(200);
    } else {
        request->send(500);
    }
}

void handleUpdateRelayTimeRequest(AsyncWebServerRequest *request, JsonVariant &json) {
    int relayIndex = json["relayIndex"];
    int timeIndex = json["timeIndex"];
    doc["relays"][relayIndex]["times"][timeIndex]["startTime"].set(json["startTime"]);
    doc["relays"][relayIndex]["times"][timeIndex]["duration"].set(json["duration"]);

    if (writeDataJson()) {
        request->send(200);
    } else {
        request->send(500);
    }
}

void handleAddRelayTimeRequest(AsyncWebServerRequest *request, JsonVariant &json) {
    JsonObject nested = doc["relays"][json["relayIndex"]]["times"].as<JsonArray>().createNestedObject();
    nested["startTime"] = "10:00";
    nested["duration"] = 30;

    if (writeDataJson()) {
        request->send(200);
    } else {
        request->send(500);
    }
}

void handleDeleteRelayTimeRequest(AsyncWebServerRequest *request) {
    int relayIndex = request->pathArg(0).toInt();
    int timeIndex = request->pathArg(1).toInt();

    doc["relays"][relayIndex]["times"].as<JsonArray>().remove(timeIndex);

    if(writeDataJson()) {
        request->send(200); 
    } else {
        request->send(500); 
    }
}

void handleAddRelayRequest(AsyncWebServerRequest *request, JsonVariant &json) {
    JsonObject nested = doc["relays"].as<JsonArray>().createNestedObject();

    const size_t CAPACITY = JSON_ARRAY_SIZE(3);

    StaticJsonDocument<CAPACITY> newArray;

    JsonArray array = newArray.to<JsonArray>();
    JsonObject newTime = array.createNestedObject();
    newTime["startTime"] = "10:00";
    newTime["duration"] = 30;
    nested["times"] = array;

    nested["name"] = json["name"];
    nested["pin"] = json["pin"];
    nested["isEnabled"] = false;

    if (writeDataJson()) {
        request->send(200);
    } else {
        request->send(500);
    }
}

void handleDeleteRelayRequest(AsyncWebServerRequest *request) {
    int relayIndex = request->pathArg(0).toInt();
    Serial.printf("Removing relay %d\n\r", relayIndex);
    doc["relays"].as<JsonArray>().remove(relayIndex);

    if (writeDataJson()) {
        request->send(200);
    } else {
        request->send(500);
    }
}

