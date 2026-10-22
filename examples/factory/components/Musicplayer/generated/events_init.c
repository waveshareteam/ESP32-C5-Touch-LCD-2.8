#include "events_init.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "bsp_board_extra.h"
#include "esp_log.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"

static const char *TAG = "MUSIC_EVENT";

// ====================== 音乐模块 全局变量 ======================
static generic_file_list_t MP3_files = {0};
static uint16_t file_count = 0;
static lv_anim_t *g_rot_anim = NULL;
static volatile bool g_is_playing = false;
static int32_t g_current_angle = 0;
static lv_obj_t *g_music_img = NULL;
static lv_obj_t *g_play_btn = NULL;
static bool has_started_play = false;
static lv_ui *g_ui = NULL;
static bool g_replay_block_flag = false; // 新增：重播屏蔽标志

static TaskHandle_t g_music_task_handle = NULL;
static volatile bool g_music_stop = false;
static volatile bool g_music_running = false;
static lv_timer_t *g_music_timer = NULL;

// 修复：只用标志位，任务绝不碰 LVGL
static volatile bool music_need_stop_flag = false;

// ====================== 函数声明 ======================
static void mp3_file_scan(void);
static esp_gmf_err_t mp3_play_first(void);
static void screen_start_music_rotate(lv_ui *ui);
static void screen_stop_music_rotate(void);
static void event_go_btn_clicked(lv_event_t *e);
static void music_control_task(void *arg);
static void music_state_reset(void);
static void music_finish_handler(lv_timer_t *timer);

// ====================== 全局状态重置 ======================
static void music_state_reset(void)
{
    screen_stop_music_rotate();
    g_is_playing = false;
    has_started_play = false;
    g_current_angle = 0;
    music_need_stop_flag = false;
    g_replay_block_flag = false;
    Audio_Stop_Play();
    ESP_LOGI(TAG, "✅ Music state reset success");
}


void music_close_from_outside(void)
{
    g_music_stop = true;
    while (g_music_running) vTaskDelay(pdMS_TO_TICKS(10));
    g_music_task_handle = NULL;
    if (g_music_timer) { lv_timer_delete(g_music_timer); g_music_timer = NULL; }
    screen_stop_music_rotate();
    g_is_playing = false;
    has_started_play = false;
    g_replay_block_flag = false;
    Audio_Play_Deinit();
    for (int i = 0; i < MP3_files.count; ++i) free(MP3_files.list[i]);
    free(MP3_files.list);
    MP3_files = (generic_file_list_t){0};
    file_count = 0;
    g_ui = NULL; g_play_btn = NULL; g_music_img = NULL;
}


// ====================== 事件初始化 ======================
void music_events_init(lv_ui *ui)
{
    music_close_from_outside();
    music_state_reset();

    g_ui = ui;
    g_music_img = ui->screen_music_img;
    g_play_btn = ui->screen_go_btn;

    lv_obj_add_event_cb(ui->screen_go_btn, event_go_btn_clicked, LV_EVENT_CLICKED, ui);
    mp3_file_scan();

    g_music_stop = false;
    g_music_running = true;
    if (xTaskCreate(music_control_task, "music_task", 4096, NULL, 5, &g_music_task_handle) != pdPASS) {
        g_music_running = false;
        ESP_LOGE(TAG, "Music controller allocation failed");
        lv_obj_add_state(g_play_btn, LV_STATE_DISABLED);
    }

    // LVGL 定时器处理 UI，任务只处理音频
    g_music_timer = lv_timer_create(music_finish_handler, 200, NULL);

    ESP_LOGI(TAG, "Music module init success!");
}

// LVGL 上下文处理停止
static void music_finish_handler(lv_timer_t *timer)
{
    if (g_ui == NULL || g_play_btn == NULL || g_music_img == NULL) return;

    if (music_need_stop_flag) {
        music_need_stop_flag = false;

        screen_stop_music_rotate();
        lv_obj_clear_state(g_play_btn, LV_STATE_CHECKED);
        g_is_playing = false;
        has_started_play = false; // 完全保留，未删除

        ESP_LOGI(TAG, "✅ 音乐播放完毕（安全处理）");
    }
}

// ====================== 音乐播放完检测 ======================
static void music_check_play_finish(void)
{
    // 屏蔽期内不检测，解决瞬间跳回问题
    if (!g_is_playing || g_replay_block_flag) return;

    esp_asp_state_t now_state = Audio_Get_Current_State();

    if (now_state == ESP_ASP_STATE_FINISHED || now_state == ESP_ASP_STATE_ERROR) {
        music_need_stop_flag = true;
        Audio_Stop_Play(); // 重置音频状态
    }
}

// ====================== 音乐扫描 ======================
static void mp3_file_scan(void)
{
    esp_err_t err = get_file_list_by_ext(CONFIG_BSP_SPIFFS_MOUNT_POINT"/music", ".mp3", &MP3_files);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Found %d MP3 file(s)", MP3_files.count);
        file_count = MP3_files.count;
    } else {
        ESP_LOGE(TAG, "No MP3 files found");
        MP3_files.count = 0;
        file_count = 0;
    }
}

// ====================== 播放第一首 ======================
static esp_gmf_err_t mp3_play_first(void)
{
    if (file_count == 0 || MP3_files.list == NULL) {
        ESP_LOGE(TAG, "Cannot play: no MP3 files in " CONFIG_BSP_SPIFFS_MOUNT_POINT "/music");
        return ESP_GMF_ERR_NOT_FOUND;
    }

    Volume_Adjustment(99);
    char uri[128] = {0};
    snprintf(uri, sizeof(uri), "file://%s", MP3_files.list[0] + strlen("/"));
    ESP_LOGI(TAG, "Start playing: %s", uri);
    
    return Audio_Play_Music(uri);
}

// ====================== 按钮点击 ======================
static void event_go_btn_clicked(lv_event_t *e)
{
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);
    g_is_playing = !g_is_playing;

    if (g_is_playing) {
        g_replay_block_flag = true; // 开启重播屏蔽
        screen_start_music_rotate(ui);
    } else {
        screen_stop_music_rotate();
    }
}

// ====================== 音频后台任务 ======================
static void music_control_task(void *arg)
{
    Audio_Play_Init();
    ESP_LOGI(TAG, "Music controller ready");

    bool last_play_state = false;
    uint32_t block_timer = 0;

    while (!g_music_stop) {
        // 500ms 后自动解除屏蔽
        if (g_replay_block_flag) {
            block_timer++;
            if (block_timer >= 10) {
                g_replay_block_flag = false;
                block_timer = 0;
            }
        }

        music_check_play_finish();

        if (g_is_playing != last_play_state) {
            last_play_state = g_is_playing;

            if (g_is_playing) {
                esp_gmf_err_t ret;
                if (!has_started_play) {
                    ret = mp3_play_first();
                    has_started_play = ret == ESP_GMF_ERR_OK;
                } else {
                    ret = Audio_Resume_Play();
                }
                if (ret != ESP_GMF_ERR_OK) {
                    ESP_LOGE(TAG, "Playback command failed: %d", ret);
                    Audio_Stop_Play();
                    has_started_play = false;
                    g_is_playing = false;
                    last_play_state = false;
                    g_replay_block_flag = false;
                    music_need_stop_flag = true;
                }
            } else {
                Audio_Pause_Play();
                g_replay_block_flag = false;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
    Audio_Stop_Play();
    g_music_running = false;
    vTaskDelete(NULL);
}

// ====================== 旋转动画 ======================
static void screen_start_music_rotate(lv_ui *ui)
{
    if (g_rot_anim != NULL) {
        lv_anim_delete(g_music_img, (lv_anim_exec_xcb_t)lv_image_set_rotation);
        g_rot_anim = NULL;
    }

    lv_image_set_pivot(g_music_img, LV_PCT(50), LV_PCT(50));
    lv_image_set_rotation(g_music_img, g_current_angle);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, g_music_img);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_image_set_rotation);
    lv_anim_set_values(&a, g_current_angle, g_current_angle + 36000);
    lv_anim_set_time(&a, 30000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);

    g_rot_anim = lv_anim_start(&a);
}

static void screen_stop_music_rotate(void)
{
    if (g_rot_anim != NULL && g_music_img != NULL) {
        g_current_angle = lv_image_get_rotation(g_music_img);
        lv_anim_delete(g_music_img, (lv_anim_exec_xcb_t)lv_image_set_rotation);
        g_rot_anim = NULL;
    }
}
