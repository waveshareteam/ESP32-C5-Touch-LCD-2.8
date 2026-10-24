#include <stdlib.h>
#include "bsp/esp-bsp.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define RATE 24000
#define CHANNELS 2
#define SECONDS 3
#define BLOCK_BYTES 1920

void app_main(void)
{
    const size_t bytes = RATE * CHANNELS * sizeof(int16_t) * SECONDS;
    int16_t *pcm = heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!pcm) { ESP_LOGE("audio", "Recording buffer unavailable"); return; }
    esp_codec_dev_handle_t codec = bsp_audio_codec_speaker_init();
    if (!codec) { free(pcm); ESP_LOGE("audio", "ES8311 initialization failed"); return; }
    esp_codec_dev_sample_info_t fs = {
        .sample_rate = RATE, .channel = CHANNELS, .bits_per_sample = 16
    };
    int ret = esp_codec_dev_open(codec, &fs);
    if (ret != ESP_CODEC_DEV_OK) goto done;
    ret = bsp_board_pa_set(false);
    if (ret == ESP_OK) ret = esp_codec_dev_set_out_mute(codec, true);
    if (ret == ESP_OK) ret = esp_codec_dev_set_in_gain(codec, 40.0f);
    if (ret != ESP_OK) goto done;
    // Discard startup samples before retaining the three-second recording.
    for (unsigned i = 0; i < 5 && ret == ESP_OK; ++i)
        ret = esp_codec_dev_read(codec, pcm, BLOCK_BYTES);
    ESP_LOGI("audio", "Speak now: recording %d seconds", SECONDS);
    for (size_t offset = 0; offset < bytes && ret == ESP_OK; offset += BLOCK_BYTES)
        ret = esp_codec_dev_read(codec, (uint8_t *)pcm + offset, BLOCK_BYTES);
    if (ret == ESP_OK) ret = esp_codec_dev_set_out_vol(codec, 60);
    if (ret == ESP_OK) ret = esp_codec_dev_set_out_mute(codec, false);
    if (ret == ESP_OK) ret = bsp_board_pa_set(true);
    ESP_LOGI("audio", "Playing the recording");
    for (size_t offset = 0; offset < bytes && ret == ESP_OK; offset += BLOCK_BYTES)
        ret = esp_codec_dev_write(codec, (uint8_t *)pcm + offset, BLOCK_BYTES);
    vTaskDelay(pdMS_TO_TICKS(200));
done:
    bsp_board_pa_set(false);
    esp_codec_dev_close(codec);
    bsp_audio_deinit();
    free(pcm);
    ESP_LOGI("audio", "Audio test finished: %s", ret == ESP_OK ? "PASS" : "FAIL");
}
