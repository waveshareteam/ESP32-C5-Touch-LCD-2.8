#pragma once

#include "esp_log.h"
#include "bsp/esp-bsp.h"
#include "lvgl.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_weather_ui(void);
void deinit_weather_ui(void);


#ifdef __cplusplus
}
#endif
