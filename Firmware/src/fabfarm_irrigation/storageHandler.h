#ifndef STORAGE_HANDLER_H
#define STORAGE_HANDLER_H

#include "Config.h"

bool writeFile(String path, String data);
String readFile(String path);
void setupStorage();

#endif