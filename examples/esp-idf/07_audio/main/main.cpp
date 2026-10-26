/* SPDX-License-Identifier: Apache-2.0 */
#include "MicTest.hpp"
#include "esp_log.h"

extern "C" void app_main(void)
{
    if (!bsp_display_start()) {
        ESP_LOGE("audio", "Display initialization failed");
        return;
    }
    static audio_demo::MicTest microphone;
    if (!bsp_display_lock(UINT32_MAX)) return;
    const bool ready = microphone.init() && microphone.run();
    bsp_display_unlock();
    if (!ready) {
        ESP_LOGE("audio", "Microphone UI or worker initialization failed");
        return;
    }
    const esp_err_t ret = bsp_display_show();
    if (ret != ESP_OK) ESP_LOGE("audio", "First frame: %s", esp_err_to_name(ret));
}
