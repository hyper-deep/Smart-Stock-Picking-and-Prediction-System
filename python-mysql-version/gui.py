# -*- coding: utf-8 -*-
"""
智能股票分析与预测系统 — Qt 图形界面 (GUI)
参考C版: gui_qt/mainwindow.cpp (5个功能标签页, 每关一个面板)
技术栈: PySide6 (Qt6) + MySQL
"""
import sys
import io
import os
from contextlib import redirect_stdout
from unittest.mock import patch

# 确保可以导入本项目模块
PROJECT_DIR = os.path.dirname(os.path.abspath(__file__))
if PROJECT_DIR not in sys.path:
    sys.path.insert(0, PROJECT_DIR)

# 本地 Qt 库(若 pip 环境无 PySide6, 使用 pylibs)
PYLIBS = os.path.join(PROJECT_DIR, "pylibs")
try:
    import PySide6  # noqa: F401
except ImportError:
    if os.path.isdir(PYLIBS):
        sys.path.insert(0, PYLIBS)
        import PySide6  # noqa: F401

# 从非标准位置加载时, 需要显式指定 Qt 插件/库路径
from PySide6 import __file__ as _pyside6_path
_PYSIDE6_DIR = os.path.dirname(_pyside6_path)
_PLUGINS_DIR = os.path.join(_PYSIDE6_DIR, "plugins")
if os.path.isdir(_PLUGINS_DIR):
    os.environ.setdefault("QT_QPA_PLATFORM_PLUGIN_PATH", _PLUGINS_DIR)
    os.environ.setdefault("QT_PLUGIN_PATH", _PLUGINS_DIR)
_LIB_DIR = os.path.join(_PYSIDE6_DIR, "lib")
if os.path.isdir(_LIB_DIR):
    os.environ.setdefault("PATH", _LIB_DIR + os.pathsep + os.environ.get("PATH", ""))

try:
    from PySide6.QtWidgets import (
        QApplication, QMainWindow, QWidget, QTabWidget, QVBoxLayout,
        QHBoxLayout, QFormLayout, QPushButton, QLabel, QLineEdit,
        QPlainTextEdit, QGroupBox, QScrollArea, QStatusBar, QMessageBox,
        QSplitter,
    )
    from PySide6.QtGui import QFont, QTextCursor
    from PySide6.QtCore import Qt
except ImportError as e:
    print(f"[GUI] PySide6 不可用: {e}")
    print("[GUI] 请先执行: pip install PySide6 或使用 pylibs 目录")
    raise SystemExit(1)

# 导入各关卡模块
import stock_maintain
import stock_seek
import stock_statistical
import stock_choice
import stock_prediction


def run_level(func, inputs):
    """
    运行一个关卡函数, 将输入序列喂给 input(), 捕获 print 输出。
    返回输出文本; 输入不足时返回错误提示。
    """
    out = io.StringIO()
    try:
        with patch("builtins.input", side_effect=list(inputs)), redirect_stdout(out):
            func()
    except StopIteration:
        return "输入参数不足或格式错误, 请检查输入!"
    except Exception as e:
        return f"执行出错: {type(e).__name__}: {e}"
    return out.getvalue()


class LevelPanel(QWidget):
    """通用功能面板: 标题 + N个输入框 + 执行按钮 + 输出区"""

    def __init__(self, title, input_labels, exec_fn, parent=None):
        super().__init__(parent)
        self.exec_fn = exec_fn
        self.inputs = []

        lay = QVBoxLayout(self)
        box = QGroupBox(title, self)
        box_lay = QVBoxLayout(box)

        if input_labels:
            form = QFormLayout()
            for label in input_labels:
                edit = QLineEdit(box)
                edit.setPlaceholderText(label)
                form.addRow(label, edit)
                self.inputs.append(edit)
            box_lay.addLayout(form)

        btn = QPushButton("执  行", box)
        btn.setMinimumHeight(34)
        btn.clicked.connect(self.on_run)
        box_lay.addWidget(btn)

        self.output = QPlainTextEdit(box)
        self.output.setReadOnly(True)
        self.output.setMinimumHeight(160)
        mono = QFont("Consolas")
        mono.setPointSize(10)
        self.output.setFont(mono)
        box_lay.addWidget(self.output)

        lay.addWidget(box)

    def on_run(self):
        inputs = [e.text().strip() for e in self.inputs]
        result = self.exec_fn(inputs)
        self.output.setPlainText(result)

    def set_output(self, text):
        self.output.setPlainText(text)

    def append_output(self, text):
        self.output.appendPlainText(text)


class MainWindow(QMainWindow):
    """主窗口: 5个功能标签页"""

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("智能股票分析与预测系统 — Qt 图形界面 (Python + MySQL)")
        self.resize(1150, 800)

        tabs = QTabWidget(self)
        tabs.addTab(self.build_maintain_tab(), "① 信息维护 (1-3关)")
        tabs.addTab(self.build_seek_tab(),     "② 信息查找 (4-8关)")
        tabs.addTab(self.build_stat_tab(),     "③ 统计分析 (9-13关)")
        tabs.addTab(self.build_graph_tab(),    "④ 知识图谱 (14-17关)")
        tabs.addTab(self.build_predict_tab(),  "⑤ 股票预测 (18-20关)")
        self.setCentralWidget(tabs)

        self.statusBar().showMessage("数据源: MySQL (stock_information 库) — 点击各面板「执行」运行对应关卡功能")

    # ---------- 标签页1: 信息维护 (1-3关) ----------

    def build_maintain_tab(self):
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        page = QWidget()
        lay = QVBoxLayout(page)

        lay.addWidget(LevelPanel(
            "第1关 股票信息存储 — 按位置输出该股票全部信息",
            ["位置 pos (1~300)"],
            lambda in_: "请输入位置编号!" if not (in_ and in_[0]) else run_level(stock_maintain.level1, [in_[0]]),
        ))

        lay.addWidget(LevelPanel(
            "第2关 股票信息删除 — 按股票简称删除并写入 new_stocks.txt",
            ["股票简称 (如: 贵州茅台)"],
            lambda in_: "请输入股票简称!" if not (in_ and in_[0]) else run_level(stock_maintain.level2, [in_[0]]),
        ))

        lay.addWidget(LevelPanel(
            "第3关 股票信息修改 — 修改公司全称并写入 new_stocks.txt",
            ["股票简称", "新的公司全称"],
            lambda in_: "请完整输入股票简称与新公司全称!" if not (in_ and in_[0] and in_[1]) else run_level(stock_maintain.level3, [in_[0], in_[1]]),
        ))

        lay.addStretch()
        scroll.setWidget(page)
        return scroll

    # ---------- 标签页2: 信息查找 (4-8关) ----------

    def build_seek_tab(self):
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        page = QWidget()
        lay = QVBoxLayout(page)

        lay.addWidget(LevelPanel(
            "第4关 基于折半查找的股票信息查找 (按英文名, 先归并排序)",
            ["股票英文名称"],
            lambda in_: "请输入英文名称!" if not (in_ and in_[0]) else run_level(stock_seek.level4, [in_[0]]),
        ))
        lay.addWidget(LevelPanel(
            "第5关 基于二叉排序树的股票信息查找 (原始/排序后两种BST对比ASL)",
            ["股票英文名称"],
            lambda in_: "请输入英文名称!" if not (in_ and in_[0]) else run_level(stock_seek.level5, [in_[0]]),
        ))
        lay.addWidget(LevelPanel(
            "第6关 基于字典树的股票信息查找 (按英文名前缀)",
            ["英文名前缀"],
            lambda in_: "请输入前缀!" if not (in_ and in_[0]) else run_level(stock_seek.level6, [in_[0]]),
        ))
        lay.addWidget(LevelPanel(
            "第7关 基于开放地址法的散列查找 (线性探测)",
            ["股票英文名称"],
            lambda in_: "请输入英文名称!" if not (in_ and in_[0]) else run_level(stock_seek.level7, [in_[0]]),
        ))
        lay.addWidget(LevelPanel(
            "第8关 基于链地址法的散列查找 (含每日行情)",
            ["股票英文名称"],
            lambda in_: "请输入英文名称!" if not (in_ and in_[0]) else run_level(stock_seek.level8, [in_[0]]),
        ))

        lay.addStretch()
        scroll.setWidget(page)
        return scroll

    # ---------- 标签页3: 统计分析 (9-13关) ----------

    def build_stat_tab(self):
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        page = QWidget()
        lay = QVBoxLayout(page)

        lay.addWidget(LevelPanel(
            "第9关 基于链表的股票涨跌额分析 (涨跌额<0前,=0中,>0后)",
            ["行业编码 (如C39)", "日期 (如2026-07-22)"],
            lambda in_: "请完整输入行业编码与日期!" if not (in_ and in_[0] and in_[1]) else run_level(stock_statistical.level9, [in_[0], in_[1]]),
        ))
        lay.addWidget(LevelPanel(
            "第10关 基于并查集的行业换手率分析",
            ["行业编码 (如C39)", "日期 (如2026-07-22)"],
            lambda in_: "请完整输入行业编码与日期!" if not (in_ and in_[0] and in_[1]) else run_level(stock_statistical.level10, [in_[0], in_[1]]),
        ))
        lay.addWidget(LevelPanel(
            "第11关 基于堆排序的成交量Top3统计",
            ["日期 (如2026-07-22)", "行业编码 (如C39)"],
            lambda in_: "请完整输入日期与行业编码!" if not (in_ and in_[0] and in_[1]) else run_level(stock_statistical.level11, [in_[0], in_[1]]),
        ))
        lay.addWidget(LevelPanel(
            "第12关 基于归并排序的成交量统计 (按日期汇总)",
            ["行业编码 (如C39)"],
            lambda in_: "请输入行业编码!" if not (in_ and in_[0]) else run_level(stock_statistical.level12, [in_[0]]),
        ))
        lay.addWidget(LevelPanel(
            "第13关 基于决策树的股票分类统计",
            ["二级门类 (如:计算机、通信和其他电子设备制造业)", "涨跌幅条件 (如>0)", "成交量条件 (如>1000万股)", "股票简称"],
            lambda in_: "请完整输入决策条件!" if not (in_ and in_[0] and in_[1] and in_[2] and in_[3]) else run_level(stock_statistical.level13, [in_[0], in_[1], in_[2], in_[3]]),
        ))

        lay.addStretch()
        scroll.setWidget(page)
        return scroll

    # ---------- 标签页4: 知识图谱 (14-17关) ----------

    def build_graph_tab(self):
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        page = QWidget()
        lay = QVBoxLayout(page)

        lay.addWidget(LevelPanel(
            "第14关 基于规则的实体识别 (BF算法, *匹配任意中文字符)",
            ["匹配规则 (如 ****隶属于)", "非结构化文本"],
            lambda in_: "请完整输入规则与文本!" if not (in_ and in_[0] and in_[1]) else run_level(stock_choice.level14, [in_[0], in_[1]]),
        ))
        lay.addWidget(LevelPanel(
            "第15关 基于规则的关系抽取 (BF算法)",
            ["实体1 (头实体)", "实体2 (尾实体)", "非结构化文本"],
            lambda in_: "请完整输入两实体与文本!" if not (in_ and in_[0] and in_[1] and in_[2]) else run_level(stock_choice.level15, [in_[0], in_[1], in_[2]]),
        ))
        lay.addWidget(LevelPanel(
            "第16关 基于邻接表的股票知识图谱构建",
            [],
            lambda in_: run_level(stock_choice.level16, []),
        ))
        lay.addWidget(LevelPanel(
            "第17关 基于知识图谱的股票推荐",
            ["股票相关信息 (如行业编码C39/股票简称/机构名称)"],
            lambda in_: "请输入信息!" if not (in_ and in_[0]) else run_level(stock_choice.level17, [in_[0]]),
        ))

        lay.addStretch()
        scroll.setWidget(page)
        return scroll

    # ---------- 标签页5: 股票预测 (18-20关) ----------

    def build_predict_tab(self):
        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        page = QWidget()
        lay = QVBoxLayout(page)

        lay.addWidget(LevelPanel(
            "第18关 基于皮尔逊相关系数的指标重要性分析",
            ["股票代码 (如 cn_600519)"],
            lambda in_: "请输入股票代码!" if not (in_ and in_[0]) else run_level(stock_prediction.level18, [in_[0]]),
        ))
        lay.addWidget(LevelPanel(
            "第19关 基于朴素贝叶斯的涨跌预测 (选做)",
            ["股票代码 (如 cn_600519)"],
            lambda in_: "请输入股票代码!" if not (in_ and in_[0]) else run_level(stock_prediction.level19, [in_[0]]),
        ))
        lay.addWidget(LevelPanel(
            "第20关 基于K近邻的涨跌幅预测 (选做)",
            ["股票代码 (如 cn_600519)"],
            lambda in_: "请输入股票代码!" if not (in_ and in_[0]) else run_level(stock_prediction.level20, [in_[0]]),
        ))

        lay.addStretch()
        scroll.setWidget(page)
        return scroll


def main():
    # Windows 下使用 Fusion 风格, 界面更现代
    app = QApplication(sys.argv)
    app.setStyle("Fusion")
    win = MainWindow()
    win.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
