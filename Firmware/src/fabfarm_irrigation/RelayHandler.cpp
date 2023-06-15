#include "RelayHandler.h"
#include "Config.h"

bool isWithinTimeslot(int startTimeInMinutes, int duration)
{
  if (duration <= 0)
  {
    return false;
  }

  int onlyHour = rtc.getHour(true);
  int onlyMin = rtc.getMinute();

  int presentTimeInMinutes = (onlyHour * 60) + onlyMin;

  bool isEnabled = false;
  if (presentTimeInMinutes >= startTimeInMinutes && presentTimeInMinutes < startTimeInMinutes + duration)
  {
    isEnabled = true;
  }

  return isEnabled;
}

void scheduleMode()
{
  Serial.println("scheduleMode");
  bool enablePump = false;
  JsonArray relays = doc["relays"];
  for (int i = 0; i < relays.size(); i++)
  {
    bool enableRelay = false;
    int pinNumber = relays[i]["pin"];
    for (int j = 0; j < relays[i]["times"].size(); j++)
    {
      pinMode(pinNumber, OUTPUT);
      int hours = relays[i]["times"][j]["startTime"].as<String>().substring(0, 2).toInt();
      int minutes = relays[i]["times"][j]["startTime"].as<String>().substring(3).toInt();
      int startTimeInMinutes = (hours * 60) + minutes;
      int cycleDuration = relays[i]["times"][j]["duration"];
      if (isWithinTimeslot(startTimeInMinutes, cycleDuration))
      {
        enableRelay = true;
      }
    }

    if (enableRelay)
    {
      digitalWrite(pinNumber, ON);
      Serial.printf("Relay %s (pin %d) is on\n\r", relays[i]["name"].as<char *>(), pinNumber);
      enablePump = true;
    }
    else
    {
      digitalWrite(pinNumber, OFF);
      Serial.printf("Relay %s (pin %d) is off\n\r", relays[i]["name"].as<char *>(), pinNumber);
    }
  }
  switchPump(enablePump);
}

void manualMode()
{
  Serial.println("manualMode");
  int enablePump = false;
  JsonArray relays = doc["relays"];
  for (int i = 0; i < relays.size(); i++)
  {
    int pinNumber = relays[i]["pin"];
    pinMode(pinNumber, OUTPUT);
    if (relays[i]["isEnabled"])
    {
      digitalWrite(pinNumber, ON);
      Serial.printf("Relay %s (pin %d) is on\n\r", relays[i]["name"].as<char *>(), pinNumber);
      enablePump = true;
    }
    else
    {
      digitalWrite(pinNumber, OFF);
      Serial.printf("Relay %s (pin %d) is off\n\r", relays[i]["name"].as<char *>(), pinNumber);
    }
  }

  switchPump(enablePump);
}

void disableAllRelays()
{
  switchPump(false);

  JsonArray relays = doc["relays"];
  for (int i = 0; i < relays.size(); i++)
  {
    int pinNumber = relays[i]["pin"];
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, OFF);
    Serial.printf("Zone %s (pin %d) is off\n\r", relays[i]["name"].as<char *>(), pinNumber);
  }
}

void switchPump(bool state)
{
  pinMode(pumpPinNumber, OUTPUT);

  if (state)
  {
    digitalWrite(pumpPinNumber, ON);
    Serial.printf("Pump (pumpPinNumber %d) is on\n\r", pumpPinNumber);
  }
  else
  {
    digitalWrite(pumpPinNumber, OFF);
    Serial.printf("Pump (pumpPinNumber %d) is off\n\r", pumpPinNumber);
  }
}


