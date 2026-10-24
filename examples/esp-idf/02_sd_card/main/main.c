#include <stdio.h>
#include <string.h>
#include "bsp/esp-bsp.h"
#include "esp_log.h"

void app_main(void)
{
    const char text[] = "ESP32-C5-Touch-LCD-2.8 SD test\n";
    char buffer[sizeof(text)] = {0};
    ESP_ERROR_CHECK(bsp_sdcard_acquire());
    FILE *file = fopen(BSP_SD_MOUNT_POINT "/test.txt", "wb");
    esp_err_t ret = ESP_FAIL;
    if (file) {
        size_t n = fwrite(text, 1, sizeof(text) - 1, file);
        int closed = fclose(file);
        if (n == sizeof(text) - 1 && closed == 0) {
            file = fopen(BSP_SD_MOUNT_POINT "/test.txt", "rb");
            if (file) {
                n = fread(buffer, 1, sizeof(buffer), file);
                bool error = ferror(file);
                closed = fclose(file);
                if (!error && closed == 0 && n == sizeof(text) - 1 &&
                    memcmp(text, buffer, n) == 0) ret = ESP_OK;
            }
        }
    }
    ESP_LOGI("sd_card", "Write/read verification: %s", esp_err_to_name(ret));
    bsp_sdcard_release();
    ESP_ERROR_CHECK(bsp_sdcard_unmount());
}
