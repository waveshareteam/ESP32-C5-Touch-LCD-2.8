/* SPDX-License-Identifier: MIT */
#pragma once
#include <array>
#include <atomic>
#include <functional>
#include <string>
#include <vector>
#include "esp_wn_iface.h"
#include "model_path.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

namespace esp_brookesia::apps {
/** ESP32-C5 single-microphone WakeNet9s. No hardware echo reference is available. */
class XiaozhiAudioProcessor {
public:
    static constexpr uint32_t SAMPLE_RATE = 16000;
    static constexpr size_t INPUT_CHANNELS = 1;
    static constexpr size_t PCM_FRAME_SAMPLES = 960;
    using WakeWordCallback = std::function<void(const std::string &)>;
    using PcmFrameCallback = std::function<void(const int16_t *, size_t)>;
    using VadCallback = std::function<void(bool)>;
    XiaozhiAudioProcessor();
    ~XiaozhiAudioProcessor();
    XiaozhiAudioProcessor(const XiaozhiAudioProcessor &) = delete;
    XiaozhiAudioProcessor &operator=(const XiaozhiAudioProcessor &) = delete;
    bool initialize();
    bool shutdown();
    bool feed(const int16_t *pcm, size_t frames);
    bool enableWakeWord(bool enable);
    bool enableVoiceProcessing(bool enable);
    bool isReady() const { return ready_.load(); }
    size_t getFeedFrames() const { return 160; }
    bool takeWakeWordPcm(std::vector<int16_t> &pcm);
    void setWakeWordCallback(WakeWordCallback callback);
    void setPcmFrameCallback(PcmFrameCallback callback);
    void setVadCallback(VadCallback callback);
private:
    struct InputBlock {
        uint32_t generation;
        size_t count;
        int16_t samples[160];
    };
    static constexpr size_t CACHE_SAMPLES = SAMPLE_RATE * 2;
    static constexpr EventBits_t TASK_EXITED = 1;
    SemaphoreHandle_t lifecycle_mutex_ = nullptr;
    SemaphoreHandle_t state_mutex_ = nullptr;
    SemaphoreHandle_t input_mutex_ = nullptr;
    EventGroupHandle_t events_ = nullptr;
    QueueHandle_t input_queue_ = nullptr;
    TaskHandle_t task_ = nullptr;
    srmodel_list_t *models_ = nullptr;
    const esp_wn_iface_t *wake_iface_ = nullptr;
    model_iface_data_t *wake_data_ = nullptr;
    std::atomic<bool> ready_{false};
    std::atomic<bool> stopping_{false};
    bool wake_enabled_ = false;
    bool voice_enabled_ = false;
    uint32_t generation_ = 0;
    std::vector<int16_t> wake_chunk_;
    size_t chunk_fill_ = 0;
    std::vector<int16_t> cache_;
    size_t cache_fill_ = 0;
    size_t cache_write_ = 0;
    std::array<int16_t, PCM_FRAME_SAMPLES> voice_frame_{};
    size_t voice_fill_ = 0;
    WakeWordCallback wake_callback_;
    PcmFrameCallback pcm_callback_;
    VadCallback vad_callback_;
    void releaseResources();
    void processingLoop();
    static void taskEntry(void *arg);
};
} // namespace esp_brookesia::apps
