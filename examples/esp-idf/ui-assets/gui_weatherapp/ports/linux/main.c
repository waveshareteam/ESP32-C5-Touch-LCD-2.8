/*
 * SPDX-License-Identifier: MIT
 * Copyright 2024 NXP
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lvgl.h"
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"

static void hal_init(void);

lv_ui guider_ui;

#if LV_USE_VIDEO
pthread_t video_thread;
void *videoPlayThread()
{
    for (;;)
    {
        video_play(&guider_ui);
    }
}
#endif

int main(void)
{
    /* Initialize LVGL */
    lv_init();

    /* Initialize the HAL (display, input devices) for LVGL */
    hal_init();

    /* Create a GUI-Guider app */
    setup_ui(&guider_ui);
    events_init(&guider_ui);
    custom_init(&guider_ui);
#if LV_USE_VIDEO
     pthread_create(&video_thread, NULL, videoPlayThread, NULL);
#endif

    uint32_t idle_time;

    /* Handle LVGL tasks */
#if LV_USE_WAYLAND
    while (1) {
        idle_time = lv_wayland_timer_handler();

        usleep(idle_time * 1000);

        /* Run until the last window closes */
        if (!lv_wayland_window_is_open(NULL)) {
            break;
        }
    }
#else
    while(1) {
        /* Return the time to the next timer execution */
        idle_time = lv_timer_handler();
	usleep(idle_time * 1000);
    }
#endif

    return 0;
}

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics library
 */
static void hal_init(void)
{
    lv_display_t * disp;
    lv_indev_t * touch;

#if LV_USE_WAYLAND
    disp = lv_wayland_window_create(LV_HOR_RES_MAX, LV_VER_RES_MAX, "GUI Guider", NULL);
    if(LV_COLOR_DEPTH == 32) {
        lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
    }
    lv_group_t * g = lv_group_create();
    lv_group_set_default(g);
#if LV_USE_GESTURE_RECOGNITION
    touch = lv_wayland_get_touchscreen(disp);
    lv_indev_set_group(touch, g);
    lv_indev_set_pinch_up_threshold(touch, 1.5);
    lv_indev_set_pinch_down_threshold(touch, 0.75);
    lv_indev_set_rotation_rad_threshold(touch, 0.2);
#else
    lv_indev_set_group(lv_wayland_get_pointer(disp), g);
#endif
#elif LV_USE_LINUX_DRM
    disp = lv_linux_drm_create();

#if LV_USE_EVDEV
    touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, LV_EVDEV_DEVICE);
    if(touch != NULL) {
        lv_indev_set_display(touch, disp);

        /* Set the cursor icon */
        LV_IMAGE_DECLARE(mouse_cursor_icon);
        lv_obj_t * cursor_obj = lv_image_create(lv_screen_active());
        lv_image_set_src(cursor_obj, &mouse_cursor_icon);
        lv_indev_set_cursor(touch, cursor_obj);
    } else {
        printf("Warnning: Can't open %s device, please run 'evtest' to check.\n", LV_EVDEV_DEVICE);
    }
#endif

    lv_linux_drm_set_file(disp, LV_LINUX_DRM_CARD, -1);
#else
#error Unsupported Backend
#endif
}
