#include "SetupSensors.h"

float readDHTTemperature()
{
  static unsigned long lastReadTime = 0;
  const unsigned long readInterval = 2000;

  if (DHTTYPE == -1)
  {
    return NAN;
  }

  DHT dht(DHTPIN, DHTTYPE);

  if (millis() - lastReadTime > readInterval)
  {
    lastReadTime = millis();

    float temperature = dht.readTemperature();

    if (isnan(temperature))
    {
      Serial.println("Failed to read from DHT sensor!");
    }
    else
    {
      Serial.printf("Temperature: %f\n\r", temperature);
    }

    return temperature;
  }

  return NAN;
}
float readDHTHumidity()
{
  if (DHTTYPE == -1)
  {
    return NAN;
  }

  DHT dht(DHTPIN, DHTTYPE);

  float humidity = dht.readHumidity();

  if (isnan(humidity))
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
    Serial.printf("Humidity: %f\n\r", humidity);
  }

  return humidity;
}
float getBatteryLevel()
{
  float batteryVoltage = analogRead(batVoltPin);
  float batteryLevel = map(batteryVoltage, 0.0f, 1866.0f, 0, 100);

  if (batteryLevel > 100)
  {
    batteryLevel = 100;
  }

  Serial.printf("Battery Level: %f\n\r", batteryLevel);

  return batteryLevel;
}

