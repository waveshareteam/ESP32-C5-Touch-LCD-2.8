/* SPDX-License-Identifier: Apache-2.0 */
#pragma once
#include <atomic>
#include "bsp/esp-bsp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

namespace audio_demo {
class MicTest {
public:
    MicTest();
    ~MicTest();
    bool init();
    bool run();
    bool close();
private:
    enum class Command { None, Record, Stop, Play };
    enum class Mode { Monitor, Record, Play };
    struct View {
        Mode mode = Mode::Monitor;
        unsigned level = 0, elapsed_ms = 0, recorded_ms = 0;
        bool ready = false, playable = false;
        char status[112] = "Preparing microphone...";
    };
    static void entry(void *);
    static void event(lv_event_t *);
    static void poll(lv_timer_t *);
    bool start();
    void stop();
    void work();
    void refresh();
    void publish(const View &);
    View snapshot();
    std::atomic<bool> stopping_{false};
    std::atomic<Command> command_{Command::None};
    bool started_ = false;
    SemaphoreHandle_t done_ = nullptr;
    portMUX_TYPE lock_ = portMUX_INITIALIZER_UNLOCKED;
    View view_;
    int16_t *recording_ = nullptr;
    size_t recorded_ = 0;
    lv_obj_t *root_ = nullptr, *level_ = nullptr, *meter_ = nullptr, *status_ = nullptr;
    lv_obj_t *time_ = nullptr;
    lv_obj_t *record_ = nullptr, *play_ = nullptr, *record_label_ = nullptr, *play_label_ = nullptr;
    lv_timer_t *timer_ = nullptr;
};
}
