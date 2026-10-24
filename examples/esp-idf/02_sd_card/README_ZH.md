# SD 卡

[English](README.md) | [简体中文](README_ZH.md)

## 功能

挂载 /sdcard，写入 test.txt 后读回并比较内容。

## 硬件需求

开发板、USB 数据线和 FAT32 microSD 卡。

## 怎么运行

启用 [product.json](../../../.github/product.json) 中的 ESP-IDF 版本，在本目录依次运行 `idf.py set-target esp32c5`、`idf.py build` 和 `idf.py -p PORT flash monitor`。保留仓库目录结构，以便找到共享组件。

## 预期现象

串口输出写入/读回校验结果 ESP_OK。

## 常见问题

test.txt 会被覆盖，请在上电前插卡；程序不会自动格式化。
