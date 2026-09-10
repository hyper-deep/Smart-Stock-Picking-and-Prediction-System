QT += core gui widgets

TARGET = StockGUI
TEMPLATE = app
CONFIG += c++17 console
CONFIG -= app_bundle

# MSVC: 源文件按 UTF-8 处理(避免中文注释/字符串乱码)
win32-msvc* {
    QMAKE_CXXFLAGS += /utf-8
    QMAKE_CFLAGS += /utf-8
}

INCLUDEPATH += ..

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    console_capture.cpp \
    ../stock_maintain.c \
    ../stock_seek.c \
    ../stock_statistical_analysis.c \
    ../stock_choice.c \
    ../stock_prediction.c \
    ../stock_sort.c

HEADERS += \
    mainwindow.h \
    console_capture.h \
    c_api.h
