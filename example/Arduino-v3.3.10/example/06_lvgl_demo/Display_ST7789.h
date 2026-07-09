#pragma once

#include <Arduino.h>
#include <SPI.h>
#include "Board_IO.h"

#define LCD_WIDTH 240
#define LCD_HEIGHT 320

#define SPIFreq 80000000
#define EXAMPLE_PIN_NUM_MISO -1
#define EXAMPLE_PIN_NUM_MOSI 7
#define EXAMPLE_PIN_NUM_SCLK 6
#define EXAMPLE_PIN_NUM_LCD_CS 10
#define EXAMPLE_PIN_NUM_LCD_DC 9
#define EXAMPLE_PIN_NUM_SD_CS 23

#define VERTICAL 0
#define HORIZONTAL 1

#define Offset_X 0
#define Offset_Y 0

extern uint8_t LCD_Backlight;

void LCD_Init(void);
void LCD_SetCursor(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend);
void LCD_addWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend, uint16_t *color);

void Backlight_Init(void);
void Set_Backlight(uint8_t Light);
