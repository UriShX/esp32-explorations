#include "esp_ver.h"

#include <esp_app_format.h>

char _version[32];

// extern "C"
// {
#include "esp_ota_ops.h"
#include "esp_image_format.h"

String _esp_return_current_fw_version()
{
    return String(esp_ota_get_app_description()->version);
}

void _esp_get_current_sha256(uint8_t * hash_ptr)
{
    for (int i = 0; i < 32; i++)
        hash_ptr[i] = (esp_ota_get_app_description()->app_elf_sha256[i]);
}

// void _esp_get_ota_stream_fw_version(String * ota_fw_verwion)
// {
    
// }