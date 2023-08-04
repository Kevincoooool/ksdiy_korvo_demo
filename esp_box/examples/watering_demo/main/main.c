/*
* SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Unlicense OR CC0-1.0
*/

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <esp_heap_caps.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "bsp_board.h"
#include "bsp_storage.h"
#include "app_audio.h"
#include "app_sr.h"
#include "app_lvgl.h"
#include "app_pump.h"
#include "app_humidity.h"
#include "app_rmaker.h"

static const char *TAG = "main";

static void app_heap_info(void)
{
    printf("\tDescription\tInternal\tSPIRAM\n");
    printf("Current Free Memory\t%d\t\t%d\n",
           heap_caps_get_free_size(MALLOC_CAP_8BIT) - heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
           heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    printf("Largest Free Block\t%d\t\t%d\n",
           heap_caps_get_largest_free_block(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL),
           heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
    printf("Min. Ever Free Size\t%d\t\t%d\n",
           heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL),
           heap_caps_get_minimum_free_size(MALLOC_CAP_SPIRAM));
}

static void app_task_info(void)
{
    const size_t bytes_per_task = 40; /* see vTaskList description */
    char *task_list_buffer = malloc(uxTaskGetNumberOfTasks() * bytes_per_task);
    if (task_list_buffer == NULL) {
        ESP_LOGE(TAG, "failed to allocate buffer for vTaskList output");
    }
    fputs("Task Name\tStatus\tPrio\tHWM\tTask#", stdout);
#ifdef CONFIG_FREERTOS_VTASKLIST_INCLUDE_COREID
    fputs("\tAffinity", stdout);
#endif
    fputs("\n", stdout);
    vTaskList(task_list_buffer);
    fputs(task_list_buffer, stdout);
    free(task_list_buffer);
}

void app_debug(void)
{
    app_heap_info();
    printf("\n");
    app_task_info();
}

static esp_err_t app_storage_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    return err;
}

void app_main(void)
{
    ESP_ERROR_CHECK(app_storage_init());
    ESP_ERROR_CHECK(app_pump_init());
    ESP_ERROR_CHECK(app_humidity_init());

    ESP_ERROR_CHECK(bsp_board_init());
    ESP_ERROR_CHECK(bsp_board_power_ctrl(POWER_MODULE_AUDIO, true));
    ESP_ERROR_CHECK(bsp_spiffs_init("model", "/srmodel", 4));
    ESP_ERROR_CHECK(bsp_spiffs_init("storage", "/spiffs", 2));

    ESP_ERROR_CHECK(app_lvgl_start());
    ESP_ERROR_CHECK(app_audio_beep_init());
    ESP_ERROR_CHECK(app_sr_start(false));
    ESP_ERROR_CHECK(app_watering_rmaker_start());
}
