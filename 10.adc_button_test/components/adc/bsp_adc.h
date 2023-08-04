/*
 * @Author: your name
 * @Date: 2021-10-13 17:24:29
 * @LastEditTime: 2021-10-29 17:52:44
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \s3_lvgl_v7\components\adc\bsp_adc.h
 */

#ifndef _BSP_ADC_
#define _BSP_ADC_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"

#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define LPF_1_(hz, t, in, out) ((out) += (1 / (1 + 1 / ((hz)*6.28f * (t)))) * ((in) - (out)))

uint16_t get_adc_vol(void);
void adc_init();
uint16_t get_adc();
uint8_t Get_Adc_Button();
#endif