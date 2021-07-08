
/*
 * This code is part of a Irrigation Sytem developed as my final project,
 * in the Fabacademy course, full documentation can be viewed on the link:
 * http://fabacademy.org/2020/labs/algarve/students/lucio/index.html
 * 
 * Project page: http://github.com/fabfarm/esplash
 */

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include "AsyncElegantOTA.h"
#include "Adafruit_Sensor.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "DHT.h"
#include "SPIFFS.h"
#include <ESP32Time.h>
ESP32Time rtc;

// read / write json to save state
const char *dataFile = "data.json";

// Specify the port of the Async server
AsyncWebServer server(80);

// Specifing the capacity of the json in bytes.
int jasonSize = 2048;
DynamicJsonDocument doc(jasonSize); // from arduinoJson

//Defining pump pin number
int pumpPinNumber = 33;

//Define Voltage read pin number
int batVolt = 35;

//Declaring wifi credentials
const char* ssid = "fabfarm_ele_container";
const char* password = "imakestuff";

//included option to use relays with TTL Logic LOW. Comment to use high
#define TTL_Logic_Low

#ifdef TTL_Logic_Low
  #define ON   LOW
  #define OFF  HIGH
#else
  #define ON   HIGH
  #define OFF  LOW
#endif

//**************************************************************************************************************
// *****************************************Setup starts here***************************************************
//**************************************************************************************************************

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

  //defining behaviour of pumpPinNumber and its startup state
  pinMode (pumpPinNumber, OUTPUT);
  digitalWrite (pumpPinNumber, OFF);
  
  //put all relays in LOW at startup
  //TODO write to Json as well otherwise it reactivates
  allRelaysdisable();

  // Configure a random time in the rtc of the ESP32
  rtc.setTime(1,1,1,1,1,2021);

  // Print the random time in the rtc
  Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));

  // Assign the time from the ntp server to the esp32 RTC

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
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false);
  });
  // Route for setup page / web page
  server.on("/setuppage.html", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(SPIFFS,"/setuppage.html", String(), false);
  });
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/all.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/all.css", "text/css");
  });
    // Route to load images file
  server.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/logo.png", "image/png");
  });
  // Route to 
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
  

  AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/updateData", 
  [](AsyncWebServerRequest *request, JsonVariant &json) {
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

  // Start ElegantOTA
  AsyncElegantOTA.begin(&server);

  // Start server here
  server.begin();
}

//**************************************************************************************************************
// ********************************Loop Stats here************************************************************** 
//**************************************************************************************************************

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
  //boolean data_manualtimeflag = data["changedtime"][0]["manualtimeenabled"];
  if ( data_internettime  == 1)
  {
    Serial.println("Time updated using internet");
      for(static bool first = true;first;first=false)
    { 
      AssignLocalTime();
    }
  }
if ( data_internettime  == 0 ){
    Serial.println("Time updated using manual input");
  for(static bool first = true;first;first=false)
  { 
   changetime();  
  }
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

//**************************************************************************************************************
//***********Bellow here only functions*************************************************************************
//**************************************************************************************************************

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
      pinMode(relays[i]["pin"], OUTPUT);
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
      digitalWrite(relays[i]["pin"], ON);
      digitalWrite(pumpPinNumber, ON);
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
        if (digitalRead (relays[y]["pin"]) == ON)
        {
          ++valveFlag;
        }
      }
      if (valveFlag == 0)
        {
          digitalWrite(pumpPinNumber, OFF);
        }
      digitalWrite(relays[i]["pin"], OFF);
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
      digitalWrite(relays[i]["pin"], ON);
      digitalWrite(pumpPinNumber, ON);
    }
    else
    {   
      int valveFlag = 0;
      for (int y = 0; y < relays.size(); y++)
      {
        if (digitalRead (relays[y]["pin"]) == ON)
        {
          ++valveFlag;
        }
      }
      if (valveFlag == 0)
        {
          digitalWrite(pumpPinNumber, OFF);
        }
      digitalWrite(relays[i]["pin"], OFF);
      Serial.print("Zone ");
      String zoneName = relays[i]["name"];
      Serial.print(zoneName);
      Serial.println(" is off");
    }
  }
}

//function to deactivate all pins usefull for safe startup not finished yet
void allRelaysdisable(){
  //JsonObject data = doc["data"];
    JsonArray relays = doc["relays"];
    for (int p = 0; p < relays.size(); p++)
  {
    int pinNumber = relays[p]["pin"];
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, OFF);
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


  int onlyHour;
  int onlyMin;

  onlyHour =rtc.getHour(true);
  onlyMin = rtc.getMinute();

  int presentTimeInMinutes = onlyHour*60+onlyMin;
  int isEnabled;
    if (startTimeInMinutes <= presentTimeInMinutes && presentTimeInMinutes <= startTimeInMinutes+duration){
    isEnabled = 1;
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
