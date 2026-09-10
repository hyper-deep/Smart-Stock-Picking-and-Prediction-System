#include <stdio.h>
#include "stock_maintain.h"
#include "stock_seek.h"
#include "stock_statistical_analysis.h"
#include "stock_choice.h"
#include <windows.h>

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    printf("股票信息管理系统\n");
    static Stock stocks[MAX_STOCK]; // 定义股票数组
    int stock_count = read_stock_file("stocks.txt", stocks); // 读取股票信息文件
    printf("读取到了%d条股票信息\n", stock_count);
	//search_by_pos(stocks, stock_count); // 输出特定位置的股票基本信息
    //add_stock_detail(stocks, stock_count); // 添加股票当日的交易信息
	//delete_stock(stocks, "贵州茅台", &stock_count);  // 删除股票信息
    //printf("剩余股票数量:%d\n", stock_count); 
	//revise_stock(stocks, stock_count, "贵州茅台", "贵州茅台酒"); // 修改股票信息
	//Binary_Search_stock(stocks, stock_count); // 二分查找股票信息 第4关
	//BST_Search_stock(stocks, stock_count); // 二叉查找树查找股票信息 第5关
    //Trie_Search_stock(stocks, stock_count); //字典树查找 第6关
    //Hash_lookup_Search(stocks, stock_count); //基于开放地址的股票信息散列查找 第7关
    //Hash_seperate_Chain_Search(stocks, stock_count); //基于链地址的股票信息散列查找（第8关）
    
    //StockRiseFallAnalysis_List(stocks, stock_count);          //第9关 链表涨跌额分析
    //StockTurnoverAnalysis_UnionFind(stocks, stock_count);     //第10关 并查集换手率分析
    //StockVolumeAnalysis_Heap(stocks, stock_count);            //第11关 堆排序成交量Top3
    //StockVolumeAnalysis_Merge(stocks, stock_count);           //第12关 归并排序成交量统计
    //StockClassify_DecisionTree(stocks, stock_count);          //第13关 决策树分类统计
    //EntityRecognition_BF();                                   //第14关 规则实体识别
    return 0;
}