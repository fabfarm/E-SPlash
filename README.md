# Fablab Irregation System

HTML <--> JavaScript  <--> C++

Need a polling mechanism 


| **Web Interface**                     | **Microprocessor** (ESP32)                                           |
|---------------------------------------|----------------------------------------------------------------------|
| Set per zone:                         | Receives:                                                            |
|[ ] start times                        |   [ ] start times                                                    |
|[ ] duration of cicle, max 1 hour      |   [ ] Duration or cicle                                              |
|                                       |   [ ] Override button                                                |
| Displays                              | [ ] Stores info above at SPIFFS (csv ? )                             |
|[x] time of day                        | logic test:                                                          |
|[x] Humidity                           |   keep checking:  ({                                    |
|[x] Temperature                        |       - am I ('zone x') in an On window  ('time + duration') ?
                                                        Yes: 'be sure' I'm on   
                                                        No: be sure I'm off
|[ ] current zone programs stored       |       -wait for override button and break cycle if received          |
|[ ] override button                    |   wait for override buton and turn on/off anytime                    |  
|[ ] current zone state  (ON/OFF)       | Updates/sends current situation:                                     |
|                                       | [x]Time of day;                                                      |
|                                       | [ ]State of zone (ON/OFF)                                            |
|                                       | [x]sensor information (humidity/temperature)                         |
|                                       |                                                                      |
|                                       |                                                                      |

microprocessor
    - Runs: webserver
        - /index.html: displays index
        - /temp.htmml: placeholder
        - /farmtimenow: returns time
        - /slider: 
        - /temperature: returns temp
        - /humidity: returns humidity
        - /update: **sets** values

On Startup: 
- we read from csv file 
- keep info in memory to check against in loop (is this the window?)
- we also have to tell the html what settings it should PRE SET

Problem: 
- If the microcontroller loses power, we can lose state. We therefore need a way to persist settings to disk
- What do we need to save?
    - N zones and for each zone:
        - Name
        - start time (timestamp)
        - duration of cycle
        - current state (one off) <-- this can be calculated - if we're between start & stop then should be on !

csv file:
zone1, 20200101 00:00:00.0000, 1234, 1, 12
zone2, 20200101 00:00:00.0000, 1234, 1, 16
zone3, 20200101 00:00:00.0000, 1234, 1, 18

When do I touch this?
- On change (new info)
- On reboot
- That's it
- Otherwise, it's in memory and you already know (!)

Question: 
- 

read the csv file. for each row: split on ',' into array ....

We need to save state: 
- csv or sqlite

