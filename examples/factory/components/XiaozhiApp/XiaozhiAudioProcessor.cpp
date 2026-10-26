/* SPDX-License-Identifier: MIT */
#include "XiaozhiAudioProcessor.hpp"
#include <algorithm>
#include <cstring>
#include <utility>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_wn_models.h"
#include "freertos/idf_additions.h"

namespace esp_brookesia::apps {
namespace {
constexpr char TAG[] = "XiaozhiAudio";
constexpr uint32_t TASK_STACK_BYTES = 12 * 1024;
class Lock {
public:
    explicit Lock(SemaphoreHandle_t mutex): mutex_(mutex) {
        if (mutex_) xSemaphoreTake(mutex_, portMAX_DELAY);
    }
    ~Lock() { if (mutex_) xSemaphoreGive(mutex_); }
private:
    SemaphoreHandle_t mutex_;
};
}

XiaozhiAudioProcessor::XiaozhiAudioProcessor()
{
    lifecycle_mutex_ = xSemaphoreCreateMutex();
    state_mutex_ = xSemaphoreCreateMutex();
    input_mutex_ = xSemaphoreCreateMutex();
}

XiaozhiAudioProcessor::~XiaozhiAudioProcessor()
{
    shutdown();
    if (input_mutex_) vSemaphoreDelete(input_mutex_);
    if (state_mutex_) vSemaphoreDelete(state_mutex_);
    if (lifecycle_mutex_) vSemaphoreDelete(lifecycle_mutex_);
}

bool XiaozhiAudioProcessor::initialize()
{
    if (!lifecycle_mutex_ || !state_mutex_ || !input_mutex_) return false;
    Lock lifecycle(lifecycle_mutex_);
    if (ready_.load()) return true;
    stopping_.store(false);
    models_ = esp_srmodel_init("model");
    char *name = models_ ? esp_srmodel_filter(models_, ESP_WN_PREFIX, "nihaoxiaozhi") : nullptr;
    wake_iface_ = name ? esp_wn_handle_from_name(name) : nullptr;
    wake_data_ = wake_iface_ ? wake_iface_->create(name, DET_MODE_90) : nullptr;
    if (!wake_data_ || wake_iface_->get_samp_rate(wake_data_) != SAMPLE_RATE ||
            wake_iface_->get_channel_num(wake_data_) != 1 ||
            wake_iface_->get_samp_chunksize(wake_data_) <= 0) {
        ESP_LOGE(TAG, "Cannot load the C5 WakeNet9s model from the model partition");
        releaseResources();
        return false;
    }
    wake_chunk_.resize(wake_iface_->get_samp_chunksize(wake_data_));
    cache_.resize(CACHE_SAMPLES);
    input_queue_ = xQueueCreateWithCaps(20, sizeof(InputBlock),
                                      MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    events_ = xEventGroupCreate();
    if (!input_queue_ || !events_) {
        ESP_LOGE(TAG, "Cannot create WakeNet9s audio queue/event group (queue data=%u bytes in PSRAM)",
                 (unsigned)(20 * sizeof(InputBlock)));
        releaseResources();
        return false;
    }
    chunk_fill_ = cache_fill_ = cache_write_ = voice_fill_ = 0;
    wake_enabled_ = voice_enabled_ = false;
    ++generation_;
    if (xTaskCreateWithCaps(taskEntry, "xiaozhi_wn9s", TASK_STACK_BYTES, this, 3, &task_,
                            MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT) != pdPASS) {
        ESP_LOGE(TAG, "Cannot create WakeNet9s task (%lu-byte PSRAM stack)",
                 static_cast<unsigned long>(TASK_STACK_BYTES));
        task_ = nullptr;
        releaseResources();
        return false;
    }
    ready_.store(true);
    ESP_LOGI(TAG, "C5 mono WakeNet9s ready: %s, %u samples/chunk; queue=%u bytes and stack=%lu in PSRAM",
             name, static_cast<unsigned>(wake_chunk_.size()), (unsigned)(20 * sizeof(InputBlock)),
             static_cast<unsigned long>(TASK_STACK_BYTES));
    return true;
}

bool XiaozhiAudioProcessor::shutdown()
{
    if (!lifecycle_mutex_) return true;
    Lock lifecycle(lifecycle_mutex_);
    if (task_ && xTaskGetCurrentTaskHandle() == task_) return false;
    ready_.store(false);
    stopping_.store(true);
    /* Exclude any feed that observed the previous ready state. */
    Lock input(input_mutex_);
    if (task_) {
        xEventGroupWaitBits(events_, TASK_EXITED, pdFALSE, pdTRUE, portMAX_DELAY);
        vTaskDelay(1);
        task_ = nullptr;
    }
    Lock state(state_mutex_);
    releaseResources();
    stopping_.store(false);
    return true;
}

void XiaozhiAudioProcessor::releaseResources()
{
    if (wake_data_) wake_iface_->destroy(wake_data_);
    wake_data_ = nullptr;
    wake_iface_ = nullptr;
    if (models_) esp_srmodel_deinit(models_);
    models_ = nullptr;
    if (input_queue_) vQueueDeleteWithCaps(input_queue_);
    input_queue_ = nullptr;
    if (events_) vEventGroupDelete(events_);
    events_ = nullptr;
    std::vector<int16_t>().swap(wake_chunk_);
    std::vector<int16_t>().swap(cache_);
    wake_enabled_ = voice_enabled_ = false;
}

bool XiaozhiAudioProcessor::feed(const int16_t *pcm, size_t frames)
{
    if (!pcm || !frames || !ready_.load()) return false;
    Lock input(input_mutex_);
    if (!ready_.load() || stopping_.load() || !input_queue_) return false;
    uint32_t generation;
    {
        Lock state(state_mutex_);
        if (!wake_enabled_ && !voice_enabled_) return false;
        generation = generation_;
    }
    while (frames) {
        InputBlock block{};
        block.generation = generation;
        block.count = std::min(frames, size_t{160});
        memcpy(block.samples, pcm, block.count * sizeof(int16_t));
        if (xQueueSend(input_queue_, &block, pdMS_TO_TICKS(20)) != pdTRUE) {
            /* Do not splice PCM across a gap if the CPU cannot keep up. */
            Lock state(state_mutex_);
            ++generation_;
            chunk_fill_ = voice_fill_ = cache_fill_ = cache_write_ = 0;
            xQueueReset(input_queue_);
            ESP_LOGW(TAG, "Input queue full; discarded discontinuous PCM (WakeNet model retained)");
            return false;
        }
        pcm += block.count;
        frames -= block.count;
    }
    return true;
}

bool XiaozhiAudioProcessor::enableWakeWord(bool enable)
{
    Lock state(state_mutex_);
    if (!ready_.load() || stopping_.load()) return false;
    /* ESP-SR 2.4.7 WakeNet9s clean() walks nine queues, but create() allocates
     * only six. Retain the model on start/stop, as upstream EspWakeWord does;
     * discard partial PCM and reject queued samples from the previous mode. */
    if (wake_enabled_ != enable) {
        wake_enabled_ = enable;
        ++generation_;
        chunk_fill_ = 0;
        if (enable) cache_fill_ = cache_write_ = 0;
        ESP_LOGI(TAG, "Wake word detection %s (model retained; PCM generation=%lu)",
                 enable ? "enabled" : "disabled", static_cast<unsigned long>(generation_));
    }
    return true;
}

bool XiaozhiAudioProcessor::enableVoiceProcessing(bool enable)
{
    Lock state(state_mutex_);
    if (!ready_.load() || stopping_.load()) return false;
    if (voice_enabled_ != enable) {
        voice_enabled_ = enable;
        voice_fill_ = 0;
        chunk_fill_ = 0;
        ++generation_;
    }
    return true;
}

bool XiaozhiAudioProcessor::takeWakeWordPcm(std::vector<int16_t> &pcm)
{
    Lock state(state_mutex_);
    pcm.clear();
    if (!ready_.load() || !cache_fill_) return false;
    pcm.resize(cache_fill_);
    const size_t start = (cache_write_ + CACHE_SAMPLES - cache_fill_) % CACHE_SAMPLES;
    for (size_t i = 0; i < cache_fill_; ++i) pcm[i] = cache_[(start + i) % CACHE_SAMPLES];
    cache_fill_ = cache_write_ = 0;
    return true;
}

void XiaozhiAudioProcessor::setWakeWordCallback(WakeWordCallback callback)
{
    Lock state(state_mutex_); wake_callback_ = std::move(callback);
}
void XiaozhiAudioProcessor::setPcmFrameCallback(PcmFrameCallback callback)
{
    Lock state(state_mutex_); pcm_callback_ = std::move(callback);
}
void XiaozhiAudioProcessor::setVadCallback(VadCallback callback)
{
    /* Speech endpoint detection remains on the server, as in the template. */
    Lock state(state_mutex_); vad_callback_ = std::move(callback);
}

void XiaozhiAudioProcessor::processingLoop()
{
    InputBlock block{};
    while (!stopping_.load()) {
        if (xQueueReceive(input_queue_, &block, pdMS_TO_TICKS(100)) != pdTRUE) continue;
        WakeWordCallback wake_cb;
        PcmFrameCallback pcm_cb;
        std::string word;
        std::array<int16_t, PCM_FRAME_SAMPLES> frame{};
        bool have_frame = false;
        {
            Lock state(state_mutex_);
            if (stopping_.load() || block.generation != generation_) continue;
            for (size_t i = 0; i < block.count; ++i) {
                const int16_t sample = block.samples[i];
                if (wake_enabled_) {
                    cache_[cache_write_] = sample;
                    cache_write_ = (cache_write_ + 1) % CACHE_SAMPLES;
                    cache_fill_ = std::min(CACHE_SAMPLES, cache_fill_ + 1);
                    wake_chunk_[chunk_fill_++] = sample;
                    if (chunk_fill_ == wake_chunk_.size()) {
                        chunk_fill_ = 0;
                        int detected = wake_iface_->detect(wake_data_, wake_chunk_.data());
                        if (detected > 0) {
                            const char *name = wake_iface_->get_word_name(wake_data_, detected);
                            word = name ? name : "你好小智";
                            wake_enabled_ = false;
                            wake_cb = wake_callback_;
                        }
                    }
                }
                if (voice_enabled_) {
                    voice_frame_[voice_fill_++] = sample;
                    if (voice_fill_ == PCM_FRAME_SAMPLES) {
                        voice_fill_ = 0;
                        frame = voice_frame_;
                        have_frame = true;
                        pcm_cb = pcm_callback_;
                    }
                }
            }
        }
        /* Callbacks can change modes, so they must run outside state_mutex_. */
        if (wake_cb && !stopping_.load()) wake_cb(word);
        if (have_frame && pcm_cb && !stopping_.load()) pcm_cb(frame.data(), frame.size());
    }
}

void XiaozhiAudioProcessor::taskEntry(void *arg)
{
    auto *self = static_cast<XiaozhiAudioProcessor *>(arg);
    self->processingLoop();
    xEventGroupSetBits(self->events_, TASK_EXITED);
    vTaskDeleteWithCaps(nullptr);
}
} // namespace esp_brookesia::apps
