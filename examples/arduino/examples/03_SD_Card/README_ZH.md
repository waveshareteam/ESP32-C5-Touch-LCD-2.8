# SD 卡识别

[English](README.md) | [简体中文](README_ZH.md)

## 功能

挂载 SD 卡，通过串口打印卡信息和容量。当前 setup 不调用文件读写辅助函数，也不会创建 test.txt。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板和 USB 数据线。上电前插入 FAT32 microSD 测试卡；本 Arduino 示例在挂载失败时可能触发格式化，请先备份卡中数据。

## 怎么运行

1. 安装 Arduino IDE，在**首选项 → 附加开发板管理器网址**中添加 `https://espressif.github.io/arduino-esp32/package_esp32_index.json`。打开开发板管理器，安装 **esp32 by Espressif Systems 3.3.12**，版本与 [product.json](../../../../.github/product.json) 一致。
2. 把**首选项 → 项目文件夹位置**设为本仓库的 `examples/arduino`，重启 IDE，让它找到附带的 [libraries](../../libraries/)；打开本示例目录里的 `.ino` 文件。
3. 在**工具**菜单选择 **ESP32C5 Dev Module**，Flash Size 选 **16 MB**，Flash Mode 选 **QIO**，PSRAM 选 **Enabled**，Partition Scheme 选 **16M Flash (3MB APP/9.9MB FATFS)**。使用 USB 数据线连接开发板并选择对应串口。
4. 点击**上传**，完成后打开**串口监视器**，设为 **115200 波特率**，再按 RESET 观察启动信息。

## 预期现象

启动后串口显示 SD 卡信息及 `practical_size` 容量，屏幕保持黑屏；需要文件校验和屏幕结果时使用 IDF 的 02_sd_card 示例。

## 常见问题

- 没有卡信息时，检查插卡及触点，并在电脑确认卡可读取，再按 RESET 重试。
- 提示缺少头文件或库：确认项目文件夹位置和附带的 libraries 目录，重启 Arduino IDE；不要把附带的 LVGL 8.4 换成其他大版本。
- 上传失败或串口无输出：检查 USB 数据线及工具菜单中的串口，关闭其他串口软件，确认监视器为 115200 波特率后按 RESET。
