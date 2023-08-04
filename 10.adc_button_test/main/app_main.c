/*
 * @Descripttion :ADC测试例程
 * @version      :
 * @Author       : Kevincoooool
 * @Date         : 2021-09-04 16:11:59
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-08-25 09:14:43
 * @FilePath: \Korvo_Demo\10.adc_button_test\main\app_main.c
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "lv_examples/src/lv_demo_widgets/lv_demo_widgets.h"
#include "lv_examples/src/lv_demo_music/lv_demo_music.h"
#include "lv_examples/src/lv_demo_benchmark/lv_demo_benchmark.h"
#include "lvgl_helpers.h"
#include "esp_freertos_hooks.h"
#include "button.h"
#include "nvs_flash.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "bsp_adc.h"
#include "esp_log.h"
#include "lv_port_indev.h"
#include "esp_err.h"
#include "tca9554.h"
#include "driver/i2c.h"

#define LCD_CTRL_GPIO BIT(1) // TCA9554_GPIO_NUM_1
#define LCD_RST_GPIO BIT(2)	 // TCA9554_GPIO_NUM_2
#define LCD_CS_GPIO BIT(3)	 // TCA9554_GPIO_NUM_2

#define TAG "ESP32S3"

static void lv_tick_task(void *arg)
{
	(void)arg;
	lv_tick_inc(10);
}
void button_task(void *arg)
{
	Button_Init();
	while (1)
	{
		Button_Process();
		vTaskDelay(20 / portTICK_PERIOD_MS);
	}
}
lv_obj_t *label_test;

void label_init(void)
{

	label_test = lv_label_create(lv_scr_act(), NULL);
	/*Modify the Label's text*/

	static lv_style_t style_label_test;
	lv_style_init(&style_label_test);

	// Write style state: LV_STATE_DEFAULT for style_label_test
	lv_style_set_radius(&style_label_test, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_label_test, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_bg_grad_color(&style_label_test, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_bg_grad_dir(&style_label_test, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_label_test, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&style_label_test, LV_STATE_DEFAULT, LV_COLOR_BLACK);

	// lv_obj_set_style_local_text_font(label_test, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_style_set_text_letter_space(&style_label_test, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_label_test, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_label_test, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_label_test, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_label_test, LV_STATE_DEFAULT, 0);

	lv_obj_add_style(label_test, LV_LABEL_PART_MAIN, &style_label_test);
	lv_label_set_long_mode(label_test, LV_LABEL_LONG_BREAK);
	lv_obj_set_pos(label_test, 0, 10);
	lv_obj_set_size(label_test, LV_HOR_RES, LV_VER_RES);
	lv_label_set_recolor(label_test, true);
	lv_label_set_text(label_test, "Hello World!\nPlease press down the button...");
}
SemaphoreHandle_t xGuiSemaphore;

static void gui_task(void *arg)
{
	xGuiSemaphore = xSemaphoreCreateMutex();
	lv_init(); // lvgl内核初始化
	tca9554_set_output_state(LCD_RST_GPIO, TCA9554_IO_LOW);
	vTaskDelay(20 / portTICK_PERIOD_MS);
	tca9554_set_output_state(LCD_RST_GPIO, TCA9554_IO_HIGH);
	vTaskDelay(200 / portTICK_PERIOD_MS);

	lvgl_driver_init(); // lvgl显示接口初始化
	//申请两个buffer来给lvgl刷屏用
	/*外部PSRAM方式*/
	// lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
	// lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

	/*内部DMA方式*/
	lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
	lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);

	// static lv_color_t buf1[DISP_BUF_SIZE];
	// static lv_color_t buf2[DISP_BUF_SIZE];
	static lv_disp_buf_t disp_buf;
	uint32_t size_in_px = DISP_BUF_SIZE;
	lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.flush_cb = disp_driver_flush;
	disp_drv.buffer = &disp_buf;
	lv_disp_drv_register(&disp_drv);

	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.read_cb = touch_driver_read;
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	lv_indev_drv_register(&indev_drv);

	// esp_register_freertos_tick_hook(lv_tick_task);
	/* 创建一个定时器中断来进入 lv_tick_inc 给lvgl运行提供心跳 这里是10ms一次 主要是动画运行要用到 */
	const esp_timer_create_args_t periodic_timer_args = {
		.callback = &lv_tick_task,
		.name = "periodic_gui"};
	esp_timer_handle_t periodic_timer;
	ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 10 * 1000));
	//按键初始化
	lv_port_indev_init();

	label_init();

	// lv_demo_widgets();
	while (1)
	{
		/* Delay 1 tick (assumes FreeRTOS tick is 10ms */
		vTaskDelay(pdMS_TO_TICKS(10));
		/* Try to take the semaphore, call lvgl related function on success */
		if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
		{

			lv_task_handler();
			xSemaphoreGive(xGuiSemaphore);
		}
	}
}

void app_main(void)
{

	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	esp_tca9554_config_t pca_cfg = {
		.i2c_scl = GPIO_NUM_18,
		.i2c_sda = GPIO_NUM_17,
		.interrupt_output = -1,
	};
	tca9554_init(&pca_cfg);
	// Set LCD_BL_CTRL output
	tca9554_set_io_config(LCD_CTRL_GPIO, TCA9554_IO_OUTPUT);
	// Set LCD_RST output
	tca9554_set_io_config(LCD_RST_GPIO, TCA9554_IO_OUTPUT);
	// Set LCD_CS pin output
	tca9554_set_io_config(LCD_CS_GPIO, TCA9554_IO_OUTPUT);

	tca9554_set_output_state(LCD_CTRL_GPIO, TCA9554_IO_HIGH);

	tca9554_set_output_state(LCD_RST_GPIO, TCA9554_IO_LOW);
	vTaskDelay(20 / portTICK_PERIOD_MS);
	tca9554_set_output_state(LCD_RST_GPIO, TCA9554_IO_HIGH);
	vTaskDelay(200 / portTICK_PERIOD_MS);

	tca9554_set_output_state(LCD_CS_GPIO, TCA9554_IO_LOW);

	uint8_t address;
	printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");
	for (int i = 0; i < 128; i += 16)
	{
	    printf("%02x: ", i);
	    for (int j = 0; j < 16; j++)
	    {
	        fflush(stdout);
	        address = i + j;
	        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	        i2c_master_start(cmd);
	        i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, 1);
	        i2c_master_stop(cmd);
	        esp_err_t ret = i2c_master_cmd_begin(0, cmd, 50 / portTICK_RATE_MS);
	        i2c_cmd_link_delete(cmd);
	        if (ret == ESP_OK)
	        {
	            printf("%02x ", address);
	        }
	        else if (ret == ESP_ERR_TIMEOUT)
	        {
	            printf("UU ");
	        }
	        else
	        {
	            printf("-- ");
	        }
	    }
	    printf("\r\n");
	}

	/*初始化ADC引脚  用于ADC按键*/
	//
	// vTaskDelay(5000);
	/*创建lvgl任务显示*/
	xTaskCreatePinnedToCore(&gui_task, "gui task", 1024 * 5, NULL, 5, NULL, 1);
	/*创建按键任务 定时扫描按键值*/
	xTaskCreatePinnedToCore(&button_task, "button_task", 1024 * 3, NULL, 8, NULL, 0);
	adc_init();
	printf("adc_value: %d\n", get_adc());
	while (1)
	{
		// printf("adc_value: %d\n", get_adc());
		if (Button_Value != BT_NONE)
		{

			lv_label_set_text(label_test, Button_Tips[Button_Value]);
			Button_Value = BT_NONE;
		}
		vTaskDelay(20 / portTICK_PERIOD_MS);
	}
}
