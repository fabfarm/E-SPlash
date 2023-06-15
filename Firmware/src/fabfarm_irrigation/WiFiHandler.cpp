#include "WiFiHandler.h"
#include "Config.h"

void startSoftAP()
{
  WiFi.softAP(soft_ap_ssid, soft_ap_password, 3);
  Serial.println();
  Serial.println("*****************************************************");
  Serial.printf("* SoftAP IP is: %s\n\r", WiFi.softAPIP().toString().c_str());
}

void startWifi()
{
  Serial.println("Connecting Wifi...");
  if (wifiMulti.run() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("WiFi connection failed.");
  }
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
}

void wifiLoop()
{
  unsigned long previousTime = 0;
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
    }
    else
    {
      Serial.println("WiFi not connected!");
    }

    previousTime = currentTime;
  }
}

void scanWifi()
{
  int networkCount = WiFi.scanNetworks();

  Serial.println("WiFi scan complete");

  if (networkCount == 0)
  {
    Serial.println("No WiFi networks found.");
  }
  else
  {
    Serial.printf("Found %d WiFi networks:\n", networkCount);

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
}

void setupWiFi()
{
  WiFi.mode(WIFI_MODE_APSTA);
  wifiMulti.addAP("ratinho_do_malandro", "gerryforever2018");
  wifiMulti.addAP("fabfarm_ele_container", "imakestuff");
  wifiMulti.addAP("liga_o_gerador", "gerryforever2018");
  wifiMulti.addAP("caravana", "imakestuff");
  wifiMulti.addAP("fabfarm", "imakestuff");
  wifiMulti.addAP("Raccaccoonie", "MalkovichMalkovich");
  wifiMulti.addAP("ubnt_mesh", "gerryforever2018");
  scanWifi();
  startWifi();
}

void setupWifi()
{
  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.softAP(soft_ap_ssid, soft_ap_password, 3);

  Serial.println();
  Serial.println("*****************************************************");
  Serial.printf("* SoftAP IP is: %s\n\r", WiFi.softAPIP().toString().c_str());

#ifdef stacticIP
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("* STA Failed to configure");
  }
#endif

  WiFi.setHostname(wifi_network_hostname);

  Serial.println("* Waiting for WIFI network...");
  Serial.println("*****************************************************");
  Serial.println();
}

