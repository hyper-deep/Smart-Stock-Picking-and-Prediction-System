#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "portable.h"
#include "stock_choice.h"

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
	{
		buf[--len] = '\0';
	}
}

//字符信息: 记录一个UTF-8字符在原始串中的字节位置
typedef struct
{
	int start;   //在原始串中的字节起点
	int len;     //字节长度(中文字符3, ASCII1)
	int is_cn;   //是否中文字符
} CharInfo;

//把UTF-8字符串解析成字符数组, 返回字符个数
static int parse_chars(const char* s, CharInfo* info, int max)
{
	int n = 0;
	int i = 0;
	while (s[i] != '\0' && n < max)
	{
		unsigned char c = (unsigned char)s[i];
		if (c >= 0xE4 && c <= 0xE9)   //中文字符(UTF-8编码, 占3字节)
		{
			info[n].start = i;
			info[n].len = 3;
			info[n].is_cn = 1;
			i += 3;
		}
		else if (c >= 0xC0)           //其它多字节字符(如全角符号), 不视为中文
		{
			int blen = 1;
			if (c >= 0xF0) blen = 4;
			else if (c >= 0xE0) blen = 3;
			else if (c >= 0xC0) blen = 2;
			info[n].start = i;
			info[n].len = blen;
			info[n].is_cn = 0;
			i += blen;
		}
		else                          //ASCII字符(英文字母、数字、标点)
		{
			info[n].start = i;
			info[n].len = 1;
			info[n].is_cn = 0;
			i += 1;
		}
		n++;
	}
	return n;
}

//规则项: 通配符* 或 具体字面字符
typedef struct
{
	int is_wild;   //1=通配符*
	int start;     //在规则串中的字节起点
	int len;       //字节长度
} RuleItem;

//解析规则(含一个连续的*通配符段), 返回规则项数; 同时返回*数量、前缀/后缀字面字符数
static int parse_rule(const char* rule, RuleItem* items,
	int* star_count, int* prefix_count, int* suffix_count)
{
	int n = 0, stars = 0, pre = 0, suf = 0;
	int seen_star = 0;
	int i = 0;
	while (rule[i] != '\0' && n < 200)
	{
		if (rule[i] == '*')
		{
			items[n].is_wild = 1;
			items[n].start = i;
			items[n].len = 1;
			n++;
			stars++;
			seen_star = 1;
			i += 1;
		}
		else
		{
			unsigned char c = (unsigned char)rule[i];
			int blen = 1;
			if (c >= 0xF0) blen = 4;
			else if (c >= 0xE0) blen = 3;
			else if (c >= 0xC0) blen = 2;
			items[n].is_wild = 0;
			items[n].start = i;
			items[n].len = blen;
			n++;
			if (!seen_star) pre++; else suf++;
			i += blen;
		}
	}
	*star_count = stars;
	*prefix_count = pre;
	*suffix_count = suf;
	return n;
}

/* ---------- 第14关 基于规则的股票信息实体识别(BF暴力匹配) ---------- */

void EntityRecognition_BF()
{
	char rule[200];
	char text[2000];
	printf("请输入匹配规则(含*通配符):");
	read_line(rule, sizeof(rule));
	printf("请输入非结构化文本:");
	read_line(text, sizeof(text));

	//解析文本为字符数组
	CharInfo T[2000];
	int m = parse_chars(text, T, 2000);

	//解析规则
	RuleItem P[200];
	int star_count, prefix_count, suffix_count;
	int n = parse_rule(rule, P, &star_count, &prefix_count, &suffix_count);

	if (star_count == 0)
	{
		printf("规则中未包含通配符*!\n");
		return;
	}

	//已匹配实体(去重用)
	char found_entities[200][700];
	int found_cnt = 0;

	//BF暴力匹配: 依次尝试每个起始位置
	for (int s = 0; s + n <= m; s++)
	{
		int ok = 1;
		for (int j = 0; j < n; j++)
		{
			if (P[j].is_wild)
			{
				if (!T[s + j].is_cn)   //*只匹配中文字符
				{
					ok = 0;
					break;
				}
			}
			else
			{
				if (T[s + j].len != P[j].len)          //字节长度不同必不匹配
				{
					ok = 0;
					break;
				}
				if (memcmp(text + T[s + j].start, rule + P[j].start, P[j].len) != 0)
				{
					ok = 0;
					break;
				}
			}
		}

		if (ok)
		{
			//实体 = 规则中通配符段对应的中文字符串
			int es = T[s + prefix_count].start;
			int ec = T[s + prefix_count + star_count - 1].start
				+ T[s + prefix_count + star_count - 1].len;
			int elen = ec - es;
			char entity[700];
			memcpy(entity, text + es, elen);
			entity[elen] = '\0';

			//去重后保存
			int dup = 0;
			for (int k = 0; k < found_cnt; k++)
			{
				if (strcmp(found_entities[k], entity) == 0)
				{
					dup = 1;
					break;
				}
			}
			if (!dup && found_cnt < 200)
			{
				strcpy_s(found_entities[found_cnt], sizeof(found_entities[found_cnt]), entity);
				found_cnt++;
			}
		}
	}

	if (found_cnt == 0)
	{
		printf("\n未匹配到符合规则的实体\n");
	}
	else
	{
		printf("\n共匹配到%d个实体:\n", found_cnt);
		for (int i = 0; i < found_cnt; i++)
		{
			printf("第%d个实体:%s\n", i + 1, found_entities[i]);
		}
	}
}

/* ---------- 第15关 基于规则的股票信息关系抽取(BF算法) ---------- */

//BF暴力匹配: 返回sub在text中首次出现的起始字节位置, 未找到返回-1
static int bf_find(const char* text, const char* sub)
{
	int n = (int)strlen(text);
	int m = (int)strlen(sub);
	if (m == 0) return -1;
	for (int i = 0; i + m <= n; i++)
	{
		int j = 0;
		while (j < m && text[i + j] == sub[j]) j++;
		if (j == m) return i;
	}
	return -1;
}

//预定义关系规则项: 关系名称 + 规则模板
typedef struct
{
	const char* relation;     //关系名称
	const char* pattern;      //规则模板
	int has_placeholder;      //1=模板含A/B占位符(与构造串比较); 0=按给定字符串(与中间内容比较)
} RelationRule;

//预定义关系规则集(字母A代表头实体, B代表尾实体)
//注: 原文"总部在"模板仅列出"总部在"(未带B), 逻辑上应为匹配"总部在"后接实体B,
//     此处按给定字符串处理: 对中间内容"总部在"直接全等比较
static const RelationRule g_relation_rules[] =
{
	{ "属于行业大类", "A行业类别为B", 1 },
	{ "属于行业大类", "A属于B", 1 },
	{ "所属机构",     "A所属机构为B", 1 },
	{ "所属机构",     "A的公司是B", 1 },
	{ "所属机构",     "A隶属B", 1 },
	{ "具有注册地址", "A位于B", 1 },
	{ "具有注册地址", "A坐落于“B", 1 },
	{ "具有注册地址", "总部在", 0 },
	{ "主营业务为",   "A包括B", 1 },
	{ "主营业务为",   "A专注于B", 1 },
	{ "主营业务为",   "A主营业务为B", 1 },
};
#define RELATION_RULE_COUNT ((int)(sizeof(g_relation_rules) / sizeof(g_relation_rules[0])))

void RelationExtraction_BF()
{
	char entity1[200];
	char entity2[700];
	char text[2000];

	printf("请输入实体1(头实体):");
	read_line(entity1, sizeof(entity1));
	printf("请输入实体2(尾实体):");
	read_line(entity2, sizeof(entity2));
	printf("请输入非结构化文本:");
	read_line(text, sizeof(text));

	if (entity1[0] == '\0' || entity2[0] == '\0')
	{
		printf("实体不能为空!\n");
		return;
	}

	//1. BF算法在文本中定位实体1
	int pos1 = bf_find(text, entity1);
	if (pos1 < 0)
	{
		printf("文本中未找到实体1, 关系抽取失败!\n");
		return;
	}

	//2. BF算法在实体1之后定位实体2
	int len1 = (int)strlen(entity1);
	int search_start = pos1 + len1;
	int rel = bf_find(text + search_start, entity2);
	if (rel < 0)
	{
		printf("文本中未找到实体2(需位于实体1之后), 关系抽取失败!\n");
		return;
	}
	int pos2 = search_start + rel;

	//3. 提取中间内容: 实体1结束之后到实体2开始之前的子字符串
	int ms = pos1 + len1;
	int me = pos2;
	int mlen = me - ms;
	char middle[2000];
	memcpy(middle, text + ms, mlen);
	middle[mlen] = '\0';

	//4. 构造待匹配串: A[中间内容]B
	char built[2004];
	built[0] = 'A';
	memcpy(built + 1, middle, mlen);
	built[1 + mlen] = 'B';
	built[1 + mlen + 1] = '\0';

	//5. 与预定义规则集逐字符全等比较
	const char* matched_relation = NULL;
	for (int i = 0; i < RELATION_RULE_COUNT; i++)
	{
		int hit = g_relation_rules[i].has_placeholder
			? (strcmp(built, g_relation_rules[i].pattern) == 0)
			: (strcmp(middle, g_relation_rules[i].pattern) == 0);
		if (hit)
		{
			matched_relation = g_relation_rules[i].relation;
			break;
		}
	}

	//6. 判定与输出
	if (matched_relation != NULL)
	{
		printf("\n%s - %s - %s\n", entity1, matched_relation, entity2);
	}
	else
	{
		printf("\n两实体之间不存在对应关系, 无法抽取有效关系\n");
	}
}

/* ---------- 第16关 基于邻接表的股票知识图谱构建 ---------- */

//关系类型名称数组(下标对应关系类型)
const char* g_relationship_names[RELATIONSHIP_COUNT] =
{
	"所属机构", "上市于", "具有行业编码", "属于行业大类", "属于行业二级类",
	"位于省份", "位于城市", "具有注册地址", "主营业务为"
};

//实体类型名称数组(下标对应实体类型)
const char* g_entity_type_names[ENTITY_TYPE_COUNT] =
{
	"股票简称", "机构", "上市交易所", "行业编码", "行业大类",
	"行业二级类", "省份", "城市", "地址", "主营业务"
};

//全局知识图谱(邻接表), 供第17关直接调用
static ALGraph g_stock_graph;

//去掉行尾换行/回车符
static void strip_crlf(char* s)
{
	int len = (int)strlen(s);
	while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
		s[--len] = '\0';
}

//去掉UTF-8 BOM(文件可能以 EF BB BF 开头)
static void strip_bom(char* s)
{
	unsigned char* p = (unsigned char*)s;
	if (p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF)
		memmove(s, s + 3, strlen(s) - 2);   //连同末尾'\0'一起前移
}

//拆分一行: 优先按'|'分隔, 否则按空白或英文逗号分隔; 返回字段个数
static int split_fields(char* line, char fields[][MAX_ENTITY_LEN], int max)
{
	int n = 0;
	int pipe_mode = (strchr(line, '|') != NULL);
	char* p = line;

	while (n < max)
	{
		if (pipe_mode)
		{
			while (*p == '|') p++;
		}
		else
		{
			while (*p == ' ' || *p == '\t' || *p == ',') p++;
		}
		if (*p == '\0') break;

		char* start = p;
		if (pipe_mode)
		{
			while (*p && *p != '|') p++;
		}
		else
		{
			while (*p && *p != ' ' && *p != '\t' && *p != ',') p++;
		}
		int len = (int)(p - start);
		if (len >= MAX_ENTITY_LEN) len = MAX_ENTITY_LEN - 1;
		memcpy(fields[n], start, len);
		fields[n][len] = '\0';
		n++;
		if (*p == '\0') break;
	}
	return n;
}

//实体类型名 -> 下标, 未找到返回-1
static int find_entity_type(const char* name)
{
	for (int i = 0; i < ENTITY_TYPE_COUNT; i++)
		if (strcmp(name, g_entity_type_names[i]) == 0)
			return i;
	return -1;
}

//关系类型名 -> 下标, 未找到返回-1
static int find_relationship(const char* name)
{
	for (int i = 0; i < RELATIONSHIP_COUNT; i++)
		if (strcmp(name, g_relationship_names[i]) == 0)
			return i;
	return -1;
}

//关系类型 -> 尾实体类型(实体类型下标); 建边时据此确定尾实体的正确类型
static int tail_entity_type_of_relationship(int rel)
{
	switch (rel)
	{
	case 0: return 1;   //所属机构 -> 机构
	case 1: return 2;   //上市于 -> 上市交易所
	case 2: return 3;   //具有行业编码 -> 行业编码
	case 3: return 4;   //属于行业大类 -> 行业大类
	case 4: return 5;   //属于行业二级类 -> 行业二级类
	case 5: return 6;   //位于省份 -> 省份
	case 6: return 7;   //位于城市 -> 城市
	case 7: return 8;   //具有注册地址 -> 地址
	case 8: return 9;   //主营业务为 -> 主营业务
	default: return -1;
	}
}

//在图中按"内容+实体类型"查找顶点, 返回下标, 未找到返回-1
static int find_vertex(const ALGraph* g, const char* info, int entity_type)
{
	for (int i = 0; i < g->vexnum; i++)
		if (g->vertices[i].entity == entity_type && strcmp(g->vertices[i].info, info) == 0)
			return i;
	return -1;
}

//添加顶点(以"内容+实体类型"为唯一标识), 返回下标; 已存在则返回已有下标
static int add_vertex(ALGraph* g, const char* info, int entity_type)
{
	int idx = find_vertex(g, info, entity_type);
	if (idx >= 0)
		return idx;
	if (g->vexnum >= MAX_VERTEX_NUM)
	{
		printf("顶点数已达上限%d, 无法添加顶点:%s\n", MAX_VERTEX_NUM, info);
		return -1;
	}
	idx = g->vexnum;
	strncpy(g->vertices[idx].info, info, MAX_ENTITY_LEN - 1);
	g->vertices[idx].info[MAX_ENTITY_LEN - 1] = '\0';
	g->vertices[idx].entity = entity_type;
	g->vertices[idx].firstarc = NULL;
	g->vexnum++;
	return idx;
}

//添加一条有向边 from -> to, 关系类型为rel(头插法)
static void add_arc(ALGraph* g, int from, int to, int rel)
{
	if (from < 0 || to < 0)
		return;
	ArcNode* node = (ArcNode*)malloc(sizeof(ArcNode));
	if (node == NULL)
		return;
	node->adjvex = to;
	node->relationship = rel;
	node->nextarc = g->vertices[from].firstarc;
	g->vertices[from].firstarc = node;
	g->arcnum++;
}

//输出邻接表(供构建完成后查看)
static void print_adjacency_list(const ALGraph* g)
{
	for (int i = 0; i < g->vexnum; i++)
	{
		const char* etype = (g->vertices[i].entity >= 0 && g->vertices[i].entity < ENTITY_TYPE_COUNT)
			? g_entity_type_names[g->vertices[i].entity] : "未知类型";
		printf("顶点%d [%s] %s", i, etype, g->vertices[i].info);
		for (ArcNode* p = g->vertices[i].firstarc; p != NULL; p = p->nextarc)
		{
			printf("  ->( %s )%s", g_relationship_names[p->relationship], g->vertices[p->adjvex].info);
		}
		printf("\n");
	}
}

//读取 entity.txt 与 relation.txt, 构建双向邻接表知识图谱
void BuildKnowledgeGraph_ALGraph()
{
	ALGraph* g = &g_stock_graph;
	g->vexnum = 0;
	g->arcnum = 0;

	//1. 读取实体文件, 初始化顶点
	FILE* fp = fopen("entity.txt", "r");
	if (fp == NULL)
	{
		printf("无法打开实体文件 entity.txt!\n");
		return;
	}

	char line[1024];
	int entity_lines = 0;
	while (fgets(line, sizeof(line), fp) != NULL)
	{
		strip_bom(line);
		strip_crlf(line);
		if (line[0] == '\0')
			continue;

		char fields[3][MAX_ENTITY_LEN];
		int n = split_fields(line, fields, 3);
		if (n < 2)
		{
			printf("实体行格式错误(已跳过): %s\n", line);
			continue;
		}

		//实体行可能为"内容 类型"或"类型 内容", 自动识别类型所在列
		int t0 = find_entity_type(fields[0]);
		int t1 = find_entity_type(fields[1]);
		int etype;
		const char* info;
		if (t0 >= 0) { etype = t0; info = fields[1]; }
		else if (t1 >= 0) { etype = t1; info = fields[0]; }
		else
		{
			printf("实体行类型无法识别(已跳过): %s\n", line);
			continue;
		}
		add_vertex(g, info, etype);
		entity_lines++;
	}
	fclose(fp);
	printf("已从 entity.txt 读取 %d 行实体, 当前顶点数=%d\n", entity_lines, g->vexnum);

	//2. 读取三元组文件, 正向+反向双向建边
	FILE* fr = fopen("relation.txt", "r");
	if (fr == NULL)
	{
		printf("无法打开三元组文件 relation.txt!\n");
		return;
	}

	int relation_lines = 0;
	while (fgets(line, sizeof(line), fr) != NULL)
	{
		strip_bom(line);
		strip_crlf(line);
		if (line[0] == '\0')
			continue;

		char fields[3][MAX_ENTITY_LEN];
		int n = split_fields(line, fields, 3);
		if (n < 3)
		{
			printf("三元组行格式错误(已跳过): %s\n", line);
			continue;
		}

		//识别关系类型所在列, 兼容"头 关系 尾"、"关系 头 尾"、"头 尾 关系"等顺序
		int rel = -1;
		int head_idx = 0, tail_idx = 2;
		if (find_relationship(fields[1]) >= 0)
		{
			rel = find_relationship(fields[1]);
			head_idx = 0; tail_idx = 2;
		}
		else if (find_relationship(fields[0]) >= 0)
		{
			rel = find_relationship(fields[0]);
			head_idx = 1; tail_idx = 2;
		}
		else if (find_relationship(fields[2]) >= 0)
		{
			rel = find_relationship(fields[2]);
			head_idx = 0; tail_idx = 1;
		}
		else
		{
			printf("三元组关系类型无法识别(已跳过): %s\n", line);
			continue;
		}

		//头实体为股票简称, 尾实体类型由关系类型决定(缺失时自动补建对应类型的顶点)
		int head = find_vertex(g, fields[head_idx], 0);
		if (head < 0)
			head = add_vertex(g, fields[head_idx], 0);
		int expected_tail_type = tail_entity_type_of_relationship(rel);
		int tail = find_vertex(g, fields[tail_idx], expected_tail_type);
		if (tail < 0)
			tail = add_vertex(g, fields[tail_idx], expected_tail_type);

		//正向建边: 头实体 -> 尾实体
		add_arc(g, head, tail, rel);
		//立即反向建边: 尾实体 -> 头实体(关系类型保持不变)
		add_arc(g, tail, head, rel);

		relation_lines++;
	}
	fclose(fr);

	//3. 输出构建结果
	printf("已从 relation.txt 读取 %d 行三元组(每条双向各建一条边)\n", relation_lines);
	printf("知识图谱构建完成: 顶点数=%d, 边数=%d\n", g->vexnum, g->arcnum);
	print_adjacency_list(g);
}

//供第17关获取已构建的知识图谱
ALGraph* GetStockKnowledgeGraph()
{
	if (g_stock_graph.vexnum == 0)
		return NULL;
	return &g_stock_graph;
}

/* ---------- 第17关 基于知识图谱的股票推荐 ---------- */

//推荐输出顺序(关系类型下标, 与任务书规定的属性输出顺序一致)
static const int g_recommend_order[] =
{
	8,   //主营业务
	7,   //注册地址
	6,   //所在城市
	5,   //位于省份
	4,   //行业二级类
	3,   //行业大类
	2,   //行业编码
	1,   //上市交易所
	0    //所属机构名称
};
#define RECOMMEND_ORDER_COUNT ((int)(sizeof(g_recommend_order) / sizeof(g_recommend_order[0])))

//与 g_recommend_order 对应的属性名称
static const char* g_recommend_labels[] =
{
	"主营业务",     //rel 8
	"注册地址",     //rel 7
	"所在城市",     //rel 6
	"位于省份",     //rel 5
	"行业二级类",   //rel 4
	"行业大类",     //rel 3
	"行业编码",     //rel 2
	"上市交易所",   //rel 1
	"所属机构名称"  //rel 0
};

//从顶点v出发, 沿邻接边查找指定关系类型rel指向的第一个顶点, 返回下标, 未找到返回-1
static int get_arc_target(const ALGraph* g, int v, int rel)
{
	for (ArcNode* p = g->vertices[v].firstarc; p != NULL; p = p->nextarc)
	{
		if (p->relationship == rel)
			return p->adjvex;
	}
	return -1;
}

//基于知识图谱的股票推荐: 输入任意股票相关信息, 定位关联股票并按固定顺序输出其属性
void StockRecommend_KnowledgeGraph()
{
	ALGraph* g = GetStockKnowledgeGraph();
	if (g == NULL)
	{
		printf("知识图谱尚未构建, 请先运行第16关构建图谱!\n");
		return;
	}

	char input[300];
	printf("请输入股票相关信息(如行业编码、股票简称、机构名称等):");
	read_line(input, sizeof(input));
	if (input[0] == '\0')
	{
		printf("输入不能为空!\n");
		return;
	}

	//1. 节点定位: 在顶点数组中查找 info 与输入匹配的顶点
	int start = -1;
	for (int i = 0; i < g->vexnum; i++)
	{
		if (strcmp(g->vertices[i].info, input) == 0)
		{
			start = i;
			break;
		}
	}
	if (start < 0)
	{
		printf("图谱中未找到与 \"%s\" 匹配的顶点!\n", input);
		return;
	}

	//2. 利用双向边收集关联的股票(股票简称类型)顶点
	int stocks[MAX_VERTEX_NUM];
	int stock_cnt = 0;
	if (g->vertices[start].entity == 0)   //0=股票简称: 起点本身即股票
	{
		stocks[stock_cnt++] = start;
	}
	else
	{
		for (ArcNode* p = g->vertices[start].firstarc; p != NULL; p = p->nextarc)
		{
			if (g->vertices[p->adjvex].entity == 0)   //反向边指向的股票
			{
				int dup = 0;
				for (int k = 0; k < stock_cnt; k++)
				{
					if (stocks[k] == p->adjvex) { dup = 1; break; }
				}
				if (!dup)
					stocks[stock_cnt++] = p->adjvex;
			}
		}
	}

	if (stock_cnt == 0)
	{
		printf("未找到与 \"%s\" 关联的股票信息!\n", input);
		return;
	}

	//3. 信息聚合与输出: 每支股票按固定顺序输出属性, 缺失属性直接跳过
	printf("\n输入 \"%s\" 关联到 %d 支股票, 推荐信息序列如下:\n", input, stock_cnt);
	for (int i = 0; i < stock_cnt; i++)
	{
		int s = stocks[i];
		printf("\n【第%d支股票】%s\n", i + 1, g->vertices[s].info);
		for (int j = 0; j < RECOMMEND_ORDER_COUNT; j++)
		{
			int t = get_arc_target(g, s, g_recommend_order[j]);
			if (t >= 0)
				printf("%s: %s\n", g_recommend_labels[j], g->vertices[t].info);
		}
	}
}
