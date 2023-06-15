#include "Config.h"

void setup()
{
  Serial.begin(9600);
  setupWiFi();
  scanWifi();
  startWifi();
  disableAllRelays();
  printCompileTime();
  setupRTC();
  setupStorage();
  server.begin();
  serverHandle();
}

void loop()
{
  wifiLoop();
  if (isScheduleMode)
  {
    scheduleMode();
  }
  sleep(1);
}