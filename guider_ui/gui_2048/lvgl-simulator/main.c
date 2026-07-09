/*
* Copyright 2024-2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#define SDL_MAIN_HANDLED        /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include "lvgl/lvgl.h"
#include "gui_guider.h"
#include "custom.h"
#include "widgets_init.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#if LV_USE_FREEMASTER
#include "external_data_init.h"
#endif

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

/*********************
 *      DEFINES
 *********************/
#ifndef USE_KEYBOARD
#define USE_KEYBOARD 0
#endif

#ifndef LV_GUIDER_SIMULATOR_WINDOW_ZOOM
#define LV_GUIDER_SIMULATOR_WINDOW_ZOOM 1
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_display_t * hal_init(int32_t w, int32_t h);
volatile int keep_running = 0;

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/
extern uint8_t simulator_icon[];
lv_ui guider_ui;

#if LV_USE_FREEMASTER
pthread_t thread[2];
pthread_mutex_t jsonrpc_mutex;
pthread_mutex_t lvgl_mutex;
pthread_mutex_t gg_edata_ll_mutex;
pthread_cond_t gg_edata_ll_cond;
#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int main(int argc, char **argv)
{
#if LV_USE_FREEMASTER
    if(argc > 1)
    {
        int sim_conf(int argc, char *argv[]);
        sim_conf(argc, argv);
    }
#endif

    /* local threads will run */
    keep_running = 1;
    /*Initialize LVGL*/
    lv_init();

    /*Initialize the HAL (display, input devices, tick) for LVGL*/
    hal_init(LV_HOR_RES_MAX, LV_VER_RES_MAX);

#if LV_USE_FREEMASTER
    pthread_mutex_init(&gg_edata_ll_mutex, NULL);
    pthread_cond_init(&gg_edata_ll_cond, NULL);
    /*Initialize the external data */
    external_task_init(&guider_ui);
#endif

    setup_ui(&guider_ui);
    custom_init(&guider_ui);
#if LV_USE_FREEMASTER
    pthread_mutex_init(&jsonrpc_mutex, NULL);
    pthread_mutex_init(&lvgl_mutex, NULL);
    memset(&thread, 0, sizeof(thread));
    /*Create a separate thread to loop the linked list*/
    pthread_create(&thread[0], NULL, gg_edata_task_exec, &jsonrpc_mutex);
#endif

    while(keep_running) {
#if LV_USE_FREEMASTER
        pthread_mutex_lock(&lvgl_mutex);
        process_cached_widget_data();
#endif
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        lv_timer_handler();
#if LV_USE_VIDEO
        video_play(&guider_ui);
#endif
#if LV_USE_FREEMASTER
        pthread_mutex_unlock(&lvgl_mutex);
#endif
        usleep(5 * 1000);
    }

#if LV_USE_FREEMASTER
    /* wakeup gg_edata_task_exec thread if it sleeps, let it finish */
    pthread_cond_signal(&gg_edata_ll_cond);
    /* wait for the gg_edata_task_exec thread*/
    pthread_join(thread[0], NULL);

    freemaster_disconnect();
#endif

    SDL_Quit();
    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static lv_display_t * hal_init(int32_t w, int32_t h)
{

    lv_group_set_default(lv_group_create());

    lv_display_t * disp = lv_sdl_window_create(w, h);
    lv_sdl_window_set_title(disp, "Simulator (C/C++)");
    lv_sdl_window_set_icon(disp, simulator_icon, 32, 32);
    lv_sdl_window_set_resizeable(disp, false);
    lv_sdl_window_set_zoom(disp, LV_GUIDER_SIMULATOR_WINDOW_ZOOM);

    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_set_group(mouse, lv_group_get_default());
    lv_indev_set_display(mouse, disp);
    lv_display_set_default(disp);

#if USE_KEYBOARD
    lv_indev_t * keyboard = lv_sdl_keyboard_create();
    lv_indev_set_display(keyboard, disp);
    lv_indev_set_group(keyboard, lv_group_get_default());
#endif

    return disp;
}
#if LV_USE_FREEMASTER
/**
 * Enable simulator to be linked and used as DLL. This function is invoked by 
 * FreeMASTER from its background thread. This function must finish when 
 * sim_exit() is called asynchronously.
 */
__declspec(dllexport)
int sim_main(int argc, char *argv[])
{
    int ret = main(argc, argv);
    return ret;
}

/**
 * Finish all running threads and terminate the application running as sim_main().
 */
__declspec(dllexport)
int sim_exit(void)
{
    keep_running = 0;
    return 0;
}

/**
 * Enable FreeMASTER to configure the simulator before starting it.
 * This function is the first one called by the FreeMASTER from its GUI thread. 
 */
__declspec(dllexport)
int sim_conf(int argc, char *argv[])
{
    /* Process the arguments */
    if(argc > 0 && argv != NULL)
    {
        for(int i=0; i<argc; i++)
        {
            /* --rpcs is used to specify FreeMASTER JSON-RPC server address. */
            if(strcmp(argv[i], "--rpcs") == 0)
            {
                i++;
                if(i<argc && strncmp(argv[i], "ws://", 5) == 0)
                {
                    freemaster_server = strdup(argv[i]);
                }
            }
        }
    }
    return 0;
}

/**
 * Enable host application to retrieve DLL information
 */
__declspec(dllexport)
const void * sim_info(void)
{
    #define SIM_F_RESIZE_UP        0x0001   /* Enable growing size within parent view */
    #define SIM_F_RESIZE_DOWN      0x0002   /* Enable shrinking size within parent view */
    #define SIM_F_KEEP_ASPECT      0x0004   /* When RESIZE used, preserve aspect ratio */
    #define SIM_F_CENTER_H         0x0008   /* Center horizontally */
    #define SIM_F_CENTER_V         0x0010   /* Center vertically */
    #define SIM_F_CENTER_FORCE     0x0020   /* Keep centered even if parent is smaller (move beyond top/left border) */
    #define SIM_F_AUTO_BACKCOLOR   0x0040   /* Determine background color automatically */

    typedef struct
    {
        uint32_t sz;
        const char* name;
        const char* desc;
        const char* copyright;
        const char* server;
        uint32_t width;
        uint32_t height;
        uint32_t flags;
        uint32_t background;
        uint32_t reserved;
    } sim_info_t;

    /* Constant options: */
    static sim_info_t info =
    {
        .sz = sizeof(sim_info_t),
        .name = "NXP GUI-Guider Project",
        .desc = "Designed to enable GUI designs created in GG to be hosted in FreeMASTER",
        .copyright = "Copyright 2024 NXP",
        .server = "",
        .width = LV_HOR_RES_MAX,
        .height = LV_VER_RES_MAX,
        .flags = SIM_F_CENTER_H,
        .background = 0,
    };

    /* Runtime-defined options: */
    info.server = freemaster_server;

    return &info;
}

#endif
