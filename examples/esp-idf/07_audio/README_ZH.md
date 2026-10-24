# ES8311 音频

[English](README.md) | [简体中文](README_ZH.md)

## 功能

录制三秒音频到 PSRAM，并通过扬声器播放一次。

## 硬件需求

开发板、USB 数据线和扬声器。

## 怎么运行

启用 ESP-IDF v5.5.4，在本目录依次运行 `idf.py set-target esp32c5`、`idf.py build` 和 `idf.py -p PORT flash monitor`。工程自带所需组件，可以单独复制并编译。

## 预期现象

串口提示 Speak now 后开始说话；录音以 60% 音量回放，完成后释放音频资源。

## 常见问题

无需 SD 卡。单麦克风使用本板 24 kHz 双声道时隙 I2S 配置，无硬件回声参考。
