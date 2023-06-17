#include  "FileManager.h"
#include "Config.h"

void setupStorage(){
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  readDataJson();
}

