/****************************************************************************
 *                                  Aknowledments                           * 
 *                                  by LucioPGN                             * 
 ****************************************************************************/
/*  Up to this date: 07th of June 2020 I don't consider myself a programer 
 *  so I need to stand on top of giants sholders for my programing projects:
 *  A Portion of this code was based on Rui Santos Code;
 *  A Portion of this code was based on Shakeels code for ESP8266 ;
 *  My contributions: 
 *     -So far I made it work on platformio :), that took me quite a lot of time
 *     -That means:
 *        +created a new project;
 *        +created a folder named data under the main folder (fabfarm_irrigation)
 *        +linked the platformio.ini to the folder of the project + the data folder
 *        +linked the needed libraries to their github repo in the platformio.ini
 *        +found a conflict with time library and A
 *  Things I still want to program for my final project:
 *    -so far I ported Shakeels code into ESP32;
 *    -separate functions from shakeels code into files;
 *    -separated functions from Rui Santos code into files.
 *    -simplify the code creating more functions;
 *    -try to separate the HTML part for a cleaner code;
 *    -Improve the appearance/Interface of the code
 *    -Add readings to HTML
 *    -Add a log of occurrences like over current
 *    -Add more safety for the equipment
 *    -Add a phone interface (APP)
 *    -Add function to set current time
 *    -Add renaming function to each relay so one can relate the relay to the area of interest or at least rename relays to actual areas of the farm.
 * 
 ****************************************************************************/
 


/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

// Replace with your network credentials
const char* ssid = "rato";
const char* password = "imakestuff";

// Set GPIO Pins
const int ledPin = 2;
const int pinValveRelay1 = 26;
const int pinValveRelay2 = 25;
const int pinValveRelay3 = 33;
const int pinPumpRelay = 32;
// RCT Pins CLK,DAT,RST
const int clkPin = 18;
const int datPin = 19;
const int rstPin = 21;

// Stores LED state
String ledState;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String& var){
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

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(ledPin, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Start server
  server.begin();
}
 
void loop(){
  
}