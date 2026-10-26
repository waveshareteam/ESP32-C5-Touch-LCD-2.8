# SD 卡读写与屏幕显示

[English](README.md) | [简体中文](README_ZH.md)

## 功能

检查 microSD 卡能否正常保存和读取文本文件，在屏幕上直接查看结果和文件内容。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板、USB 数据线和 FAT32 microSD 卡。上电前插卡，每次运行会覆盖卡根目录的 `test.txt`，本示例不会格式化 SD 卡。

## 怎么运行

1. 先断电插好 FAT32 测试卡，再连接 USB。首次使用按 [ESP-IDF 入门说明](../../../docs/getting-started-idf_ZH.md)准备环境，再在 **`02_sd_card`** 目录打开 ESP-IDF 终端。
2. 执行以下命令，第二条会烧录示例并以 **115200 波特率**打开串口监视器。

   ```sh
   idf.py build
   idf.py flash monitor
   ```

3. 程序会自动测试，在屏幕上查看结果，需要重测时按 RESET。

## 预期现象

屏幕显示 **Write / read passed**（读写通过）及 `/sdcard/test.txt`，串口显示 `Write successful`、`Read successful` 和 `Write/read verification: ESP_OK`。

文件内容如下；结果显示后，可以取卡在电脑中打开 `test.txt` 检查，每次重测会覆盖这个文件。

```text
ESP32-C5-Touch-LCD-2.8 SD test
```

## 常见问题

- **SD card not mounted（未挂载）：**插好 FAT32 卡后按 RESET；仅插卡不会自动重测，本示例不会格式化卡片。
- **File write/read failed（读写失败）：**换一张可正常写入的卡，并根据屏幕提示查看失败阶段。
- **串口有 `cmd=5 ... command not supported`：**后续校验为 `ESP_OK` 就表示文件读写通过，不能仅凭这一行判断卡故障。
