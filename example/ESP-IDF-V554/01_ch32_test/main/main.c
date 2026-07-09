/********************************************************************************************************
 * @file    main.c
 * @author  Javen
 * @company Waveshare
 * @date    2026/03/09
 * @brief   CH32V003 IO Expander Application Entry
 * @note    Initializes CH32V003 IO expander and executes functional test
 ********************************************************************************************************/
#include "ch32v003.h"

/**
 * @brief  Main application entry point for CH32V003 IO expander
 * @note   1. Initializes CH32V003 IO expander (I2C bus + device)
 *         2. Executes IO expander functional test (output toggle + input read)
 * @return None
 */
void app_main(void)
{
    // Initialize CH32V003 IO expander (I2C bus + device init)
    ch32v003_init();
    
    // Execute CH32V003 IO expander functional test
    ch32_test();
}