#include "Config.h"
#include "Board_Config.h"

// Define configuration settings for different boards
const BoardConfig configs[] = {
    {
        "0_point_1_main",  // Board name
        33,                // Pump pin
        35,                // Voltage read pin
        21,                // DHT sensor pin
        DHT11,             // DHT sensor type
        "irrigationmain",  // WiFi network hostname
        "irrigation_main", // Soft AP SSID
        ""                 // Soft AP password
    },
    {
        "0_point",                   // Board name
        13,                          // Pump pin
        35,                          // Voltage read pin
        21,                          // DHT sensor pin
        DHT11,                       // DHT sensor type
        "test",                      // WiFi network hostname
        "irrigation_main_prototype", // Soft AP SSID
        ""                           // Soft AP password
    },
    {
        "_1_point_0",      // Board name
        13,                // Pin number for pump
        35,                // Pin number for voltage reading
        21,                // Pin number for DHT sensor
        DHT11,             // DHT sensor type (-1=None, 11=DHT11, 22=DHT22, 21=AM2301)
        "test",            // Hostname of WiFi network
        "irrigation_test", // Soft AP SSID
        ""                 // Soft AP password
    },
    {"_0_point_greenhouse",       // Board name
     33,                          // Pin number for pump
     35,                          // Pin number for voltage reading
     21,                          // Pin number for DHT sensor
     -1,                          // DHT sensor type (-1=None, 11=DHT11, 22=DHT22, 21=AM2301)
     "greenhousetestboard",       // Hostname of WiFi network
     "irrigation_greenhousetest", // Soft AP SSID
     ""}};