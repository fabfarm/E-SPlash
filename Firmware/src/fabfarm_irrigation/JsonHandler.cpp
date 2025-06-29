#include "JsonHandler.h"

bool writeDataJson()
{
    Serial.println("JSON serialize - BEGIN");
    String jsonString;
    serializeJson(doc, jsonString);
    Serial.println("JSON serialize - COMPLETE");
    return writeFile("/data.json", jsonString);
}

void initializeHardwarePins()
{
    Serial.println("Initializing hardware-specific pins");
    
    int relayPins[] = {RELAY1_PIN, RELAY2_PIN, RELAY3_PIN};
    
    // Update relay pins in JSON document
    JsonArray relays = doc["relays"];
    for (int i = 0; i < min((int)relays.size(), 3); i++) {
        if (relays[i]) {
            relays[i]["pin"] = relayPins[i];
            Serial.printf("Relay %d pin set to %d\n", i, relayPins[i]);
        }
    }
}

void readDataJson()
{
    Serial.println("Starting to look for JSON file");
    String json = LittleFS.exists("/data.json") ? readFile("/data.json") : readFile("/sample.json");
    Serial.println("JSON deserialize - BEGIN");
    deserializeJson(doc, json);
    Serial.println("JSON deserialize - COMPLETE");
    
    // Initialize hardware-specific pins after reading JSON
    initializeHardwarePins();
}

