/*
 * @Author: Kevincoooool
 * @Date: 2024-06-13 11:44:48
 * @Description: 
 * @version:  
 * @Filename: Do not Edit
 * @LastEditTime: 2024-06-13 12:20:24
 * @FilePath: \Firmware\22.cherryusb_cdc\main\app_main.c
 */
/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "usbd_core.h"
#include "usbh_core.h"
#include "usbx_msc.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
void app_main(void)
{
    printf("Hello world!\n");
    const char *base_path = "/S";
    static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

    const esp_vfs_fat_mount_config_t mount_config = {
        .max_files = 4,
        .format_if_mount_failed = true,
        .allocation_unit_size = 4096};
    esp_err_t err = esp_vfs_fat_spiflash_mount(base_path, "storage", &mount_config, &s_wl_handle); // esp_vfs_fat_spiflash_mount_rw_wl esp_vfs_fat_spiflash_mount
    if (err != ESP_OK)
    {
        ESP_LOGE("TAG", "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }
    msc_flash_init1(0, 0x60080000);

    // usbh_initialize(0, 0x60080000);
    while(1)
    {
        // extern void cdc_acm_data_send_with_dtr_test(uint8_t busid);
        // cdc_acm_data_send_with_dtr_test(0);
        vTaskDelay(10);
    }
}
