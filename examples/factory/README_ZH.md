# 出厂应用工程

[English](README.md) | [简体中文](README_ZH.md)

## 功能

提供 240 × 320 桌面及 12 个应用，包括 AIChats、SD 音乐、相册、麦克风、显示和按键测试。各应用操作与 Wi-Fi 配网见[使用说明](USER_GUIDE_ZH.md)。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板、USB 数据线，音频测试需要扬声器。音乐和相册另需 FAT32 microSD 卡；Mic Test 使用 PSRAM 录音，无需 SD 卡。

## 怎么运行

1. 安装并启用 **ESP-IDF v5.5.4**，用 VS Code 或 ESP-IDF 终端打开本 `factory` 目录，工程默认选择 ESP32-C5。
2. 使用 USB 数据线连接开发板，测试声音时接好扬声器。音乐与相册需要 FAT32 microSD 卡，分别在卡根目录建立 `/music` 放 MP3、`/photo` 放 JPEG，上电前插卡。
3. 在本目录运行以下命令。`flash` 会烧录包括语音模型在内的全部构建分文件，不要只烧录应用 BIN。

   ```sh
   idf.py build
   idf.py flash monitor
   ```

4. 监视器按工程配置使用 **2,000,000 波特率**。有多个串口时改用 `idf.py -p PORT flash monitor`，将 `PORT` 替换为开发板串口；按 `Ctrl+]` 退出监视器。

只想恢复已实测的桌面、不需要编译时，参照[出厂固件烧录说明](../../firmware/README_ZH.md)。

## 预期现象

屏幕显示背景图、白色应用名称及居中时间。左右滑动翻页，点击图标打开应用，从底部中间向上滑动返回；在 Settings 中配网，各应用的详细操作见使用说明。

## 常见问题

- 音乐或相册找不到文件：检查 SD 卡目录与文件格式，再重新打开应用。Mic Test 使用 Record/Stop/Play，最长录音 10 秒，关闭应用后清除。
- 编译或连接异常：使用 v5.5.4，切换 SDK 后用 `idf.py fullclean` 清理旧构建，并检查串口及监视器波特率。
