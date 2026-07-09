/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"


typedef struct
{
  
	lv_obj_t *Game2048;
	bool Game2048_del;
	lv_obj_t *Game2048_btnm_2048;
	lv_obj_t *Game2048_label_2048;
	lv_obj_t *Game2048_btn_new_game;
	lv_obj_t *Game2048_btn_new_game_label;
	lv_obj_t *Game2048_label_score_title;
	lv_obj_t *Game2048_label_score;
	lv_obj_t *Game2048_label_best_title;
	lv_obj_t *Game2048_label_best;
	lv_obj_t *Game2048_cont_msgbox;
	lv_obj_t *Game2048_label_warning;
	lv_obj_t *Game2048_label_results_title;
	lv_obj_t *Game2048_btn_again;
	lv_obj_t *Game2048_btn_again_label;
	lv_obj_t *Game2048_label_results;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, uint32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                  uint32_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                  lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_completed_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_bottom_layer(void);

void setup_ui(lv_ui *ui);

void video_play(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_Game2048(lv_ui *ui);

LV_FONT_DECLARE(lv_font_Alatsi_Regular_10)
LV_FONT_DECLARE(lv_font_Alatsi_Regular_30)
LV_FONT_DECLARE(lv_font_Acme_Regular_10)
LV_FONT_DECLARE(lv_font_Alatsi_Regular_11)
LV_FONT_DECLARE(lv_font_Alatsi_Regular_20)
LV_FONT_DECLARE(lv_font_Alatsi_Regular_13)


#ifdef __cplusplus
}
#endif
#endif
