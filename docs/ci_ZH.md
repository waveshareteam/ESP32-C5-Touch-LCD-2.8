# CI 与本地检查

[English](ci.md) | 简体中文 | [首页](../README_ZH.md)

[Example checks](../.github/workflows/examples.yml) 在每个 PR 及 main 分支推送时执行轻量文档检查与编译选择。即使跳过示例编译，也会显示最终汇总检查。

- 根目录、示例内、配套库内的 Markdown 修改不会触发示例编译。
- 单个示例的源码或配置修改，只编译该示例。
- 共享 IDF 组件修改触发全部 IDF 工程；Arduino 配套库修改触发全部 Arduino 示例。
- 工作流、CI 脚本、测试或规则配置修改触发全部 22 项编译。
- 出厂固件目录单独分类，其源码或交付文件修改不触发示例编译。
- 变更信息缺失或为空时检查失败。手动运行可填写 `all`、准确示例名或仓库相对路径。
- 同一 PR 或分支的新运行会取消过时运行。

完整矩阵为 8 个工程 × 2 个 IDF 版本，以及 6 个 Arduino 示例。示例产物不会作为出厂固件发布。

## 本地验证

在仓库根目录执行：

```sh
python -m unittest discover -s tests -v
python scripts/ci_plan.py --working-tree
python scripts/ci_plan.py --dispatch all
```

工作区命令使用本仓库归属及路由规则运行两个完整审查脚本，输出 `.ci-output/plan.json`。纯文档变更还会执行 `--expect-docs-only` 和 `--expect-no-example-builds` 断言。手动选择命令审查全部维护文档，并选择指定示例。

CI 的实际入口为 `python scripts/ci_plan.py --event`；集成测试覆盖这一入口及输出矩阵。基础审查脚本另有归属、双语配对、链接及锚点、首页结构、隐私、固件边界和不完整变更的合成测试。

参见[编译说明](../README_ZH.md)和[验证记录](validation_ZH.md)。编译产物、下载的依赖及本地编辑器配置已加入忽略规则。

## 文档归属

英文文档使用相邻的 `_ZH.md` 中文配套文件。旧 `README.en.md` 和 `README.cn.md` 作为一对兼容入口保留。GitHub 默认 PR 模板要求单一路径，因此该模板使用中英双语。

配套库、GUI Guider 资源、Brookesia 及 SHTC3 上游文档保留原有归属和文件名。板级 BSP/驱动维护说明统一放在[兼容性说明](compatibility_ZH.md)。
