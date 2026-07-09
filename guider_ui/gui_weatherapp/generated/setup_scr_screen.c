/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_screen(lv_ui *ui)
{
    //Write codes screen
    ui->screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen, 240, 320);
    lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->screen, &_bk1_RGB565A8_240x320, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->screen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_img
    ui->screen_weather_img = lv_image_create(ui->screen);
    lv_obj_set_pos(ui->screen_weather_img, 10, 60);
    lv_obj_set_size(ui->screen_weather_img, 92, 84);
    lv_obj_add_flag(ui->screen_weather_img, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_weather_img, &_Cloudy_100x100_RGB565A8_92x84);
    lv_image_set_pivot(ui->screen_weather_img, 50,50);
    lv_image_set_rotation(ui->screen_weather_img, 0);

    //Write style for screen_weather_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_weather_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_weather_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_diqu_lab
    ui->screen_diqu_lab = lv_label_create(ui->screen);
    lv_obj_set_pos(ui->screen_diqu_lab, 71, 12);
    lv_obj_set_size(ui->screen_diqu_lab, 104, 22);
    lv_label_set_text(ui->screen_diqu_lab, "ShenZhen");
    lv_label_set_long_mode(ui->screen_diqu_lab, LV_LABEL_LONG_WRAP);

    //Write style for screen_diqu_lab, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_diqu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_diqu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_diqu_lab, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_diqu_lab, &lv_font_FontAwesome5_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_diqu_lab, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_diqu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_diqu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_diqu_lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_diqu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_diqu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_diqu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_diqu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_diqu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_diqu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_data_lab
    ui->screen_data_lab = lv_label_create(ui->screen);
    lv_obj_set_pos(ui->screen_data_lab, 106, 82);
    lv_obj_set_size(ui->screen_data_lab, 121, 18);
    lv_label_set_text(ui->screen_data_lab, "2026-04-22\n\n");
    lv_label_set_long_mode(ui->screen_data_lab, LV_LABEL_LONG_WRAP);

    //Write style for screen_data_lab, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_data_lab, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_data_lab, &lv_font_FontAwesome5_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_data_lab, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_data_lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_weather_lab
    ui->screen_weather_lab = lv_label_create(ui->screen);
    lv_obj_set_pos(ui->screen_weather_lab, 129, 114);
    lv_obj_set_size(ui->screen_weather_lab, 79, 20);
    lv_label_set_text(ui->screen_weather_lab, "Sunny");
    lv_label_set_long_mode(ui->screen_weather_lab, LV_LABEL_LONG_WRAP);

    //Write style for screen_weather_lab, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_weather_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_weather_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_weather_lab, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_weather_lab, &lv_font_FontAwesome5_18, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_weather_lab, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_weather_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_weather_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_weather_lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_weather_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_weather_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_weather_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_weather_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_weather_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_weather_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wendu_lab
    ui->screen_wendu_lab = lv_label_create(ui->screen);
    lv_obj_set_pos(ui->screen_wendu_lab, 59, 191);
    lv_obj_set_size(ui->screen_wendu_lab, 52, 24);
    lv_label_set_text(ui->screen_wendu_lab, "28℃");
    lv_label_set_long_mode(ui->screen_wendu_lab, LV_LABEL_LONG_WRAP);

    //Write style for screen_wendu_lab, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_wendu_lab, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_wendu_lab, &lv_font_FontAwesome5_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_wendu_lab, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_wendu_lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_wendu_img
    ui->screen_wendu_img = lv_image_create(ui->screen);
    lv_obj_set_pos(ui->screen_wendu_img, 10, 180);
    lv_obj_set_size(ui->screen_wendu_img, 40, 40);
    lv_obj_add_flag(ui->screen_wendu_img, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_wendu_img, &_wendu_RGB565A8_40x40);
    lv_image_set_pivot(ui->screen_wendu_img, 50,50);
    lv_image_set_rotation(ui->screen_wendu_img, 0);

    //Write style for screen_wendu_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_wendu_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_wendu_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_shidu_lab
    ui->screen_shidu_lab = lv_label_create(ui->screen);
    lv_obj_set_pos(ui->screen_shidu_lab, 65, 262);
    lv_obj_set_size(ui->screen_shidu_lab, 43, 14);
    lv_label_set_text(ui->screen_shidu_lab, "20%");
    lv_label_set_long_mode(ui->screen_shidu_lab, LV_LABEL_LONG_WRAP);

    //Write style for screen_shidu_lab, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_shidu_lab, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_shidu_lab, &lv_font_FontAwesome5_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_shidu_lab, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_shidu_lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_shidu_img
    ui->screen_shidu_img = lv_image_create(ui->screen);
    lv_obj_set_pos(ui->screen_shidu_img, 10, 249);
    lv_obj_set_size(ui->screen_shidu_img, 49, 40);
    lv_obj_add_flag(ui->screen_shidu_img, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_shidu_img, &_shidu_RGB565A8_49x40);
    lv_image_set_pivot(ui->screen_shidu_img, 50,50);
    lv_image_set_rotation(ui->screen_shidu_img, 0);

    //Write style for screen_shidu_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_shidu_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_shidu_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_fengsu_img
    ui->screen_fengsu_img = lv_image_create(ui->screen);
    lv_obj_set_pos(ui->screen_fengsu_img, 123, 249);
    lv_obj_set_size(ui->screen_fengsu_img, 40, 40);
    lv_obj_add_flag(ui->screen_fengsu_img, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_fengsu_img, &_windspeed_RGB565A8_40x40);
    lv_image_set_pivot(ui->screen_fengsu_img, 50,50);
    lv_image_set_rotation(ui->screen_fengsu_img, 0);

    //Write style for screen_fengsu_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_fengsu_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_fengsu_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_fengsu_lab
    ui->screen_fengsu_lab = lv_label_create(ui->screen);
    lv_obj_set_pos(ui->screen_fengsu_lab, 167, 262);
    lv_obj_set_size(ui->screen_fengsu_lab, 71, 20);
    lv_label_set_text(ui->screen_fengsu_lab, "100km/h\n");
    lv_label_set_long_mode(ui->screen_fengsu_lab, LV_LABEL_LONG_WRAP);

    //Write style for screen_fengsu_lab, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_fengsu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_fengsu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_fengsu_lab, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_fengsu_lab, &lv_font_FontAwesome5_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_fengsu_lab, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_fengsu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_fengsu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_fengsu_lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_fengsu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_fengsu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_fengsu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_fengsu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_fengsu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_fengsu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_fengxiang_img
    ui->screen_fengxiang_img = lv_image_create(ui->screen);
    lv_obj_set_pos(ui->screen_fengxiang_img, 123, 180);
    lv_obj_set_size(ui->screen_fengxiang_img, 40, 40);
    lv_obj_add_flag(ui->screen_fengxiang_img, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->screen_fengxiang_img, &_wind_RGB565A8_40x40);
    lv_image_set_pivot(ui->screen_fengxiang_img, 50,50);
    lv_image_set_rotation(ui->screen_fengxiang_img, 0);

    //Write style for screen_fengxiang_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_fengxiang_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_fengxiang_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_fengxiang_lab
    ui->screen_fengxiang_lab = lv_label_create(ui->screen);
    lv_obj_set_pos(ui->screen_fengxiang_lab, 167, 191);
    lv_obj_set_size(ui->screen_fengxiang_lab, 56, 17);
    lv_label_set_text(ui->screen_fengxiang_lab, "10°");
    lv_label_set_long_mode(ui->screen_fengxiang_lab, LV_LABEL_LONG_WRAP);

    //Write style for screen_fengxiang_lab, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->screen_fengxiang_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_fengxiang_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_fengxiang_lab, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_fengxiang_lab, &lv_font_FontAwesome5_15, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_fengxiang_lab, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->screen_fengxiang_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->screen_fengxiang_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_fengxiang_lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->screen_fengxiang_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->screen_fengxiang_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->screen_fengxiang_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->screen_fengxiang_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->screen_fengxiang_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_fengxiang_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen);

}
