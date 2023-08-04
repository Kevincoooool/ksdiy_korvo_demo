/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "audio_error.h"
#include "esp_log.h"
#include "recorder_action.h"
#include "audio_recorder.h"

static const char *TAG = "REC_ACTION";

esp_err_t recorder_action_rec_wav_turn_on(void *instance, action_arg_t *arg, action_result_t *result)
{
    ESP_LOGI(TAG, "%s", __func__);
    int ret = audio_recorder_trigger_start(instance);
    return ret;
}

esp_err_t recorder_action_rec_wav_turn_off(void *instance, action_arg_t *arg, action_result_t *result)
{
    ESP_LOGI(TAG, "%s", __func__);
    int ret = audio_recorder_trigger_stop(instance);
    return ret;
}

esp_err_t recorder_action_rec_amr_turn_on(void *instance, action_arg_t *arg, action_result_t *result)
{
    ESP_LOGI(TAG, "%s", __func__);
    return ESP_OK;
}
esp_err_t recorder_action_rec_amr_turn_off(void *instance, action_arg_t *arg, action_result_t *result)
{
    ESP_LOGI(TAG, "%s", __func__);
    return ESP_OK;
}
