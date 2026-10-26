# 出厂桌面演示

[English](README.md) | [简体中文](README_ZH.md)

## 功能

通过触摸屏桌面体验 12 个应用，包括语音对话、天气、SD 音乐、相册和硬件测试；各应用操作见[使用说明](USER_GUIDE_ZH.md)。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板和 USB 数据线；声音功能需要扬声器，音乐和相册需要 FAT32 microSD 卡，天气和 AIChats 需要可上网的 Wi-Fi。

## 怎么运行

1. 想直接体验桌面，先按[出厂固件说明](../../firmware/README_ZH.md)烧录最新镜像，再按 RESET；想从源码烧录，按 [ESP-IDF 入门说明](../../docs/getting-started-idf_ZH.md)准备环境，在本目录依次执行 `idf.py build` 和 `idf.py flash monitor`，出厂工程监视器为 **2,000,000 波特率**。
2. 左右滑动桌面翻页，点击图标进入应用；从底部中间向上滑动返回桌面，Display Test 则在屏幕任意位置双击退出。
3. 使用在线功能前，打开 **Settings → Wi-Fi setup**，按[配网步骤](USER_GUIDE_ZH.md#连接-wi-fi)联网；使用音乐和相册时，在 SD 卡根目录建立 `/music` 放 MP3、`/photo` 放 JPG/JPEG，上电前插卡。

## 预期现象

桌面显示应用图标和时间，联网后时间会更新；打开 Mic Test 可以录音并回放，打开 Display Test 可以观察红、绿、蓝、黄、白全屏循环。

## 常见问题

- **没有音乐或照片：**检查 FAT32 格式、目录名和文件格式，插卡后重新打开应用。
- **AIChats 或天气不可用：**确认 Wi-Fi 可以上网，再按[应用使用说明](USER_GUIDE_ZH.md)检查。
- **如何退出应用？**从底部中间向上滑动；Display Test 双击屏幕任意位置退出，离开音乐播放器后音乐会停止。
