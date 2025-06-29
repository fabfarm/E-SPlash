#include "OTAHandler.h"

// OTA Configuration
const char* ota_hostname = "esp32-irrigation";
const char* ota_password = "irrigation2024"; // Change this to a secure password

static bool otaInProgress = false;

void initializeOTA() {
  // Only initialize OTA if WiFi is connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected - skipping OTA initialization");
    return;
  }

  // Set OTA hostname
  ArduinoOTA.setHostname(ota_hostname);
  
  // Set OTA password for security
  ArduinoOTA.setPassword(ota_password);
  
  // Setup OTA callbacks
  setupOTACallbacks();
  
  // Start OTA service
  ArduinoOTA.begin();
  
  Serial.println("OTA Ready");
  Serial.printf("OTA Hostname: %s\n", ota_hostname);
  Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
}

void setupOTACallbacks() {
  ArduinoOTA.onStart([]() {
    otaInProgress = true;
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    
    // Disable all devices during OTA for safety
    disableAllDevices();
    
    Serial.println("Start updating " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    otaInProgress = false;
    Serial.println("\nOTA Update completed successfully!");
    Serial.println("Rebooting...");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    static unsigned int lastPercent = 0;
    unsigned int percent = (progress / (total / 100));
    
    // Only print every 10% to reduce spam
    if (percent >= lastPercent + 10) {
      Serial.printf("Progress: %u%%\r", percent);
      lastPercent = percent;
    }
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    otaInProgress = false;
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
}

void handleOTA() {
  // Only handle OTA if WiFi is connected
  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.handle();
  }
}

bool isOTAActive() {
  return otaInProgress;
}
