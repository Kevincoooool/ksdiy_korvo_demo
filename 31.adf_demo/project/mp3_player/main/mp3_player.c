/*
 * @Descripttion :
 * @version      :
 * @Author       : Kevincoooool
 * @Date         : 2021-05-25 09:20:06
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-08-25 14:05:24
 * @FilePath: \Korvo_Demo\31.adf_demo\project\mp3_player\main\mp3_player.c
 */
#include "mp3_player.h"

#include <stdio.h>

/* freertos includes */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_freertos_hooks.h"

#include "sdkconfig.h"

/* lvgl includes */
#include "lvgl.h"

/* lvgl calibration includes */
// #include "lv_calibration.h"

/* esp includes */
#include "esp_log.h"
#include "esp_err.h"
#include "ff.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "board.h"
#include "file_manager.h"
/* Param Include */
// #include "iot_param.h"
#include "nvs_flash.h"

#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_mem.h"
#include "audio_common.h"
#include "http_stream.h"
#include "i2s_stream.h"
#include "mp3_decoder.h"
#include "wav_decoder.h"
#include "fatfs_stream.h"
#include "esp_peripherals.h"
#include "periph_sdcard.h"
#include "esp_peripherals.h"
#include "periph_wifi.h"
#include "audio_alc.h"
#include <inttypes.h>
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"
#include "fatfs_stream.h"
#include "i2s_stream.h"
#include "mp3_decoder.h"
#include "filter_resample.h"

#include "esp_peripherals.h"
#include "periph_sdcard.h"
#include "periph_touch.h"
#include "periph_button.h"
#include "input_key_service.h"
#include "periph_adc_button.h"
#include "board.h"

#include "sdcard_list.h"
#include "sdcard_scan.h"

// #define LVGL_TEST_THEME
#define LVGL_EXAMPLE

#define CONTROL_CURRENT -1
#define CONTROL_NEXT -2
#define CONTROL_PREV -3

static char directory[1024][100];
static uint16_t filecount = 0;

#ifdef LVGL_EXAMPLE
static const char *TAG = "example_lvgl";
#endif

static audio_pipeline_handle_t pipeline;
static audio_element_handle_t http_stream_reader;
static audio_element_handle_t alc_el;
static audio_board_handle_t board_handle;

audio_element_handle_t i2s_stream_writer, mp3_decoder, fatfs_stream_reader, rsp_handle, wav_decoder;
playlist_operator_handle_t sdcard_list_handle = NULL;

/*目前增在播放的音乐名字label*/
static lv_obj_t *current_music;
/*上一首 播放暂停 下一首*/
static lv_obj_t *button[3];
/*三个按键的图片*/
static lv_obj_t *img[3];
/*播放列表*/
static lv_obj_t *list_music;
static lv_obj_t *process_bar;
static int file_length = 0, read_length = 0;
void *img_src[] = {LV_SYMBOL_PREV, LV_SYMBOL_PLAY, LV_SYMBOL_NEXT, LV_SYMBOL_PAUSE};
void read_content(const char *path, uint8_t *filecount);
static int Get_File_Length(void)
{

    return file_length;
}
/*
    获取文件句柄 用于给mp3输入流读数据
*/
static FILE *get_file(int next_file)
{
    static FILE *file;
    static int file_index = 0;

    if (next_file != CONTROL_CURRENT)
    {
        if (next_file == CONTROL_NEXT)
        {
            // advance to the next file
            if (++file_index > filecount - 1)
            {
                file_index = 0;
            }
        }
        else if (next_file == CONTROL_PREV)
        {
            // advance to the prev file
            if (--file_index < 0)
            {
                file_index = filecount - 1;
            }
        }
        else if (next_file >= 0 && next_file < filecount)
        {
            file_index = next_file;
        }
        if (file != NULL)
        {
            fclose(file);
            file = NULL;
        }
        ESP_LOGI(TAG, "[ * ] File index %d", file_index);
    }
    // return a handle to the current file
    if (file == NULL)
    {
        printf("directory[file_index]:%s \r\n", directory[file_index]);
        lv_label_set_text(current_music, strstr(directory[file_index], "d/") + 2);
        // audio_element_set_uri(fatfs_stream_reader, directory[file_index]);
        // audio_element_set_uri(fatfs_stream_reader, directory[file_index]);
        file = fopen(directory[file_index], "r");
        struct stat siz = {0};
        stat(directory[file_index], &siz);
        ESP_LOGW(TAG, "File size: %d byte", (int)siz.st_size);
        read_length = 0;
        file_length = (int)siz.st_size;
        if (file == NULL)
        {
            ESP_LOGE(TAG, "Error opening file");
            return NULL;
        }
        else
        {
            ESP_LOGI(TAG, "Opening file success %s", directory[file_index]);
        }
    }
    return file;
}
static lv_res_t audio_control(void)
{
    audio_element_state_t el_state = audio_element_get_state(i2s_stream_writer);
    switch (el_state)
    {
    case AEL_STATE_INIT:
        lv_img_set_src(img[1], img_src[3]);
        audio_pipeline_run(pipeline);
        break;
    case AEL_STATE_RUNNING:
        lv_img_set_src(img[1], img_src[1]);
        audio_pipeline_pause(pipeline);
        break;
    case AEL_STATE_PAUSED:
        lv_img_set_src(img[1], img_src[3]);
        audio_pipeline_resume(pipeline);
        break;
    default:
        break;
    }
    return LV_RES_OK;
}

static lv_res_t play_list_event(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        char src[50], dest[50];
        strcpy(dest, "file://sdcard/");
        strcpy(src, lv_list_get_btn_text(obj));
        strcat(dest, src);
        printf("Clicked %s\n", dest);
        // char *url = NULL;
        audio_pipeline_stop(pipeline);
        audio_pipeline_wait_for_stop(pipeline);
        audio_pipeline_terminate(pipeline);
        // sdcard_list_prev(sdcard_list_handle, 1, &url);
        ESP_LOGW(TAG, "URL: %s", dest);
        lv_label_set_text(current_music, dest);

        audio_element_set_uri(fatfs_stream_reader, dest);
        audio_pipeline_reset_ringbuffer(pipeline);
        audio_pipeline_reset_elements(pipeline);
        audio_pipeline_run(pipeline);
    }
    return LV_RES_OK;
}
/*左键*/
static void event_handler1(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        printf("Clicked\n");
        ESP_LOGI(TAG, "[ * ] [Set] touch tap event");

        if (audio_element_get_state(i2s_stream_writer) == AEL_STATE_RUNNING) //如果正在播放就暂停
            audio_pipeline_pause(pipeline);
        char *url = NULL;
        audio_pipeline_stop(pipeline);
        audio_pipeline_wait_for_stop(pipeline);
        audio_pipeline_terminate(pipeline);
        sdcard_list_prev(sdcard_list_handle, 1, &url);
        ESP_LOGW(TAG, "URL: %s", url);
        lv_label_set_text(current_music, url + 14);

        audio_element_set_uri(fatfs_stream_reader, url);
        audio_pipeline_reset_ringbuffer(pipeline);
        audio_pipeline_reset_elements(pipeline);
        audio_pipeline_run(pipeline);

        lv_img_set_src(img[1], img_src[3]);
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        printf("Toggled\n");
    }
}
/*确认键*/
static void event_handler2(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        printf("Clicked\n");
        audio_control();
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        printf("Toggled\n");
    }
}
/*右键*/
static void event_handler3(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        printf("Clicked\n");
        ESP_LOGI(TAG, "[ * ] [Set] touch tap event");
        if (audio_element_get_state(i2s_stream_writer) == AEL_STATE_RUNNING) //如果正在播放就暂停
            audio_pipeline_pause(pipeline);
        ESP_LOGI(TAG, "[ * ] Stopped, advancing to the next song");
        char *url = NULL;
        audio_pipeline_stop(pipeline);
        audio_pipeline_wait_for_stop(pipeline);
        audio_pipeline_terminate(pipeline);
        sdcard_list_next(sdcard_list_handle, 1, &url);
        ESP_LOGW(TAG, "URL: %s", url);
        lv_label_set_text(current_music, url + 14);

        audio_element_set_uri(fatfs_stream_reader, url);
        audio_pipeline_reset_ringbuffer(pipeline);
        audio_pipeline_reset_elements(pipeline);
        audio_pipeline_run(pipeline);

        lv_img_set_src(img[1], img_src[3]);
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        printf("Toggled\n");
    }
}
FILE *file_;
uint8_t buff[100] = {0};

static void sl_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        // printf("Volume Value: %d\n", lv_slider_get_value(obj));
        // i2s_alc_volume_set(i2s_stream_writer, lv_slider_get_value(obj));

        audio_hal_set_volume(board_handle->audio_hal, lv_slider_get_value(obj));
        // ESP_LOGI(TAG, "[ * ] Volume set to %d %%", player_volume);
    }
}
LV_FONT_DECLARE(myFont);
void mp3_player_demo(void)
{
    /*先初始化一下SD卡 读取MP3列表以显示*/
    fm_sdcard_init();
    // vTaskDelay(1000);
    filecount = 0;
    read_content("/sdcard", &filecount);
    printf("%d\n", filecount);
    // vTaskDelay(1000);
    // sd_unmount();

    xTaskCreate(&audio_sdcard_task, "audio_sdcard_task", 1024 * 5, NULL, 4, NULL);
    vTaskDelay(300);

    lv_obj_t *tabview = lv_tabview_create(lv_scr_act(), NULL);
    lv_obj_t *tab1 = lv_tabview_add_tab(tabview, LV_SYMBOL_AUDIO);
    lv_obj_t *tab2 = lv_tabview_add_tab(tabview, LV_SYMBOL_LIST);
    // lv_obj_t *tab3 = lv_tabview_add_tab(tabview, LV_SYMBOL_SETTINGS);
    lv_tabview_set_tab_act(tabview, 0, false);

    static lv_style_t style_label_speech;
    lv_style_init(&style_label_speech);

    // Write style state: LV_STATE_DEFAULT for style_label_speech
    lv_style_set_radius(&style_label_speech, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&style_label_speech, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_bg_grad_color(&style_label_speech, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_bg_grad_dir(&style_label_speech, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_label_speech, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_label_speech, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_letter_space(&style_label_speech, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_left(&style_label_speech, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_label_speech, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_label_speech, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_label_speech, LV_STATE_DEFAULT, 0);

    current_music = lv_label_create(tab1, NULL);

    lv_obj_align(current_music, tab1, LV_ALIGN_IN_TOP_MID, 0, 0); /*Align to center*/
    lv_obj_set_style_local_text_font(current_music, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
    lv_label_set_text(current_music, strstr(directory[0], "d/") + 2);
    lv_obj_add_style(current_music, LV_LABEL_PART_MAIN, &style_label_speech);
    lv_label_set_long_mode(current_music, LV_LABEL_LONG_SROLL_CIRC);
    lv_obj_set_pos(current_music, 20, 30);
    lv_obj_set_size(current_music, 220, 40);

    // audio_element_set_uri(fatfs_stream_reader, directory[0]);
    // lv_cont_set_fit(cont, false, false);
    // lv_cont_set_fit2(cont, LV_FIT_NONE, LV_FIT_NONE);

    //三个按钮播放
    for (uint8_t i = 0; i < 3; i++)
    {
        button[i] = lv_btn_create(tab1, NULL);
        lv_obj_set_size(button[i], 50, 50);
        img[i] = lv_img_create(button[i], NULL);
        lv_img_set_src(img[i], img_src[i]);
    }
    lv_obj_align(button[0], tab1, LV_ALIGN_IN_LEFT_MID, 20, 20);
    for (uint8_t i = 1; i < 3; i++)
    {
        lv_obj_align(button[i], button[i - 1], LV_ALIGN_OUT_RIGHT_MID, 40, 0);
    }
    lv_obj_set_event_cb(button[0], event_handler1);
    lv_obj_set_event_cb(button[1], event_handler2);
    lv_obj_set_event_cb(button[2], event_handler3);

    //音量控制滑块
    lv_obj_t *slider = lv_slider_create(tab1, NULL);
    lv_obj_set_size(slider, 5, 140);
    lv_obj_set_event_cb(slider, sl_event_handler);
    lv_obj_set_adv_hittest(slider, true);
    lv_obj_set_pos(slider, 270, 10);

    process_bar = lv_bar_create(lv_scr_act(), NULL);
    lv_bar_set_value(process_bar, 0, LV_ANIM_ON);
    lv_obj_set_size(process_bar, LV_HOR_RES_MAX, 10);
    lv_obj_align(process_bar, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    //文件列表
    lv_obj_t *list = lv_list_create(tab2, NULL);
    lv_obj_align(list, NULL, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_align(list, NULL, LV_ALIGN_IN_TOP_MID, 0, 0); /*Align to center*/
    lv_obj_set_size(list, LV_HOR_RES_MAX - 50, LV_VER_RES - 75);
    lv_obj_set_style_local_text_font(list, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &myFont); // 字体大小

    for (uint8_t i = 0; i < filecount; i++)
    {
        list_music = lv_list_add_btn(list, LV_SYMBOL_AUDIO, strstr(directory[i], "d/") + 2);
        lv_obj_set_style_local_text_font(list_music, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &myFont); // 字体大小
        lv_obj_set_event_cb(list_music, play_list_event);
    }

    ESP_LOGI("LvGL", "app_main last: %d", esp_get_free_heap_size());
}

void read_content(const char *path, uint8_t *filecount)
{
    char nextpath[2048];
    struct dirent *de;
    DIR *dir = opendir(path);
    while (1)
    {
        de = readdir(dir);
        if (!de)
        {
            // printf("error\n");
            return;
        }
        else if (de->d_type == DT_REG)
        {
            // printf("f:%s\n",de->d_name);
            // if(strcasecmp(de->d_name, "hello.txt") == 0){
            //     printf("equal\n");
            // } else {
            //     printf("not equal\n");
            // }
            // if (strstr(de->d_name, ".mp3") || strstr(de->d_name, ".MP3") || strstr(de->d_name, ".WAV") || strstr(de->d_name, ".wav"))
            if (strstr(de->d_name, ".mp3") || strstr(de->d_name, ".MP3"))
            {
                sprintf(directory[*filecount], "%s/%s", path, de->d_name);
                printf("%s\n", directory[*filecount]);
                (*filecount)++;
            }
        }
        else if (de->d_type == DT_DIR)
        {
            // printf("%s\n",de->d_name);
            sprintf(nextpath, "%s/%s", path, de->d_name);
            // printf("d:%s\n",nextpath);
            // sprintf(directory[*filecount], "%s", de->d_name);
            // (*filecount)++;
            read_content(nextpath, filecount);
        }
    }
    closedir(dir);
}

/*
 * Callback function to feed audio data stream from sdcard to mp3 decoder element
 */

static int my_sdcard_read_cb(audio_element_handle_t el, char *buf, int len, TickType_t wait_time, void *ctx)
{
    int read_len = fread(buf, 1, len, get_file(CONTROL_CURRENT));
    // int read_len = fread(buf, 1, len, file_);
    read_length += read_len;
    float pro = (float)((float)(read_length * 100) / (float)file_length);
    printf("read_len%d read_length %dfile_length %d  process %.2f\n", read_len, read_length, file_length, (float)(pro));
    lv_bar_set_value(process_bar, (float)(pro), LV_ANIM_ON);
    if (read_len == 0)
    {
        read_len = AEL_IO_DONE;
    }

    return read_len;
}

static esp_err_t input_key_service_cb(periph_service_handle_t handle, periph_service_event_t *evt, void *ctx)
{
    /* Handle touch pad events
           to start, pause, resume, finish current song and adjust volume
        */
    // audio_board_handle_t board_handle = (audio_board_handle_t)ctx;
    int player_volume;
    audio_hal_get_volume(board_handle->audio_hal, &player_volume);

    if (evt->type == INPUT_KEY_SERVICE_ACTION_CLICK_RELEASE)
    {
        ESP_LOGI(TAG, "[ * ] input key id is %d", (int)evt->data);
        switch ((int)evt->data)
        {
        case INPUT_KEY_USER_ID_PLAY:
            ESP_LOGI(TAG, "[ * ] [Play] input key event");
            audio_element_state_t el_state = audio_element_get_state(i2s_stream_writer);
            switch (el_state)
            {
            case AEL_STATE_INIT:
                ESP_LOGI(TAG, "[ * ] Starting audio pipeline");
                audio_pipeline_run(pipeline);
                break;
            case AEL_STATE_RUNNING:
                ESP_LOGI(TAG, "[ * ] Pausing audio pipeline");
                audio_pipeline_pause(pipeline);
                break;
            case AEL_STATE_PAUSED:
                ESP_LOGI(TAG, "[ * ] Resuming audio pipeline");
                audio_pipeline_resume(pipeline);
                break;
            default:
                ESP_LOGI(TAG, "[ * ] Not supported state %d", el_state);
            }
            break;
        case INPUT_KEY_USER_ID_SET:
            ESP_LOGI(TAG, "[ * ] [Set] input key event");
            ESP_LOGI(TAG, "[ * ] Stopped, advancing to the next song");
            char *url = NULL;
            audio_pipeline_stop(pipeline);
            audio_pipeline_wait_for_stop(pipeline);
            audio_pipeline_terminate(pipeline);
            sdcard_list_next(sdcard_list_handle, 1, &url);
            ESP_LOGW(TAG, "URL: %s", url);
            audio_element_set_uri(fatfs_stream_reader, url);
            audio_pipeline_reset_ringbuffer(pipeline);
            audio_pipeline_reset_elements(pipeline);
            audio_pipeline_run(pipeline);
            break;
        case INPUT_KEY_USER_ID_VOLUP:
            ESP_LOGI(TAG, "[ * ] [Vol+] input key event");
            player_volume += 10;
            if (player_volume > 100)
            {
                player_volume = 100;
            }
            audio_hal_set_volume(board_handle->audio_hal, player_volume);
            ESP_LOGI(TAG, "[ * ] Volume set to %d %%", player_volume);
            break;
        case INPUT_KEY_USER_ID_VOLDOWN:
            ESP_LOGI(TAG, "[ * ] [Vol-] input key event");
            player_volume -= 10;
            if (player_volume < 0)
            {
                player_volume = 0;
            }
            audio_hal_set_volume(board_handle->audio_hal, player_volume);
            ESP_LOGI(TAG, "[ * ] Volume set to %d %%", player_volume);
            break;
        }
    }

    return ESP_OK;
}

void sdcard_url_save_cb(void *user_data, char *url)
{
    playlist_operator_handle_t sdcard_handle = (playlist_operator_handle_t)user_data;
    esp_err_t ret = sdcard_list_save(sdcard_handle, url);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Fail to save sdcard url to sdcard playlist");
    }
}

void audio_sdcard_task(void *para)
{
    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set(TAG, ESP_LOG_INFO);

    ESP_LOGI(TAG, "[1.0] Initialize peripherals management");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);

    ESP_LOGI(TAG, "[1.1] Initialize and start peripherals");
    audio_board_key_init(set);
    // audio_board_sdcard_init(set, SD_MODE_1_LINE);

    ESP_LOGI(TAG, "[1.2] Set up a sdcard playlist and scan sdcard music save to it");
    sdcard_list_create(&sdcard_list_handle);
    sdcard_scan(sdcard_url_save_cb, "/sdcard", 0, (const char *[]){"mp3"}, 1, sdcard_list_handle);
    sdcard_list_show(sdcard_list_handle);

    ESP_LOGI(TAG, "[ 2 ] Start codec chip");
    board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

    ESP_LOGI(TAG, "[ 3 ] Create and start input key service");
    input_key_service_info_t input_key_info[] = INPUT_KEY_DEFAULT_INFO();
    input_key_service_cfg_t input_cfg = INPUT_KEY_SERVICE_DEFAULT_CONFIG();
    input_cfg.handle = set;
    periph_service_handle_t input_ser = input_key_service_create(&input_cfg);
    input_key_service_add_key(input_ser, input_key_info, INPUT_KEY_NUM);
    periph_service_set_callback(input_ser, input_key_service_cb, (void *)board_handle);

    ESP_LOGI(TAG, "[4.0] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(pipeline);

    ESP_LOGI(TAG, "[4.1] Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.i2s_config.sample_rate = 48000;
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);

    ESP_LOGI(TAG, "[4.2] Create mp3 decoder to decode mp3 file");
    mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    mp3_decoder = mp3_decoder_init(&mp3_cfg);

    /* ZL38063 audio chip on board of ESP32-LyraTD-MSC does not support 44.1 kHz sampling frequency,
       so resample filter has been added to convert audio data to other rates accepted by the chip.
       You can resample the data to 16 kHz or 48 kHz.
    */
    ESP_LOGI(TAG, "[4.3] Create resample filter");
    rsp_filter_cfg_t rsp_cfg = DEFAULT_RESAMPLE_FILTER_CONFIG();
    rsp_handle = rsp_filter_init(&rsp_cfg);

    ESP_LOGI(TAG, "[4.4] Create fatfs stream to read data from sdcard");
    char *url = NULL;
    sdcard_list_current(sdcard_list_handle, &url);
    fatfs_stream_cfg_t fatfs_cfg = FATFS_STREAM_CFG_DEFAULT();
    fatfs_cfg.type = AUDIO_STREAM_READER;
    fatfs_stream_reader = fatfs_stream_init(&fatfs_cfg);
    audio_element_set_uri(fatfs_stream_reader, url);

    ESP_LOGI(TAG, "[4.5] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, fatfs_stream_reader, "file");
    audio_pipeline_register(pipeline, mp3_decoder, "mp3");
    audio_pipeline_register(pipeline, rsp_handle, "filter");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");

    ESP_LOGI(TAG, "[4.6] Link it together [sdcard]-->fatfs_stream-->mp3_decoder-->resample-->i2s_stream-->[codec_chip]");
    const char *link_tag[4] = {"file", "mp3", "filter", "i2s"};
    audio_pipeline_link(pipeline, &link_tag[0], 4);

    ESP_LOGI(TAG, "[5.0] Set up  event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[5.1] Listen for all pipeline events");
    audio_pipeline_set_listener(pipeline, evt);

    ESP_LOGW(TAG, "[ 6 ] Press the keys to control music player:");
    ESP_LOGW(TAG, "      [Play] to start, pause and resume, [Set] next song.");
    ESP_LOGW(TAG, "      [Vol-] or [Vol+] to adjust volume.");

    while (1)
    {
        /* Handle event interface messages from pipeline
           to set music info and to advance to the next song
        */
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT)
        {
            // Set music info for a new song to be played
            if (msg.source == (void *)mp3_decoder && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO)
            {
                audio_element_info_t music_info = {0};
                audio_element_getinfo(mp3_decoder, &music_info);
                ESP_LOGI(TAG, "[ * ] Received music info from mp3 decoder, sample_rates=%d, bits=%d, ch=%d",
                         music_info.sample_rates, music_info.bits, music_info.channels);
                audio_element_setinfo(i2s_stream_writer, &music_info);
                rsp_filter_set_src_info(rsp_handle, music_info.sample_rates, music_info.channels);
                continue;
            }
            // Advance to the next song when previous finishes
            if (msg.source == (void *)i2s_stream_writer && msg.cmd == AEL_MSG_CMD_REPORT_STATUS)
            {
                audio_element_state_t el_state = audio_element_get_state(i2s_stream_writer);
                if (el_state == AEL_STATE_FINISHED)
                {
                    ESP_LOGI(TAG, "[ * ] Finished, advancing to the next song");
                    sdcard_list_next(sdcard_list_handle, 1, &url);
                    ESP_LOGW(TAG, "URL: %s", url);
                    /* In previous versions, audio_pipeline_terminal() was called here. It will close all the elememnt task and when use
                     * the pipeline next time, all the tasks should be restart again. It speed too much time when we switch to another music.
                     * So we use another method to acheive this as below.
                     */
                    audio_element_set_uri(fatfs_stream_reader, url);
                    audio_pipeline_reset_ringbuffer(pipeline);
                    audio_pipeline_reset_elements(pipeline);
                    audio_pipeline_change_state(pipeline, AEL_STATE_INIT);
                    audio_pipeline_run(pipeline);
                }
                continue;
            }
        }
    }

    ESP_LOGI(TAG, "[ 7 ] Stop audio_pipeline");
    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_pipeline_terminate(pipeline);

    audio_pipeline_unregister(pipeline, mp3_decoder);
    audio_pipeline_unregister(pipeline, i2s_stream_writer);
    audio_pipeline_unregister(pipeline, rsp_handle);

    /* Terminate the pipeline before removing the listener */
    audio_pipeline_remove_listener(pipeline);

    /* Stop all peripherals before removing the listener */
    esp_periph_set_stop_all(set);
    audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt);

    /* Make sure audio_pipeline_remove_listener & audio_event_iface_remove_listener are called before destroying event_iface */
    audio_event_iface_destroy(evt);

    /* Release all resources */
    sdcard_list_destroy(sdcard_list_handle);
    audio_pipeline_deinit(pipeline);
    audio_element_deinit(i2s_stream_writer);
    audio_element_deinit(mp3_decoder);
    audio_element_deinit(rsp_handle);
    esp_periph_set_destroy(set);
    periph_service_destroy(input_ser);
}
