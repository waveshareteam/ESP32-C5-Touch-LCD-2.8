# 验证记录

[English](validation.md) | 简体中文 | [首页](../README_ZH.md)

本地验证于 2026-10-13 完成，22 项示例编译全部通过，使用的框架版本如下：

| 框架 | 覆盖范围 | 本地结果 |
| --- | --- | --- |
| ESP-IDF v5.5.5 | 全部 8 个 ESP-IDF 示例 | 8/8 通过 |
| ESP-IDF v6.1（版本输出为 v6.1.0） | 全部 8 个 ESP-IDF 示例 | 8/8 通过 |
| Arduino-ESP32 3.3.12 | 全部 6 个产品 Arduino 示例 | 6/6 通过 |

完整本地矩阵在源码提交 `73751613844a7d6761220743318006aba4ec6d1d` 验证。随后仅修正了 QMI8658 头文件引用大小写，该示例已在提交 `6fd37989f97f5014759b159573f4a06a65e779fb` 使用 Arduino-ESP32 3.3.12 重新编译通过；其他示例和辅助脚本未变。

ESP-IDF 示例包括 `01_ch32_test`、`02_2048`、`03_sd_lvgl`、`04_sd_music`、`05_spiff_music`、`06_wifi_weather`、`07_Ball_qmi` 和 `08_bookesia`。Arduino 示例包括 `01_exio`、`02_I2C_qmi8658`、`03_SD_Card`、`04_I2C_pcf85063`、`05_shtc3` 和 `06_lvgl_demo`，使用仓库附带的库以及 [CI 说明](ci_ZH.md) 中的开发板选项。

首次迁移的 86 项 Python 辅助脚本测试全部通过，包含工作流实际调用的变更路由命令的集成测试。当时变更范围内的 Markdown 审核结果为 0 个错误、151 个警告：所有警告均来自随示例迁移的上游 Markdown 文件，已通过 Git 确认内容完全相同，重命名相似度为 100%。此次迁移的路由检查选中了完整的 16 项 ESP-IDF 编译和 6 项 Arduino 编译。

[GitHub Actions 运行 38358002478](https://github.com/waveshareteam/ESP32-C5-Touch-LCD-2.8/actions/runs/38358002478) 已在源码提交 `6fd37989f97f5014759b159573f4a06a65e779fb` 通过全部 24 项检查：16 项 ESP-IDF 编译、6 项 Arduino 编译、文档与编译选择检查、汇总检查。[PR #4](https://github.com/waveshareteam/ESP32-C5-Touch-LCD-2.8/pull/4) 于 2026-10-13 将该源码合并至 `main`，合并提交为 `1efb7b11e9605b77d1bea2a4cba8fc22e5ffb4e0`。首次 Linux 编译发现 QMI8658 示例的本地头文件引用大小写不一致，现已修正为实际文件名 `i2c_bsp.h`。

以上结果覆盖编译及仓库检查。尚未烧录硬件；显示颜色、触摸、SD、传感器和音频仍需实物测试，尤其需要确认按原理图修正音频数据方向后的播放效果。保留依赖及框架适配的具体原因见[兼容性说明](compatibility_ZH.md)。

## 示例固件包验证

新增打包功能后，102 项 Python 测试全部通过，包含包内文件、烧录地址、校验值、芯片与版本核对、损坏镜像拒绝烧录，以及工作流路由入口。变更范围内的文档审核为 0 个错误、0 个警告，路由选中全部 22 项编译。

已成功打包并检查全部 22 份保留的本地示例编译产物，共 58 个镜像，核对校验值、ZIP 内容及生成的烧录命令，未连接硬件烧录。这些产物来自上文记录的既有编译，并非本次重新编译产品源码；本次新增打包功能没有修改示例源码。本地验证包记录原编译提交，CI 则重新编译并在可下载包内记录当前源码提交。参见[打包说明](firmware-artifacts_ZH.md)及[当前工作流记录](https://github.com/waveshareteam/ESP32-C5-Touch-LCD-2.8/actions/workflows/examples.yml)。

出厂 BIN 保持不变，SHA-256 为 `92d0f25d7b8c43900c8882e2ca9cee1ca7c8a9338504bd392567f3d1e8cdf174`。此项仅检查文件完整性，不代表重新编译或完成固件实物验证。

检查时 GitHub 私密漏洞报告未开启，需要维护者确定私密报告方式后再添加相关安全报告文档。
