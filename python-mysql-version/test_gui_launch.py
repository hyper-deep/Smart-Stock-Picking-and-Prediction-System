# -*- coding: utf-8 -*-
"""真实GUI启动冒烟测试: 启动窗口, 1.5秒后自动关闭(验证Qt平台插件正常加载)"""
import os
import sys
import threading

PROJECT = r"D:\creative\Smart Stock Picking and Prediction System(python+mysql)"
sys.path.insert(0, PROJECT)
sys.path.insert(0, os.path.join(PROJECT, "pylibs"))

from PySide6.QtCore import QTimer
from PySide6.QtWidgets import QApplication
import gui

app = QApplication(sys.argv)
app.setStyle("Fusion")
win = gui.MainWindow()
win.show()

# 1.5秒后自动退出
QTimer.singleShot(1500, app.quit)
print("GUI窗口已显示, 等待自动关闭...")
rc = app.exec()
print(f"GUI正常退出, exit code={rc}")
sys.exit(rc)
