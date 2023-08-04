/*
 * Copyright © 2020 Wolfgang Christl
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

#include "cst816.h"

#include "i2c_bus.h"
#include "esp_err.h"
#include "bsp_i2c.h"

#define TAG "CST816"
#define CST816_TOUCH_QUEUE_ELEMENTS 1

static CST816_status_t CST816_status;
static uint8_t current_dev_addr; // set during init
// static CST816_touch_t touch_inputs = {-1, -1, LV_INDEV_STATE_REL}; // -1 coordinates to designate it was never touched
static i2c_bus_device_handle_t cst816_handle = NULL;

static esp_err_t cst816_i2c_write8(uint8_t slave_addr, uint8_t register_addr, uint8_t *data_buf)
{
    return i2c_bus_write_byte(cst816_handle, register_addr, data_buf);
}
static esp_err_t cst816_i2c_read8(uint8_t slave_addr, uint8_t register_addr, uint8_t *data_buf)
{
    return i2c_bus_read_byte(cst816_handle, register_addr, data_buf);

    // return lvgl_i2c_read(CONFIG_LV_I2C_TOUCH_PORT, slave_addr, register_addr, data_buf, 1);
}

/**
 * @brief  Read the CST816 gesture ID. Initialize first!
 * @param  CST816S_ADDRESS: I2C CST816 Slave address.
 * @retval The gesture ID or 0x00 in case of failure
 */
uint8_t cst816_get_gesture_id()
{
    if (!CST816_status.inited)
    {
        ESP_LOGE(TAG, "Init first!");
        return 0x00;
    }
    uint8_t data_buf;
    esp_err_t ret;
    if ((ret = cst816_i2c_read8(current_dev_addr, CST816_GEST_ID_REG, &data_buf) != ESP_OK))
        ESP_LOGE(TAG, "Error reading from device: %s", esp_err_to_name(ret));
    return data_buf;
}

/**
 * @brief  Initialize for CST816 communication via I2C
 * @param  CST816S_ADDRESS: Device address on communication Bus (I2C slave address of CST816).
 * @retval None
 */

esp_err_t cst816_init(void)
{
    if (NULL != cst816_handle)
    {
        return ESP_ERR_INVALID_STATE;
    }
    bsp_i2c_add_device(&cst816_handle, CST816S_ADDRESS);

    CST816_status.inited = true;
    current_dev_addr = CST816S_ADDRESS;
    uint8_t data_buf = 0x01;
    uint8_t standby_value = 0x01;
    uint8_t xy_buf[6] = {0};
    esp_err_t ret;
    ESP_LOGI(TAG, "Found touch panel controller");
    cst816_i2c_write8(CST816S_ADDRESS, TP_REG_DIS_AUTO_SLEEP, &data_buf);

    if ((ret = cst816_i2c_read8(CST816S_ADDRESS, CST816S_ADDRESS, &data_buf) != ESP_OK))
        ESP_LOGE(TAG, "Error reading from device: %s", esp_err_to_name(ret)); // Only show error the first time
    ESP_LOGI(TAG, "\tDevice ID: 0x%02x", data_buf);

    cst816_i2c_read8(CST816S_ADDRESS, TP_REG_CHIPID, &data_buf);
    ESP_LOGI(TAG, "\tChip ID: 0x%02x", data_buf);
    return ESP_OK;
}

/**
 * @brief  Get the touch screen X and Y positions values. Ignores multi touch
 * @param  drv:
 * @param  data: Store data here
 * @retval Always false
 */

esp_err_t cst816_read_pos(uint8_t *touch_points_num, uint16_t *x, uint16_t *y)
// bool cst816_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    static int16_t last_x = 0; // 12bit pixel value
    static int16_t last_y = 0; // 12bit pixel value
    uint8_t data_buf[6];       // 1 byte status, 2 bytes X, 2 bytes Y

    esp_err_t ret = i2c_bus_read_bytes(cst816_handle, 0x01, 6, &data_buf[0]);
    if (ret != ESP_OK)
    {
        // ESP_LOGE(TAG, "Error talking to touch IC: %s", esp_err_to_name(ret));
        return ESP_FAIL;
    }
    uint8_t touch_pnt_cnt = data_buf[1]; // Number of detected touch points
    *touch_points_num = data_buf[1];
    if (ret != ESP_OK || touch_pnt_cnt != 1)
    { // ignore no touch & multi touch
        // if (touch_inputs.current_state != LV_INDEV_STATE_REL)
        // {
        //     touch_inputs.current_state = LV_INDEV_STATE_REL;
        // }
        // data->point.x = touch_inputs.last_x;
        // data->point.y = touch_inputs.last_y;
        // data->state = touch_inputs.current_state;

        return ESP_FAIL;
    }

    // touch_inputs.current_state = LV_INDEV_STATE_PR;
    last_x = ((data_buf[2] & CST816_MSB_MASK) << 8) | (data_buf[3] & CST816_LSB_MASK);
    last_y = ((data_buf[4] & CST816_MSB_MASK) << 8) | (data_buf[5] & CST816_LSB_MASK);

    // #if CONFIG_LV_CST816_SWAPXY
    int16_t swap_buf = last_x;
    last_x = last_y;
    last_y = swap_buf;
    // #endif
    // #if CONFIG_LV_CST816_INVERT_X
    last_x = 280 - last_x;
// #endif
#if CONFIG_LV_CST816_INVERT_Y
    last_y = 240 - last_y;
#endif
    *x = last_x;
    *y = last_y;
    ESP_LOGI(TAG, "X=%u Y=%u", *x, *y);

    return ESP_OK;
}
