#pragma once

#include <Arduino.h>
#include <Wire.h>

#define BOARD_I2C_SDA_PIN 0
#define BOARD_I2C_SCL_PIN 1
#define BOARD_I2C_FREQ_HZ 400000

#define BOARD_IO_EXTENSION_ADDR 0x24
#define BOARD_IO_EXTENSION_MODE_REG 0x02
#define BOARD_IO_EXTENSION_OUTPUT_REG 0x03
#define BOARD_IO_EXTENSION_PWM_REG 0x05

#define BOARD_TP_RST_EXIO 0
#define BOARD_LCD_RST_EXIO 1
#define BOARD_PA_CTRL_EXIO 3

bool Board_IO_Init(TwoWire &wire = Wire);
bool Board_IO_SetOutput(uint8_t pin, bool level);
bool Board_SetBacklight(uint8_t percent);
void Board_LCD_Reset(void);
void Board_Touch_Reset(void);
