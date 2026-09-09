#include "esp_sntp.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <time.h>
#include <sys/time.h>

const char *TAG = "sntp";

esp_err_t sync_time(uint32_t timeout_ms)
{
    esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_setservername(1, "time.google.com");
    esp_sntp_init();

    time_t now = 0;
    struct tm timeinfo = { 0 };
    const uint32_t step = 500;
    uint32_t waited = 0;

    while (timeinfo.tm_year < (2020 - 1900) && waited < timeout_ms) {
        vTaskDelay(pdMS_TO_TICKS(step));
        waited += step;
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if (timeinfo.tm_year < (2020 - 1900)) {
        ESP_LOGE(TAG, "SNTP sync failed after %u ms", (unsigned)waited);
        return ESP_ERR_TIMEOUT;
    }

    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    ESP_LOGI(TAG, "Time synced: %s UTC", buf);
    return ESP_OK;
}