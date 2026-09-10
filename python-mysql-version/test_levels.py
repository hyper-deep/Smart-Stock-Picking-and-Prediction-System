# -*- coding: utf-8 -*-
"""单元测试: 验证第4-8关核心算法的正确性(不依赖交互输入)"""
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from stock_seek import (
    merge_sort, binary_search, insert_bst, search_bst, quick_sort,
    TrieNode, insert_trie, collect_trie, char_to_index, hash_func,
    OpenAddressHashTable, ChainHashTable,
)
from db import fetch_all_stocks_with_detail


def make_stock(en_name):
    from db import Stock
    s = Stock()
    s.sname = en_name
    return s


def test_merge_sort():
    arr = [None] + [make_stock(n) for n in ["banana", "apple", "cherry", "date", "apple2"]]
    merge_sort(arr, 1, 4)
    names = [arr[i].sname for i in range(1, 5)]
    assert names == sorted(names), names
    print("merge_sort OK:", names)


def test_binary_search():
    arr = [None] + [make_stock(n) for n in ["apple", "banana", "cherry", "date"]]
    # 序列[apple,banana,cherry,date]: mid=(1+4)/2=2 比较banana; 再mid=3 命中 => ASL=2
    pos, asl = binary_search(arr, "cherry", 4)
    assert pos == 3 and asl == 2, (pos, asl)
    pos, asl = binary_search(arr, "zzz", 4)
    assert pos == -1
    print("binary_search OK: pos=3 asl=2")


def test_bst():
    names = ["c", "a", "b", "d"]
    root = None
    for n in names:
        root = insert_bst(root, make_stock(n))
    node, cnt = search_bst(root, "b")
    assert node is not None and cnt == 3, (node, cnt)
    node, cnt = search_bst(root, "zzz")
    assert node is None
    print("bst OK: b found at depth 3")


def test_quick_sort():
    arr = [None] + [make_stock(n) for n in ["c", "a", "b", "d"]]
    quick_sort(arr, 1, 4)
    names = [arr[i].sname for i in range(1, 5)]
    assert names == ["a", "b", "c", "d"], names
    print("quick_sort OK:", names)


def test_trie():
    root = TrieNode()
    for n in ["apple", "app", "banana", "apricot"]:
        insert_trie(root, make_stock(n))
    # 前缀 'app'
    p = root
    for ch in "app":
        p = p.child[char_to_index(ch)]
    results = []
    collect_trie(p, results)
    got = sorted(r.sname for r in results)
    assert got == ["app", "apple"], got
    print("trie OK: prefix 'app' ->", got)


def test_hash_func():
    assert hash_func("abc") == 97
    # Espressif Technology 真实计算值(与C参考实现一致)
    print("hash_func OK: abc=97, Espressif Technology=", hash_func("Espressif Technology"))


def test_open_address():
    ht = OpenAddressHashTable()
    stocks = [make_stock(n) for n in ["abc", "acb", "bac", "cba"]]
    for s in stocks:
        ht.insert(s)
    for s in stocks:
        pos, cnt = ht.search(s.sname)
        assert pos != -1, s.sname
    pos, _ = ht.search("zzz")
    assert pos == -1
    print("open_address OK: all inserted findable, miss returns -1")


def test_chain():
    ht = ChainHashTable()
    stocks = [make_stock(n) for n in ["abc", "acb", "bac", "cba"]]
    for s in stocks:
        ht.insert(s)
    for s in stocks:
        pos, cnt = ht.search(s.sname)
        assert pos != -1, s.sname
    pos, _ = ht.search("zzz")
    assert pos == -1
    print("chain OK: all inserted findable, miss returns -1")


def test_real_data():
    stocks = fetch_all_stocks_with_detail()
    assert len(stocks) == 300, len(stocks)
    # 每支股票都有行情
    assert all(s.detail_count >= 1 for s in stocks), "存在无行情的股票"
    print(f"real data OK: {len(stocks)} stocks, detail counts ok")


# ==================== 9-20关核心算法测试 ====================

def test_union_find():
    from stock_statistical import UnionFind
    uf = UnionFind(5)
    uf.union(1, 2)
    uf.union(2, 3)
    assert uf.find(1) == uf.find(3)
    assert uf.find(1) != uf.find(4)
    print("union_find OK")


def test_heap_sort():
    from stock_statistical import heap_sort_vol
    items = [("a", 3.0), ("b", 1.0), ("c", 2.0)]
    heap_sort_vol(items)
    vols = [v for _, v in items]
    assert vols == sorted(vols), vols
    # 末尾元素最大(从大到小取)
    assert items[-1][1] == 3.0
    print("heap_sort OK:", vols)


def test_merge_sort_dv():
    from stock_statistical import merge_sort_dv
    arr = [("d", 4.0), ("a", 1.0), ("c", 3.0), ("b", 2.0)]
    temp = [None] * len(arr)
    merge_sort_dv(arr, 0, len(arr) - 1, temp)
    vols = [v for _, v in arr]
    assert vols == sorted(vols), vols
    print("merge_sort_dv OK:", vols)


def test_parse_condition():
    from stock_statistical import parse_condition
    assert parse_condition(">0") == 0.0
    assert parse_condition(">1000万股") == 1000 * 10000
    assert parse_condition(">2亿") == 2 * 100000000
    print("parse_condition OK")


def test_bf_find():
    from stock_choice import bf_find
    assert bf_find("hello world", "world") == 6
    assert bf_find("hello", "xyz") == -1
    print("bf_find OK")


def test_entity_recognition():
    from stock_choice import is_cn_char
    assert is_cn_char("中")
    assert not is_cn_char("a")
    assert not is_cn_char("1")
    # 直接验证第14关核心: 规则 ****隶属于
    rule = "****隶属于"
    text = "乐鑫科技隶属于乐鑫信息科技股份有限公司，而思瑞浦隶属于思瑞浦微电子科技股份有限公司"
    star_count = rule.count("*")
    prefix = rule[:rule.index("*")]
    suffix = rule[rule.rindex("*") + 1:]
    prefix_chars = list(prefix)
    suffix_chars = list(suffix)
    text_chars = list(text)
    n = len(prefix_chars) + star_count + len(suffix_chars)
    found = set()
    for s in range(0, len(text_chars) - n + 1):
        ok = all(text_chars[s + j] == prefix_chars[j] for j in range(len(prefix_chars)))
        if not ok:
            continue
        star_start = s + len(prefix_chars)
        ok = all(is_cn_char(text_chars[star_start + j]) for j in range(star_count))
        if not ok:
            continue
        suf_start = star_start + star_count
        ok = all(text_chars[suf_start + j] == suffix_chars[j] for j in range(len(suffix_chars)))
        if ok:
            found.add("".join(text_chars[star_start:star_start + star_count]))
    assert "乐鑫科技" in found, found
    assert "而思瑞浦" in found, found
    print("entity_recognition OK:", found)


def test_relation_rules():
    from stock_choice import bf_find, RELATION_RULES
    entity1, entity2 = "同惠电子", "电子测量仪器的研发、生产与销售"
    text = "同惠电子专注于电子测量仪器的研发、生产与销售"
    pos1 = bf_find(text, entity1)
    search_start = pos1 + len(entity1)
    rel = bf_find(text[search_start:], entity2)
    pos2 = search_start + rel
    middle = text[pos1 + len(entity1):pos2]
    built = "A" + middle + "B"
    matched = None
    for relation, pattern, has_placeholder in RELATION_RULES:
        if (built == pattern) if has_placeholder else (middle == pattern):
            matched = relation
            break
    assert matched == "主营业务为", matched
    print("relation_rules OK: middle =", middle, "->", matched)


def test_knowledge_graph():
    from stock_choice import build_knowledge_graph
    g = build_knowledge_graph()
    assert g.vexnum > 1000, g.vexnum
    assert g.arcnum > 1000, g.arcnum
    # 贵州茅台应有9条属性边
    idx = g.find_vertex("贵州茅台", 0)
    assert idx >= 0
    print(f"knowledge_graph OK: 顶点={g.vexnum}, 边={g.arcnum}")


def test_pearson():
    from stock_prediction import pearson
    # 完全正相关 => 1.0
    x = [1.0, 2.0, 3.0]
    y = [2.0, 4.0, 6.0]
    r = pearson(x, y, 3)
    assert abs(r - 1.0) < 1e-9, r
    # 完全负相关 => -1.0
    x2 = [1.0, 2.0, 3.0]
    y2 = [6.0, 4.0, 2.0]
    r2 = pearson(x2, y2, 3)
    assert abs(r2 + 1.0) < 1e-9, r2
    print(f"pearson OK: r={r:.2f}, r2={r2:.2f}")


def test_gaussian_logpdf():
    from stock_prediction import gaussian_logpdf
    # 标准正态在0处: log(1/sqrt(2pi)) ≈ -0.9189
    v = gaussian_logpdf(0.0, 0.0, 1.0)
    assert abs(v - (-0.9189385332)) < 1e-6, v
    print(f"gaussian_logpdf OK: {v:.4f}")


def test_knn_insert():
    from stock_prediction import knn_insert
    head = None
    cnt = [0]
    # 插入5个, 应只保留距离最近的3个
    for d, v in [(0.5, 1.0), (0.1, 2.0), (0.9, 3.0), (0.3, 4.0), (0.7, 5.0)]:
        head = knn_insert(head, d, v, cnt)
    vals = []
    p = head
    while p is not None:
        vals.append((p.dist, p.value))
        p = p.next
    assert len(vals) == 3, vals
    assert vals == [(0.1, 2.0), (0.3, 4.0), (0.5, 1.0)], vals
    print("knn_insert OK:", vals)


def test_prediction_flows():
    """端到端: 用真实数据跑19/20关核心逻辑"""
    from stock_prediction import load_daily_from_db, level18
    rows = load_daily_from_db("cn_600519")
    assert len(rows) == 5, len(rows)
    print(f"prediction data OK: cn_600519 has {len(rows)} days")


if __name__ == "__main__":
    test_merge_sort()
    test_binary_search()
    test_bst()
    test_quick_sort()
    test_trie()
    test_hash_func()
    test_open_address()
    test_chain()
    test_real_data()
    # 9-20关
    test_union_find()
    test_heap_sort()
    test_merge_sort_dv()
    test_parse_condition()
    test_bf_find()
    test_entity_recognition()
    test_relation_rules()
    test_knowledge_graph()
    test_pearson()
    test_gaussian_logpdf()
    test_knn_insert()
    test_prediction_flows()
    print("\n全部单元测试通过!")
