#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Heap-allocated chat reply. Always zero-initialize before passing
 *        to chat_client_send(), and free with chat_client_free_response()
 *        once done with it (safe to call even if @c text is NULL).
 */
typedef struct {
    char *text; /**< Assistant's reply, owned by the caller after a
                     successful chat_client_send(). NULL on failure. */
} chat_response_t;

/**
 * @brief Create the underlying OpenAI_t / OpenAI_ChatCompletion_t objects
 *        and apply every sampling parameter from
 *        components/chat_client/Kconfig (model, system prompt, max tokens,
 *        temperature, top-p, penalties, user id).
 *
 * Call once, after Wi-Fi is up. Safe to call more than once (a no-op if
 * already initialized).
 *
 * @return ESP_OK on success.
 *         ESP_ERR_INVALID_STATE if CONFIG_OPENAI_API_KEY is empty.
 *         ESP_ERR_NO_MEM if the underlying component failed to allocate.
 */
esp_err_t chat_client_init(void);

/** @brief Releases the chat completion and OpenAI objects. */
void chat_client_deinit(void);

/**
 * @brief Send one user message and get the model's reply.
 *
 * Wraps the esp-iot-solution `openai` component's
 * OpenAI_ChatCompletion_t::message() call.
 *
 * @param user_message  Null-terminated UTF-8 text from the user.
 * @param out_response  Must be zero-initialized by the caller. On ESP_OK,
 *                       out_response->text holds a heap-allocated copy of
 *                       the reply - free it with chat_client_free_response().
 * @return ESP_OK on success.
 *         ESP_ERR_INVALID_STATE if chat_client_init() hasn't succeeded yet.
 *         ESP_ERR_INVALID_ARG if either pointer argument is NULL.
 *         ESP_ERR_NO_MEM on allocation failure.
 *         ESP_FAIL on a transport or API-level error (details are logged
 *         via ESP_LOGE under the "chat_client" tag).
 */
esp_err_t chat_client_send(const char *user_message, chat_response_t *out_response);

/** @brief Frees the text owned by @p resp and clears the pointer. */
void chat_client_free_response(chat_response_t *resp);

/**
 * @brief Clears conversation history kept by the underlying chat
 *        completion object (see CONFIG_OPENAI_SAVE_HISTORY).
 */
void chat_client_reset_conversation(void);

#ifdef __cplusplus
}
#endif
