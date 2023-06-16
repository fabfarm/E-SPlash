#include "Config.h"

void setup()
{
  Serial.begin(9600);
  configureWiFiSettings();
  scanAvailableWiFiNetworks();
  initiateWiFiConnection();
  disableAllRelays();
  printCompileTime();
  setupRTC();
  setupStorage();
  server.begin();
  serverHandle(); 
}

void loop()
{
  handleWiFiConnection();
  if (isScheduleMode)
  {
    scheduleMode();
  }
  sleep(1);
}