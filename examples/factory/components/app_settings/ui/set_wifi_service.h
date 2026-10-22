#pragma once

#include "bsp/esp-bsp.h"

#ifdef __cplusplus
extern "C" {
#endif


esp_err_t wifi_c5_init_and_connect(void);
void init_wifi_settings_screen(lv_obj_t *parent, lv_obj_t *ret_scr, lv_group_t *ret_group);
void open_wifi_page(void);
void close_wifi_settings_screen(void);


#ifdef __cplusplus
}
#endif
