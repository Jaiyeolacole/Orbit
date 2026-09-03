#ifndef WIFI_H
#define WIFI_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize Wi-Fi in station mode and start connecting.
 *
 * @param ssid AP SSID.
 * @param password AP password.
 * @return esp_err_t ESP_OK on success, otherwise an ESP error.
 */
esp_err_t wifi_init_sta(const char *ssid, const char *password);

#ifdef __cplusplus
}
#endif

#endif // WIFI_H
