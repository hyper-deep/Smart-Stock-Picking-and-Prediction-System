#pragma once
#ifndef STOCK_SEEK_H
#define STOCK_SEEK_H

#include "stock.h"
void merge_sort(Stock stock[], int left, int right); //归并排序，使公司英文名有序

int Binary_Search(Stock stock[], char* stock_name, int stock_count, float* ASL); //二分查找查找股票信息

void Binary_Search_stock(Stock stock[], int stock_count); //合并连个函数，先归并排序，再二分查找

typedef struct BSTNode
{

	Stock data;  //存储股票信息

    struct BSTNode* left;

    struct BSTNode* right;

}bst_node;

bst_node* creat_bst_node(Stock stock);  //创建二叉查找树根节点

bst_node* insert_bst(bst_node* root, Stock stock);  //生成左右子树，插入节点

bst_node* search_bst(bst_node* root, char* name, int* count);   //二叉排序树查找股票信息,count为查找次数

void Quick_Sort(Stock stock[], int left, int right); //快速排序

void BST_Search_stock(Stock stock[], int stock_count); //二叉查找树查找股票信息测试

int Is_Prefix(char str[], char prefix[]);  //判断是否为前缀

void Print_Stock_Info(Stock stock);//打印股票信息

void PrefixSearch(Stock stock[], int count);   //股票英文名前缀查找

//字典树结构体定义
typedef struct TNode {
    Stock* stockPtr;         //股票指针, 英文名结尾结点指向对应股票
    struct TNode* child[53]; //53个后继字符: 26小写+26大写+1空格
} TNode, *TrieTree;

void Trie_Search_stock(Stock stock[], int stock_count); //字典树按英文名前缀查找股票信息

void Hash_lookup_Search(Stock stock[], int stock_count); //开放地址法散列查找股票信息

//链地址法散列表结点
typedef struct HashNode
{
    Stock* stockPtr;        //指向股票结构体
    struct HashNode* next;  //指向同槽位链表的下一个结点
} HashNode;

void Hash_seperate_Chain_Search(Stock stock[], int stock_count); //链地址法散列查找股票信息

#endif // STOCK_SEEK_H