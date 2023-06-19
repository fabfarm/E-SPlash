#include "JsonHandler.h"

bool writeDataJson()
{
    Serial.println("JSON serialize - BEGIN");
    String jsonString;
    serializeJson(doc, jsonString);
    Serial.println("JSON serialize - COMPLETE");
    return writeFile("/data.json", jsonString);
}

void readDataJson()
{
    Serial.println("Starting to look for JSON file");
    String json = LittleFS.exists("/data.json") ? readFile("/data.json") : readFile("/sample.json");
    Serial.println("JSON deserialize - BEGIN");
    deserializeJson(doc, json);
    Serial.println("JSON deserialize - COMPLETE");
}

