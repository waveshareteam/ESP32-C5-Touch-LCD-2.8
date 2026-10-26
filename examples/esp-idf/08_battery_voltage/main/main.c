#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    ESP_ERROR_CHECK(bsp_io_expander_init());
    ESP_ERROR_CHECK(bsp_display_start() ? ESP_OK : ESP_FAIL);
    lv_obj_t *label = NULL;
    if (bsp_display_lock(UINT32_MAX)) {
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xf3f6fa), 0);
        label = lv_label_create(lv_screen_active());
        lv_obj_set_style_text_color(label, lv_color_hex(0x203040), 0);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_text(label, "Battery voltage\nReading ADC...");
        lv_obj_center(label);
        bsp_display_unlock();
    }
    ESP_ERROR_CHECK(bsp_display_show());
    while (true) {
        float voltage = 0;
        uint16_t raw = 0;
        esp_err_t ret = bsp_battery_get_voltage(&voltage, &raw);
        if (ret == ESP_OK) ESP_LOGI("battery", "ADC=%u; voltage=%.3f V", raw, voltage);
        else ESP_LOGW("battery", "Read: %s", esp_err_to_name(ret));
        if (label && bsp_display_lock(UINT32_MAX)) {
            if (ret == ESP_OK) lv_label_set_text_fmt(label, "Battery voltage\n%.3f V\nADC: %u / 1023", (double)voltage, raw);
            else lv_label_set_text(label, "Battery voltage\nADC read failed");
            lv_obj_center(label);
            bsp_display_unlock();
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
