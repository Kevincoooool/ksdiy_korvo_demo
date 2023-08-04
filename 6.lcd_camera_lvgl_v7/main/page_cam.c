/*
 * @Descripttion :
 * @version      :
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-02-18 18:54:29
 * @FilePath: \6.lcd_camera_lvgl_v7\main\page_cam.c
 */
#include "page_cam.h"
#include "app_main.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>

#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "lv_port_indev.h"
#include "app_camera.h"

#include <esp_system.h>
#include "esp_log.h"
#include "lv_port_indev.h"

#define TAG "PAGE_CAM"
extern camera_fb_t *fb;
lv_obj_t *img_cam; //要显示图像
lv_img_dsc_t img_dsc = {
	.header.always_zero = 0,
	.header.w = 320,
	.header.h = 240,
	.data_size = 320 * 240 * 2,
	.header.cf = LV_IMG_CF_TRUE_COLOR,
	.data = NULL,
};
void Cam_Task(void *pvParameters)
{

	// /* 入口处检测一次 */
	ESP_LOGI(TAG, "Run Run uxHighWaterMark = %d", uxTaskGetStackHighWaterMark(NULL));
	// FILE *fp = NULL;
	portTickType xLastWakeTime;
	vTaskDelay(100 / portTICK_PERIOD_MS);
	while (1)
	{

		static int64_t last_frame = 0;
		if (!last_frame)
		{
			last_frame = esp_timer_get_time();
		}
		fb = esp_camera_fb_get();
		if (fb == NULL)
		{
			vTaskDelay(100);
			ESP_LOGE(TAG, "Get image failed!");
		}
		else
		{
			// for (int i = 0; i < fb->len; i += 2)
			// {
			// 	uint8_t temp = 0;
			// 	temp = fb->buf[i];
			// 	fb->buf[i] = fb->buf[i + 1];
			// 	fb->buf[i + 1] = temp;
			// }
			img_dsc.data = fb->buf;
			lv_img_set_src(img_cam, &img_dsc);

			esp_camera_fb_return(fb);
			// fb = NULL;
			int64_t fr_end = esp_timer_get_time();
			int64_t frame_time = fr_end - last_frame;
			last_frame = fr_end;
			frame_time /= 1000;
			ESP_LOGI("esp", "MJPG:  %ums (%.1ffps)", (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time);
		}
	}

	// never reach
	while (1)
	{
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}

void imgcam_init(void)
{

	img_cam = lv_img_create(lv_scr_act(), NULL);
	static lv_style_t style_img;
	lv_style_init(&style_img);

	// Write style state: LV_STATE_DEFAULT for style_img
	lv_style_set_image_recolor(&style_img, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_image_recolor_opa(&style_img, LV_STATE_DEFAULT, 0);
	lv_style_set_image_opa(&style_img, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(img_cam, LV_IMG_PART_MAIN, &style_img);
	lv_obj_set_pos(img_cam, 0, 0);
	lv_obj_set_size(img_cam, 320, 240);
}
void page_cam_load()
{
	app_camera_init();//初始化摄像头
	vTaskDelay(100);
	imgcam_init();
	vTaskDelay(1000);
	xTaskCreatePinnedToCore(&Cam_Task, "Cam_Task", 1024 * 5, NULL, 14, NULL, 0);
}
