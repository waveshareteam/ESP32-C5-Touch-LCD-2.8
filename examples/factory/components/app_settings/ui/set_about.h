#pragma once

#include "bsp/esp-bsp.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_about_settings_screen(lv_obj_t *parent, lv_obj_t *ret_scr,lv_group_t *ret_group);
void open_about_page(void);

#ifdef __cplusplus
}
#endif
