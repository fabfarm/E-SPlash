
/**
 * This code is part of a Irrigation Sytem developed as my final project,
 * in the Fabacademy course, full documentation can be viewed on the link:
 * http://fabacademy.org/2020/labs/algarve/students/lucio/index.html
 * 
 * The begining of the code was based on the works of Rui Santos. With the 
 * contributions of Jeff and my obssession in learning to code its significantly 
 * different a Special thanks to Jeffrey Knight who without I would be probabily
 * struglyng with the most advanced parts of the project.
 * 
 * Project page: http://github.com/fabfarm/esplash
 * 
 * Contributors:
 *  Lucio PGN http://github.com/lpgn
 *  Jeffrey Knight http://github.com/jknight
 */
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
//#include <string>
#include "AsyncElegantOTA.h"

#include "Adafruit_Sensor.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"

#include "DHT.h"
#include "SPIFFS.h"

// #include "printFarmTime.cpp"
//#include "modifiedPrintLocalTime.cpp"

#include <ESP32Time.h>
ESP32Time rtc;

// read / write json to save state
const char *dataFile = "data.json";

// Specify the port of the Async server
AsyncWebServer server(80);

// Specifing the capacity of the json in bytes.
int jasonSize = 1520;
DynamicJsonDocument doc(jasonSize); // from arduinoJson

//Defining pump pin number
int pumpPin = 13;
int batVolt = 35;

//Declaring wifi credentials
const char* ssid = "fabfarm_ele_container";
const char* password = "imakestuff";
// const char* ssid = doc["data"]["credentials"][0]["ssid"];
// const char* password = doc["data"]["credentials"][0]["ssid"];

void setup(){
  // Serial port for debugging purposes
  Serial.begin(9600);
  
  //start wifi sessions as a client.
  //Wifi client setup
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
    }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  //Soft Wifi Access point setup
  WiFi.softAP("softap", "imakestuff");
  IPAddress IP = WiFi.softAPIP();

  //defining behaviour of pumpPin and its startup state
  pinMode (pumpPin, OUTPUT);
  digitalWrite (pumpPin, LOW);
  
  //put all relays in LOW at startup
  //TODO write to Json as well otherwise it reactivates
  allRelaysdisable();

  // Configure a random time in the rtc of the ESP32
  rtc.setTime(1,1,1,1,1,2021);

  // Print the random time in the rtc
  Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));

  // Assign the time from the ntp server to the esp32 RTC
  // AssignLocalTime();
  // Print the updated time in the rtc
  Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));

  //Initialize SPIFFS
  //That is the file system.
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

  // Route for root / web page
    server.on("/setuppage.html", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(SPIFFS,"/setuppage.html", String(), false);
  });
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
  
  Serial.println("/getData");
  JsonObject data = doc["data"];
  data["currentTime"] = rtc.getTime("%A, %B %d %Y %H:%M:%S");
  data["temperature"] = readDHTTemperature();
  data["humidity"] = readDHTHumidity();
  data["batLevel"] = batLevel();
  char json[1520];
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
    Serial.println("Failed to open file for writing");
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

  //start OTA
  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  // Start server here
  server.begin();
}

void loop()
{
  AsyncElegantOTA.loop();
  if  (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting to WiFi..");
  }
  else
  {
    Serial.println("Connected to WiFi!");
    // Print ESP32 Local IP Address
    Serial.print("The Fabfarm Irrigation system network IP is:");
    Serial.println(WiFi.localIP());
  }
  Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));

  // Call function that assigns the array in the json to the rtc of ESP32
  JsonObject data = doc["data"];
  boolean data_internettime = data["changedtime"][0]["internettime"] ;
  if ( data_internettime  == 1){
    Serial.println("Time updated using internet");
  AssignLocalTime();
  // rtc.setTime(1,1,1,1,1,2021);
  // Serial.println("7mida");
  }
if ( data_internettime  == 0){
    Serial.println("Time updated using manual input");
   changetime();
  }

  boolean data_isScheduleMode = data["isScheduleMode"];

  if (data_isScheduleMode == 0){
    manualMode();
  }
  else{
    scheduleMode();
  }

//*****end of loop*****
}

void changetime (){

 JsonObject data = doc["data"];
 JsonArray data_changedtime = data["changedtime"];
 int mIn = data_changedtime[0]["min"];
 int hOur = data_changedtime[0]["hour"];
 int dAy = data_changedtime[0]["day"];
 int mOnth = data_changedtime[0]["month"];
 int yEar = data_changedtime[0]["year"];
 rtc.setTime(0,mIn,hOur,dAy,mOnth,yEar);
}

void scheduleMode(){

  //matrix logic test
  JsonArray relays = doc["relays"];
  for (int i = 0; i < relays.size(); i++){
    int flagEnableRelay = 0;
    for (int j = 0; j < relays[i]["times"].size(); j++) {
      //aparentelly there is no problem to set digital write several times as it only dows write a different value, need to check that. https://forum.arduino.cc/index.php?topic=52806.0
      pinMode(relays[i]["pin"], OUTPUT);
      const char* relaysStartTime = relays[i]["times"][j]["startTime"];
      int hOurs = relays[i]["times"][j]["hour"];
      int mIns = relays[i]["times"][j]["min"];
      int cycleDuration = relays[i]["times"][j]["duration"];
      //Probabilly should learn about bitwise... https://playground.arduino.cc/Code/BitMath/
      if (isEnabledFunc(hOurs*60+mIns, cycleDuration) == 1)
      {
        ++flagEnableRelay;
      }
    }
    if (flagEnableRelay >= 1)
    {
      digitalWrite(relays[i]["pin"], 1);
      digitalWrite(pumpPin, 1);
      Serial.print("Zone ");
      String zoneName = relays[i]["name"];
      Serial.print(zoneName);
      Serial.println(" is Enabled!");
    }
    else
    {   
      int valveFlag = 0;
      for (int y = 0; y < relays.size(); y++)
      {
        if (digitalRead (relays[y]["pin"]) == HIGH)
        {
          ++valveFlag;
        }
      }
      if (valveFlag == 0)
        {
          digitalWrite(pumpPin, 0);
        }
      digitalWrite(relays[i]["pin"], 0);
      Serial.print("Zone ");
      String zoneName = relays[i]["name"];
      Serial.print(zoneName);
      Serial.println(" is off");
    }
  }
}

void manualMode()
{
  Serial.println("now Manual Mode");
  JsonArray relays = doc["relays"];
  for (int i = 0; i < relays.size(); i++)
  {
    pinMode(relays[i]["pin"], OUTPUT);
    if (relays[i]["isEnabled"] == 1)
    {
      digitalWrite(relays[i]["pin"], 1);
      digitalWrite(pumpPin, 1);
    }
    else
    {   
      int valveFlag = 0;
      for (int y = 0; y < relays.size(); y++)
      {
        if (digitalRead (relays[y]["pin"]) == HIGH)
        {
          ++valveFlag;
        }
      }
      if (valveFlag == 0)
        {
          digitalWrite(pumpPin, 0);
        }
      digitalWrite(relays[i]["pin"], 0);
      Serial.print("Zone ");
      String zoneName = relays[i]["name"];
      Serial.print(zoneName);
      Serial.println(" is off");
    }
  }
}

//function to deactivate all pins usefull for safe startup not finished yet
void allRelaysdisable(){
  JsonObject data = doc["data"];
    JsonArray relays = doc["relays"];
    for (int p = 0; p < relays.size(); p++)
  {
    int pin = relays[p]["pin"];
    pinMode(pin, OUTPUT);
    digitalWrite(pin,LOW);
  }
}

String readDHTTemperature()
{
  // Digital pin connected to the DHT sensor
  #define DHTPIN 14
  // Uncomment the type of sensor in use:
  #define DHTTYPE DHT22 // DHT 11
  //#define DHTTYPE    DHT22     // DHT 22 (AM2302)
  //#define DHTTYPE    DHT21     // DHT 21 (AM2301)
  //Send the pin and type of sensor
  DHT dht(DHTPIN, DHTTYPE);
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  // float t = dht.readTemperature(true);
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
  #define DHTPIN 14
  // Uncomment the type of sensor in use:
  #define DHTTYPE DHT22 // DHT 11
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

int isEnabledFunc (int startTimeInMinutes, int duration)
{

  // const char *ntpServer = "us.pool.ntp.org";
  // const long gmtOffset_sec = 0;
  // const int daylightOffset_sec = 3600;
  int onlyHour;
  int onlyMin;
  int onlySec;
  // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // struct tm timeinfo;
  // getLocalTime(&timeinfo);
  // onlyHour = timeinfo.tm_hour;
  // onlyMin = timeinfo.tm_min;
  // onlySec = timeinfo.tm_sec;

  onlyHour =rtc.getHour(true);
  onlyMin = rtc.getMinute();
  onlySec = rtc.getSecond();

  int presentTimeInMinutes = onlyHour*60+onlyMin;
  int isEnabled;
    if (startTimeInMinutes <= presentTimeInMinutes && presentTimeInMinutes <= startTimeInMinutes+duration){
    isEnabled = 1;
    //Serial.println("time to start the pump!");
  }
    else
    {
      isEnabled = 0;
    }
  return isEnabled;
}

float batLevel(){
  analogRead(batVolt);
  float batteryLevel = map(analogRead(batVolt), 0.0f, 1866.0f, 0, 100);
  if (batteryLevel >= 100) {
    return 100;
  }
  else
  {
    Serial.print("Batery Level: ");
    Serial.print(batteryLevel);
    Serial.println("%");
    return batteryLevel;
  }  
}

void AssignLocalTime(){
  int THour; 
  int TMin; 
  int TSec; 
  int TYear;
  int TMonth;
  int TDay;
  const char* ntpServer = "pool.ntp.org";
  const long  gmtOffset_sec = 3600;
  const int   daylightOffset_sec = 0;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  TYear = timeinfo.tm_year + 1900;
  TMonth = timeinfo.tm_mon + 1;
  TDay = timeinfo.tm_mday;
  THour = timeinfo.tm_hour;
  TMin = timeinfo.tm_min;
  TSec = timeinfo.tm_sec;
  rtc.setTime(TSec,TMin,THour,TDay,TMonth,TYear);
  return ;
}
