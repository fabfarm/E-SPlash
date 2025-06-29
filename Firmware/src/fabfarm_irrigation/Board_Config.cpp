#include "Config.h"
#include "Board_Config.h"

// Single board configuration - relay pins defined in platformio.ini
const BoardConfig config = {
    "FabFarm_Irrigation",      // Board name
    "fabfarm-irrigation",      // WiFi network hostname
    "FabFarm_Irrigation_AP",   // Soft AP SSID
    ""                         // Soft AP password
};