#include <Arduino.h>
#include <Wire.h>

// ESP32-C5-Touch-LCD-2.8 BSP: SCL = GPIO1, SDA = GPIO0
static constexpr uint8_t I2C_SDA = 0;
static constexpr uint8_t I2C_SCL = 1;
static constexpr uint32_t I2C_FREQ = 400000;

static constexpr uint8_t SHTC3_ADDR = 0x70;
static constexpr uint16_t SHTC3_CMD_WAKE = 0x3517;
static constexpr uint16_t SHTC3_CMD_SLEEP = 0xB098;
static constexpr uint16_t SHTC3_CMD_SOFT_RESET = 0x805D;
static constexpr uint16_t SHTC3_CMD_READ_ID = 0xEFC8;
static constexpr uint16_t SHTC3_CMD_MEASURE_T_RH = 0x7866; // T first, no clock stretching, normal mode

static uint8_t shtc3Crc(const uint8_t *data, size_t len)
{
  uint8_t crc = 0xFF;

  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t bit = 0; bit < 8; bit++) {
      crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);
    }
  }

  return crc;
}

static bool writeCommand(uint16_t cmd)
{
  Wire.beginTransmission(SHTC3_ADDR);
  Wire.write(cmd >> 8);
  Wire.write(cmd & 0xFF);
  return Wire.endTransmission() == 0;
}

static bool wakeShtc3()
{
  if (!writeCommand(SHTC3_CMD_WAKE)) {
    return false;
  }

  delayMicroseconds(240);
  return true;
}

static void sleepShtc3()
{
  writeCommand(SHTC3_CMD_SLEEP);
}

static bool readWordWithCrc(uint16_t *value)
{
  uint8_t buf[3] = {0};

  if (Wire.requestFrom(SHTC3_ADDR, static_cast<uint8_t>(3)) != 3) {
    return false;
  }

  for (uint8_t i = 0; i < sizeof(buf); i++) {
    buf[i] = Wire.read();
  }

  if (shtc3Crc(buf, 2) != buf[2]) {
    return false;
  }

  *value = (static_cast<uint16_t>(buf[0]) << 8) | buf[1];
  return true;
}

static bool readShtc3Id(uint16_t *id)
{
  if (!wakeShtc3()) {
    return false;
  }

  bool ok = writeCommand(SHTC3_CMD_READ_ID) && readWordWithCrc(id);
  sleepShtc3();
  return ok;
}

static bool readShtc3(float *temperature, float *humidity)
{
  uint8_t buf[6] = {0};

  if (!wakeShtc3()) {
    return false;
  }

  if (!writeCommand(SHTC3_CMD_MEASURE_T_RH)) {
    sleepShtc3();
    return false;
  }

  delay(20);

  if (Wire.requestFrom(SHTC3_ADDR, static_cast<uint8_t>(6)) != 6) {
    sleepShtc3();
    return false;
  }

  for (uint8_t i = 0; i < sizeof(buf); i++) {
    buf[i] = Wire.read();
  }

  sleepShtc3();

  if (shtc3Crc(&buf[0], 2) != buf[2] || shtc3Crc(&buf[3], 2) != buf[5]) {
    return false;
  }

  uint16_t rawTemp = (static_cast<uint16_t>(buf[0]) << 8) | buf[1];
  uint16_t rawHumidity = (static_cast<uint16_t>(buf[3]) << 8) | buf[4];

  *temperature = -45.0f + 175.0f * rawTemp / 65535.0f;
  *humidity = 100.0f * rawHumidity / 65535.0f;
  return true;
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("SHTC3 test");

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(I2C_FREQ);

  if (!wakeShtc3()) {
    Serial.println("SHTC3 not found. Check SDA/SCL and power.");
    return;
  }

  writeCommand(SHTC3_CMD_SOFT_RESET);
  delay(2);

  uint16_t id = 0;
  if (readShtc3Id(&id)) {
    Serial.printf("SHTC3 ID: 0x%04X\r\n", id);
  } else {
    Serial.println("SHTC3 ID read failed, continue measuring...");
  }
}

void loop()
{
  float temperature = 0.0f;
  float humidity = 0.0f;

  if (readShtc3(&temperature, &humidity)) {
    Serial.printf("Temperature: %.2f C, Humidity: %.2f %%\r\n", temperature, humidity);
  } else {
    Serial.println("Failed to read SHTC3");
  }

  delay(1000);
}
