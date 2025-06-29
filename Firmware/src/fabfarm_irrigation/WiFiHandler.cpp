#include "WiFiHandler.h"
#include "Config.h"
#include "TimeHandler.h"


void printAccessPointIP()
{
    Serial.println();
    Serial.println("*********************");
    Serial.printf("* SoftAP IP is: %s\n\r", WiFi.softAPIP().toString().c_str());
}

void printConnectedWiFiStatus()
{
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void printDisconnectedWiFiStatus()
{
    Serial.println("WiFi not connected!");
}

void printWiFiScanStatus(int networkCount)
{
    Serial.println("WiFi scan complete");

    if (networkCount == 0)
    {
        Serial.println("No WiFi networks found.");
    }
    else
    {
        Serial.printf("Found %d WiFi networks:\n", networkCount);
    }
}

void printDiscoveredWiFiNetworks(int networkCount)
{
    for (int i = 0; i < networkCount; i++)
    {
        Serial.printf("%d: ", i + 1);
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(" dBm");

        if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)
        {
            Serial.print(", open");
        }
        else
        {
            Serial.print(", encrypted");
        }

        Serial.println();
    }
}

void initiateWiFiConnection()
{
    Serial.println("Connecting Wifi...");
    if (wifiMulti.run() == WL_CONNECTED)
    {
        printConnectedWiFiStatus();
        
        // Schedule initial internet time sync (non-blocking)
        Serial.println("* WiFi connected - internet time sync will be attempted");
        // Note: syncWithInternetTime() will be called from the main loop when convenient
    }
    else
    {
        Serial.println("WiFi connection failed.");
    }
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
}

void handleWiFiConnection()
{
    static unsigned long previousTime = 0; // Declare as a static variable
    const unsigned long eventInterval = 10000;
    const uint32_t connectTimeoutMs = 10000;

    unsigned long currentTime = millis();

    if (currentTime - previousTime >= eventInterval)
    {
        if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED)
        {
            Serial.print("WiFi connected: ");
            Serial.print(WiFi.SSID());
            Serial.print(" ");
            Serial.println(WiFi.RSSI());
            
            // Handle internet time synchronization when WiFi is connected
            handleInternetTimeSync();
        }
        else
        {
            printDisconnectedWiFiStatus();
        }

        previousTime = currentTime;
    }
}

void scanAvailableWiFiNetworks()
{
    int networkCount = WiFi.scanNetworks();
    printWiFiScanStatus(networkCount);
    if (networkCount > 0)
    {
        printDiscoveredWiFiNetworks(networkCount);
    }
}

void loadCredentialsFromFile()
{
    Serial.println("Reading credentials file...");
    String credentialsData = readFile("/credentials.txt");
    for (int pos = 0, foundAt = 0; foundAt != -1; pos = foundAt + 1)
    {
        foundAt = credentialsData.indexOf('\n', pos);
        String line = credentialsData.substring(pos, foundAt != -1 ? foundAt : credentialsData.length());
        int separatorIndex = line.indexOf(",");
        if (separatorIndex != -1)
        {
            String ssid = line.substring(0, separatorIndex);
            String password = line.substring(separatorIndex + 1);
            wifiMulti.addAP(ssid.c_str(), password.c_str());
            Serial.printf("Loaded credential - SSID: %s, Password: %s\n", ssid.c_str(), password.c_str());
        }
    }
}

void configureWiFiSettings()
{
    WiFi.mode(WIFI_MODE_APSTA);
    loadCredentialsFromFile();
    scanAvailableWiFiNetworks();
    initiateWiFiConnection();
}

void initializeServer()
{
    server.begin();
}