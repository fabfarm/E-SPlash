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
  for (JsonObject device : devices)
  {
    int pinNumber = device["pin"];
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

  JsonArray relays = doc["relays"];
  for (JsonObject relay : relays)
  {
    switchDevice(relay["pin"], false, relay["name"]);
  }
}
