#include  "JsonHandler.h"
#include "Config.h"

bool writeDataJson()
{
  String jsonString;
  Serial.println("JSON serialize - BEGIN");
  serializeJson(doc, jsonString);
  Serial.println("JSON serialize - COMPLETE");

  Serial.println("Write JSON file - BEGIN");
  File f = SPIFFS.open("/data.json", "w");
  if (!f)
  {
    Serial.println("Failed to open file for writing");
    Serial.println("Write JSON file - FAILED");
    return false;
  }
  else
  {
    int bytesWritten = f.print(jsonString);
    Serial.println(jsonString);
    Serial.printf("Write JSON file - COMPLETE (%d bytes)\n\r", bytesWritten);

    f.close();

    return true;
  }
}
void readDataJson()
{
  Serial.println("Starting to look for JSON file");

  File f;
  if (SPIFFS.exists("/data.json"))
  {
    f = SPIFFS.open("/data.json", "r");
  }
  else
  {
    Serial.println("data.json does not exist yet, reading sample.json instead!");
    f = SPIFFS.open("/sample.json", "r");
  }

  Serial.println("Reading JSON file - BEGIN");
  String json = f.readString();
  Serial.println(json);
  Serial.println("Reading JSON file - COMPLETE");

  f.close();

  Serial.println("JSON deserialize - BEGIN");
  deserializeJson(doc, json);
  Serial.println("JSON deserialize - COMPLETE");
}
