#include "Config.h"
#include "Fabfarm_Irrigation.h"
#include "JsonHandler.h"


//**************************************************************************************************************
// *****************************************Setup starts here***************************************************
//**************************************************************************************************************

void serverHandle()
{
    server.on("/data.json", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    Serial.println("/data.json");
    DynamicJsonDocument data = doc;

    data["data"]["currentTime"] = rtc.getTime("%A, %B %d %Y %H:%M");
    data["data"]["temperature"] = readDHTTemperature();
    data["data"]["humidity"] = readDHTHumidity();
    data["data"]["batLevel"] = getBatteryLevel();

    char jsonReply[1800];
    Serial.println("Serialize JSON & return to caller - BEGIN");
    serializeJson(data, jsonReply);
    Serial.println(jsonReply);
    request->send(200, "application/json", jsonReply);
    Serial.println("Serialize JSON & return to caller - COMPLETE"); });

  AsyncCallbackJsonWebHandler *updateData = new AsyncCallbackJsonWebHandler("/updateData", [](AsyncWebServerRequest *request, JsonVariant &json)
                                                                            {
    doc = json.as<JsonObject>();

    if (writeDataJson()) {
      request->send(200);
    } else {
      request->send(500);
    } });

  server.on("^\\/mode\\/((manual)|(scheduled))$", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    isScheduleMode = request->pathArg(0).equals("scheduled");
    doc["data"]["isScheduleMode"].set(isScheduleMode);

    if (writeDataJson()) {
      if (isScheduleMode) {
        scheduleMode();
      } else {
        manualMode();
      }

      request->send(200);
    } else {
      request->send(500);
    } });

  server.on("^\\/relay\\/([0-9]+)\\/((on)|(off))$", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (isScheduleMode) {
      request->send(405);
    }

    int relayIndex = request->pathArg(0).toInt();
    bool state = request->pathArg(1).equals("on");

    Serial.printf("Relay %d: %d\n\r", relayIndex, state);
    doc["relays"][relayIndex]["isEnabled"].set(state);

    if (writeDataJson()) {
      manualMode();
      request->send(200);
    } else {
      request->send(500);
    } });

  AsyncCallbackJsonWebHandler *updateRelayTime = new AsyncCallbackJsonWebHandler("/relay/update-time", [](AsyncWebServerRequest *request, JsonVariant &json)
                                                                                 {
    int relayIndex = json["relayIndex"];
    int timeIndex = json["timeIndex"];
    doc["relays"][relayIndex]["times"][timeIndex]["startTime"].set(json["startTime"]);
    doc["relays"][relayIndex]["times"][timeIndex]["duration"].set(json["duration"]);

    if (writeDataJson()) {
      request->send(200);
    } else {
      request->send(500);
    } });

  AsyncCallbackJsonWebHandler *addRelayTime = new AsyncCallbackJsonWebHandler("/relay/add-time",
                                                                              [](AsyncWebServerRequest *request, JsonVariant &json)
                                                                              {
                                                                                JsonObject nested = doc["relays"][json["relayIndex"]]["times"].as<JsonArray>().createNestedObject();
                                                                                nested["startTime"] = "10:00";
                                                                                nested["duration"] = 30;

                                                                                if (writeDataJson())
                                                                                {
                                                                                  request->send(200);
                                                                                }
                                                                                else
                                                                                {
                                                                                  request->send(500);
                                                                                }
                                                                              });

  server.on("^\\/relay\\/([0-9]+)\\/time\\/([0-9]+)$", HTTP_DELETE, [](AsyncWebServerRequest *request)
            {
    int relayIndex = request->pathArg(0).toInt();
    int timeIndex = request->pathArg(1).toInt();


    doc["relays"][relayIndex]["times"].as<JsonArray>().remove(timeIndex);

    if(writeDataJson())
    {
      request->send(200); 
    }
    else
    {
      request->send(500); 
    } });

  AsyncCallbackJsonWebHandler *addRelay = new AsyncCallbackJsonWebHandler("/relay/add",
                                                                          [](AsyncWebServerRequest *request, JsonVariant &json)
                                                                          {
                                                                            JsonObject nested = doc["relays"].as<JsonArray>().createNestedObject();

                                                                            const size_t CAPACITY = JSON_ARRAY_SIZE(3);

                                                                            StaticJsonDocument<CAPACITY> newArray;

                                                                            JsonArray array = newArray.to<JsonArray>();
                                                                            JsonObject newTime = array.createNestedObject();
                                                                            newTime["startTime"] = "10:00";
                                                                            newTime["duration"] = 30;
                                                                            nested["times"] = array;

                                                                            nested["name"] = json["name"];
                                                                            nested["pin"] = json["pin"];
                                                                            nested["isEnabled"] = false;

                                                                            if (writeDataJson())
                                                                            {
                                                                              request->send(200);
                                                                            }
                                                                            else
                                                                            {
                                                                              request->send(500);
                                                                            }
                                                                          });

  server.on("^\\/relay\\/([0-9]+)$", HTTP_DELETE, [](AsyncWebServerRequest *request)
            {
    int relayIndex = request->pathArg(0).toInt();
              Serial.printf("Removing relay %d\n\r", relayIndex);
              doc["relays"].as<JsonArray>().remove(relayIndex);

              if (writeDataJson())
              {
                request->send(200);
              }
              else
              {
                request->send(500);
              } });

  server.addHandler(updateData);
  server.addHandler(updateRelayTime);
  server.addHandler(addRelayTime);
  server.addHandler(addRelay);

  // AsyncElegantOTA.begin(&server);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false); });
  server.on("^(\\/[a-zA-Z0-9_.-]*)$", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String file = request->pathArg(0);
              Serial.printf("Serving file %s\n\r", file.c_str());
              request->send(SPIFFS, file, String(), false); });

  isScheduleMode = doc["data"]["isScheduleMode"];
  if (!isScheduleMode)
  {
    manualMode();
  }
}
void setupSerialMonitor()
{
  Serial.begin(9600);
}

void setupWiFi()
{
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
}

void printCompileTime()
{
  Serial.println("*****************************************************");
  Serial.printf("* This program was compiled on: %s at %s\n\r", __DATE__, __TIME__);
  Serial.println("*****************************************************\n\r");
}

void setupStorage()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  readDataJson();
}

void readDataJson()
{
  Serial.println("Starting to look for JSON file");

  File f;
  if (SPIFFS.exists("/data.json"))
  {
    f = SPIFFS.open("/data.json", "r");
  }
  else
  {
    Serial.println("data.json does not exist yet, reading sample.json instead!");
    f = SPIFFS.open("/sample.json", "r");
  }

  Serial.println("Reading JSON file - BEGIN");
  String json = f.readString();
  Serial.println(json);
  Serial.println("Reading JSON file - COMPLETE");

  f.close();

  Serial.println("JSON deserialize - BEGIN");
  deserializeJson(doc, json);
  Serial.println("JSON deserialize - COMPLETE");
}



void scheduleMode()
{
  Serial.println("scheduleMode");
  bool enablePump = false;
  JsonArray relays = doc["relays"];
  for (int i = 0; i < relays.size(); i++)
  {
    bool enableRelay = false;
    int pinNumber = relays[i]["pin"];
    for (int j = 0; j < relays[i]["times"].size(); j++)
    {
      pinMode(pinNumber, OUTPUT);
      int hours = relays[i]["times"][j]["startTime"].as<String>().substring(0, 2).toInt();
      int minutes = relays[i]["times"][j]["startTime"].as<String>().substring(3).toInt();
      int startTimeInMinutes = (hours * 60) + minutes;
      int cycleDuration = relays[i]["times"][j]["duration"];
      if (isWithinTimeslot(startTimeInMinutes, cycleDuration))
      {
        enableRelay = true;
      }
    }

    if (enableRelay)
    {
      digitalWrite(pinNumber, ON);
      Serial.printf("Relay %s (pin %d) is on\n\r", relays[i]["name"].as<char *>(), pinNumber);
      enablePump = true;
    }
    else
    {
      digitalWrite(pinNumber, OFF);
      Serial.printf("Relay %s (pin %d) is off\n\r", relays[i]["name"].as<char *>(), pinNumber);
    }
  }
  switchPump(enablePump);
}

void manualMode()
{
  Serial.println("manualMode");
  int enablePump = false;
  JsonArray relays = doc["relays"];
  for (int i = 0; i < relays.size(); i++)
  {
    int pinNumber = relays[i]["pin"];
    pinMode(pinNumber, OUTPUT);
    if (relays[i]["isEnabled"])
    {
      digitalWrite(pinNumber, ON);
      Serial.printf("Relay %s (pin %d) is on\n\r", relays[i]["name"].as<char *>(), pinNumber);
      enablePump = true;
    }
    else
    {
      digitalWrite(pinNumber, OFF);
      Serial.printf("Relay %s (pin %d) is off\n\r", relays[i]["name"].as<char *>(), pinNumber);
    }
  }

  switchPump(enablePump);
}

void disableAllRelays()
{
  switchPump(false);

  JsonArray relays = doc["relays"];
  for (int i = 0; i < relays.size(); i++)
  {
    int pinNumber = relays[i]["pin"];
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, OFF);
    Serial.printf("Zone %s (pin %d) is off\n\r", relays[i]["name"].as<char *>(), pinNumber);
  }
}

void switchPump(bool state)
{
  pinMode(pumpPinNumber, OUTPUT);

  if (state)
  {
    digitalWrite(pumpPinNumber, ON);
    Serial.printf("Pump (pumpPinNumber %d) is on\n\r", pumpPinNumber);
  }
  else
  {
    digitalWrite(pumpPinNumber, OFF);
    Serial.printf("Pump (pumpPinNumber %d) is off\n\r", pumpPinNumber);
  }
}

bool isWithinTimeslot(int startTimeInMinutes, int duration)
{
  if (duration <= 0)
  {
    return false;
  }

  int onlyHour = rtc.getHour(true);
  int onlyMin = rtc.getMinute();

  int presentTimeInMinutes = (onlyHour * 60) + onlyMin;

  bool isEnabled = false;
  if (presentTimeInMinutes >= startTimeInMinutes && presentTimeInMinutes < startTimeInMinutes + duration)
  {
    isEnabled = true;
  }

  return isEnabled;
}

float readDHTTemperature()
{
  static unsigned long lastReadTime = 0;
  const unsigned long readInterval = 2000;

  if (DHTTYPE == -1)
  {
    return NAN;
  }

  DHT dht(DHTPIN, DHTTYPE);

  if (millis() - lastReadTime > readInterval)
  {
    lastReadTime = millis();

    float temperature = dht.readTemperature();

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

  return NAN;
}

float readDHTHumidity()
{
  if (DHTTYPE == -1)
  {
    return NAN;
  }

  DHT dht(DHTPIN, DHTTYPE);

  float humidity = dht.readHumidity();

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
float getBatteryLevel()
{
  float batteryVoltage = analogRead(batVoltPin);
  float batteryLevel = map(batteryVoltage, 0.0f, 1866.0f, 0, 100);

  if (batteryLevel > 100)
  {
    batteryLevel = 100;
  }

  Serial.printf("Battery Level: %f\n\r", batteryLevel);

  return batteryLevel;
}

void assignLocalTime()
{
  int THour;
  int TMin;
  int TSec;
  int TYear;
  int TMonth;
  int TDay;

  const char *ntpServer = "pool.ntp.org";
  const long gmtOffset_sec = 3600;
  const int daylightOffset_sec = 0;

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;
  getLocalTime(&timeinfo);

  TYear = timeinfo.tm_year + 1900;
  TMonth = timeinfo.tm_mon + 1;
  TDay = timeinfo.tm_mday;
  THour = timeinfo.tm_hour;
  TMin = timeinfo.tm_min;
  TSec = timeinfo.tm_sec;

  rtc.setTime(TSec, TMin, THour, TDay, TMonth, TYear);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime &dt)
{
  char datestring[20];

  snprintf_P(datestring, countof(datestring), PSTR("%02u/%02u/%04u %02u:%02u:%02u"), dt.Month(), dt.Day(), dt.Year(), dt.Hour(), dt.Minute(), dt.Second());

  Serial.println(datestring);
}

void updateInternalRTC(const RtcDateTime &dt)
{
  char datestring[20];

  snprintf_P(datestring, countof(datestring), PSTR("%02u/%02u/%04u %02u:%02u:%02u"), dt.Month(), dt.Day(), dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
  rtc.setTime(dt.Second(), dt.Minute(), dt.Hour(), dt.Day(), dt.Month(), dt.Year());

  Serial.println("*****************************************************");
  Serial.println("* Function updateInternalRTC()");
  Serial.printf("* Time from external RTC: %s\n\r", datestring);
  Serial.println("* This function updates the internal RTC with the time");
  Serial.println("* from the external RTC");
  Serial.println("*****************************************************");
}

void setupRTC()
{
  Serial.println("*****************************************************");
  Serial.println("* Running function setupRTC()");

#ifdef ds_3231
  Wire.begin(sdaPin, sclPin);
#endif
  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

  Serial.println("* ");
  Serial.printf("* Time from internal RTC on boot: %s\n\r", rtc.getTime("%A, %B %d %Y %H:%M:%S").c_str());

  updateInternalRTC(Rtc.GetDateTime());

  Serial.println("* ");
  Serial.printf("* Time from internal RTC after external RTC update: %s\n\r", rtc.getTime("%A, %B %d %Y %H:%M:%S").c_str());

  if (!Rtc.IsDateTimeValid())
  {
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
    Serial.println("*RTC was not actively running, starting now");
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

  Serial.println("* END of data from first external RTC function");
  Serial.println("*****************************************************");
}

void testRtcOnLoop()
{
  unsigned long previousMillis = 0;
  const long printTimeInterval = 1000;

  Serial.println("This data comes from void testRtcOnLoop(). Time will appear every 1 second.");

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= printTimeInterval)
  {
    previousMillis = currentMillis;

    RtcDateTime now = Rtc.GetDateTime();
    if (now.IsValid())
    {
      printDateTime(now);
      Serial.println();
    }
    else
    {
      Serial.println("RTC lost confidence in the DateTime!");
    }
  }

  Serial.println("End of data from void testRtcOnLoop().");
}

void setupWifi()
{
  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(soft_ap_ssid, soft_ap_password, 3);

  Serial.println();
  Serial.println("*****************************************************");
  Serial.printf("* SoftAP IP is: %s\n\r", WiFi.softAPIP().toString().c_str());

#ifdef stacticIP
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("* STA Failed to configure");
  }
#endif

  WiFi.setHostname(wifi_network_hostname);

  Serial.println("* Waiting for WIFI network...");
  Serial.println("*****************************************************");
  Serial.println();
}

bool writeDataJson()
{
  String jsonString;
  Serial.println("JSON serialize - BEGIN");
  serializeJson(doc, jsonString);
  Serial.println("JSON serialize - COMPLETE");

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
    Serial.printf("Write JSON file - COMPLETE (%d bytes)\n\r", bytesWritten);

    f.close();

    return true;
  }
}


