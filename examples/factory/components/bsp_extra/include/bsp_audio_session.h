#pragma once

#include "bsp/esp-bsp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CODEC_DEFAULT_SAMPLE_RATE 16000
#define CODEC_VOICE_SAMPLE_RATE 24000
#define CODEC_DEFAULT_BIT_WIDTH 16
#define CODEC_DEFAULT_ADC_VOLUME 40.0
#define CODEC_DEFAULT_CHANNEL 2
#define CODEC_VOICE_INPUT_CHANNELS 2
#define CODEC_DEFAULT_VOLUME 70

typedef enum {
    BSP_EXTRA_AUDIO_OWNER_NONE = 0,
    BSP_EXTRA_AUDIO_OWNER_MUSIC,
    BSP_EXTRA_AUDIO_OWNER_RECORDER,
    BSP_EXTRA_AUDIO_OWNER_XIAOZHI,
    BSP_EXTRA_AUDIO_OWNER_MAX,
} bsp_extra_audio_owner_t;

/* Acquire once for the entire audio session; stop/join workers before release. */
esp_err_t bsp_extra_audio_session_acquire(bsp_extra_audio_owner_t owner);
esp_err_t bsp_extra_audio_session_release(bsp_extra_audio_owner_t owner);
bsp_extra_audio_owner_t bsp_extra_audio_session_get_owner(void);
const char *bsp_extra_audio_owner_name(bsp_extra_audio_owner_t owner);

esp_err_t bsp_extra_codec_mute_set(bool enable);
esp_err_t bsp_extra_codec_volume_set(int volume, int *volume_set);
int bsp_extra_codec_volume_get(void);
esp_err_t bsp_extra_codec_dev_stop(void);
esp_err_t bsp_extra_codec_dev_resume(void);
esp_err_t bsp_extra_codec_set_fs(uint32_t rate, uint32_t bits_cfg, i2s_slot_mode_t ch);
/* Single ES8311: record_channels=2, slot mask=3, physical microphone mask=1. */
esp_err_t bsp_extra_codec_set_voice_fs(uint32_t rate, uint32_t bits_cfg,
                                     uint8_t record_channels, uint16_t record_tdm_slot_mask,
                                     uint16_t record_mic_gain_mask);
bool bsp_extra_codec_pa_is_enabled(void);
/* Voice transfers honor timeout_ms and may be partial. Ordinary APP transfers
 * use the codec's format conversion and complete the supplied block. */
esp_err_t bsp_extra_i2s_read(void *audio_buffer, size_t len, size_t *bytes_read, uint32_t timeout_ms);
esp_err_t bsp_extra_i2s_write(void *audio_buffer, size_t len, size_t *bytes_written, uint32_t timeout_ms);
esp_err_t bsp_extra_codec_init(void);

/* Board APP extensions: workers own these sessions in their calling task.
 * timeout_ms=0 waits forever, matching bsp_display_lock(). */
bool bsp_extra_audio_acquire(bsp_extra_audio_owner_t owner, uint32_t timeout_ms);
void bsp_extra_audio_release(void);
esp_err_t bsp_extra_audio_drain(void);

#ifdef __cplusplus
}
#endif
