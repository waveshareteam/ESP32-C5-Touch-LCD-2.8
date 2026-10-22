#pragma once

#include "bsp/esp-bsp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*settings_close_cb_t)(void);

void settings_ui_screen_init(settings_close_cb_t close_cb);
void settings_ui_screen_deinit(void);

#ifdef __cplusplus
}
#endif
