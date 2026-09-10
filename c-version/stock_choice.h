#pragma once
#ifndef STOCK_CHOICE_H
#define STOCK_CHOICE_H

//第14关 基于规则的股票信息实体识别(BF暴力匹配算法)
//规则中 * 表示任意一个中文字符(不匹配英文字母、数字或标点)
//连续的 * 数量代表待匹配实体的固定长度
void EntityRecognition_BF();

//第15关 基于规则的股票信息关系抽取(BF算法定位实体)
//提取两实体之间的中间内容, 左右拼接字母A、B后与预定义关系规则集逐字符全等比较
//匹配成功输出三元组: 实体1 - 关系名称 - 实体2
void RelationExtraction_BF();

/* ---------- 第16关 基于邻接表的股票知识图谱构建 ---------- */

//顶点/实体内容的最大字节数(UTF-8 中文每字3字节)
#define MAX_ENTITY_LEN 300
//邻接表顶点数组最大容量
#define MAX_VERTEX_NUM 2000

//关系类型数组(下标对应关系类型)
#define RELATIONSHIP_COUNT 9
extern const char* g_relationship_names[RELATIONSHIP_COUNT];

//实体类型数组(下标对应实体类型)
#define ENTITY_TYPE_COUNT 10
extern const char* g_entity_type_names[ENTITY_TYPE_COUNT];

//边结点结构体(邻接表的链表结点)
typedef struct ArcNode
{
	int adjvex;                 //该边所指向的顶点在顶点数组中的位置(下标)
	int relationship;           //边的类型, 即关系类型对应的数组下标(0~8)
	struct ArcNode* nextarc;    //指向下一条边的指针
} ArcNode;

//顶点结点结构体(邻接表的表头结点)
typedef struct VNode
{
	int entity;                 //顶点的类型, 即实体类型对应的数组下标(0~9), -1表示未知
	char info[MAX_ENTITY_LEN];  //顶点的内容, 即实体的具体名称/值
	ArcNode* firstarc;          //指向第一条依附该顶点的边的指针
} VNode;

typedef VNode AdjList[MAX_VERTEX_NUM];

//图的结构体
typedef struct ALGraph
{
	AdjList vertices;           //顶点数组
	int vexnum, arcnum;         //顶点数和边数
} ALGraph;

//读取 entity.txt 与 relation.txt, 构建双向邻接表知识图谱并输出结果
void BuildKnowledgeGraph_ALGraph();

//供第17关获取已构建的知识图谱(尚未构建时返回NULL)
ALGraph* GetStockKnowledgeGraph();

//第17关 基于知识图谱的股票推荐
//输入任意股票相关信息(行业编码、股票简称、机构名称等), 从匹配顶点出发利用双向边定位
//所有关联股票, 并严格按固定顺序(主营业务/注册地址/所在城市/位于省份/行业二级类/
//行业大类/行业编码/上市交易所/所属机构名称)输出每支股票的属性信息, 缺失属性跳过
void StockRecommend_KnowledgeGraph();

#endif // STOCK_CHOICE_H
