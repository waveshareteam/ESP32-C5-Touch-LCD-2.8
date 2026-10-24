# 出厂应用工程

[English](README.md) | [简体中文](README_ZH.md)

## 功能

提供 240 × 320 竖屏桌面，包含集成式 AIChats、SD 音乐、相册、屏幕测试和按键测试等 12 个应用。应用操作及联网方式见[使用说明](USER_GUIDE_ZH.md)。

## 硬件需求

使用 ESP32-C5-Touch-LCD-2.8、USB 数据线和扬声器。音乐与相册需要 FAT32 microSD 卡，分别在 `/music` 放置 MP3、在 `/photo` 放置 JPEG；Mic Test 使用 PSRAM。

## 怎么运行

启用 ESP-IDF v5.5.4，在本目录执行 `idf.py set-target esp32c5`、`idf.py build` 和 `idf.py -p PORT flash monitor`。工程使用 LVGL 9.5.0，并引用 ../esp-idf/components 中的共享 BSP（请保留仓库目录结构），烧录时包含全部构建分文件及语音模型。

## 预期现象

桌面显示原背景、白色应用名称和居中时间，完整首帧刷新后才开启背光。Display Test 循环五种颜色，Button Test 检查 BOOT，其余应用共用显示、SD 卡和音频资源。

## 常见问题

音乐读取 SD 卡而非 SPIFFS，请使用 `/music` 目录，插卡后重新打开应用。另行提供的[固件镜像](../../firmware/README_ZH.md)保留对应版本的功能，源码更新不会替换镜像；串口监视器使用 2,000,000 波特率。
