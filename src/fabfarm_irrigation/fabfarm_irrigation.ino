
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
#define cout Serial
//#define endl '\n'
//#include <String.h>

//#include <Time.h>
//#include <SPI.h>
//#include <DS3234RTC.h>
//#include <avr/pgmspace.h>
//#include <MemoryFree.h>
//#define lcloffset (3600UL)*4 //summer offset from UCT
//#include <Streaming.h>


// read / write json to save state
const char *dataFile = "data.json";

// Specify the port of the Async server
AsyncWebServer server(80);
// Specifing the capacity of the json in bytes.

int jasonSize = 2048;
DynamicJsonDocument doc(jasonSize); // from arduinoJson

void setup(){
  
  //put all relays in LOW at startup
  //TODO write to Json as well otherwise it reactivates
  allRelaysdisable();

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
  tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_AP, "irrigation");
  //start wifi sessions as a client.
  //Wifi client setup
  const char* ssid = doc["data"]["ssid"];
  const char* password = doc["data"]["pass"];
  ssid = "rato";
  password = "imakestuff";
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
  server.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/logo.png", "image/png");
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
  data["currentTime"] = printFarmTime();
  data["temperature"] = readDHTTemperature();
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
    manualMode();
  }
  else{
    scheduleMode();
  }

//*****end of loop*****
}

void scheduleMode(){
  Serial.println("now Schedule Mode");
  delay(1000);

  //matrix logic test
  JsonArray relays = doc["relays"];
  JsonObject times = relays.createNestedObject();
  for (int i = 0; i < relays.size(); i++) 
  {
    delay(2000);
    Serial.print("Print value of P: ");
    Serial.println(i);
    //the 3 in the test bellow needs to be replaced by the size of times in each pin
    for (int j = 0; j < relays[i]["times"].size(); j++) {
      int pin = relays[i]["pin"];
      const char* relaysStartTime = relays[i]["times"][j]["startTime"]; // "12:00"
      int hOurs = relays[i]["times"][j]["hour"];;
      int mIns = relays[i]["times"][j]["min"];;
      int relaysDuration = relays[i]["times"][j]["duration"]; // "0"

      Serial.println("**********************");
      Serial.print("Print value of P: ");
      Serial.println(i);
      Serial.print("Print value of T: ");
      Serial.println(j);
      Serial.print("Print value of pin: ");
      Serial.println(pin);
      Serial.print("Print value of Start Time: ");
      Serial.println(relaysStartTime);
      Serial.print("Print only hour: ");
      Serial.println(hOurs);
      Serial.print("Print only Min: ");
      Serial.println(mIns);
      Serial.print("Print value of duration: ");
      Serial.println(relaysDuration);
      Serial.println("**********************");
      /*
      int StartTimeMinutes = (hOurs*60+mIns);
      int timeNow = 120;
      int timer=0;
      if(timeNow == relaysStartTime){
      timer=millis();
      digitalWrite (pin, HIGH);
      if((millis() - timer) >= relaysDuration) {
        digitalWrite (pin, LOW);  
      }
      */

      //int farmtempo = 4;
/*
      if (farm time is different than time windows)
      {
        digitalWrite(pin[p], HIGH);
      }
      else
      {
        digitalWrite(pin[p], LOW);
      }
      */
    }
  }
}

void manualMode()
{
  Serial.println("now Manual Mode");
  delay(1000);

  JsonArray relays = doc["relays"];
  for (int i = 0; i < relays.size(); i++)
  {
    const char *relayName = relays[i]["name"]; // "relay1"
    int pin = relays[i]["pin"];                // 123
    int isEnabled = relays[i]["isEnabled"];    // 1
    pinMode(pin, OUTPUT);
    digitalWrite(pin, isEnabled ? HIGH : LOW);
    }
}

//function to deactivate all pins usefull for safe startup
//write to json
void allRelaysdisable(){
  File f = SPIFFS.open("/data.json", "r");
  // Declares a String named json from the data contained in f????
  String json = f.readString();
  deserializeJson(doc, json);
  JsonObject data = doc["data"];
  f.close();
    JsonArray relays = doc["relays"];
    for (int p = 0; p < relays.size(); p++)
  {
    int pin = relays[p]["pin"];
    pinMode(pin, OUTPUT);
    digitalWrite(pin,LOW);
  }
}


void checkTime()
{
 
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

void activateValveThanPump(){

//If valveRelay "armed" AND the current time minus the start time is greater than x millisecons (pumpRelay, ditalwrite, HIGH);
//If pumpRelay "armed" AND current reading is outside threshold, reset "armed"

}

void activatePumpThanValve(){

//when pumpRelay disarmed --> "LOW" AND the current time minus the stop time is greater than x millisecons (valveRelay, ditalwrite, LOW);


}

void checktime () {
/*

        */
}








