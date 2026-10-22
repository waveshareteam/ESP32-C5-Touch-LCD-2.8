#include "set_lv_style.h"

static bool set_styles_init_flag = false;
static set_screen_styles_t set_styles;


static void init_set_lv_style(void)
{
    // ===================== 初始化样式对象 =====================
    lv_style_init(&set_styles.list);
    lv_style_init(&set_styles.list_btn);
    lv_style_init(&set_styles.list_focus_key);
    lv_style_init(&set_styles.list_btn_pressed);
    lv_style_init(&set_styles.list_btn_hover);


    // ===================== 列表容器样式 =====================
    lv_style_set_pad_top(&set_styles.list, 24);
    lv_style_set_pad_bottom(&set_styles.list, 8);
    lv_style_set_pad_left(&set_styles.list, 26);
    lv_style_set_pad_right(&set_styles.list, 26);
    lv_style_set_pad_row(&set_styles.list, 0);
    lv_style_set_border_width(&set_styles.list, 0);
    lv_style_set_radius(&set_styles.list, 0);
    lv_style_set_bg_color(&set_styles.list, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_opa(&set_styles.list, LV_OPA_COVER);
    lv_style_set_shadow_width(&set_styles.list, 0);
    lv_style_set_shadow_color(&set_styles.list, lv_color_hex(0x000000));
    lv_style_set_shadow_opa(&set_styles.list, LV_OPA_10);
    lv_style_set_shadow_offset_y(&set_styles.list, 2);
    lv_style_set_text_font(&set_styles.list, &lv_font_montserrat_18);

    // ===================== 列表标题文本 =====================
    lv_style_init(&set_styles.list_title);
    lv_style_set_bg_color(&set_styles.list_title, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_opa(&set_styles.list_title, LV_OPA_COVER);
    lv_style_set_text_color(&set_styles.list_title, lv_color_hex(0x6B7280));
    lv_style_set_text_font(&set_styles.list_title, &lv_font_montserrat_22);
    lv_style_set_height(&set_styles.list_title, 37);
    lv_style_set_pad_top(&set_styles.list_title, 0);
    lv_style_set_pad_bottom(&set_styles.list_title, 0);
    lv_style_set_border_width(&set_styles.list_title, 0);

    // ===================== 列表按钮默认样式 =====================
    lv_style_set_height(&set_styles.list_btn, 43);
    lv_style_set_pad_left(&set_styles.list_btn, 0);
    lv_style_set_pad_right(&set_styles.list_btn, 0);
    lv_style_set_radius(&set_styles.list_btn, 0);
    lv_style_set_bg_color(&set_styles.list_btn, lv_color_hex(0xFFFFFF));
    lv_style_set_text_color(&set_styles.list_btn, lv_color_hex(0x1F2933));
    // 底部分隔线
    lv_style_set_border_width(&set_styles.list_btn, 1);
    lv_style_set_border_width(&set_styles.list_btn, 0);
    lv_style_set_border_side(&set_styles.list_btn, LV_BORDER_SIDE_NONE);
    lv_style_set_border_color(&set_styles.list_btn, lv_color_hex(0xD1D5DB));

    // ===================== 按钮按下样式 =====================
    lv_style_set_bg_color(&set_styles.list_btn_pressed, lv_color_hex(0xEAF4FF));
    lv_style_set_text_color(&set_styles.list_btn_pressed, lv_color_hex(0x0D6EFD));
    lv_style_set_border_color(&set_styles.list_btn_pressed, lv_color_hex(0xCFE6FF));

    // ===================== 按钮悬浮样式（轻微高亮） =====================
    lv_style_set_bg_color(&set_styles.list_btn_hover, lv_color_hex(0xF0F6FF));
    lv_style_set_text_color(&set_styles.list_btn_hover, lv_color_hex(0x1F2933));

    // ===================== 列表按钮聚焦（只改圆角，无边框） =====================
    lv_style_set_outline_width(&set_styles.list_focus_key, 0);    // 关闭外框
    lv_style_set_outline_opa(&set_styles.list_focus_key, 0);      // 彻底隐藏
    lv_style_set_outline_pad(&set_styles.list_focus_key, 0);     // 👈 关键！去掉外扩，聚焦宽度=按钮宽度
    lv_style_set_radius(&set_styles.list_focus_key, 16);
}

set_screen_styles_t* get_set_screen_styles(void)
{
    if(set_styles_init_flag){
        return &set_styles;
    }
    init_set_lv_style();
    set_styles_init_flag = true;
    return &set_styles;
}
