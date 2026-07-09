#include <Arduino.h>
#define LV_CONF_INCLUDE_SIMPLE
#include <lvgl.h>
#include <esp_heap_caps.h>

#include "Board_IO.h"
#include "Display_ST7789.h"
#include "Touch_CST3530.h"

#if LV_COLOR_DEPTH != 16
#error "This example requires LV_COLOR_DEPTH 16."
#endif

#if LV_COLOR_16_SWAP != 0
#error "This example follows the ST7789 reference project and passes LVGL RGB565 data directly. Keep LV_COLOR_16_SWAP 0."
#endif

static constexpr int LCD_DRAW_BUF_HEIGHT = 40;

static bool touch_ready = false;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *draw_buf_1 = nullptr;
static lv_color_t *draw_buf_2 = nullptr;
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;
static lv_obj_t *touch_label = nullptr;
static lv_obj_t *brightness_label = nullptr;

static void display_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
  LCD_addWindow(area->x1, area->y1, area->x2, area->y2, (uint16_t *)&color_p->full);
  lv_disp_flush_ready(drv);
}

static void touch_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
  (void)drv;

  uint16_t touchpad_x[CST3530_LCD_TOUCH_MAX_POINTS] = {0};
  uint16_t touchpad_y[CST3530_LCD_TOUCH_MAX_POINTS] = {0};
  uint16_t strength[CST3530_LCD_TOUCH_MAX_POINTS] = {0};
  uint8_t touchpad_cnt = 0;

  if (touch_ready) {
    Touch2_Read_Data();
    uint8_t pressed = Touch2_Get_XY(touchpad_x, touchpad_y, strength, &touchpad_cnt, CST3530_LCD_TOUCH_MAX_POINTS);

    if (pressed && touchpad_cnt > 0) {
      if (touchpad_x[0] >= LCD_WIDTH) {
        touchpad_x[0] = LCD_WIDTH - 1;
      }
      if (touchpad_y[0] >= LCD_HEIGHT) {
        touchpad_y[0] = LCD_HEIGHT - 1;
      }

      data->point.x = touchpad_x[0];
      data->point.y = touchpad_y[0];
      data->state = LV_INDEV_STATE_PR;

      if (touch_label != nullptr) {
        lv_label_set_text_fmt(touch_label, "Touch: %u, %u", touchpad_x[0], touchpad_y[0]);
      }
      return;
    }
  }

  data->state = LV_INDEV_STATE_REL;
}

static void brightness_slider_event_cb(lv_event_t *e)
{
  lv_obj_t *slider = lv_event_get_target(e);
  int32_t brightness = lv_slider_get_value(slider);

  Set_Backlight((uint8_t)brightness);

  if (brightness_label != nullptr) {
    lv_label_set_text_fmt(brightness_label, "Backlight %ld%%", (long)brightness);
  }
}

static void create_demo_ui()
{
  lv_obj_t *scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x101820), 0);
  lv_obj_set_style_bg_grad_color(scr, lv_color_hex(0x1D3557), 0);
  lv_obj_set_style_bg_grad_dir(scr, LV_GRAD_DIR_VER, 0);

  lv_obj_t *title = lv_label_create(scr);
  lv_label_set_text(title, "ESP32-C5 Touch LCD 2.8");
  lv_obj_set_style_text_color(title, lv_color_white(), 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 28);

  lv_obj_t *subtitle = lv_label_create(scr);
  lv_label_set_text(subtitle, "Arduino + ST7789 + CST3530");
  lv_obj_set_style_text_color(subtitle, lv_color_hex(0xA8DADC), 0);
  lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 62);

  lv_obj_t *panel = lv_obj_create(scr);
  lv_obj_set_size(panel, 220, 142);
  lv_obj_align(panel, LV_ALIGN_CENTER, 0, 8);
  lv_obj_set_style_radius(panel, 8, 0);
  lv_obj_set_style_bg_color(panel, lv_color_hex(0xF1FAEE), 0);
  lv_obj_set_style_border_width(panel, 0, 0);
  lv_obj_set_style_shadow_width(panel, 14, 0);
  lv_obj_set_style_shadow_opa(panel, LV_OPA_30, 0);

  lv_obj_t *status = lv_label_create(panel);
  lv_label_set_text(status, "Display OK");
  lv_obj_set_style_text_color(status, lv_color_hex(0x1D3557), 0);
  lv_obj_align(status, LV_ALIGN_TOP_MID, 0, 14);

  touch_label = lv_label_create(panel);
  lv_label_set_text(touch_label, touch_ready ? "Touch: ready" : "Touch: not found");
  lv_obj_set_style_text_color(touch_label, lv_color_hex(0x457B9D), 0);
  lv_obj_align(touch_label, LV_ALIGN_CENTER, 0, -8);

  lv_obj_t *button = lv_btn_create(panel);
  lv_obj_set_size(button, 92, 30);
  lv_obj_align(button, LV_ALIGN_BOTTOM_MID, 0, -8);

  lv_obj_t *button_label = lv_label_create(button);
  lv_label_set_text(button_label, "Touch");
  lv_obj_center(button_label);

  brightness_label = lv_label_create(scr);
  lv_label_set_text(brightness_label, "Backlight 100%");
  lv_obj_set_style_text_color(brightness_label, lv_color_white(), 0);
  lv_obj_align(brightness_label, LV_ALIGN_BOTTOM_MID, 0, -48);

  lv_obj_t *brightness_slider = lv_slider_create(scr);
  lv_obj_set_size(brightness_slider, 208, 16);
  lv_slider_set_range(brightness_slider, 5, 100);
  lv_slider_set_value(brightness_slider, 100, LV_ANIM_OFF);
  lv_obj_align(brightness_slider, LV_ALIGN_BOTTOM_MID, 0, -24);
  lv_obj_add_event_cb(brightness_slider, brightness_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_set_style_bg_color(brightness_slider, lv_color_hex(0x324A5F), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(brightness_slider, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_bg_color(brightness_slider, lv_color_hex(0x2A9D8F), LV_PART_INDICATOR);
  lv_obj_set_style_bg_color(brightness_slider, lv_color_hex(0xF1FAEE), LV_PART_KNOB);
  lv_obj_set_style_pad_all(brightness_slider, 2, LV_PART_KNOB);
}

void setup()
{
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("ESP32-C5-Touch-LCD-2.8 Arduino LVGL demo");

  if (!Board_IO_Init(Wire)) {
    Serial.println("CH32V003 IO expander init failed");
  }

  LCD_Init();
  Backlight_Init();
  touch_ready = TOUCH2_Init();

  lv_init();

  const uint32_t buf_pixels = LCD_WIDTH * LCD_DRAW_BUF_HEIGHT;
  draw_buf_1 = static_cast<lv_color_t *>(heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
  draw_buf_2 = static_cast<lv_color_t *>(heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
  if (!draw_buf_1 || !draw_buf_2) {
    draw_buf_1 = static_cast<lv_color_t *>(heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_8BIT));
    draw_buf_2 = static_cast<lv_color_t *>(heap_caps_malloc(buf_pixels * sizeof(lv_color_t), MALLOC_CAP_8BIT));
  }

  if (!draw_buf_1 || !draw_buf_2) {
    Serial.println("LVGL draw buffer allocation failed");
    while (true) {
      delay(1000);
    }
  }

  lv_disp_draw_buf_init(&draw_buf, draw_buf_1, draw_buf_2, buf_pixels);

  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = LCD_WIDTH;
  disp_drv.ver_res = LCD_HEIGHT;
  disp_drv.flush_cb = display_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  if (touch_ready) {
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touch_read;
    lv_indev_drv_register(&indev_drv);
  }

  create_demo_ui();
  Serial.println("LVGL demo started");
}

void loop()
{
  lv_tick_inc(5);
  lv_timer_handler();

  delay(5);
}
