#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    ESP_ERROR_CHECK(bsp_io_expander_init());
    esp_io_expander_handle_t io = bsp_get_io_expander_handle();
    // EXIO6/7 are header outputs. Do not toggle reset, amplifier or sensor pins.
    const uint32_t pins = IO_EXPANDER_PIN_NUM_6 | IO_EXPANDER_PIN_NUM_7;
    ESP_ERROR_CHECK(esp_io_expander_set_dir(io, pins, IO_EXPANDER_OUTPUT));
    bool high = false;
    while (true) {
        ESP_ERROR_CHECK(esp_io_expander_set_level(io, pins, high));
        ESP_ERROR_CHECK(custom_io_expander_set_pwm(io, high ? 255 : 0));
        ESP_LOGI("exio", "EXIO6/7=%u; backlight PWM=%u", high, high ? 255 : 0);
        high = !high;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
