#ifndef OTA_CONFIG_H
#define OTA_CONFIG_H

#include <Arduino.h>

String eota_responses_strings[] = {
    "EOTA_OK",
    "EOTA_ERROR",
    "EOTA_FAILED",
    "EOTA_NO_MATCH",
    "EOTA_RUNAWAY",
    "EOTA_SIZE_ERROR"
};

// const unsigned short VERSION_NUMBER = 4;
const char * const   VERSION_NUMBER = "4.50.156a"; // fetched from FW
// const char * const   UPDATE_URL     = "https://raw.githubusercontent.com/urishX/esp32-explorations/main/cfg.txt";
const char * const   UPDATE_URL     = CDN_URL;
const unsigned long  UPDATE_INTERVAL= 1000; // ms


#endif