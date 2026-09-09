#pragma once

#include "sdkconfig.h"

#define APP_LOG_TAG "chat_app"

#define APP_WIFI_CONNECT_TIMEOUT_MS CONFIG_APP_WIFI_CONNECT_TIMEOUT_MS
#define APP_CHAT_INPUT_BUFFER_SIZE  CONFIG_APP_CHAT_INPUT_BUFFER_SIZE
#define APP_TASK_STACK_SIZE         CONFIG_APP_TASK_STACK_SIZE
#define APP_TASK_PRIORITY           CONFIG_APP_TASK_PRIORITY
#define APP_TASK_NAME               "chat_task"

#define APP_CMD_RESET "/reset"
#define APP_CMD_QUIT  "/quit"
