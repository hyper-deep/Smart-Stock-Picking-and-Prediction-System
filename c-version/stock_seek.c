#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "portable.h"
#include "stock_seek.h"
#include "stock_maintain.h" //add_stock_detail: 读取详细交易数据

static Stock merge_temp[MAX_STOCK]; //归并排序临时缓冲区

void merge_sort(Stock stock[], int left, int right)  //归并排序，使公司英文名有序
{
	if (left >= right)
	{
		return;
	}

	int mid = left + (right - left) / 2;
	merge_sort(stock, left, mid);      //递归排序左半部分
	merge_sort(stock, mid + 1, right); //递归排序右半部分

	int i = left, j = mid + 1, k = left;
	while (i <= mid && j <= right)
	{
		if (strcmp(stock[i].sname, stock[j].sname) <= 0)
		{
			merge_temp[k++] = stock[i++];
		}
		else
		{
			merge_temp[k++] = stock[j++];
		}
	}
	while (i <= mid)
	{
		merge_temp[k++] = stock[i++];
	}
	while (j <= right)
	{
		merge_temp[k++] = stock[j++];
	}
	for (i = left; i <= right; i++)
	{
		stock[i] = merge_temp[i];
	}
}
int Binary_Search(Stock stock[], char* stock_name, int stock_count,float *ASL)  //二分查找查找股票信息
{
	int low = 1;
	int high = stock_count;  // 数据存放在 stock[1..stock_count]，stock[0]为字段名
	int times = 0;
	while (low <= high)
	{
		int mid = (low + high) / 2;
		times++;
		int result =strcmp(stock[mid].sname,stock_name);
		if (result == 0)
		{
			*ASL = times;
			return mid;
		}
		else if (result > 0)
		{
			high = mid - 1;
		}
		else
		{
			low = mid + 1;
		}
	}
	*ASL = 0;
	return -1;
}
void Binary_Search_stock(Stock stock[], int stock_count) //合并连个函数，先归并排序，再二分查找
{
	
	//1.按照英文名归并排序 (stock[0]为字段名，排序范围[1, stock_count])
	merge_sort(stock,1,stock_count);
	printf("排序完成!\n");
	//2.输入查询关键字
	char name[100];

	if (fgets(name, sizeof(name), stdin) == NULL)
	{
		name[0] = '\0';
	}
	size_t nlen = strlen(name);
	while (nlen > 0 && (name[nlen - 1] == '\n' || name[nlen - 1] == '\r'))
	{
		name[--nlen] = '\0';
	}

	//3.折半查找
	float ASL=0;
	int pos =Binary_Search(stock,name,stock_count,&ASL);
	if (pos == -1)
	{
		printf("查找失败\n");
	}
	else
	{
		printf("\n查找成功!\n");
		printf("ASL=%f\n",ASL);
	}
}
bst_node* creat_bst_node(Stock stock)  //创建二叉查找树根节点
{
	bst_node *new_node = (bst_node *)malloc(sizeof(bst_node));
	if (new_node == NULL)
	{
		printf("内存分配失败\n");
		exit(1);
	}
	new_node->data =stock;
	new_node->left = NULL;
	new_node->right = NULL;
	return new_node;
}
bst_node* insert_bst(bst_node* root, Stock stock)  //生成左右子树，插入节点
{
	if (root == NULL)
	{
		return creat_bst_node(stock);
	}
	if (strcmp(stock.sname, root->data.sname) < 0)
	{
		root->left = insert_bst(root->left, stock);
	}
	else if (strcmp(stock.sname, root->data.sname) > 0)
	{
		root->right = insert_bst(root->right, stock);
	}
	return root;
} 
bst_node* search_bst(bst_node* root, char* name, int* count)   //二叉排序树查找股票信息,count为查找次数
{
	if (root == NULL)
	{
		return NULL;
	}
	(*count)++;
	int result =strcmp(name,root->data.sname);
	if (result == 0)
	{
		return root;
	}
	else if (result < 0)  //名字小于当前节点，向左子树查找
	{
		return search_bst(root->left, name, count);
	}
	else //名字大于当前节点，向右子树查找
	{
		return search_bst(root->right, name, count);
	}
}
void Quick_Sort(Stock stock[], int left, int right) //快速排序
{
	if (left >= right)
	{
		return;
	}
	int i = left;
	int j = right;
	Stock key =stock[left];
	while (i < j)
	{
		while (i < j &&strcmp(stock[j].sname,key.sname) >= 0)
		{
			j--;
		}
		stock[i] = stock[j];
		while (i < j &&strcmp(stock[i].sname,key.sname) <= 0)
		{
			i++;
		}
		stock[j] = stock[i];
	}
	stock[i] = key;
	Quick_Sort(stock,left,i - 1);
	Quick_Sort(stock,i + 1,right);
}
void BST_Search_stock(Stock stock[], int stock_count) //二叉查找树查找股票信息测试
{
	char name[100];
	//读取整行, 支持含空格的英文名
	if (fgets(name, sizeof(name), stdin) == NULL)
	{
		name[0] = '\0';
	}
	size_t nlen = strlen(name);
	while (nlen > 0 && (name[nlen - 1] == '\n' || name[nlen - 1] == '\r'))
	{
		name[--nlen] = '\0';
	}
	//-------------------------
	//第一种情况
	//原始序列建立BST
	//-------------------------
	bst_node* root1 =creat_bst_node(stock[1]);
	for (int i = 2; i <= stock_count; i++)
	{
		insert_bst(root1, stock[i]);
	}
	int asl1 = 0;
	bst_node* result1 =search_bst(root1, name, &asl1);
	if (result1)
	{
		printf("原始序列BST查找成功\n");
	}
	else 
	{
		printf("原始序列BST查找失败\n");
	}
	printf("原始BST查找ASL=%d\n",asl1);
	//-------------------------
	//快速排序
	//-------------------------



	Quick_Sort(stock,1,stock_count);
	//-------------------------
	//重新建立BST
	//-------------------------
	bst_node* root2 =creat_bst_node(stock[1]);
	for (int i = 2; i <= stock_count; i++)
	{
		insert_bst(root2, stock[i]);
	}
	int asl2 = 0;
	bst_node* result2 =search_bst(root2, name, &asl2);
	if (result2)
	{
		printf("排序后BST查找成功\n");
	}
	else
	{
		printf("排序后BST查找失败\n");
	}
	printf("排序后BST查找ASL=%d\n",asl2);
}

int Is_Prefix(char str[], char prefix[])  //判断是否为前缀
{

	int len = strlen(prefix);
	//如果股票英文名长度小于前缀
	//一定不匹配
	if (strlen(str) < len)
	{
		return 0;
	}
	for (int i = 0;i < len;i++)
	{

		if (str[i] != prefix[i])
		{
			return 0;
		}

	}
	return 1;

}
void Print_Stock_Info(Stock stock)
{	//打印股票信息
	
	printf("\n============================\n");
	printf("股票代码:%s\n",
		stock.stockcode);


	printf("股票简称:%s\n",
		stock.indusname);


	printf("英文名称:%s\n",
		stock.sname);


	printf("行业编码:%s\n",
		stock.briname);


	printf("一级分类:%s\n",
		stock.pricat);


	printf("二级分类:%s\n",
		stock.seccat);


	printf("上市交易所:%s\n",
		stock.listexchange);


	printf("公司全称:%s\n",
		stock.wholecompany);


	printf("上市日期:%s\n",
		stock.launchdate);


	printf("省份:%s\n",
		stock.provin);


	printf("城市:%s\n",
		stock.city);


	printf("法人:%s\n",
		stock.legalper);


	printf("地址:%s\n",
		stock.addr);


	printf("网址:%s\n",
		stock.url);


	printf("邮箱:%s\n",
		stock.email);


	printf("电话:%s\n",
		stock.calling);


	printf("主营业务:%s\n",
		stock.mainbus);


	printf("经营范围:%s\n",
		stock.scopeofbus);
	printf("============================\n");

}
void PrefixSearch(Stock stock[], int count)   //股票英文名前缀查找
{
	char prefix[100];
	printf("请输入股票英文名前缀:");
	scanf_s("%s", prefix, sizeof(prefix));
	int flag = 0;
	int number = 0;
	/*
	   注意：
	   stock[0]保存字段名称
	   所以从stock[1]开始遍历
	*/
	for (int i = 1;i <= count;i++)
	{
		if (Is_Prefix(stock[i].briname,prefix))
		{
			flag = 1;
			number++;
			printf("\n第%d个匹配结果:\n",number);
			Print_Stock_Info(stock[i]);
		}
	}
	if (flag == 0)
	{

		printf("\n查找失败,没有找到以%s开头的股票\n",prefix);
	}
	else
	{
		printf("\n共找到%d支股票\n",number);
	}
}

//----------------------- 字典树(按英文名前缀查找) -----------------------

static TrieTree create_trie_node()  //创建字典树结点
{
	TrieTree node = (TrieTree)malloc(sizeof(TNode));
	if (node == NULL)
	{
		printf("内存分配失败\n");
		exit(1);
	}
	node->stockPtr = NULL;
	for (int i = 0; i < 53; i++)
	{
		node->child[i] = NULL;
	}
	return node;
}

static int char_to_index(char c)  //将字符映射到子结点下标, 不支持的字符返回-1
{
	if (c >= 'a' && c <= 'z')
	{
		return c - 'a';        // 0..25 小写字母
	}
	if (c >= 'A' && c <= 'Z')
	{
		return 26 + (c - 'A'); // 26..51 大写字母
	}
	if (c == ' ')
	{
		return 52;             // 空格
	}
	return -1;                 // 其它字符(如 - ' &)跳过
}

static void insert_trie(TrieTree root, Stock* stock)  //按英文名 sname 插入字典树
{
	TrieTree p = root;
	char* name = stock->sname;
	for (int i = 0; name[i] != '\0'; i++)
	{
		int idx = char_to_index(name[i]);
		if (idx < 0)
		{
			continue;          // 跳过不支持的字符
		}
		if (p->child[idx] == NULL)
		{
			p->child[idx] = create_trie_node();
		}
		p = p->child[idx];
	}
	p->stockPtr = stock;       // 英文名结尾结点指向该股票
}

static void collect_trie(TrieTree node, int* number)  //收集以该结点为根的所有股票
{
	if (node == NULL)
	{
		return;
	}
	if (node->stockPtr != NULL)
	{
		(*number)++;
		printf("\n第%d个匹配结果:\n", *number);
		Print_Stock_Info(*(node->stockPtr));
	}
	for (int i = 0; i < 53; i++)
	{
		collect_trie(node->child[i], number);
	}
}

void Trie_Search_stock(Stock stock[], int stock_count)  //字典树按英文名前缀查找股票信息
{
	//1.构建字典树, 同时统计英文名称长度总和
	TrieTree root = create_trie_node();
	int total_len = 0;
	for (int i = 1; i <= stock_count; i++)
	{
		insert_trie(root, &stock[i]);
		total_len += (int)strlen(stock[i].sname);
	}
	printf("字典树构建完成!\n");

	//2.输入英文名前缀
	char prefix[100];
	//scanf_s 读取输入
	if (scanf_s("%s", prefix, sizeof(prefix)) != 1)
	{
		printf("\n输入读取失败\n");
		return;
	}

	//3.按前缀在字典树中查找
	TrieTree p = root;
	for (int i = 0; prefix[i] != '\0'; i++)
	{
		int idx = char_to_index(prefix[i]);
		if (idx < 0)
		{
			continue;          // 跳过不支持的字符
		}
		if (p->child[idx] == NULL)
		{
			p = NULL;
			break;
		}
		p = p->child[idx];
	}

	if (p == NULL)
	{
		printf("\n查找失败,没有找到以%s开头的股票\n", prefix);
	}
	else
	{
		int number = 0;
		collect_trie(p, &number);
		if (number == 0)
		{
			printf("\n查找失败,没有找到以%s开头的股票\n", prefix);
		}
		else
		{
			printf("\n共找到%d支股票\n", number);
		}
	}

	//4.统计 ASL
	float ASL = (stock_count > 0) ? (float)total_len / stock_count : 0.0f;
	printf("字典树中英文名称长度总和=%d\n", total_len);
	printf("ASL=%f\n", ASL);
}

//----------------------- 开放地址法散列查找(线性探测) -----------------------

#define HASH_SIZE 400   //散列表表长
#define HASH_PRIME 397  //除数(小于400的最大质数)

static Stock* hash_table[HASH_SIZE]; //开放地址法散列表, 空位为NULL

static int hash_func(char* key)  //散列函数: Hash(key)=(Σ int(c_i)*i²) mod 397
{
	int sum = 0;
	int n = (int)strlen(key);
	for (int i = 0; i < n; i++)
	{
		sum += (int)key[i] * (i * i);
	}
	return sum % HASH_PRIME;
}

static int hash_insert(Stock* stock, int* cmp_count)  //线性探测插入, 返回存储位置
{
	int index = hash_func(stock->sname);
	for (int i = 0; i < HASH_SIZE; i++)
	{
		(*cmp_count)++;               //记录一次探测比较
		if (hash_table[index] == NULL)
		{
			hash_table[index] = stock;
			return index;
		}
		index = (index + 1) % HASH_SIZE;
	}
	return -1;                        //表满
}

static int hash_search(char* key, int* cmp_count)  //线性探测查找, 成功返回位置, 失败返回-1
{
	int index = hash_func(key);
	for (int i = 0; i < HASH_SIZE; i++)
	{
		(*cmp_count)++;               //记录一次探测比较
		if (hash_table[index] == NULL)
		{
			return -1;                //探测到空位, 说明不存在
		}
		if (strcmp(hash_table[index]->sname, key) == 0)
		{
			return index;             //查找成功
		}
		index = (index + 1) % HASH_SIZE;
	}
	return -1;                        //探测完整个表仍未找到
}

void Hash_lookup_Search(Stock stock[], int stock_count)  //开放地址法散列查找股票信息
{
	//1.初始化散列表(全部置空)
	for (int i = 0; i < HASH_SIZE; i++)
	{
		hash_table[i] = NULL;
	}

	//2.插入所有股票, 记录插入总比较次数
	int insert_cmp = 0;
	for (int i = 1; i <= stock_count; i++)
	{
		if (hash_insert(&stock[i], &insert_cmp) == -1)
		{
			printf("散列表已满, 插入失败!\n");
			return;
		}
	}
	printf("散列表构建完成!\n");
	printf("插入%d支股票, 插入总比较次数=%d\n", stock_count, insert_cmp);

	//3.输入待查找的股票英文名称
	char name[100];
	//读取整行(英文名可能含空格)
	if (fgets(name, sizeof(name), stdin) == NULL)
	{
		printf("输入读取失败\n");
		return;
	}
	size_t len = strlen(name);
	while (len > 0 && (name[len - 1] == '\n' || name[len - 1] == '\r'))
	{
		name[--len] = '\0';   //去掉末尾换行符
	}

	//4.按线性探测查找, 判断散列表中是否存在该股票
	int cmp_count = 0;
	int pos = hash_search(name, &cmp_count);
	if (pos == -1)
	{
		printf("查找失败, 散列表中不存在股票:%s\n", name);
	}
	else
	{
		printf("\n查找成功!\n");
		printf("散列表位置:%d\n", pos);
		printf("本次查找ASL=%d\n", cmp_count);
	}
}

//----------------------- 链地址法(拉链法)散列查找 -----------------------

static HashNode* chain_table[HASH_SIZE]; //链地址法散列表: 每个槽位指向一条链表

static HashNode* create_hash_node(Stock* stock)  //创建链地址法结点
{
	HashNode* node = (HashNode*)malloc(sizeof(HashNode));
	if (node == NULL)
	{
		printf("内存分配失败\n");
		exit(1);
	}
	node->stockPtr = stock;
	node->next = NULL;
	return node;
}

static int chain_insert(Stock* stock, int* cmp_count)  //链地址法插入, 返回槽位下标
{
	int index = hash_func(stock->sname);
	HashNode* p = chain_table[index];
	//遍历链表, 记录比较次数, 若关键字已存在则不重复插入
	while (p != NULL)
	{
		(*cmp_count)++;
		if (strcmp(p->stockPtr->sname, stock->sname) == 0)
		{
			return index;   //已存在
		}
		p = p->next;
	}
	//头插法将新结点插入该槽位链表
	HashNode* node = create_hash_node(stock);
	node->next = chain_table[index];
	chain_table[index] = node;
	return index;
}

static int chain_search(char* key, int* cmp_count)  //链地址法查找, 成功返回槽位下标, 失败返回-1
{
	int index = hash_func(key);
	HashNode* p = chain_table[index];
	while (p != NULL)
	{
		(*cmp_count)++;
		if (strcmp(p->stockPtr->sname, key) == 0)
		{
			return index;   //返回链表头指针所在槽位(数组下标)
		}
		p = p->next;
	}
	return -1;              //链表中无该股票
}

static void Print_Stock_Detail(Stock* stock)  //打印某支股票的详细交易数据(带单位)
{
	printf("\n======详细交易数据(共%d条)======\n", stock->detail_count);
	for (int i = 0; i < stock->detail_count; i++)
	{
		StockDetail* d = &stock->detail[i];
		double turnover_yi = atof(d->turnover) / 100000000.0;   //成交额 元 -> 亿元
		printf("第%d条 日期:%s 开盘:%s 收盘:%s 最高:%s 最低:%s 成交量:%s 成交额:%.2f亿元 换手率:%s 涨跌幅:%s 涨跌额:%s\n",
			i + 1,
			d->date, d->openprice, d->closeprice, d->highest, d->lowest,
			d->volume, turnover_yi, d->changerate, d->risefallrate, d->risefallamount);
	}
}

void Hash_seperate_Chain_Search(Stock stock[], int stock_count)  //链地址法散列查找股票信息
{
	//1.读取每支股票的详细交易数据
	add_stock_detail(stock, stock_count);

	//2.初始化散列表(所有槽位链表置空)
	for (int i = 0; i < HASH_SIZE; i++)
	{
		chain_table[i] = NULL;
	}

	//3.插入所有股票, 记录插入总比较次数
	int insert_cmp = 0;
	for (int i = 1; i <= stock_count; i++)
	{
		chain_insert(&stock[i], &insert_cmp);
	}
	printf("链地址法散列表构建完成!\n");
	printf("插入%d支股票, 插入总比较次数=%d\n", stock_count, insert_cmp);

	//4.输入待查找的股票英文名称
	char name[100];
	printf("请输入股票英文名称:");
	//读取整行(英文名可能含空格)
	if (fgets(name, sizeof(name), stdin) == NULL)
	{
		printf("输入读取失败\n");
		return;
	}
	size_t len = strlen(name);
	while (len > 0 && (name[len - 1] == '\n' || name[len - 1] == '\r'))
	{
		name[--len] = '\0';   //去掉末尾换行符
	}

	//5.链地址法查找
	int cmp_count = 0;
	int pos = chain_search(name, &cmp_count);
	if (pos == -1)
	{
		printf("查找失败, 散列表中不存在股票:%s\n", name);
	}
	else
	{
		//定位到该槽位链表中匹配的结点, 以便输出股票信息
		HashNode* p = chain_table[pos];
		while (p != NULL && strcmp(p->stockPtr->sname, name) != 0)
		{
			p = p->next;
		}
		printf("\n查找成功!\n");
		printf("链表头指针所在槽位(数组下标):%d\n", pos);
		if (p != NULL)
		{
			Print_Stock_Info(*(p->stockPtr));
			Print_Stock_Detail(p->stockPtr);
		}
		printf("本次查找ASL=%d\n", cmp_count);
	}
}