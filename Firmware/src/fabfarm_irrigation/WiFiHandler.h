#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include "Config.h"

void printAccessPointIP();
void printConnectedWiFiStatus();
void printDisconnectedWiFiStatus();
void printWiFiScanStatus(int networkCount);
void printDiscoveredWiFiNetworks(int networkCount);
void initiateWiFiConnection();
void handleWiFiConnection();
void scanAvailableWiFiNetworks();
void configureWiFiSettings();

#endif // WIFIHANDLER_H