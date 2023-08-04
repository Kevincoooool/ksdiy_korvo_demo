/*
 * @Author: your name
 * @Date: 2021-10-13 17:24:53
 * @LastEditTime: 2021-11-27 17:53:09
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \s3_lvgl_v7\components\bili_http\bili_http.h
 */

#ifndef _BILI_HTTP_H
#define _BILI_HTTP_H
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "cJSON.h"
#include "esp_spiffs.h"
// #include "esp_tls.h"

typedef struct
{
    uint16_t following;
    uint16_t follower;
    char *name;
    char *face_url;
} BILIBILI;
struct DATA_WEATHER
{
    char *day;
    uint8_t code_day;
    char text_day[20];
    int8_t high;
    int8_t low;
    float rainfall;
    char wind_direction[10];
    uint16_t wind_direction_degree;
    float wind_speed;
    uint8_t humidity;
};
extern char *face_buffer;
void show_weather();
esp_err_t http_get_task(const char *REQUEST, const char *SERVER, esp_err_t (*fun)(char *text));
esp_err_t cjson_bilibili(char *text);
esp_err_t cjson_weather(char *text);
esp_err_t read_weather();
esp_err_t read_fans();
uint16_t get_value_ksdiyfs(uint8_t i);
esp_err_t get_blbl_url(char *dev_id);
//void draw_weather(uint8_t i,int x,int y);
#endif