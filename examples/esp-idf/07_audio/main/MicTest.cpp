/* SPDX-License-Identifier: Apache-2.0 */
#include "MicTest.hpp"
#include "bsp_audio_session.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/idf_additions.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace {
constexpr unsigned RATE = 16000, MAX_SAMPLES = RATE * 10;
lv_obj_t *label(lv_obj_t *parent, const char *text, int y, int width, const lv_font_t *font)
{
    auto *obj = lv_label_create(parent);
    lv_obj_set_width(obj, width);
    lv_obj_set_style_text_font(obj, font, 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(0x1d2b42), 0);
    lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(obj, text);
    lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
    return obj;
}
}
namespace audio_demo {
MicTest::MicTest() = default;
MicTest::~MicTest() { close(); if (done_) vSemaphoreDelete(done_); }
bool MicTest::init() { if (!done_) done_ = xSemaphoreCreateBinary(); return done_ != nullptr; }
void MicTest::publish(const View &v) { portENTER_CRITICAL(&lock_); view_ = v; portEXIT_CRITICAL(&lock_); }
MicTest::View MicTest::snapshot() { portENTER_CRITICAL(&lock_); View v = view_; portEXIT_CRITICAL(&lock_); return v; }

bool MicTest::run()
{
    root_ = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(root_);
    lv_obj_set_size(root_, 240, 320);
    lv_obj_remove_flag(root_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(root_, lv_color_hex(0xe9eff8), 0);
    lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);
    label(root_, "Mic Test", 14, 216, &lv_font_montserrat_20);
    auto make_button = [&](const char *text, int x, int y, int width, uintptr_t id, bool primary) {
        auto *btn = lv_button_create(root_);
        lv_obj_remove_style_all(btn);
        lv_obj_set_pos(btn, x, y); lv_obj_set_size(btn, width, 32);
        lv_obj_set_style_radius(btn, 12, 0);
        lv_obj_set_style_bg_opa(btn, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(btn, lv_color_hex(primary ? 0x376df6 : 0xffffff), 0);
        lv_obj_set_style_bg_color(btn, lv_color_hex(primary ? 0x244eb5 : 0xe5ecf8), LV_STATE_PRESSED);
        lv_obj_set_style_opa(btn, LV_OPA_40, LV_STATE_DISABLED);
        auto *caption = label(btn, text, 0, width - 8, &lv_font_montserrat_16);
        lv_obj_center(caption);
        lv_obj_set_style_text_color(caption, primary ? lv_color_white() : lv_color_hex(0x1d2b42), 0);
        lv_obj_set_user_data(btn, reinterpret_cast<void *>(id));
        lv_obj_add_event_cb(btn, event, LV_EVENT_CLICKED, this);
        return btn;
    };
    auto *card = lv_obj_create(root_);
    lv_obj_remove_style_all(card);
    lv_obj_set_pos(card, 12, 54); lv_obj_set_size(card, 216, 156);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(card, 16, 0);
    lv_obj_set_style_bg_color(card, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    auto *subtitle = label(card, "MICROPHONE", 12, 200, &lv_font_montserrat_12);
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0x78859a), 0);
    level_ = label(card, "--", 37, 200, &lv_font_montserrat_32);
    lv_obj_set_style_text_color(level_, lv_color_hex(0x376df6), 0);
    meter_ = lv_bar_create(card);
    lv_obj_set_pos(meter_, 18, 83); lv_obj_set_size(meter_, 180, 12);
    lv_bar_set_range(meter_, 0, 100);
    lv_obj_set_style_bg_color(meter_, lv_color_hex(0xe5ecf8), 0);
    lv_obj_set_style_bg_color(meter_, lv_color_hex(0x376df6), LV_PART_INDICATOR);
    auto *hint = label(card, "Speak to check the microphone\n16 kHz / Mono / 16-bit", 112, 200, &lv_font_montserrat_12);
    lv_obj_set_style_text_color(hint, lv_color_hex(0x78859a), 0);
    time_ = label(root_, "0.0 / 10.0 s", 218, 216, &lv_font_montserrat_16);
    status_ = label(root_, "Preparing microphone...", 242, 216, &lv_font_montserrat_12);
    lv_obj_set_height(status_, 32);
    record_ = make_button("Record", 12, 280, 102, 1, true);
    play_ = make_button("Play", 126, 280, 102, 2, false);
    record_label_ = lv_obj_get_child(record_, 0); play_label_ = lv_obj_get_child(play_, 0);
    recorded_ = 0;
    timer_ = lv_timer_create(poll, 50, this);
    if (!timer_ || !start()) { close(); return false; }
    refresh();
    return true;
}

bool MicTest::start()
{
    if (started_) return true;
    if (!done_) return false;
    xSemaphoreTake(done_, 0);
    stopping_ = false; command_ = Command::None;
    publish(View{});
    started_ = xTaskCreateWithCaps(entry, "mic_test", 6144, this, 3, nullptr,
                                  MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT) == pdPASS;
    return started_;
}
void MicTest::stop()
{
    if (!started_) return;
    stopping_ = true;
    xSemaphoreTake(done_, portMAX_DELAY);
    started_ = false;
}
void MicTest::entry(void *arg)
{
    auto *self = static_cast<MicTest *>(arg);
    self->work();
    xSemaphoreGive(self->done_);
    vTaskDeleteWithCaps(nullptr);
}
void MicTest::work()
{
    View v;
    const int64_t entered=esp_timer_get_time();
    bool first_pcm=true,first_nonzero=true;
    auto elapsed=[&] { return (unsigned)((esp_timer_get_time()-entered)/1000); };
    auto status = [&](const char *s) { snprintf(v.status, sizeof(v.status), "%s", s); publish(v); };
    if (bsp_extra_codec_init() != ESP_OK) {
        status("Audio initialization failed. Press RESET to retry."); return;
    }
    if (!bsp_extra_audio_acquire(BSP_EXTRA_AUDIO_OWNER_RECORDER, 1000)) {
        status("Audio is busy. Press RESET to retry."); return;
    }
    ESP_LOGI("mic_test","Session acquired in %u ms",elapsed());
    if (bsp_extra_codec_set_fs(RATE, 16, I2S_SLOT_MODE_MONO) != ESP_OK) {
        status("Microphone initialization failed."); bsp_extra_audio_release(); return;
    }
    bsp_extra_codec_mute_set(true);
    ESP_LOGI("mic_test","Codec ready in %u ms; waiting for first PCM",elapsed());
    v.playable = recorded_ > 0;
    v.recorded_ms = recorded_ * 1000 / RATE;
    status("Waiting for microphone data...");
    int16_t samples[256]; size_t playback = 0;
    while (!stopping_) {
        const Command cmd = command_.exchange(Command::None);
        if (cmd == Command::Stop) {
            if (v.mode == Mode::Play) bsp_extra_audio_drain();
            v.mode = Mode::Monitor; bsp_extra_codec_mute_set(true);
            v.playable = recorded_ > 0; v.elapsed_ms = 0;
            status("Stopped. Tap Play to listen.");
        } else if (cmd == Command::Record) {
            if(!recording_) recording_=static_cast<int16_t *>(heap_caps_malloc(MAX_SAMPLES*sizeof(int16_t),MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT));
            if(!recording_) { status("Not enough memory to record 10 s."); continue; }
            bsp_extra_codec_mute_set(true);
            v.mode = Mode::Record; recorded_ = 0; v.playable = false; v.elapsed_ms = 0; v.recorded_ms = 0;
            status("Recording. Tap Stop to finish early.");
        } else if (cmd == Command::Play && recorded_) {
            playback = 0; v.mode = Mode::Play; v.elapsed_ms = 0;
            bsp_extra_codec_mute_set(false); status("Playing your recording.");
        }
        if (v.mode == Mode::Play) {
            const size_t count = std::min<size_t>(256, recorded_ - playback);
            size_t written = 0;
            esp_err_t ret = bsp_extra_i2s_write(recording_ + playback, count * 2, &written, 500);
            if (ret != ESP_OK || written != count * 2) { v.ready = false; status("Audio playback failed."); break; }
            playback += count; v.elapsed_ms = playback * 1000 / RATE;
            if (playback == recorded_) {
                bsp_extra_audio_drain(); bsp_extra_codec_mute_set(true);
                v.mode = Mode::Monitor; v.elapsed_ms = 0;
                status("Playback finished. Tap Play to replay.");
            }
        } else {
            size_t bytes = 0;
            esp_err_t ret = bsp_extra_i2s_read(samples, sizeof(samples), &bytes, 500);
            if (ret != ESP_OK || !bytes) { v.ready = false; status("Microphone read failed."); break; }
            size_t count = bytes / sizeof(int16_t);
            double sum = 0;
            for (size_t i = 0; i < count; ++i) sum += double(samples[i]) * samples[i];
            if(first_pcm) {
                first_pcm=false; v.ready=true;
                ESP_LOGI("mic_test","First PCM in %u ms (%u samples)",elapsed(),(unsigned)count);
                status("Record up to 10 s, then play back.");
            }
            if(first_nonzero && sum>0) {
                first_nonzero=false;
                ESP_LOGI("mic_test","First nonzero PCM in %u ms",elapsed());
            }
            const double rms = std::sqrt(sum / count);
            // Map -60..0 dBFS onto the level meter, as in factory Mic Test.
            v.level = std::clamp<int>(int((20 * std::log10(std::max(rms, 1.0) / 32768.0) + 60) * 100 / 60), 0, 100);
            if (v.mode == Mode::Record) {
                count = std::min<size_t>(count, MAX_SAMPLES - recorded_);
                memcpy(recording_ + recorded_, samples, count * sizeof(int16_t));
                recorded_ += count; v.elapsed_ms = recorded_ * 1000 / RATE;
                v.recorded_ms = v.elapsed_ms;
                if (recorded_ == MAX_SAMPLES) {
                    v.mode = Mode::Monitor; v.playable = true;
                    status("10 seconds recorded. Tap Play.");
                }
            }
        }
        publish(v);
    }
    if (v.mode == Mode::Play) bsp_extra_audio_drain();
    bsp_extra_audio_release();
    v.ready = false; publish(v);
    ESP_LOGI("mic_test", "Microphone stopped; shared codec released");
}

void MicTest::refresh()
{
    if (!root_) return;
    const View v = snapshot();
    char text[64];
    if(v.ready) snprintf(text,sizeof(text),"%u%%",v.level);
    else snprintf(text,sizeof(text),"--");
    if (strcmp(lv_label_get_text(level_), text)) lv_label_set_text(level_, text);
    lv_bar_set_value(meter_, v.level, LV_ANIM_OFF);
    if (v.mode == Mode::Play) {
        snprintf(text, sizeof(text), "PLAY %u.%u / %u.%u s", v.elapsed_ms / 1000, v.elapsed_ms / 100 % 10,
                 v.recorded_ms / 1000, v.recorded_ms / 100 % 10);
    } else {
        const unsigned elapsed = v.mode == Mode::Record ? v.elapsed_ms : v.recorded_ms;
        snprintf(text, sizeof(text), "%s%u.%u / 10.0 s", v.mode == Mode::Record ? "REC " : "",
                 elapsed / 1000, elapsed / 100 % 10);
    }
    if (strcmp(lv_label_get_text(time_), text)) lv_label_set_text(time_, text);
    if (strcmp(lv_label_get_text(status_), v.status)) lv_label_set_text(status_, v.status);
    const char *record_text=v.mode==Mode::Record ? "Stop" : "Record";
    const char *play_text=v.mode==Mode::Play ? "Stop" : "Play";
    if(strcmp(lv_label_get_text(record_label_),record_text)) lv_label_set_text(record_label_,record_text);
    if(strcmp(lv_label_get_text(play_label_),play_text)) lv_label_set_text(play_label_,play_text);
    if (v.ready && v.mode != Mode::Play) lv_obj_remove_state(record_, LV_STATE_DISABLED);
    else lv_obj_add_state(record_, LV_STATE_DISABLED);
    if (v.ready && v.mode != Mode::Record && v.playable) lv_obj_remove_state(play_, LV_STATE_DISABLED);
    else lv_obj_add_state(play_, LV_STATE_DISABLED);
}
void MicTest::event(lv_event_t *e)
{
    auto *self = static_cast<MicTest *>(lv_event_get_user_data(e));
    auto *button = static_cast<lv_obj_t *>(lv_event_get_current_target(e));
    const uintptr_t id = reinterpret_cast<uintptr_t>(lv_obj_get_user_data(button));
    const View v = self->snapshot();
    if (!v.ready) return;
    self->command_ = v.mode != Mode::Monitor ? Command::Stop : id == 1 ? Command::Record : Command::Play;
}
void MicTest::poll(lv_timer_t *timer) { static_cast<MicTest *>(lv_timer_get_user_data(timer))->refresh(); }
bool MicTest::close()
{
    if (timer_) { lv_timer_delete(timer_); timer_ = nullptr; }
    stop();
    if (root_) { lv_obj_delete(root_); root_ = nullptr; }
    heap_caps_free(recording_); recording_ = nullptr; recorded_ = 0;
    return true;
}
}
