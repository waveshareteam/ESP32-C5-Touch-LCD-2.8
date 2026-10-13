/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

#include "custom.h"

static void Game2048_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_GESTURE:
    {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        switch(dir) {
        case LV_DIR_LEFT:
        {
            lv_indev_wait_release(lv_indev_active());
            movement_check(&guider_ui, GG_2048_MOVE_LEFT);
            break;
        }
        case LV_DIR_RIGHT:
        {
            lv_indev_wait_release(lv_indev_active());
            movement_check(&guider_ui, GG_2048_MOVE_RIGHT);
            break;
        }
        case LV_DIR_TOP:
        {
            lv_indev_wait_release(lv_indev_active());
            movement_check(&guider_ui, GG_2048_MOVE_UP);
            break;
        }
        case LV_DIR_BOTTOM:
        {
            lv_indev_wait_release(lv_indev_active());
            movement_check(&guider_ui, GG_2048_MOVE_DOWN);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

static void Game2048_btn_new_game_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        new_game(&guider_ui);
        break;
    }
    default:
        break;
    }
}

static void Game2048_btn_again_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_add_flag(guider_ui.Game2048_cont_msgbox, LV_OBJ_FLAG_HIDDEN);
        new_game(&guider_ui);
        break;
    }
    default:
        break;
    }
}

void events_init_Game2048 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->Game2048, Game2048_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->Game2048_btn_new_game, Game2048_btn_new_game_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->Game2048_btn_again, Game2048_btn_again_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
