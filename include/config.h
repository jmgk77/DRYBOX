#pragma once

#define config_SIGNATURE 'J'

struct config_data {
  unsigned char sign = config_SIGNATURE;
  unsigned int checksum;
  //
  char device_name[32];
  char mqtt_server_ip[64];
  unsigned int mqtt_server_port;
  char mqtt_server_username[64];
  char mqtt_server_password[64];
};

void default_config();
void save_config();
void init_config();