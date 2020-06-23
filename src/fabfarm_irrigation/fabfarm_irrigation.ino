
/**
 *  LucioPGN  
 * Contributors:
 *  Jeffrey Knight http://github.com/jknight
 *
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>

#include "Adafruit_Sensor.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "AsyncTCP.h"
#include "DHT.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "WiFi.h"

#include "printFarmTime.cpp"
#include "modifiedPrintLocalTime.cpp"

// read / write json to save state
const char *dataFile = "data.json";

// Specify the port of the Async server
AsyncWebServer server(80);
// Specifing the capacity of the json in bytes.

int jasonSize = 2048;
DynamicJsonDocument doc(jasonSize); // from arduinoJson

void setup(){
  // Serial port for debugging purposes
  Serial.begin(9600);    

  // Initialize SPIFFS
  // That is the file system.
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // we always read the data.json from disk on startup (always!)
  // If freshlly burned we have to send the sample json... TODO: generate json if not existent
  // open the json file with the name "data.json" from SPIFFS ...
  File f = SPIFFS.open("/data.json", "r");
  // Declares a String named json from the data contained in f????
  String json = f.readString();
  Serial.println("read file - BEGIN");
  Serial.println(json);
  Serial.println("read file - COMPLETE");
  //closes the file
  f.close();

  // we take json from memory & create json object
  Serial.println("json deserialize test - BEGIN");
  deserializeJson(doc, json);

  //
  //test override value in Jason
  //Serial.print("Printing override value in void setup after deserialisation: ");
  //JsonArray data = doc["data"];
  //int override = data["data"]["override"];
  //Serial.println(override);
  //delay(1000);

  // TODO: proactively disable everything / consider if we want to have it start in stopped state
  // TODO: set OUTPUT for each relay
  // TODO: also set each to off initially
  //pinMode(relayGPIOs[i - 1], OUTPUT);
  //digitalWrite(relayGPIOs[i - 1], HIGH);

  //Serial.println("Reading ssid: %s / password: %s from json\n", ssid, password);
  
  //Soft Wifi Access point setup
  WiFi.softAP("softap", "imakestuff");
  IPAddress IP = WiFi.softAPIP();
  //start wifi sessions as a client.
  //Wifi client setup
  const char* ssid = doc["data"]["ssid"];
  const char* password = doc["data"]["pass"];
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  // Print ESP32 Local IP Address
  Serial.println("The Fabfarm Irrigation system network IP is:");
  Serial.println(WiFi.localIP());

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
  server.on("/getData", HTTP_GET, [](AsyncWebServerRequest *request)
    {
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
  char json[2048];
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
  File f = SPIFFS.open("/data.json", "r");
  // Declares a String named json from the data contained in f????
  String json = f.readString();
  deserializeJson(doc, json);
  JsonObject data = doc["data"];
  boolean data_override = data["override"];
  f.close();

  if (data_override == 0){
    Serial.println("Manual Mode");
    manualMode();
  }
  else{
    Serial.println("Schedule Mode");
    scheduleMode();
  }

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








