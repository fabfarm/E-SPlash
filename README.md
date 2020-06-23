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
"data": {
        "currentTime": "Saturday, January 10th 10:45",
        "temperature": 23,
        "humidity": 45,
        "override": 1        
    },
    "relays": [
        {
            "name": "Sauna",
            "pin": 25,
            "isRunning": 1,
            "isEnabled": 1,
            "times": [
                { "startTime": "10:00", "duration": 5}
            ]},
        {
            "name": "Garden",
            "pin": 26,
            "isRunning": 1,
            "isEnabled": 0,
            "status": 1,
            "times": [
                { "startTime": "12:45", "duration": 15},
                { "startTime": "16:45", "duration": 20}
            ]
        },
        {
            "name": "Hot Tub",
            "pin": 33,
            "isRunning": 1,
            "isEnabled": 1,
            "status": 1,
            "times": [
                { "startTime": "12:45", "duration": 25},
                { "startTime": "10:00", "duration": 10},
                { "startTime": "16:45", "duration": 30}
            ]
        }

    ]
}

```