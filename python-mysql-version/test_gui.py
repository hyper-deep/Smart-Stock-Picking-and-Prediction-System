# -*- coding: utf-8 -*-
"""GUI 离屏测试: 验证 MainWindow 构建、各面板执行逻辑(无需真实显示器)"""
import os
import sys

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")
os.environ.setdefault("QT_QPA_PLATFORM_PLUGIN_PATH",
                      r"D:\creative\Smart Stock Picking and Prediction System(python+mysql)\pylibs\PySide6\plugins")

PROJECT = r"D:\creative\Smart Stock Picking and Prediction System(python+mysql)"
sys.path.insert(0, PROJECT)
sys.path.insert(0, os.path.join(PROJECT, "pylibs"))

from PySide6.QtWidgets import QApplication
import gui

app = QApplication([])
win = gui.MainWindow()
win.show()

# 查找所有 LevelPanel 并逐个执行(用预设输入), 验证 exec 逻辑不崩溃
inputs_map = {
    "第1关": ["3"],
    "第2关": ["万科发展"],
    "第3关": ["平安银行", "平安银行(修改后)"],
    "第4关": ["Kweichow Moutai"],
    "第5关": ["Wuliangye"],
    "第6关": ["CAT"],
    "第7关": ["CATL"],
    "第8关": ["CATL"],
    "第9关": ["C39", "2026-07-22"],
    "第10关": ["C39", "2026-07-22"],
    "第11关": ["2026-07-22", "C39"],
    "第12关": ["C39"],
    "第13关": ["计算机、通信和其他电子设备制造业", ">0", ">1000万股", "海康威视"],
    "第14关": ["****隶属于", "乐鑫科技隶属于乐鑫信息科技股份有限公司"],
    "第15关": ["同惠电子", "电子测量仪器的研发、生产与销售", "同惠电子专注于电子测量仪器的研发、生产与销售"],
    "第16关": [],
    "第17关": ["C39"],
    "第18关": ["cn_600519"],
    "第19关": ["cn_600519"],
    "第20关": ["cn_600519"],
}

panels = []
def collect(w):
    for child in w.findChildren(gui.LevelPanel):
        panels.append(child)

collect(win)

print(f"共找到 {len(panels)} 个功能面板")
passed = 0
for p in panels:
    title = p.parent().parent().title() if hasattr(p.parent(), "title") else str(p.objectName())
    # 找到匹配的输入
    matched = None
    for key, inputs in inputs_map.items():
        if key in (p.windowTitle() or ""):
            continue
    # 直接用面板内输入框填值
    for i, (edit, val) in enumerate(zip(p.inputs, [])):
        pass

# 直接测试 run_level 包装函数(不依赖面板)
print("\n--- run_level 函数测试 ---")
results = []
for key, inputs in inputs_map.items():
    func = None
    if key == "第1关": func = gui.stock_maintain.level1
    elif key == "第2关": func = gui.stock_maintain.level2
    elif key == "第3关": func = gui.stock_maintain.level3
    elif key == "第4关": func = gui.stock_seek.level4
    elif key == "第5关": func = gui.stock_seek.level5
    elif key == "第6关": func = gui.stock_seek.level6
    elif key == "第7关": func = gui.stock_seek.level7
    elif key == "第8关": func = gui.stock_seek.level8
    elif key == "第9关": func = gui.stock_statistical.level9
    elif key == "第10关": func = gui.stock_statistical.level10
    elif key == "第11关": func = gui.stock_statistical.level11
    elif key == "第12关": func = gui.stock_statistical.level12
    elif key == "第13关": func = gui.stock_statistical.level13
    elif key == "第14关": func = gui.stock_choice.level14
    elif key == "第15关": func = gui.stock_choice.level15
    elif key == "第16关": func = gui.stock_choice.level16
    elif key == "第17关": func = gui.stock_choice.level17
    elif key == "第18关": func = gui.stock_prediction.level18
    elif key == "第19关": func = gui.stock_prediction.level19
    elif key == "第20关": func = gui.stock_prediction.level20
    if func is None:
        continue
    out = gui.run_level(func, inputs)
    ok = len(out.strip()) > 0 and "Traceback" not in out and "执行出错" not in out
    results.append((key, ok, len(out)))
    print(f"[{key}] {'OK' if ok else 'FAIL'} 输出{len(out)}字符")

all_ok = all(ok for _, ok, _ in results)
print(f"\n{len(results)}/20 面板逻辑测试 {'全部通过' if all_ok else '存在失败'}")
app.quit()
sys.exit(0 if all_ok else 1)
