#include "WebServerManager.h"
#include "Config.h"

// HTTP status codes
const int HTTP_OK = 200;
const int HTTP_INTERNAL_SERVER_ERROR = 500;
const int HTTP_METHOD_NOT_ALLOWED = 405;

// Helper function to serialize JSON and send response
void sendJsonResponse(AsyncWebServerRequest *request, JsonDocument &data)
{
    char jsonReply[1800];
    serializeJson(data, jsonReply);
    request->send(HTTP_OK, "application/json", jsonReply);
}

// Handler for GET /data.json
void handleGetDataJsonRequest(AsyncWebServerRequest *request)
{
    Serial.println("/data.json");
    JsonDocument data = doc;

    data["data"]["currentTime"] = rtc.getTime("%A, %B %d %Y %H:%M");

    sendJsonResponse(request, data);
}

// Handler for POST /updateData
void handleUpdateDataRequest(AsyncWebServerRequest *request, JsonVariant &json)
{
    doc = json.as<JsonObject>();

    // Check if UTC offset was updated and reconfigure timezone
    if (json["data"]["changedtime"][0]["utcOffset"].is<int>()) {
        updateTimezone();
    }

    if (writeDataJson())
    {
        request->send(HTTP_OK);
    }
    else
    {
        request->send(HTTP_INTERNAL_SERVER_ERROR);
    }
}

// Handler for GET /mode/(manual|scheduled)
void handleModeChangeRequest(AsyncWebServerRequest *request)
{
    isScheduleMode = request->pathArg(0).equals("scheduled");
    doc["data"]["isScheduleMode"].set(isScheduleMode);

    if (writeDataJson())
    {
        if (isScheduleMode)
        {
            scheduleMode();
        }
        else
        {
            manualMode();
        }

        request->send(HTTP_OK);
    }
    else
    {
        request->send(HTTP_INTERNAL_SERVER_ERROR);
    }
}

// Handler for GET /relay/(number)/(on|off)
void handleRelayRequest(AsyncWebServerRequest *request)
{
    if (isScheduleMode)
    {
        request->send(HTTP_METHOD_NOT_ALLOWED);
    }

    int relayIndex = request->pathArg(0).toInt();
    bool state = request->pathArg(1).equals("on");

    Serial.printf("Relay %d: %d\n\r", relayIndex, state);
    doc["relays"][relayIndex]["isEnabled"].set(state);

    if (writeDataJson())
    {
        manualMode();
        request->send(HTTP_OK);
    }
    else
    {
        request->send(HTTP_INTERNAL_SERVER_ERROR);
    }
}

// Handler for POST /relay/update-time
void handleUpdateRelayTimeRequest(AsyncWebServerRequest *request, JsonVariant &json)
{
    int relayIndex = json["relayIndex"];
    int timeIndex = json["timeIndex"];
    doc["relays"][relayIndex]["times"][timeIndex]["startTime"].set(json["startTime"]);
    doc["relays"][relayIndex]["times"][timeIndex]["duration"].set(json["duration"]);

    if (writeDataJson())
    {
        request->send(HTTP_OK);
    }
    else
    {
        request->send(HTTP_INTERNAL_SERVER_ERROR);
    }
}

// Handler for POST /relay/add-time
void handleAddRelayTimeRequest(AsyncWebServerRequest *request, JsonVariant &json)
{
    JsonObject nested = doc["relays"][json["relayIndex"]]["times"].as<JsonArray>().add<JsonObject>();
    nested["startTime"] = "10:00";
    nested["duration"] = 30;

    if (writeDataJson())
    {
        request->send(HTTP_OK);
    }
    else
    {
        request->send(HTTP_INTERNAL_SERVER_ERROR);
    }
}

// Handler for DELETE /relay/(number)/time/(number)
void handleDeleteRelayTimeRequest(AsyncWebServerRequest *request)
{
    int relayIndex = request->pathArg(0).toInt();
    int timeIndex = request->pathArg(1).toInt();

    doc["relays"][relayIndex]["times"].as<JsonArray>().remove(timeIndex);

    if (writeDataJson())
    {
        request->send(HTTP_OK);
    }
    else
    {
        request->send(HTTP_INTERNAL_SERVER_ERROR);
    }
}

// Handler for POST /relay/add
void handleAddRelayRequest(AsyncWebServerRequest *request, JsonVariant &json)
{
    JsonObject nested = doc["relays"].as<JsonArray>().add<JsonObject>();

    JsonDocument newArray;

    JsonArray array = newArray.to<JsonArray>();
    JsonObject newTime = array.add<JsonObject>();
    newTime["startTime"] = "10:00";
    newTime["duration"] = 30;
    nested["times"] = array;

    nested["name"] = json["name"];
    nested["pin"] = json["pin"];
    nested["isEnabled"] = false;

    if (writeDataJson())
    {
        request->send(HTTP_OK);
    }
    else
    {
        request->send(HTTP_INTERNAL_SERVER_ERROR);
    }
}

// Handler for DELETE /relay/(number)
void handleDeleteRelayRequest(AsyncWebServerRequest *request)
{
    int relayIndex = request->pathArg(0).toInt();
    Serial.printf("Removing relay %d\n\r", relayIndex);
    doc["relays"].as<JsonArray>().remove(relayIndex);

    if (writeDataJson())
    {
        request->send(HTTP_OK);
    }
    else
    {
        request->send(HTTP_INTERNAL_SERVER_ERROR);
    }
}

void serverHandle()
{
    server.on("/data.json", HTTP_GET, handleGetDataJsonRequest);
    AsyncCallbackJsonWebHandler *updateData = new AsyncCallbackJsonWebHandler("/updateData", handleUpdateDataRequest);
    server.on("^\\/mode\\/((manual)|(scheduled))$", HTTP_GET, handleModeChangeRequest);
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

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/index.html", String(), false); });

    server.on("^(\\/[a-zA-Z0-9_.-]*)$", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String file = request->pathArg(0);
        Serial.printf("Serving file %s\n\r", file.c_str());
        request->send(LittleFS, file, String(), false); });

    isScheduleMode = doc["data"]["isScheduleMode"];
    if (!isScheduleMode)
    {
        manualMode();
    }
}
