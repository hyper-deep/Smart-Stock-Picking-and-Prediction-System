// GUI 功能自动化冒烟测试:
// 1) 验证 ConsoleCapture 重定向能捕获 C 函数 printf 输出
// 2) 验证每关 C 函数在 GUI 环境下可执行(通过重定向输入)
// 以控制台方式运行, 不启动事件循环
#include <QCoreApplication>
#include <QDebug>
#include <cstdio>
#include "console_capture.h"
#include "c_api.h"

static int g_fail = 0;

static void check(const QString& name, bool ok, const QString& detail = QString())
{
    qInfo().noquote() << (ok ? "[PASS]" : "[FAIL]") << name << detail;
    if (!ok) g_fail++;
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    // 准备数据(工作目录需为项目根)
    static Stock stocks[MAX_STOCK];
    int count = read_stock_file((char*)"stocks.txt", stocks);
    check("read_stock_file", count == 300, QString("count=%1").arg(count));

    // ---- 1. ConsoleCapture 捕获 printf ----
    QString out = ConsoleCapture::run({}, []() {
        printf("hello-capture-42\n");
    });
    check("ConsoleCapture capture", out.contains("hello-capture-42"), out.trimmed());

    // ---- 2. 第1关: 按位置输出 (含 GUI-v2 版本标记; 验证首次调用不报"输入文件创建失败") ----
    out = ConsoleCapture::run({"1"}, [&]() { search_by_pos(stocks, count); });
    check("L1 search_by_pos(first call)", out.contains("贵州茅台") && out.contains("GUI-v2")
          && !out.contains("输入文件创建失败"), "pos=1");

    // ---- 3. 第4关: 折半查找 (fgets 读整行, 支持含空格英文名) ----
    out = ConsoleCapture::run({"Kweichow Moutai"}, [&]() { Binary_Search_stock(stocks, count); });
    check("L4 Binary_Search space-name", out.contains("查找成功"), out.left(60));

    // ---- 4. 第6关: 字典树前缀 (放在排序型关卡前, 避免数组顺序被改变) ----
    out = ConsoleCapture::run({"Kweichow"}, [&]() { Trie_Search_stock(stocks, count); });
    check("L6 Trie prefix", out.contains("共找到"), out.left(60));

    // ---- 5. 第7关: 开放地址散列 (fgets 读整行, 含空格完整名) ----
    out = ConsoleCapture::run({"Kweichow Moutai"}, [&]() { Hash_lookup_Search(stocks, count); });
    check("L7 hash lookup", out.contains("查找成功"), out.left(60));

    // ---- 6. 第8关: 链地址散列 (需读取detail行情, 验证无"正在读取/读取完成"噪音) ----
    out = ConsoleCapture::run({"Kweichow Moutai"}, [&]() { Hash_seperate_Chain_Search(stocks, count); });
    check("L8 chain hash", out.contains("查找成功")
          && !out.contains("正在读取") && !out.contains("读取完成"), out.left(60));

    // ---- 7. 第9关: 链表涨跌额分析 (验证无进度噪音) ----
    out = ConsoleCapture::run({"B01", "2026-07-22"}, [&]() { StockRiseFallAnalysis_List(stocks, count); });
    check("L9 risefall list", out.contains("涨跌额")
          && !out.contains("正在读取") && !out.contains("读取完成"), out.left(60));

    // ---- 8. 第10关: 并查集换手率 ----
    out = ConsoleCapture::run({"B01", "2026-07-22"}, [&]() { StockTurnoverAnalysis_UnionFind(stocks, count); });
    check("L10 union-find", out.contains("换手率之和"), out.left(60));

    // ---- 9. 第11关: 堆排序Top3 ----
    out = ConsoleCapture::run({"2026-07-22", "B01"}, [&]() { StockVolumeAnalysis_Heap(stocks, count); });
    check("L11 heap top3", out.contains("Top3"), out.left(60));

    // ---- 10. 第12关: 归并排序成交量 ----
    out = ConsoleCapture::run({"B01"}, [&]() { StockVolumeAnalysis_Merge(stocks, count); });
    check("L12 merge volume", out.contains("成交量合计"), out.left(60));

    // ---- 11. 第13关: 决策树 ----
    out = ConsoleCapture::run({"食品饮料", ">0", ">1000万股", "贵州茅台"}, [&]() { StockClassify_DecisionTree(stocks, count); });
    check("L13 decision tree", out.contains("决策条件"), out.left(60));

    // ---- 12. 第14关: 实体识别 ----
    out = ConsoleCapture::run({"****隶属于", "某公司隶属于贵州茅台酒股份有限公司, 该公司位于仁怀市。"}, []() { EntityRecognition_BF(); });
    check("L14 entity BF", out.contains("实体"), out.left(60));

    // ---- 13. 第15关: 关系抽取 ----
    out = ConsoleCapture::run({"贵州茅台", "贵州茅台酒股份有限公司", "贵州茅台所属机构为贵州茅台酒股份有限公司"}, []() { RelationExtraction_BF(); });
    check("L15 relation", out.contains("所属机构"), out.left(60));

    // ---- 14. 第16关: 知识图谱构建 ----
    out = ConsoleCapture::run({}, []() { BuildKnowledgeGraph_ALGraph(); });
    check("L16 build KG", out.contains("构建完成"), out.left(80));

    // ---- 15. 第17关: 知识图谱推荐 (行业编码 B01 存在于图谱) ----
    out = ConsoleCapture::run({"B01"}, []() { StockRecommend_KnowledgeGraph(); });
    check("L17 recommend", out.contains("关联到") || out.contains("推荐"), out.left(80));

    // ---- 16. 第18关: 皮尔逊相关 ----
    out = ConsoleCapture::run({"cn_600519"}, []() { Pearson_Correlation_Analysis(); });
    check("L18 pearson", out.contains("前3个指标"), out.left(80));

    // ---- 17. 第19关: 朴素贝叶斯 ----
    out = ConsoleCapture::run({"cn_600519"}, []() { NaiveBayes_StockPredict(); });
    check("L19 naive bayes", out.contains("预测结论"), out.left(80));

    // ---- 18. 第20关: KNN ----
    out = ConsoleCapture::run({"cn_600519"}, []() { KNN_StockPredict(); });
    check("L20 KNN", out.contains("预测次日涨跌幅"), out.left(80));

    // ---- 19. 第5关: 二叉排序树查找 (放最后: 内部 Quick_Sort 会改变全局数组顺序) ----
    out = ConsoleCapture::run({"Kweichow Moutai"}, [&]() { BST_Search_stock(stocks, count); });
    check("L5 BST space-name", out.contains("原始序列BST查找成功") && out.contains("排序后BST查找成功"), out.left(60));

    qInfo() << "================================";
    qInfo() << (g_fail == 0 ? "ALL PASS" : QString("FAILURES: %1").arg(g_fail));
    return g_fail == 0 ? 0 : 1;
}
