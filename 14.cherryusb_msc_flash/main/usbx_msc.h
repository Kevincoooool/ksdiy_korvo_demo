/*
 * @Author: Kevincoooool
 * @Date: 2022-10-13 23:05:35
 * @Description:
 * @version:
 * @Filename: Do not Edit
 * @LastEditTime: 2024-06-13 13:00:52
 * @FilePath: \Firmware\24.cherryusb_msc_flash\main\usbx_msc.h
 */
#ifndef USBX_MSC_H
#define USBX_MSC_H

#include "esp_netif.h"

#include "usbd_core.h"
#include "usbd_cdc.h"


void msc_flash_init1(uint8_t busid, uint32_t reg_base);

#endif
