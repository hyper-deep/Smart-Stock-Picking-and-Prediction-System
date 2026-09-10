# -*- coding: utf-8 -*-
"""
股票信息查找（第4-8关）
参考C实现: stock_seek.c
"""
import sys
import io

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

from db import fetch_all_stocks_with_detail

# 散列表常量
HASH_SIZE = 400    # 散列表表长
HASH_PRIME = 397   # 除数(小于400的最大质数)


def load_stocks():
    """从MySQL读取股票(含行情), 返回 (stocks, count); stocks[0]占位"""
    rows = fetch_all_stocks_with_detail()
    stocks = [None]
    stocks.extend(rows)
    return stocks, len(rows)


# ==================== 第4关: 折半查找 ====================

def merge_sort(stocks, left, right):
    """二路归并排序(按英文名sname), 对应C的merge_sort"""
    if left >= right:
        return
    mid = left + (right - left) // 2
    merge_sort(stocks, left, mid)
    merge_sort(stocks, mid + 1, right)

    temp = [None] * (right - left + 1)
    i, j, k = left, mid + 1, 0
    while i <= mid and j <= right:
        if stocks[i].sname <= stocks[j].sname:
            temp[k] = stocks[i]
            i += 1
        else:
            temp[k] = stocks[j]
            j += 1
        k += 1
    while i <= mid:
        temp[k] = stocks[i]
        i += 1
        k += 1
    while j <= right:
        temp[k] = stocks[j]
        j += 1
        k += 1
    for i in range(left, right + 1):
        stocks[i] = temp[i - left]


def binary_search(stocks, stock_name, stock_count):
    """折半查找, 返回 (位置, ASL)"""
    low, high = 1, stock_count
    times = 0
    while low <= high:
        mid = (low + high) // 2
        times += 1
        if stocks[mid].sname == stock_name:
            return mid, times
        elif stocks[mid].sname > stock_name:
            high = mid - 1
        else:
            low = mid + 1
    return -1, 0


def level4():
    """第4关: 归并排序 + 折半查找"""
    stocks, count = load_stocks()
    merge_sort(stocks, 1, count)
    print("排序完成!")
    name = input().strip()
    pos, asl = binary_search(stocks, name, count)
    if pos == -1:
        print("查找失败")
    else:
        print("\n查找成功!")
        print(f"ASL={asl}")


# ==================== 第5关: 二叉排序树查找 ====================

class BSTNode:
    """二叉排序树结点"""
    __slots__ = ("data", "left", "right")

    def __init__(self, stock):
        self.data = stock
        self.left = None
        self.right = None


def insert_bst(root, stock):
    """插入结点, 返回根"""
    if root is None:
        return BSTNode(stock)
    if stock.sname < root.data.sname:
        root.left = insert_bst(root.left, stock)
    elif stock.sname > root.data.sname:
        root.right = insert_bst(root.right, stock)
    return root


def search_bst(root, name):
    """二叉排序树查找, 返回 (结点, 查找次数)"""
    count = 0
    p = root
    while p is not None:
        count += 1
        if name == p.data.sname:
            return p, count
        elif name < p.data.sname:
            p = p.left
        else:
            p = p.right
    return None, count


def quick_sort(stocks, left, right):
    """快速排序(按英文名), 对应C的Quick_Sort"""
    if left >= right:
        return
    i, j = left, right
    key = stocks[left]
    while i < j:
        while i < j and stocks[j].sname >= key.sname:
            j -= 1
        stocks[i] = stocks[j]
        while i < j and stocks[i].sname <= key.sname:
            i += 1
        stocks[j] = stocks[i]
    stocks[i] = key
    quick_sort(stocks, left, i - 1)
    quick_sort(stocks, i + 1, right)


def level5():
    """第5关: 二叉排序树查找(对比排序前后ASL)"""
    stocks, count = load_stocks()
    name = input().strip()

    # 原始序列建立BST
    root1 = BSTNode(stocks[1])
    for i in range(2, count + 1):
        insert_bst(root1, stocks[i])
    result1, asl1 = search_bst(root1, name)
    if result1:
        print("原始序列BST查找成功")
    else:
        print("原始序列BST查找失败")
    print(f"原始BST查找ASL={asl1}")

    # 快速排序
    quick_sort(stocks, 1, count)

    # 排序后重新建立BST
    root2 = BSTNode(stocks[1])
    for i in range(2, count + 1):
        insert_bst(root2, stocks[i])
    result2, asl2 = search_bst(root2, name)
    if result2:
        print("排序后BST查找成功")
    else:
        print("排序后BST查找失败")
    print(f"排序后BST查找ASL={asl2}")


# ==================== 第6关: 字典树查找 ====================

class TrieNode:
    """字典树结点: child[53] = 26小写+26大写+1空格"""
    __slots__ = ("stockPtr", "child")

    def __init__(self):
        self.stockPtr = None
        self.child = [None] * 53


def char_to_index(c):
    """字符映射到子结点下标, 不支持返回-1"""
    if "a" <= c <= "z":
        return ord(c) - ord("a")
    if "A" <= c <= "Z":
        return 26 + (ord(c) - ord("A"))
    if c == " ":
        return 52
    return -1


def insert_trie(root, stock):
    """按英文名sname插入字典树"""
    p = root
    for ch in stock.sname:
        idx = char_to_index(ch)
        if idx < 0:
            continue
        if p.child[idx] is None:
            p.child[idx] = TrieNode()
        p = p.child[idx]
    p.stockPtr = stock


def collect_trie(node, results):
    """收集以该结点为根的所有股票"""
    if node is None:
        return
    if node.stockPtr is not None:
        results.append(node.stockPtr)
    for i in range(53):
        collect_trie(node.child[i], results)


def print_stock_info(stock):
    """打印股票信息 (对应C的Print_Stock_Info)"""
    print("\n============================")
    print(f"股票代码:{stock.stockcode}")
    print(f"股票简称:{stock.indusname}")
    print(f"英文名称:{stock.sname}")
    print(f"行业编码:{stock.briname}")
    print(f"一级分类:{stock.pricat}")
    print(f"二级分类:{stock.seccat}")
    print(f"上市交易所:{stock.listexchange}")
    print(f"公司全称:{stock.wholecompany}")
    print(f"上市日期:{stock.launchdate}")
    print(f"省份:{stock.provin}")
    print(f"城市:{stock.city}")
    print(f"法人:{stock.legalper}")
    print(f"地址:{stock.addr}")
    print(f"网址:{stock.url}")
    print(f"邮箱:{stock.email}")
    print(f"电话:{stock.calling}")
    print(f"主营业务:{stock.mainbus}")
    print(f"经营范围:{stock.scopeofbus}")
    print("============================")


def level6():
    """第6关: 字典树按英文名前缀查找"""
    stocks, count = load_stocks()

    # 1.构建字典树, 统计英文名称长度总和
    root = TrieNode()
    total_len = 0
    for i in range(1, count + 1):
        insert_trie(root, stocks[i])
        total_len += len(stocks[i].sname)
    print("字典树构建完成!")

    # 2.输入英文名前缀
    prefix = input().strip()

    # 3.按前缀在字典树中查找
    p = root
    for ch in prefix:
        idx = char_to_index(ch)
        if idx < 0:
            continue
        if p.child[idx] is None:
            p = None
            break
        p = p.child[idx]

    if p is None:
        print(f"\n查找失败,没有找到以{prefix}开头的股票")
    else:
        results = []
        collect_trie(p, results)
        if len(results) == 0:
            print(f"\n查找失败,没有找到以{prefix}开头的股票")
        else:
            for number, st in enumerate(results, 1):
                print(f"\n第{number}个匹配结果:")
                print_stock_info(st)
            print(f"\n共找到{len(results)}支股票")

    # 4.统计ASL
    asl = (float(total_len) / count) if count > 0 else 0.0
    print(f"字典树中英文名称长度总和={total_len}")
    print(f"ASL={asl}")


# ==================== 第7关: 开放地址法散列查找 ====================

def hash_func(key):
    """散列函数: Hash(key)=(Σ int(c_i)*i²) mod 397"""
    total = 0
    for i, ch in enumerate(key):
        total += ord(ch) * (i * i)
    return total % HASH_PRIME


class OpenAddressHashTable:
    """开放地址法散列表(线性探测)"""

    def __init__(self, size=HASH_SIZE):
        self.size = size
        self.table = [None] * size

    def insert(self, stock):
        """
        线性探测插入, 返回(位置, 探测比较次数); 表满返回(-1, 次数)
        对应C的hash_insert: 每次探测都计数
        """
        index = hash_func(stock.sname)
        cmp_count = 0
        for _ in range(self.size):
            cmp_count += 1
            if self.table[index] is None:
                self.table[index] = stock
                return index, cmp_count
            index = (index + 1) % self.size
        return -1, cmp_count

    def search(self, key):
        """线性探测查找, 成功返回位置, 失败返回-1; 返回(位置, 比较次数)"""
        index = hash_func(key)
        cmp_count = 0
        for _ in range(self.size):
            cmp_count += 1
            if self.table[index] is None:
                return -1, cmp_count
            if self.table[index].sname == key:
                return index, cmp_count
            index = (index + 1) % self.size
        return -1, cmp_count


def level7():
    """第7关: 开放地址法散列查找"""
    stocks, count = load_stocks()

    ht = OpenAddressHashTable()
    insert_cmp = 0
    for i in range(1, count + 1):
        pos, cmp_ = ht.insert(stocks[i])
        if pos == -1:
            print("散列表已满, 插入失败!")
            return
        insert_cmp += cmp_
    print("散列表构建完成!")
    print(f"插入{count}支股票, 插入总比较次数={insert_cmp}")

    name = input("请输入股票英文名称:").strip()
    pos, cmp_count = ht.search(name)
    if pos == -1:
        print(f"查找失败, 散列表中不存在股票:{name}")
    else:
        print("\n查找成功!")
        print(f"散列表位置:{pos}")
        print(f"本次查找ASL={cmp_count}")


# ==================== 第8关: 链地址法散列查找 ====================

class HashNode:
    """链地址法散列表结点"""
    __slots__ = ("stockPtr", "next")

    def __init__(self, stock):
        self.stockPtr = stock
        self.next = None


class ChainHashTable:
    """链地址法(拉链法)散列表"""

    def __init__(self, size=HASH_SIZE):
        self.size = size
        self.table = [None] * size

    def insert(self, stock):
        """
        头插法插入, 返回(槽位下标, 比较次数); 关键字已存在则不重复插入
        对应C的chain_insert: 遍历链表时每个结点比较一次
        """
        index = hash_func(stock.sname)
        p = self.table[index]
        cmp_count = 0
        while p is not None:
            cmp_count += 1
            if p.stockPtr.sname == stock.sname:
                return index, cmp_count
            p = p.next
        node = HashNode(stock)
        node.next = self.table[index]
        self.table[index] = node
        return index, cmp_count

    def search(self, key):
        """链地址法查找, 成功返回槽位下标, 失败返回-1; 返回(位置, 比较次数)"""
        index = hash_func(key)
        p = self.table[index]
        cmp_count = 0
        while p is not None:
            cmp_count += 1
            if p.stockPtr.sname == key:
                return index, cmp_count
            p = p.next
        return -1, cmp_count


def print_stock_detail(stock):
    """打印某支股票的详细交易数据 (对应C的Print_Stock_Detail)"""
    print(f"\n======详细交易数据(共{stock.detail_count}条)======")
    for i, d in enumerate(stock.detail, 1):
        print(f"第{i}条 日期:{d.date} 开盘:{d.openprice} 收盘:{d.closeprice} "
              f"最高:{d.highest} 最低:{d.lowest} 成交量:{d.volume} 成交额:{d.turnover} "
              f"换手率:{d.changerate} 涨跌幅:{d.risefallrate} 涨跌额:{d.risefallamount}")


def level8():
    """第8关: 链地址法散列查找"""
    stocks, count = load_stocks()

    ht = ChainHashTable()
    insert_cmp = 0
    for i in range(1, count + 1):
        _, cmp_ = ht.insert(stocks[i])
        insert_cmp += cmp_
    print("链地址法散列表构建完成!")
    print(f"插入{count}支股票, 插入总比较次数={insert_cmp}")

    name = input("请输入股票英文名称:").strip()
    pos, cmp_count = ht.search(name)
    if pos == -1:
        print(f"查找失败, 散列表中不存在股票:{name}")
    else:
        # 定位到该槽位链表中匹配的结点, 以便输出股票信息
        p = ht.table[pos]
        while p is not None and p.stockPtr.sname != name:
            p = p.next
        print("\n查找成功!")
        print(f"链表头指针所在槽位(数组下标):{pos}")
        if p is not None:
            print_stock_info(p.stockPtr)
            print_stock_detail(p.stockPtr)
        print(f"本次查找ASL={cmp_count}")


if __name__ == "__main__":
    choice = input("请输入关卡(4-8):").strip()
    if choice == "4":
        level4()
    elif choice == "5":
        level5()
    elif choice == "6":
        level6()
    elif choice == "7":
        level7()
    elif choice == "8":
        level8()
