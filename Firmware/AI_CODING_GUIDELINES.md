# AI Coding Guidelines for FabFarm Irrigation Project

## CRITICAL RULES FOR AI ASSISTANTS

### 1. MAIN.CPP MUST REMAIN CLEAN
**DO NOT add any implementation code to `main.cpp`**

- `main.cpp` contains ONLY function calls
- All logic belongs in separate modules
- Keep the non-monolithic architecture
- New functionality goes in appropriate handler files

**Current main.cpp structure (PRESERVE THIS):**
```cpp
#include "Config.h"

void setup() {
  Serial.begin(9600);
  setupStorage();
  readDataJson();
  configureWiFiSettings();
  scanAvailableWiFiNetworks();
  initiateWiFiConnection();
  disableAllDevices();
  printCompilationTimestamp();
  initializeRtc();
  initializeServer();
  serverHandle();
}

void loop() {
  handleWiFiConnection();
  if (isScheduleMode) {
    scheduleMode();
  }
}
```

### 2. NO DELAY() FUNCTIONS ON ESP32
**NEVER use `delay()` with ESP32 web servers**

- ESP32 with web servers cannot afford to block
- Use `millis()` for timing instead
- Non-blocking code is mandatory

**Example - WRONG:**
```cpp
delay(1000); // NEVER DO THIS
```

**Example - CORRECT:**
```cpp
unsigned long previousMillis = 0;
const long interval = 1000;

unsigned long currentMillis = millis();
if (currentMillis - previousMillis >= interval) {
  previousMillis = currentMillis;
  // Do your timed action here
}
```

### 3. HARDWARE CONFIGURATION (FIXED)
**Pins are HARD-WIRED on PCB - DO NOT CHANGE:**
- Relay 1 (Fruit): Pin 27
- Relay 2 (Vegetables): Pin 26  
- Relay 3 (Water): Pin 25
- Pump: Pin 33

These are defined in `platformio.ini` build flags and cannot be modified.

### 4. MODULAR ARCHITECTURE
**Keep functionality separated in dedicated files:**
- `Config.h/.cpp` - Global configuration and variables
- `Board_Config.h/.cpp` - Hardware abstraction
- `WiFiHandler.h/.cpp` - WiFi management
- `WebServerManager.h/.cpp` - Web server and API endpoints
- `RelayHandler.h/.cpp` - Relay control logic
- `TimeHandler.h/.cpp` - Time and scheduling
- `JsonHandler.h/.cpp` - JSON parsing and creation
- `storageHandler.h/.cpp` - File system operations

### 5. IMPLEMENTATION GUIDELINES
- Add new functions to appropriate handler files
- Declare functions in corresponding `.h` files
- Include necessary headers in `Config.h` if globally needed
- Use consistent naming conventions
- Maintain existing code style

### 6. WEB SERVER CONSIDERATIONS
- All server endpoints in `WebServerManager.cpp`
- Use async web server patterns
- Avoid blocking operations in request handlers
- Keep response times minimal

### 7. WHAT NOT TO DO
- ❌ Add implementation code to `main.cpp`
- ❌ Use `delay()` functions
- ❌ Change hardware pin assignments
- ❌ Create monolithic functions
- ❌ Block the main loop
- ❌ Ignore the modular structure

### 8. WHEN ADDING NEW FEATURES
1. Identify the appropriate handler file
2. Add function declaration to `.h` file
3. Implement function in `.cpp` file
4. Add function call to appropriate location (usually in a handler, not main.cpp)
5. Test compilation
6. Maintain clean separation of concerns

**Remember: This project uses a clean, modular architecture. Keep it that way!**
