#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#include "bsp_board_extra.h"
#include "esp_log.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"

static const char *TAG = "MUSIC_EVENT";

// ====================== 音乐模块 全局变量 ======================
static generic_file_list_t MP3_files = {0};
static uint16_t file_count = 0;

// ====================== 动画/状态变量 ======================
static lv_anim_t *g_rot_anim = NULL;
static bool g_is_playing = false;
static int32_t g_current_angle = 0;
static lv_obj_t *g_music_img = NULL;
static lv_obj_t *g_play_btn = NULL;  // 保存按钮
static bool has_started_play = false;

// ====================== 内部函数声明 ======================
static void mp3_file_scan(void);
static void mp3_play_first(void);
static void screen_start_music_rotate(lv_ui *ui);
static void screen_stop_music_rotate(void);
static void event_go_btn_clicked(lv_event_t *e);
static void music_check_timer_cb(lv_timer_t *timer);  

static lv_ui *g_ui = NULL;

void events_init(lv_ui *ui)
{
    g_ui = ui;
    g_music_img = ui->screen_music_img;
    g_play_btn = ui->screen_go_btn;  

    lv_obj_add_event_cb(ui->screen_go_btn, event_go_btn_clicked, LV_EVENT_CLICKED, ui);

    Audio_Play_Init();
    mp3_file_scan();

    lv_timer_create(music_check_timer_cb, 200, NULL);

    ESP_LOGI(TAG, "Music module init success!");
}

static void music_check_timer_cb(lv_timer_t *timer)
{
    if (!g_is_playing)
        return;

    esp_asp_state_t now_state = Audio_Get_Current_State();

    if (now_state == ESP_ASP_STATE_FINISHED)
    {
        ESP_LOGI(TAG, "✅ 音乐播放完毕 → 停止动画 + 恢复播放按钮");

        screen_stop_music_rotate();

        lv_obj_clear_state(g_play_btn, LV_STATE_CHECKED);  

        g_is_playing = false;
        has_started_play = false;
    }
}

static void mp3_file_scan(void)
{
    esp_err_t err = get_file_list_by_ext(CONFIG_BSP_SPIFFS_MOUNT_POINT"/music", ".mp3", &MP3_files);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "找到 %d 首MP3", MP3_files.count);
        file_count = MP3_files.count;
    } else {
        ESP_LOGE(TAG, "未找到音乐文件");
        MP3_files.count = 0;
        file_count = 0;
    }
}

static void mp3_play_first(void)
{
    if (file_count == 0 || MP3_files.list == NULL) {
        return;
    }

    Volume_Adjustment(99);

    char uri[128] = {0};
    snprintf(uri, sizeof(uri), "file://%s", MP3_files.list[0] + strlen("/"));
    ESP_LOGI(TAG, "开始播放: %s", uri);
    Audio_Play_Music(uri);
}

static void event_go_btn_clicked(lv_event_t *e)
{
    lv_ui *ui = (lv_ui *)lv_event_get_user_data(e);

    if (g_is_playing) {
        // 暂停
        Audio_Pause_Play();
        screen_stop_music_rotate();
        g_is_playing = false;
    } else {
        // 播放
        if (!has_started_play) {
            mp3_play_first();
            has_started_play = true;
        } else {
            Audio_Resume_Play();
        }
        screen_start_music_rotate(ui);
        g_is_playing = true;
    }
}

static void screen_start_music_rotate(lv_ui *ui)
{
    if (g_rot_anim != NULL) {
        lv_anim_del_all();
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
        lv_anim_del_all();
        g_rot_anim = NULL;
    }
}