#include "bsp_audio_session.h"

#include <atomic>
#include <climits>
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

static const char *TAG = "bsp_extra_board";
static esp_codec_dev_handle_t codec;
static SemaphoreHandle_t session_mutex, control_mutex;
static std::atomic<bsp_extra_audio_owner_t> audio_owner{BSP_EXTRA_AUDIO_OWNER_NONE};
static bool opened;
static std::atomic<bool> pa_enabled{false};
static uint8_t opened_channels;
static uint32_t opened_rate;
static int volume = 70;

static esp_err_t codec_result(int ret) { return ret == ESP_CODEC_DEV_OK ? ESP_OK : ESP_FAIL; }

const char *bsp_extra_audio_owner_name(bsp_extra_audio_owner_t owner)
{
    switch (owner) {
    case BSP_EXTRA_AUDIO_OWNER_NONE: return "None";
    case BSP_EXTRA_AUDIO_OWNER_MUSIC: return "Music";
    case BSP_EXTRA_AUDIO_OWNER_RECORDER: return "Mic Test";
    case BSP_EXTRA_AUDIO_OWNER_XIAOZHI: return "Xiaozhi";
    default: return "Invalid";
    }
}
bsp_extra_audio_owner_t bsp_extra_audio_session_get_owner(void) { return audio_owner.load(); }

esp_err_t bsp_extra_audio_session_acquire(bsp_extra_audio_owner_t owner)
{
    if (owner <= BSP_EXTRA_AUDIO_OWNER_NONE || owner >= BSP_EXTRA_AUDIO_OWNER_MAX) return ESP_ERR_INVALID_ARG;
    if (!codec) return ESP_ERR_INVALID_STATE;
    bsp_extra_audio_owner_t expected = BSP_EXTRA_AUDIO_OWNER_NONE;
    return audio_owner.compare_exchange_strong(expected, owner) ? ESP_OK : ESP_ERR_INVALID_STATE;
}
esp_err_t bsp_extra_audio_session_release(bsp_extra_audio_owner_t owner)
{
    if (owner <= BSP_EXTRA_AUDIO_OWNER_NONE || owner >= BSP_EXTRA_AUDIO_OWNER_MAX) return ESP_ERR_INVALID_ARG;
    if (opened) return ESP_ERR_INVALID_STATE;
    return audio_owner.compare_exchange_strong(owner, BSP_EXTRA_AUDIO_OWNER_NONE) ? ESP_OK : ESP_ERR_INVALID_STATE;
}

static bool owns_session(void)
{
    return session_mutex && xSemaphoreGetMutexHolder(session_mutex) == xTaskGetCurrentTaskHandle();
}
static bool can_access(void)
{
    // GMF writes from its output task while the Music controller owns a session.
    return audio_owner.load() == BSP_EXTRA_AUDIO_OWNER_XIAOZHI ||
           audio_owner.load() == BSP_EXTRA_AUDIO_OWNER_MUSIC || owns_session();
}

esp_err_t bsp_extra_i2s_read(void *audio_buffer, size_t len, size_t *bytes_read, uint32_t timeout_ms)
{
    size_t count = 0;
    if (!bytes_read) bytes_read = &count;
    *bytes_read = 0;
    if (!audio_buffer || !len || len > INT_MAX) return ESP_ERR_INVALID_ARG;
    if (!codec || !opened || !can_access()) return ESP_ERR_INVALID_STATE;
    if (audio_owner.load() == BSP_EXTRA_AUDIO_OWNER_XIAOZHI) {
        if (len % 4) return ESP_ERR_INVALID_ARG;
        return bsp_audio_i2s_read(audio_buffer, len, bytes_read, timeout_ms);
    }
    esp_err_t ret = codec_result(esp_codec_dev_read(codec, audio_buffer, len));
    if (ret == ESP_OK) *bytes_read = len;
    return ret;
}
esp_err_t bsp_extra_i2s_write(void *audio_buffer, size_t len, size_t *bytes_written, uint32_t timeout_ms)
{
    size_t count = 0;
    if (!bytes_written) bytes_written = &count;
    *bytes_written = 0;
    if (!audio_buffer || !len || len > INT_MAX) return ESP_ERR_INVALID_ARG;
    if (!codec || !opened || !can_access()) return ESP_ERR_INVALID_STATE;
    if (audio_owner.load() == BSP_EXTRA_AUDIO_OWNER_XIAOZHI) {
        if (len % 4) return ESP_ERR_INVALID_ARG;
        return bsp_audio_i2s_write(audio_buffer, len, bytes_written, timeout_ms);
    }
    esp_err_t ret = codec_result(esp_codec_dev_write(codec, audio_buffer, len));
    if (ret == ESP_OK) *bytes_written = len;
    return ret;
}

static esp_err_t open_codec(uint32_t rate, uint8_t channels)
{
    if (!codec || !can_access()) return ESP_ERR_INVALID_STATE;
    if (rate < 8000 || rate > 96000 || (channels != 1 && channels != 2)) return ESP_ERR_INVALID_ARG;
    xSemaphoreTake(control_mutex, portMAX_DELAY);
    esp_err_t ret = ESP_OK;
    if (opened) {
        bsp_board_pa_set(false);
        pa_enabled.store(false);
        ret = codec_result(esp_codec_dev_close(codec));
        if (ret != ESP_OK) { xSemaphoreGive(control_mutex); return ret; }
        opened = false;
    }
    esp_codec_dev_sample_info_t fs = {};
    fs.sample_rate = rate;
    fs.channel = channels;
    fs.bits_per_sample = 16;
    ret = codec_result(esp_codec_dev_open(codec, &fs));
    if (ret == ESP_OK) ret = codec_result(esp_codec_dev_set_out_vol(codec, volume));
    if (ret == ESP_OK) ret = codec_result(esp_codec_dev_set_in_gain(codec, 40.0f));
    if (ret == ESP_OK) ret = codec_result(esp_codec_dev_set_out_mute(codec, false));
    if (ret == ESP_OK) ret = bsp_board_pa_set(true);
    pa_enabled.store(ret == ESP_OK);
    if (ret != ESP_OK) { bsp_board_pa_set(false); esp_codec_dev_close(codec); }
    opened = ret == ESP_OK;
    opened_channels = opened ? channels : 0;
    if (opened) opened_rate = rate;
    xSemaphoreGive(control_mutex);
    return ret;
}

esp_err_t bsp_extra_codec_set_fs(uint32_t rate, uint32_t bits_cfg, i2s_slot_mode_t ch)
{
    if (bits_cfg != 16) return ESP_ERR_INVALID_ARG;
    return open_codec(rate, static_cast<uint8_t>(ch));
}
esp_err_t bsp_extra_codec_set_voice_fs(uint32_t rate, uint32_t bits_cfg,
                                     uint8_t record_channels, uint16_t record_tdm_slot_mask,
                                     uint16_t record_mic_gain_mask)
{
    // ES8311 has one physical microphone in stereo STD slots, not ES7210 TDM.
    if (audio_owner.load() != BSP_EXTRA_AUDIO_OWNER_XIAOZHI) return ESP_ERR_INVALID_STATE;
    if (bits_cfg != 16 || record_channels != 2 || record_tdm_slot_mask != 3 || record_mic_gain_mask != 1)
        return ESP_ERR_INVALID_ARG;
    esp_err_t ret = open_codec(rate, 2);
    if (ret == ESP_OK) ESP_LOGI(TAG, "ES8311 voice: %lu Hz, single microphone, no hardware echo reference", (unsigned long)rate);
    return ret;
}
bool bsp_extra_codec_pa_is_enabled(void)
{
    return pa_enabled.load();
}
esp_err_t bsp_extra_codec_volume_set(int value, int *volume_set)
{
    if (value < 0 || value > 100) return ESP_ERR_INVALID_ARG;
    if (!control_mutex || !codec) return ESP_ERR_INVALID_STATE;
    xSemaphoreTake(control_mutex, portMAX_DELAY);
    esp_err_t ret = opened ? codec_result(esp_codec_dev_set_out_vol(codec, value)) : ESP_OK;
    if (ret == ESP_OK) volume = value;
    if (volume_set) *volume_set = volume;
    xSemaphoreGive(control_mutex);
    return ret;
}
int bsp_extra_codec_volume_get(void)
{
    if (!control_mutex) return volume;
    xSemaphoreTake(control_mutex, portMAX_DELAY);
    int result = volume;
    xSemaphoreGive(control_mutex);
    return result;
}
esp_err_t bsp_extra_codec_mute_set(bool enable)
{
    if (!codec || !opened || !can_access()) return ESP_ERR_INVALID_STATE;
    xSemaphoreTake(control_mutex, portMAX_DELAY);
    esp_err_t ret = codec_result(esp_codec_dev_set_out_mute(codec, enable));
    xSemaphoreGive(control_mutex);
    return ret;
}
esp_err_t bsp_extra_codec_dev_stop(void)
{
    if (!codec || !can_access()) return ESP_ERR_INVALID_STATE;
    xSemaphoreTake(control_mutex, portMAX_DELAY);
    esp_err_t ret = ESP_OK;
    if (opened) {
        esp_codec_dev_set_out_mute(codec, true);
        ret = bsp_board_pa_set(false);
        if (ret == ESP_OK) pa_enabled.store(false);
        esp_err_t closed = codec_result(esp_codec_dev_close(codec));
        if (closed == ESP_OK) opened = false;
        if (ret == ESP_OK) ret = closed;
    }
    xSemaphoreGive(control_mutex);
    return ret;
}
esp_err_t bsp_extra_codec_dev_resume(void)
{
    return open_codec(opened_rate ? opened_rate : 48000, opened_channels ? opened_channels : 1);
}
esp_err_t bsp_extra_codec_init(void)
{
    if (codec) return ESP_OK;
    if (!session_mutex) session_mutex = xSemaphoreCreateMutex();
    if (!control_mutex) control_mutex = xSemaphoreCreateMutex();
    if (!session_mutex || !control_mutex) return ESP_ERR_NO_MEM;
    codec = bsp_audio_codec_speaker_init();
    if (!codec) return ESP_FAIL;
    ESP_RETURN_ON_ERROR(bsp_board_pa_set(false), TAG, "PA off");
    // Validate this board's 24 kHz ES8311 profile without playing.
    esp_codec_dev_sample_info_t fs = {};
    fs.sample_rate = 24000;
    fs.channel = 2;
    fs.bits_per_sample = 16;
    esp_err_t ret = codec_result(esp_codec_dev_open(codec, &fs));
    if (ret == ESP_OK) ret = codec_result(esp_codec_dev_set_out_mute(codec, true));
    if (ret == ESP_OK) ret = codec_result(esp_codec_dev_set_out_vol(codec, volume));
    if (ret == ESP_OK) ret = codec_result(esp_codec_dev_set_in_gain(codec, 40.0f));
    esp_err_t closed = codec_result(esp_codec_dev_close(codec));
    if (ret == ESP_OK) ret = closed;
    if (ret != ESP_OK) { codec = nullptr; bsp_audio_deinit(); }
    return ret;
}

/* APP extensions: task-owned Music/Mic sessions and complete DMA-ring drain. */
bool bsp_extra_audio_acquire(bsp_extra_audio_owner_t owner, uint32_t timeout_ms)
{
    if (!codec || !session_mutex ||
        (owner != BSP_EXTRA_AUDIO_OWNER_MUSIC && owner != BSP_EXTRA_AUDIO_OWNER_RECORDER)) return false;
    const TickType_t started = xTaskGetTickCount();
    do {
        if (bsp_extra_audio_session_acquire(owner) == ESP_OK) {
            if (xSemaphoreTake(session_mutex, 0) == pdTRUE) return true;
            bsp_extra_audio_session_release(owner);
        }
        if (timeout_ms && xTaskGetTickCount() - started >= pdMS_TO_TICKS(timeout_ms)) return false;
        vTaskDelay(pdMS_TO_TICKS(10));
    } while (true);
}
void bsp_extra_audio_release(void)
{
    if (!owns_session()) return;
    if (bsp_extra_codec_dev_stop() != ESP_OK) return;
    auto owner = audio_owner.load();
    xSemaphoreGive(session_mutex);
    bsp_extra_audio_session_release(owner);
}
esp_err_t bsp_extra_audio_drain(void)
{
    if (!owns_session() || !opened) return ESP_ERR_INVALID_STATE;
    int16_t silence[256] = {};
    size_t remaining = BSP_AUDIO_DMA_DESCRIPTORS * BSP_AUDIO_DMA_FRAMES * opened_channels * sizeof(int16_t) + sizeof(silence);
    while (remaining) {
        size_t count = remaining < sizeof(silence) ? remaining : sizeof(silence);
        ESP_RETURN_ON_ERROR(bsp_extra_i2s_write(silence, count, nullptr, 1000), TAG, "Drain audio");
        remaining -= count;
    }
    return ESP_OK;
}
