#ifndef STOCK_MAINTAIN_H
#define STOCK_MAINTAIN_H


#include "stock.h"

int read_stock_file(char filename[], Stock stocks[]); //读取股票信息文件 

void search_by_pos(Stock stock[], int count);//输出特定位置的股票基本信息

void add_stock_detail(Stock stock[], int stock_count);// 添加股票当日的交易信息

void delete_stock(Stock stocks[], char indusname[], int* stock_count); //删除指定股票信息函数，传入股票数组、股票简称和股票数量指针

void save_stock_file(Stock stock[], int stock_count); //保存股票信息文件

void revise_stock(Stock stock[], int stock_count, char target_name[], char new_name[]);//修改指定股票的简称

#endif
