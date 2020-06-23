/****************************************************************************
 *                                  Aknowledments                           *
 *                                  by LucioPGN                             *
 ****************************************************************************/
/*  So much I learned since started this journey in 07th of June 2020 I still 
 *  don't consider myself a programer.
 *  So I need to stand on top of giants sholders for my programing projects:
 *  A small Portion of this code was based on Rui Santos Code:
 *  https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system/
 *  https://randomnerdtutorials.com/esp32-relay-module-ac-web-server/
 *  https://randomnerdtutorials.com/esp32-date-time-ntp-client-server-arduino/
 *  From Techtutorialsx.com
 *  https://techtutorialsx.com/2017/12/01/esp32-arduino-asynchronous-http-webserver/
 *  Probabilly unrecognisable but still Shakeels code for ESP8266 is somewhere here;
 *  Jeff joined the effort in the 20th and with food he coded until 5am introducing:
 *    -A new way to parse data with Json file.
 * 
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
//#include <Arduino.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>

//Required Libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "AsyncTCP.h"
#include "Adafruit_Sensor.h"
#include "DHT.h"

#include "AsyncJson.h"
#include "ArduinoJson.h"

#include "printFarmTime.cpp"
#include "modifiedPrintLocalTime.cpp"

// read / write json to save state
const char *dataFile = "data.json";

// Network Credentials
// TODO get this credentials from jason file
const char *ssid = "rato";
const char *password = "imakestuff";

//Start the Async Web Server listening on port 80
AsyncWebServer server(80);

// "the brains" - a json object
DynamicJsonDocument doc(2048); // from arduinoJson

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(9600);

  // Initialize SPIFFS
  //That is the file system.
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // we always read the data.json from disk on startup (always!)
  // Read json from the file ...

  File f = SPIFFS.open("/data.json", "r");
  String json = f.readString();
  Serial.println("read file - BEGIN");
  Serial.println(json);
  Serial.println("read file - COMPLETE");
  f.close();

  // we take json from disk & create json object
  Serial.println("json deserialize test - BEGIN");
  deserializeJson(doc, json);

  // TODO: set OUTPUT for each relay
  // TODO: also set each to off initially
  //pinMode(relayGPIOs[i - 1], OUTPUT);
  //digitalWrite(relayGPIOs[i - 1], HIGH);

  //Lucio TODO: proactively disable everything / consider if we want to have it start in stopped state

  WiFi.softAP("softap", "imakestuff");
  IPAddress IP = WiFi.softAPIP();

  //here  start wifi sessions as a client.
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println("The Fabfarm Irrigation system network IP is:");
  Serial.println(WiFi.localIP());

  //printShortFarmTime();
  // Get time from time server

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/all.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/all.css", "text/css");
  });
  server.on("/logo.jpeg", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/logo.jpeg", "image/jpeg");
  });

  server.on("/getData", HTTP_GET, [](AsyncWebServerRequest *request) {
    /* 
    1) start with our json object.
    2) we get new data (temp/humidit/y/relay status)
    3) update the json object
    4) serialize ==> json
    5) return json to html 
    */

    Serial.println("/getData");
    JsonObject data = doc["data"];
    data["currentTime"] = printFarmTime(); // TODO: why aren't times working ?
    data["temperature"] = readDHTTemperature(); // TODO: fix me
    data["humidity"] = readDHTHumidity();
    char json[1024];
    Serial.println("Serialize json & return to caller - BEGIN");
    serializeJson(doc, json);
    Serial.println("Serialize json & return to caller - COMPLETE");
    request->send(200, "application/json", json);
  });

  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/updateData", [](AsyncWebServerRequest *request, JsonVariant &json) {
    doc = json.as<JsonObject>();

    String jsonString;
    serializeJson(doc, jsonString);

    //write this to disk
    // Read json from the file ...
    Serial.println("Saving to disk - BEGIN");
    File f = SPIFFS.open("/data.json", "w");
    if (!f)
    {
      Serial.println("Faile to open file for writing");
    }

    int bytesWritten = f.print(jsonString);
    f.close();
    Serial.printf("Saving to disk - COMPLETE(%d bytes)\n", bytesWritten);

    request->send(200); // "application/json", jsonString);
    Serial.println("-------------------");
    Serial.println(jsonString);
    Serial.println("-------------------");
  });
  
  server.addHandler(handler);
  // Start server here
  server.begin();
}

void loop()
{
  JsonArray data = doc["data"];
  int override = data["override"];
  Serial.print("Printing override value inside void loop: ");
  Serial.println(override);
  delay(1000);

  /*if (override == 1){
    Serial.println(override);
    manualMode();
  }
  else{
    Serial.println(override);
    scheduleMode();
  }*/

  //*****end of loop*****
}

void scheduleMode(){
  //Serial.println("Schedule Mode");
  delay(1000);
}

void manualMode()
{
  //Serial.println("Manual Mode");
  delay(1000);

  JsonArray relays = doc["relays"];
  for (int i = 0; i < relays.size(); i++)
  {

    const char *relayName = relays[i]["name"]; // "relay1"
    int pin = relays[i]["pin"];                // 123
    int isEnabled = relays[i]["isEnabled"];    // 1
    int duration = relays[i]["duration"];      // 1000
    pinMode(pin, OUTPUT);
    digitalWrite(pin, isEnabled ? HIGH : LOW);
    }
  /* 
  for (int j = 0; j < relays[i]["times"].size(); j++)
  {
    JsonObject times = relays[i][j].createNestedObject();
    const char *startTime = times["startTime"]; // = "12:45";
    int duration = times["duration"];           // = 25;
    ---> turn on and off in here <---
  }
  */
  //      Serial.println("looping ...");
  //turns things on and off: digitalWrite(relayGPIOs[i - 1], RELAY_NO ? HIGH : LOW);
  //if (digitalRead(relayGPIOs[valveRelayNum - 1]))
}

String readDHTTemperature()
{
  // Digital pin connected to the DHT sensor
  #define DHTPIN 32
  // Uncomment the type of sensor in use:
  #define DHTTYPE DHT11 // DHT 11
  //#define DHTTYPE    DHT22     // DHT 22 (AM2302)
  //#define DHTTYPE    DHT21     // DHT 21 (AM2301)
  //Send the pin and type of sensor
  DHT dht(DHTPIN, DHTTYPE);
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return "Failed to read temperator from sensor";
  }
  else
  {
    Serial.println(t);
    return String(t);
  }
}

String readDHTHumidity()
{
  // Digital pin connected to the DHT sensor
  #define DHTPIN 32
  // Uncomment the type of sensor in use:
  #define DHTTYPE DHT11 // DHT 11
  //#define DHTTYPE    DHT22     // DHT 22 (AM2302)
  //#define DHTTYPE    DHT21     // DHT 21 (AM2301)
  //Send the pin and type of sensor
  DHT dht(DHTPIN, DHTTYPE);
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  if (isnan(h))
  {
    Serial.println("Failed to read from DHT sensor!");
    return "Failed to read humidity from sensor";
  }
  else
  {
    Serial.println(h);
    return String(h);
  }
}
