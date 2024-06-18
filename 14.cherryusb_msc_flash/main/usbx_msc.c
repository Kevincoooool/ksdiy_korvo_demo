/*
 * Copyright (c) 2024, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "usbd_core.h"
#include "usbd_msc.h"
#include "ffconf.h"
#include "ff.h"
#include "diskio.h"
#include "esp_log.h"

#define MSC_IN_EP  0x81
#define MSC_OUT_EP 0x02

#define USBD_VID           0xFFFF
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

#define USB_CONFIG_SIZE (9 + MSC_DESCRIPTOR_LEN)

#ifdef CONFIG_USB_HS
#define MSC_MAX_MPS 512
#else
#define MSC_MAX_MPS 64
#endif

const uint8_t msc_ram_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    MSC_DESCRIPTOR_INIT(0x00, MSC_OUT_EP, MSC_IN_EP, MSC_MAX_MPS, 0x02),
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
    0x26,                       /* bLength */
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
    'M', 0x00,                  /* wcChar10 */
    'S', 0x00,                  /* wcChar11 */
    'C', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
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
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
#endif
    0x00
};

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            break;
        case USBD_EVENT_RESUME:
            break;
        case USBD_EVENT_SUSPEND:
            break;
        case USBD_EVENT_CONFIGURED:
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

// #define BLOCK_SIZE  512
// #define BLOCK_COUNT 10

// typedef struct
// {
//     uint8_t BlockSpace[BLOCK_SIZE];
// } BLOCK_TYPE;

// BLOCK_TYPE mass_block[BLOCK_COUNT];

// void usbd_msc_get_cap(uint8_t busid, uint8_t lun, uint32_t *block_num, uint32_t *block_size)
// {
//     *block_num = 1000; //Pretend having so many buffer,not has actually.
//     *block_size = BLOCK_SIZE;
// }
// int usbd_msc_sector_read(uint8_t busid, uint8_t lun, uint32_t sector, uint8_t *buffer, uint32_t length)
// {
//     if (sector < BLOCK_COUNT)
//         memcpy(buffer, mass_block[sector].BlockSpace, length);
//     return 0;
// }

// int usbd_msc_sector_write(uint8_t busid, uint8_t lun, uint32_t sector, uint8_t *buffer, uint32_t length)
// {
//     if (sector < BLOCK_COUNT)
//         memcpy(mass_block[sector].BlockSpace, buffer, length);
//     return 0;
// }
#define BLOCK_SIZE 512
#define BLOCK_COUNT 10
#include "esp_ota_ops.h"
#include "esp_partition.h"
#define OP_READ 0
#define OP_WRITE 1

// TODO: Split the caching out of supervisor/shared/external_flash so we can use it.
#define SECTOR_SIZE 4096
#define FILESYSTEM_BLOCK_SIZE 4096
static uint8_t _cache[SECTOR_SIZE];
static uint32_t _cache_lba = 0xffffffff;

#if CIRCUITPY_STORAGE_EXTEND
#if FF_MAX_SS == FF_MIN_SS
#define SECSIZE(fs) (FF_MIN_SS)
#else
#define SECSIZE(fs) ((fs)->ssize)
#endif // FF_MAX_SS == FF_MIN_SS
static DWORD fatfs_bytes(void)
{
    FATFS *fatfs = filesystem_circuitpy();
    return (fatfs->csize * SECSIZE(fatfs)) * (fatfs->n_fatent - 2);
}
static bool storage_extended = true;
static const esp_partition_t *_partition[2];
#else
static const esp_partition_t *_partition[1];
#endif // CIRCUITPY_STORAGE_EXTEND

void supervisor_flash_init(void)
{
    if (_partition[0] != NULL)
    {
        return;
    }
    _partition[0] = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                             ESP_PARTITION_SUBTYPE_DATA_FAT,
                                             NULL);
#if CIRCUITPY_STORAGE_EXTEND
    _partition[1] = esp_ota_get_next_update_partition(NULL);
#endif
}
uint32_t supervisor_flash_get_block_size(void)
{
    return FILESYSTEM_BLOCK_SIZE;
}

uint32_t supervisor_flash_get_block_count(void)
{
#if CIRCUITPY_STORAGE_EXTEND
    return ((storage_extended) ? (_partition[0]->size + _partition[1]->size) : _partition[0]->size) / FILESYSTEM_BLOCK_SIZE;
#else
    return _partition[0]->size / FILESYSTEM_BLOCK_SIZE;
#endif
}

void port_internal_flash_flush(void)
{
}

static void single_partition_rw(const esp_partition_t *partition, uint8_t *data,
                                const uint32_t offset, const uint32_t size_total, const bool op)
{
    if (op == OP_READ)
    {
        esp_partition_read(partition, offset, data, size_total);
    }
    else
    {
        esp_partition_erase_range(partition, offset, size_total);
        esp_partition_write(partition, offset, _cache, size_total);
    }
}

#if CIRCUITPY_STORAGE_EXTEND
static void multi_partition_rw(uint8_t *data,
                               const uint32_t offset, const uint32_t size_total, const bool op)
{
    if (offset > _partition[0]->size)
    {
        // only r/w partition 1
        single_partition_rw(_partition[1], data, (offset - _partition[0]->size), size_total, op);
    }
    else if ((offset + size_total) > _partition[0]->size)
    {
        // first r/w partition 0, then partition 1
        uint32_t size_0 = _partition[0]->size - offset;
        uint32_t size_1 = size_total - size_0;
        if (op == OP_READ)
        {
            esp_partition_read(_partition[0], offset, data, size_0);
            esp_partition_read(_partition[1], 0, (data + size_0), size_1);
        }
        else
        {
            esp_partition_erase_range(_partition[0], offset, size_0);
            esp_partition_write(_partition[0], offset, _cache, size_0);
            esp_partition_erase_range(_partition[1], 0, size_1);
            esp_partition_write(_partition[1], 0, (_cache + size_0), size_1);
        }
    }
    else
    {
        // only r/w partition 0
        single_partition_rw(_partition[0], data, offset, size_total, op);
    }
}
#endif

uint8_t supervisor_flash_read_blocks(uint8_t *dest, uint32_t block, uint32_t num_blocks)
{
    const uint32_t offset = block * FILESYSTEM_BLOCK_SIZE;
    const uint32_t read_total = num_blocks * FILESYSTEM_BLOCK_SIZE;
#if CIRCUITPY_STORAGE_EXTEND
    multi_partition_rw(dest, offset, read_total, OP_READ);
#else
    single_partition_rw(_partition[0], dest, offset, read_total, OP_READ);
#endif
    return 0; // success
}

uint8_t supervisor_flash_write_blocks(const uint8_t *src, uint32_t lba, uint32_t num_blocks)
{
    uint32_t blocks_per_sector = SECTOR_SIZE / FILESYSTEM_BLOCK_SIZE;
    uint32_t block = 0;
    while (block < num_blocks)
    {
        uint32_t block_address = lba + block;
        uint32_t sector_offset = block_address / blocks_per_sector * SECTOR_SIZE;
        uint8_t block_offset = block_address % blocks_per_sector;
        if (_cache_lba != block_address)
        {
            supervisor_flash_read_blocks(_cache, sector_offset / FILESYSTEM_BLOCK_SIZE, blocks_per_sector);
            _cache_lba = sector_offset;
        }
        for (uint8_t b = block_offset; b < blocks_per_sector; b++)
        {
            // Stop copying after the last block.
            if (block >= num_blocks)
            {
                break;
            }
            memcpy(_cache + b * FILESYSTEM_BLOCK_SIZE,
                   src + block * FILESYSTEM_BLOCK_SIZE,
                   FILESYSTEM_BLOCK_SIZE);
            block++;
        }
#if CIRCUITPY_STORAGE_EXTEND
        multi_partition_rw(_cache, sector_offset, SECTOR_SIZE, OP_WRITE);
#else
        single_partition_rw(_partition[0], _cache, sector_offset, SECTOR_SIZE, OP_READ);
#endif
    }
    return 0; // success
}

void supervisor_flash_release_cache(void)
{
}

void supervisor_flash_set_extended(bool extended)
{
#if CIRCUITPY_STORAGE_EXTEND
    storage_extended = extended;
#endif
}

bool supervisor_flash_get_extended(void)
{
#if CIRCUITPY_STORAGE_EXTEND
    return storage_extended;
#else
    return false;
#endif
}

void supervisor_flash_update_extended(void)
{
#if CIRCUITPY_STORAGE_EXTEND
    storage_extended = (_partition[0]->size < fatfs_bytes());
#endif
}
typedef struct
{
    uint8_t BlockSpace[BLOCK_SIZE];
} BLOCK_TYPE;

BLOCK_TYPE mass_block[BLOCK_COUNT];
static uint8_t s_pdrv = 0;
void usbd_msc_get_cap(uint8_t busid, uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{

    *block_num = supervisor_flash_get_block_count(); // Pretend having so many buffer,not has actually.
    *block_size = supervisor_flash_get_block_size();

    ESP_LOGI(__func__, "lun = %u GET_SECTOR_COUNT = %d，GET_SECTOR_SIZE = %d", lun, *block_num, *block_size);
}
int usbd_msc_sector_read(uint8_t busid, uint8_t lun, uint32_t sector, uint8_t *buffer, uint32_t length)
{

    disk_read(s_pdrv, buffer, sector, 1);
    return 0;
}

int usbd_msc_sector_write(uint8_t busid, uint8_t lun,uint32_t sector, uint8_t *buffer, uint32_t length)
{

    disk_write(s_pdrv, buffer, sector, 1);
    return 0;
}
struct usbd_interface intf0;

void msc_flash_init1(uint8_t busid, uint32_t reg_base)
{
    supervisor_flash_init();
    usbd_desc_register(busid, msc_ram_descriptor);
    usbd_add_interface(busid, usbd_msc_init_intf(busid, &intf0, MSC_OUT_EP, MSC_IN_EP));

    usbd_initialize(busid, reg_base, usbd_event_handler);
}
