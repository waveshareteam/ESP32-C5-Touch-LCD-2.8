#include "bsp/esp-bsp.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    ESP_ERROR_CHECK(bsp_environment_init());
    while (true) {
        float temperature, humidity;
        esp_err_t ret = bsp_environment_get_data(&temperature, &humidity);
        if (ret == ESP_OK) ESP_LOGI("shtc3", "%.2f C; %.2f %%RH", temperature, humidity);
        else ESP_LOGW("shtc3", "Read: %s", esp_err_to_name(ret));
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
