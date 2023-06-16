#ifndef SETUPSENSORS_H
#define SETUPSENSORS_H

#include "Config.h"

float readDHTData(bool isTemperature);
float readDHTTemperature();
float readDHTHumidity();
float getBatteryLevel(); 

#endif // SETUPSENSORS_H