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

#define CDC_IN_EP 0x81
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x83
extern uint8_t pc_msg_busy;
extern USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[2 * 1024];
extern USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[2 * 1024];
void cdc_acm_msc_init1(uint8_t busid, uint32_t reg_base);

#endif
