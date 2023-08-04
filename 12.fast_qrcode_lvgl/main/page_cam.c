/*
 * @Descripttion :
 * @version      :
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-03-09 14:30:08
 * @FilePath: \S3_DEMO\12.fast_qrcode_lvgl\main\page_cam.c
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
#include "esp_code_scanner.h"

#define TAG "PAGE_CAM"
extern camera_fb_t *fb;
lv_obj_t *img_cam; //要显示图像
lv_obj_t *label_detect;
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
	int64_t time1, time2;
	// esp_image_scanner_t *esp_scn = esp_code_scanner_create();
	// esp_code_scanner_config_t config = {ESP_CODE_SCANNER_MODE_FAST, ESP_CODE_SCANNER_IMAGE_RGB565, 320, 240};
	// esp_code_scanner_set_config(esp_scn, config);
	while (1)
	{

		static int64_t last_frame = 0;
		if (!last_frame)
		{
			last_frame = esp_timer_get_time();
		}
		fb = esp_camera_fb_get();//获取图像
		time1 = esp_timer_get_time();
		if (fb == NULL)
		{
			vTaskDelay(100);
			ESP_LOGE(TAG, "Get image failed!");
		}
		else
		{
			img_dsc.data = fb->buf;//把获取到的图片的数据传给img_dsc
			lv_img_set_src(img_cam, &img_dsc);//设置img_cam的源文件为img_dsc  完成显示图片
			// Decode Progress
			esp_image_scanner_t *esp_scn = esp_code_scanner_create();
			esp_code_scanner_config_t config = {ESP_CODE_SCANNER_MODE_FAST, ESP_CODE_SCANNER_IMAGE_RGB565, fb->width, fb->height};
			esp_code_scanner_set_config(esp_scn, config);
			int decoded_num = esp_code_scanner_scan_image(esp_scn, fb->buf);

			if (decoded_num)//如果识别到了一个二维码
			{
				esp_code_scanner_symbol_t result = esp_code_scanner_result(esp_scn);
				time2 = esp_timer_get_time();
				ESP_LOGI(TAG, "Decode time in %lld ms.", (time2 - time1) / 1000);
				ESP_LOGI(TAG, "Decoded %s symbol \"%s\"\n", result.type_name, result.data);
				lv_label_set_text(label_detect, (char *)result.data);
			}
			else
				lv_label_set_text(label_detect, "Nothing...");//没有识别到
			esp_code_scanner_destroy(esp_scn);
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

	label_detect = lv_label_create(lv_scr_act(), NULL);
	/*Modify the Label's text*/

	static lv_style_t style_label_detect;
	lv_style_init(&style_label_detect);

	// Write style state: LV_STATE_DEFAULT for style_label_detect
	lv_style_set_radius(&style_label_detect, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_label_detect, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_bg_grad_color(&style_label_detect, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_bg_grad_dir(&style_label_detect, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_label_detect, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&style_label_detect, LV_STATE_DEFAULT, LV_COLOR_BLACK);

	// lv_obj_set_style_local_text_font(label_detect, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_style_set_text_letter_space(&style_label_detect, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_label_detect, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_label_detect, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_label_detect, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_label_detect, LV_STATE_DEFAULT, 0);

	lv_obj_add_style(label_detect, LV_LABEL_PART_MAIN, &style_label_detect);
	lv_label_set_long_mode(label_detect, LV_LABEL_LONG_BREAK);
	lv_obj_set_pos(label_detect, 0, LV_VER_RES - 50);
	lv_obj_set_size(label_detect, LV_HOR_RES, 50);
	lv_label_set_recolor(label_detect, true);
	lv_label_set_text(label_detect, "Hello!");
}
void page_cam_load()
{
	app_camera_init();//初始化摄像头
	vTaskDelay(100);
	imgcam_init();//初始化一个imgcam用于显示
	vTaskDelay(1000);
	xTaskCreatePinnedToCore(&Cam_Task, "Cam_Task", 1024 * 5, NULL, 14, NULL, 0);
}
