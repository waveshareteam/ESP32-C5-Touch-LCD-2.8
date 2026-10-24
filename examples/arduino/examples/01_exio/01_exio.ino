#include <Arduino.h>
#include <Board_IO.h>

static bool ready;
static bool high;

void setup()
{
  Serial.begin(115200);
  ready = Board_IO_Init();
  if (!ready) Serial.println("CH32V003 initialization failed");
}

void loop()
{
  if (ready) {
    // Only the exposed header outputs are toggled.
    bool ok = Board_IO_SetOutput(6, high) && Board_IO_SetOutput(7, high) &&
              Board_SetBacklight(high ? 100 : 0);
    Serial.printf("EXIO6/7=%u, backlight=%u%%: %s\n", high, high ? 100 : 0, ok ? "OK" : "FAIL");
    high = !high;
  }
  delay(1000);
}
