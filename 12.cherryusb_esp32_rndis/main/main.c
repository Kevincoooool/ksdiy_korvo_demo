/*
 * @Author       : 陈科进
 * @Date         : 2023-05-18 13:27:40
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2024-06-19 08:52:00
 * @FilePath: \ksdiy_korvo_demo\12.cherryusb_esp32_rndis\main\main.c
 * @Description  : ESP32 candlelight firmware object
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "hal/usb_hal.h"
#include "soc/usb_periph.h"
#include "driver/gpio.h"
#include "driver/periph_ctrl.h"
#include <stdio.h>
#include "lwip/pbuf.h"
#include "lwip/lwip_napt.h"
// #include "esp_wifi_netif.h"
#include "rndis_protocol.h"
#include "esp_netif.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
// #include "esp_private/wifi.h"
#include "usbx_net.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "app_wifi.h"

static void ConfigureUsbPins(usb_hal_context_t *usb)
{
    /* usb_periph_iopins currently configures USB_OTG as USB Device.
     * Introduce additional parameters in usb_hal_context_t when adding support
     * for USB Host.
     */
    for (const usb_iopin_dsc_t *iopin = usb_periph_iopins; iopin->pin != -1; ++iopin)
    {
        if ((usb->use_external_phy) || (iopin->ext_phy_only == 0))
        {
            gpio_pad_select_gpio(iopin->pin);
            if (iopin->is_output)
            {
                gpio_matrix_out(iopin->pin, iopin->func, false, false);
            }
            else
            {
                gpio_matrix_in(iopin->pin, iopin->func, false);
                gpio_pad_input_enable(iopin->pin);
            }
            gpio_pad_unhold(iopin->pin);
        }
    }
    if (!usb->use_external_phy)
    {
        gpio_set_drive_capability(USBPHY_DM_NUM, GPIO_DRIVE_CAP_3);
        gpio_set_drive_capability(USBPHY_DP_NUM, GPIO_DRIVE_CAP_3);
    }
}
extern void check_usb_task(void *arg);
void app_main()
{
    // extern void cdc_rndis_init();

    extern void cdc_rndis_init(uint8_t busid, uint32_t reg_base);

    periph_module_reset(PERIPH_USB_MODULE);
    periph_module_enable(PERIPH_USB_MODULE);

    usb_hal_context_t hal = {
        .use_external_phy = false};
    usb_hal_init(&hal);
    ConfigureUsbPins(&hal);

    printf("Hello cherry!\n");
    // cdc_acm_init00();
    static esp_netif_t *usb_netif;
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_LOGI("TAG", "esp_netif init success.");
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    app_wifi_init();
    ESP_ERROR_CHECK(usbx_netif_init(usb_netif));
    ESP_LOGI("TAG", "usbx_netif init success.");

    // cdc_rndis_init(0, 0x60080000);
    xTaskCreatePinnedToCore(check_usb_task, "check_usb_task", 4096, NULL, 8, NULL, 1);

    while (1)
    {
        // extern void cdc_acm_data_send_with_dtr_test();

        // cdc_acm_data_send_with_dtr_test();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}