# -*- coding: utf-8 -*-
"""
股票信息管理（第1-3关）
参考C实现: stock_maintain.c
数据来源: MySQL (stock_information 库)
"""
import sys
import io

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

from db import get_conn, fetch_all_stocks_with_detail


def split_line(line):
    """按 '|' 分割一行, 返回字段列表 (对应C的Split函数, 去掉换行)"""
    return line.rstrip("\r\n").split("|")


def save_stock_file(stocks, stock_count):
    """
    将顺序表中的股票信息写入 new_stocks.txt (对应C的save_stock_file, 文件名按任务书为new_stocks.txt)
    注意: stocks[0] 为字段名表头, 数据从 stocks[1] 开始
    """
    # 表头
    header = "股票代码|股票简称|英文名|行业编码|一级分类|二级分类|上市交易所|公司全称|上市日期|省份|城市|法人|地址|网址|邮箱|电话|主营业务|经营范围"
    with open("new_stocks.txt", "w", encoding="utf-8") as f:
        f.write(header + "\n")
        for i in range(1, stock_count + 1):
            s = stocks[i]
            f.write("|".join([
                s.stockcode, s.indusname, s.sname, s.briname, s.pricat, s.seccat,
                s.listexchange, s.wholecompany, s.launchdate, s.provin, s.city,
                s.legalper, s.addr, s.url, s.email, s.calling, s.mainbus, s.scopeofbus,
            ]) + "\n")
    print("股票信息已经保存到new_stocks.txt")


def read_stock_file():
    """
    从 MySQL 读取股票基本信息和每日行情, 构建顺序表
    返回: (stocks, stock_count)  stocks[0]为字段名占位, 数据从stocks[1]开始
    """
    rows = fetch_all_stocks_with_detail()
    stocks = [None]  # stocks[0] 占位 (对应C中存放字段名的位置)
    for row in rows:
        stocks.append(row)
    return stocks, len(rows)


# ==================== 第1关: 股票信息存储 ====================

def search_by_pos(stocks, count):
    """输出特定位置的股票基本信息"""
    while True:
        try:
            pos = int(input(f"请输入要查询股票的位置(1-{count}):").strip())
        except ValueError:
            print("位置不存在,请重新输入?")
            continue
        if 1 <= pos <= count:
            break
        print("位置不存在,请重新输入?")

    s = stocks[pos]
    print("\n======股票详细信息 ======")
    print(f"股票代码:{s.stockcode}")
    print(f"股票简称:{s.indusname}")
    print(f"英文名称:{s.sname}")
    print(f"行业编码:{s.briname}")
    print(f"一级分类:{s.pricat}")
    print(f"二级分类:{s.seccat}")
    print(f"上市交易所:{s.listexchange}")
    print(f"公司全称:{s.wholecompany}")
    print(f"上市日期:{s.launchdate}")
    print(f"省份:{s.provin}")
    print(f"城市:{s.city}")
    print(f"法人:{s.legalper}")
    print(f"地址:{s.addr}")
    print(f"网址:{s.url}")
    print(f"邮箱:{s.email}")
    print(f"电话:{s.calling}")
    print(f"主营业务:{s.mainbus}")
    print(f"经营范围:{s.scopeofbus}")


def level1():
    """第1关: 股票信息存储 (从MySQL读取数据存入顺序表, 按位置输出)"""
    stocks, count = read_stock_file()
    print(f"读取到了{count}条股票信息")
    search_by_pos(stocks, count)


# ==================== 第2关: 股票信息删除 ====================

def delete_stock(stocks, indusname, stock_count):
    """删除指定股票简称的股票信息"""
    pos = -1
    for i in range(1, stock_count + 1):
        if stocks[i].indusname == indusname:
            pos = i
            break

    if pos == -1:
        print(f"未找到股票代码为{indusname}的股票")
        return stock_count

    temp = stocks[pos]  # 保存要删除的股票信息
    for i in range(pos, stock_count):
        stocks[i] = stocks[i + 1]
    stocks[stock_count] = None
    stock_count -= 1
    print(f"股票{indusname}已删除")

    save_stock_file(stocks, stock_count)
    # 输出被删除股票的全部信息(任务书要求)
    print("\n======删除股票信息======")
    print(f"股票代码:{temp.stockcode}")
    print(f"股票简称:{temp.indusname}")
    print(f"英文名称:{temp.sname}")
    print(f"行业编码:{temp.briname}")
    print(f"一级分类:{temp.pricat}")
    print(f"二级分类:{temp.seccat}")
    print(f"上市交易所:{temp.listexchange}")
    print(f"公司全称:{temp.wholecompany}")
    print(f"上市日期:{temp.launchdate}")
    print(f"省份:{temp.provin}")
    print(f"城市:{temp.city}")
    print(f"法人:{temp.legalper}")
    print(f"地址:{temp.addr}")
    print(f"网址:{temp.url}")
    print(f"邮箱:{temp.email}")
    print(f"电话:{temp.calling}")
    print(f"主营业务:{temp.mainbus}")
    print(f"经营范围:{temp.scopeofbus}")
    return stock_count


def level2():
    """第2关: 股票信息删除"""
    stocks, count = read_stock_file()
    print(f"读取到了{count}条股票信息")
    name = input("请输入要删除的股票简称:").strip()
    count = delete_stock(stocks, name, count)


# ==================== 第3关: 股票信息修改 ====================

def revise_stock(stocks, target_name, new_name, stock_count):
    """修改指定股票简称的公司的公司全称(机构名称)"""
    pos = -1
    for i in range(1, stock_count + 1):
        if stocks[i].indusname == target_name:
            pos = i
            break

    if pos == -1:
        print(f"未找到股票代码为{target_name}的股票")
        return

    stocks[pos].wholecompany = new_name
    print(f"股票{target_name}已修改为{new_name}")
    save_stock_file(stocks, stock_count)
    # 输出修改后结点的全部信息
    s = stocks[pos]
    print("\n======修改后股票信息======")
    print(f"股票代码:{s.stockcode}")
    print(f"股票简称:{s.indusname}")
    print(f"英文名称:{s.sname}")
    print(f"行业编码:{s.briname}")
    print(f"一级分类:{s.pricat}")
    print(f"二级分类:{s.seccat}")
    print(f"上市交易所:{s.listexchange}")
    print(f"公司全称:{s.wholecompany}")
    print(f"上市日期:{s.launchdate}")
    print(f"省份:{s.provin}")
    print(f"城市:{s.city}")
    print(f"法人:{s.legalper}")
    print(f"地址:{s.addr}")
    print(f"网址:{s.url}")
    print(f"邮箱:{s.email}")
    print(f"电话:{s.calling}")
    print(f"主营业务:{s.mainbus}")
    print(f"经营范围:{s.scopeofbus}")


def level3():
    """第3关: 股票信息修改"""
    stocks, count = read_stock_file()
    print(f"读取到了{count}条股票信息")
    target = input("请输入所要修改的股票简称:").strip()
    new_name = input("请输入修改后的公司全称(机构名称):").strip()
    revise_stock(stocks, target, new_name, count)


if __name__ == "__main__":
    choice = input("请输入关卡(1-3):").strip()
    if choice == "1":
        level1()
    elif choice == "2":
        level2()
    elif choice == "3":
        level3()
