#include "Arduino.h"
#include "DHT.h"

String readDHTHumidity()
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
    float h = dht.readHumidity();
    if (isnan(h))
    {
        Serial.println("Failed to read from DHT sensor!");
        return "Failed to read humidity from sensor";
    }
    else
    {
        Serial.println(h);
        return String(h);
    }
}