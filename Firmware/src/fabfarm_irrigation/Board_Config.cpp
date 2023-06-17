#include "Config.h"
#include "Board_Config.h"

// Define configuration settings for different boards
const BoardConfig configs[] = {
    {
        "0_point_1_main", // Board name
        33,                // Pump pin
        35,                // Voltage read pin
        21,                // DHT sensor pin
        DHT11,             // DHT sensor type (-1=None, 11=DHT11, 22=DHT22, 21=AM2301)
        "irrigationmain",  // WiFi network hostname
        "irrigation_main", // Soft AP SSID
        ""                 // Soft AP password
    },
    {
        "0_point",
        33,
        35,
        21,
        DHT11,
        "test",
        "irrigation_main_prototype",
        ""
    },
    {
        "_1_point_0",
        13,
        35,
        21,
        DHT11,
        "test",
        "irrigation_test",
        ""
    },
    {
        "_0_point_greenhouse",
        33,
        35,
        21,
        -1,
        "greenhousetestboard",
        "irrigation_greenhousetest",
        ""
    }
    };