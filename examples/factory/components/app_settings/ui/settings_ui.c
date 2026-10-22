#include "settings_ui.h"
#include "set_lv_style.h"
#include "assets/app_setting_assets.h"
#include "set_about.h"
#include "set_wifi_service.h"

static lv_group_t *s_app_group;
static lv_obj_t *setting_main_screen; 
static lv_obj_t *wifi_screen;  
static lv_obj_t *about_screen;  
static lv_obj_t *setting_list;
static lv_obj_t *tf_label;
static settings_close_cb_t s_close_cb;

static lv_obj_t *add_button_with_arrow(lv_obj_t *list,const void *icon, const char *text, lv_event_cb_t cb, void *user_data);
static lv_obj_t *add_button_with_slider(lv_obj_t *list,const void *icon, const char *text, lv_event_cb_t cb, void *user_data);
static lv_obj_t *add_button_with_label(lv_obj_t *list,const void *icon, const char *text);


static void wifi_btn_click_cb(lv_event_t *e)
{
    lv_screen_load(wifi_screen); 
    open_wifi_page();
}

static void about_btn_click_cb(lv_event_t *e)
{
    lv_screen_load(about_screen); 
    open_about_page();
}

static void back_btn_click_cb(lv_event_t *e)
{
    if (s_close_cb != NULL) {
        s_close_cb();
    }
}

static void slider_value_change_brightness_cb(lv_event_t *e)
{
    lv_obj_t *slider = (lv_obj_t *)lv_event_get_target(e);
    uint32_t cur_val = lv_slider_get_value(slider) * 10;

    lv_obj_t *label = lv_obj_get_child(slider, 0);
    lv_label_set_text_fmt(label, "%ld", cur_val);
    lv_obj_center(label); 

    bsp_display_brightness_set(cur_val);
}

void settings_ui_screen_init(settings_close_cb_t close_cb)
{
    s_close_cb = close_cb;
    s_app_group = lv_group_create();
    lv_indev_set_group(bsp_display_get_input_dev(), s_app_group);

    setting_main_screen = lv_screen_active(); 
    lv_obj_remove_flag(setting_main_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(setting_main_screen, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(setting_main_screen, LV_OPA_COVER, LV_PART_MAIN);  // 不透明

    setting_list = lv_list_create(setting_main_screen); 
    set_screen_styles_t *styles = get_set_screen_styles();
    lv_obj_add_style(setting_list, &styles->list, 0);
    lv_obj_set_size(setting_list, 240, 320);
    lv_obj_align(setting_list, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_scrollbar_mode(setting_list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(setting_list, LV_DIR_VER); 
    //lv_obj_set_scroll_snap_y(setting_list, LV_SCROLL_SNAP_CENTER);
    
    //
    lv_obj_t *item;
    lv_obj_t *title;
    title = lv_list_add_text(setting_list, "Wireless");
    lv_obj_add_style(title, &styles->list_title, 0); 
    lv_obj_set_height(title, 37);
    lv_obj_set_style_translate_y(title, -6, 0);
    item = add_button_with_arrow(setting_list, &icon_wifi, "WLAN", wifi_btn_click_cb, NULL);


    title = lv_list_add_text(setting_list, "Media");
    lv_obj_add_style(title, &styles->list_title, 0); 
    lv_obj_set_height(title, 37);
    lv_obj_set_style_translate_y(title, 7, 0);
    item = add_button_with_slider(setting_list, &icon_backlights, "", slider_value_change_brightness_cb, NULL);
    
    item = add_button_with_arrow(setting_list,&icon_info,"About",about_btn_click_cb,NULL);

    tf_label = add_button_with_label(setting_list,&icon_tf_card,"TF_CARD");
    sdmmc_card_t *sdcard = bsp_sdcard_get_handle();
    if (sdcard != NULL)
    {
        uint64_t total_bytes = (uint64_t)sdcard->csd.capacity * sdcard->csd.sector_size;
        uint32_t total_mb = (uint32_t)(total_bytes / (1024 * 1024));
        lv_label_set_text_fmt(tf_label, "%lu MB",total_mb); 
    } else {
        lv_label_set_text(tf_label, "Not mounted");
    }

    item = lv_list_add_button(setting_list, &icon_back, "Back");
    lv_obj_set_style_layout(item, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_flex_main_place(item, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(item, LV_FLEX_ALIGN_CENTER, 0);
    
    lv_obj_add_style(item, &styles->list_btn, LV_STATE_DEFAULT);
    lv_obj_add_style(item, &styles->list_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_style(item, &styles->list_btn_hover, LV_STATE_HOVERED);
    lv_obj_add_style(item, &styles->list_focus_key, LV_STATE_FOCUS_KEY);
    lv_obj_add_event_cb(item, back_btn_click_cb, LV_EVENT_CLICKED, NULL);
    lv_group_add_obj(s_app_group, item);

    wifi_screen = lv_obj_create(NULL);
    about_screen = lv_obj_create(NULL);

    init_wifi_settings_screen(wifi_screen,setting_main_screen,s_app_group);
    init_about_settings_screen(about_screen,setting_main_screen,s_app_group);
    
    
}



















/**
 * @brief Create a list button with a right-side arrow icon
 * 
 * This function creates a list button object with specified icon, text, and a right-aligned arrow icon.
 * It applies pre-defined styles (default/pressed/hovered) and binds click callback if provided.
 * 
 * @param icon      Pointer to LVGL image descriptor (LV_IMG_DECLARE) for the left icon, NULL for no icon
 * @param text      Text string to display on the button (UTF-8 supported), must be non-NULL
 * @param cb        Event callback function for LV_EVENT_CLICKED event, NULL for no callback
 * @param user_data User-defined data to pass to the callback function, NULL for no user data
 * @return          None (void)
 * 
 * @note Performs null pointer checks to prevent runtime crashes
 */
static lv_obj_t *add_button_with_arrow(lv_obj_t *list,const void *icon, const char *text, lv_event_cb_t cb, void *user_data)
{
    // Guard clause: Return early if global list object is not initialized
    if(list == NULL) return NULL; 
    
    set_screen_styles_t *styles = get_set_screen_styles();

    // 1. Create list button object
    lv_obj_t *btn = lv_list_add_button(list, icon, text); 
    lv_obj_set_style_layout(btn, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_flex_main_place(btn, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(btn, LV_FLEX_ALIGN_CENTER, 0);

    // 2. Apply style configurations to button
    lv_obj_add_style(btn, &styles->list_btn, LV_STATE_DEFAULT);
    lv_obj_add_style(btn, &styles->list_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_style(btn, &styles->list_btn_hover, LV_STATE_HOVERED);
    lv_obj_add_style(btn, &styles->list_focus_key, LV_STATE_FOCUS_KEY);
    // 3. Bind click event callback if provided
    if (cb != NULL) {
        lv_obj_add_event_cb(btn, cb, LV_EVENT_CLICKED, user_data);
    }

    // 4. Create and configure right-side arrow icon
    lv_obj_t *arrow_label = lv_label_create(btn);
    lv_label_set_text(arrow_label, LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_color(arrow_label, lv_color_hex(0x0D6EFD), LV_STATE_DEFAULT);
    lv_obj_align(arrow_label, LV_ALIGN_RIGHT_MID, -12, 0);

    lv_group_add_obj(s_app_group, btn);
    return btn;
}


/**
 * @brief Create a list button with an embedded slider component
 * 
 * This function creates a non-clickable list button containing a slider on the right side.
 * The slider is pre-configured with a range of 1-100, default value of 100, and custom styling.
 * A numeric label displaying the current slider value is centered on the slider knob.
 * 
 * @param icon      Pointer to LVGL image descriptor (LV_IMG_DECLARE) for the left icon, NULL for no icon
 * @param text      Text string to display on the button (UTF-8 supported), must be non-NULL
 * @param cb        Event callback function for LV_EVENT_CLICKED event (unused as button is non-clickable), NULL for no callback
 * @param user_data User-defined data to pass to the callback function, NULL for no user data
 * @return          None (void)
 * 
 * @note The button is set as non-clickable (LV_OBJ_FLAG_CLICKABLE removed) to prioritize slider interaction
 */
static lv_obj_t *add_button_with_slider(lv_obj_t *list,const void *icon, const char *text, lv_event_cb_t cb, void *user_data)
{
    if(list == NULL) return NULL;
    
    set_screen_styles_t *styles = get_set_screen_styles();

    // 1. Create list button object
    lv_obj_t *btn = lv_list_add_button(list, icon, text); 
    lv_obj_set_style_layout(btn, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_flex_main_place(btn, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(btn, LV_FLEX_ALIGN_CENTER, 0);
    
    lv_obj_add_style(btn, &styles->list_btn, LV_STATE_DEFAULT);
    lv_obj_add_style(btn, &styles->list_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_style(btn, &styles->list_btn_hover, LV_STATE_HOVERED);
    lv_obj_add_style(btn, &styles->list_focus_key, LV_STATE_FOCUS_KEY);
    lv_obj_remove_flag(btn, LV_OBJ_FLAG_CLICKABLE); // Disable button clickability

    // 2. Create slider container
    lv_obj_t *slider_cont = lv_obj_create(btn);
    lv_obj_set_size(slider_cont, 140, 32);
    lv_obj_align(slider_cont, LV_ALIGN_RIGHT_MID, -2, 0);
    lv_obj_set_style_bg_opa(slider_cont, LV_OPA_TRANSP, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(slider_cont, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(slider_cont, 0, LV_STATE_DEFAULT);

    // 3. Create slider component
    lv_obj_t *slider = lv_slider_create(slider_cont);
    lv_obj_set_size(slider, 132, 20);
    lv_obj_align(slider, LV_ALIGN_CENTER, 0, 0);
    lv_slider_set_range(slider, 1, 10);
    lv_slider_set_value(slider, 100, LV_ANIM_OFF);
    lv_obj_set_style_opa(slider, LV_OPA_TRANSP, LV_PART_KNOB);
    
    // 4. Configure slider styling
    lv_obj_set_style_radius(slider, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_INDICATOR | LV_STATE_DEFAULT); 
    lv_obj_set_style_shadow_width(slider, 2, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(slider, LV_OPA_30, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0xFFBF00), LV_PART_INDICATOR);

    // 5. Create numeric label for slider value
    lv_obj_t *slider_label = lv_label_create(slider);
    lv_label_set_text_fmt(slider_label, "%ld", lv_slider_get_value(slider) * 10);
    lv_obj_set_style_text_color(slider_label, lv_color_hex(0x1F2933), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(slider_label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_center(slider_label);

    // 6. Bind callback and user data (if provided)
    if (cb != NULL) {
        lv_obj_add_event_cb(slider, cb, LV_EVENT_VALUE_CHANGED, user_data);
    }

    lv_group_add_obj(s_app_group, slider);

    return slider;
}

/**
 * @brief Create a list button with a right-aligned label component
 * 
 * This function creates a clickable list button with a left icon/text and an empty right-aligned label.
 * The right label is pre-styled with black color, montserrat 22 font, and 10px left margin from the button edge.
 * Returns the right label object to allow dynamic text updates after creation.
 * 
 * @param icon  Pointer to LVGL image descriptor (LV_IMG_DECLARE) for the left icon, NULL for no icon
 * @param text  Text string to display on the left side of the button (UTF-8 supported), must be non-NULL
 * @return      Pointer to the created right-side label object (lv_obj_t *) | NULL if creation fails (e.g., invalid list object)
 * 
 * @note Uses LVGL9 API (lv_list_add_btn instead of lv_list_add_button)
 */
static lv_obj_t *add_button_with_label(lv_obj_t *list,const void *icon, const char *text)
{
    if(list == NULL) return NULL;
    
    set_screen_styles_t *styles = get_set_screen_styles();
    
    // 1. Create list button object (LVGL9 API fix: lv_list_add_btn)
    lv_obj_t *btn = lv_list_add_btn(list, icon, text); 
    lv_obj_set_style_layout(btn, LV_LAYOUT_FLEX, 0);
    lv_obj_set_style_flex_main_place(btn, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(btn, LV_FLEX_ALIGN_CENTER, 0);
    
    lv_obj_add_style(btn, &styles->list_btn, LV_STATE_DEFAULT);
    lv_obj_add_style(btn, &styles->list_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_style(btn, &styles->list_btn_hover, LV_STATE_HOVERED);
    lv_obj_add_style(btn, &styles->list_focus_key, LV_STATE_FOCUS_KEY);
    lv_obj_remove_flag(btn, LV_OBJ_FLAG_CLICKABLE); // Enable button clickability

    // 2. Create right-aligned label component
    lv_obj_t *char_label = lv_label_create(btn);
    lv_label_set_text(char_label, " "); // Initialize with empty space
    lv_obj_set_style_text_color(char_label, lv_color_hex(0x1F2933), LV_STATE_DEFAULT);
    lv_obj_align(char_label, LV_ALIGN_RIGHT_MID, -10, 0); // Align to right middle (10px left margin)

    lv_group_add_obj(s_app_group, btn);

    return char_label;
}


void settings_ui_screen_deinit(void)
{
    close_wifi_settings_screen();
    if (wifi_screen != NULL) {
        lv_obj_delete(wifi_screen);
        wifi_screen = NULL;
    }
    if (about_screen != NULL) {
        lv_obj_delete(about_screen);
        about_screen = NULL;
    }
    if (s_app_group != NULL) {
        lv_group_delete(s_app_group);
        s_app_group = NULL;
    }
    setting_main_screen = NULL;
    setting_list = NULL;
    tf_label = NULL;
    s_close_cb = NULL;
}
