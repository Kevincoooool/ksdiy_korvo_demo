/*
* Copyright © 2021 Sturnus Inc.

* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the “Software”), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
* to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include <esp_log.h>
// #ifdef LV_LVGL_H_INCLUDE_SIMPLE
// #include <lvgl.h>
// #else
// #include <lvgl/lvgl.h>
// #endif
#include "gt911.h"
#include "i2c_bus.h"
#include "esp_err.h"
#include "bsp_i2c.h"

#define TAG "GT911"

gt911_status_t gt911_status;
static i2c_bus_device_handle_t gt911_handle = NULL;

static uint8_t gt911_addr = 0;

esp_err_t gt911_read_byte(uint16_t reg_addr, uint8_t *data)
{

    esp_err_t ret = i2c_bus_read_reg16(gt911_handle, reg_addr, 2, data);
    if (ret != ESP_OK)
    {
        ret = i2c_bus_read_reg16(gt911_handle, reg_addr, 2, data);
    }
    return ret;
}
esp_err_t gt911_read_bytes(uint16_t reg_addr, size_t data_len, uint8_t *data)
{

    return i2c_bus_read_reg16(gt911_handle, reg_addr, data_len, data);
}

esp_err_t gt911_write_byte(uint16_t reg_addr, uint8_t data)
{

    esp_err_t ret = i2c_bus_write_reg16(gt911_handle, reg_addr, 2, &data);
    if (ret != ESP_OK)
    {
        ret = i2c_bus_write_reg16(gt911_handle, reg_addr, 2, &data);
    }
    return ret;
}

/**
 * @brief  Initialize for GT911 communication via I2C
 * @param  dev_addr: Device address on communication Bus (I2C slave address of GT911).
 * @retval None
 */
esp_err_t gt911_init(void)
{
    if (NULL != gt911_handle)
    {
        return ESP_ERR_INVALID_STATE;
    }

    bsp_i2c_add_device(&gt911_handle, GT911_I2C_SLAVE_ADDR);
    if (NULL == gt911_handle)
    {
        return ESP_FAIL;
    }
    // gt911_addr = 0x5d << 1;
    uint8_t data_buf;

    // i2c_config_t i2c_cfg = {
    //     .mode = I2C_MODE_MASTER,
    //     .sda_pullup_en = GPIO_PULLUP_ENABLE,
    //     .scl_pullup_en = GPIO_PULLUP_ENABLE,
    //     .master.clk_speed = 100000,
    // };
    //  i2c_config_t es_i2c_cfg = {
    //     .mode = I2C_MODE_MASTER,
    //     .sda_pullup_en = GPIO_PULLUP_ENABLE,
    //     .scl_pullup_en = GPIO_PULLUP_ENABLE,
    //     .master.clk_speed = 600000,
    //     .sda_io_num = GPIO_NUM_17,
    //     .scl_io_num = GPIO_NUM_18,
    // };
    // gt911_handle = i2c_bus_create(I2C_NUM_0, &es_i2c_cfg);
    // uint8_t address;
    // vTaskDelay(1000);
    // printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");
    // for (int i = 0; i < 128; i += 16)
    // {
    //     printf("%02x: ", i);
    //     for (int j = 0; j < 16; j++)
    //     {
    //         fflush(stdout);
    //         address = i + j;
    //         i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    //         i2c_master_start(cmd);
    //         i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, 1);
    //         i2c_master_stop(cmd);
    //         esp_err_t ret = i2c_master_cmd_begin(0, cmd, 50 / portTICK_RATE_MS);
    //         i2c_cmd_link_delete(cmd);
    //         if (ret == ESP_OK)
    //         {
    //             printf("%02x ", address);
    //         }
    //         else if (ret == ESP_ERR_TIMEOUT)
    //         {
    //             printf("UU ");
    //         }
    //         else
    //         {
    //             printf("-- ");
    //         }
    //     }
    //     printf("\r\n");
    // }

    esp_err_t ret = ESP_OK;
    for (int i = 0; i < GT911_PRODUCT_ID_LEN; i++)
    {
        ret = gt911_read_byte((GT911_PRODUCT_ID1 + i), (uint8_t *)&(gt911_status.product_id[i]));
    }
    ESP_LOGI("TAG", "\tProduct ID: %s", gt911_status.product_id);

    ret = gt911_read_byte(GT911_VENDOR_ID, &data_buf);
    ESP_LOGI("TAG", "\tVendor ID: 0x%02x", data_buf);

    if (ret == ESP_OK)
        ESP_LOGI("gt911", "gt911 init ok");
    else
    {
        ESP_LOGI("gt911", "gt911 init fail");
        // esp_restart();
    }

    gt911_read_byte(GT911_X_COORD_RES_L, &data_buf);
    gt911_status.max_x_coord = data_buf;
    gt911_read_byte(GT911_X_COORD_RES_H, &data_buf);
    gt911_status.max_x_coord |= ((uint16_t)data_buf << 8);
    ESP_LOGI("TAG", "\tX Resolution: %d", gt911_status.max_x_coord);

    gt911_read_byte(GT911_Y_COORD_RES_L, &data_buf);
    gt911_status.max_y_coord = data_buf;
    gt911_read_byte(GT911_Y_COORD_RES_H, &data_buf);
    gt911_status.max_y_coord |= ((uint16_t)data_buf << 8);
    ESP_LOGI("TAG", "\tY Resolution: %d", gt911_status.max_y_coord);

    return ESP_OK;
}
esp_err_t gt911_read_pos(uint8_t *touch_points_num, uint16_t *x, uint16_t *y)
{
    esp_err_t ret_val = ESP_OK;
    // static uint8_t data[4];

    // ret_val |= ft5x06_get_touch_points_num(touch_points_num);

    // if (0 == *touch_points_num)
    // {
    // }
    // else
    // {
    //     ret_val |= ft5x06_read_bytes(FT5x06_TOUCH1_XH, 4, data);

    //     *x = ((data[0] & 0x0f) << 8) + data[1];
    //     *y = ((data[2] & 0x0f) << 8) + data[3];
    // }
    static uint8_t data_now[4];
    // uint8_t touch_pnt_cnt;     // Number of detected touch points
    static int16_t last_x = 0; // 12bit pixel value
    static int16_t last_y = 0; // 12bit pixel value
    uint8_t data_buf;
    uint8_t status_reg;
    gt911_read_byte(GT911_STATUS_REG, &status_reg);
    // ESP_LOGI("TAG", "\tstatus: 0x%02x", status_reg);
    *touch_points_num = status_reg & 0x0F;
    if ((status_reg & 0x80) || (*touch_points_num < 6))
    {
        // Reset Status Reg Value
        gt911_write_byte(GT911_STATUS_REG, 0x00);
    }
    if (*touch_points_num != 1)
    { // ignore no touch & multi touch
        // data->point.x = last_x;
        // data->point.y = last_y;
        // data->state = LV_INDEV_STATE_REL;
        return ESP_FAIL;
    }

    ret_val |= gt911_read_bytes(GT911_PT1_X_COORD_L, 4, &data_now);
    //    gt911_i2c_read(gt911_status.i2c_dev_addr, GT911_TRACK_ID1, &data_buf, 1);
    //    ESP_LOGI(TAG, "\ttrack_id: %d", data_buf);
    // for (size_t i = 0; i < 4; i++)
    // {
    //     printf("%x ", data_now[i]);
    // }
    // printf("\n");
    last_x = ((data_now[1]) << 8) + data_now[0];
    last_y = ((data_now[3]) << 8) + data_now[2];

#if CONFIG_LV_GT911_INVERT_X
    last_x = gt911_status.max_x_coord - last_x;
#endif
#if CONFIG_LV_GT911_INVERT_Y
    last_y = gt911_status.max_y_coord - last_y;
#endif
#if CONFIG_LV_GT911_SWAPXY
    int16_t swap_buf = last_x;
    last_x = last_y;
    last_y = swap_buf;
#endif
    // data->point.x = last_x;
    // data->point.y = last_y;
    // data->state = LV_INDEV_STATE_PR;
    *x = last_x;
    *y = last_y;
    ESP_LOGI(TAG, "X=%u Y=%u", *x, *y);
    // ESP_LOGV(TAG, "X=%u Y=%u", data->point.x, data->point.y);

    return ret_val;
}

// /**
//  * @brief  Get the touch screen X and Y positions values. Ignores multi touch
//  * @param  drv:
//  * @param  data: Store data here
//  * @retval Always false
//  */
// bool gt911_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
// {

//     static uint8_t data_now[4];
//     uint8_t touch_pnt_cnt;     // Number of detected touch points
//     static int16_t last_x = 0; // 12bit pixel value
//     static int16_t last_y = 0; // 12bit pixel value
//     uint8_t data_buf;
//     uint8_t status_reg;
//     gt911_read_byte(GT911_STATUS_REG, &status_reg);
//     // ESP_LOGI("TAG", "\tstatus: 0x%02x", status_reg);
//     touch_pnt_cnt = status_reg & 0x0F;
//     if ((status_reg & 0x80) || (touch_pnt_cnt < 6))
//     {
//         // Reset Status Reg Value
//         gt911_write_byte(GT911_STATUS_REG, 0x00);
//     }
//     if (touch_pnt_cnt != 1)
//     { // ignore no touch & multi touch
//         data->point.x = last_x;
//         data->point.y = last_y;
//         data->state = LV_INDEV_STATE_REL;
//         return false;
//     }

//     gt911_read_bytes(GT911_PT1_X_COORD_L, 4, &data_now);
//     //    gt911_i2c_read(gt911_status.i2c_dev_addr, GT911_TRACK_ID1, &data_buf, 1);
//     //    ESP_LOGI(TAG, "\ttrack_id: %d", data_buf);

//     last_x = ((data_now[1]) << 8) + data_now[0];
//     last_y = ((data_now[3]) << 8) + data_now[2];

// #if CONFIG_LV_GT911_INVERT_X
//     last_x = gt911_status.max_x_coord - last_x;
// #endif
// #if CONFIG_LV_GT911_INVERT_Y
//     last_y = gt911_status.max_y_coord - last_y;
// #endif
// #if CONFIG_LV_GT911_SWAPXY
//     int16_t swap_buf = last_x;
//     last_x = last_y;
//     last_y = swap_buf;
// #endif
//     data->point.x = last_x;
//     data->point.y = last_y;
//     data->state = LV_INDEV_STATE_PR;
//     ESP_LOGI(TAG, "X=%u Y=%u", data->point.x, data->point.y);
//     ESP_LOGV(TAG, "X=%u Y=%u", data->point.x, data->point.y);

//     // #if LCD_DIRECTION_LANDSCAPE // landscape mode
//     //     *y = LCD_HEIGHT - ((data_now[0] & 0x0f) << 8) - data_now[1];
//     //     *x = ((data_now[2] & 0x0f) << 8) + data_now[3];
//     // #else // portrait mode
//     //     *x = ((data_now[0] & 0x0f) << 8) + data_now[1];
//     //     *y = ((data_now[2] & 0x0f) << 8) + data_now[3];
//     // #endif
//     //     ESP_LOGI("TAG", "X=%u Y=%u", *x, *y);
//     // return ESP_OK;
//     return false;
// }
