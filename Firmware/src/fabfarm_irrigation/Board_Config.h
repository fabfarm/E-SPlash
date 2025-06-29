#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "Config.h"

// Hardware configuration - pins are defined in platformio.ini
struct BoardConfig
{
  const char *name;
  const char *wifi_hostname;
  const char *ap_ssid;
  const char *ap_password;
};

extern const BoardConfig config;

#endif