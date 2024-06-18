#ifndef USBX_NET_H
#define USBX_NET_H

#include "esp_netif.h"

//tinyusb network interface callbacks
bool tud_network_recv_cb(const uint8_t *src, uint16_t size);
uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg);
void tud_network_init_cb(void);

esp_err_t usbx_netif_init(esp_netif_t *netif);

#endif
