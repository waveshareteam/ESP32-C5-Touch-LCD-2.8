#include "weather_init.h"

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

lv_ui weather_guider_ui;

// 函数名可以保留，内部调用全部对齐前缀
void init_weather_ui(void)
{
    weather_setup_ui(&weather_guider_ui);
    weather_custom_init(&weather_guider_ui);
    weather_events_init(&weather_guider_ui);
}

void deinit_weather_ui(void)
{
    weather_events_deinit();
}
