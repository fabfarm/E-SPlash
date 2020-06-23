#include "Arduino.h"

void modifiedPrintLocalTime()
// Function based on post in the https://forum.arduino.cc/index.php?topic=536464.0 Arduino Forum by user Abhay
{ 
  const char *ntpServer = "us.pool.ntp.org";
  const long gmtOffset_sec = 0;
  const int daylightOffset_sec = 3600;
  
  int onlyYear;
  int onlyMonth;
  int onlyDay;
  int onlyHour;
  int onlyMin;
  int onlySec;

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  //Serial.println(&timeinfo, "mdY /H:M:S");
  //scanf(&timeinfo, "mdY /H:M:S")
  onlyHour = timeinfo.tm_hour;
  onlyMin = timeinfo.tm_min;
  onlySec = timeinfo.tm_sec;
  onlyDay = timeinfo.tm_mday;
  onlyMonth = timeinfo.tm_mon + 1;
  onlyYear = timeinfo.tm_year + 1900;

  //test
  Serial.print("Print only Hour and Minutes...");
  Serial.print(onlyHour);
  Serial.print(":");
  Serial.print(onlyMin);
  Serial.print(":");
  Serial.println(onlySec);
}