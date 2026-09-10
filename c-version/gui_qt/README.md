# 智能股票分析与预测系统 — Qt 图形界面 (GUI)

本目录 (gui_qt/) 是《数据结构课程练习项目-智能股票》的 **Qt 图形界面（选做）** 实现，
将第 1~20 关的全部功能集成到 Qt Widgets 桌面应用，用户通过按钮/输入框操作，
无需命令行交互。

## 界面结构

主窗口包含 5 个功能标签页：

| 标签页 | 功能 |
| ------ | ---- |
| ① 信息维护 (1-3关) | 第1关 按位置输出股票、第2关 删除股票、第3关 修改公司全称 |
| ② 信息查找 (4-8关) | 第4关 折半查找、第5关 二叉排序树、第6关 字典树前缀、第7关 开放地址散列、第8关 链地址散列 |
| ③ 统计分析 (9-13关) | 第9关 链表涨跌额分析、第10关 并查集换手率、第11关 堆排序Top3、第12关 归并排序成交量、第13关 决策树分类 |
| ④ 知识图谱 (14-17关) | 第14关 实体识别、第15关 关系抽取、第16关 邻接表知识图谱构建、第17关 知识图谱推荐 |
| ⑤ 股票预测 (18-20关) | 第18关 皮尔逊相关分析、第19关 朴素贝叶斯预测(选做)、第20关 KNN预测(选做) |

每个功能面板：输入参数（一行一个）→ 点击「执行」→ 结果输出到文本框。
所有功能**直接复用现有 C 代码**（stock_maintain.c / stock_seek.c / stock_statistical_analysis.c /
stock_choice.c / stock_prediction.c），GUI 通过重定向 stdin/stdout 与 C 函数交互，
与头歌平台评测逻辑完全一致。

## 运行

双击 `output\gui\StockGUI.exe` 即可（需与 stocks.txt、entity.txt、relation.txt、detail/ 同目录，
部署脚本已自动放置）。

## 构建 (Windows + MSVC + Qt 6.8)

环境要求：
- Visual Studio 18 Community (MSVC x64)
- Qt 6.8.0 msvc2022_64 (已下载并解压到 `qt\extract\6.8.0\msvc2022_64`)

步骤：
1. `pwsh -ExecutionPolicy Bypass -File build_gui.ps1`  —— 编译生成 `gui_qt\release\StockGUI.exe`
2. `pwsh -ExecutionPolicy Bypass -File deploy_gui.ps1` —— 部署到 `output\gui`

构建说明：qmake 的编译器探测在受限环境下不可用，故直接调用 cl.exe 编译
(moc → 编译 C/C++ 源 → link)，链接 Qt6Core/Gui/Widgets 与 Qt6EntryPoint。

## 测试

`gui_qt\run_smoke_test.ps1` 编译并运行 18 项自动化冒烟测试（覆盖第1/4/6/7/8/9/10/11/12/13/14/15/16/17/18/19/20关），
验证 GUI 与 C 函数的重定向交互全部可用。
