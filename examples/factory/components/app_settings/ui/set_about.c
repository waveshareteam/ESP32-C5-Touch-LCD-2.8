#include "set_about.h"

#include "esp_mac.h"
#include "esp_system.h"
#include "esp_idf_version.h"
#include "set_lv_style.h"

static lv_obj_t *return_screen = NULL;
static lv_group_t *return_group = NULL;
static lv_group_t *about_group = NULL;
static lv_obj_t *about_con = NULL;
static lv_obj_t *back_btn = NULL;
static lv_obj_t *scroll_container = NULL;

static void back_btn_click_event_cb(lv_event_t *e);
static char *get_mac_address(void);
static void init_about_lv_style(void);

static lv_style_t style_about_title;
static lv_style_t style_about_label;
static lv_style_t style_about_value;
static lv_style_t style_about_row;

void init_about_settings_screen(lv_obj_t *parent, lv_obj_t *ret_scr, lv_group_t *ret_group)
{
    return_screen = ret_scr;
    return_group = ret_group;

    if (about_group != NULL) {
        lv_group_del(about_group);
    }
    about_group = lv_group_create();
    lv_group_set_wrap(about_group, false);

    // 屏幕背景
    lv_obj_set_style_bg_color(parent, lv_color_hex(0xF7F8FA), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(parent, LV_OPA_COVER, LV_PART_MAIN);

    init_about_lv_style();

    // ===================== 主容器 =====================
    about_con = lv_obj_create(parent);
    lv_obj_set_size(about_con, 240, 296);
    lv_obj_align(about_con, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(about_con, lv_color_hex(0xF7F8FA), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(about_con, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(about_con, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(about_con, 8, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(about_con, 4, LV_PART_MAIN);
    lv_obj_set_style_shadow_color(about_con, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(about_con, LV_OPA_10, LV_PART_MAIN);
    lv_obj_set_style_shadow_offset_y(about_con, 4, LV_PART_MAIN);
    lv_obj_set_scrollbar_mode(about_con, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(about_con, LV_OBJ_FLAG_SCROLLABLE);

    // ===================== 返回按钮 =====================
    back_btn = lv_btn_create(about_con);
    lv_obj_set_size(back_btn, 42, 28);
    lv_obj_align(back_btn, LV_ALIGN_TOP_LEFT, 8, 6);
    lv_obj_add_event_cb(back_btn, back_btn_click_event_cb, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(about_group, back_btn);

    lv_obj_set_style_bg_color(back_btn, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_radius(back_btn, 8, LV_PART_MAIN);
    lv_obj_set_style_border_width(back_btn, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(back_btn, lv_color_hex(0xE5E7EB), LV_PART_MAIN);

    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_color(back_label, lv_color_hex(0x1F2933), LV_PART_MAIN);
    lv_obj_center(back_label);

    // ===================== 标题 =====================
    lv_obj_t *title_label = lv_label_create(about_con);
    lv_label_set_text(title_label, "Device info");
    lv_obj_add_style(title_label, &style_about_title, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 40);

    // ===================== 滚动容器 =====================
    scroll_container = lv_obj_create(about_con);
    lv_obj_set_size(scroll_container, 222, 226);
    lv_obj_align(scroll_container, LV_ALIGN_BOTTOM_MID, 0, 6);
    lv_obj_set_flex_flow(scroll_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_ver(scroll_container, 6, 0);
    lv_obj_set_style_pad_hor(scroll_container, 6, 0);
    lv_obj_set_style_pad_row(scroll_container, 1, 0);
    lv_obj_set_style_bg_opa(scroll_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(scroll_container, 0, 0);
    lv_obj_set_scrollbar_mode(scroll_container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(scroll_container, LV_OBJ_FLAG_SCROLLABLE);

    // ===================== 【修复】行创建宏：固定宽度 + 右侧滚动文字 =====================
    #define CREATE_ABOUT_ROW(container, label_text, value_text) do { \
        lv_obj_t *row = lv_obj_create(container); \
        lv_obj_add_style(row, &style_about_row, 0); \
        lv_obj_set_size(row, lv_pct(100), LV_SIZE_CONTENT); \
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW); \
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, 0); \
        lv_obj_set_style_pad_ver(row, 10, 0); \
        lv_obj_set_style_pad_hor(row, 0, 0); \
        lv_obj_set_style_border_width(row, 1, 0); \
        lv_obj_set_style_border_side(row, LV_BORDER_SIDE_BOTTOM, 0); \
        lv_obj_set_style_border_color(row, lv_color_hex(0xD1D5DB), 0); \
        \
        /* 左侧固定宽度 */ \
        lv_obj_t *lbl = lv_label_create(row); \
        lv_label_set_text(lbl, label_text); \
        lv_obj_add_style(lbl, &style_about_label, 0); \
        lv_obj_set_width(lbl, 74); \
        \
        /* 右侧固定宽度 + 滚动播放 */ \
        lv_obj_t *val = lv_label_create(row); \
        lv_label_set_text(val, value_text); \
        lv_obj_add_style(val, &style_about_value, 0); \
        lv_obj_set_width(val, 128); \
        lv_label_set_long_mode(val, LV_LABEL_LONG_SCROLL_CIRCULAR); \
    } while(0)

    // ===================== 信息项 =====================
    CREATE_ABOUT_ROW(scroll_container, "Board:", "ESP32-C5-Touch-LCD-2.8");
    CREATE_ABOUT_ROW(scroll_container, "FW:", "V1.0.0");
    CREATE_ABOUT_ROW(scroll_container, "MAC:", get_mac_address());
    CREATE_ABOUT_ROW(scroll_container, "SDK:", esp_get_idf_version());
}

static void init_about_lv_style(void)
{
    lv_style_init(&style_about_title);
    lv_style_set_text_font(&style_about_title, &lv_font_montserrat_14);
    lv_style_set_text_color(&style_about_title, lv_color_hex(0x1F2933));
    lv_style_set_text_align(&style_about_title, LV_TEXT_ALIGN_CENTER);

    lv_style_init(&style_about_label);
    lv_style_set_text_font(&style_about_label, &lv_font_montserrat_14);
    lv_style_set_text_color(&style_about_label, lv_color_hex(0x6B7280));

    lv_style_init(&style_about_value);
    lv_style_set_text_font(&style_about_value, &lv_font_montserrat_14);
    lv_style_set_text_color(&style_about_value, lv_color_hex(0x1F2933));

    lv_style_init(&style_about_row);
    lv_style_set_bg_opa(&style_about_row, LV_OPA_TRANSP);
    lv_style_set_border_width(&style_about_row, 0);
}

static void back_btn_click_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {

        lv_screen_load(return_screen);
        if (return_group) {
            lv_indev_set_group(bsp_display_get_input_dev(), return_group);
        }

    }
}

static char *get_mac_address(void)
{
    static char mac_str[18] = {0};
    uint8_t mac_addr[6] = {0};

    esp_err_t err = esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
    if (err != ESP_OK) {
        err = esp_efuse_mac_get_default(mac_addr);
    }

    if (err != ESP_OK) {
        snprintf(mac_str, sizeof(mac_str), "Unknown");
        return mac_str;
    }

    snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac_addr[0], mac_addr[1], mac_addr[2],
             mac_addr[3], mac_addr[4], mac_addr[5]);
    return mac_str;
}

void open_about_page(void)
{
    if (about_group != NULL) {
        lv_indev_set_group(bsp_display_get_input_dev(), about_group);
    }
}
