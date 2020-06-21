#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include "ArduinoJson-v6.15.2.h"

/* 
Sample POC for json serialize / deserialize 
---
Sample testing json serialize / deserialize.

Command line compiled + run with:
> g++ -I. test.cpp -o test.exe
> ./test.exe 

Sample json for https://arduinojson.org/v6/assistant/
{
    "data": { "time": 1351824120, "temperature": 23, "humidity": 45}, 
    "zones": [
      { "name": "valve1", "pin": 123,  "status": 1 , "duration": 1000, "times": ["00:00", "01:00"]}, 
      { "name": "valve2", "pin": 234, "status": 1 , "duration": 3000, "times": ["00:00", "01:00"]}, 
      { "name": "valve3", "pin": 345, "status": 0, "duration": 2000, "times": ["00:00", "01:00"]}
    ]
  }
*/

using namespace std;
// prototype
void serialize();
void deserialize(const char*);


int main() { 

    // dump to disk 
    const char *output = "input.json";
    ofstream f;
    f.open (output);
    f << "Writing this to a file";
    f.close();

    // read from disk
    const char *dataFile = "output.json";
    ifstream t(dataFile);
    stringstream buffer;
    buffer << t.rdbuf();
    cout<<buffer.str()<<"\n";

    const char* json = "{\"data\":{\"time\":1351824120,\"temperature\":23,\"humidity\":45},\"valves\":[{\"name\":\"valve1\",\"pin\":123,\"status\":1,\"duration\":1000,\"times\":[\"00:00\",\"01:00\"]},{\"name\":\"valve2\",\"pin\":234,\"status\":1,\"duration\":3000,\"times\":[\"00:00\",\"01:00\"]},{\"name\":\"valve3\",\"pin\":345,\"status\":10,\"duration\":2000,\"times\":[\"00:00\",\"01:00\"]}]}";

    serialize();
    return 0;
}

void serialize() {
    const size_t capacity = 3*JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 3*JSON_OBJECT_SIZE(5);
    DynamicJsonDocument doc(capacity);

    JsonObject data = doc.createNestedObject("data");
    data["time"] = 1351824120;
    data["temperature"] = 23;
    data["humidity"] = 45;

    JsonArray valves = doc.createNestedArray("valves");

    JsonObject valves_0 = valves.createNestedObject();
    valves_0["name"] = "valve1";
    valves_0["pin"] = 123;
    valves_0["status"] = 1;
    valves_0["duration"] = 1000;

    JsonArray valves_0_times = valves_0.createNestedArray("times");
    valves_0_times.add("00:00");
    valves_0_times.add("01:00");

    JsonObject valves_1 = valves.createNestedObject();
    valves_1["name"] = "valve2";
    valves_1["pin"] = 234;
    valves_1["status"] = 1;
    valves_1["duration"] = 3000;

    JsonArray valves_1_times = valves_1.createNestedArray("times");
    valves_1_times.add("00:00");
    valves_1_times.add("01:00");

    JsonObject valves_2 = valves.createNestedObject();
    valves_2["name"] = "valve3";
    valves_2["pin"] = 345;
    valves_2["status"] = 10;
    valves_2["duration"] = 2000;

    JsonArray valves_2_times = valves_2.createNestedArray("times");
    valves_2_times.add("00:00");
    valves_2_times.add("01:00");

    //File file = SD.open("out.json", FILE_WRITE);
    string output;
    serializeJson(doc, output);
    //deserializeJson(doc, Serial);
    cout<<output<<"\n";

}

void deserialize(const char* json) {

    const size_t capacity = 3*JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 3*JSON_OBJECT_SIZE(5) + 210;
    DynamicJsonDocument doc(capacity);

    deserializeJson(doc, json);

    JsonObject data = doc["data"];
    long data_time = data["time"]; // 1351824120
    int data_temperature = data["temperature"]; // 23
    int data_humidity = data["humidity"]; // 45

    JsonArray valves = doc["valves"];

    JsonObject valves_0 = valves[0];
    const char* valves_0_name = valves_0["name"]; // "valve1"
    int valves_0_pin = valves_0["pin"]; // 123
    int valves_0_status = valves_0["status"]; // 1
    int valves_0_duration = valves_0["duration"]; // 1000

    const char* valves_0_times_0 = valves_0["times"][0]; // "00:00"
    const char* valves_0_times_1 = valves_0["times"][1]; // "01:00"

    JsonObject valves_1 = valves[1];
    const char* valves_1_name = valves_1["name"]; // "valve2"
    int valves_1_pin = valves_1["pin"]; // 234
    int valves_1_status = valves_1["status"]; // 1
    int valves_1_duration = valves_1["duration"]; // 3000

    const char* valves_1_times_0 = valves_1["times"][0]; // "00:00"
    const char* valves_1_times_1 = valves_1["times"][1]; // "01:00"

    JsonObject valves_2 = valves[2];
    const char* valves_2_name = valves_2["name"]; // "valve3"
    int valves_2_pin = valves_2["pin"]; // 345
    int valves_2_status = valves_2["status"]; // 10
    int valves_2_duration = valves_2["duration"]; // 2000

    const char* valves_2_times_0 = valves_2["times"][0]; // "00:00"
    const char* valves_2_times_1 = valves_2["times"][1]; // "01:00"
}
