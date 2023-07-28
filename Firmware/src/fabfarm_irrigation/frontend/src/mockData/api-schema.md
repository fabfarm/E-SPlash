## API SCHEMA

### Fetch all data

GET /data  
response: 200 `testdata.json`  
errors: 401 or 500

### toggle a relay

POST /relays/:relayId/enable  
payload: `{ "isEnabled": boolean }`  
response: 200 `{ "isEnabled": boolean }`  
errors: 401 or 500

### toggle a relay schedule mode

POST /relays/:relayId/schedule-mode  
payload: `{ "isScheduleMode": boolean }`  
response: 200 `{ "isScheduleMode": boolean }`  
errors: 401, 500

### add a schedule to a relay

POST /relays/:relayId/schedule  
payload: `{ "startTime: string, "duration": number }`  
_example ISO date string: "1970-01-01T00:00"_  
response: 200 `{ "id": string, "startTime": string , "duration": number }`  
errors: 401, 500

### remove a relay schedule

DELETE /relays/:relayId/schedule/:scheduleid  
response: 200 `OK`  
errors: 401, 500

### modify a relay schedule

PATCH /relays/:relayId/schedule/:scheduleid  
payload: `{ "startTime: string, "duration": number }`  
response: 200 `{ "id": string, "startTime": string , "duration": number }`  
errors: 401, 500

### set the global time

POST /time/set  
payload: `{ time: "1970-01-01T00:00" }`  
response: 200 `{ time: "1970-01-01T00:00" }`  
errors: 401, 500

### remove a relay

DELETE /relays/:id  
response: 200 `OK`  
errors: 401, 500

### add a relay

POST /relays/add  
payload: `{ "name": "Fruit", "pin": 27 }`  
response: 200 `{ "id": string, "name": string, "pin": number, "isScheduleMode": boolean, "isEnabled": boolean, "schedules": object[] }`  
errors: 401, 500

### set wifi credentials

POST /credentials  
payload: `{"ssid": string, "password": string }`  
response: 200 `OK`  
errors: 401, 500
