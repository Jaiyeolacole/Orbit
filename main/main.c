#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"

#include "config.h"
#include "wifi_manager.h"
#include "chat_client.h"

static const char *TAG = APP_LOG_TAG;

static void chat_task(void *arg)
{
    static char line[APP_CHAT_INPUT_BUFFER_SIZE];

    printf("\nChat ready. Type a message and press Enter.\n");
    printf("Commands: %s clears history, %s exits the chat loop.\n\n",
           APP_CMD_RESET, APP_CMD_QUIT);

    while (1) {
        printf("you> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[--len] = '\0';
        }
        if (len == 0) {
            continue;
        }

        if (strcmp(line, APP_CMD_QUIT) == 0) {
            break;
        }
        if (strcmp(line, APP_CMD_RESET) == 0) {
            chat_client_reset_conversation();
            printf("(conversation history cleared)\n\n");
            continue;
        }

        if (!wifi_manager_is_connected()) {
            printf("(no wifi connection - message not sent)\n\n");
            continue;
        }

        chat_response_t response = { 0 };
        esp_err_t err = chat_client_send(line, &response);
        if (err == ESP_OK) {
            printf("assistant> %s\n\n", response.text ? response.text : "");
            chat_client_free_response(&response);
        } else {
            printf("(chat request failed: %s - see logs for detail)\n\n",
                   esp_err_to_name(err));
        }
    }

    ESP_LOGI(TAG, "chat task exiting");
    vTaskDelete(NULL);
}

void app_main(void)
{
    ESP_LOGI(TAG, "connecting to wifi...");
    esp_err_t ret = wifi_manager_connect(APP_WIFI_CONNECT_TIMEOUT_MS);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "wifi connection failed: %s", esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "wifi connected");

    ret = chat_client_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "chat client init failed: %s", esp_err_to_name(ret));
        return;
    }

    xTaskCreate(chat_task, APP_TASK_NAME, APP_TASK_STACK_SIZE, NULL,
                APP_TASK_PRIORITY, NULL);
}
