#include "Arduino.h"

String printShortFarmTime()
{ 
  const char *ntpServer = "us.pool.ntp.org";
  const long gmtOffset_sec = 0;
  const int daylightOffset_sec = 3600;

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //time_t rawtime;
  struct tm timeinfo;

  getLocalTime(&timeinfo);
  char timeStringBuff[50]; //50 chars should be enough
  strftime(timeStringBuff, sizeof(timeStringBuff), "H:M:S", &timeinfo);

  //print like "const char*"
  Serial.println("Checking time - BEGIN");
  Serial.printf("Hour: %d\n", timeinfo.tm_hour);
  Serial.printf("Minute: %d\n", timeinfo.tm_min);
  Serial.println(timeStringBuff);
  Serial.println("Checking time - COMPLETE");

  //Construct to create the String object
  String timeAsAString(timeStringBuff);
  return timeAsAString;
}