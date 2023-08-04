/*
 * @Author: Kevincoooool
 * @Date: 2021-11-29 14:15:49
 * @Description: 
 * @version:  
 * @Filename: Do not Edit
 * @LastEditTime: 2021-11-29 18:08:21
 * @FilePath: \s3_lvgl_v7\components\lv_lib_split_jpg\lv_sjpg.h
 */
/**
 * @file lv_sjpg.h
 *
 */

#ifndef _LV_SJPEG_
#define _LV_SJPEG_

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stdio.h>
#include "tjpgd.h"

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include <lvgl.h>
#else
#include <lvgl/lvgl.h>
#endif

/*********************
 *      DEFINES
 *********************/

/* Scaling factor */
// #define LV_TJPGD_CONFIG_SCALING_FACTOR_NONE
//#define LV_TJPGD_CONFIG_SCALING_FACTOR_1_2
#define LV_TJPGD_CONFIG_SCALING_FACTOR_1_4
//#define LV_TJPGD_CONFIG_SCALING_FACTOR_1_8

/* When setting scaling factor to none we should set JD_USE_SCALE to 0
 * set it to 1 otherwise */
#if defined (LV_TJPGD_CONFIG_SCALING_FACTOR_NONE)
    #define LV_TJPGD_SCALING_FACTOR     0
    #define LV_TJPGD_SCALING_FACTOR_DIV 1
#elif defined (LV_TJPGD_CONFIG_SCALING_FACTOR_1_2)
    #define LV_TJPGD_SCALING_FACTOR     1
    #define LV_TJPGD_SCALING_FACTOR_DIV 2
#elif defined (LV_TJPGD_CONFIG_SCALING_FACTOR_1_4)
    #define LV_TJPGD_SCALING_FACTOR     2
    #define LV_TJPGD_SCALING_FACTOR_DIV 4
#elif defined (LV_TJPGD_CONFIG_SCALING_FACTOR_1_8)
    #define LV_TJPGD_SCALING_FACTOR     3
    #define LV_TJPGD_SCALING_FACTOR_DIV 8
#else
    #error "Invalid TJPGD scaling factor configuration"
#endif // defined


/**********************
 *      TYPEDEFS
 **********************/
enum io_source_type {
    SJPEG_IO_SOURCE_C_ARRAY,
    SJPEG_IO_SOURCE_DISK,
};

typedef struct {
    enum io_source_type type;
#if LV_USE_FILESYSTEM
    lv_fs_file_t lv_file;
#endif
    uint8_t* img_cache_buff;
    int img_cache_x_res;
    int img_cache_y_res;
    uint8_t *raw_sjpg_data;               //Used when type==SJPEG_IO_SOURCE_C_ARRAY.
    uint32_t raw_sjpg_data_size;          //Num bytes pointed to by raw_sjpg_data.
    uint32_t raw_sjpg_data_next_read_pos; //Used for all types.
} io_source_t;


typedef struct {
    uint8_t *sjpeg_data;
    uint32_t sjpeg_data_size;
    int sjpeg_x_res;
    int sjpeg_y_res;
    int sjpeg_total_frames;
    int sjpeg_single_frame_height;
    int sjpeg_cache_frame_index;
    uint8_t **frame_base_array;         //to save base address of each split frames upto sjpeg_total_frames.
    int *frame_base_offset;             //to save base offset for fseek
    uint8_t *frame_cache;
    uint8_t* workb;                     //JPG work buffer for jpeg library
    JDEC *tjpeg_jd;
    io_source_t io;
} SJPEG;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_split_jpeg_init(void);

/**********************
 *      MACROS
 **********************/



#ifdef __cplusplus
}
#endif

#endif /* _LV_JPEG_WRAPPER */