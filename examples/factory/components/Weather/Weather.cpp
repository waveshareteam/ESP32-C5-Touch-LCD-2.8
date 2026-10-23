#include "Weather.hpp"
#include "app_support.hpp"
#include "weather_service.h"
#include "assets/weather_icons.h"
#include <cstdio>
#include <cstring>
#include <new>
LV_IMAGE_DECLARE(img_app_weather);
using namespace app_ui;
namespace {
constexpr uint32_t PAPER = 0xf6faf8, TEXT = 0x20362e, SECONDARY = 0x6a7e74;
constexpr uint32_t ACCENT = 0x16765b, RULE = 0xe0eae4, WARNING = 0xa9701e;
void single_line(lv_obj_t *object)
{
    lv_label_set_long_mode(object, LV_LABEL_LONG_DOT);
    lv_obj_set_height(object, lv_obj_get_style_text_font(object, LV_PART_MAIN)->line_height);
}
lv_obj_t *toolbar_button(lv_obj_t *parent, const char *symbol, int x, int id,
                         lv_event_cb_t callback, void *context)
{
    auto *object = button(parent, symbol, x, 10, 34, 32, id, callback, context);
    lv_obj_set_style_radius(object, 8, 0);
    lv_obj_set_style_bg_color(object, lv_color_hex(PAPER), 0);
    lv_obj_set_style_bg_color(object, lv_color_hex(0xe1eee7), LV_STATE_PRESSED);
    lv_obj_set_style_text_color(lv_obj_get_child(object, 0), lv_color_hex(ACCENT), 0);
    return object;
}
}
namespace esp_brookesia::apps {
Weather *Weather::instance_;
Weather *Weather::requestInstance() { if(!instance_) instance_=new(std::nothrow) Weather; return instance_; }
Weather::Weather():App("Weather",&img_app_weather,true,false,false) {}
Weather::~Weather() { close(); instance_=nullptr; }
bool Weather::init()
{
    // Match RLCD: start the background service when the APP is installed.
    // Keep the APP available for an on-screen retry if task creation fails.
    service_ready_=weather_service_start();
    return true;
}
bool Weather::run() {
    root_ = box(lv_screen_active(), 0, 0, 240, 320, PAPER, 0);
    toolbar_button(root_, LV_SYMBOL_LEFT, 10, 0, event, this);
    label(root_, "Weather", 54, 14, 132, &lv_font_montserrat_20, TEXT);
    refresh_button_ = toolbar_button(root_, LV_SYMBOL_REFRESH, 196, 1, event, this);
    city_ = label(root_, "Weather", 14, 58, 130, &lv_font_montserrat_24, TEXT);
    single_line(city_);
    date_ = label(root_, "--", 14, 88, 126, &lv_font_montserrat_12, SECONDARY);
    single_line(date_);
    icon_ = lv_image_create(root_);
    lv_obj_set_pos(icon_, 144, 60);
    lv_image_set_src(icon_, &weather_icons[WEATHER_UNKNOWN]);
    lv_image_set_pivot(icon_,0,0);
    lv_image_set_scale(icon_, 192);
    label(root_, "CURRENT WEATHER", 14, 118, 128, &lv_font_montserrat_12, ACCENT);
    temp_ = label(root_, "--\xc2\xb0" "C", 12, 142, 216, &lv_font_montserrat_40, TEXT);
    single_line(temp_);
    condition_ = label(root_, "Waiting for weather", 14, 190, 212, &lv_font_montserrat_18, TEXT);
    single_line(condition_);

    // A full-width detail band keeps the two measurements aligned on this display.
    auto *details = box(root_, 0, 216, 240, 68, 0xffffff, 0);
    box(details, 14, 0, 212, 1, RULE, 0);
    box(details, 120, 10, 1, 48, RULE, 0);
    label(details, "HUMIDITY", 14, 9, 100, &lv_font_montserrat_12, SECONDARY);
    label(details, "WIND", 134, 9, 100, &lv_font_montserrat_12, SECONDARY);
    humidity_ = label(details, "-- %", 14, 28, 100, &lv_font_montserrat_20, TEXT);
    wind_ = label(details, "-- m/s", 134, 28, 100, &lv_font_montserrat_18, TEXT);
    single_line(wind_);
    label(details, "Relative humidity", 14, 51, 106, &lv_font_montserrat_12, SECONDARY);
    direction_ = label(details, "Direction --", 134, 51, 100, &lv_font_montserrat_12, SECONDARY);
    single_line(direction_);
    status_dot_ = box(root_, 14, 292, 6, 6, SECONDARY, LV_RADIUS_CIRCLE);
    status_ = label(root_, "Waiting for Wi-Fi", 28, 287, 200, &lv_font_montserrat_12, SECONDARY);
    message_ = label(root_, "", 14, 304, 212, &lv_font_montserrat_12, SECONDARY);
    lv_obj_set_height(message_, 14);
    icon_condition_ = -1;
    status_color_ = 0;
    timer_=lv_timer_create(poll,500,this);
    if(!timer_) { close(); return false; }
    if(!service_ready_) service_ready_=weather_service_start();
    refresh(); return true;
}
void Weather::refresh() {
    if(!service_ready_) { text(condition_,"Weather unavailable"); text(status_,"Weather unavailable"); text(message_,"Tap refresh to try again."); return; }
    weather_snapshot_t s={}; weather_service_get_snapshot(&s);
    text(city_,s.data.city[0]?s.data.city:"Weather"); text(date_,s.data.date[0]?s.data.date:"--");
    int icon = WEATHER_UNKNOWN;
    if(s.cached) {
        char v[64]; snprintf(v,sizeof(v),"%s\xc2\xb0" "C",s.data.temp); text(temp_,v);
        // Keep unusually long readings inside the temperature and wind columns.
        const lv_font_t *temperature_font=std::strlen(s.data.temp)>5?&lv_font_montserrat_32:&lv_font_montserrat_40;
        if(lv_obj_get_style_text_font(temp_,LV_PART_MAIN)!=temperature_font)
            lv_obj_set_style_text_font(temp_,temperature_font,0);
        text(condition_,s.data.weather);
        snprintf(v,sizeof(v),"%s %%",s.data.humi); text(humidity_,v);
        snprintf(v,sizeof(v),"%s m/s",s.data.wind_speed); text(wind_,v);
        const lv_font_t *wind_font=std::strlen(s.data.wind_speed)>4?&lv_font_montserrat_14:&lv_font_montserrat_18;
        if(lv_obj_get_style_text_font(wind_,LV_PART_MAIN)!=wind_font)
            lv_obj_set_style_text_font(wind_,wind_font,0);
        snprintf(v,sizeof(v),"Direction %s",s.data.wind_dir); text(direction_,v);
        if(s.data.condition>=WEATHER_UNKNOWN && s.data.condition<WEATHER_ICON_COUNT) icon=s.data.condition;
    } else {
        text(condition_,s.state==WEATHER_LOADING?"Getting your weather":s.state==WEATHER_ERROR?"Weather unavailable":"Waiting for Wi-Fi");
    }
    if(icon_condition_!=icon) { lv_image_set_src(icon_,&weather_icons[icon]); icon_condition_=icon; }
    const char *status="Live weather", *message="";
    uint32_t color=ACCENT;
    switch(s.state) {
    case WEATHER_WAIT_NETWORK:
        status=s.cached?"Offline / saved reading":"Waiting for Wi-Fi";
        message="Connect in Settings > Wi-Fi setup."; color=SECONDARY; break;
    case WEATHER_LOADING:
        status=s.cached?"Updating / saved reading":"Updating weather"; break;
    case WEATHER_ERROR:
        status=s.cached?"Saved reading":"Weather unavailable";
        message=s.message; color=WARNING; break;
    case WEATHER_READY: break;
    }
    text(status_,status); text(message_,message);
    if(status_color_!=color) {
        lv_obj_set_style_bg_color(status_dot_,lv_color_hex(color),0);
        lv_obj_set_style_text_color(status_,lv_color_hex(color),0);
        status_color_=color;
    }
    enabled(refresh_button_,s.state!=WEATHER_LOADING);
}
void Weather::event(lv_event_t *e) {
    auto *a=static_cast<Weather *>(lv_event_get_user_data(e));
    if(action(e)==0) { a->back(); return; }
    if(!a->service_ready_) a->service_ready_=weather_service_start();
    weather_service_request_refresh(); a->refresh();
}
void Weather::poll(lv_timer_t *t) { static_cast<Weather *>(lv_timer_get_user_data(t))->refresh(); }
bool Weather::pause() { if(timer_) lv_timer_pause(timer_); return true; }
bool Weather::resume() { if(timer_) lv_timer_resume(timer_); refresh(); return true; }
bool Weather::close() { if(timer_) { lv_timer_delete(timer_); timer_=nullptr; } if(root_) lv_obj_delete(root_); root_=nullptr; return true; }
}
