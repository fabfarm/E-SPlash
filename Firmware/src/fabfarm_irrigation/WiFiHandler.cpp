#include "WiFiHandler.h"
#include "Config.h"

void printAccessPointIP() {
    Serial.println();
    Serial.println("*****************************************************");
    Serial.printf("* SoftAP IP is: %s\n\r", WiFi.softAPIP().toString().c_str());
}

void printConnectedWiFiStatus() {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void printDisconnectedWiFiStatus() {
    Serial.println("WiFi not connected!");
}

void printWiFiScanStatus(int networkCount) {
    Serial.println("WiFi scan complete");

    if (networkCount == 0) {
        Serial.println("No WiFi networks found.");
    } else {
        Serial.printf("Found %d WiFi networks:\n", networkCount);
    }
}

void printDiscoveredWiFiNetworks(int networkCount) {
    for (int i = 0; i < networkCount; i++) {
        Serial.printf("%d: ", i + 1);
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(" dBm");

        if (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) {
            Serial.print(", open");
        } else {
            Serial.print(", encrypted");
        }

        Serial.println();
    }
}


void initiateWiFiConnection() {
    Serial.println("Connecting Wifi...");
    if (wifiMulti.run() == WL_CONNECTED) {
        printConnectedWiFiStatus();
    } else {
        Serial.println("WiFi connection failed.");
    }
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
}

void handleWiFiConnection() {
    unsigned long previousTime = 0;
    const unsigned long eventInterval = 10000;
    const uint32_t connectTimeoutMs = 10000;

    unsigned long currentTime = millis();

    if (currentTime - previousTime >= eventInterval) {
        if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
            Serial.print("WiFi connected: ");
            Serial.print(WiFi.SSID());
            Serial.print(" ");
            Serial.println(WiFi.RSSI());
        } else {
            printDisconnectedWiFiStatus();
        }

        previousTime = currentTime;
    }
}

void scanAvailableWiFiNetworks() {
    int networkCount = WiFi.scanNetworks();
    printWiFiScanStatus(networkCount);
    if (networkCount > 0) {
        printDiscoveredWiFiNetworks(networkCount);
    }
}

void configureWiFiSettings() {
    WiFi.mode(WIFI_MODE_APSTA);
    wifiMulti.addAP("ratinho_do_malandro", "gerryforever2018");
    wifiMulti.addAP("fabfarm_ele_container", "imakestuff");
    wifiMulti.addAP("liga_o_gerador", "gerryforever2018");
    wifiMulti.addAP("caravana", "imakestuff");
    wifiMulti.addAP("fabfarm", "imakestuff");
    wifiMulti.addAP("Raccaccoonie", "MalkovichMalkovich");
    wifiMulti.addAP("ubnt_mesh", "gerryforever2018");
    scanAvailableWiFiNetworks();
    initiateWiFiConnection();
}

