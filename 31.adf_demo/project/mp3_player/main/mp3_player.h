/*
 * @Author: your name
 * @Date: 2021-10-13 17:25:16
 * @LastEditTime: 2021-12-01 16:56:38
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \s3_lvgl_v7\main\mp3_player.h
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
#ifndef _mp3_player_h_
#define _mp3_player_h_
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"


void mp3_player_demo(void);
void audio_sdcard_task(void *para);

#ifdef __cplusplus
}
#endif
#endif