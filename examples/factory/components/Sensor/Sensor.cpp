#include "Sensor.hpp"
#include <cstdio>
#include <new>
#include "esp_timer.h"
LV_IMAGE_DECLARE(img_app_sensor);
using namespace app_ui;
namespace esp_brookesia::apps {
Sensor *Sensor::instance_;
Sensor *Sensor::requestInstance() { if(!instance_) instance_=new(std::nothrow) Sensor; return instance_; }
Sensor::Sensor():App("Sensor",&img_app_sensor,true,false,false) {}
Sensor::~Sensor() { close(); instance_=nullptr; }
bool Sensor::run() {
    root_=screen("Sensors",event,this);
    auto *rtc=box(root_,12,54,216,78);
    label(rtc,"PCF85063A / RTC",12,6,192,&lv_font_montserrat_12,MUTED);
    clock_=label(rtc,"--:--:--",12,24,192,&lv_font_montserrat_24);
    date_=label(rtc,"--",12,56,192,&lv_font_montserrat_12,MUTED);
    auto *imu=box(root_,12,140,216,112);
    label(imu,"QMI8658 / MOTION",12,7,192,&lv_font_montserrat_12,MUTED);
    label(imu,"Accel (g)",12,29,92,&lv_font_montserrat_12,BLUE);
    label(imu,"Gyro (deg/s)",114,29,96,&lv_font_montserrat_12,0xd07a25);
    for(int i=0;i<6;++i) values_[i]=label(imu,"--",12+(i/3)*102,49+(i%3)*20,96,&lv_font_montserrat_14);
    auto *env=box(root_,12,260,216,40);
    label(env,"SHTC3",10,4,196,&lv_font_montserrat_12,MUTED);
    temperature_=label(env,"-- C",10,20,92,&lv_font_montserrat_14);
    humidity_=label(env,"-- %RH",112,20,96,&lv_font_montserrat_14);
    status_=label(root_,"Reading sensors...",12,305,216,&lv_font_montserrat_12,MUTED);
    timer_=lv_timer_create(poll,100,this);
    if(!timer_ || !start("sensor",4096)) { close(); return false; }
    return true;
}
void Sensor::work() {
    int64_t next_rtc=0,last_imu=0;
    while(!stopping()) {
        auto now=esp_timer_get_time();
        if(now>=next_rtc) {
            bsp_sensor_datetime_t t={}; auto err=bsp_rtc_get_datetime(&t);
            portENTER_CRITICAL(&lock_); rtc_error_=err; if(err==ESP_OK) time_=t; portEXIT_CRITICAL(&lock_);
            float temperature=0,humidity=0;
            err=bsp_environment_get_data(&temperature,&humidity);
            portENTER_CRITICAL(&lock_);
            environment_error_=err;
            if(err==ESP_OK) { temperature_c_=temperature; humidity_percent_=humidity; }
            portEXIT_CRITICAL(&lock_);
            next_rtc=now+1000000;
        }
        bsp_imu_data_t m={}; auto err=bsp_imu_get_data(&m);
        if(err==ESP_OK) last_imu=now;
        if(err!=ESP_ERR_NOT_FINISHED || now-last_imu>1000000) {
            portENTER_CRITICAL(&lock_); imu_error_=err; if(err==ESP_OK) motion_=m; portEXIT_CRITICAL(&lock_);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
void Sensor::refresh() {
    bsp_sensor_datetime_t t; bsp_imu_data_t m; esp_err_t rtc,imu;
    portENTER_CRITICAL(&lock_); t=time_; m=motion_; rtc=rtc_error_; imu=imu_error_; portEXIT_CRITICAL(&lock_);
    float temperature,humidity; esp_err_t environment;
    portENTER_CRITICAL(&lock_);
    temperature=temperature_c_; humidity=humidity_percent_; environment=environment_error_;
    portEXIT_CRITICAL(&lock_);
    char s[80];
    if(rtc==ESP_OK) {
        snprintf(s,sizeof(s),"%02u:%02u:%02u",t.hour,t.minute,t.second); text(clock_,s);
        snprintf(s,sizeof(s),"%04u-%02u-%02u",t.year,t.month,t.day); text(date_,s);
    } else { text(clock_,"--:--:--"); text(date_,"RTC unavailable"); }
    float data[]={m.accel_x,m.accel_y,m.accel_z,m.gyro_x,m.gyro_y,m.gyro_z};
    for(int i=0;i<6;++i) {
        if(imu==ESP_OK) snprintf(s,sizeof(s),"%c  %+.2f","XYZ"[i%3],(double)data[i]);
        else snprintf(s,sizeof(s),"%c  --","XYZ"[i%3]);
        text(values_[i],s);
    }
    if(environment==ESP_OK) snprintf(s,sizeof(s),"%.1f C",(double)temperature);
    else snprintf(s,sizeof(s),"-- C");
    text(temperature_,s);
    if(environment==ESP_OK) snprintf(s,sizeof(s),"%.1f %%RH",(double)humidity);
    else snprintf(s,sizeof(s),"-- %%RH");
    text(humidity_,s);
    snprintf(s,sizeof(s),"RTC %s / IMU %s / SHT %s",rtc==ESP_OK?"OK":"--",imu==ESP_OK?"OK":"--",environment==ESP_OK?"OK":"--");
    text(status_,s);
}
void Sensor::poll(lv_timer_t *t) { static_cast<Sensor *>(lv_timer_get_user_data(t))->refresh(); }
void Sensor::event(lv_event_t *e) { static_cast<Sensor *>(lv_event_get_user_data(e))->back(); }
bool Sensor::pause() { if(timer_) lv_timer_pause(timer_); stop(); return true; }
bool Sensor::resume() { if(timer_) lv_timer_resume(timer_); return start("sensor",4096); }
bool Sensor::close() { stop(); if(timer_) { lv_timer_delete(timer_); timer_=nullptr; } if(root_) lv_obj_delete(root_); root_=nullptr; return true; }
}
