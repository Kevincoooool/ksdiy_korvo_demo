
#include "usbx_cdc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

/*!< endpoint address */

#define USBD_VID 0xAABB
#define USBD_PID 0xCCDD
#define USBD_MAX_POWER 100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE (9 + CDC_ACM_DESCRIPTOR_LEN)

#define CDC_MAX_MPS 64

/*!< global descriptor */
static const uint8_t cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02),
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x32,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'C', 0x00,                  /* wcChar10 */
    'D', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    '_', 0x00,                  /* wcChar10 */
    'K', 0x00,                  /* wcChar11 */
    'S', 0x00,                  /* wcChar12 */
    'D', 0x00,                  /* wcChar13 */
    'I', 0x00,                  /* wcChar14 */
    'Y', 0x00,                  /* wcChar15 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x02,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
#endif
    0x00};

static char *TAG = "device_cdc_main";
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[2 * 1024];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t write_buffer[2 * 1024];
static int write_buffer_len = 0;

volatile bool ep_tx_busy_flag = false;
static uint8_t usb_reset = 0;
static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event)
    {
    case USBD_EVENT_RESET:
        esp_rom_printf("USBD_EVENT_RESET\r\n");
        usb_reset++;
        if (usb_reset == 2)
        {
        }

        break;
    case USBD_EVENT_CONNECTED:
        esp_rom_printf("USBD_EVENT_CONNECTED\r\n");

        break;
    case USBD_EVENT_DISCONNECTED:
        esp_rom_printf("USBD_EVENT_DISCONNECTED\r\n");

        break;
    case USBD_EVENT_RESUME:
        esp_rom_printf("USBD_EVENT_RESUME\r\n");

        break;
    case USBD_EVENT_SUSPEND:
        break;
    case USBD_EVENT_CONFIGURED:
        ep_tx_busy_flag = false;
        /* setup first out ep read transfer */
        usbd_ep_start_read(busid, CDC_OUT_EP, read_buffer, sizeof(read_buffer));
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

void usbd_cdc_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    // USB_LOG_RAW("actual out len:%d\r\n", nbytes);
    esp_rom_printf("actual out len:%d\r\n", nbytes);
    // for (int i = 0; i < nbytes; i++)
    // {
    //     esp_rom_printf("%02x ", read_buffer[i]);
    // }

    usbd_ep_start_read(busid, CDC_OUT_EP, read_buffer, sizeof(read_buffer));

    // for (int i = 0; i < nbytes; i++) {
    //     esp_rom_printf("%02x ", read_buffer[i]);
    // }
    usbd_ep_start_write(busid, CDC_IN_EP, read_buffer, nbytes);
}

void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    // USB_LOG_RAW("actual in len:%d\r\n", nbytes);
    esp_rom_printf("actual in len:%d\r\n", nbytes);

    if ((nbytes % CDC_MAX_MPS) == 0 && nbytes)
    {
        /* send zlp */
        usbd_ep_start_write(busid, CDC_IN_EP, NULL, 0);
    }
    else
    {
        ep_tx_busy_flag = false;
    }
}

/*!< endpoint call back */
struct usbd_endpoint cdc_out_ep = {
    .ep_addr = CDC_OUT_EP,
    .ep_cb = usbd_cdc_acm_bulk_out};

struct usbd_endpoint cdc_in_ep = {
    .ep_addr = CDC_IN_EP,
    .ep_cb = usbd_cdc_acm_bulk_in};

static struct usbd_interface intf0;
static struct usbd_interface intf1;

void cdc_acm_init1(uint8_t busid, uint32_t reg_base)
{
    const uint8_t data[10] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30};

    memcpy(&write_buffer[0], data, 10);
    memset(&write_buffer[10], 'a', 2038);

    usbd_desc_register(busid, cdc_descriptor);

    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf0));

    usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &intf1));

    usbd_add_endpoint(busid, &cdc_out_ep);

    usbd_add_endpoint(busid, &cdc_in_ep);

    usbd_initialize(busid, reg_base, usbd_event_handler);
}

volatile uint8_t dtr_enable = 0;

void usbd_cdc_acm_set_dtr(uint8_t busid, uint8_t intf, bool dtr)
{
    if (dtr)
    {
        dtr_enable = 1;
    }
    else
    {
        dtr_enable = 0;
    }
}

void cdc_acm_data_send_with_dtr_test(uint8_t busid)
{
    if (dtr_enable)
    {
        ep_tx_busy_flag = true;
        usbd_ep_start_write(busid, CDC_IN_EP, write_buffer, 2048);
        while (ep_tx_busy_flag)
        {
        }
    }
}
// void app_main()
// {
//     // Initialize the USB driver and CDC interface
//     cdc_acm_init();
//     ESP_LOGI(TAG, "usb cdc acm init done");

//     while (1) {
//         // Send back the data received from the host
//         cdc_acm_data_send_with_dtr_test();
//         vTaskDelay(1);
//     }
// }