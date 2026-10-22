#include "music_init.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_heap_caps.h"


#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"

lv_ui music_guider_ui;

// 函数名可以保留，内部调用全部对齐前缀
void init_music_player_ui(void)
{
    music_setup_ui(&music_guider_ui);
    music_custom_init(&music_guider_ui);
    music_events_init(&music_guider_ui);
}