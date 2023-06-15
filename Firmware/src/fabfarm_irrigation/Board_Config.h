#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "Config.h"

struct BoardConfig
{
  const char *name;
  int pump_pin;
  int voltage_pin;
  int dht_pin;
  int dht_type;
  const char *wifi_hostname;
  const char *ap_ssid;
  const char *ap_password;
#ifdef static_IP
  IPAddress local_IP;
#endif
};

extern const BoardConfig configs[];

#endif