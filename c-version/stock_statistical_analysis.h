#ifndef STOCK_STATISTICAL_ANALYSIS_H
#define STOCK_STATISTICAL_ANALYSIS_H

#include "stock.h"

//第9关 基于链表的股票涨跌额分析
//以涨跌额0为分界线: 小于0在前, 等于0居中, 大于0在后
void StockRiseFallAnalysis_List(Stock stock[], int stock_count);

//第10关 基于并查集的股票行业换手率分析
//用并查集按行业编码归类, 汇聚指定日期该行业全部股票换手率之和
void StockTurnoverAnalysis_UnionFind(Stock stock[], int stock_count);

//第11关 基于堆排序的股票成交量统计
//指定日期与行业编码, 堆排序后按成交量从大到小输出Top3股票全部信息
void StockVolumeAnalysis_Heap(Stock stock[], int stock_count);

//第12关 基于归并排序的股票成交量统计
//指定行业编码, 按日期累加成交量到新顺序表, 再按成交量从小到大归并排序输出
void StockVolumeAnalysis_Merge(Stock stock[], int stock_count);

//第13关 基于决策树的股票分类统计
//依次按"二级门类""最近一天涨跌幅""最近一天成交量"进行决策, 构建决策树(二叉树存储)
void StockClassify_DecisionTree(Stock stock[], int stock_count);

#endif // STOCK_STATISTICAL_ANALYSIS_H
