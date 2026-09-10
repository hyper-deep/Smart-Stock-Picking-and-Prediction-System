# -*- coding: utf-8 -*-
"""
数据库访问模块
MySQL: stock_information 库
密码: 1433223aA!
"""
import pymysql

DB_CONFIG = dict(
    host="localhost",
    user="root",
    password="1433223aA!",
    database="stock_information",
    charset="utf8mb4",
)


def get_conn():
    return pymysql.connect(**DB_CONFIG)


class StockDetail:
    """股票每日行情 (对应C的stockdetail结构体)"""
    __slots__ = ("date", "openprice", "closeprice", "highest", "lowest",
                 "volume", "turnover", "changerate", "risefallrate", "risefallamount")

    def __init__(self, date="", openprice="", closeprice="", highest="", lowest="",
                 volume="", turnover="", changerate="", risefallrate="", risefallamount=""):
        self.date = date
        self.openprice = openprice
        self.closeprice = closeprice
        self.highest = highest
        self.lowest = lowest
        self.volume = volume
        self.turnover = turnover
        self.changerate = changerate
        self.risefallrate = risefallrate
        self.risefallamount = risefallamount


class Stock:
    """股票基本信息 (对应C的stock结构体)"""
    __slots__ = ("stockcode", "indusname", "sname", "briname", "pricat", "seccat",
                 "listexchange", "wholecompany", "launchdate", "provin", "city",
                 "legalper", "addr", "url", "email", "calling", "mainbus", "scopeofbus",
                 "detail", "detail_count")

    def __init__(self, stockcode="", indusname="", sname="", briname="", pricat="",
                 seccat="", listexchange="", wholecompany="", launchdate="", provin="",
                 city="", legalper="", addr="", url="", email="", calling="",
                 mainbus="", scopeofbus="", detail=None, detail_count=0):
        self.stockcode = stockcode
        self.indusname = indusname
        self.sname = sname
        self.briname = briname
        self.pricat = pricat
        self.seccat = seccat
        self.listexchange = listexchange
        self.wholecompany = wholecompany
        self.launchdate = launchdate
        self.provin = provin
        self.city = city
        self.legalper = legalper
        self.addr = addr
        self.url = url
        self.email = email
        self.calling = calling
        self.mainbus = mainbus
        self.scopeofbus = scopeofbus
        self.detail = detail if detail is not None else []
        self.detail_count = detail_count


def fetch_all_stocks_with_detail():
    """
    从MySQL读取全部股票基本信息和每日行情
    返回: list[Stock], 每支股票的detail为StockDetail列表, detail_count为天数
    """
    conn = get_conn()
    cur = conn.cursor()
    try:
        cur.execute("""
            SELECT stock_code, stock_name, en_name, industry_code, industry_l1,
                   industry_l2, exchange, company_name, launch_date, province,
                   city, legal_person, address, url, email, phone,
                   main_business, business_scope
            FROM stocks ORDER BY id
        """)
        stocks = []
        for r in cur.fetchall():
            s = Stock(
                stockcode=r[0], indusname=r[1], sname=r[2], briname=r[3],
                pricat=r[4], seccat=r[5], listexchange=r[6], wholecompany=r[7],
                launchdate=r[8], provin=r[9], city=r[10], legalper=r[11],
                addr=r[12], url=r[13], email=r[14], calling=r[15],
                mainbus=r[16], scopeofbus=r[17],
            )
            stocks.append(s)

        # 批量读取每日行情
        cur.execute("""
            SELECT stock_code, trade_date, open_price, close_price, highest, lowest,
                   volume, turnover, change_rate, rise_fall_rate, rise_fall_amount
            FROM stock_daily ORDER BY stock_code, trade_date
        """)
        from collections import defaultdict
        daily_map = defaultdict(list)
        for r in cur.fetchall():
            d = StockDetail(date=r[1], openprice=r[2], closeprice=r[3], highest=r[4],
                            lowest=r[5], volume=r[6], turnover=r[7], changerate=r[8],
                            risefallrate=r[9], risefallamount=r[10])
            daily_map[r[0]].append(d)

        for s in stocks:
            s.detail = daily_map.get(s.stockcode, [])
            s.detail_count = len(s.detail)

        return stocks
    finally:
        cur.close()
        conn.close()
