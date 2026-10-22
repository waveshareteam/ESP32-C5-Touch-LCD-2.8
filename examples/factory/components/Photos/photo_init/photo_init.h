#pragma once

#include "esp_log.h"
#include "bsp/esp-bsp.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_photo_player(lv_display_t *disp);
void init_photo_ui_screen(lv_obj_t *parent,generic_file_list_t *file_list);
void deinit_photo_player(void);

#ifdef __cplusplus
}
#endif
