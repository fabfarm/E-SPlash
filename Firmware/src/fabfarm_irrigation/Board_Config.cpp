#include "Config.h"
#include "Board_Config.h"

// Define configuration settings for different boards
const BoardConfig configs[] = {
    {
        "0_point_1_main", // Board name
        33,                // Pump pin
        "irrigationmain",  // WiFi network hostname
        "irrigation_main", // Soft AP SSID
        ""                 // Soft AP password
    },
    {
        "0_point",
        33,
        "test",
        "irrigation_main_prototype",
        ""
    },
    {
        "_1_point_0",
        13,
        "test",
        "irrigation_test",
        ""
    },
    {
        "_0_point_greenhouse",
        33,
        "greenhousetestboard",
        "irrigation_greenhousetest",
        ""
    }
    };