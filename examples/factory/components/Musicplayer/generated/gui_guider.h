/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

// 结构体类型名可以不变，实例名必须改！
typedef struct
{
	lv_obj_t *screen;
	bool screen_del;
	lv_obj_t *screen_music_img;
	lv_obj_t *screen_button_con;
	lv_obj_t *screen_left_btn;
	lv_obj_t *screen_left_btn_label;
	lv_obj_t *screen_go_btn;
	lv_obj_t *screen_go_btn_label;
	lv_obj_t *screen_right_btn;
	lv_obj_t *screen_right_btn_label;
	lv_obj_t *screen_label_1;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

// ==============================================
// 所有函数 统一加 music_ 前缀（关键！）
// ==============================================
void music_ui_init_style(lv_style_t * style);
void music_ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);
void music_ui_animation(void * var, uint32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                  uint32_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                  lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_completed_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);
void music_init_scr_del_flag(lv_ui *ui);
void music_setup_bottom_layer(void);
void music_setup_ui(lv_ui *ui);  // 你已经改对了
void music_video_play(lv_ui *ui);
void music_init_keyboard(lv_ui *ui);

// ==============================================
// 全局变量重命名！！（解决编译重定义错误）
// ==============================================
extern lv_ui music_guider_ui;

// ==============================================
// 界面初始化函数 也必须加前缀
// ==============================================
void music_setup_scr_screen(lv_ui *ui);

// 图片/字体声明 不需要修改
LV_IMAGE_DECLARE(_music_RGB565A8_145x145);
LV_IMAGE_DECLARE(_left_RGB565A8_35x35);
LV_IMAGE_DECLARE(_stop_RGB565A8_35x35);
LV_IMAGE_DECLARE(_go_RGB565A8_35x35);
LV_IMAGE_DECLARE(_right_RGB565A8_35x35);

LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_montserratMedium_12)
LV_FONT_DECLARE(lv_font_montserratMedium_30)

#ifdef __cplusplus
}
#endif
#endif