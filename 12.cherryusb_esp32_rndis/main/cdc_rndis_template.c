#include "usbd_core.h"
#include "usbd_rndis.h"

/*!< endpoint address */
#define CDC_IN_EP 0x81
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x83

#define USBD_VID 0xEFFF
#define USBD_PID 0xEFFF
#define USBD_MAX_POWER 100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE (9 + CDC_RNDIS_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define CDC_MAX_MPS 512
#else
#define CDC_MAX_MPS 64
#endif

/*!< global descriptor */
static const uint8_t cdc_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_RNDIS_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02),
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
    0x2A,                       /* bLength */
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
    'R', 0x00,                  /* wcChar10 */
    'N', 0x00,                  /* wcChar11 */
    'D', 0x00,                  /* wcChar12 */
    'I', 0x00,                  /* wcChar13 */
    'S', 0x00,                  /* wcChar14 */
    ' ', 0x00,                  /* wcChar15 */
    'D', 0x00,                  /* wcChar16 */
    'E', 0x00,                  /* wcChar17 */
    'M', 0x00,                  /* wcChar18 */
    'O', 0x00,                  /* wcChar19 */
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
const uint8_t tud_network_mac_address[6] = {0x02, 0x02, 0x84, 0x6A, 0x96, 0x00};

const uint8_t mac[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
uint8_t recieve_ok = 0;

#include "esp_netif.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_wifi_netif.h"
#include "esp_private/wifi.h"
static uint8_t usb_reset = 0;

static uint8_t tud_rx_buf[4500];
static size_t tud_rx_len = 0;
static const char *TAG = "usbx_net";

#include "netif/etharp.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"


static esp_netif_t *usb_netif;

static void usb_task(void *arg);
static esp_err_t netsuite_io_transmit(void *h, void *buffer, size_t len);
static esp_err_t netsuite_io_transmit_wrap(void *h, void *buffer, size_t len, void *netstack_buf);
static esp_err_t netsuite_io_attach(esp_netif_t *esp_netif, void *arg);
// static httpd_handle_t server = NULL;
// TODO: implement rx here?
const esp_netif_driver_ifconfig_t driver_ifconfig = {
    .driver_free_rx_buffer = NULL,
    .transmit = netsuite_io_transmit,
    .transmit_wrap = netsuite_io_transmit_wrap,
    .handle = "usbx_net-netsuite-io-object" // this IO object is a singleton, its handle uses as a name
};
const esp_netif_driver_base_t s_driver_base = {
    .post_attach = netsuite_io_attach};

void *netsuite_io_new(void)
{
    return (void *)&s_driver_base;
}
const esp_netif_ip_info_t netif_subnet_ip = {
    // mesh subnet IP info
    .ip = {.addr = ESP_IP4TOADDR(189, 254, 1, 1)}, // 189.254.4.1
    .gw = {.addr = ESP_IP4TOADDR(189, 254, 1, 1)},
    .netmask = {.addr = ESP_IP4TOADDR(255, 255, 255, 0)},
};
const esp_netif_ip_info_t my_g_esp_netif_soft_ap_ip = {
    .ip = {.addr = ESP_IP4TOADDR(189, 254, 1, 1)}, // 189.254.4.1
    .gw = {.addr = ESP_IP4TOADDR(189, 254, 1, 1)},
    .netmask = {.addr = ESP_IP4TOADDR(255, 255, 255, 0)},
};

/** Event handler for IP_EVENT_ETH_GOT_IP */
static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGW(TAG, "USB PC interface Got IP Address");
}
static uint8_t cnt = 0;
static char cnt_buf[10] = {0};

esp_err_t usbx_netif_init(esp_netif_t *netif)
{
    cnt++;
    usb_netif = netif;

    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_AP();
    esp_netif_config.ip_info = &my_g_esp_netif_soft_ap_ip;
    esp_netif_config.if_desc = "mywifi";

    sprintf(cnt_buf, "test%d", cnt);
    esp_netif_config.if_key = cnt_buf;
    esp_netif_config.route_prio = 10;

    netif = esp_netif_create_wifi(WIFI_IF_AP, &esp_netif_config);

    esp_netif_ip_info_t ipInfo;
    IP4_ADDR(&ipInfo.ip, 189, 254, 1, 1);        // 设置 IP 地址
    IP4_ADDR(&ipInfo.gw, 189, 254, 1, 1);        // 设置网关
    IP4_ADDR(&ipInfo.netmask, 255, 255, 255, 0); // 设置子网掩码
    esp_netif_set_ip_info(netif, &ipInfo);
    // esp_netif_set_mac(netif, tud_network_mac_address);
    assert(netif);

    esp_netif_attach(netif, netsuite_io_new());
    // esp_netif_set_ip_info(netif, &ipInfo);
    uint8_t actual_mac[6] = {0};
    esp_netif_get_mac(netif, actual_mac);
    for (size_t i = 0; i < 6; i++)
    {
        printf("%x", actual_mac[i]);
    }
    esp_netif_action_start(netif, NULL, 0, NULL);
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &got_ip_event_handler, netif));
    // esp_netif_set_ip_info(netif, &ipInfo);

    return ESP_OK;
}

void usbd_event_handler(uint8_t event)
{
    switch (event)
    {
    case USBD_EVENT_RESET:
        CONFIG_USB_PRINTF("connect USBD_EVENT_RESET\n");
        usb_reset = 1;

        break;
    case USBD_EVENT_CONNECTED:
        CONFIG_USB_PRINTF("connect USBD_EVENT_CONNECTED\n");
        break;
    case USBD_EVENT_DISCONNECTED:
        CONFIG_USB_PRINTF("disconnect USBD_EVENT_DISCONNECTED\n");

        break;
    case USBD_EVENT_RESUME:
        CONFIG_USB_PRINTF("USBD_EVENT_RESUME \n");
        break;
    case USBD_EVENT_SUSPEND:
        CONFIG_USB_PRINTF("USBD_EVENT_SUSPEND \n");
        break;
    case USBD_EVENT_CONFIGURED:
#ifdef RT_USING_LWIP
        eth_device_linkchange(&rndis_dev, RT_TRUE);
#endif
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

struct usbd_interface intf0;
struct usbd_interface intf1;

void cdc_rndis_init(void)
{

    usbd_desc_register(cdc_descriptor);
    usbd_add_interface(usbd_rndis_init_intf(&intf0, CDC_OUT_EP, CDC_IN_EP, CDC_INT_EP, tud_network_mac_address));
    usbd_add_interface(usbd_rndis_init_intf(&intf1, CDC_OUT_EP, CDC_IN_EP, CDC_INT_EP, tud_network_mac_address));
    usbd_initialize();
}

static esp_err_t netsuite_io_attach(esp_netif_t *esp_netif, void *arg)
{
    ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &driver_ifconfig));
    return ESP_OK;
}

static esp_err_t netsuite_io_transmit_wrap(void *h, void *buffer, size_t len, void *netstack_buf)
{
    return netsuite_io_transmit(h, buffer, len);
}

static esp_err_t netsuite_io_transmit(void *h, void *buffer, size_t len)
{
    static err_t err;

    for (;;)
    {
        struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
        if (p != NULL)
        {
            memcpy(p->payload, buffer, len);
            usbd_rndis_eth_tx(p);
            pbuf_free(p); // crucial to prevent memleak
        }
        return ESP_OK;

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void check_usb_task(void *arg)
{
    static struct pbuf *p;
    while (1)
    {
        if (usb_reset == 1)
        {
            usbd_deinitialize();

            if (usb_netif)
            {
                // esp_netif_destroy_default_wifi(usb_netif);
                esp_netif_destroy(usb_netif);
                usb_netif = NULL;
            }
            usbx_netif_init(usb_netif);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            usb_reset = 0;
        }
        else
        {
            p = usbd_rndis_eth_rx();
            if (p != NULL)
            {
                if (usb_netif)
                {
                    esp_netif_receive(usb_netif, p->payload, p->len, NULL);
                }
                pbuf_free(p);
            }
        }
        vTaskDelay(1);
    }
}
