# SD 音乐

[English](README.md) | [简体中文](README_ZH.md)

## 功能

扫描 SD 卡 MP3 文件并通过 ES8311 播放。

## 硬件需求

ESP32-C5-Touch-LCD-2.8。开发板、已连接的扬声器和 FAT32 microSD 卡。

## 怎么运行

在此目录启用对应 ESP-IDF 环境后执行：

```sh
idf.py set-target esp32c5 build flash monitor
```

## 预期现象

串口列出曲目，扬声器播放选中的文件。

## 常见问题

MP3 文件需放在卡根目录的 music 文件夹，可使用 [示例音乐](../resources/sdcard/music/)。
