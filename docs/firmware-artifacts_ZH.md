# 示例固件包

[English](firmware-artifacts.md) | 简体中文 | [首页](../README_ZH.md)

[Example checks 工作流](https://github.com/waveshareteam/ESP32-C5-Touch-LCD-2.8/actions/workflows/examples.yml) 会编译并上传各示例固件包。无需安装 ESP-IDF 或 Arduino 即可烧录。完整运行生成 22 个包：ESP-IDF `v5.5.5` 的 8 个示例、`v6.1` 的同样 8 个示例，以及 Arduino 核心 `3.3.12` 的 6 个示例。

## 下载

1. 登录 GitHub，打开上述工作流。
2. 选择所需源码提交及框架版本对应的成功运行记录。已合并的修改优先选择 `main` 分支的成功记录。
3. 在 **Artifacts** 区域选择开发框架、版本及示例，名称如 `esp-idf-v5.5.5-02_2048` 或 `arduino-3.3.12-06_lvgl_demo`。
4. 解压下载的 Artifact ZIP，里面包含示例 ZIP 和 `.zip.sha256` 文件。再将示例 ZIP 解压到独立文件夹。

Artifacts 保留 30 天。自动运行可能只包含受变更影响的示例。维护者可通过 **Run workflow** 将 `example` 填为 `all`，重新编译全部示例。这些下载位于 Actions Artifacts，此工作流不发布 GitHub Release。

## 烧录

安装 Python 3.10 或更新版本，以及烧录工具：

```sh
python -m pip install "esptool>=5,<6"
```

通过 USB 连接开发板。在解压后的示例文件夹中执行以下命令，将 `PORT` 替换为开发板的实际串口名称：

```sh
python flash.py --port PORT
```

需要时，可按住 BOOT、按下 RESET，然后松开 BOOT 进入下载模式。脚本先检查每个镜像的大小和 SHA-256，再调用 esptool 烧录。若使用其他烧录工具，包内 README 提供了所需地址。

烧录后运行的是所选示例。SD 卡示例仍需准备相应的 [SD 资源](../sdcard/)。天气示例需要在源码中配置 Wi-Fi 并重新编译后使用。编译及打包不能代替实物测试，具体见[验证记录](validation_ZH.md)。恢复出厂演示请使用单独维护的[出厂固件](../Firmware/)。

## 包内文件及地址

| 文件 | 用途 |
| --- | --- |
| `bin/*.bin` | 编译所需的烧录镜像 |
| `manifest.json` | 产品、芯片、示例、框架版本、源码提交、烧录地址、大小及镜像校验值 |
| `flash.py` | 完整性检查及 esptool 烧录脚本 |
| `SHA256SUMS` | 镜像、清单、脚本及说明文档的 SHA-256 校验值 |
| `README.md`、`README_ZH.md` | 中英文烧录说明 |

ESP-IDF 地址来自构建生成的 `flasher_args.json`，同时收集其中的 SPIFFS 等存储镜像。此开发板的 bootloader 地址为 `0x2000`，其余地址请使用所选包内的完整表格。Arduino 包使用核心生成的 `.ino.merged.bin`，烧录地址为 `0x0`，合并镜像中的填充保留了芯片镜像布局；缺失此文件时打包会失败。

打包时读取构建信息，核对芯片、开发板选项及实际框架版本。公开清单仅记录仓库相对示例路径，不包含本地编译目录。PR 包记录 PR 源分支提交；编译、打包、上传均成功，该 CI 任务才算通过。

## 本地打包

先使用[已验证的开发环境](../README_ZH.md)完成编译。打包 ESP-IDF 示例时需激活同一个 ESP-IDF 环境。在仓库根目录执行，将 `SOURCE_COMMIT_SHA` 替换为编译所用的完整源码提交：

```sh
python scripts/package_example.py esp-idf examples/esp-idf/02_2048 --version v5.5.5 --git-sha SOURCE_COMMIT_SHA
python scripts/package_example.py arduino examples/arduino/06_lvgl_demo --version 3.3.12 --git-sha SOURCE_COMMIT_SHA
```

生成文件位于 Git 忽略的 `.ci-output/artifacts/`。可用 `--build-dir` 指定已有编译输出，用 `--output-dir` 指定生成目录。打包只读取示例编译产物，不修改它们。
