#include "game_init.h"

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


lv_ui guider_ui;


void init_game_ui_screen(void)
{
    
    setup_ui(&guider_ui);
    custom_init(&guider_ui);
    events_init(&guider_ui);

}