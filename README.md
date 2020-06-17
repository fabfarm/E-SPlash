# Fablab Irregation System

HTML <--> JavaScript  <--> C++

Need a polling mechanism 


| **Web Interface**                     | **Microprocessor** (ESP32)                                           |
|---------------------------------------|----------------------------------------------------------------------|
| Set per zone:                         | Receives:                                                            |
|[ ] start times                        |   [ ] start times                                                    |
|[ ] duration of cicle, max 1 hour      |   [ ] Duration or cicle                                              |
|                                       |   [ ] Override buton                                                 |
| Displays                              | [ ] Stores info above at SPIFFS (JSON file? txt file)                |
|[x] time of day                        | logic test:                                                          |
|[x] Humidity                           |   while (time within settime   ){                                    |
|[x] Temperature                        |       -run cicle at irrigation zone;                                 |
|[ ] current zone programs stored       |       -wait for override button and break cycle if received          |
|[ ] override button                    |   wait for override buton and turn on/off anytime                    |  
|[ ] current zone state  (ON/OFF)       | Updates/sends current situation:                                     |
|                                       | [x]Time of day;                                                      |
|                                       | [ ]State of zone (ON/OFF)                                            |
|                                       | [x]sensor information (humidity/temperature)                         |
|                                       |                                                                      |
|                                       |                                                                      |
|
|
Problem to solve: stacked events

Do I already have something scheduled ?
    - stacked events :o  ==> boolean : isActive ?
If a new scheduling even 'árrives', cancel any others ! <===

Day / time

Open Question:

What does my scheduling look like ? time wise
- every day at time X comes ON
- every day at time Y goes OFF


on... of .... on .. off ..


on  on on off on <-- events get messy




