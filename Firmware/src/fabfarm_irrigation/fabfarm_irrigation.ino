/*
 * This code is part of an Irrigation System developed as my final project for the Fabacademy course. 
 * Full documentation can be viewed on the link: http://fabacademy.org/2020/labs/algarve/students/lucio/index.html
 * Project page: http://github.com/fabfarm/esplash
 */
// Include necessary libraries
#include "ArduinoJson.h"    // For working with JSON data
#include "AsyncJson.h"      // For handling JSON data asynchronously
#include "DHT.h"            // For working with DHT sensors
#include "SPIFFS.h"         // For working with files on the flash file system
#include <ESP32Time.h>      // For working with time on the ESP32
#include "AsyncElegantOTA.h"// For over-the-air updates
#include <WiFi.h>           // For working with WiFi
#include <WiFiMulti.h>      // For handling multiple WiFi networks

// Create a WiFiMulti object to handle multiple WiFi networks
WiFiMulti wifiMulti;

//#define static_IP // if you want to use static IP, uncomment this line

// Define a struct to hold the configuration settings for different boards
struct BoardConfig {
  const char* name;         // Name of the board
  const char* rtc_type;     // RTC type
  const char* voltage_logic;// Voltage logic
  int pump_pin;             // Pin number for the pump
  int voltage_pin;          // Pin number for voltage read
  int dht_pin;              // Pin number for the DHT sensor
  int dht_type;             // Type of DHT sensor (-1 for none, DHT11, DHT22, DHT21)
  const char* wifi_hostname;// WiFi network hostname
  const char* ap_ssid;      // Soft AP SSID
  const char* ap_password;  // Soft AP password
  #ifdef static_IP
  IPAddress local_IP;       // Static IP address
  #endif
};

// Define configuration settings for different boards
const BoardConfig configs[] = {
  {
    "0_point_1_main",               // Board name
    "ds_3231",                      // RTC type
    "TTL_Logic_Low",                // Voltage logic
    33,                             // Pump pin
    35,                             // Voltage read pin
    21,                             // DHT sensor pin
    DHT11,                          // DHT sensor type
    "irrigationmain",               // WiFi network hostname
    "irrigation_main",              // Soft AP SSID
    "",                             // Soft AP password
    #ifdef static_IP
    IPAddress(192, 168, 1, 23)      // Static IP address
    #endif
  },
  {
    "0_point",                      // Board name
    "ds_3231",                      // RTC type
    "TTL_Logic_High",               // Voltage logic
    13,                             // Pump pin
    35,                             // Voltage read pin
    21,                             // DHT sensor pin
    DHT11,                          // DHT sensor type
    "test",                         // WiFi network hostname
    "irrigation_main_prototype",    // Soft AP SSID
    "",                             // Soft AP password
    #ifdef static_IP
    IPAddress(192, 168, 1, 22)      // Static IP address
    #endif
  },
  {
    "_1_point_0",                   // Board name
    "ds_3231",                      // RTC type
    "TTL_Logic_Low",                // Voltage logic
    13,                             // Pin number for pump
    35,                             // Pin number for voltage reading
    21,                             // Pin number for DHT sensor
    DHT11,                          // DHT sensor type (-1=None, 11=DHT11, 22=DHT22, 21=AM2301)
    "test",                         // Hostname of WiFi network
    "irrigation_test",              // Soft AP SSID
    "",                             // Soft AP password
    #ifdef static_IP
    IPAddress(192, 168, 1, 22)      // Static IP address
    #endif
  },
  {
    "_0_point_greenhouse",           // Board name
    "ds_3231",                       // RTC type
    "TTL_Logic_Low",                 // Voltage logic
    33,                              // Pin number for pump
    35,                              // Pin number for voltage reading
    21,                              // Pin number for DHT sensor
    -1,                              // DHT sensor type (-1=None, 11=DHT11, 22=DHT22, 21=AM2301)
    "greenhousetestboard",           // Hostname of WiFi network
    "irrigation_greenhousetest",     // Soft AP SSID
    "",                              // Soft AP password
    #ifdef static_IP
    IPAddress(192, 168, 1, 25)       // Static IP address
    #endif
  }
};

// Choose the board configuration
#define BOARD_CONFIG configs[1]

// Define board-specific configuration settings
#define ds_3231 BOARD_CONFIG.rtc_type // RTC type, set in board configuration
#define TTL_Logic_Low BOARD_CONFIG.voltage_logic // Voltage logic level, set in board configuration
int pumpPinNumber = BOARD_CONFIG.pump_pin; // Pin number for the water pump, set in board configuration
int batVoltPin = BOARD_CONFIG.voltage_pin; // Pin number for the battery voltage sensor, set in board configuration
#define DHTPIN BOARD_CONFIG.dht_pin // Pin number for the DHT sensor, set in board configuration
#define DHTTYPE BOARD_CONFIG.dht_type // DHT sensor type, set in board configuration
const char* wifi_network_hostname = BOARD_CONFIG.wifi_hostname; // Hostname for WiFi network, set in board configuration
const char *soft_ap_ssid = BOARD_CONFIG.ap_ssid; // Soft AP SSID, set in board configuration
const char *soft_ap_password = BOARD_CONFIG.ap_password; // Soft AP password, set in board configuration
#ifdef static_IP
IPAddress local_IP = BOARD_CONFIG.local_IP; // Local IP address, set in board configuration if using static IP
#endif

// External RTC specific configuration settings
#ifdef ds_3231
  #include <Wire.h>
  #include <RtcDS3231.h>
  RtcDS3231<TwoWire> Rtc(Wire); // Create a new RTC object using the DS3231 library and TwoWire interface
  int sdaPin = 05; // SDA pin for I2C communication with the RTC
  int sclPin = 17; // SCL pin for I2C communication with the RTC
#endif
  #ifdef ds_1302
  #include <ThreeWire.h>
  #include <RtcDS1302.h>
  ThreeWire myWire(14,13,32); // DAT, CLK, RST
  RtcDS1302<ThreeWire> Rtc(myWire); // Create a new RTC object using the DS1302 library and ThreeWire interface
#endif

// Included option to use relays with TTL Logic LOW. Comment to use high
#ifdef TTL_Logic_Low
  #define ON   LOW
  #define OFF  HIGH
#else
  #define ON   HIGH
  #define OFF  LOW
#endif

// Set your Gateway and Subnet IP addresses
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
// Set optional DNS IP addresses
IPAddress primaryDNS(  8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

ESP32Time rtc; // Create a new ESP32Time object for working with the RTC
const char *dataFile = "data.json"; // File name for the JSON data file used to save state
AsyncWebServer server(80); // Create a new AsyncWebServer object that listens on port 80
int jsonSize = 1800; // Specify the capacity of the JSON in bytes
DynamicJsonDocument doc(jsonSize); // From arduinoJson
boolean isScheduleMode = false;

//**************************************************************************************************************
// *****************************************Setup starts here***************************************************
//**************************************************************************************************************

void setup(){
  // Configure the serial monitor
  Serial.begin(9600);

  WiFi.mode(WIFI_MODE_APSTA);

  wifiMulti.addAP("ratinho_do_malandro", "gerryforever2018");
  wifiMulti.addAP("fabfarm_ele_container", "imakestuff");
  wifiMulti.addAP("liga_o_gerador", "gerryforever2018");
  wifiMulti.addAP("caravana", "imakestuff");
  wifiMulti.addAP("fabfarm", "imakestuff");
  wifiMulti.addAP("Raccaccoonie", "MalkovichMalkovich");
  wifiMulti.addAP("ubnt_mesh", "gerryforever2018");

  scanWifi();

  startWifi();

  //startSoftAP();

  // Set all relays off at startup
  disableAllRelays();

  // Prints compile time to serial monitor
  printCompileTime();

  // Setup Real Time Clock
  setupRTC();
  
  // Setup storage
  setupStorage();

  // Start the HTTP server
  server.begin();

  // Routes to data end-points
  server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.println("/data.json");
    DynamicJsonDocument data = doc;

    // Create a copy of the data set, modify it, serialize it and then send to the browser
    // this had to be changed as the serialization cleared the content of the jsondocument "doc"
    data["data"]["currentTime"] = rtc.getTime("%A, %B %d %Y %H:%M");
    data["data"]["temperature"] = readDHTTemperature();
    data["data"]["humidity"]    = readDHTHumidity();
    data["data"]["batLevel"]    = getBatteryLevel();

    char jsonReply[1800];
    Serial.println("Serialize JSON & return to caller - BEGIN");
    serializeJson(data, jsonReply);
    Serial.println(jsonReply);
    request->send(200, "application/json", jsonReply); // OK
    Serial.println("Serialize JSON & return to caller - COMPLETE");
  });
  
  // Update data general
  AsyncCallbackJsonWebHandler *updateData = new AsyncCallbackJsonWebHandler("/updateData", 
      [](AsyncWebServerRequest *request, JsonVariant &json) {
    doc = json.as<JsonObject>();

    if(writeDataJson())
    {
      request->send(200); // OK
    }
    else
    {
      request->send(500); // Internal Server Error
    }
  });

  // Update scheduling mode
  server.on("^\\/mode\\/((manual)|(scheduled))$", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    isScheduleMode = request->pathArg(0).equals("scheduled");
    doc["data"]["isScheduleMode"].set(isScheduleMode);

    if(writeDataJson())
    {
      if (isScheduleMode)
      {
	scheduleMode();
      }
      else
      {
	manualMode();
      }

      request->send(200); // OK
    }
    else
    {
      request->send(500); // Internal Server Error
    }
  });

  // Update relay enabled - MANUAL MODE
  server.on("^\\/relay\\/([0-9]+)\\/((on)|(off))$", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    if(isScheduleMode) {
      // Return Method Not Allowed when we're not in manual mode
      request->send(405); // Method Not Allowed
    }

    int relayIndex = request->pathArg(0).toInt();
    bool state = request->pathArg(1).equals("on");

    //ToDo Check if relay exists

    Serial.printf("Relay %d: %d\n\r", relayIndex, state);
    doc["relays"][relayIndex]["isEnabled"].set(state);

    if(writeDataJson())
    {
      manualMode();
      request->send(200); // OK
    }
    else
    {
      request->send(500); // Internal Server Error
    }
  });
  
  // Update relay times - AUTOMATIC MODE
  // AsyncCallbackJsonWebHandler does not support regex :-(
  //AsyncCallbackJsonWebHandler *updateRelayTime = new AsyncCallbackJsonWebHandler("^\\/relay\\/([0-9]+)\\/time\\/([0-9]+)$",
  //    [](AsyncWebServerRequest *request, JsonVariant &json) {
  //  int relayIndex = request->pathArg(0).toInt();
  //  int timeIndex = request->pathArg(1).toInt();
  AsyncCallbackJsonWebHandler *updateRelayTime = new AsyncCallbackJsonWebHandler("/relay/update-time",
      [](AsyncWebServerRequest *request, JsonVariant &json) {
    //TODO Input validation
    int relayIndex = json["relayIndex"];
    int timeIndex = json["timeIndex"];
    doc["relays"][relayIndex]["times"][timeIndex]["startTime"].set(json["startTime"]);
    doc["relays"][relayIndex]["times"][timeIndex]["duration"].set(json["duration"]);

    if(writeDataJson())
    {
      request->send(200); // OK
    }
    else
    {
      request->send(500); // Internal Server Error
    }
  });

  // Add new relay time
  AsyncCallbackJsonWebHandler *addRelayTime = new AsyncCallbackJsonWebHandler("/relay/add-time",
      [](AsyncWebServerRequest *request, JsonVariant &json) {
    JsonObject nested = doc["relays"][json["relayIndex"]]["times"].as<JsonArray>().createNestedObject();
    nested["startTime"] = "10:00";
    nested["duration"] = 30;

    if(writeDataJson())
    {
      request->send(200); // OK
    }
    else
    {
      request->send(500); // Internal Server Error
    }
  });
  
  // Remove relay time
  server.on("^\\/relay\\/([0-9]+)\\/time\\/([0-9]+)$", HTTP_DELETE, [](AsyncWebServerRequest *request) {
    int relayIndex = request->pathArg(0).toInt();
    int timeIndex = request->pathArg(1).toInt();

    //ToDo Check if relay and time exist

    doc["relays"][relayIndex]["times"].as<JsonArray>().remove(timeIndex);

    if(writeDataJson())
    {
      request->send(200); // OK
    }
    else
    {
      request->send(500); // Internal Server Error
    }
  });

  // Add new relay
  AsyncCallbackJsonWebHandler *addRelay = new AsyncCallbackJsonWebHandler("/relay/add",
      [](AsyncWebServerRequest *request, JsonVariant &json) {
    JsonObject nested = doc["relays"].as<JsonArray>().createNestedObject();

    // compute the required size
    const size_t CAPACITY = JSON_ARRAY_SIZE(3);

    // allocate the memory for the document
    StaticJsonDocument<CAPACITY> newArray;

    // create an empty array
    JsonArray array = newArray.to<JsonArray>();
    JsonObject newTime = array.createNestedObject();
    // create an empty array
    newTime["startTime"] = "10:00";
    newTime["duration"] = 30;
    nested["times"] = array;

    nested["name"] = json["name"];
    nested["pin"] = json["pin"];
    nested["isEnabled"] = false;

    if(writeDataJson())
    {
      request->send(200); // OK
    }
    else
    {
      request->send(500); // Internal Server Error
    }
  });
  
  // Remove relay
  server.on("^\\/relay\\/([0-9]+)$", HTTP_DELETE, [](AsyncWebServerRequest *request) {
    int relayIndex = request->pathArg(0).toInt();

    //ToDo Check if relay exists

    Serial.printf("Removing relay %d\n\r", relayIndex);
    doc["relays"].as<JsonArray>().remove(relayIndex);

    if(writeDataJson())
    {
      request->send(200); // OK
    }
    else
    {
      request->send(500); // Internal Server Error
    }
  });

  server.addHandler(updateData);
  server.addHandler(updateRelayTime);
  server.addHandler(addRelayTime);
  server.addHandler(addRelay);

  // Start ElegantOTA for Over The Air updates
  AsyncElegantOTA.begin(&server);

  /*
   * Define HTTP server paths
   */
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false);
  });

  // Use regular expression to catch valid file names to serve
  server.on("^(\\/[a-zA-Z0-9_.-]*)$", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String file = request->pathArg(0);

    Serial.printf("Serving file %s\n\r", file.c_str());
    request->send(SPIFFS, file, String(), false);
  });

  isScheduleMode = doc["data"]["isScheduleMode"];
  if (!isScheduleMode)
  {
    manualMode();
  }
}

//**************************************************************************************************************
// ********************************Loop Stats here************************************************************** 
//**************************************************************************************************************

void loop(){

  wifiLoop();

  //testRtcOnLoop();  // Uncomment testRtcOnLoop() to display time every second on serial monitor

  if (isScheduleMode)
  {
    scheduleMode();
  }

  sleep(1);
}

//**************************************************************************************************************
//***********Bellow here only functions*************************************************************************
//**************************************************************************************************************

/*
 * This function simply prints the compile time to the serial monitor
 */
void printCompileTime() {
  Serial.println("*****************************************************");
  Serial.printf(  "* This program was Compiled on: %s at %s\n\r", __DATE__, __TIME__);
  Serial.println("*****************************************************");
  Serial.println();
}

/*
 * Initialize the file system and create jSON file if missing
 */
void setupStorage() {
  // Initialize SPIFFS (file system)
  if (!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  readDataJson();
}

/*
 * This function reads the settings file data.json or if this file does not exist yet the file sample.json
 * We always read the data.json from disk on startup (always!)
 * If freshly burned we have to send the sample JSON...
 */
void readDataJson() {
  Serial.println("Starting looking for JSON file");

  File f;
  if(SPIFFS.exists("/data.json")) {
    f = SPIFFS.open("/data.json", "r");
  }
  else
  {
    Serial.println("data.json does not exist yet, reading sample.json instead!");
    f = SPIFFS.open("/sample.json", "r");
  }

  // Read JSON string from JSON file into memory
  Serial.println("Read JSON file - BEGIN");
  String json = f.readString();
  Serial.println(json);
  Serial.println("Read JSON file - COMPLETE");

  // Close the JSON file
  f.close();

  // We take JSON from memory & create JSON object
  Serial.println("JSON deserialize - BEGIN");
  deserializeJson(doc, json);
  Serial.println("JSON deserialize - COMPLETE");
}

bool writeDataJson() {
  String jsonString;
  Serial.println("JSON serialize - BEGIN");
  serializeJson(doc, jsonString);
  Serial.println("JSON serialize - COMPLETE");

  // Write this to disk
  Serial.println("Write JSON file - BEGIN");
  File f = SPIFFS.open("/data.json", "w");
  if (!f)
  {
    Serial.println("Failed to open file for writing");
    Serial.println("Write JSON file - FAILED");

    return false;
  }
  else
  {
    int bytesWritten = f.print(jsonString);
    Serial.println(jsonString);
    Serial.printf("Write JSON file - COMPLETE(%d bytes)\n\r", bytesWritten);

    // Close the JSON file
    f.close();

    return true;
  }
}

void scheduleMode() {
  Serial.println("scheduleMode");
  bool enablePump = false;

  // matrix logic test
  JsonArray relays = doc["relays"];
  for (int i = 0; i < relays.size(); i++){
    bool enableRelay = false;
    // Get GPIO pin number for relay
    int pinNumber = relays[i]["pin"];

    for (int j = 0; j < relays[i]["times"].size(); j++) {
      // Set GPIO pin to output
      pinMode(pinNumber, OUTPUT);
      // Split startTime into hour and minute parts
      int hOurs = relays[i]["times"][j]["startTime"].as<String>().substring(0,2).toInt();
      int mIns = relays[i]["times"][j]["startTime"].as<String>().substring(3).toInt();
      // Calculate start time in minutes
      int startTimeInMinutes = (hOurs * 60) + mIns;
      int cycleDuration = relays[i]["times"][j]["duration"];
      // Probably should learn about bitwise... https://playground.arduino.cc/Code/BitMath/
      if (isWithinTimeslot(startTimeInMinutes, cycleDuration))
      {
	  enableRelay = true;
      }
    }

    if (enableRelay)
    {
      // Set GPIO pin to on
      digitalWrite(pinNumber, ON);
      Serial.printf("Relay %s (pin %d) is on\n\r", relays[i]["name"].as<char*>(), pinNumber);
      enablePump = true;
    }
    else
    {   
      // Set GPIO pin to off
      digitalWrite(pinNumber, OFF);
      Serial.printf("Relay %s (pin %d) is off\n\r", relays[i]["name"].as<char*>(), pinNumber);
    }
  }

  // Switch off the pump if no relays are on
  switchPump(enablePump);
}

void manualMode()
{
  Serial.println("manualMode");
  int enablePump = false;

  JsonArray relays = doc["relays"];
  for (int i = 0; i < relays.size(); i++)
  {
    // Get GPIO pin number for relay
    int pinNumber = relays[i]["pin"];
    // Set GPIO pin to output
    pinMode(pinNumber, OUTPUT);

    if (relays[i]["isEnabled"])
    {
      // Set GPIO pin to on
      digitalWrite(pinNumber, ON);
      Serial.printf("Relay %s (pin %d) is on\n\r", relays[i]["name"].as<char*>(), pinNumber);
      enablePump = true;
    }
    else
    {
      // Set GPIO pin to off
      digitalWrite(pinNumber, OFF);
      Serial.printf("Relay %s (pin %d) is off\n\r", relays[i]["name"].as<char*>(), pinNumber);
    }
  }

  // Switch off the pump if no relays are on
  switchPump(enablePump);
}

/*
 * Function to deactivate all relays. Safe startup.
 * TODO: (question? does it reactivates unintentionaly like previously noted?)
 */
void disableAllRelays(){
  // Defining behavior of pumpPinNumber and its startup state
  switchPump(false);

  // Loop over configured relays and set to off
  JsonArray relays = doc["relays"];
  for (int i = 0; i < relays.size(); i++)
  {
    // Get GPIO pin number for relay
    int pinNumber = relays[i]["pin"];
    // Set GPIO pin to output
    pinMode(pinNumber, OUTPUT);
    // Set GPIO pin to off
    digitalWrite(pinNumber, OFF);
    Serial.printf("Zone %s (pin %n) is off\n\r", relays[i]["name"].as<char*>(), &pinNumber);
  }
}

/*
 * Switch the pump on or off
 */
void switchPump(bool state)
{
  // Set GPIO pin to output
  pinMode(pumpPinNumber, OUTPUT);
  if (state)
  {
    // Set GPIO pin to on
    digitalWrite(pumpPinNumber, ON);
    Serial.printf("Pump (pumpPinNumber %d) is on\n\r", pumpPinNumber);
  }
  else
  {
    // Set GPIO pin to off
    digitalWrite(pumpPinNumber, OFF);
    Serial.printf("Pump (pumpPinNumber %d) is off\n\r", pumpPinNumber);
  }
}

/*
 * Function is used to evaluate if a certain relay should be OFF or ON depending on the time it was set to run
 */
bool isWithinTimeslot (int startTimeInMinutes, int duration)
{
  if (duration <= 0)
  {
      return false;
  }

  // Get current time from RTC
  int onlyHour = rtc.getHour(true);
  int onlyMin = rtc.getMinute();
  // Calculate present time in minutes since midnight
  int presentTimeInMinutes = (onlyHour * 60) + onlyMin;

  int isEnabled = false;
  if (presentTimeInMinutes >= startTimeInMinutes && presentTimeInMinutes < startTimeInMinutes + duration)
  {
    isEnabled = true;
  }

  return isEnabled;
}

/*
 * This function reads the temperature sensor data from the DHT in Celsius
 */
float readDHTTemperature()
{
  if(DHTTYPE == -1)
  {
      return NAN;
  }

  // Send the pin and type of sensor
  DHT dht(DHTPIN, DHTTYPE);

  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();

  // Check if any reads failed
  if (isnan(temperature))
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
    Serial.printf("Temperature: %f\n\r", temperature);
  }

  return temperature;
}

/*
 * This function reads the humidity sensor data from the DHT
 */
float readDHTHumidity()
{
  if(DHTTYPE == -1)
  {
      return NAN;
  }

  // Send the pin and type of sensor
  DHT dht(DHTPIN, DHTTYPE);

  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float humidity = dht.readHumidity();

  // Check if any reads failed
  if (isnan(humidity))
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
    Serial.printf("Humidity: %f\n\r", humidity);
  }

  return humidity;
}

/*
 * Determine battery percentage based on the battery level voltage
 */
float getBatteryLevel(){
  analogRead(batVoltPin);

  // Calculate the battery percentage
  float batteryLevel = map(analogRead(batVoltPin), 0.0f, 1866.0f, 0, 100);

  if (batteryLevel >= 100) {
      batteryLevel = 100;
  }
  else
  {
    // Print the battery level to the serial monitor
    Serial.printf("Battery Level: %f\n\r", batteryLevel);
  }

  return batteryLevel;
}

/*
 * This function assigns local time based on an internet server
 */
void assignLocalTime(){
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
  TYear  = timeinfo.tm_year + 1900;
  TMonth = timeinfo.tm_mon + 1;
  TDay   = timeinfo.tm_mday;
  THour  = timeinfo.tm_hour;
  TMin   = timeinfo.tm_min;
  TSec   = timeinfo.tm_sec;
  rtc.setTime(TSec,TMin,THour,TDay,TMonth,TYear);
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

/*
 * Real Time Clock (RTC) functions
 */

/*
 * Function to update the internal RTC with the time from the external RTC
 */
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

  Serial.println("*****************************************************");
  Serial.println("* Function updateInternalRTC()");
  Serial.printf( "* Time from external RTC: %s\n\r", datestring);
  Serial.println("* This function updates the internal RTC with the time");
  Serial.println("* from the external RTC");
  Serial.println("*****************************************************");
}

/*
 * This function will do a series of logical tests on the external Real Time Clock
 * in order to set its time in case is needed and print then status
 */
void setupRTC()
{ 
  Serial.println("*****************************************************");
  Serial.println("* Running function setupRTC()");

  // Initialize external Real Time Clock
  #ifdef ds_3231
  Wire.begin(sdaPin, sclPin); // SDA, SCL
  #endif
  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  // Print the Time before updated from external RTC
  Serial.println("* ");
  Serial.printf( "* Time from internal RTC on boot: %s\n\r", rtc.getTime("%A, %B %d %Y %H:%M:%S").c_str());
  // Configure internal RTC from external rtc time
  updateInternalRTC(Rtc.GetDateTime());
  // Print the Time updated from external RTC
  Serial.println("* ");
  Serial.printf( "* Time from internal RTC after external RTC update:  %s\n\r", rtc.getTime("%A, %B %d %Y %H:%M:%S").c_str());

  if (!Rtc.IsDateTimeValid()) 
  {
    // Common Causes:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing

    Serial.println("* RTC lost confidence in the DateTime!");
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
    Serial.println("* RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) 
  {
    Serial.println("* RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  }
  else if (now > compiled) 
  {
    Serial.println("* RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled) 
  {
    Serial.println("* RTC is the same as compile time! (not expected but all is fine)");
  }
  Serial.println("*");

  Serial.println("* END of data from first external RTC function");
  Serial.println("*****************************************************");
}

/*
 * This function prints the clock on serial monitor with a predetermined interval
 */
void testRtcOnLoop(){
  // This data is for the functions that need an interval and use millis to instead of delay
  unsigned long previousMillis = 0;
  const long printTimeInterval = 10000;

  Serial.println("This data comes from void testRtcOnLoop() time will appear only every 1 second" );
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

// void onWifiConnected(WiFiEvent_t event, WiFiEventInfo_t info){
//   Serial.println("Successfully connected to Access Point");
// }

// void onGetIPAddress(WiFiEvent_t event, WiFiEventInfo_t info){
//   Serial.println("");
//   Serial.println("*****************************************************");
//   Serial.println("* WIFI is connected!");
//   Serial.printf( "* The IP address is: %s\n\r", WiFi.localIP().toString().c_str());
//   Serial.printf( "* The hostname is: %s\n\r", WiFi.getHostname());
//   Serial.println("*****************************************************");
//   Serial.println("");
// }

// void onWifiDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
//   Serial.println("");
//   Serial.println("*****************************************************");
//   Serial.println("* Disconnected from WIFI Access Point");
//   Serial.printf( "* WiFi lost connection. Reason: %u\n\r", (unsigned int) info.disconnected.reason);
//   Serial.println("* Reconnecting...");
//   WiFi.begin(wifi_network_ssid, wifi_network_password);
//   Serial.println("*****************************************************");
// }

/*
 * Function with WiFi settings and initialisation
 */
void setupWifi()
{
  WiFi.mode(WIFI_MODE_APSTA);

  // Disconnect the WiFi
  // WiFi.disconnect(true);
  // WiFi.onEvent(onWifiConnected,SYSTEM_EVENT_STA_CONNECTED);
  // WiFi.onEvent(onGetIPAddress, SYSTEM_EVENT_STA_GOT_IP);
  // WiFi.onEvent(onWifiDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);

  WiFi.softAP(soft_ap_ssid, soft_ap_password, 3);

  // Print WiFi configuration to serial monitor
  Serial.println();
  Serial.println("*****************************************************");
  Serial.printf( "* SoftAP IP is: %s\n\r", WiFi.softAPIP().toString().c_str());

  #ifdef stacticIP
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("* STA Failed to configure");
    }
  #endif

  //WiFi.begin(wifi_network_ssid, wifi_network_password);
  WiFi.setHostname(wifi_network_hostname);
  Serial.println("* Waiting for WIFI network...");
  Serial.println("*****************************************************");
  Serial.println();

  //delay(2000);
}

/*
 * Time functions
 */

/*
 * Function to change time using the time that was input in the JSON
 */
void changeTimeFromJSON(){
  JsonObject data = doc["data"];
  JsonArray data_changedtime = data["changedtime"];
  int mIn = data_changedtime[0]["min"];
  int hOur = data_changedtime[0]["hour"];
  int dAy = data_changedtime[0]["day"];
  int mOnth = data_changedtime[0]["month"];
  int yEar = data_changedtime[0]["year"];
  rtc.setTime(0,mIn,hOur,dAy,mOnth,yEar);
}

/*
 * This function will select to update time either from the internet or from a manual entry in the html that populates the JSON
 */
// void internetOrManualTime()
// {
//   // Call function that assigns the array in the JSON to the rtc of ESP32
//   Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));
//   boolean data_internettime = data["changedtime"][0]["internettime"] ;
//   //boolean data_manualtimeflag = data["changedtime"][0]["manualtimeenabled"];
//   if ( data_internettime  == 1)
//   {
//     Serial.println("Time updated using internet");
//       for(static bool first = true;first;first=false)
//       { 
//         	assignLocalTime();
//       }
//   }
//   if ( data_internettime  == 0 )
//   { 
//     Serial.println("Time updated using manual input");
//     for(static bool first = true;first;first=false)
//     { 
//       changeTimeFromJSON();
//     }
//   }
// }

void scanWifi() {
  // Call WiFi.scanNetworks to scan for available WiFi networks
  int networkCount = WiFi.scanNetworks();
  
  // Print a message indicating the scan is complete
  Serial.println("WiFi scan complete");
  
  // Check if any networks were found
  if (networkCount == 0) {
    Serial.println("No WiFi networks found.");
  } 
  else {
    // Print the number of networks found
    Serial.printf("Found %d WiFi networks:\n", networkCount);
    
    // Loop through each network and print its name, signal strength, and encryption type
    for (int i = 0; i < networkCount; i++) {
      // Print the network number
      Serial.printf("%d: ", i + 1);
      
      // Print the network name (SSID)
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      
      // Print the signal strength (RSSI)
      Serial.print(WiFi.RSSI(i));
      Serial.print(" dBm");
      
      // Print the encryption type
      if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) {
        Serial.print(", open");
      } else {
        Serial.print(", encrypted");
      }
      
      Serial.println();
    }
  }
}


void startSoftAP(){
  // Start the soft access point (AP)
  WiFi.softAP(soft_ap_ssid, soft_ap_password, 3);

  // Print WiFi configuration to serial monitor
  Serial.println();
  Serial.println("*****************************************************");
  Serial.printf("* SoftAP IP is: %s\n\r", WiFi.softAPIP().toString().c_str());
}

void startWifi() {
  // Connect to Wi-Fi using wifiMulti (connects to the SSID with strongest connection)
  Serial.println("Connecting Wifi...");
  if (wifiMulti.run() == WL_CONNECTED) {
    // Connected to WiFi
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    // Failed to connect to WiFi
    Serial.println("WiFi connection failed.");
  }
  // Set the Wi-Fi transmit power to the maximum value
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
}

void wifiLoop() {
  unsigned long previousTime = 0; // static variable to keep track of the previous time
  const unsigned long eventInterval = 10000; // interval between Wi-Fi connection attempts
  const uint32_t connectTimeoutMs = 10000; // Wi-Fi connection timeout in milliseconds

  // get the current time
  unsigned long currentTime = millis();

  // check if enough time has passed to attempt a Wi-Fi connection
  if (currentTime - previousTime >= eventInterval) {

    // attempt to connect to the Wi-Fi network
    if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
      // if connected, print the network name and signal strength
      Serial.print("WiFi connected: ");
      Serial.print(WiFi.SSID());
      Serial.print(" ");
      Serial.println(WiFi.RSSI());
    } else {
      // if not connected, print a message to the serial monitor
      Serial.println("WiFi not connected!");
    }

    // update the timing for the next time around
    previousTime = currentTime;
  }
}