/* SPDX-License-Identifier: Apache-2.0 */
#include "Qmi8658Ball.hpp"
#include "esp_timer.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <new>
LV_IMAGE_DECLARE(img_app_qmi8658ball);
using namespace app_ui;
namespace esp_brookesia::apps {
Qmi8658Ball *Qmi8658Ball::instance_;
Qmi8658Ball *Qmi8658Ball::requestInstance()
{
    if(!instance_) instance_=new(std::nothrow) Qmi8658Ball;
    return instance_;
}
Qmi8658Ball::Qmi8658Ball():App("GravityBall",&img_app_qmi8658ball,true,false,false) {}
Qmi8658Ball::~Qmi8658Ball() { close(); instance_=nullptr; }
bool Qmi8658Ball::run()
{
    root_=screen("Gravity Ball",event,this);
    box(root_,BSP_LCD_H_RES/2,BSP_LCD_V_RES/2-12,1,24,0xd0daeb,0);
    box(root_,BSP_LCD_H_RES/2-12,BSP_LCD_V_RES/2,24,1,0xd0daeb,0);
    ball_=box(root_,MAX_X/2,MAX_Y/2,BALL_SIZE,BALL_SIZE,BLUE,LV_RADIUS_CIRCLE);
    lv_obj_set_style_shadow_width(ball_,12,0);
    lv_obj_set_style_shadow_opa(ball_,LV_OPA_20,0);
    hint_=label(root_,"Keep level to calibrate",12,284,216,&lv_font_montserrat_12,MUTED);
    lv_obj_set_style_text_align(hint_,LV_TEXT_ALIGN_CENTER,0);
    progress_=lv_bar_create(root_);
    lv_obj_set_pos(progress_,24,305); lv_obj_set_size(progress_,192,5);
    lv_bar_set_range(progress_,0,100);
    timer_=lv_timer_create(poll,30,this);
    if(!timer_ || !start("gravity_ball",4096)) { close(); return false; }
    return true;
}
void Qmi8658Ball::work()
{
    State view;
    uint32_t presses=bsp_user_button_get_press_count();
    float sum_x=0,sum_y=0,bias_x=0,bias_y=0,min_x=0,max_x=0,min_y=0,max_y=0;
    unsigned samples=0;
    int64_t last_sample=esp_timer_get_time();
    while(!stopping()) {
        const uint32_t count=bsp_user_button_get_press_count();
        if(count!=presses) {
            presses=count; view=State{}; samples=0; sum_x=sum_y=0;
        }
        bsp_imu_data_t m={};
        view.error=bsp_imu_get_data(&m);
        const auto now=esp_timer_get_time();
        float dt=std::clamp((now-last_sample)/1000000.0f,0.001f,0.08f);
        last_sample=now;
        if(view.error==ESP_OK) {
            if(!view.calibrated) {
                if(!samples) min_x=max_x=m.accel_x,min_y=max_y=m.accel_y;
                min_x=std::min(min_x,m.accel_x); max_x=std::max(max_x,m.accel_x);
                min_y=std::min(min_y,m.accel_y); max_y=std::max(max_y,m.accel_y);
                sum_x+=m.accel_x; sum_y+=m.accel_y;
                view.progress=++samples*100/64;
                if(samples==64) {
                    if(max_x-min_x>0.05f || max_y-min_y>0.05f) {
                        samples=0; sum_x=sum_y=0; view.progress=0;
                    } else {
                        bias_x=sum_x/64; bias_y=sum_y/64;
                        view.calibrated=true;
                    }
                }
            } else {
                // Preserve the original C5 orientation: X <- accelY, Y <- -accelX.
                float ax=m.accel_y-bias_y,ay=-(m.accel_x-bias_x);
                if(std::fabs(ax)<0.005f) ax=0;
                if(std::fabs(ay)<0.005f) ay=0;
                // Clamp the ball's top-left corner to the full display bounds.
                view.x=std::clamp(view.x+ax*700*dt,0.0f,float(MAX_X));
                view.y=std::clamp(view.y+ay*700*dt,0.0f,float(MAX_Y));
            }
        }
        portENTER_CRITICAL(&lock_); state_=view; portEXIT_CRITICAL(&lock_);
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}
void Qmi8658Ball::refresh()
{
    State view; portENTER_CRITICAL(&lock_); view=state_; portEXIT_CRITICAL(&lock_);
    lv_obj_set_pos(ball_,int(view.x),int(view.y));
    text(hint_,view.error!=ESP_OK ? "IMU unavailable" : "Keep level to calibrate");
    if(view.calibrated && view.error==ESP_OK) lv_obj_add_flag(hint_,LV_OBJ_FLAG_HIDDEN);
    else lv_obj_remove_flag(hint_,LV_OBJ_FLAG_HIDDEN);
    lv_bar_set_value(progress_,view.progress,LV_ANIM_OFF);
    if(view.calibrated) lv_obj_add_flag(progress_,LV_OBJ_FLAG_HIDDEN);
    else lv_obj_remove_flag(progress_,LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(ball_);
}
void Qmi8658Ball::event(lv_event_t *e) { static_cast<Qmi8658Ball *>(lv_event_get_user_data(e))->back(); }
void Qmi8658Ball::poll(lv_timer_t *t) { static_cast<Qmi8658Ball *>(lv_timer_get_user_data(t))->refresh(); }
bool Qmi8658Ball::pause() { if(timer_) lv_timer_pause(timer_); stop(); return true; }
bool Qmi8658Ball::resume() { if(timer_) lv_timer_resume(timer_); return start("gravity_ball",4096); }
bool Qmi8658Ball::close()
{
    stop();
    if(timer_) { lv_timer_delete(timer_); timer_=nullptr; }
    if(root_) { lv_obj_delete(root_); root_=nullptr; }
    return true;
}
}
