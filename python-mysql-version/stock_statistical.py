# -*- coding: utf-8 -*-
"""
股票行情信息统计分析（第9-13关）
参考C实现: stock_statistical_analysis.c
数据来源: MySQL (stock_information 库)
"""
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

from db import fetch_all_stocks_with_detail


def load_stocks():
    """从MySQL读取股票(含行情), 返回 (stocks, count); stocks[0]占位"""
    rows = fetch_all_stocks_with_detail()
    stocks = [None]
    stocks.extend(rows)
    return stocks, len(rows)


def find_detail(stock, date):
    """在股票的每日行情中查找指定日期, 返回该日行情, 找不到返回None"""
    for d in stock.detail:
        if d.date == date:
            return d
    return None


def last_detail(stock):
    """取'最近一天'(最后一条)行情记录, 无则返回None"""
    if stock.detail_count <= 0:
        return None
    return stock.detail[stock.detail_count - 1]


def print_stock_info(stock):
    """打印股票基本信息"""
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


def print_detail(d):
    """打印某日行情记录"""
    print(f"日期:{d.date} 开盘:{d.openprice} 收盘:{d.closeprice} 最高:{d.highest} "
          f"最低:{d.lowest} 成交量:{d.volume} 成交额:{d.turnover} 换手率:{d.changerate} "
          f"涨跌幅:{d.risefallrate} 涨跌额:{d.risefallamount}")


# ==================== 第9关 基于链表的股票涨跌额分析 ====================

class RANode:
    """链表结点"""
    __slots__ = ("stock", "next")

    def __init__(self, stock):
        self.stock = stock
        self.next = None


def level9():
    """第9关: 基于链表的股票涨跌额分析(以涨跌额0为分界线: <0前, =0中, >0后)"""
    stocks, count = load_stocks()

    industry = input("请输入行业编码:").strip()
    date = input("请输入日期(如2026-07-22):").strip()

    # 三个分区链表: 涨跌额<0, =0, >0 (尾插法)
    neg_head = neg_tail = None
    zero_head = zero_tail = None
    pos_head = pos_tail = None

    def append(head, tail, stock):
        node = RANode(stock)
        if head is None:
            return node, node
        tail.next = node
        return head, node

    for i in range(1, count + 1):
        if stocks[i].briname != industry:
            continue
        d = find_detail(stocks[i], date)
        if d is None:
            continue
        val = float(d.risefallamount)
        if val < 0:
            neg_head, neg_tail = append(neg_head, neg_tail, stocks[i])
        elif val > 0:
            pos_head, pos_tail = append(pos_head, pos_tail, stocks[i])
        else:
            zero_head, zero_tail = append(zero_head, zero_tail, stocks[i])

    # 拼接: 负 -> 零 -> 正
    result = None
    if neg_head is not None:
        result = neg_head
        if zero_head is not None:
            neg_tail.next = zero_head
            if pos_head is not None:
                zero_tail.next = pos_head
        elif pos_head is not None:
            neg_tail.next = pos_head
    elif zero_head is not None:
        result = zero_head
        if pos_head is not None:
            zero_tail.next = pos_head
    else:
        result = pos_head

    print(f"\n行业编码[{industry}] 在日期[{date}] 的股票涨跌额排序信息(涨跌额<0在前,=0居中,>0在后):")
    k = 0
    p = result
    while p is not None:
        k += 1
        d = find_detail(p.stock, date)
        print(f"\n第{k}支 涨跌额:{d.risefallamount if d else '无'}")
        print_stock_info(p.stock)
        if d is not None:
            print_detail(d)
        p = p.next
    if k == 0:
        print(f"没有找到行业编码[{industry}]在日期[{date}]的股票数据")


# ==================== 第10关 基于并查集的股票行业换手率分析 ====================

class UnionFind:
    """并查集(带路径压缩)"""

    def __init__(self, n):
        self.parent = list(range(n + 1))  # 下标1..n

    def find(self, x):
        while self.parent[x] != x:
            self.parent[x] = self.parent[self.parent[x]]
            x = self.parent[x]
        return x

    def union(self, a, b):
        ra, rb = self.find(a), self.find(b)
        if ra != rb:
            self.parent[ra] = rb


def level10():
    """第10关: 基于并查集的股票行业换手率分析"""
    stocks, count = load_stocks()

    # 初始化并查集: 每支股票自成一个集合, 按行业编码合并
    uf = UnionFind(count)
    for i in range(1, count + 1):
        for j in range(i + 1, count + 1):
            if stocks[i].briname == stocks[j].briname:
                uf.union(i, j)

    industry = input("请输入行业编码:").strip()
    date = input("请输入日期(如2026-07-22):").strip()

    # 找到该行业编码对应子集的代表元
    root = -1
    for i in range(1, count + 1):
        if stocks[i].briname == industry:
            root = uf.find(i)
            break
    if root == -1:
        print(f"未找到行业编码[{industry}]的股票")
        return

    # 汇聚该行业所有股票在指定日期的换手率并求和
    total = 0.0
    cnt = 0
    for i in range(1, count + 1):
        if uf.find(i) != root:
            continue
        d = find_detail(stocks[i], date)
        if d is None:
            continue
        total += float(d.changerate)
        cnt += 1
    print(f"行业编码[{industry}] 在日期[{date}] 的换手率之和 = {total} (共{cnt}支股票)")


# ==================== 第11关 基于堆排序的股票成交量统计(Top3) ====================

def sift_down_vol(items, n, i):
    """大根堆下沉调整"""
    largest = i
    l = 2 * i + 1
    r = 2 * i + 2
    if l < n and items[l][1] > items[largest][1]:
        largest = l
    if r < n and items[r][1] > items[largest][1]:
        largest = r
    if largest != i:
        items[i], items[largest] = items[largest], items[i]
        sift_down_vol(items, n, largest)


def heap_sort_vol(items):
    """堆排序(结果升序, 数组末尾元素最大)"""
    n = len(items)
    for i in range(n // 2 - 1, -1, -1):
        sift_down_vol(items, n, i)
    for i in range(n - 1, 0, -1):
        items[0], items[i] = items[i], items[0]
        sift_down_vol(items, i, 0)


def level11():
    """第11关: 基于堆排序的股票成交量统计(Top3)"""
    stocks, count = load_stocks()

    date = input("请输入日期(如2026-07-22):").strip()
    industry = input("请输入行业编码:").strip()

    # 收集该行业在指定日期有成交量的股票: [(stock, volume)]
    items = []
    for i in range(1, count + 1):
        if stocks[i].briname != industry:
            continue
        d = find_detail(stocks[i], date)
        if d is None:
            continue
        items.append((stocks[i], float(d.volume)))

    if len(items) == 0:
        print(f"行业编码[{industry}]在日期[{date}]没有成交量数据")
        return

    heap_sort_vol(items)

    print(f"\n行业编码[{industry}] 在日期[{date}] 成交量Top3(按成交量从大到小):")
    top = min(3, len(items))
    for k in range(top):
        stock, vol = items[len(items) - 1 - k]  # 从大到小依次取末尾元素
        d = find_detail(stock, date)
        print(f"\n第{k + 1}名 成交量:{d.volume if d else '无'}({vol:.0f})")
        print_stock_info(stock)
        if d is not None:
            print_detail(d)


# ==================== 第12关 基于归并排序的股票成交量统计(按日期汇总) ====================

def merge_sort_dv(arr, left, right, temp):
    """归并排序(按total_volume从小到大)"""
    if left >= right:
        return
    mid = left + (right - left) // 2
    merge_sort_dv(arr, left, mid, temp)
    merge_sort_dv(arr, mid + 1, right, temp)

    i, j, k = left, mid + 1, left
    while i <= mid and j <= right:
        if arr[i][1] <= arr[j][1]:
            temp[k] = arr[i]
            i += 1
        else:
            temp[k] = arr[j]
            j += 1
        k += 1
    while i <= mid:
        temp[k] = arr[i]
        i += 1
        k += 1
    while j <= right:
        temp[k] = arr[j]
        j += 1
        k += 1
    for i in range(left, right + 1):
        arr[i] = temp[i]


def level12():
    """第12关: 基于归并排序的股票成交量统计(按日期累加汇总)"""
    stocks, count = load_stocks()

    industry = input("请输入行业编码:").strip()

    # 按日期累加该行业所有股票的成交量: [(date, total_volume)]
    table = {}
    for i in range(1, count + 1):
        if stocks[i].briname != industry:
            continue
        for d in stocks[i].detail:
            table[d.date] = table.get(d.date, 0.0) + float(d.volume)

    if len(table) == 0:
        print(f"行业编码[{industry}]没有成交量数据")
        return

    items = [(date, vol) for date, vol in table.items()]
    temp = [None] * len(items)
    merge_sort_dv(items, 0, len(items) - 1, temp)

    print(f"\n行业编码[{industry}] 按日期成交量汇总统计(按成交量从小到大):")
    for date, vol in items:
        print(f"日期:{date} 成交量合计:{vol:.0f}")


# ==================== 第13关 基于决策树的股票分类统计 ====================

class DTNode:
    """决策树结点(二叉树存储: left=满足, right=不满足)"""
    __slots__ = ("condition", "level", "left", "right", "count", "indices")

    def __init__(self, condition, level, indices):
        self.condition = condition
        self.level = level
        self.left = None
        self.right = None
        self.count = len(indices)
        self.indices = indices


# 全局决策参数(供建树与查询使用)
g_seccat_cond = ""
g_rf_thr = 0.0
g_vol_thr = 0.0


def parse_condition(s):
    """解析条件字符串, 如 '>0' -> 0, '>1000万股' -> 1000*10000"""
    num = ""
    has_dot = False
    for ch in s:
        if ch.isdigit() or ch == ".":
            if ch == ".":
                if has_dot:
                    continue
                has_dot = True
            num += ch
    v = float(num) if num else 0.0
    if "万" in s:
        v *= 10000
    if "亿" in s:
        v *= 100000000
    return v


def dt_satisfy(stock, level):
    """判断股票在某个决策层次下是否满足条件"""
    if level == 0:
        return stock.seccat == g_seccat_cond
    d = last_detail(stock)
    if d is None:
        return False  # 无行情数据视为不满足
    if level == 1:
        return float(d.risefallrate) > g_rf_thr
    return float(d.volume) > g_vol_thr


def build_dt(stocks, indices, level):
    """递归构建决策树(二叉树存储: left=满足, right=不满足)"""
    if level == 0:
        condition = f"二级门类是否为[{g_seccat_cond}]"
    elif level == 1:
        condition = f"最近一天涨跌幅是否>{g_rf_thr}"
    else:
        condition = f"最近一天成交量是否>{g_vol_thr}(股)"

    node = DTNode(condition, level, indices)
    if level >= 3:
        return node  # 叶子结点

    left_idx, right_idx = [], []
    for idx in indices:
        if dt_satisfy(stocks[idx], level):
            left_idx.append(idx)
        else:
            right_idx.append(idx)
    node.left = build_dt(stocks, left_idx, level + 1)
    node.right = build_dt(stocks, right_idx, level + 1)
    return node


def level13():
    """第13关: 基于决策树的股票分类统计"""
    global g_seccat_cond, g_rf_thr, g_vol_thr
    stocks, count = load_stocks()

    # 1.输入三个决策条件
    seccat_buf = input("请输入二级门类(如:计算机、通信和其他电子设备制造业):").strip()
    rf_buf = input("请输入最近一天涨跌幅条件(如>0):").strip()
    vol_buf = input("请输入最近一天成交量条件(如>1000万股):").strip()

    g_seccat_cond = seccat_buf
    g_rf_thr = parse_condition(rf_buf)
    g_vol_thr = parse_condition(vol_buf)

    # 2.初始森林为所有股票, 根结点包含全部股票, 依次按决策条件分裂构建决策树
    all_idx = list(range(1, count + 1))
    root = build_dt(stocks, all_idx, 0)
    print("决策树构建完成!")

    # 3.输入股票简称查询
    sname = input("请输入股票简称:").strip()
    idx = -1
    for i in range(1, count + 1):
        if stocks[i].indusname == sname:
            idx = i
            break
    if idx == -1:
        print(f"未找到股票简称[{sname}]")
        return

    # 4.从根沿决策路径下探, 记录每个决策结点
    path = []
    p = root
    while p is not None and p.left is not None and p.right is not None:
        path.append(p)
        if dt_satisfy(stocks[idx], p.level):
            p = p.left
        else:
            p = p.right

    # 5.从决策树的底部开始, 依次输出该股票的各决策条件
    print(f"\n股票[{sname}] 从决策树底部开始的决策条件:")
    for i in range(len(path) - 1, -1, -1):
        sat = dt_satisfy(stocks[idx], path[i].level)
        print(f"决策条件: {path[i].condition}  (该股票{'满足' if sat else '不满足'})")


if __name__ == "__main__":
    choice = input("请输入关卡(9-13):").strip()
    if choice == "9":
        level9()
    elif choice == "10":
        level10()
    elif choice == "11":
        level11()
    elif choice == "12":
        level12()
    elif choice == "13":
        level13()
