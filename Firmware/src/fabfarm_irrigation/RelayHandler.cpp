#include "RelayHandler.h"

bool isWithinTimeslot(int startTimeInMinutes, int duration)
{
  if (duration <= 0)
  {
    return false;
  }

  int presentTimeInMinutes = (rtc.getHour(true) * 60) + rtc.getMinute();

  return presentTimeInMinutes >= startTimeInMinutes && presentTimeInMinutes < startTimeInMinutes + duration;
}

void switchDevice(int pinNumber, bool state, const char *deviceName)
{
  pinMode(pinNumber, OUTPUT);
  digitalWrite(pinNumber, state ? ON : OFF);
  Serial.printf("%s (pin %d) is %s\n\r", deviceName, pinNumber, state ? "on" : "off");
}

void handleDevices(JsonArray devices, bool (*shouldEnableDevice)(JsonObject &))
{
  bool enablePump = false;
  int relayPins[] = {RELAY1_PIN, RELAY2_PIN, RELAY3_PIN};
  
  for (int i = 0; i < devices.size() && i < 3; i++)
  {
    JsonObject device = devices[i];
    int pinNumber = relayPins[i];
    bool enableDevice = shouldEnableDevice(device);
    switchDevice(pinNumber, enableDevice, device["name"]);
    enablePump = enablePump || enableDevice;
  }
  switchDevice(pumpPinNumber, enablePump, "Pump");
}

bool shouldEnableDeviceInScheduleMode(JsonObject &device)
{
  for (JsonObject time : device["times"].as<JsonArray>())
  {
    int hours = time["startTime"].as<String>().substring(0, 2).toInt();
    int minutes = time["startTime"].as<String>().substring(3).toInt();
    int startTimeInMinutes = (hours * 60) + minutes;
    int cycleDuration = time["duration"];
    if (isWithinTimeslot(startTimeInMinutes, cycleDuration))
    {
      return true;
    }
  }
  return false;
}

void scheduleMode()
{
  Serial.println("Schedule Mode");
  handleDevices(doc["relays"], shouldEnableDeviceInScheduleMode);
}

bool shouldEnableDeviceInManualMode(JsonObject &device)
{
  return device["isEnabled"];
}

void manualMode()
{
  Serial.println("Manual Mode");
  handleDevices(doc["relays"], shouldEnableDeviceInManualMode);
}

void disableAllDevices()
{
  switchDevice(pumpPinNumber, false, "Pump");

  int relayPins[] = {RELAY1_PIN, RELAY2_PIN, RELAY3_PIN};
  JsonArray relays = doc["relays"];
  for (int i = 0; i < relays.size() && i < 3; i++)
  {
    JsonObject relay = relays[i];
    switchDevice(relayPins[i], false, relay["name"]);
  }
}
