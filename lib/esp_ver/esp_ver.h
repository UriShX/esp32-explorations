#ifndef _ESP_VER_H_
#define _ESP_VER_H_

#include <Arduino.h>

String _esp_return_current_fw_version();
void _esp_get_current_sha256(uint8_t * hash_ptr);

#endif