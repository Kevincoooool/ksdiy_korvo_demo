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

#ifndef _PARTITION_LIST_H_
#define _PARTITION_LIST_H_

#include "playlist.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a playlist in flash partition by list id
 *
 * @note  Please add 2 partitions to partition table whose subtype are 0x06 and 0x07 first
 * 
 * @param[out] handle   The playlist handle from application layer
 *
 * @return 
 *     - ESP_OK   success
 *     - ESP_FAIL failed
 */
esp_err_t partition_list_create(playlist_operator_handle_t *handle);

/**
 * @brief Save URL to partition playlist
 *
 * @param handle     Playlist handle
 * @param url        URL to be saved
 *
 * @return 
 *     - ESP_OK   success
 *     - ESP_FAIL failed
 */
esp_err_t partition_list_save(playlist_operator_handle_t handle, const char *url);

/**
 * @brief The following URLs in partition playlist
 *
 * @param      handle          Playlist handle
 * @param      step            The offset of URL from current URL
 * @param[out] url_buff        A second rank pointer to get a address of URL
 *
 * @return 
 *     - ESP_OK   success
 *     - ESP_FAIL failed
 */
esp_err_t partition_list_next(playlist_operator_handle_t handle, int step, char **url_buff);

/**
 * @brief The previous URLs in partition playlist
 *
 * @param      handle         Playlist handle
 * @param      step           The offset of URL from current URL
 * @param[out] url_buff       A second rank pointer to get a address of URL
 *
 * @return 
 *     - ESP_OK   success
 *     - ESP_FAIL failed
 */
esp_err_t partition_list_prev(playlist_operator_handle_t handle, int step, char **url_buff);

/**
 * @brief Judge whether the url exists in partition playlist
 *
 * @param handle   Playlist handle
 * @param url      The url to be checked
 *
 * @return 
 *     - true    existence
 *     - false   Non-existent
 */
bool partition_list_exist(playlist_operator_handle_t handle, const char *url);

/**
 * @brief Reset partition playlist
 *
 * @param handle   Playlist handle
 *
 * @return 
 *     - ESP_OK   success
 *     - ESP_FAIL failed
 */
esp_err_t partition_list_reset(playlist_operator_handle_t handle);

/**
 * @brief Get current URL in the partition playlist
 *
 * @param      handle         Playlist handle
 * @param[out] url_buff       A second rank pointer to get a address of URL
 *
 * @return 
 *     - ESP_OK   success
 *     - ESP_FAIL failed
 */
esp_err_t partition_list_current(playlist_operator_handle_t handle, char **url_buff);

/**
 * @brief Choose a url by url id
 *
 * @param      handle          Playlist handle
 * @param      url_id          The id of url in partition list
 * @param[out] url_buff        A second rank pointer to get a address of URL
 *
 * @return 
 *     - ESP_OK   success
 *     - ESP_FAIL failed
 */
esp_err_t partition_list_choose(playlist_operator_handle_t handle, int url_id, char **url_buff);

/**
 * @brief Get URLs number in the partition playlist
 *
 * @param handle        Playlist handle
 *
 * @return 
 *     - URLs number in partition playlist
 *     - ESP_FAIL     Fail to get number of urls
 */
int partition_list_get_url_num(playlist_operator_handle_t handle);

/**
 * @brief Get curernt url id in the partition playlist
 *
 * @param handle        Playlist handle
 *
 * @return 
 *     - Current url id in partition playlist
 *     - ESP_FAIL     Fail to get url id
 */
int partition_list_get_url_id(playlist_operator_handle_t handle);

/**
 * @brief Show all the URLs in the partition playlist
 *
 * @param handle     Playlist handle
 *
 * @return 
 *     - ESP_OK   success
 *     - ESP_FAIL failed
 */
esp_err_t partition_list_show(playlist_operator_handle_t handle);

/**
 * @brief Destroy the partition playlist
 *
 * @param handle     Playlist handle
 *
 * @return 
 *     - ESP_OK   success
 *     - ESP_FAIL failed
 */
esp_err_t partition_list_destroy(playlist_operator_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif
