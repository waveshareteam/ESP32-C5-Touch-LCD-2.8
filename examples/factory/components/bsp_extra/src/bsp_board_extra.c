/* SPDX-License-Identifier: Apache-2.0
 * Legacy MusicPlayer API on top of the shared BSP audio session.
 * No second ES8311 instance and no GPIO reset on player shutdown. */
#include "bsp_board_extra.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include <string.h>

static esp_asp_handle_t player;
static SemaphoreHandle_t player_mutex;
static const char *TAG = "bsp_music";

static void log_memory(const char *stage)
{
    ESP_LOGI(TAG, "%s: DMA free=%u, largest=%u; PSRAM free=%u", stage,
             (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA),
             (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA),
             (unsigned)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
}

static int player_event(esp_asp_event_pkt_t *event, void *context)
{
    (void)context;
    // This library updates its public state only with an event callback set.
    // Never acquire player_mutex here: stop() waits for the pipeline callback.
    if (event->type == ESP_ASP_EVENT_TYPE_STATE && event->payload &&
        event->payload_size == sizeof(esp_asp_state_t)) {
        esp_asp_state_t state;
        memcpy(&state, event->payload, sizeof(state));
        ESP_LOGI(TAG, "Player state: %s", esp_audio_simple_player_state_to_str(state));
    }
    return 0;
}

// Called with player_mutex held. A new player has no pipeline until run().
static esp_gmf_err_t stop_player(void)
{
    if (!player) return ESP_GMF_ERR_OK;
    esp_gmf_pipeline_handle_t pipeline = NULL;
    esp_gmf_err_t ret = esp_audio_simple_player_get_pipeline(player, &pipeline);
    if (ret != ESP_GMF_ERR_OK || !pipeline) return ret;
    return esp_audio_simple_player_stop(player);
}

static int output(uint8_t *data, int size, void *context)
{
    (void)context;
    size_t written = 0;
    esp_err_t ret = bsp_extra_i2s_write(data, size, &written, 1000);
    if (ret != ESP_OK || written != size) {
        ESP_LOGE(TAG, "PCM output failed: %s, written=%u/%d", esp_err_to_name(ret), (unsigned)written, size);
        return -1;
    }
    return 0;
}

static esp_err_t claim_codec(void)
{
    if (bsp_extra_audio_session_get_owner() == BSP_EXTRA_AUDIO_OWNER_MUSIC) return ESP_OK;
    esp_err_t ret = bsp_extra_audio_session_acquire(BSP_EXTRA_AUDIO_OWNER_MUSIC);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Audio busy: owner=%s", bsp_extra_audio_owner_name(bsp_extra_audio_session_get_owner()));
        return ret;
    }
    ret = bsp_extra_codec_set_fs(24000, 16, I2S_SLOT_MODE_STEREO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Music codec configuration failed: %s", esp_err_to_name(ret));
        bsp_extra_codec_dev_stop();
        bsp_extra_audio_session_release(BSP_EXTRA_AUDIO_OWNER_MUSIC);
    }
    return ret;
}

static void release_codec(void)
{
    if (bsp_extra_audio_session_get_owner() == BSP_EXTRA_AUDIO_OWNER_MUSIC &&
        bsp_extra_codec_dev_stop() == ESP_OK)
        bsp_extra_audio_session_release(BSP_EXTRA_AUDIO_OWNER_MUSIC);
}

void Audio_Play_Init(void)
{
    // Called by the Music controller, before it accepts playback commands.
    if (!player_mutex) player_mutex = xSemaphoreCreateMutex();
    if (!player_mutex) return;
    xSemaphoreTake(player_mutex, portMAX_DELAY);
    if (!player) {
        log_memory("Before player creation");
        esp_asp_cfg_t cfg = { .out = { .cb = output }, .task_prio = 5 };
        esp_gmf_err_t ret = esp_audio_simple_player_new(&cfg, &player);
        if (ret != ESP_GMF_ERR_OK) ESP_LOGE(TAG, "Player creation failed: %d", ret);
        if (ret == ESP_GMF_ERR_OK) esp_audio_simple_player_set_event(player, player_event, NULL);
        log_memory("After player creation");
    }
    xSemaphoreGive(player_mutex);
}

void Volume_Adjustment(uint8_t volume) { bsp_extra_codec_volume_set(volume, NULL); }
uint8_t get_audio_volume(void) { return bsp_extra_codec_volume_get(); }

esp_gmf_err_t Audio_Play_Music(const char *url)
{
    if (!player_mutex || !url) return ESP_GMF_ERR_INVALID_ARG;
    xSemaphoreTake(player_mutex, portMAX_DELAY);
    esp_gmf_err_t ret = ESP_GMF_ERR_FAIL;
    if (player) {
        ret = stop_player();
        if (ret == ESP_GMF_ERR_OK) {
            if (claim_codec() == ESP_OK) {
                ret = esp_audio_simple_player_run(player, url, NULL);
                if (ret != ESP_GMF_ERR_OK && stop_player() == ESP_GMF_ERR_OK) release_codec();
            } else {
                ret = ESP_GMF_ERR_FAIL;
            }
        }
    }
    if (ret != ESP_GMF_ERR_OK) ESP_LOGE(TAG, "Music start failed: %d", ret);
    log_memory("After music start");
    xSemaphoreGive(player_mutex);
    return ret;
}

esp_gmf_err_t Audio_Stop_Play(void)
{
    if (!player_mutex) return ESP_GMF_ERR_OK;
    xSemaphoreTake(player_mutex, portMAX_DELAY);
    esp_gmf_err_t ret = stop_player();
    // GMF has joined its pipeline before the codec can be reused by another APP.
    if (ret == ESP_GMF_ERR_OK) release_codec();
    xSemaphoreGive(player_mutex);
    return ret;
}

esp_gmf_err_t Audio_Resume_Play(void)
{
    if (!player_mutex) return ESP_GMF_ERR_FAIL;
    xSemaphoreTake(player_mutex, portMAX_DELAY);
    esp_gmf_err_t ret = player && claim_codec() == ESP_OK ?
        esp_audio_simple_player_resume(player) : ESP_GMF_ERR_FAIL;
    xSemaphoreGive(player_mutex);
    return ret;
}

esp_gmf_err_t Audio_Pause_Play(void)
{
    if (!player_mutex) return ESP_GMF_ERR_OK;
    xSemaphoreTake(player_mutex, portMAX_DELAY);
    esp_gmf_err_t ret = player ? esp_audio_simple_player_pause(player) : ESP_GMF_ERR_OK;
    xSemaphoreGive(player_mutex);
    return ret;
}

esp_asp_state_t Audio_Get_Current_State(void)
{
    if (!player_mutex) return ESP_ASP_STATE_ERROR;
    xSemaphoreTake(player_mutex, portMAX_DELAY);
    esp_asp_state_t state = ESP_ASP_STATE_ERROR;
    if (player) esp_audio_simple_player_get_state(player, &state);
    xSemaphoreGive(player_mutex);
    return state;
}

void Audio_Play_Deinit(void)
{
    if (!player_mutex) return;
    xSemaphoreTake(player_mutex, portMAX_DELAY);
    // Join the pipeline before releasing the codec and its decoder task/pool.
    esp_gmf_err_t ret = stop_player();
    if (ret == ESP_GMF_ERR_OK) {
        release_codec();
        if (player) {
            esp_audio_simple_player_destroy(player);
            player = NULL;
            log_memory("Player released");
        }
    } else {
        ESP_LOGE(TAG, "Player stop failed; resources retained: %d", ret);
    }
    xSemaphoreGive(player_mutex);
}
