#include <Arduino.h>
#include <lvgl.h>
#include <Board_IO.h>
#include <Display_ST7789.h>

static lv_color_t pixels[LCD_WIDTH * 10];
static lv_disp_draw_buf_t buffer;
static lv_disp_drv_t display;
static lv_obj_t *label;
static uint32_t last_sample, last_tick;

static void flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color)
{
  LCD_addWindow(area->x1, area->y1, area->x2, area->y2, (uint16_t *)color);
  lv_disp_flush_ready(drv);
}

void setup()
{
  Serial.begin(115200);
  if (!Board_IO_Init()) {
    Serial.println("CH32V003 initialization failed");
    return;
  }
  Board_SetBacklight(0);
  LCD_Init();
  lv_init();
  lv_disp_draw_buf_init(&buffer, pixels, nullptr, LCD_WIDTH * 10);
  lv_disp_drv_init(&display);
  display.hor_res = LCD_WIDTH;
  display.ver_res = LCD_HEIGHT;
  display.flush_cb = flush;
  display.draw_buf = &buffer;
  lv_disp_drv_register(&display);
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xf3f6fa), 0);
  label = lv_label_create(lv_scr_act());
  lv_obj_set_style_text_color(label, lv_color_hex(0x203040), 0);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
  lv_label_set_text(label, "Battery voltage\nReading ADC...");
  lv_obj_center(label);
  lv_refr_now(nullptr);
  Board_SetBacklight(100);
  last_tick = millis();
  last_sample = last_tick - 1000;
}

void loop()
{
  if (!label) { delay(100); return; }
  uint32_t now = millis();
  lv_tick_inc(now - last_tick);
  last_tick = now;
  if (now - last_sample >= 1000) {
    last_sample = now;
    float voltage;
    uint16_t raw;
    if (Board_ReadBatteryVoltage(voltage, raw)) {
      char text[96];
      snprintf(text, sizeof(text), "Battery voltage\n%.3f V\nADC: %u / 1023", voltage, raw);
      lv_label_set_text(label, text);
      Serial.println(text);
    } else {
      lv_label_set_text(label, "Battery voltage\nADC read failed");
      Serial.println("Battery ADC read failed");
    }
    lv_obj_center(label);
  }
  lv_timer_handler();
  delay(5);
}
