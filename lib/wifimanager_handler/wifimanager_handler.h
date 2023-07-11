#ifndef WIFIMANAGER_HANDLER_H
#define WIFIMANAGER_HANDLER_H

#include <Arduino.h>

// Function Prototypes
uint8_t connectMultiWiFi();

// void printLocalTime();
void heartBeatPrint();

void check_status();

void wifimanager_init_prints();
void format_filesystem_handler();
void wifimanager_config_and_initialize();
void wifimanager_start_portal();

#endif