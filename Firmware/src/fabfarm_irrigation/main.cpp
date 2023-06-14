#include "Config.h"
#include "Fabfarm_Irrigation.h"
#include "WiFiHandler.h"
#include "JsonHandler.h"

void setup()
{
  setupSerialMonitor();
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