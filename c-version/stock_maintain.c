#include "stock_maintain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "portable.h"

//分割字符串函数，将一行字符串按"|"分割成多个子字符串，并把每个子字符串存入 data 数组。
//max 为 data 数组容量，防止字段过多时越界写入。同时去掉最后一个字段末尾的换行符。
int Split(char line[], char* data[], int max)
{
    if (max <= 0)
        return 0;

    int i = 0;
    char* p = line;
    data[i++] = p;

    for (char* q = line; *q != '\0'; q++)
    {
        if (*q == '|')
        {
            *q = '\0';
            if (i < max)
                data[i++] = q + 1;
        }
    }

    //去掉最后一个字段末尾的换行符(\n 或 \r\n)
    if (i > 0)
    {
        size_t len = strlen(data[i - 1]);
        while (len > 0 && (data[i - 1][len - 1] == '\n' || data[i - 1][len - 1] == '\r'))
            data[i - 1][--len] = '\0';
    }

    return i;  //返回分割后的子字符串数量
}
void save_stock_file(Stock stock[], int stock_count)  //保存股票信息文件函数，将顺序表中的股票信息写入到文件�?
{

    FILE* fp = NULL;
    errno_t err = fopen_s(
        &fp,
        "new_stock.txt",
        "w"
    );
    if (err != 0 || fp == NULL)
    {
        printf("new_stock.txt打开失败!\n");
        return;
    }

    //遍历顺序表写入数据 (stock[0]为字段名，数据从stock[1]开始)
    for (int i = 1; i <= stock_count; i++)
    {
        fprintf(fp,
            "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",

            stock[i].stockcode,
            stock[i].indusname,
            stock[i].sname,
            stock[i].briname,
            stock[i].pricat,
            stock[i].seccat,
            stock[i].listexchange,
            stock[i].wholecompany,
            stock[i].launchdate,
            stock[i].provin,
            stock[i].city,
            stock[i].legalper,
            stock[i].addr,
            stock[i].url,
            stock[i].email,
            stock[i].calling,
            stock[i].mainbus,
            stock[i].scopeofbus
        );

    }
    fclose(fp);
    printf("股票信息已经保存到new_stock.txt\n");

}
int read_stock_file(char filename[], Stock stocks[]) //读取股票信息文件函数，将文件中的每一行数据分割成多个字段，并存储在stocks数组中
{

	FILE* fp = fopen(filename, "r");  //打开文件
        if (fp == NULL)
        {
            printf("打开文件失败!\n");
            return 0;
        }
		char line[4000];   //定义一个字符数组，用于存储每一行数
		int count = 0;  // 股票计数器，记录读取到的股票数量
        while (fgets(line, sizeof(line), fp) != NULL)
        {
         
            // 跳过空行
            if (strlen(line) == 0)
                continue;
            
            char* data[20] = {0}; //股票信息字段数组，用于存储分割后的每个字
			int fields = Split(line, data, 20);  //分割字符

            //表头行: 将字段名存入 stocks[0]
            if (fields == 18 && strcmp(data[0], "股票代码") == 0)
            {
                strcpy_s(stocks[0].stockcode, sizeof(stocks[0].stockcode), data[0]);
                strcpy_s(stocks[0].indusname, sizeof(stocks[0].indusname), data[1]);
                strcpy_s(stocks[0].sname, sizeof(stocks[0].sname), data[2]);
                strcpy_s(stocks[0].briname, sizeof(stocks[0].briname), data[3]);
                strcpy_s(stocks[0].pricat, sizeof(stocks[0].pricat), data[4]);
                strcpy_s(stocks[0].seccat, sizeof(stocks[0].seccat), data[5]);
                strcpy_s(stocks[0].listexchange, sizeof(stocks[0].listexchange), data[6]);
                strcpy_s(stocks[0].wholecompany, sizeof(stocks[0].wholecompany), data[7]);
                strcpy_s(stocks[0].launchdate, sizeof(stocks[0].launchdate), data[8]);
                strcpy_s(stocks[0].provin, sizeof(stocks[0].provin), data[9]);
                strcpy_s(stocks[0].city, sizeof(stocks[0].city), data[10]);
                strcpy_s(stocks[0].legalper, sizeof(stocks[0].legalper), data[11]);
                strcpy_s(stocks[0].addr, sizeof(stocks[0].addr), data[12]);
                strcpy_s(stocks[0].url, sizeof(stocks[0].url), data[13]);
                strcpy_s(stocks[0].email, sizeof(stocks[0].email), data[14]);
                strcpy_s(stocks[0].calling, sizeof(stocks[0].calling), data[15]);
                strcpy_s(stocks[0].mainbus, sizeof(stocks[0].mainbus), data[16]);
                strcpy_s(stocks[0].scopeofbus, sizeof(stocks[0].scopeofbus), data[17]);
                continue;
            }
            if (fields != 18)
            {
				printf("数据格式错误，股票信息不完整!\n");
				continue;  //跳过该行，继续读取下一支股票的信息
            }

            count++;  //第一支股票存入 stocks[1]，依次类推

			//使用安全复制函数将分割后的字符串存储到stocks数
         
            strcpy_s(stocks[count].stockcode, sizeof(stocks[count].stockcode), data[0]);
	

            strcpy_s(stocks[count].indusname, sizeof(stocks[count].indusname) , data[1]);


            strcpy_s(stocks[count].sname, sizeof(stocks[count].sname), data[2]);


            strcpy_s(stocks[count].briname, sizeof(stocks[count].briname), data[3]);


            strcpy_s(stocks[count].pricat, sizeof(stocks[count].pricat), data[4]);


            strcpy_s(stocks[count].seccat, sizeof(stocks[count].seccat), data[5]);


            strcpy_s(stocks[count].listexchange, sizeof(stocks[count].listexchange), data[6]);


            strcpy_s(stocks[count].wholecompany, sizeof(stocks[count].wholecompany), data[7]);


            strcpy_s(stocks[count].launchdate, sizeof(stocks[count].launchdate), data[8]);


            strcpy_s(stocks[count].provin, sizeof(stocks[count].provin), data[9]);


            strcpy_s(stocks[count].city, sizeof(stocks[count].city), data[10]);


            strcpy_s(stocks[count].legalper, sizeof(stocks[count].legalper), data[11]);


            strcpy_s(stocks[count].addr, sizeof(stocks[count].addr), data[12]);


            strcpy_s(stocks[count].url, sizeof(stocks[count].url), data[13]);


            strcpy_s(stocks[count].email, sizeof(stocks[count].email), data[14]);


            strcpy_s(stocks[count].calling, sizeof(stocks[count].calling), data[15]);


            strcpy_s(stocks[count].mainbus, sizeof(stocks[count].mainbus), data[16]);


            strcpy_s(stocks[count].scopeofbus, sizeof(stocks[count].scopeofbus), data[17]);

		}
        for (int i = 1; i <= count; i++)
        {
            stocks[i].detail =
                malloc(sizeof(StockDetail) * MAX_DETAIL);
            if (stocks[i].detail == NULL)
            {
                printf("内存申请失败\n");
                fclose(fp);
                return 0;
            }
            stocks[i].detail_count = 0;//初始化detail_count，表示该股票没有详细信息

        }
		fclose(fp);
		return count;   //返回读取到的股票数量
}
void add_stock_detail(Stock stock[], int stock_count)   //读取每支股票的每天交易数
{
    int i = 0;              // 从第一支股票开始(stock[0]为字段名)
	char filename[100];     // 打开的股票交易文件名
    for (i = 1;i <= stock_count;i++)
    {
           //根据股票代码生成detail文件路径
           sprintf_s(
                filename,
                sizeof(filename),
                "detail/%s.txt",
                stock[i].stockcode
            );


           FILE* fp = fopen(filename, "r");

           if (fp == NULL)
           {
                printf("%s文件打开失败!\n",
                    stock[i].stockcode);
                continue;
            }
           else
           {
               char line[2000];  //用于存储每一行数据
               while (fgets(line, sizeof(line), fp) != NULL)
               {
                  

                   char* data[20] = {0};

                   int fields = Split(line, data, 20);

                   //跳过表头行(第一行以"日期"开头)
                   if (fields == 10 && strcmp(data[0], "日期") == 0)
                       continue;

                   if (fields == 10)
                   {
                       //防止超过已分配的行情记录容量
                       if (stock[i].detail_count >= MAX_DETAIL)
                       {
                           printf("%s超过最大行情记录数\n", stock[i].stockcode);
                           break;
                       }

                       int index =
                           stock[i].detail_count;

                       strcpy_s(
                           stock[i].detail[index].date,
                           sizeof(stock[i].detail[index].date),
                           data[0]
                       );

                       strcpy_s(
                           stock[i].detail[index].openprice,
                           sizeof(stock[i].detail[index].openprice),
                           data[1]
                       );

                       strcpy_s(
                           stock[i].detail[index].closeprice,
                           sizeof(stock[i].detail[index].closeprice),
                           data[2]
                       );

                       strcpy_s(
                           stock[i].detail[index].highest,
                           sizeof(stock[i].detail[index].highest),
                           data[3]
                       );

                       strcpy_s(
                           stock[i].detail[index].lowest,
                           sizeof(stock[i].detail[index].lowest),
                           data[4]
                       );

                       strcpy_s(
                           stock[i].detail[index].volume,
                           sizeof(stock[i].detail[index].volume),
                           data[5]
                       );

                       strcpy_s(
                           stock[i].detail[index].turnover,
                           sizeof(stock[i].detail[index].turnover),
                           data[6]
                       );


                       strcpy_s(
                           stock[i].detail[index].changerate,
                           sizeof(stock[i].detail[index].changerate),
                           data[7]
                       );


                       strcpy_s(
                           stock[i].detail[index].risefallrate,
                           sizeof(stock[i].detail[index].risefallrate),
                           data[8]
                       );


                       strcpy_s(
                           stock[i].detail[index].risefallamount,
                           sizeof(stock[i].detail[index].risefallamount),
                           data[9]
                       );
                       //成功保存一天数
                       stock[i].detail_count++;
                   }

                   else
                   {
                       printf(
                           "%s存在错误数据\n",
                           stock[i].stockcode
                       );
                   }

               }
               fclose(fp);
           }
    }
}
void search_by_pos(Stock stock[], int count) //输出特定位置的股票基本信息
{
    int pos;
    while (1)
    {
        printf("请输入要查询股票的位置(1-%d):", count);
        scanf_s("%d", &pos);
        //判断输入是否合法
        if (pos >= 1 && pos <= count)
        {
            break;      
        }
        printf("位置不存在,请重新输入?\n");

    }
    printf("\n======股票详细信息 (GUI-v2) ======\n");
    printf("股票代码:%s\n",
        stock[pos].stockcode);


    printf("股票简称:%s\n",
        stock[pos].indusname);


    printf("英文名称:%s\n",
        stock[pos].sname);


    printf("行业编码:%s\n",
        stock[pos].briname);


    printf("一级分类:%s\n",
        stock[pos].pricat);


    printf("二级分类:%s\n",
        stock[pos].seccat);


    printf("上市交易所:%s\n",
        stock[pos].listexchange);



    printf("公司全称:%s\n",
        stock[pos].wholecompany);



    printf("上市日期:%s\n",
        stock[pos].launchdate);



    printf("省份:%s\n",
        stock[pos].provin);



    printf("城市:%s\n",
        stock[pos].city);



    printf("法人:%s\n",
        stock[pos].legalper);



    printf("地址:%s\n",
        stock[pos].addr);



    printf("网址:%s\n",
        stock[pos].url);



    printf("邮箱:%s\n",
        stock[pos].email);



    printf("电话:%s\n",
        stock[pos].calling);



    printf("主营业务:%s\n",
        stock[pos].mainbus);



    printf("经营范围:%s\n",
        stock[pos].scopeofbus);

}

void delete_stock(Stock stocks[], char indusname[], int* stock_count) //删除指定股票信息函数，传入股票数组、股票简称和股票数量指针
{
	
	int pos = -1;   // 用于记录要删除的股票在数组中的位置，初始化为-1
	for (int i = 1;i <= *stock_count;i++)
	{
		if (strcmp(stocks[i].indusname, indusname) == 0)
		{
			pos = i;
			break;
		}
	}
	
    if (pos == -1)
	{
		printf("未找到股票代码为%s的股票\n", indusname);
		return;
	}
	free(stocks[pos].detail);   // 释放被删除股票的行情内存
	stocks[pos].detail = NULL;

	Stock temp = stocks[pos]; // 保存要删除的股票信息

    for (int i = pos;i < *stock_count;i++)
    {
        stocks[i] = stocks[i + 1];
    }

    // 被移出数组的最后位置置空，避免悬空指针
    stocks[*stock_count].detail = NULL;
    stocks[*stock_count].detail_count = 0;

	*stock_count -= 1;
	printf("股票%s已删除\n", indusname);
    
	save_stock_file(stocks, *stock_count);
    //输出被删除股票的信息

    printf("\n======删除股票信息======\n");

    printf("股票代码:%s\n", temp.stockcode);

    printf("行业:%s\n", temp.indusname);

    printf("股票简称:%s\n", temp.sname);

    printf("公司简称:%s\n", temp.briname);

    printf("上市日期:%s\n", temp.launchdate);

    printf("省份:%s\n", temp.provin);

    printf("城市:%s\n", temp.city);

    printf("法人:%s\n", temp.legalper);

    printf("主营业务:%s\n", temp.mainbus);
}

void revise_stock(Stock stock[], int stock_count, char target_name[], char new_name[]) //修改指定股票的简称
{
    int pos = -1;   // 用于记录要删除的股票在数组中的位置，初始化为-1
    for (int i = 1; i <= stock_count; i++)
    {
        if (strcmp(stock[i].indusname, target_name) == 0)
        {
            pos = i;
            break;
        }
    }

    if (pos == -1)
    {
        printf("未找到股票代码为%s的股票\n", target_name);
        return;
    }
	strcpy_s(stock[pos].indusname, sizeof(stock[pos].indusname), new_name);
	printf("股票%s已修改为%s\n", target_name, new_name);
	save_stock_file(stock, stock_count);
}