#pragma once
#include <stdbool.h>
#include "esp_err.h"
esp_err_t bsp_spi_bus_init(void);
bool bsp_board_i2c_in_use(void);
