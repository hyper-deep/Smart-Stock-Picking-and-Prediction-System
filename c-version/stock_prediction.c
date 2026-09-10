#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "portable.h"
#include "stock_prediction.h"

/* ---------- 工具函数 ---------- */

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
		buf[--len] = '\0';
}

//按'|'拆分一行, 返回字段个数
static int split_line(char* line, char* data[], int max)
{
	if (max <= 0)
		return 0;
	int i = 0;
	data[i++] = line;
	for (char* q = line; *q != '\0'; q++)
	{
		if (*q == '|')
		{
			*q = '\0';
			if (i < max)
				data[i++] = q + 1;
		}
	}
	//去掉最后一个字段末尾的换行符
	if (i > 0)
	{
		size_t len = strlen(data[i - 1]);
		while (len > 0 && (data[i - 1][len - 1] == '\n' || data[i - 1][len - 1] == '\r'))
			data[i - 1][--len] = '\0';
	}
	return i;
}

//去掉UTF-8 BOM(文件可能以 EF BB BF 开头)
static void strip_bom(char* s)
{
	unsigned char* p = (unsigned char*)s;
	if (p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF)
		memmove(s, s + 3, strlen(s) - 2);
}

/* ---------- 第18关 基于皮尔逊相关系数的股票指标相关性分析 ---------- */

#define MAX_ROWS 500       //日交易记录最大行数
#define INDICATOR_COUNT 5  //指标个数

//指标项: 名称 + 相关系数绝对值
typedef struct
{
	const char* name;
	double value;
} Indicator;

//计算皮尔逊相关系数 r = [n*Σxy - Σx*Σy] / sqrt([n*Σx²-(Σx)²]*[n*Σy²-(Σy)²])
//分母为0(样本不足或指标恒为常数)时返回0
static double pearson(const double x[], const double y[], int n)
{
	if (n <= 0)
		return 0.0;
	double sx = 0, sy = 0, sxy = 0, sxx = 0, syy = 0;
	for (int i = 0; i < n; i++)
	{
		sx += x[i];
		sy += y[i];
		sxy += x[i] * y[i];
		sxx += x[i] * x[i];
		syy += y[i] * y[i];
	}
	double numerator = n * sxy - sx * sy;
	double denom = (n * sxx - sx * sx) * (n * syy - sy * sy);
	if (denom <= 0.0)
		return 0.0;
	return numerator / sqrt(denom);
}

//第18关: 读取指定股票详细日交易数据, 计算5个指标与次日涨跌的皮尔逊相关系数,
//        取绝对值后降序排序, 输出相关性最强的前3个指标
void Pearson_Correlation_Analysis()
{
	char input[300];
	char filepath[300];
	double openp[MAX_ROWS], highp[MAX_ROWS], lowp[MAX_ROWS];
	double volume[MAX_ROWS];    //成交量(股)
	double turnover[MAX_ROWS];  //成交额(元)
	double changerate[MAX_ROWS];
	double risefallrate[MAX_ROWS];
	int rows = 0;

	printf("请输入股票详细数据文件路径或股票代码(如 detail/cn_600519.txt):");
	read_line(input, sizeof(input));
	if (input[0] == '\0')
	{
		printf("输入不能为空!\n");
		return;
	}

	//支持直接输入文件路径, 或仅输入股票代码(自动补全 detail/ 前缀与 .txt 后缀)
	if (strchr(input, '/') != NULL || strchr(input, '\\') != NULL || strstr(input, ".txt") != NULL)
		strcpy_s(filepath, sizeof(filepath), input);
	else
		sprintf_s(filepath, sizeof(filepath), "detail/%s.txt", input);

	FILE* fp = fopen(filepath, "r");
	if (fp == NULL)
	{
		printf("无法打开文件: %s\n", filepath);
		return;
	}

	char line[1024];
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		strip_bom(line);
		char* data[12] = { 0 };
		int fields = split_line(line, data, 12);
		if (fields < 10)
			continue;
		if (strcmp(data[0], "日期") == 0)   //跳过表头行
			continue;
		if (rows >= MAX_ROWS)
			break;
		openp[rows] = atof(data[1]);
		highp[rows] = atof(data[3]);
		lowp[rows] = atof(data[4]);
		volume[rows] = atof(data[5]);       //成交量(股)
		turnover[rows] = atof(data[6]);     //成交额(元)
		changerate[rows] = atof(data[7]);   //换手率
		risefallrate[rows] = atof(data[8]); //涨跌幅
		rows++;
	}
	fclose(fp);

	if (rows < 2)
	{
		printf("有效交易数据不足(至少需要2行), 无法进行相关性分析!\n");
		return;
	}

	//有效样本量 n = 总行数 - 1 (最后一天无次日数据)
	int n = rows - 1;

	//第t日的5个指标与第t+1日的涨跌标签对齐
	double x[INDICATOR_COUNT][MAX_ROWS];  //5个指标的有效样本序列
	double yy[MAX_ROWS];                  //次日涨跌标签(涨为1, 跌为0)
	const char* names[INDICATOR_COUNT] = { "涨跌幅", "成交量", "振幅", "换手率", "成交额" };

	for (int i = 0; i < n; i++)
	{
		x[0][i] = risefallrate[i];                          //涨跌幅(直接读取)
		x[1][i] = volume[i] / 10000.0;                      //成交量: 股 -> 万股
		x[2][i] = (openp[i] > 0) ? (highp[i] - lowp[i]) / openp[i] * 100.0 : 0.0; //振幅
		x[3][i] = changerate[i];                            //换手率(直接读取)
		x[4][i] = turnover[i] / 100000000.0;                //成交额: 元 -> 亿元
		yy[i] = (risefallrate[i + 1] > 0) ? 1.0 : 0.0;      //次日涨跌标签
	}

	//计算各指标与标签的皮尔逊相关系数并取绝对值
	Indicator inds[INDICATOR_COUNT];
	for (int k = 0; k < INDICATOR_COUNT; k++)
	{
		double r = pearson(x[k], yy, n);
		inds[k].name = names[k];
		inds[k].value = (r < 0) ? -r : r;
	}

	//按相关系数绝对值降序排序(选择排序)
	for (int a = 0; a < INDICATOR_COUNT - 1; a++)
	{
		int maxi = a;
		for (int b = a + 1; b < INDICATOR_COUNT; b++)
		{
			if (inds[b].value > inds[maxi].value)
				maxi = b;
		}
		if (maxi != a)
		{
			Indicator tmp = inds[a];
			inds[a] = inds[maxi];
			inds[maxi] = tmp;
		}
	}

	//输出: 降序排序结果
	printf("\n各指标与次日涨跌的皮尔逊相关系数(绝对值)降序:\n");
	for (int k = 0; k < INDICATOR_COUNT; k++)
		printf("%d. %s: %.4f\n", k + 1, inds[k].name, inds[k].value);

	//输出: 相关性最强的前3个指标
	printf("\n相关性最强的前3个指标: ");
	for (int k = 0; k < 3 && k < INDICATOR_COUNT; k++)
	{
		printf("%s", inds[k].name);
		if (k < 2)
			printf("、");
	}
	printf("\n");
}

/* ---------- 第19关 基于朴素贝叶斯分类器的股票涨跌预测(选做) ---------- */

#define NB_FEATURE_COUNT 3   //特征数(第18关筛选: 涨跌幅/振幅/成交额)
#define NB_CLASS_COUNT 2     //类别数(涨/跌)

//贝叶斯网络边: 特征节点与标签节点之间的高斯条件概率参数
typedef struct
{
	int label;      //标签类别(0=跌, 1=涨)
	int feature;    //特征下标(0=涨跌幅, 1=振幅, 2=成交额)
	double mu;      //均值 μ
	double var;     //方差 σ²
} NBEdge;

//贝叶斯网络图结构: 节点(特征/标签) + 边(条件概率参数)
typedef struct
{
	char feature_name[NB_FEATURE_COUNT][30];  //特征节点名称
	char label_name[NB_CLASS_COUNT][10];      //标签节点名称(涨/跌)
	double prior[NB_CLASS_COUNT];             //标签节点先验概率 P(Y)
	NBEdge edges[NB_FEATURE_COUNT][NB_CLASS_COUNT];  //边: 特征→标签 的条件概率参数
} NBGraph;

//高斯分布对数概率密度: log f(x; μ, σ²)
static double gaussian_logpdf(double x, double mu, double var)
{
	if (var <= 0.0)
		var = 1e-9;   //防止方差为0导致除零/log(0)
	double diff = x - mu;
	return -0.5 * log(2.0 * 3.14159265358979323846)
		- 0.5 * log(var)
		- (diff * diff) / (2.0 * var);
}

//第19关: 基于第18关筛选的3个关键指标(涨跌幅/振幅/成交额), 用朴素贝叶斯分类器
//        预测最新交易日的次日涨跌方向(涨/跌)
void NaiveBayes_StockPredict()
{
	char input[300];
	char filepath[300];
	double openp[MAX_ROWS], highp[MAX_ROWS], lowp[MAX_ROWS];
	double turnover[MAX_ROWS];      //成交额(元)
	double risefallrate[MAX_ROWS];  //涨跌幅
	double feat[NB_FEATURE_COUNT][MAX_ROWS];  //3个关键指标(涨跌幅/振幅/成交额)
	int rows = 0;


	read_line(input, sizeof(input));
	if (input[0] == '\0')
	{
		printf("输入不能为空!\n");
		return;
	}

	if (strchr(input, '/') != NULL || strchr(input, '\\') != NULL || strstr(input, ".txt") != NULL)
		strcpy_s(filepath, sizeof(filepath), input);
	else
		sprintf_s(filepath, sizeof(filepath), "detail/%s.txt", input);

	FILE* fp = fopen(filepath, "r");
	if (fp == NULL)
	{
		printf("无法打开文件: %s\n", filepath);
		return;
	}

	char line[1024];
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		strip_bom(line);
		char* data[12] = { 0 };
		int fields = split_line(line, data, 12);
		if (fields < 10)
			continue;
		if (strcmp(data[0], "日期") == 0)   //跳过表头行
			continue;
		if (rows >= MAX_ROWS)
			break;
		openp[rows] = atof(data[1]);
		highp[rows] = atof(data[3]);
		lowp[rows] = atof(data[4]);
		turnover[rows] = atof(data[6]);        //成交额(元)
		risefallrate[rows] = atof(data[8]);    //涨跌幅
		feat[0][rows] = risefallrate[rows];    //涨跌幅
		feat[1][rows] = (openp[rows] > 0) ? (highp[rows] - lowp[rows]) / openp[rows] * 100.0 : 0.0; //振幅
		feat[2][rows] = turnover[rows] / 100000000.0;  //成交额(亿元)
		rows++;
	}
	fclose(fp);

	if (rows < 2)
	{
		printf("有效交易数据不足(至少需要2行), 无法进行预测!\n");
		return;
	}

	//训练集: 除最新一条数据(倒数第1行, 即文件最后一行)外的所有交易日, 样本数 = rows-1
	int train_n = rows - 1;
	//待预测样本: 最新交易日(最后一行)的3个特征
	double pred_feat[NB_FEATURE_COUNT];
	for (int f = 0; f < NB_FEATURE_COUNT; f++)
		pred_feat[f] = feat[f][rows - 1];

	//1. 训练: 计算先验概率 P(Y) 与各类别下的高斯参数(μ, σ²)
	NBGraph net;
	strcpy_s(net.feature_name[0], sizeof(net.feature_name[0]), "涨跌幅");
	strcpy_s(net.feature_name[1], sizeof(net.feature_name[1]), "振幅");
	strcpy_s(net.feature_name[2], sizeof(net.feature_name[2]), "成交额");
	strcpy_s(net.label_name[0], sizeof(net.label_name[0]), "跌");
	strcpy_s(net.label_name[1], sizeof(net.label_name[1]), "涨");

	int cnt[NB_CLASS_COUNT] = { 0, 0 };
	for (int t = 0; t < train_n; t++)
	{
		int y = (risefallrate[t + 1] > 0) ? 1 : 0;   //次日涨跌标签
		cnt[y]++;
	}
	for (int c = 0; c < NB_CLASS_COUNT; c++)
		net.prior[c] = (train_n > 0) ? (double)cnt[c] / train_n : 0.0;

	double mu[NB_FEATURE_COUNT][NB_CLASS_COUNT] = { { 0 } };
	double sum[NB_FEATURE_COUNT][NB_CLASS_COUNT] = { { 0 } };
	for (int t = 0; t < train_n; t++)
	{
		int y = (risefallrate[t + 1] > 0) ? 1 : 0;
		for (int f = 0; f < NB_FEATURE_COUNT; f++)
			sum[f][y] += feat[f][t];
	}
	for (int c = 0; c < NB_CLASS_COUNT; c++)
		for (int f = 0; f < NB_FEATURE_COUNT; f++)
			mu[f][c] = (cnt[c] > 0) ? sum[f][c] / cnt[c] : 0.0;

	double var[NB_FEATURE_COUNT][NB_CLASS_COUNT] = { { 0 } };
	double sq[NB_FEATURE_COUNT][NB_CLASS_COUNT] = { { 0 } };
	for (int t = 0; t < train_n; t++)
	{
		int y = (risefallrate[t + 1] > 0) ? 1 : 0;
		for (int f = 0; f < NB_FEATURE_COUNT; f++)
		{
			double d = feat[f][t] - mu[f][y];
			sq[f][y] += d * d;
		}
	}
	for (int c = 0; c < NB_CLASS_COUNT; c++)
		for (int f = 0; f < NB_FEATURE_COUNT; f++)
			var[f][c] = (cnt[c] > 0) ? sq[f][c] / cnt[c] : 0.0;

	//将高斯参数写入图结构的边
	for (int c = 0; c < NB_CLASS_COUNT; c++)
	{
		for (int f = 0; f < NB_FEATURE_COUNT; f++)
		{
			net.edges[f][c].label = c;
			net.edges[f][c].feature = f;
			net.edges[f][c].mu = mu[f][c];
			net.edges[f][c].var = var[f][c];
		}
	}

	//2. 预测: 计算两类对数后验概率 log P(Y) + Σ log P(Xi|Y)
	double score[NB_CLASS_COUNT] = { 0.0, 0.0 };
	for (int c = 0; c < NB_CLASS_COUNT; c++)
	{
		if (cnt[c] == 0)
		{
			score[c] = -1e300;   //该类无样本, 置为极小值
			continue;
		}
		score[c] = log(net.prior[c]);
		for (int f = 0; f < NB_FEATURE_COUNT; f++)
			score[c] += gaussian_logpdf(pred_feat[f], net.edges[f][c].mu, net.edges[f][c].var);
	}
	int predict = (score[1] >= score[0]) ? 1 : 0;

	//3. 输出训练结果与预测结论
	printf("\n===== 朴素贝叶斯训练结果(图结构) =====");
	printf("\n训练样本数: %d (涨:%d, 跌:%d)\n", train_n, cnt[1], cnt[0]);
	printf("先验概率: P(涨)=%.4f, P(跌)=%.4f\n", net.prior[1], net.prior[0]);
	printf("类条件高斯参数(μ, σ²):\n");
	for (int f = 0; f < NB_FEATURE_COUNT; f++)
	{
		for (int c = 0; c < NB_CLASS_COUNT; c++)
		{
			printf("  %s | %s: μ=%.4f, σ²=%.4f\n",
				net.feature_name[f], net.label_name[c],
				net.edges[f][c].mu, net.edges[f][c].var);
		}
	}
	printf("\n待预测样本(最新交易日): 涨跌幅=%.4f%%, 振幅=%.4f%%, 成交额=%.4f亿元\n",
		pred_feat[0], pred_feat[1], pred_feat[2]);
	printf("涨类对数后验概率: %.4f\n", score[1]);
	printf("跌类对数后验概率: %.4f\n", score[0]);

	printf("\n预测结论: 次日为%s\n", (predict == 1) ? "涨" : "跌");
}

/* ---------- 第20关 基于K近邻算法的股票涨跌预测(选做) ---------- */

#define KNN_K 3              //近邻个数 K=3
#define KNN_FEATURE 3        //特征数(第18关筛选: 涨跌幅/振幅/成交额)

static const char* g_knn_names[KNN_FEATURE] = { "涨跌幅", "振幅", "成交额" };

//有序链表结点: 保存历史样本与目标日的距离及其次日涨跌幅数值
typedef struct KNNNode
{
	double dist;              //与目标日的欧氏距离
	double value;             //该历史样本的次日涨跌幅(具体数值)
	struct KNNNode* next;
} KNNNode;

//按距离升序插入有序链表; 若长度超过K则剔除链表末尾(距离最远)的结点
static void knn_insert(KNNNode** head, double dist, double value, int* cnt)
{
	KNNNode* node = (KNNNode*)malloc(sizeof(KNNNode));
	if (node == NULL)
		return;
	node->dist = dist;
	node->value = value;
	node->next = NULL;

	if (*head == NULL || dist < (*head)->dist)
	{
		node->next = *head;
		*head = node;
	}
	else
	{
		KNNNode* p = *head;
		while (p->next != NULL && p->next->dist <= dist)
			p = p->next;
		node->next = p->next;
		p->next = node;
	}
	(*cnt)++;

	if (*cnt > KNN_K)
	{
		if ((*head)->next == NULL)
		{
			free(*head);
			*head = NULL;
			*cnt = 0;
		}
		else
		{
			KNNNode* q = *head;
			while (q->next->next != NULL)
				q = q->next;
			free(q->next);
			q->next = NULL;
			(*cnt)--;
		}
	}
}

//第20关: 基于3个关键指标(涨跌幅/振幅/成交额), 用K近邻(K=3)回归算法预测
//        最新交易日次日涨跌幅的具体数值
void KNN_StockPredict()
{
	char input[300];
	char filepath[300];
	double openp[MAX_ROWS], highp[MAX_ROWS], lowp[MAX_ROWS];
	double turnover[MAX_ROWS];      //成交额(元)
	double risefallrate[MAX_ROWS];  //涨跌幅
	double feat[KNN_FEATURE][MAX_ROWS];  //3个关键指标(涨跌幅/振幅/成交额)
	int rows = 0;

	read_line(input, sizeof(input));
	if (input[0] == '\0')
	{
		printf("输入不能为空!\n");
		return;
	}

	if (strchr(input, '/') != NULL || strchr(input, '\\') != NULL || strstr(input, ".txt") != NULL)
		strcpy_s(filepath, sizeof(filepath), input);
	else
		sprintf_s(filepath, sizeof(filepath), "detail/%s.txt", input);

	FILE* fp = fopen(filepath, "r");
	if (fp == NULL)
	{
		printf("无法打开文件: %s\n", filepath);
		return;
	}

	char line[1024];
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		strip_bom(line);
		char* data[12] = { 0 };
		int fields = split_line(line, data, 12);
		if (fields < 10)
			continue;
		if (strcmp(data[0], "日期") == 0)   //跳过表头行
			continue;
		if (rows >= MAX_ROWS)
			break;
		openp[rows] = atof(data[1]);
		highp[rows] = atof(data[3]);
		lowp[rows] = atof(data[4]);
		turnover[rows] = atof(data[6]);        //成交额(元)
		risefallrate[rows] = atof(data[8]);    //涨跌幅
		feat[0][rows] = risefallrate[rows];    //涨跌幅
		feat[1][rows] = (openp[rows] > 0) ? (highp[rows] - lowp[rows]) / openp[rows] * 100.0 : 0.0; //振幅
		feat[2][rows] = turnover[rows] / 100000000.0;  //成交额(亿元)
		rows++;
	}
	fclose(fp);

	if (rows < 2)
	{
		printf("有效交易数据不足(至少需要2行), 无法进行预测!\n");
		return;
	}

	//待预测目标: 最新交易日(最后一行)的3个特征
	int target_idx = rows - 1;
	double target[KNN_FEATURE];
	for (int f = 0; f < KNN_FEATURE; f++)
		target[f] = feat[f][target_idx];

	//历史训练样本: 除最新交易日外的其余交易日, 每个样本含3特征 + 次日涨跌幅数值
	int hist_n = rows - 1;   //历史样本数

	//1. Min-Max归一化: 基于历史样本求各特征的最小值/最大值
	double fmin[KNN_FEATURE], fmax[KNN_FEATURE];
	for (int f = 0; f < KNN_FEATURE; f++)
	{
		fmin[f] = feat[f][0];
		fmax[f] = feat[f][0];
		for (int t = 1; t < hist_n; t++)
		{
			if (feat[f][t] < fmin[f]) fmin[f] = feat[f][t];
			if (feat[f][t] > fmax[f]) fmax[f] = feat[f][t];
		}
	}

	//归一化目标向量(使用历史样本的同一组min/max)
	double tnorm[KNN_FEATURE];
	for (int f = 0; f < KNN_FEATURE; f++)
		tnorm[f] = (fmax[f] > fmin[f]) ? (target[f] - fmin[f]) / (fmax[f] - fmin[f]) : 0.0;

	//2-3. 计算各历史样本与目标的欧氏距离, 用有序链表动态维护K个最近邻
	KNNNode* head = NULL;
	int cnt = 0;
	for (int t = 0; t < hist_n; t++)
	{
		double hnorm[KNN_FEATURE];
		for (int f = 0; f < KNN_FEATURE; f++)
			hnorm[f] = (fmax[f] > fmin[f]) ? (feat[f][t] - fmin[f]) / (fmax[f] - fmin[f]) : 0.0;

		double sum = 0.0;
		for (int f = 0; f < KNN_FEATURE; f++)
		{
			double d = tnorm[f] - hnorm[f];
			sum += d * d;
		}
		double dist = sqrt(sum);

		//该历史样本的次日涨跌幅数值
		double next_val = risefallrate[t + 1];
		knn_insert(&head, dist, next_val, &cnt);
	}

	//4. 预测 = K个近邻次日涨跌幅的算术平均
	double total = 0.0;
	int count = 0;
	for (KNNNode* p = head; p != NULL; p = p->next)
	{
		total += p->value;
		count++;
	}
	double prediction = (count > 0) ? total / count : 0.0;

	//输出
	printf("\n===== K近邻(K=%d)回归预测 =====", KNN_K);
	printf("\n历史样本数: %d\n", hist_n);
	printf("目标日(最新交易日)特征: 涨跌幅=%.4f%%, 振幅=%.4f%%, 成交额=%.4f亿元\n",
		target[0], target[1], target[2]);
	printf("Min-Max归一化参数(基于历史样本):\n");
	for (int f = 0; f < KNN_FEATURE; f++)
		printf("  %s: min=%.4f, max=%.4f\n", g_knn_names[f], fmin[f], fmax[f]);

	printf("距离最近的%d个近邻(距离升序):\n", count);
	for (KNNNode* p = head; p != NULL; p = p->next)
		printf("  距离=%.4f, 次日涨跌幅=%.4f%%\n", p->dist, p->value);

	printf("\n预测次日涨跌幅: %.4f%%\n", prediction);

	//释放有序链表
	while (head != NULL)
	{
		KNNNode* nx = head->next;
		free(head);
		head = nx;
	}
}

