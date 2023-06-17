#include  "FileManager.h"
#include "Config.h"

void setupStorage(){
  if (!LittleFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  readDataJson();
}

