# -*- coding: utf-8 -*-
"""数据库配置与初始化：创建 stock_information 库及 1-8 关所需的表，并从 txt 导入数据。"""
import pymysql

DB_CONFIG = dict(host="localhost", user="root", password="1433223aA!", charset="utf8mb4")
DB_NAME = "stock_information"

# 数据源目录（任务书要求的三个文本文件与 detail 文件夹）
SRC_DIR = r"D:\creative\Smart Stock Picking and Prediction System"


def get_conn(db=DB_NAME):
    cfg = dict(DB_CONFIG)
    if db:
        cfg["database"] = db
    return pymysql.connect(**cfg)


def init_database():
    conn = get_conn(db=None)
    cur = conn.cursor()
    cur.execute(f"CREATE DATABASE IF NOT EXISTS `{DB_NAME}` DEFAULT CHARSET utf8mb4")
    conn.commit()
    cur.close()
    conn.close()
    print(f"数据库 {DB_NAME} 就绪")


def create_tables():
    conn = get_conn()
    cur = conn.cursor()
    cur.execute("""
        CREATE TABLE IF NOT EXISTS stocks (
            id INT AUTO_INCREMENT PRIMARY KEY,
            stock_code VARCHAR(20) NOT NULL UNIQUE COMMENT '股票代码',
            stock_name VARCHAR(50) NOT NULL COMMENT '股票简称',
            en_name VARCHAR(100) COMMENT '英文名',
            industry_code VARCHAR(20) COMMENT '行业编码',
            industry_l1 VARCHAR(50) COMMENT '一级分类',
            industry_l2 VARCHAR(100) COMMENT '二级分类',
            exchange VARCHAR(50) COMMENT '上市交易所',
            company_name VARCHAR(100) COMMENT '公司全称',
            launch_date VARCHAR(20) COMMENT '上市日期',
            province VARCHAR(50) COMMENT '省份',
            city VARCHAR(50) COMMENT '城市',
            legal_person VARCHAR(50) COMMENT '法人',
            address VARCHAR(200) COMMENT '地址',
            url VARCHAR(300) COMMENT '网址',
            email VARCHAR(200) COMMENT '邮箱',
            phone VARCHAR(50) COMMENT '电话',
            main_business VARCHAR(700) COMMENT '主营业务',
            business_scope VARCHAR(700) COMMENT '经营范围'
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='股票基本信息(stocks.txt)'
    """)
    cur.execute("""
        CREATE TABLE IF NOT EXISTS stock_daily (
            id INT AUTO_INCREMENT PRIMARY KEY,
            stock_code VARCHAR(20) NOT NULL COMMENT '股票代码',
            trade_date VARCHAR(20) NOT NULL COMMENT '交易日期',
            open_price VARCHAR(30) COMMENT '开盘价',
            close_price VARCHAR(30) COMMENT '收盘价',
            highest VARCHAR(30) COMMENT '最高价',
            lowest VARCHAR(30) COMMENT '最低价',
            volume VARCHAR(30) COMMENT '成交量',
            turnover VARCHAR(30) COMMENT '成交额',
            change_rate VARCHAR(30) COMMENT '换手率',
            rise_fall_rate VARCHAR(30) COMMENT '涨跌幅',
            rise_fall_amount VARCHAR(30) COMMENT '涨跌额',
            UNIQUE KEY uk_code_date (stock_code, trade_date)
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='每日行情(detail/*.txt)'
    """)
    cur.execute("""
        CREATE TABLE IF NOT EXISTS entities (
            id INT AUTO_INCREMENT PRIMARY KEY,
            entity_name VARCHAR(300) NOT NULL COMMENT '实体内容',
            entity_type VARCHAR(50) NOT NULL COMMENT '实体类别'
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='知识图谱实体(entity.txt)'
    """)
    cur.execute("""
        CREATE TABLE IF NOT EXISTS relations (
            id INT AUTO_INCREMENT PRIMARY KEY,
            head_entity VARCHAR(300) NOT NULL COMMENT '头实体',
            relation VARCHAR(50) NOT NULL COMMENT '关系',
            tail_entity VARCHAR(300) NOT NULL COMMENT '尾实体'
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='知识图谱三元组(relation.txt)'
    """)
    conn.commit()
    cur.close()
    conn.close()
    print("数据表就绪: stocks / stock_daily / entities / relations")


def import_stocks(cur):
    """读取 stocks.txt（UTF-8，'|' 分隔，首行为表头）导入 stocks 表。"""
    path = f"{SRC_DIR}\\stocks.txt"
    rows = []
    with open(path, "r", encoding="utf-8") as f:
        lines = [ln.strip() for ln in f if ln.strip()]
    # 跳过表头行
    data_lines = lines[1:]
    for ln in data_lines:
        parts = ln.split("|")
        if len(parts) != 18:
            continue
        rows.append(tuple(parts))
    sql = """INSERT IGNORE INTO stocks
        (stock_code, stock_name, en_name, industry_code, industry_l1, industry_l2,
         exchange, company_name, launch_date, province, city, legal_person,
         address, url, email, phone, main_business, business_scope)
        VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    cur.executemany(sql, rows)
    return len(rows)


def import_stock_daily(cur):
    """读取 detail 文件夹下全部股票行情文件导入 stock_daily 表。"""
    import os
    detail_dir = f"{SRC_DIR}\\detail"
    if not os.path.isdir(detail_dir):
        print(f"警告: 找不到目录 {detail_dir}")
        return 0
    rows = []
    for fn in sorted(os.listdir(detail_dir)):
        if not fn.endswith(".txt"):
            continue
        code = fn[:-4]  # 文件名即股票代码
        with open(os.path.join(detail_dir, fn), "r", encoding="utf-8") as f:
            lines = [ln.strip() for ln in f if ln.strip()]
        data_lines = lines[1:]  # 跳过表头
        for ln in data_lines:
            parts = ln.split("|")
            if len(parts) != 10:
                continue
            rows.append((code, *parts))
    sql = """INSERT IGNORE INTO stock_daily
        (stock_code, trade_date, open_price, close_price, highest, lowest,
         volume, turnover, change_rate, rise_fall_rate, rise_fall_amount)
        VALUES (%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)"""
    cur.executemany(sql, rows)
    return len(rows)


def import_entities(cur):
    path = f"{SRC_DIR}\\entity.txt"
    rows = []
    with open(path, "r", encoding="utf-8") as f:
        for ln in f:
            ln = ln.strip()
            if not ln:
                continue
            parts = ln.split("|")
            if len(parts) == 2:
                rows.append((parts[0], parts[1]))
    sql = "INSERT IGNORE INTO entities (entity_name, entity_type) VALUES (%s,%s)"
    cur.executemany(sql, rows)
    return len(rows)


def import_relations(cur):
    path = f"{SRC_DIR}\\relation.txt"
    rows = []
    with open(path, "r", encoding="utf-8") as f:
        for ln in f:
            ln = ln.strip()
            if not ln:
                continue
            parts = ln.split("|")
            if len(parts) == 3:
                rows.append((parts[0], parts[1], parts[2]))
    sql = "INSERT IGNORE INTO relations (head_entity, relation, tail_entity) VALUES (%s,%s,%s)"
    cur.executemany(sql, rows)
    return len(rows)


def run():
    init_database()
    create_tables()
    conn = get_conn()
    cur = conn.cursor()
    # 清空后重新导入，保证幂等
    for t in ("stocks", "stock_daily", "entities", "relations"):
        cur.execute(f"DELETE FROM `{t}`")
    n1 = import_stocks(cur)
    n2 = import_stock_daily(cur)
    n3 = import_entities(cur)
    n4 = import_relations(cur)
    conn.commit()
    print(f"导入完成: stocks={n1}, stock_daily={n2}, entities={n3}, relations={n4}")
    cur.close()
    conn.close()


if __name__ == "__main__":
    run()
