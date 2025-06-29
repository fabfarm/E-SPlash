#include "Config.h"

void setup()
{
  Serial.begin(9600);
  setupStorage();
  readDataJson();
  initializeHardwarePins();
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
  if (isScheduleMode)
  {
    scheduleMode();
  }
  //sleep(1);
}