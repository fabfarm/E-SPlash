#include "Config.h"

void setup()
{
  Serial.begin(9600);
  setupStorage();
  readDataJson();
  configureWiFiSettings();
  scanAvailableWiFiNetworks();
  initiateWiFiConnection();
  disableAllDevices();
  printCompilationTimestamp();
  initializeRtc();
  initializeServer();
  serverHandle();
}

void loop()
{
  handleWiFiConnection();
  
  // Run irrigation logic if in schedule mode
  if (isScheduleMode)
  {
    scheduleMode();
  }
}