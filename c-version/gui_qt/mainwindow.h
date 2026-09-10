#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <functional>
#include <QList>

#include "c_api.h"

// 通用功能面板: 标题 + N个输入框 + 执行按钮 + 输出区
class LevelPanel : public QWidget
{
    Q_OBJECT
public:
    // title: 面板标题; inputLabels: 各输入框的标签; exec: 接收输入文本列表, 返回输出文本
    LevelPanel(const QString& title, const QStringList& inputLabels,
               std::function<QString(const QStringList&)> exec,
               QWidget* parent = nullptr);

    // 追加一段文本到输出区(供日志/状态显示)
    void appendOutput(const QString& text);
    void setOutput(const QString& text);

private slots:
    void onRun();

private:
    QList<QLineEdit*> m_inputs;
    QPlainTextEdit* m_output;
    std::function<QString(const QStringList&)> m_exec;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);

private:
    Stock stocks[MAX_STOCK];   // 股票数组(stock[0] 为字段名, 数据从 stock[1] 开始)
    int stock_count = 0;

    // 构建各功能标签页
    QWidget* buildMaintainTab();
    QWidget* buildSeekTab();
    QWidget* buildStatTab();
    QWidget* buildGraphTab();
    QWidget* buildPredictTab();

    // 重置所有股票的 detail_count(避免 add_stock_detail 重复追加)
    void resetDetailCount();

    // 重新加载 stocks.txt
    void reloadStocks();
};
