#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "portable.h"
#include "stock_statistical_analysis.h"
#include "stock_maintain.h"   //add_stock_detail: 读取详细交易数据

/* ===================== 公共辅助函数 ===================== */

//读取一行输入并去掉末尾换行符
static void read_line(char* buf, int size)
{
	if (fgets(buf, size, stdin) == NULL)
	{
		buf[0] = '\0';
		return;
	}
	int len = (int)strlen(buf);
	while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r'))
	{
		buf[--len] = '\0';
	}
}

//在股票的每日行情中查找指定日期, 返回该日行情, 找不到返回NULL
static StockDetail* find_detail(Stock* stock, const char* date)
{
	for (int i = 0; i < stock->detail_count; i++)
	{
		if (strcmp(stock->detail[i].date, date) == 0)
		{
			return &stock->detail[i];
		}
	}
	return NULL;
}

//取"最近一天"(最后一条)行情记录, 无则返回NULL
static StockDetail* last_detail(Stock* stock)
{
	if (stock->detail_count <= 0)
	{
		return NULL;
	}
	return &stock->detail[stock->detail_count - 1];
}

//打印股票基本信息
static void print_stock_info(Stock* stock)
{
	printf("\n============================\n");
	printf("股票代码:%s\n", stock->stockcode);
	printf("股票简称:%s\n", stock->indusname);
	printf("英文名称:%s\n", stock->sname);
	printf("行业编码:%s\n", stock->briname);
	printf("一级分类:%s\n", stock->pricat);
	printf("二级分类:%s\n", stock->seccat);
	printf("上市交易所:%s\n", stock->listexchange);
	printf("公司全称:%s\n", stock->wholecompany);
	printf("上市日期:%s\n", stock->launchdate);
	printf("省份:%s\n", stock->provin);
	printf("城市:%s\n", stock->city);
	printf("法人:%s\n", stock->legalper);
	printf("地址:%s\n", stock->addr);
	printf("网址:%s\n", stock->url);
	printf("邮箱:%s\n", stock->email);
	printf("电话:%s\n", stock->calling);
	printf("主营业务:%s\n", stock->mainbus);
	printf("经营范围:%s\n", stock->scopeofbus);
	printf("============================\n");
}

//打印某日行情记录 (带单位)
static void print_detail(StockDetail* d)
{
	// 成交额原始为"元", 显示时转换为"亿元"
	double turnover_yi = atof(d->turnover) / 100000000.0;
	printf("日期:%s 开盘:%s 收盘:%s 最高:%s 最低:%s 成交量:%s 成交额:%.2f亿元 换手率:%s 涨跌幅:%s 涨跌额:%s\n",
		d->date, d->openprice, d->closeprice, d->highest, d->lowest,
		d->volume, turnover_yi, d->changerate, d->risefallrate, d->risefallamount);
}

/* ===================== 第9关 基于链表的股票涨跌额分析 ===================== */

typedef struct RANode
{
	Stock* stock;          //指向股票
	struct RANode* next;
} RANode;

//链表尾部追加
static void ra_append(RANode** head, RANode** tail, Stock* stock)
{
	RANode* node = (RANode*)malloc(sizeof(RANode));
	if (node == NULL)
	{
		printf("内存分配失败\n");
		exit(1);
	}
	node->stock = stock;
	node->next = NULL;
	if (*head == NULL)
	{
		*head = *tail = node;
	}
	else
	{
		(*tail)->next = node;
		*tail = node;
	}
}

static void ra_free(RANode* head)
{
	while (head != NULL)
	{
		RANode* tmp = head;
		head = head->next;
		free(tmp);
	}
}

void StockRiseFallAnalysis_List(Stock stock[], int stock_count)
{
	add_stock_detail(stock, stock_count);

	char industry[100], date[50];
	printf("请输入行业编码:");
	read_line(industry, sizeof(industry));
	printf("请输入日期(如2026-07-22):");
	read_line(date, sizeof(date));

	//三个分区链表: 涨跌额<0, =0, >0
	RANode *neg = NULL, *zero = NULL, *pos = NULL;
	RANode *negT = NULL, *zeroT = NULL, *posT = NULL;

	for (int i = 1; i <= stock_count; i++)
	{
		if (strcmp(stock[i].briname, industry) != 0)
		{
			continue;
		}
		StockDetail* d = find_detail(&stock[i], date);
		if (d == NULL)
		{
			continue;
		}
		double val = atof(d->risefallamount);
		if (val < 0)
		{
			ra_append(&neg, &negT, &stock[i]);
		}
		else if (val > 0)
		{
			ra_append(&pos, &posT, &stock[i]);
		}
		else
		{
			ra_append(&zero, &zeroT, &stock[i]);
		}
	}

	//拼接: 负 -> 零 -> 正
	RANode* result = NULL;
	RANode* resultT = NULL;
	if (neg != NULL) { result = neg; resultT = negT; }
	if (zero != NULL)
	{
		if (result == NULL) { result = zero; resultT = zeroT; }
		else { resultT->next = zero; resultT = zeroT; }
	}
	if (pos != NULL)
	{
		if (result == NULL) { result = pos; resultT = posT; }
		else { resultT->next = pos; resultT = posT; }
	}

	printf("\n行业编码[%s] 在日期[%s] 的股票涨跌额排序信息(涨跌额<0在前,=0居中,>0在后):\n", industry, date);
	int k = 0;
	RANode* p = result;
	while (p != NULL)
	{
		k++;
		StockDetail* d = find_detail(p->stock, date);
		printf("\n第%d支 涨跌额:%s\n", k, d != NULL ? d->risefallamount : "无");
		print_stock_info(p->stock);
		if (d != NULL)
		{
			print_detail(d);
		}
		p = p->next;
	}
	if (k == 0)
	{
		printf("没有找到行业编码[%s]在日期[%s]的股票数据\n", industry, date);
	}
	ra_free(result);
}

/* ===================== 第10关 基于并查集的股票行业换手率分析 ===================== */

static int uf_parent[MAX_STOCK + 1];

//带路径压缩的查找
static int uf_find(int x)
{
	while (uf_parent[x] != x)
	{
		uf_parent[x] = uf_parent[uf_parent[x]];
		x = uf_parent[x];
	}
	return x;
}

static void uf_union(int a, int b)
{
	int ra = uf_find(a);
	int rb = uf_find(b);
	if (ra != rb)
	{
		uf_parent[ra] = rb;
	}
}

void StockTurnoverAnalysis_UnionFind(Stock stock[], int stock_count)
{
	add_stock_detail(stock, stock_count);

	//初始化并查集: 每支股票自成一个集合
	for (int i = 1; i <= stock_count; i++)
	{
		uf_parent[i] = i;
	}
	//按行业编码合并: 相同行业编码的股票归入同一集合
	for (int i = 1; i <= stock_count; i++)
	{
		for (int j = i + 1; j <= stock_count; j++)
		{
			if (strcmp(stock[i].briname, stock[j].briname) == 0)
			{
				uf_union(i, j);
			}
		}
	}

	char industry[100], date[50];
	printf("请输入行业编码:");
	read_line(industry, sizeof(industry));
	printf("请输入日期(如2026-07-22):");
	read_line(date, sizeof(date));

	//找到该行业编码对应子集的代表元
	int root = -1;
	for (int i = 1; i <= stock_count; i++)
	{
		if (strcmp(stock[i].briname, industry) == 0)
		{
			root = uf_find(i);
			break;
		}
	}
	if (root == -1)
	{
		printf("未找到行业编码[%s]的股票\n", industry);
		return;
	}

	//汇聚该行业所有股票在指定日期的换手率并求和
	double total = 0.0;
	int cnt = 0;
	for (int i = 1; i <= stock_count; i++)
	{
		if (uf_find(i) != root)
		{
			continue;
		}
		StockDetail* d = find_detail(&stock[i], date);
		if (d == NULL)
		{
			continue;
		}
		total += atof(d->changerate);
		cnt++;
	}
	printf("行业编码[%s] 在日期[%s] 的换手率之和 = %f%% (共%d支股票)\n",
		industry, date, total, cnt);
}

/* ===================== 第11关 基于堆排序的股票成交量统计(Top3) ===================== */

typedef struct VolItem
{
	Stock* stock;      //指向股票
	double volume;     //当日成交量
} VolItem;

static void swap_vol(VolItem* a, VolItem* b)
{
	VolItem t = *a;
	*a = *b;
	*b = t;
}

//大根堆下沉调整
static void sift_down_vol(VolItem* a, int n, int i)
{
	int largest = i;
	int l = 2 * i + 1;
	int r = 2 * i + 2;
	if (l < n && a[l].volume > a[largest].volume)
	{
		largest = l;
	}
	if (r < n && a[r].volume > a[largest].volume)
	{
		largest = r;
	}
	if (largest != i)
	{
		swap_vol(&a[i], &a[largest]);
		sift_down_vol(a, n, largest);
	}
}

//堆排序(结果升序, 数组末尾元素最大)
static void heap_sort_vol(VolItem* a, int n)
{
	for (int i = n / 2 - 1; i >= 0; i--)
	{
		sift_down_vol(a, n, i);
	}
	for (int i = n - 1; i > 0; i--)
	{
		swap_vol(&a[0], &a[i]);
		sift_down_vol(a, i, 0);
	}
}

void StockVolumeAnalysis_Heap(Stock stock[], int stock_count)
{
	add_stock_detail(stock, stock_count);

	char date[50], industry[100];
	printf("请输入日期(如2026-07-22):");
	read_line(date, sizeof(date));
	printf("请输入行业编码:");
	read_line(industry, sizeof(industry));

	//收集该行业在指定日期有成交量的股票
	VolItem items[MAX_STOCK];
	int n = 0;
	for (int i = 1; i <= stock_count; i++)
	{
		if (strcmp(stock[i].briname, industry) != 0)
		{
			continue;
		}
		StockDetail* d = find_detail(&stock[i], date);
		if (d == NULL)
		{
			continue;
		}
		items[n].stock = &stock[i];
		items[n].volume = atof(d->volume);
		n++;
	}

	if (n == 0)
	{
		printf("行业编码[%s]在日期[%s]没有成交量数据\n", industry, date);
		return;
	}

	heap_sort_vol(items, n);

	printf("\n行业编码[%s] 在日期[%s] 成交量Top3(按成交量从大到小):\n", industry, date);
	int top = n < 3 ? n : 3;
	for (int k = 0; k < top; k++)
	{
		VolItem* it = &items[n - 1 - k];   //从大到小依次取末尾元素
		StockDetail* d = find_detail(it->stock, date);
		printf("\n第%d名 成交量:%s\n", k + 1, d != NULL ? d->volume : "无");
		print_stock_info(it->stock);
		if (d != NULL)
		{
			print_detail(d);
		}
	}
}

/* ===================== 第12关 基于归并排序的股票成交量统计(按日期汇总) ===================== */

typedef struct DVItem
{
	char date[50];
	double total_volume;   //该日期成交量和
} DVItem;

//归并排序(按total_volume从小到大)
static void merge_sort_dv(DVItem* a, int left, int right, DVItem* temp)
{
	if (left >= right)
	{
		return;
	}
	int mid = left + (right - left) / 2;
	merge_sort_dv(a, left, mid, temp);
	merge_sort_dv(a, mid + 1, right, temp);

	int i = left, j = mid + 1, k = left;
	while (i <= mid && j <= right)
	{
		if (a[i].total_volume <= a[j].total_volume)
		{
			temp[k++] = a[i++];
		}
		else
		{
			temp[k++] = a[j++];
		}
	}
	while (i <= mid)
	{
		temp[k++] = a[i++];
	}
	while (j <= right)
	{
		temp[k++] = a[j++];
	}
	for (i = left; i <= right; i++)
	{
		a[i] = temp[i];
	}
}

void StockVolumeAnalysis_Merge(Stock stock[], int stock_count)
{
	add_stock_detail(stock, stock_count);

	char industry[100];
	printf("请输入行业编码:");
	read_line(industry, sizeof(industry));

	//按日期累加该行业所有股票的成交量, 存入新顺序表
	int capacity = stock_count * MAX_DETAIL + 1;
	DVItem* table = (DVItem*)malloc(sizeof(DVItem) * capacity);
	DVItem* temp = (DVItem*)malloc(sizeof(DVItem) * capacity);
	if (table == NULL || temp == NULL)
	{
		printf("内存分配失败\n");
		exit(1);
	}
	int cnt = 0;
	for (int i = 1; i <= stock_count; i++)
	{
		if (strcmp(stock[i].briname, industry) != 0)
		{
			continue;
		}
		for (int j = 0; j < stock[i].detail_count; j++)
		{
			StockDetail* d = &stock[i].detail[j];
			int found = -1;
			for (int k = 0; k < cnt; k++)
			{
				if (strcmp(table[k].date, d->date) == 0)
				{
					found = k;
					break;
				}
			}
			if (found == -1)
			{
				strcpy_s(table[cnt].date, sizeof(table[cnt].date), d->date);
				table[cnt].total_volume = atof(d->volume);
				cnt++;
			}
			else
			{
				table[found].total_volume += atof(d->volume);
			}
		}
	}

	if (cnt == 0)
	{
		printf("行业编码[%s]没有成交量数据\n", industry);
		free(table);
		free(temp);
		return;
	}

	//按成交量从小到大归并排序
	merge_sort_dv(table, 0, cnt - 1, temp);

	printf("\n行业编码[%s] 按日期成交量汇总统计(按成交量从小到大):\n", industry);
	for (int i = 0; i < cnt; i++)
	{
		printf("日期:%s 成交量合计:%.0f股\n", table[i].date, table[i].total_volume);
	}
	free(table);
	free(temp);
}

/* ===================== 第13关 基于决策树的股票分类统计 ===================== */

typedef struct DTNode
{
	char condition[200];        //决策条件描述
	int level;                  //决策层次(0/1/2)
	struct DTNode* left;        //满足条件的分支
	struct DTNode* right;       //不满足条件的分支
	int count;                  //该结点包含的股票数
	int indices[MAX_STOCK];     //该结点包含的股票下标
} DTNode;

//全局决策参数(供建树与查询使用)
static char g_seccat_cond[200];
static double g_rf_thr = 0.0;
static double g_vol_thr = 0.0;

static DTNode* create_dt_node(const char* condition, int level, int* indices, int count)
{
	DTNode* node = (DTNode*)malloc(sizeof(DTNode));
	if (node == NULL)
	{
		printf("内存分配失败\n");
		exit(1);
	}
	strcpy_s(node->condition, sizeof(node->condition), condition);
	node->level = level;
	node->left = NULL;
	node->right = NULL;
	node->count = count;
	for (int i = 0; i < count; i++)
	{
		node->indices[i] = indices[i];
	}
	return node;
}

//解析条件字符串, 如 ">0" -> 0, ">1000万股" -> 1000*10000
static double parse_condition(const char* s)
{
	char num[100];
	int k = 0;
	int has_dot = 0;
	for (int i = 0; s[i] != '\0'; i++)
	{
		char c = s[i];
		if ((c >= '0' && c <= '9') || c == '.')
		{
			if (c == '.')
			{
				if (has_dot) continue;
				has_dot = 1;
			}
			num[k++] = c;
		}
	}
	num[k] = '\0';
	double v = atof(num);
	if (strstr(s, "万") != NULL) v *= 10000;       //"万" = 1e4
	if (strstr(s, "亿") != NULL) v *= 100000000;   //"亿" = 1e8
	return v;
}

//判断股票在某个决策层次下是否满足条件
static int dt_satisfy(Stock* stock, int level)
{
	if (level == 0)
	{
		return strcmp(stock->seccat, g_seccat_cond) == 0;
	}
	StockDetail* d = last_detail(stock);
	if (d == NULL)
	{
		return 0;   //无行情数据视为不满足
	}
	if (level == 1)
	{
		return atof(d->risefallrate) > g_rf_thr;
	}
	return atof(d->volume) > g_vol_thr;
}

//递归构建决策树(二叉树存储: left=满足, right=不满足)
static DTNode* build_dt(Stock* stock, int* indices, int count, int level)
{
	char condition[200];
	if (level == 0)
	{
		sprintf_s(condition, sizeof(condition), "二级门类是否为[%s]", g_seccat_cond);
	}
	else if (level == 1)
	{
		sprintf_s(condition, sizeof(condition), "最近一天涨跌幅是否>%f", g_rf_thr);
	}
	else
	{
		sprintf_s(condition, sizeof(condition), "最近一天成交量是否>%f(股)", g_vol_thr);
	}
	DTNode* node = create_dt_node(condition, level, indices, count);
	if (level >= 3)
	{
		return node;   //叶子结点
	}

	int leftIdx[MAX_STOCK], rightIdx[MAX_STOCK];
	int lc = 0, rc = 0;
	for (int i = 0; i < count; i++)
	{
		if (dt_satisfy(&stock[indices[i]], level))
		{
			leftIdx[lc++] = indices[i];
		}
		else
		{
			rightIdx[rc++] = indices[i];
		}
	}
	node->left = build_dt(stock, leftIdx, lc, level + 1);
	node->right = build_dt(stock, rightIdx, rc, level + 1);
	return node;
}

static void free_dt(DTNode* node)
{
	if (node == NULL)
	{
		return;
	}
	free_dt(node->left);
	free_dt(node->right);
	free(node);
}

void StockClassify_DecisionTree(Stock stock[], int stock_count)
{
	add_stock_detail(stock, stock_count);

	//1.输入三个决策条件
	char seccat_buf[200], rf_buf[100], vol_buf[100];
	printf("请输入二级门类(如:计算机、通信和其他电子设备制造业):");
	read_line(seccat_buf, sizeof(seccat_buf));
	printf("请输入最近一天涨跌幅条件(如>0):");
	read_line(rf_buf, sizeof(rf_buf));
	printf("请输入最近一天成交量条件(如>1000万股):");
	read_line(vol_buf, sizeof(vol_buf));

	strcpy_s(g_seccat_cond, sizeof(g_seccat_cond), seccat_buf);
	g_rf_thr = parse_condition(rf_buf);
	g_vol_thr = parse_condition(vol_buf);

	//2.初始森林为所有股票, 根结点包含全部股票, 依次按决策条件分裂构建决策树
	int all[MAX_STOCK];
	for (int i = 0; i < stock_count; i++)
	{
		all[i] = i + 1;
	}
	DTNode* root = build_dt(stock, all, stock_count, 0);
	printf("决策树构建完成!\n");

	//3.输入股票简称查询
	char sname[100];
	printf("请输入股票简称:");
	read_line(sname, sizeof(sname));
	int idx = -1;
	for (int i = 1; i <= stock_count; i++)
	{
		if (strcmp(stock[i].indusname, sname) == 0)
		{
			idx = i;
			break;
		}
	}
	if (idx == -1)
	{
		printf("未找到股票简称[%s]\n", sname);
		free_dt(root);
		return;
	}

	//4.从根沿决策路径下探, 记录每个决策结点
	DTNode* path[10];
	int depth = 0;
	DTNode* p = root;
	while (p != NULL && p->left != NULL && p->right != NULL)
	{
		path[depth++] = p;
		if (dt_satisfy(&stock[idx], p->level))
		{
			p = p->left;
		}
		else
		{
			p = p->right;
		}
	}

	//5.从决策树的底部开始, 依次输出该股票的各决策条件
	printf("\n股票[%s] 从决策树底部开始的决策条件:\n", sname);
	for (int i = depth - 1; i >= 0; i--)
	{
		int sat = dt_satisfy(&stock[idx], path[i]->level);
		printf("决策条件: %s  (该股票%s)\n", path[i]->condition, sat ? "满足" : "不满足");
	}
	free_dt(root);
}
