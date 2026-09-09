#include "chat_client.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "sdkconfig.h"

#include "OpenAI.h"

static const char *TAG = "chat_client";

static OpenAI_t *s_openai = NULL;
static OpenAI_ChatCompletion_t *s_chat = NULL;

esp_err_t chat_client_init(void)
{
    if (s_openai != NULL) {
        return ESP_OK; 
    }

    if (strlen(CONFIG_OPENAI_API_KEY) == 0) {
        ESP_LOGE(TAG, "CONFIG_OPENAI_API_KEY is empty - set it with "
                       "`idf.py menuconfig` under "
                       "'OpenAI Chat Client Configuration'");
        return ESP_ERR_INVALID_STATE;
    }

    s_openai = OpenAICreate(CONFIG_OPENAI_API_KEY);
    if (s_openai == NULL) {
        ESP_LOGE(TAG, "OpenAICreate() failed");
        return ESP_ERR_NO_MEM;
    }

    if (strlen(CONFIG_OPENAI_BASE_URL) > 0) {
        OpenAIChangeBaseURL(s_openai, CONFIG_OPENAI_BASE_URL);
    }

    s_chat = s_openai->chatCreate(s_openai);
    if (s_chat == NULL) {
        ESP_LOGE(TAG, "chatCreate() failed");
        OpenAIDelete(s_openai);
        s_openai = NULL;
        return ESP_ERR_NO_MEM;
    }

    s_chat->setModel(s_chat, CONFIG_OPENAI_MODEL);
    s_chat->setSystem(s_chat, CONFIG_OPENAI_SYSTEM_PROMPT);
    s_chat->setMaxTokens(s_chat, CONFIG_OPENAI_MAX_TOKENS);
    s_chat->setTemperature(s_chat, CONFIG_OPENAI_TEMPERATURE / 100.0f);
    s_chat->setTopP(s_chat, CONFIG_OPENAI_TOP_P / 100.0f);
    s_chat->setPresencePenalty(s_chat, CONFIG_OPENAI_PRESENCE_PENALTY / 100.0f);
    s_chat->setFrequencyPenalty(s_chat, CONFIG_OPENAI_FREQUENCY_PENALTY / 100.0f);
    s_chat->setUser(s_chat, CONFIG_OPENAI_USER_ID);

    ESP_LOGI(TAG, "chat client ready (model=\"%s\")", CONFIG_OPENAI_MODEL);
    return ESP_OK;
}

void chat_client_deinit(void)
{
    if (s_openai != NULL && s_chat != NULL) {
        s_openai->chatDelete(s_chat);
        s_chat = NULL;
    }
    if (s_openai != NULL) {
        OpenAIDelete(s_openai);
        s_openai = NULL;
    }
}

esp_err_t chat_client_send(const char *user_message, chat_response_t *out_response)
{
    if (s_openai == NULL || s_chat == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (user_message == NULL || out_response == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    out_response->text = NULL;

#if CONFIG_OPENAI_SAVE_HISTORY
    const bool save_history = true;
#else
    const bool save_history = false;
#endif

    OpenAI_StringResponse_t *result = s_chat->message(s_chat, user_message, save_history);
    if (result == NULL) {
        ESP_LOGE(TAG, "message() returned NULL (out of memory or transport failure)");
        return ESP_FAIL;
    }

    char *api_error = result->getError(result);
    if (api_error != NULL) {
        ESP_LOGE(TAG, "OpenAI API error: %s", api_error);
        result->deleteResponse(result);
        return ESP_FAIL;
    }

    if (result->getLen(result) == 0) {
        ESP_LOGW(TAG, "empty response from model");
        result->deleteResponse(result);
        return ESP_FAIL;
    }

    const char *choice = result->getData(result, 0);
    out_response->text = choice ? strdup(choice) : NULL;

    result->deleteResponse(result);

    return (out_response->text != NULL) ? ESP_OK : ESP_ERR_NO_MEM;
}

void chat_client_free_response(chat_response_t *resp)
{
    if (resp == NULL) {
        return;
    }
    free(resp->text);
    resp->text = NULL;
}

void chat_client_reset_conversation(void)
{
    if (s_chat != NULL) {
        s_chat->clearConversation(s_chat);
    }
}
