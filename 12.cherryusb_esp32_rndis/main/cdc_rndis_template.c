#include "usbd_core.h"
#include "usbd_rndis.h"

#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_netif_net_stack.h"

#include "esp_bridge_internal.h"
#include "esp_bridge.h"
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

void usbd_rndis_data_recv_done(void)
{
}

static uint8_t usb_reset = 0;
static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event)
    {
    case USBD_EVENT_RESET:
        // usb_reset = 1;
        USB_LOG_RAW("USBD_EVENT_RESET\r\n");
        usb_reset++;
        break;
    case USBD_EVENT_CONNECTED:
        USB_LOG_RAW("USBD_EVENT_CONNECTED\r\n");

        break;
    case USBD_EVENT_DISCONNECTED:
        USB_LOG_RAW("USBD_EVENT_DISCONNECTED\r\n");

        break;
    case USBD_EVENT_RESUME:
        USB_LOG_RAW("USBD_EVENT_RESUME\r\n");

        break;
    case USBD_EVENT_SUSPEND:
        USB_LOG_RAW("USBD_EVENT_SUSPEND\r\n");

        break;
    case USBD_EVENT_CONFIGURED:
        USB_LOG_RAW("USBD_EVENT_CONFIGURED\r\n");

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

uint8_t recieve_ok = 0;

#include "esp_netif.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_wifi_netif.h"
#include "esp_private/wifi.h"

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
static void usb_driver_free_rx_buffer(void *h, void *buffer)
{
    if (buffer)
    {
        free(buffer);
    }
}
const esp_netif_driver_ifconfig_t usb_driver_ifconfig = {
    .driver_free_rx_buffer = usb_driver_free_rx_buffer,
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

const esp_netif_ip_info_t my_g_esp_netif_soft_ap_ip = {
    // .ip = {.addr = ESP_IP4TOADDR(189, 254, 1, 1)}, // 189.254.4.1
    // .gw = {.addr = ESP_IP4TOADDR(189, 254, 1, 1)},
    // .netmask = {.addr = ESP_IP4TOADDR(255, 255, 255, 0)},
    .ip = {.addr = ESP_IP4TOADDR(192, 168, 4, 1)}, // 189.254.4.1
    .gw = {.addr = ESP_IP4TOADDR(192, 168, 4, 1)},
    .netmask = {.addr = ESP_IP4TOADDR(255, 255, 0, 0)},
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
uint8_t mac_addr[6];
struct usbd_interface intf0;
struct usbd_interface intf1;

void cdc_rndis_init(uint8_t busid, uint32_t reg_base)
{
    esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
    usbd_desc_register(busid, cdc_descriptor);
    usbd_add_interface(busid, usbd_rndis_init_intf(&intf0, CDC_OUT_EP, CDC_IN_EP, CDC_INT_EP, mac_addr));
    usbd_add_interface(busid, usbd_rndis_init_intf(&intf1, CDC_OUT_EP, CDC_IN_EP, CDC_INT_EP, mac_addr));
    usbd_initialize(busid, reg_base, usbd_event_handler);
}
esp_err_t usbx_netif_init(esp_netif_t *netif)
{

    // cdc_rndis_init(0, 0x60080000);
    cnt++;
    usb_netif = netif;

    const esp_netif_inherent_config_t esp_netif_common_config = {
        .flags = (ESP_NETIF_DHCP_SERVER | ESP_NETIF_FLAG_GARP | ESP_NETIF_FLAG_EVENT_IP_MODIFIED),
        .get_ip_event = IP_EVENT_STA_GOT_IP,
        .lost_ip_event = IP_EVENT_STA_LOST_IP,
        .if_key = "USB_key",
        .if_desc = "USB_Netif"};

    usb_netif = esp_netif_create_wifi(WIFI_IF_STA, &esp_netif_common_config);
    esp_netif_attach(usb_netif, netsuite_io_new());
    //    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_WIFI_AP();
    // usb_netif = esp_netif_new(&usb_config);
    esp_netif_dhcps_stop(usb_netif);
    esp_netif_ip_info_t allocate_ip_info = {0};

    esp_bridge_netif_request_ip(&allocate_ip_info);
    esp_netif_set_ip_info(usb_netif, &allocate_ip_info);
    esp_netif_action_start(usb_netif, NULL, 0, NULL);
    esp_netif_up(usb_netif);

    esp_netif_dns_info_t dns;
    dns.ip.u_addr.ip4.addr = ESP_IP4TOADDR(114, 114, 114, 114);
    dns.ip.type = IPADDR_TYPE_V4;
    dhcps_offer_t dhcps_dns_value = OFFER_DNS;
    ESP_ERROR_CHECK(esp_netif_dhcps_option(usb_netif, ESP_NETIF_OP_SET, ESP_NETIF_DOMAIN_NAME_SERVER, &dhcps_dns_value, sizeof(dhcps_dns_value)));
    ESP_ERROR_CHECK(esp_netif_set_dns_info(usb_netif, ESP_NETIF_DNS_MAIN, &dns));
    ESP_ERROR_CHECK(esp_netif_dhcps_start(usb_netif));
    esp_netif_ip_info_t netif_ip_info = {0};

    esp_netif_get_ip_info(usb_netif, &netif_ip_info);
    ESP_LOGI(TAG, "USB IP Address:" IPSTR, IP2STR(&netif_ip_info.ip));
    ip_napt_enable(netif_ip_info.ip.addr, 1);

    uint8_t actual_mac[6] = {0};
    esp_netif_get_mac(usb_netif, actual_mac);
    for (size_t i = 0; i < 6; i++)
    {
        printf("%x ", actual_mac[i]);
    }
    // esp_bridge_create_usb_netif(NULL, NULL, true, true);
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &got_ip_event_handler, usb_netif));
    ESP_LOGI(TAG, "Start netif ok");
    xTaskCreate(
        usb_task,   /* Task function. */
        "usbxTask", /* name of task. */
        3 * 1024,   /* Stack size of task */
        usb_netif,  /* parameter of the task */
        8,          /* priority of the task */
        NULL);      /* Task handle to keep track of created task */
    return ESP_OK;
}

static esp_err_t netsuite_io_attach(esp_netif_t *esp_netif, void *arg)
{
    ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &usb_driver_ifconfig));
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
static void usb_task(void *arg)
{
    esp_netif_t *netif = (esp_netif_t *)arg;
    size_t hs;
    static struct pbuf *p;
    static uint8_t state = 0;

    while (1)
    {
        p = usbd_rndis_eth_rx();
        if (p != NULL)
        {
            if (usb_netif)
                esp_netif_receive(usb_netif, p->payload, p->len, NULL);
            pbuf_free(p);
        }
        else
        {
            vTaskDelay(1);
        }
    }
}
void check_usb_task(void *arg)
{
    static uint8_t state = 0;
    while (1)
    {
        switch (state)
        {
        case 0: // 开机初始化
            cdc_rndis_init(0, 0x60080000);
            state = 1;

            break;
        case 1: // 已经成功初始化USB 等待检测到拔出后重新初始化
            if (usb_reset == 4)
            {
                usbd_deinitialize(0);
                cdc_rndis_init(0, 0x60080000);
                usb_reset = 0;
            }
            break;
        default:
            break;
        }
        vTaskDelay(1);
    }
}

