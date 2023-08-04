/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-04 16:13:33
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2022-08-25 09:17:41
 * @FilePath: \Korvo_Demo\10.adc_button_test\components\adc\bsp_adc.c
 */
#include "bsp_adc.h"

#define TAG "BSP_ADC"

#define DEFAULT_VREF 3300
#define NO_OF_SAMPLES 32 //Multisampling

static const adc_channel_t channel = ADC_CHANNEL_4; //GPIO1 if ADC1
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
static const adc_atten_t atten = ADC_ATTEN_DB_11;

static esp_adc_cal_characteristics_t adc_chars;
// static const adc_unit_t unit = ADC_UNIT_1;
//ADC Attenuation
#define ADC_EXAMPLE_ATTEN ADC_ATTEN_DB_11

//ADC Calibration
#if CONFIG_IDF_TARGET_ESP32
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_VREF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_EXAMPLE_CALI_SCHEME ESP_ADC_CAL_VAL_EFUSE_TP_FIT
#endif
bool cali_enable = false;
uint16_t voltage = 0;
uint16_t read_raw = 0;
float read_F = 0;
static bool adc_calibration_init(void)
{
    esp_err_t ret;
    cali_enable = false;

    ret = esp_adc_cal_check_efuse(ADC_EXAMPLE_CALI_SCHEME);
    if (ret == ESP_ERR_NOT_SUPPORTED)
    {
        ESP_LOGW(TAG, "Calibration scheme not supported, skip software calibration");
    }
    else if (ret == ESP_ERR_INVALID_VERSION)
    {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    }
    else if (ret == ESP_OK)
    {
        cali_enable = true;
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_EXAMPLE_ATTEN, ADC_WIDTH_BIT_12, 0, &adc_chars);
        // esp_adc_cal_characterize(ADC_UNIT_2, ADC_EXAMPLE_ATTEN, ADC_WIDTH_BIT_12, 0, &adc2_chars);
    }
    else
    {
        ESP_LOGE(TAG, "Invalid arg");
    }

    return cali_enable;
}
void adc_init()
{

    cali_enable = adc_calibration_init();

    esp_err_t r;

    gpio_num_t adc_gpio_num;
    adc1_config_width(width);
    r = adc1_pad_get_io_num(channel, &adc_gpio_num);
    assert(r == ESP_OK);

    printf("ADC1 channel %d @ GPIO %d\n", channel, adc_gpio_num);

    //be sure to do the init before using adc2.
    printf("adc1_init...\n");
    adc1_config_channel_atten(channel, atten);

    vTaskDelay(200 / portTICK_PERIOD_MS);

    printf("start conversion.\n");
    
}
uint16_t get_adc(void)
{
    // uint16_t read_raw;
    // uint16_t read_F = 0;
    //Multisampling
    // for (int i = 0; i < NO_OF_SAMPLES; i++)
    // {
    read_raw = adc1_get_raw(channel); //从单个通道获取ADC1读数。
    //     read_raw += read_raw;
    // }
    // read_raw /= NO_OF_SAMPLES;
    // ESP_LOGI(TAG, "raw  data: %d", read_raw);
    // if (cali_enable)
    // {
    //     voltage = esp_adc_cal_raw_to_voltage(read_raw, &adc_chars);
    //     // ESP_LOGI(TAG, "cali data: %d mV", voltage);
    // }

    // LPF_1_(0.2f, 20 * 1e-3f, read_raw, read_F);
    return read_raw;
}
uint16_t get_adc_vol(void)
{
    // uint16_t read_raw;
    // uint16_t read_F = 0;
    //Multisampling
    // for (int i = 0; i < NO_OF_SAMPLES; i++)
    // {
    read_raw = adc1_get_raw(channel); //从单个通道获取ADC1读数。
    //     read_raw += read_raw;
    // }
    // read_raw /= NO_OF_SAMPLES;
    // ESP_LOGI(TAG, "raw  data: %d", read_raw);
    if (cali_enable)
    {
        voltage = esp_adc_cal_raw_to_voltage(read_raw, &adc_chars);
        ESP_LOGI(TAG, "cali data: %d mV", voltage);
    }

    // LPF_1_(0.2f, 20 * 1e-3f, read_raw, read_F);
    return read_raw;
}

uint16_t touch_chart[][3] =
    {
        // {0, 60, 65},
        // {1, 99, 102},
        // {2, 70, 75},

        // {0, 80, 85},
        // {1, 39, 43},
        // {2, 60, 65},
        // {3, 15, 21},
        // {4, 105, 108},

        // {0, 165, 170},
        // {1, 79, 85},
        // {2, 120, 125},
        // {3, 36, 42},
        // {4, 210, 220},

        // {0, 1900, 2200}, //1键  2.25v
        // {1, 2600, 2900}, //2键  1.12v
        // {2, 3000, 3150}, //3键  1.689v
        // {3, 3200, 3400}, //  4下键  0.562v
        // {4, 3450, 3600}, //5键  2.81v
        {0, 280, 350}, //1键  2.25v     315
        {1, 780, 900}, //2键  1.12v     820
        {2, 1100, 1400}, //3键  1.689v  1290
        {3, 1500, 1700}, //4键  0.562v  1460
        {4, 2300, 2600}, //5键  2.81v   2480

};

uint8_t Get_Adc_Button()
{
    // int adc_value = get_adc();
    // static uint64_t last_time = 0;
    for (uint8_t i = 0; i < 5; i++)
    {
        // if (adc_value >= touch_chart[i][1] && adc_value <= touch_chart[i][2] && esp_timer_get_time() - last_time > 200000) //限制进入时间
        if (adc1_get_raw(channel) >= touch_chart[i][1] && adc1_get_raw(channel) <= touch_chart[i][2])
        {
            // printf("adc_button: %d\n", i + 1);
            // last_time = esp_timer_get_time();
            return i + 1;
        }
    }
    return 0;
}
