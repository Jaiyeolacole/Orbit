#include <stdio.h>

#include "config.h"
#include "wifi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"


#include "OpenAI.h"


static const char *TAG = "orbit";

esp_err_t start_wifi()
{
    esp_err_t err = wifi_init_sta(WIFI_SSID, WIFI_PASS);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Wi-Fi init failed: %s", esp_err_to_name(err));
        vTaskDelete(NULL);
        return ESP_OK;
    }
    return ESP_OK;
}

// ******************* main entry ************************
void app_main(void)
{
    start_wifi();
}
