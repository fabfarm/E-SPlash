# FabLab Irrigation System

## Design

C++ for a Microprocessor that includes an embedded http server.

The http server displays controls for N zones, where each zone has:

- turn on time
- turn on duration

```
Valve1 \
Valve2  --- Microprocessor <==> Web Interface  
ValveN /
```

## Web server methods

| **Endpoint** | **Purpose**       |
|--------------|-------------------|
| /getData     | returns json      |
| /saveData    | gets updated json |

## Core Logic

**State Problem**  
When the microcontroller loses power, we lose state. We therefore need to persist settings to disk.

- Settings are *read* from disk on startup
- Settings are *saved* each time a user changes settings in the web ui
- Settings are kept in memory and we check against them

## Data format

```json
{
    "data": {
        "currentTime": "Tuesday, June 23 2020 11:54:16",
        "temperature": "29.90",
        "humidity": "51.00",
        "override": 0,
        "ssid": "rato",
        "pass": "imakestuff"
    },
    "relays": [
        {
            "name": "Fruit Tree",
            "pin": 25,
            "isRunning": 1,
            "isEnabled": 0,
            "times": [
                {
                    "startTime": "03:00",
                    "duration": "0"
                }
            ]
        },
        {
            "name": "Vegie Garden",
            "pin": 26,
            "isRunning": 1,
            "isEnabled": 0,
            "status": 1,
            "times": [
                {
                    "startTime": "12:00",
                    "duration": "0"
                },
                {
                    "startTime": "12:30",
                    "duration": "0"
                }
            ]
        },
        {
            "name": "Cypress Hill",
            "pin": 33,
            "isRunning": 1,
            "isEnabled": 0,
            "status": 1,
            "times": [
                {
                    "startTime": "12:45",
                    "duration": "0"
                },
                {
                    "startTime": "10:00",
                    "duration": "0"
                },
                {
                    "startTime": "16:45",
                    "duration": "0"
                }
            ]
        }
    ]
}


```
