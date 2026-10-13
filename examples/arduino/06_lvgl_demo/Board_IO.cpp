#include "Board_IO.h"

struct BoardIoState {
  TwoWire *wire;
  uint8_t output_value;
  bool initialized;
};

static BoardIoState board_io = {
  &Wire,
  0,
  false,
};

static bool board_io_write(uint8_t reg, const uint8_t *data, size_t len)
{
  if (board_io.wire == nullptr) {
    return false;
  }

  board_io.wire->beginTransmission(BOARD_IO_EXTENSION_ADDR);
  board_io.wire->write(reg);
  if (data != nullptr && len > 0) {
    board_io.wire->write(data, len);
  }
  return board_io.wire->endTransmission() == 0;
}

static bool board_io_write_u8(uint8_t reg, uint8_t value)
{
  return board_io_write(reg, &value, 1);
}

bool Board_IO_Init(TwoWire &wire)
{
  board_io.wire = &wire;
  board_io.wire->begin(BOARD_I2C_SDA_PIN, BOARD_I2C_SCL_PIN);
  board_io.wire->setClock(BOARD_I2C_FREQ_HZ);

  board_io.output_value = (1U << BOARD_TP_RST_EXIO) |
                          (1U << BOARD_LCD_RST_EXIO) |
                          (1U << BOARD_PA_CTRL_EXIO);

  if (!board_io_write_u8(BOARD_IO_EXTENSION_MODE_REG, 0xFF)) {
    board_io.initialized = false;
    return false;
  }

  board_io.initialized = board_io_write_u8(BOARD_IO_EXTENSION_OUTPUT_REG, board_io.output_value);
  return board_io.initialized;
}

bool Board_IO_SetOutput(uint8_t pin, bool level)
{
  if (pin > 7) {
    return false;
  }

  if (!board_io.initialized && !Board_IO_Init(*board_io.wire)) {
    return false;
  }

  if (level) {
    board_io.output_value |= (1U << pin);
  } else {
    board_io.output_value &= ~(1U << pin);
  }

  return board_io_write_u8(BOARD_IO_EXTENSION_OUTPUT_REG, board_io.output_value);
}

bool Board_SetBacklight(uint8_t percent)
{
  if (percent > 100) {
    percent = 100;
  }

  if (!board_io.initialized && !Board_IO_Init(*board_io.wire)) {
    return false;
  }

  const uint8_t pwm = static_cast<uint8_t>(percent * 255 / 100);
  return board_io_write_u8(BOARD_IO_EXTENSION_PWM_REG, pwm);
}

void Board_LCD_Reset(void)
{
  Board_IO_SetOutput(BOARD_LCD_RST_EXIO, false);
  delay(50);
  Board_IO_SetOutput(BOARD_LCD_RST_EXIO, true);
  delay(120);
}

void Board_Touch_Reset(void)
{
  Board_IO_SetOutput(BOARD_TP_RST_EXIO, false);
  delay(100);
  Board_IO_SetOutput(BOARD_TP_RST_EXIO, true);
  delay(500);
}
