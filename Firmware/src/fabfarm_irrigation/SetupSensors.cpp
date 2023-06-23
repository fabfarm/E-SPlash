#include "SetupSensors.h"

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);
const unsigned long readInterval = 20000;

// Function to read DHT sensor data
float readDHTData(bool isTemperature)
{
    static unsigned long lastReadTime = 0;
    
    if (DHTTYPE == -1)
    {
        Serial.println("Invalid DHT Type!");
        return NAN;
    }

    if (millis() - lastReadTime > readInterval)
    {
        lastReadTime = millis();

        float data = isTemperature ? dht.readTemperature() : dht.readHumidity();

        if (isnan(data))
        {
            Serial.println("Failed to read from DHT sensor!");
        }
        else
        {
            Serial.printf("%s: %f\n\r", isTemperature ? "Temperature" : "Humidity", data);
        }

        return data;
    }

    // If the time since last read is less than the readInterval
    // Serial.println("Too soon to read sensor again!");
    return NAN;
}

// Function to read DHT temperature
float readDHTTemperature()
{
    return readDHTData(true);
}

// Function to read DHT humidity
float readDHTHumidity()
{
    return readDHTData(false);
}

// Function to get battery level
float getBatteryLevel()
{
    static unsigned long lastReadTime = 0;
    if (millis() - lastReadTime > readInterval)
    {
        lastReadTime = millis();
        float batteryVoltage = analogRead(batVoltPin);
        float batteryLevel = map(batteryVoltage, 0.0f, 1866.0f, 0, 100);
        if (batteryLevel > 100)
        {
            batteryLevel = 100;
        }
        else if (batteryLevel < 0)
        {
            batteryLevel = 0;
        }
        Serial.printf("Battery Level: %f\n\r", batteryLevel);
        return batteryLevel;
    }
    return NAN;
}
