#include <lvgl/lvgl.h>
#include "lv_lib_split_jpg/lv_sjpg.h"

LV_IMG_DECLARE( small_image_sjpg );
LV_IMG_DECLARE( wallpaper_jpg );

#if defined(_WIN32)
#define OS_WINDOWS
#endif

#if defined(OS_WINDOWS)
static const char kTestFile[] = "S.\\lv_lib_split_jpg\\example\\images\\small_image.sjpg";
#else
static const char kTestFile[] = "S/lv_lib_split_jpg/example/images/small_image.sjpg";
#endif

/*Change to your screen resolution*/
#define screenWidth  320
#define screenHeight 240

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * 10 ];

static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  lv_disp_flush_ready(disp);
}

void demo_jpg_sjpg( void )
{
  lv_obj_t * img1;
  lv_obj_t * img2;

  lv_split_jpeg_init();
  img1 = lv_img_create(lv_scr_act());
  img2 = lv_img_create(lv_scr_act());

  //jpg from c array
  lv_img_set_src(img1, &wallpaper_jpg);

  //sjpg from file (with lv_fs)
  lv_img_set_src(img2, kTestFile);
}

#if defined(LV_LIB_SJPG_TEST)

int main(int argc, const char **argv)
{
  lv_init();

  //Initialize the display.
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);

  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  demo_jpg_sjpg();
}

#endif  // defined(LV_LIB_SJPG_TEST)

