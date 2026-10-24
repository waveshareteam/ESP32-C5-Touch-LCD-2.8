#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    ESP_ERROR_CHECK(bsp_rtc_init());
    // Demonstration time is set once on every boot.
    const bsp_sensor_datetime_t initial = {
        .year = 2026, .month = 10, .day = 24, .hour = 12, .weekday = 6
    };
    ESP_ERROR_CHECK(bsp_rtc_set_datetime(&initial));
    while (true) {
        bsp_sensor_datetime_t date;
        esp_err_t ret = bsp_rtc_get_datetime(&date);
        if (ret == ESP_OK) {
            ESP_LOGI("rtc", "%04u-%02u-%02u %02u:%02u:%02u", date.year, date.month, date.day,
                     date.hour, date.minute, date.second);
        } else ESP_LOGW("rtc", "Read: %s", esp_err_to_name(ret));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
