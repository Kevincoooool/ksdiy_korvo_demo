/*
 * @Author: Kevincoooool
 * @Date: 2021-12-06 16:24:39
 * @Description: 
 * @version:  
 * @Filename: Do not Edit
 * @LastEditTime: 2021-12-06 16:33:21
 * @FilePath: \6.lcd_camera_lvgl_v7\main\page_cam.h
 */
/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-07-07 10:39:37
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_cam.h
 */


#ifndef _page_cam_
#define _page_cam_

#ifdef __cplusplus
extern "C" {
#endif
/*********************
* INCLUDES
*********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lv_ex_conf.h"
#else
#include "../../lvgl/lvgl.h"
#endif
extern uint8_t cam_en, color_en, face_en;
void Cam_Task(void *pvParameters);
void page_cam_load(void);
void page_cam_end(void);

#ifdef __cplusplus
} /* extern "C" */
#endif




#endif // _TEST_


