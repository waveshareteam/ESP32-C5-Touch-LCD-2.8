#pragma once
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <atomic>
#include <cstring>

namespace app_ui {
constexpr uint32_t BG=0xe9eff8, INK=0x1d2b42, MUTED=0x78859a, BLUE=0x376df6;
inline lv_obj_t *box(lv_obj_t *p,int x,int y,int w,int h,uint32_t color=0xffffff,int radius=16) {
    auto *o=lv_obj_create(p); lv_obj_remove_style_all(o);
    lv_obj_set_pos(o,x,y); lv_obj_set_size(o,w,h);
    lv_obj_set_style_bg_color(o,lv_color_hex(color),0); lv_obj_set_style_bg_opa(o,LV_OPA_COVER,0);
    lv_obj_set_style_radius(o,radius,0); lv_obj_remove_flag(o,LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_CLICKABLE); return o;
}
inline lv_obj_t *label(lv_obj_t *p,const char *s,int x,int y,int w=216,const lv_font_t *font=&lv_font_montserrat_16,uint32_t color=INK) {
    auto *o=lv_label_create(p); lv_obj_set_pos(o,x,y); lv_obj_set_width(o,w);
    lv_obj_set_style_text_font(o,font,0); lv_obj_set_style_text_color(o,lv_color_hex(color),0);
    lv_label_set_text(o,s); lv_obj_remove_flag(o,LV_OBJ_FLAG_CLICKABLE); return o;
}
inline void text(lv_obj_t *o,const char *s) { if(o && std::strcmp(lv_label_get_text(o),s)) lv_label_set_text(o,s); }
inline lv_obj_t *button(lv_obj_t *p,const char *s,int x,int y,int w,int h,int action,lv_event_cb_t cb,void *ctx,bool primary=false) {
    auto *o=box(p,x,y,w,h,primary?BLUE:0xffffff,12); lv_obj_add_flag(o,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(o,lv_color_hex(primary?0x244eb5:0xe5ecf8),LV_STATE_PRESSED);
    lv_obj_set_style_opa(o,LV_OPA_40,LV_STATE_DISABLED);
    lv_obj_set_user_data(o,reinterpret_cast<void *>(static_cast<intptr_t>(action)));
    lv_obj_add_event_cb(o,cb,LV_EVENT_CLICKED,ctx);
    if(s) { auto *l=label(o,s,0,0,w,&lv_font_montserrat_16,primary?0xffffff:INK);
        lv_obj_set_style_text_align(l,LV_TEXT_ALIGN_CENTER,0); lv_obj_center(l); }
    return o;
}
inline int action(lv_event_t *e) { return static_cast<int>(reinterpret_cast<intptr_t>(lv_obj_get_user_data(static_cast<lv_obj_t *>(lv_event_get_current_target(e))))); }
inline void enabled(lv_obj_t *o,bool yes) { if(!o) return; if(yes) lv_obj_remove_state(o,LV_STATE_DISABLED); else lv_obj_add_state(o,LV_STATE_DISABLED); }
inline lv_obj_t *screen(const char *title,lv_event_cb_t cb,void *ctx) {
    auto *root=box(lv_screen_active(),0,0,BSP_LCD_H_RES,BSP_LCD_V_RES,BG,0);
    button(root,LV_SYMBOL_LEFT,10,10,34,32,0,cb,ctx);
    label(root,title,54,14,176,&lv_font_montserrat_20); return root;
}
}
class AppWorker {
public:
    virtual ~AppWorker() { configASSERT(!active_); }
    bool start(const char *name,uint32_t stack=6144,UBaseType_t priority=2) {
        if(active_) return true;
        stop_=false; active_=true;
        if(xTaskCreate(entry,name,stack,this,priority,nullptr)!=pdPASS) { active_=false; return false; }
        return true;
    }
    void stop() { stop_=true; while(active_) vTaskDelay(pdMS_TO_TICKS(10)); }
protected:
    bool stopping() const { return stop_.load(); }
    virtual void work()=0;
private:
    static void entry(void *p) { auto *w=static_cast<AppWorker *>(p); w->work(); w->active_=false; vTaskDelete(nullptr); }
    std::atomic<bool> stop_{false},active_{false};
};
