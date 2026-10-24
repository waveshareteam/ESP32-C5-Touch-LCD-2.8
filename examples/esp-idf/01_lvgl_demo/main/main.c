#include "bsp/esp-bsp.h"
#include "lv_demos.h"

void app_main(void)
{
    ESP_ERROR_CHECK(bsp_display_start() ? ESP_OK : ESP_FAIL);
    if (bsp_display_lock(UINT32_MAX)) {
        lv_demo_widgets();
        bsp_display_unlock();
    }
    ESP_ERROR_CHECK(bsp_display_show());
}
