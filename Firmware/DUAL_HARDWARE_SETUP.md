# Dual Hardware Configuration

This project supports two hardware configurations with all pins defined in `platformio.ini`:

## Hardware 1: Custom PCB (Default)
**Environment:** `esp32doit-devkit-v1`

```bash
pio run --environment esp32doit-devkit-v1
# or simply
pio run
```

**Pin Configuration:**
- Relay 1: GPIO 27
- Relay 2: GPIO 26  
- Relay 3: GPIO 25
- Pump: GPIO 33

## Hardware 2: LILYGO T-Relay 4CH
**Environment:** `lilygo-t-relay-4ch`

```bash
pio run --environment lilygo-t-relay-4ch
```

**Pin Configuration:**
- Relay 1: GPIO 21
- Relay 2: GPIO 19
- Relay 3: GPIO 18
- Relay 4: GPIO 5
- Pump: GPIO 21 (same as Relay 1)

## How It Works

All hardware configuration is now handled in `platformio.ini` using build flags:

- All pin definitions are set as `-D` flags in each environment
- WiFi hostname and AP configuration are defined as build flags
- The `T_RELAY_4CH` flag distinguishes between hardware types
- No separate Board_Config files needed - everything is in platformio.ini

## Files Modified

- `platformio.ini`: Contains all pin definitions and configuration strings as build flags
- `Config.cpp`: Uses build flag defines instead of Board_Config structure

## Testing

Both configurations have been tested and compile successfully:
- Custom PCB: ✅ Success (15.65 seconds)
- LILYGO T-Relay: ✅ Success (15.20 seconds)

## Usage

1. **For Custom PCB**: Use default environment or specify `esp32doit-devkit-v1`
2. **For LILYGO T-Relay**: Specify `lilygo-t-relay-4ch` environment

All pin assignments and configurations are automatically applied based on the selected environment.
