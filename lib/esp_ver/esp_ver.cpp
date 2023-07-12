#include "esp_ver.h"

#include <esp_app_format.h>

#include "esp_ota_ops.h"
#include "esp_image_format.h"

String _esp_return_current_fw_version()
{
    return String(esp_ota_get_app_description()->version);
}