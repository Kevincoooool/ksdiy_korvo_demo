/*
 * @Author: your name
 * @Date: 2021-10-13 17:25:16
 * @LastEditTime: 2021-11-02 10:16:57
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \s3_lvgl_v7\main\app_camera.h
 */
/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-05-25 09:20:06
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-10-13 10:42:21
 * @FilePath     : \esp-idf\pro\S3_LVGL\main\app_camera.h
 */
#ifndef MATERIALS_CLASSIFIER_ESP_APP_CAMERA_ESP_H_
#define MATERIALS_CLASSIFIER_ESP_APP_CAMERA_ESP_H_

#include "esp_camera.h"
#include "esp_log.h"
#include "esp_system.h"
#include "sensor.h"

/**
 * PIXFORMAT_RGB565,    // 2BPP/RGB565
 * PIXFORMAT_YUV422,    // 2BPP/YUV422
 * PIXFORMAT_GRAYSCALE, // 1BPP/GRAYSCALE
 * PIXFORMAT_JPEG,      // JPEG/COMPRESSED
 * PIXFORMAT_RGB888,    // 3BPP/RGB888
 */
#define CAMERA_PIXEL_FORMAT PIXFORMAT_RGB565

/*
 * FRAMESIZE_96X96,    // 96x96
 * FRAMESIZE_QQVGA,    // 160x120
 * FRAMESIZE_QQVGA2,   // 128x160
 * FRAMESIZE_QCIF,     // 176x144
 * FRAMESIZE_HQVGA,    // 240x176
 * FRAMESIZE_240X240    // 240x240
 * FRAMESIZE_QVGA,     // 320x240
 * FRAMESIZE_CIF,      // 400x296
 * FRAMESIZE_VGA,      // 640x480
 * FRAMESIZE_SVGA,     // 800x600
 * FRAMESIZE_XGA,      // 1024x768
 * FRAMESIZE_SXGA,     // 1280x1024
 * FRAMESIZE_UXGA,     // 1600x1200
 */
#define CAMERA_FRAME_SIZE FRAMESIZE_QVGA

#define CAM_PIN_PWDN -1
#define CAM_PIN_RESET -1
#define CAM_PIN_XCLK 40
#define CAM_PIN_SIOD 17
#define CAM_PIN_SIOC 18

#define CAM_PIN_D7 39
#define CAM_PIN_D6 41
#define CAM_PIN_D5 42
#define CAM_PIN_D4 12
#define CAM_PIN_D3 3
#define CAM_PIN_D2 14
#define CAM_PIN_D1 47
#define CAM_PIN_D0 13
#define CAM_PIN_VSYNC 21
#define CAM_PIN_HREF 38
#define CAM_PIN_PCLK 11

#define FLIP_CAMERA 1

#define I2C_SDA 21
#define I2C_SCL 19

#define IIS_SCK 2
#define IIS_WS 22
#define IIS_DOUT 15

#define TFT_CS_PIN 5
#define TFT_DC_PIN 4
#define TFT_MOSI_PIN 23
#define TFT_MISO_PIN -1
#define TFT_SCLK_PIN 18
#define TFT_BL_PIN -1

#define SDCARD_CS_PIN 0

// #define ENABLE_IP5306
#define ENABLE_TFT

#define XCLK_FREQ 20000000

#ifdef __cplusplus
extern "C"
{
#endif

    void app_camera_init();//初始化摄像头
    void web_camera_init();
#ifdef __cplusplus
}
#endif
#endif