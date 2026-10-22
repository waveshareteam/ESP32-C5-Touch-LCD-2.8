# AIChats 组件

[English](README.md) | [简体中文](README_ZH.md)

## 功能

将小智激活、WakeNet9s 唤醒词和语音对话接入 Brookesia 应用生命周期，桌面仍显示 AIChats。打开后直接在出厂固件运行，不再重启或切换到单独的小智固件。

## 硬件需求

使用 C5-2.8 的 ES8311 与单麦克风，没有硬件回声参考通道；本板不注册摄像头工具。界面为 240×320。

## 怎么运行

完整编译、烧录出厂工程，包含语音模型分区。在 Settings 配置网络，打开 AIChats 完成激活，说“你好小智”或点击 Talk 开始对话，点击 Stop 取消。

## 预期现象

识别文本和回复通过字幕、音频输出。退出或切换 APP 时停止音频任务并释放共用的 codec，供音乐和 Mic Test 使用。

## 常见问题

从旧双固件方案升级时，应完整烧录工程，包含新的分区表、storage 和 model。音频缓冲使用 PSRAM，控制和 MCP 工作任务栈保留在内部 RAM，兼容 Flash/NVS 访问。上游 OGG 适配代码的来源及 MIT 许可证保留在 [third_party/xiaozhi_esp32](third_party/xiaozhi_esp32/)。
