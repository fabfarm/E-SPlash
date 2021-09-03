/*
 * This code is part of a Irrigation Sytem developed as my final project,
 * in the Fabacademy course, full documentation can be viewed on the link:
 * http://fabacademy.org/2020/labs/algarve/students/lucio/index.html
 * 
 * Project page: http://github.com/fabfarm/esplash
 */



#include "ArduinoJson.h"
#include "AsyncJson.h"
#include "DHT.h"
#include "SPIFFS.h"
#include <ESP32Time.h>
#include "AsyncElegantOTA.h"
#include <WiFi.h>

ESP32Time rtc;
const char *dataFile = "data.json";// read / write json to save state
AsyncWebServer server(80);// Specify the port of the Async server
int jasonSize = 1800;// Specifing the capacity of the json in bytes.
DynamicJsonDocument doc(jasonSize); // from arduinoJson

//chosing the type of board if 0.1(_0_point_1) or 1.0(_1_point_0)
//#define _0_point_1_main
#define _0_point_greenhouse
//#define _0_point// prototype relays on board pcb designed with eagle
//#define _1_point_0// second prototype no relays on board pcb designed with eagle
#ifdef _0_point
  #define ds_1302
  #define TTL_Logic_Low
  //Defining pump pin number
  int pumpPinNumber = 33;
  //Define Voltage read pin number
  int batVoltPin = 35;

  const char* wifi_network_hostname = "test";

  const char* wifi_network_ssid = "caravan";
  const char* wifi_network_password =  "imakestuff";
  
  const char *soft_ap_ssid = "irrigation_main";
  const char *soft_ap_password = "";
  // Set your Static IP address
  IPAddress local_IP(192, 168, 1, 22);
#endif
#ifdef _0_point_1_main
  #define ds_1302
  #define TTL_Logic_Low
  //Defining pump pin number
  int pumpPinNumber = 33;
  //Define Voltage read pin number
  int batVoltPin = 35;

  const char* wifi_network_hostname = "irrigationmain";

  const char* wifi_network_ssid = "caravan";
  const char* wifi_network_password =  "imakestuff";
  
  const char *soft_ap_ssid = "irrigation_main";
  const char *soft_ap_password = "";
  // Set your Static IP address
  IPAddress local_IP(192, 168, 1, 23);
#endif
#ifdef _1_point_0
  #define ds_3231
  #define TTL_Logic_Low
  //Defining pump pin number
  int pumpPinNumber = 33;
  //Define Voltage read pin number
  int batVoltPin = 35;

  const char* wifi_network_hostname = "test";

  const char* wifi_network_ssid = "fabfarm_ele_container";
  const char* wifi_network_password =  "gerryforever2018";
  
  const char *soft_ap_ssid = "irrigation_test";
  const char *soft_ap_password = "";
  // Set your Static IP address
  IPAddress local_IP(192, 168, 1, 24);
#endif
#ifdef _0_point_greenhouse
  #define ds_3231
  #define TTL_Logic_Low
  //Defining pump pin number
  int pumpPinNumber = 13;
  //Define Voltage read pin number
  int batVoltPin = 35;

  const char* wifi_network_hostname = "greenhousetestboard";

  const char* wifi_network_ssid = "ratinha_do_malandro";
  const char* wifi_network_password =  "gerryforever2018";
  
  const char *soft_ap_ssid = "irrigation_greenhouse";
  const char *soft_ap_password = "";
  // Set your Static IP address
  IPAddress local_IP(192, 168, 1, 25);
#endif

//define the tipe of external RTC

//
#ifdef ds_3231
  #include <Wire.h>
  #include <RtcDS3231.h>
  RtcDS3231<TwoWire> Rtc(Wire); 
  int sdaPin = 05;//SDA
  int sclPin = 17;//SCL
#endif
#ifdef ds_1302
  #include <ThreeWire.h>
  #include <RtcDS1302.h>
  ThreeWire myWire(14,13,32); // DAT, CLK, RST
  RtcDS1302<ThreeWire> Rtc(myWire);
#endif

//included option to use relays with TTL Logic LOW. Comment to use high

#ifdef TTL_Logic_Low
  #define ON   LOW
  #define OFF  HIGH
#else
  #define ON   HIGH
  #define OFF  LOW
#endif


// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

//**************************************************************************************************************
// *****************************************Setup starts here***************************************************
//**************************************************************************************************************

void setup(){
  Serial.begin(9600);
  initWiFi();  //function with wifi settings and initialisation
  server.begin();// Start server
  allRelaysdisable();  //put all relays in LOW at startup: (question? does it reactivates unatendelly like previously noted?)
  compileTimePrinting(); //prints compile time at serial monitor
  //external rtc initiation
  #ifdef ds_3231
  Wire.begin(sdaPin, sclPin); // SDA, SCL
  #endif

  Rtc.Begin();
  first_rtc_function();  //this function will do a series of logical tests on external rtc in order to set its time in case is needed and print then  status
  
  //Initialize SPIFFS (file system)
  if (!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  createDataJson();//creates a data.json file in case it does not exist

  // we always read the data.json from disk on startup (always!)
  // If freshlly burned we have to send the sample json... TODO: generate json if not existent
  // open the json file with the name "data.json" from SPIFFS ...
  File f = SPIFFS.open("/data.json", "r");
  // Declares a String named json from the data contained in f????
  String json = f.readString();
  Serial.println("read file - BEGIN");
  Serial.println(json);
  Serial.println("read file - COMPLETE");
  f.close();  //closes the file


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
    server.on("/fa-solid-900.woff2", HTTP_GET, [](AsyncWebServerRequest * request) {
      Serial.println("Send : fa-solid-900 css");
    request->send(SPIFFS, "/fa-solid-900.woff2", "application/x-font-woff2");
      Serial.println("Sended : fa-solid-900 css");
  });
    server.on("/fa-solid-900.woff", HTTP_GET, [](AsyncWebServerRequest * request) {
      Serial.println("Send : fa-solid-900 css");
    request->send(SPIFFS, "/fa-solid-900.woff", "application/x-font-woff");
      Serial.println("Sended : fa-solid-900 css");
  });
    server.on("/fa-solid-900.ttf", HTTP_GET, [](AsyncWebServerRequest * request) {
      Serial.println("Send : fa-solid-900 css");
    request->send(SPIFFS, "/fa-solid-900.ttf", "application/x-font-ttf");
      Serial.println("Sended : fa-solid-900 css");
  });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/logo.png", "image/png");
  });
    // Route to load images file
  server.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/logo.png", "image/png");
  });
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(SPIFFS, "/script.js", "text/javascript");
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
    char json[1800];
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
    //Read json from the file ...
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

}

//**************************************************************************************************************
// ********************************Loop Stats here************************************************************** 
//**************************************************************************************************************

void loop(){
  
  //testRtcOnLoop();  //uncomment testRtcOnLoop() to display time every second on serial monitor

  AsyncElegantOTA.loop();

  JsonObject data = doc["data"];
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

//This function simply prints the compile time.
void compileTimePrinting(){
  Serial.println("This program was Compiled on: ");
  Serial.print("date: ");
  Serial.println(__DATE__);
  Serial.print("time: ");
  Serial.println(__TIME__);
}
//This function creates a file data.json based on the file sample.json in case the data.json does not exists.
void createDataJson() {
  Serial.println("Starting looking for Json file");
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  if(!SPIFFS.exists("/data.json")){
    Serial.println("data.json doe not exist! Creating it now!");
    File dataJsonReadfile = SPIFFS.open("/sample.json", "r");
    String fileString = dataJsonReadfile.readString();
    dataJsonReadfile.close();
    File dataJsonwritefile = SPIFFS.open("/data.json", "w");  
    dataJsonwritefile.print(fileString);
    dataJsonwritefile.close();
    if(!SPIFFS.exists("/data.json")){
      Serial.println("data.json was not created!");
      return;
    }
    return;
  }
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
      // Serial.print("Zone ");
      String zoneName = relays[i]["name"];
      // Serial.print(zoneName);
      // Serial.println(" is Enabled!");
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
      // Serial.print("Zone ");
      String zoneName = relays[i]["name"];
      // Serial.print(zoneName);
      // Serial.println(" is off");
    }
  }
}
void manualMode()
{
  
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
      // Serial.print("Zone ");
      String zoneName = relays[i]["name"];
      // Serial.print(zoneName);
      // Serial.println(" is off");
    }
  }
}

//function to deactivate all pins usefull for safe startup not finished yet
void allRelaysdisable(){
  pinMode (pumpPinNumber, OUTPUT);  //defining behaviour of pumpPinNumber and its startup state
  digitalWrite (pumpPinNumber, OFF);
  //JsonObject data = doc["data"];
  JsonArray relays = doc["relays"];
  for (int p = 0; p < relays.size(); p++)
  {
    int pinNumber = relays[p]["pin"];
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, OFF);
  }
}

//This function reads the temperature sensor data from the DHT
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

//This function reads the humidity sensor data from the DHT
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
// function is used to evaluate if a certain relay should be low or hight depending on the time it was set to run
int isEnabledFunc (int startTimeInMinutes, int duration)
{


  int onlyHour;
  int onlyMin;

  onlyHour = rtc.getHour(true);
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

//This function reads the battery level voltage in order to determine battery percentage
float batLevel(){
  analogRead(batVoltPin);
  float batteryLevel = map(analogRead(batVoltPin), 0.0f, 1866.0f, 0, 100);
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
//This function assigns local time based on an internet server
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

//
#define countof(a) (sizeof(a) / sizeof(a[0]))
void printDateTime(const RtcDateTime& dt)
{
  char datestring[20];

  snprintf_P(datestring, 
          countof(datestring),
          PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
          dt.Month(),
          dt.Day(),
          dt.Year(),
          dt.Hour(),
          dt.Minute(),
          dt.Second() );
  Serial.println(datestring);
}

//Function to update internal RTC using External RTC
void updateInternalRTC(const RtcDateTime& dt)
{
  char datestring[20];

  snprintf_P(datestring, 
          countof(datestring),
          PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
          dt.Month(),
          dt.Day(),
          dt.Year(),
          dt.Hour(),
          dt.Minute(),
          dt.Second() );
  rtc.setTime(dt.Second(),dt.Minute(),dt.Hour(),dt.Day(),dt.Month(),dt.Year());

  Serial.println("--------------------------------------------------------------------------------------");
  Serial.println("--------------------------------------------------------------------------------------");
  Serial.println("This data is a data stream from function void updateInternalRTC(const RtcDateTime& dt)");
  Serial.println(datestring);
  Serial.println("End of data from function updateInternalRTC(const RtcDateTime& dt)");
  Serial.println("--------------------------------------------------------------------------------------");
  Serial.println("--------------------------------------------------------------------------------------");
}

//This function
void first_rtc_function()
{ 
  Serial.println("**********************************************************************************");
  Serial.println("**********************************************************************************");
  Serial.println("**********************************************************************************");
  Serial.println("This data comes from void first_rtc_function()");
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  // Print the Time before updated from external RTC
  Serial.println("Time is from from internal RTC on boot");
  Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));
  // Configure internal RTC from external rtc time
  updateInternalRTC(Rtc.GetDateTime());
  // Print the Time updated from external RTC
  Serial.println("Time from internal RTC after external RTC update");
  Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));

  if (!Rtc.IsDateTimeValid()) 
  {
      // Common Causes:
      //    1) first time you ran and the device wasn't running yet
      //    2) the battery on the device is low or even missing

      Serial.println("RTC lost confidence in the DateTime!");
      Rtc.SetDateTime(compiled);
  }
  #ifdef ds_3231
  #else
  if (Rtc.GetIsWriteProtected())
  {
    Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }
  #endif

  if (!Rtc.GetIsRunning())
  {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) 
  {
      Serial.println("RTC is older than compile time!  (Updating DateTime)");
      Rtc.SetDateTime(compiled);
  }
  else if (now > compiled) 
  {
      Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled) 
  {
      Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }
      Serial.println();

  Serial.println("END of data from first external RTC function");
  Serial.println("**********************************************************************************");
  Serial.println("**********************************************************************************");
  Serial.println("**********************************************************************************");

}
//This function prints the clock on serial monitor with a predetermined interval

void testRtcOnLoop(){
  //this data is for the functions that need an interval and use millis to instead of delay 
  unsigned long previousMillis = 0;
  const long printTimeInterval = 10000;

  Serial.println("This data comes from void testRtcOnLoop() time will apear only every 1 second" );
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= printTimeInterval) 
  {
    previousMillis = currentMillis;
    RtcDateTime now = Rtc.GetDateTime();
    printDateTime(Rtc.GetDateTime());
    Serial.println();
    if (!now.IsValid())
    {
      // Common Causes:
      //    1) the battery on the device is low or even missing and the power line was disconnected
      Serial.println("RTC lost confidence in the DateTime!");
    }
  }
  Serial.println("End of data from void testRtcOnLoop()" );
}

void Wifi_connected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Successfully connected to Access Point");
}

void Get_IPAddress(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("WIFI is connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("hostname: ");
  Serial.println(WiFi.getHostname());

}

void Wifi_disconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WIFI access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  Serial.println("Reconnecting...");
  WiFi.begin(wifi_network_ssid, wifi_network_password);
}
//This function initiates wifi
void initWiFi()
{
  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.disconnect(true);
  WiFi.onEvent(Wifi_connected,SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(Get_IPAddress, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(Wifi_disconnected, SYSTEM_EVENT_STA_DISCONNECTED); 
  WiFi.softAP(soft_ap_ssid, soft_ap_password, 3);
  Serial.print("ESP32 IP as soft AP: ");
  Serial.println(WiFi.softAPIP());
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(wifi_network_ssid, wifi_network_password);
  WiFi.setHostname(wifi_network_hostname);
  Serial.println("Waiting for WIFI network...");
}

// This function will select to update time either from the internet or from a manual entry in the html that populates the json
// void internetOrManualTime()
// {
//   // Call function that assigns the array in the json to the rtc of ESP32
//   Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));
//   boolean data_internettime = data["changedtime"][0]["internettime"] ;
//   //boolean data_manualtimeflag = data["changedtime"][0]["manualtimeenabled"];
//   if ( data_internettime  == 1)
//   {
//     Serial.println("Time updated using internet");
//       for(static bool first = true;first;first=false)
//       { 
//         	AssignLocalTime();
//       }
//   }
//   if ( data_internettime  == 0 )
//   { 
//     Serial.println("Time updated using manual input");
//     for(static bool first = true;first;first=false)
//     { 
//       changetime();  
//     }
//   }
// }

// function to change time using the time that was input in the json
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

// Function in developmentto display status on serial monitor only when changed state
//void generalStatusPrint()
// {
//   oldValue = isEnabled;
//   if(oldValue != newValue)
//   {
//     Serial.println(newValue);
//     oldValue = newValue;
//   }
// }