
#include "Arduino.h"
#include "DHT.h"

String readDHTTemperature()
{ 
    // Digital pin connected to the DHT sensor
    #define DHTPIN 32

    // Uncomment the type of sensor in use:
    #define DHTTYPE DHT11 // DHT 11
    //#define DHTTYPE    DHT22     // DHT 22 (AM2302)
    //#define DHTTYPE    DHT21     // DHT 21 (AM2301)
    //Send the pin and type of sensor
    DHT dht(DHTPIN, DHTTYPE);
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float t = dht.readTemperature(true);
    // Check if any reads failed and exit early (to try again).
    if (isnan(t))
    {
        Serial.println("Failed to read from DHT sensor!");
        return "Failed to read temperator from sensor";
    }
    else
    {
        Serial.println(t);
        return String(t);
    }
}