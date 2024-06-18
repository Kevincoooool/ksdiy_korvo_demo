/*
 * @Author: Kevincoooool
 * @Date: 2024-06-13 11:44:48
 * @Description: 
 * @version:  
 * @Filename: Do not Edit
 * @LastEditTime: 2024-06-13 13:33:04
 * @FilePath: \Firmware\26.cherryusb_hid_mouse\main\app_main.c
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
#include "usbx_demo.h"

void app_main(void)
{
    printf("Hello world!\n");

    hid_mouse_init1(0, 0x60080000);

    // usbh_initialize(0, 0x60080000);
    while(1)
    {
        // extern void cdc_acm_data_send_with_dtr_test(uint8_t busid);
        // cdc_acm_data_send_with_dtr_test(0);
        vTaskDelay(10);
    }
}
