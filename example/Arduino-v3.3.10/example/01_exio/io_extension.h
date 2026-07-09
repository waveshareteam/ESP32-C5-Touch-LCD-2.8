#pragma once

#include <Arduino.h>
#include <Wire.h>

// IO extension I2C address and register map used by the ESP32-C5-Touch-LCD-2.8 board.
#define IO_EXTENSION_ADDR             0x24

#define IO_EXTENSION_Mode             0x02
#define IO_EXTENSION_IO_OUTPUT_ADDR   0x03
#define IO_EXTENSION_IO_INPUT_ADDR    0x04
#define IO_EXTENSION_PWM_ADDR         0x05
#define IO_EXTENSION_ADC_ADDR         0x06
#define IO_EXTENSION_RTC_INT_ADDR     0x07

#define IO_EXTENSION_IO_0             0x00
#define IO_EXTENSION_IO_1             0x01
#define IO_EXTENSION_IO_2             0x02
#define IO_EXTENSION_IO_3             0x03
#define IO_EXTENSION_IO_4             0x04
#define IO_EXTENSION_IO_5             0x05
#define IO_EXTENSION_IO_6             0x06
#define IO_EXTENSION_IO_7             0x07
bool IO_EXTENSION_Init(TwoWire &wire = Wire);
bool IO_EXTENSION_IO_Mode(uint16_t pin_mask);
bool IO_EXTENSION_Output(uint8_t pin, uint8_t value);
uint8_t IO_EXTENSION_Input(uint8_t pin);
bool IO_EXTENSION_Pwm_Output(uint8_t value);
uint16_t IO_EXTENSION_Adc_Input();
uint8_t IO_EXTENSION_RTC_INT_READ();
