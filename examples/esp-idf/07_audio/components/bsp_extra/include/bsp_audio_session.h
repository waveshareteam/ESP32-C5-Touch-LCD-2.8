/* APP audio sessions share one ES8311/I2S device. */
#pragma once

#include "bsp/esp-bsp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CODEC_DEFAULT_SAMPLE_RATE 16000 /* Hz */
#define CODEC_VOICE_SAMPLE_RATE 24000   /* Hz */
#define CODEC_DEFAULT_BIT_WIDTH 16
#define CODEC_DEFAULT_ADC_VOLUME 40.0   /* Microphone gain, dB */
#define CODEC_DEFAULT_CHANNEL 2
#define CODEC_VOICE_INPUT_CHANNELS 2    /* Two I2S slots, one physical microphone */
#define CODEC_DEFAULT_VOLUME 100        /* 0..100 */

typedef enum {
    BSP_EXTRA_AUDIO_OWNER_NONE = 0,
    BSP_EXTRA_AUDIO_OWNER_MUSIC,
    BSP_EXTRA_AUDIO_OWNER_RECORDER,
    BSP_EXTRA_AUDIO_OWNER_XIAOZHI,
    BSP_EXTRA_AUDIO_OWNER_MAX,
} bsp_extra_audio_owner_t;

/* Nonblocking session reservation; stop/join workers and close codec before release. */
esp_err_t bsp_extra_audio_session_acquire(bsp_extra_audio_owner_t owner);
esp_err_t bsp_extra_audio_session_release(bsp_extra_audio_owner_t owner);
bsp_extra_audio_owner_t bsp_extra_audio_session_get_owner(void);
const char *bsp_extra_audio_owner_name(bsp_extra_audio_owner_t owner);

/* Mute requires an open session. */
esp_err_t bsp_extra_codec_mute_set(bool enable);
/* Volume: 0..100; cached while closed. volume_set may be NULL. */
esp_err_t bsp_extra_codec_volume_set(int volume, int *volume_set);
int bsp_extra_codec_volume_get(void);
/* Close/reopen codec without deleting shared hardware resources. */
esp_err_t bsp_extra_codec_dev_stop(void);
esp_err_t bsp_extra_codec_dev_resume(void);
/* PCM format: rate in Hz (8000..96000), 16-bit, mono/stereo; requires a session. */
esp_err_t bsp_extra_codec_set_fs(uint32_t rate, uint32_t bits_cfg, i2s_slot_mode_t ch);
/* XIAOZHI session only: 16-bit, channels=2, slot mask=3, microphone mask=1. */
esp_err_t bsp_extra_codec_set_voice_fs(uint32_t rate, uint32_t bits_cfg,
                                     uint8_t record_channels, uint16_t record_tdm_slot_mask,
                                     uint16_t record_mic_gain_mask);
bool bsp_extra_codec_pa_is_enabled(void);
/* PCM sizes/counts are bytes; count pointers may be NULL.
 * Voice: len must be a multiple of 4; timeout_ms applies, transfers may be partial.
 * Other APPs: codec conversion completes a block; timeout_ms is not forwarded. */
esp_err_t bsp_extra_i2s_read(void *audio_buffer, size_t len, size_t *bytes_read, uint32_t timeout_ms);
esp_err_t bsp_extra_i2s_write(void *audio_buffer, size_t len, size_t *bytes_written, uint32_t timeout_ms);
/* Initialize session resources and validate codec; leave codec and amplifier off. */
esp_err_t bsp_extra_codec_init(void);

/* Music/Mic workers: acquire/release in the same task. timeout_ms=0 waits forever. */
bool bsp_extra_audio_acquire(bsp_extra_audio_owner_t owner, uint32_t timeout_ms);
void bsp_extra_audio_release(void);
/* Push trailing PCM through DMA with silence before closing the codec; blocks. */
esp_err_t bsp_extra_audio_drain(void);

#ifdef __cplusplus
}
#endif
