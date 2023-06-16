#include "Config.h"

void setup()
{
  Serial.begin(9600);
  configureWiFiSettings();
  scanAvailableWiFiNetworks();
  initiateWiFiConnection();
  disableAllRelays();
  printCompilationTimestamp();
  initializeRtc();
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