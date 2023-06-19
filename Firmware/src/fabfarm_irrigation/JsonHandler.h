#ifndef JSON_HANDLER_H
#define JSON_HANDLER_H

#include "Config.h"

bool writeFile(String path, String data);
String readFile(String path);
bool writeDataJson();
void readDataJson();
void setupStorage();

#endif // JSON_HANDLER_H