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



void sensor_setup_scr_start_screen(lv_ui *ui)
{
    //Write codes start_screen
    ui->start_screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->start_screen, 240, 320);
    lv_obj_set_scrollbar_mode(ui->start_screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for start_screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->start_screen, LV_OPA_COVER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->start_screen, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->start_screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    lv_obj_t *start_screen_bg = lv_obj_create(ui->start_screen);
    lv_obj_remove_style_all(start_screen_bg);
    lv_obj_set_pos(start_screen_bg, 0, 0);
    lv_obj_set_size(start_screen_bg, 240, 320);
    lv_obj_set_style_bg_opa(start_screen_bg, LV_OPA_COVER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(start_screen_bg, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_clear_flag(start_screen_bg, LV_OBJ_FLAG_SCROLLABLE);

    //Write codes start_screen_mid_con
    ui->start_screen_mid_con = lv_obj_create(ui->start_screen);
    lv_obj_set_pos(ui->start_screen_mid_con, 0, 30);
    lv_obj_set_size(ui->start_screen_mid_con, 240, 290);
    lv_obj_set_scrollbar_mode(ui->start_screen_mid_con, LV_SCROLLBAR_MODE_OFF);

    //Write style for start_screen_mid_con, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_mid_con, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_mid_con, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_mid_con, LV_OPA_COVER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->start_screen_mid_con, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->start_screen_mid_con, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_mid_con, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_mid_con, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_mid_con, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_mid_con, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_mid_con, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_con2
    ui->start_screen_con2 = lv_obj_create(ui->start_screen_mid_con);
    lv_obj_set_pos(ui->start_screen_con2, 29, 164);
    lv_obj_set_size(ui->start_screen_con2, 181, 108);
    lv_obj_set_scrollbar_mode(ui->start_screen_con2, LV_SCROLLBAR_MODE_OFF);

    //Write style for start_screen_con2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_con2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_con2, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_con2, LV_OPA_COVER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->start_screen_con2, lv_color_hex(0x181818), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->start_screen_con2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_con2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_con2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_con2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_con2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_con2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_z_lab2
    ui->start_screen_z_lab2 = lv_label_create(ui->start_screen_con2);
    lv_label_set_text(ui->start_screen_z_lab2, "");
    lv_label_set_long_mode(ui->start_screen_z_lab2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->start_screen_z_lab2, 100, 78);
    lv_obj_set_size(ui->start_screen_z_lab2, 65, 20);

    //Write style for start_screen_z_lab2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_z_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_z_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->start_screen_z_lab2, lv_color_hex(0x009bff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->start_screen_z_lab2, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->start_screen_z_lab2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->start_screen_z_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->start_screen_z_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->start_screen_z_lab2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_z_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_z_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_z_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_z_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_z_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_z_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_z_lab1
    ui->start_screen_z_lab1 = lv_label_create(ui->start_screen_con2);
    lv_label_set_text(ui->start_screen_z_lab1, "Z");
    lv_label_set_long_mode(ui->start_screen_z_lab1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->start_screen_z_lab1, 40, 77);
    lv_obj_set_size(ui->start_screen_z_lab1, 21, 18);

    //Write style for start_screen_z_lab1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_z_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_z_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->start_screen_z_lab1, lv_color_hex(0x009bff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->start_screen_z_lab1, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->start_screen_z_lab1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->start_screen_z_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->start_screen_z_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->start_screen_z_lab1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_z_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_z_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_z_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_z_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_z_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_z_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_y_lab2
    ui->start_screen_y_lab2 = lv_label_create(ui->start_screen_con2);
    lv_label_set_text(ui->start_screen_y_lab2, "");
    lv_label_set_long_mode(ui->start_screen_y_lab2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->start_screen_y_lab2, 100, 55);
    lv_obj_set_size(ui->start_screen_y_lab2, 55, 15);

    //Write style for start_screen_y_lab2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_y_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_y_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->start_screen_y_lab2, lv_color_hex(0x00ff40), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->start_screen_y_lab2, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->start_screen_y_lab2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->start_screen_y_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->start_screen_y_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->start_screen_y_lab2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_y_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_y_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_y_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_y_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_y_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_y_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_y_lab1
    ui->start_screen_y_lab1 = lv_label_create(ui->start_screen_con2);
    lv_label_set_text(ui->start_screen_y_lab1, "Y");
    lv_label_set_long_mode(ui->start_screen_y_lab1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->start_screen_y_lab1, 41, 53);
    lv_obj_set_size(ui->start_screen_y_lab1, 18, 17);

    //Write style for start_screen_y_lab1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_y_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_y_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->start_screen_y_lab1, lv_color_hex(0x00ff40), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->start_screen_y_lab1, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->start_screen_y_lab1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->start_screen_y_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->start_screen_y_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->start_screen_y_lab1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_y_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_y_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_y_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_y_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_y_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_y_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_x_lab2
    ui->start_screen_x_lab2 = lv_label_create(ui->start_screen_con2);
    lv_label_set_text(ui->start_screen_x_lab2, "    ");
    lv_label_set_long_mode(ui->start_screen_x_lab2, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->start_screen_x_lab2, 100, 32);
    lv_obj_set_size(ui->start_screen_x_lab2, 55, 15);

    //Write style for start_screen_x_lab2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_x_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_x_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->start_screen_x_lab2, lv_color_hex(0xf00000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->start_screen_x_lab2, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->start_screen_x_lab2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->start_screen_x_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->start_screen_x_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->start_screen_x_lab2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_x_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_x_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_x_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_x_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_x_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_x_lab2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_x_lab1
    ui->start_screen_x_lab1 = lv_label_create(ui->start_screen_con2);
    lv_label_set_text(ui->start_screen_x_lab1, "X");
    lv_label_set_long_mode(ui->start_screen_x_lab1, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->start_screen_x_lab1, 35, 32);
    lv_obj_set_size(ui->start_screen_x_lab1, 30, 13);

    //Write style for start_screen_x_lab1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_x_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_x_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->start_screen_x_lab1, lv_color_hex(0xf00000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->start_screen_x_lab1, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->start_screen_x_lab1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->start_screen_x_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->start_screen_x_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->start_screen_x_lab1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_x_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_x_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_x_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_x_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_x_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_x_lab1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_tuoluoyi_lab
    ui->start_screen_tuoluoyi_lab = lv_label_create(ui->start_screen_con2);
    lv_label_set_text(ui->start_screen_tuoluoyi_lab, "GYROSCOPE");
    lv_label_set_long_mode(ui->start_screen_tuoluoyi_lab, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->start_screen_tuoluoyi_lab, 23, 7);
    lv_obj_set_size(ui->start_screen_tuoluoyi_lab, 129, 14);

    //Write style for start_screen_tuoluoyi_lab, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_tuoluoyi_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_tuoluoyi_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->start_screen_tuoluoyi_lab, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->start_screen_tuoluoyi_lab, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->start_screen_tuoluoyi_lab, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->start_screen_tuoluoyi_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->start_screen_tuoluoyi_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->start_screen_tuoluoyi_lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_tuoluoyi_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_tuoluoyi_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_tuoluoyi_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_tuoluoyi_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_tuoluoyi_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_tuoluoyi_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_con1
    ui->start_screen_con1 = lv_obj_create(ui->start_screen_mid_con);
    lv_obj_set_pos(ui->start_screen_con1, 30, 43);
    lv_obj_set_size(ui->start_screen_con1, 181, 108);
    lv_obj_set_scrollbar_mode(ui->start_screen_con1, LV_SCROLLBAR_MODE_OFF);

    //Write style for start_screen_con1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_con1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_con1, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_con1, LV_OPA_COVER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->start_screen_con1, lv_color_hex(0x181818), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->start_screen_con1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_con1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_con1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_con1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_con1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_con1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_shidu_lab
    ui->start_screen_shidu_lab = lv_label_create(ui->start_screen_con1);
    lv_label_set_text(ui->start_screen_shidu_lab, "");
    lv_label_set_long_mode(ui->start_screen_shidu_lab, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->start_screen_shidu_lab, 90, 72);
    lv_obj_set_size(ui->start_screen_shidu_lab, 85, 45);

    //Write style for start_screen_shidu_lab, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->start_screen_shidu_lab, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->start_screen_shidu_lab, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->start_screen_shidu_lab, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->start_screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->start_screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->start_screen_shidu_lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_shidu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_shidu_img
    ui->start_screen_shidu_img = lv_image_create(ui->start_screen_con1);
    lv_obj_add_flag(ui->start_screen_shidu_img, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->start_screen_shidu_img, &_shidu_alpha_40x40);
    lv_image_set_pivot(ui->start_screen_shidu_img, 50,50);
    lv_image_set_rotation(ui->start_screen_shidu_img, 0);
    lv_obj_set_pos(ui->start_screen_shidu_img, 25, 60);
    lv_obj_set_size(ui->start_screen_shidu_img, 40, 40);

    //Write style for start_screen_shidu_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->start_screen_shidu_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->start_screen_shidu_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_shidu_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->start_screen_shidu_img, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_wendu_lab
    ui->start_screen_wendu_lab = lv_label_create(ui->start_screen_con1);
    lv_label_set_text(ui->start_screen_wendu_lab, "");
    lv_label_set_long_mode(ui->start_screen_wendu_lab, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->start_screen_wendu_lab, 90, 21);
    lv_obj_set_size(ui->start_screen_wendu_lab, 85, 45);

    //Write style for start_screen_wendu_lab, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->start_screen_wendu_lab, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->start_screen_wendu_lab, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->start_screen_wendu_lab, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->start_screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->start_screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->start_screen_wendu_lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_wendu_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_wendu_img
    ui->start_screen_wendu_img = lv_image_create(ui->start_screen_con1);
    lv_obj_add_flag(ui->start_screen_wendu_img, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->start_screen_wendu_img, &_wendu_alpha_40x40);
    lv_image_set_pivot(ui->start_screen_wendu_img, 50,50);
    lv_image_set_rotation(ui->start_screen_wendu_img, 0);
    lv_obj_set_pos(ui->start_screen_wendu_img, 24, 10);
    lv_obj_set_size(ui->start_screen_wendu_img, 40, 40);

    //Write style for start_screen_wendu_img, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->start_screen_wendu_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->start_screen_wendu_img, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_wendu_img, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_clip_corner(ui->start_screen_wendu_img, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_line_1
    ui->start_screen_line_1 = lv_line_create(ui->start_screen_con1);
    static lv_point_precise_t start_screen_line_1[] = {{0, 0},{181, 0},};
    lv_line_set_points(ui->start_screen_line_1, start_screen_line_1, 2);
    lv_obj_set_pos(ui->start_screen_line_1, 0, 53);
    lv_obj_set_size(ui->start_screen_line_1, 185, 6);

    //Write style for start_screen_line_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_line_width(ui->start_screen_line_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->start_screen_line_1, lv_color_hex(0x757575), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->start_screen_line_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_line_rounded(ui->start_screen_line_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

    
    //Write codes start_screen_time_lab
    ui->start_screen_time_lab = lv_label_create(ui->start_screen_mid_con);
    lv_label_set_text(ui->start_screen_time_lab, "");
    lv_label_set_long_mode(ui->start_screen_time_lab, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->start_screen_time_lab, 6, 6);
    lv_obj_set_size(ui->start_screen_time_lab, 140, 40);

    //Write style for start_screen_time_lab, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_time_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_time_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->start_screen_time_lab, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->start_screen_time_lab, &lv_font_montserratMedium_30, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->start_screen_time_lab, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->start_screen_time_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->start_screen_time_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->start_screen_time_lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_time_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_time_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_time_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_time_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_time_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_time_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_top_con
    ui->start_screen_top_con = lv_obj_create(ui->start_screen);
    lv_obj_set_pos(ui->start_screen_top_con, 0, 0);
    lv_obj_set_size(ui->start_screen_top_con, 240, 30);
    lv_obj_set_scrollbar_mode(ui->start_screen_top_con, LV_SCROLLBAR_MODE_OFF);

    //Write style for start_screen_top_con, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_top_con, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_top_con, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_top_con, LV_OPA_COVER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->start_screen_top_con, lv_color_hex(0x212121), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->start_screen_top_con, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_top_con, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_top_con, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_top_con, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_top_con, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_top_con, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes start_screen_data_lab
    ui->start_screen_data_lab = lv_label_create(ui->start_screen_top_con);
    lv_label_set_text(ui->start_screen_data_lab, "");
    lv_label_set_long_mode(ui->start_screen_data_lab, LV_LABEL_LONG_WRAP);
    lv_obj_set_pos(ui->start_screen_data_lab, 0, 5);
    lv_obj_set_size(ui->start_screen_data_lab, 122, 19);

    //Write style for start_screen_data_lab, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->start_screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->start_screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->start_screen_data_lab, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->start_screen_data_lab, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->start_screen_data_lab, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->start_screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->start_screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->start_screen_data_lab, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->start_screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->start_screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->start_screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->start_screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->start_screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->start_screen_data_lab, 0, LV_PART_MAIN|LV_STATE_DEFAULT);


    //The custom code of start_screen.

    //Update current screen layout.
    lv_obj_update_layout(ui->start_screen);

}
