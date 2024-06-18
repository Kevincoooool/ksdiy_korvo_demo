/*
 * @Author: Kevincoooool
 * @Date: 2022-10-13 23:05:35
 * @Description:
 * @version:
 * @Filename: Do not Edit
 * @LastEditTime: 2024-06-13 12:08:18
 * @FilePath: \Firmware\22.cherryusb_cdc\main\usbx_cdc.h
 */
#ifndef USBX_CDC_H
#define USBX_CDC_H

#include "esp_netif.h"

#include "usbd_core.h"
#include "usbd_cdc.h"


void hid_keyboard_init1(uint8_t busid, uint32_t reg_base);
void hid_keyboard_test(uint8_t busid);

#endif
