#ifndef STOCK_H
#define STOCK_H

#define MAX_STOCK 400   //最大股票数量
#define MAX_DETAIL 500   //最大股票行情记录数量
#define MAX_DATA 10  //统计的每支股票的信息的天数

//股票每日行情

typedef struct stockdetail
{

    char date[50];          //交易日期


    char openprice[50];       //开盘价

    char closeprice[50];      //收盘价

    char highest[50];         //最高价

    char lowest[50];          //最低价

    char volume[50];          //成交量

    char turnover[50];        //成交量

    char changerate[50];      //换手率

    char risefallrate[50];    //涨跌幅

    char risefallamount[50];  //涨跌额


}StockDetail;

//股票基本信息

typedef struct stock
{

    char stockcode[20];     //股票代码


    char indusname[50];     //股票简称

    char sname[100];        //英文名


    char briname[50];       //行业编码


    char pricat[50];        //一级分类

    char seccat[100];        //二级分类


    char listexchange[50];  //上市交易所


    char wholecompany[100];//公司全称


    char launchdate[20];    //上市日期


    char provin[50];        //省份

    char city[50];          //城市


    char legalper[50];      //法人


    char addr[200];         //地址


    char url[300];          //网址


    char email[200];        //邮箱


    char calling[50];       //电话


    char mainbus[700];      //主营业务


    char scopeofbus[700];   //经营范围


    //股票每日行情数组

    StockDetail* detail;


	int detail_count;  //股票每日行情记录数量


}Stock;


#endif