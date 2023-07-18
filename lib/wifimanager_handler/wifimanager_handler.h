#ifndef WIFIMANAGER_HANDLER_H
#define WIFIMANAGER_HANDLER_H

#include <Arduino.h>

typedef enum {
    wifimanager_wrapper_OK = 0,
    wifimanager_wrapper_DISCONNECTED,
    wifimanager_wrapper_MISSING,
    wifimanager_wrapper_CONNECTED = -1
} wifimanager_wrapper_t;

class Wifimanager_wrapper
{
    private:
        // Function Prototypes
        uint8_t connectMultiWiFi();
        void printLocalTime();
        void heartBeatPrint();
        void format_filesystem_handler();
        void check_WiFi();

    public:
        Wifimanager_wrapper() {};
        void check_status();
        void wifimanager_init_prints();
        void wifimanager_config_and_initialize();
        void wifimanager_start_portal();

    private:
        String _hostname;
};

#endif