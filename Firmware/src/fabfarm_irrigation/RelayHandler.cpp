#include "RelayHandler.h"

// State tracking to prevent redundant logging
static bool lastRelayStates[3] = {false, false, false};
static bool lastPumpState = false;
static unsigned long lastScheduleCheck = 0;
static unsigned long nextScheduleEvent = 0;
static const unsigned long SCHEDULE_CHECK_INTERVAL = 60000; // Check every minute as fallback

bool isWithinTimeslot(int startTimeInMinutes, int duration)
{
  if (duration <= 0)
  {
    return false;
  }

  int presentTimeInMinutes = (rtc.getHour(true) * 60) + rtc.getMinute();

  return presentTimeInMinutes >= startTimeInMinutes && presentTimeInMinutes < startTimeInMinutes + duration;
}

// Calculate the next schedule event time (start or end of any schedule)
unsigned long getNextScheduleEventTime()
{
  unsigned long nextEvent = ULONG_MAX;
  int currentMinutes = (rtc.getHour(true) * 60) + rtc.getMinute();
  
  for (JsonObject relay : doc["relays"].as<JsonArray>()) {
    for (JsonObject time : relay["times"].as<JsonArray>()) {
      int hours = time["startTime"].as<String>().substring(0, 2).toInt();
      int minutes = time["startTime"].as<String>().substring(3).toInt();
      int startTimeInMinutes = (hours * 60) + minutes;
      int endTimeInMinutes = startTimeInMinutes + time["duration"].as<int>();
      
      // Find next start event after current time
      if (startTimeInMinutes > currentMinutes && startTimeInMinutes < nextEvent) {
        nextEvent = startTimeInMinutes;
      }
      
      // Find next end event after current time
      if (endTimeInMinutes > currentMinutes && endTimeInMinutes < nextEvent) {
        nextEvent = endTimeInMinutes;
      }
      
      // Handle schedules that cross midnight (next day)
      int nextDayStart = startTimeInMinutes + (24 * 60);
      int nextDayEnd = endTimeInMinutes + (24 * 60);
      if (nextDayStart < nextEvent) nextEvent = nextDayStart;
      if (nextDayEnd < nextEvent) nextEvent = nextDayEnd;
    }
  }
  
  // If no events found or all events are past, check again in 1 hour
  if (nextEvent == ULONG_MAX) {
    nextEvent = currentMinutes + 60;
  }
  
  return nextEvent;
}

void switchDevice(int pinNumber, bool state, const char *deviceName)
{
  pinMode(pinNumber, OUTPUT);
  digitalWrite(pinNumber, state ? ON : OFF);
  
  // Only print if this is a manual override or forced print (not from handleDevices with state tracking)
  Serial.printf("%s (pin %d) is %s\n\r", deviceName, pinNumber, state ? "on" : "off");
}

void switchDeviceQuiet(int pinNumber, bool state, const char *deviceName, bool &lastState, bool &hasChanged)
{
  pinMode(pinNumber, OUTPUT);
  digitalWrite(pinNumber, state ? ON : OFF);
  
  if (lastState != state) {
    hasChanged = true;
    lastState = state;
    Serial.printf("%s (pin %d) is %s\n\r", deviceName, pinNumber, state ? "on" : "off");
  }
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

bool handleDevicesWithStateTracking(JsonArray devices, bool (*shouldEnableDevice)(JsonObject &))
{
  bool enablePump = false;
  int relayPins[] = {RELAY1_PIN, RELAY2_PIN, RELAY3_PIN};
  bool hasAnyChange = false;
  
  for (int i = 0; i < devices.size() && i < 3; i++)
  {
    JsonObject device = devices[i];
    int pinNumber = relayPins[i];
    bool enableDevice = shouldEnableDevice(device);
    switchDeviceQuiet(pinNumber, enableDevice, device["name"], lastRelayStates[i], hasAnyChange);
    enablePump = enablePump || enableDevice;
  }
  switchDeviceQuiet(pumpPinNumber, enablePump, "Pump", lastPumpState, hasAnyChange);
  
  return hasAnyChange;
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
  unsigned long currentMillis = millis();
  int currentMinutes = (rtc.getHour(true) * 60) + rtc.getMinute();
  
  // Check if we've reached a scheduled event time or fallback interval
  bool shouldCheck = (nextScheduleEvent > 0 && currentMinutes >= nextScheduleEvent) ||
                     (currentMillis - lastScheduleCheck >= SCHEDULE_CHECK_INTERVAL);
  
  if (shouldCheck) {
    lastScheduleCheck = currentMillis;
    
    bool hasChanges = handleDevicesWithStateTracking(doc["relays"], shouldEnableDeviceInScheduleMode);
    
    // Calculate when the next schedule event will occur
    nextScheduleEvent = getNextScheduleEventTime();
    
    // Only print when there are actual changes
    if (hasChanges) {
      Serial.printf("Schedule Mode - State Changed (next check at %02d:%02d)\n\r", 
                    (int)(nextScheduleEvent / 60), (int)(nextScheduleEvent % 60));
    }
  }
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
