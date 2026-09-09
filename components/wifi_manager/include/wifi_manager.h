#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t wifi_manager_connect(uint32_t timeout_ms);

bool wifi_manager_is_connected(void);

#ifdef __cplusplus
}
#endif
