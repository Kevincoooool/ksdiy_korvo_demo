/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
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

#ifndef __LED_BAR_WS2812_H__
#define __LED_BAR_WS2812_H__

#include "display_service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct led_bar_ws2812_impl *led_bar_ws2812_handle_t;

/**
 * @brief Initialize led_bar_ws2812_handle_t instance
 *
 * @param gpio_num    The GPIO number of ws2812
 * @param led_num     The number of all ws2812
 *
 * @return
 *     - led_bar_ws2812_handle_t
 */
led_bar_ws2812_handle_t led_bar_ws2812_init(gpio_num_t gpio_num, int led_num);

/**
 * @brief Set ws2812 pattern
 *
 * @param      handle   ws2812 indicator instance
 * @param      pat      display pattern
 * @param      value    value of pattern
 *
 * @return
 *     - ESP_OK, success
 *     - Others, fail

 */
esp_err_t led_bar_ws2812_pattern(void *handle, int pat, int value);

/**
 * @brief Destroy led_bar_ws2812_handle_t instance
 *
 * @param      handle   ws2812 indicator instance
 *
 * @return
 * @return
 *     - ESP_OK, success
 *     - Others, fail
 */
esp_err_t led_bar_ws2812_deinit(led_bar_ws2812_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif
