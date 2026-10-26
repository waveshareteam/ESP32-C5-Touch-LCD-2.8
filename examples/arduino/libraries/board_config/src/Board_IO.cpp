#include "Board_IO.h"

struct BoardIoState {
  TwoWire *wire;
  uint8_t output_value;
  bool initialized;
  uint8_t direction;
};

static BoardIoState board_io = {
  &Wire,
  0,
  false,
  0,
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

static bool board_io_read(uint8_t reg, uint8_t *data, size_t length)
{
  board_io.wire->beginTransmission(BOARD_IO_EXTENSION_ADDR);
  board_io.wire->write(reg);
  if (board_io.wire->endTransmission(false) != 0) return false;
  if (board_io.wire->requestFrom(BOARD_IO_EXTENSION_ADDR, (uint8_t)length) != length) return false;
  for (size_t i = 0; i < length; ++i) data[i] = board_io.wire->read();
  return true;
}

bool Board_IO_Init(TwoWire &wire)
{
  if (board_io.initialized) return board_io.wire == &wire;
  board_io.wire = &wire;
  board_io.wire->begin(BOARD_I2C_SDA_PIN, BOARD_I2C_SCL_PIN);
  board_io.wire->setClock(BOARD_I2C_FREQ_HZ);

  // Keep reset lines released and the amplifier off; leave sensor IO alone.
  board_io.direction = (1U << BOARD_TP_RST_EXIO) | (1U << BOARD_LCD_RST_EXIO) |
                       (1U << BOARD_PA_CTRL_EXIO);
  board_io.output_value = (1U << BOARD_TP_RST_EXIO) | (1U << BOARD_LCD_RST_EXIO);
  board_io.initialized = board_io_write_u8(BOARD_IO_EXTENSION_OUTPUT_REG, board_io.output_value) &&
                         board_io_write_u8(BOARD_IO_EXTENSION_MODE_REG, board_io.direction);
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

  if (!(board_io.direction & (1U << pin))) {
    uint8_t direction = board_io.direction | (1U << pin);
    if (!board_io_write_u8(BOARD_IO_EXTENSION_MODE_REG, direction)) return false;
    board_io.direction = direction;
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

bool Board_ReadBatteryVoltage(float &voltage, uint16_t &raw)
{
  if (!board_io.initialized && !Board_IO_Init(*board_io.wire)) return false;
  uint32_t sum = 0;
  for (unsigned i = 0; i < 8; ++i) {
    uint8_t data[2];
    if (!board_io_read(BOARD_IO_EXTENSION_ADC_REG, data, sizeof(data))) return false;
    uint16_t sample = data[0] | (uint16_t(data[1]) << 8);
    if (sample > 1023) return false;
    sum += sample;
    delay(5);
  }
  raw = (sum + 4) / 8;
  voltage = raw * 3.3f / 1023.0f * 3.0f;
  return true;
}

void Board_Touch_Reset(void)
{
  Board_IO_SetOutput(BOARD_TP_RST_EXIO, false);
  delay(100);
  Board_IO_SetOutput(BOARD_TP_RST_EXIO, true);
  delay(500);
}
