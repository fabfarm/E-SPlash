#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

// Replaces placeholder with LED state value
String processor(const String& var){

// Set LED GPIO
const int ledPin = 2;
// Stores LED state // This was in the mail file before had to include since moved the function out of the file
String ledState;

  Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(ledPin)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  return String();
}