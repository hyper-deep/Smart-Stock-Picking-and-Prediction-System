#include "mainwindow.h"
#include "console_capture.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QScrollArea>
#include <QMessageBox>
#include <QComboBox>
#include <QStatusBar>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <cstring>

/* ==================== LevelPanel ==================== */

LevelPanel::LevelPanel(const QString& title, const QStringList& inputLabels,
                       std::function<QString(const QStringList&)> exec, QWidget* parent)
    : QWidget(parent), m_exec(std::move(exec))
{
    QVBoxLayout* lay = new QVBoxLayout(this);

    QGroupBox* box = new QGroupBox(title, this);
    QVBoxLayout* boxLay = new QVBoxLayout(box);

    if (!inputLabels.isEmpty()) {
        QFormLayout* form = new QFormLayout;
        for (const QString& label : inputLabels) {
            QLineEdit* edit = new QLineEdit(box);
            edit->setPlaceholderText(label);
            form->addRow(label, edit);
            m_inputs.append(edit);
        }
        boxLay->addLayout(form);
    }

    QPushButton* btn = new QPushButton(QStringLiteral("执  行"), box);
    btn->setMinimumHeight(34);
    connect(btn, &QPushButton::clicked, this, &LevelPanel::onRun);
    boxLay->addWidget(btn);

    m_output = new QPlainTextEdit(box);
    m_output->setReadOnly(true);
    m_output->setMinimumHeight(180);
    QFont mono(QStringLiteral("Consolas"));
    mono.setPointSize(10);
    m_output->setFont(mono);
    boxLay->addWidget(m_output);

    lay->addWidget(box);
}

void LevelPanel::appendOutput(const QString& text)
{
    m_output->appendPlainText(text);
}

void LevelPanel::setOutput(const QString& text)
{
    m_output->setPlainText(text);
}

void LevelPanel::onRun()
{
    QStringList inputs;
    for (QLineEdit* e : m_inputs)
        inputs << e->text().trimmed();
    if (!m_exec) {
        setOutput(QStringLiteral("未定义执行逻辑"));
        return;
    }
    QString result = m_exec(inputs);
    setOutput(result);
}

/* ==================== MainWindow ==================== */

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("智能股票分析与预测系统 — Qt 图形界面"));
    resize(1100, 760);

    QTabWidget* tabs = new QTabWidget(this);
    tabs->addTab(buildMaintainTab(), QStringLiteral("① 信息维护 (1-3关)"));
    tabs->addTab(buildSeekTab(),     QStringLiteral("② 信息查找 (4-8关)"));
    tabs->addTab(buildStatTab(),     QStringLiteral("③ 统计分析 (9-13关)"));
    tabs->addTab(buildGraphTab(),    QStringLiteral("④ 知识图谱 (14-17关)"));
    tabs->addTab(buildPredictTab(),  QStringLiteral("⑤ 股票预测 (18-20关)"));
    setCentralWidget(tabs);

    reloadStocks();
    if (stock_count > 0) {
        statusBar()->showMessage(QStringLiteral("已加载 %1 支股票 (stocks.txt)").arg(stock_count));
    } else {
        statusBar()->showMessage(QStringLiteral("加载股票失败: 请确认 stocks.txt 与 detail 目录位于程序工作目录"));
    }
}

void MainWindow::resetDetailCount()
{
    for (int i = 1; i <= stock_count; i++)
        stocks[i].detail_count = 0;
}

void MainWindow::reloadStocks()
{
    stock_count = read_stock_file((char*)"stocks.txt", stocks);
}

QWidget* MainWindow::buildMaintainTab()
{
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    QWidget* page = new QWidget;
    QVBoxLayout* lay = new QVBoxLayout(page);

    // ---------- 第1关 股票信息存储(按位置输出) ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第1关 股票信息存储 — 按位置输出该股票全部信息"),
        { QStringLiteral("位置 pos (1~%1)").arg(stock_count ? stock_count : 300) },
        [this](const QStringList& in) {
            if (in.isEmpty() || in[0].isEmpty())
                return QStringLiteral("请输入位置编号!");
            return ConsoleCapture::run(in, [this]() { search_by_pos(stocks, stock_count); });
        }));

    // ---------- 第2关 删除股票 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第2关 股票信息删除 — 按股票简称删除并写入 new_stocks.txt"),
        { QStringLiteral("股票简称 (如: 贵州茅台)") },
        [this](const QStringList& in) {
            if (in.isEmpty() || in[0].isEmpty())
                return QStringLiteral("请输入股票简称!");
            return ConsoleCapture::run(in, [this, name = in[0]]() {
                char buf[100];
                strcpy_s(buf, sizeof(buf), name.toUtf8().constData());
                delete_stock(stocks, buf, &stock_count);
            });
        }));

    // ---------- 第3关 修改股票 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第3关 股票信息修改 — 修改公司全称并写入 new_stocks.txt"),
        { QStringLiteral("股票简称"), QStringLiteral("新的公司全称") },
        [this](const QStringList& in) {
            if (in.size() < 2 || in[0].isEmpty() || in[1].isEmpty())
                return QStringLiteral("请完整输入股票简称与新公司全称!");
            return ConsoleCapture::run(in, [this, a = in[0], b = in[1]]() {
                char na[100], nb[200];
                strcpy_s(na, sizeof(na), a.toUtf8().constData());
                strcpy_s(nb, sizeof(nb), b.toUtf8().constData());
                revise_stock(stocks, stock_count, na, nb);
            });
        }));

    // ---------- 重新加载数据 ----------
    QPushButton* reload = new QPushButton(QStringLiteral("重新加载 stocks.txt 数据"));
    connect(reload, &QPushButton::clicked, this, [this]() {
        reloadStocks();
        statusBar()->showMessage(QStringLiteral("已重新加载 %1 支股票").arg(stock_count));
    });
    lay->addWidget(reload);
    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget* MainWindow::buildSeekTab()
{
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    QWidget* page = new QWidget;
    QVBoxLayout* lay = new QVBoxLayout(page);

    // ---------- 第4关 折半查找 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第4关 基于折半查找的股票信息查找 (按英文名, 先归并排序)"),
        { QStringLiteral("股票英文名称") },
        [this](const QStringList& in) {
            if (in.isEmpty() || in[0].isEmpty())
                return QStringLiteral("请输入英文名称!");
            return ConsoleCapture::run(in, [this]() { Binary_Search_stock(stocks, stock_count); });
        }));

    // ---------- 第5关 二叉排序树查找 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第5关 基于二叉排序树的股票信息查找 (原始/排序后两种BST对比ASL)"),
        { QStringLiteral("股票英文名称") },
        [this](const QStringList& in) {
            if (in.isEmpty() || in[0].isEmpty())
                return QStringLiteral("请输入英文名称!");
            return ConsoleCapture::run(in, [this]() { BST_Search_stock(stocks, stock_count); });
        }));

    // ---------- 第6关 字典树前缀查找 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第6关 基于字典树的股票信息查找 (英文名前缀)"),
        { QStringLiteral("英文名称前缀") },
        [this](const QStringList& in) {
            if (in.isEmpty() || in[0].isEmpty())
                return QStringLiteral("请输入英文名称前缀!");
            return ConsoleCapture::run(in, [this]() { Trie_Search_stock(stocks, stock_count); });
        }));

    // ---------- 第7关 开放地址散列查找 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第7关 基于开放地址法的散列查找 (线性探测)"),
        { QStringLiteral("股票英文名称") },
        [this](const QStringList& in) {
            if (in.isEmpty() || in[0].isEmpty())
                return QStringLiteral("请输入英文名称!");
            return ConsoleCapture::run(in, [this]() { Hash_lookup_Search(stocks, stock_count); });
        }));

    // ---------- 第8关 链地址散列查找 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第8关 基于链地址法的散列查找 (需读取detail行情)"),
        { QStringLiteral("股票英文名称") },
        [this](const QStringList& in) {
            if (in.isEmpty() || in[0].isEmpty())
                return QStringLiteral("请输入英文名称!");
            return ConsoleCapture::run(in, [this]() { Hash_seperate_Chain_Search(stocks, stock_count); });
        }));

    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget* MainWindow::buildStatTab()
{
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    QWidget* page = new QWidget;
    QVBoxLayout* lay = new QVBoxLayout(page);

    // ---------- 第9关 链表涨跌额分析 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第9关 基于链表的股票涨跌额分析 (负/零/正三段排序)"),
        { QStringLiteral("行业编码 (如 B01)"), QStringLiteral("日期 (如 2026-07-22)") },
        [this](const QStringList& in) {
            if (in.size() < 2 || in[0].isEmpty() || in[1].isEmpty())
                return QStringLiteral("请完整输入行业编码与日期!");
            return ConsoleCapture::run(in, [this]() { StockRiseFallAnalysis_List(stocks, stock_count); });
        }));

    // ---------- 第10关 并查集换手率分析 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第10关 基于并查集的股票行业换手率分析 (按行业求和)"),
        { QStringLiteral("行业编码"), QStringLiteral("日期 (如 2026-07-22)") },
        [this](const QStringList& in) {
            if (in.size() < 2 || in[0].isEmpty() || in[1].isEmpty())
                return QStringLiteral("请完整输入行业编码与日期!");
            return ConsoleCapture::run(in, [this]() { StockTurnoverAnalysis_UnionFind(stocks, stock_count); });
        }));

    // ---------- 第11关 堆排序成交量Top3 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第11关 基于堆排序的股票成交量统计 (行业Top3)"),
        { QStringLiteral("日期"), QStringLiteral("行业编码") },
        [this](const QStringList& in) {
            if (in.size() < 2 || in[0].isEmpty() || in[1].isEmpty())
                return QStringLiteral("请完整输入日期与行业编码!");
            return ConsoleCapture::run(in, [this]() { StockVolumeAnalysis_Heap(stocks, stock_count); });
        }));

    // ---------- 第12关 归并排序成交量统计 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第12关 基于归并排序的股票成交量统计 (按日期累加汇总)"),
        { QStringLiteral("行业编码") },
        [this](const QStringList& in) {
            if (in.isEmpty() || in[0].isEmpty())
                return QStringLiteral("请输入行业编码!");
            return ConsoleCapture::run(in, [this]() { StockVolumeAnalysis_Merge(stocks, stock_count); });
        }));

    // ---------- 第13关 决策树分类统计 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第13关 基于决策树的股票分类统计 (二级门类/涨跌幅/成交量)"),
        { QStringLiteral("二级门类 (如: 食品饮料)"),
          QStringLiteral("最近一天涨跌幅条件 (如: >0)"),
          QStringLiteral("最近一天成交量条件 (如: >1000万股)"),
          QStringLiteral("股票简称 (查询对象)") },
        [this](const QStringList& in) {
            if (in.size() < 4 || in[0].isEmpty() || in[1].isEmpty() || in[2].isEmpty() || in[3].isEmpty())
                return QStringLiteral("请完整输入四个参数!");
            return ConsoleCapture::run(in, [this]() { StockClassify_DecisionTree(stocks, stock_count); });
        }));

    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget* MainWindow::buildGraphTab()
{
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    QWidget* page = new QWidget;
    QVBoxLayout* lay = new QVBoxLayout(page);

    // ---------- 第14关 实体识别 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第14关 基于规则的股票信息实体识别 (BF算法, *匹配中文字符)"),
        { QStringLiteral("匹配规则 (如: ****隶属于)"), QStringLiteral("非结构化文本") },
        [](const QStringList& in) {
            if (in.size() < 2 || in[0].isEmpty() || in[1].isEmpty())
                return QStringLiteral("请完整输入规则与文本!");
            return ConsoleCapture::run(in, []() { EntityRecognition_BF(); });
        }));

    // ---------- 第15关 关系抽取 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第15关 基于规则的股票信息关系抽取 (两实体间匹配关系规则)"),
        { QStringLiteral("实体1 (头实体)"), QStringLiteral("实体2 (尾实体)"), QStringLiteral("非结构化文本") },
        [](const QStringList& in) {
            if (in.size() < 3 || in[0].isEmpty() || in[1].isEmpty() || in[2].isEmpty())
                return QStringLiteral("请完整输入两个实体与文本!");
            return ConsoleCapture::run(in, []() { RelationExtraction_BF(); });
        }));

    // ---------- 第16关 知识图谱构建 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第16关 基于邻接表的股票知识图谱构建 (读取 entity.txt / relation.txt)"),
        {},
        [this](const QStringList&) {
            return ConsoleCapture::run({}, [this]() {
                // 清空后重新构建
                BuildKnowledgeGraph_ALGraph();
            });
        }));

    // ---------- 第17关 知识图谱推荐 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第17关 基于知识图谱的股票推荐 (先执行第16关构建图谱)"),
        { QStringLiteral("股票相关信息 (如行业编码 C40 / 股票简称 / 机构名称)") },
        [](const QStringList& in) {
            if (in.isEmpty() || in[0].isEmpty())
                return QStringLiteral("请输入股票相关信息!");
            return ConsoleCapture::run(in, []() { StockRecommend_KnowledgeGraph(); });
        }));

    // ---------- 图谱状态查看 ----------
    QPushButton* check = new QPushButton(QStringLiteral("查看知识图谱构建状态"));
    connect(check, &QPushButton::clicked, this, [this]() {
        ALGraph* g = GetStockKnowledgeGraph();
        if (g == nullptr)
            QMessageBox::information(this, QStringLiteral("知识图谱"),
                QStringLiteral("知识图谱尚未构建, 请先在\"第16关\"面板中执行构建。"));
        else
            QMessageBox::information(this, QStringLiteral("知识图谱"),
                QStringLiteral("知识图谱已构建: 顶点数=%1, 边数=%2").arg(g->vexnum).arg(g->arcnum));
    });
    lay->addWidget(check);
    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}

QWidget* MainWindow::buildPredictTab()
{
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    QWidget* page = new QWidget;
    QVBoxLayout* lay = new QVBoxLayout(page);

    // ---------- 第18关 皮尔逊相关系数分析 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第18关 基于皮尔逊相关系数的股票指标重要性分析"),
        { QStringLiteral("股票代码或文件路径 (如 cn_600519)") },
        [](const QStringList& in) {
            if (in.isEmpty() || in[0].isEmpty())
                return QStringLiteral("请输入股票代码或文件路径!");
            return ConsoleCapture::run(in, []() { Pearson_Correlation_Analysis(); });
        }));

    // ---------- 第19关 朴素贝叶斯预测 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第19关 基于朴素贝叶斯分类器的股票涨跌预测 (选做)"),
        { QStringLiteral("股票代码或文件路径 (如 cn_600519)") },
        [](const QStringList& in) {
            if (in.isEmpty() || in[0].isEmpty())
                return QStringLiteral("请输入股票代码或文件路径!");
            return ConsoleCapture::run(in, []() { NaiveBayes_StockPredict(); });
        }));

    // ---------- 第20关 KNN预测 ----------
    lay->addWidget(new LevelPanel(
        QStringLiteral("第20关 基于K近邻算法的股票涨跌预测 (选做, K=3)"),
        { QStringLiteral("股票代码或文件路径 (如 cn_600519)") },
        [](const QStringList& in) {
            if (in.isEmpty() || in[0].isEmpty())
                return QStringLiteral("请输入股票代码或文件路径!");
            return ConsoleCapture::run(in, []() { KNN_StockPredict(); });
        }));

    lay->addStretch();
    scroll->setWidget(page);
    return scroll;
}
