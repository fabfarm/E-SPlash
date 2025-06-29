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
  initializeOTA();  // Initialize OTA after WiFi connection
  initializeServer();
  serverHandle();
}

void loop()
{
  handleWiFiConnection();
  handleOTA();  // Handle OTA updates
  
  // Only run irrigation logic if OTA is not in progress
  if (!isOTAActive() && isScheduleMode)
  {
    scheduleMode();
  }
}