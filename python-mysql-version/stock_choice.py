# -*- coding: utf-8 -*-
"""
股票信息推荐(第14-17关): BF实体识别/关系抽取/邻接表知识图谱/图谱推荐
参考C实现: stock_choice.c
数据来源: MySQL (stock_information 库: entities / relations 表)
"""
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

from db import get_conn

# ---------- 工具函数 ----------

def is_cn_char(ch):
    """判断一个字符是否中文字符(基本区U+4E00-U+9FFF)"""
    return 0x4E00 <= ord(ch) <= 0x9FFF


# ==================== 第14关 基于规则的股票信息实体识别(BF暴力匹配) ====================

def level14():
    """第14关: 基于规则的实体识别。规则中 * 表示任意一个中文字符。"""
    rule = input("请输入匹配规则(含*通配符):").strip()
    text = input("请输入非结构化文本:").strip()

    if "*" not in rule:
        print("规则中未包含通配符*!")
        return

    # 解析规则: 前缀字面字符、*数量、后缀字面字符
    star_count = rule.count("*")
    prefix = rule[:rule.index("*")]
    suffix = rule[rule.rindex("*") + 1:]

    prefix_chars = list(prefix)
    suffix_chars = list(suffix)
    text_chars = list(text)

    found = []
    seen = set()

    # BF暴力匹配: 依次尝试每个起始位置, 规则项数与文本对齐
    n = len(prefix_chars) + star_count + len(suffix_chars)
    for s in range(0, len(text_chars) - n + 1):
        ok = True
        for j in range(len(prefix_chars)):
            if text_chars[s + j] != prefix_chars[j]:
                ok = False
                break
        if not ok:
            continue
        # *段必须全为中文字符
        star_start = s + len(prefix_chars)
        for j in range(star_count):
            if not is_cn_char(text_chars[star_start + j]):
                ok = False
                break
        if not ok:
            continue
        # 后缀
        suf_start = star_start + star_count
        for j in range(len(suffix_chars)):
            if text_chars[suf_start + j] != suffix_chars[j]:
                ok = False
                break
        if not ok:
            continue
        entity = "".join(text_chars[star_start:star_start + star_count])
        if entity not in seen:
            seen.add(entity)
            found.append(entity)

    if len(found) == 0:
        print("\n未匹配到符合规则的实体")
    else:
        print(f"\n共匹配到{len(found)}个实体:")
        for i, e in enumerate(found, 1):
            print(f"第{i}个实体:{e}")


# ==================== 第15关 基于规则的股票信息关系抽取(BF算法) ====================

def bf_find(text, sub):
    """BF暴力匹配: 返回sub在text中首次出现的起始位置, 未找到返回-1"""
    n, m = len(text), len(sub)
    if m == 0:
        return -1
    for i in range(n - m + 1):
        if text[i:i + m] == sub:
            return i
    return -1


# 预定义关系规则集(字母A代表头实体, B代表尾实体)
RELATION_RULES = [
    ("属于行业大类", "A行业类别为B", True),
    ("属于行业大类", "A属于B", True),
    ("所属机构", "A所属机构为B", True),
    ("所属机构", "A的公司是B", True),
    ("所属机构", "A隶属B", True),
    ("具有注册地址", "A位于B", True),
    ("具有注册地址", "A坐落于\u201cB", True),
    ("具有注册地址", "总部在", False),
    ("主营业务为", "A包括B", True),
    ("主营业务为", "A专注于B", True),
    ("主营业务为", "A主营业务为B", True),
]


def level15():
    """第15关: 基于规则的关系抽取(输出三元组)"""
    entity1 = input("请输入实体1(头实体):").strip()
    entity2 = input("请输入实体2(尾实体):").strip()
    text = input("请输入非结构化文本:").strip()

    if not entity1 or not entity2:
        print("实体不能为空!")
        return

    # 1. BF算法在文本中定位实体1
    pos1 = bf_find(text, entity1)
    if pos1 < 0:
        print("文本中未找到实体1, 关系抽取失败!")
        return

    # 2. BF算法在实体1之后定位实体2
    search_start = pos1 + len(entity1)
    rel = bf_find(text[search_start:], entity2)
    if rel < 0:
        print("文本中未找到实体2(需位于实体1之后), 关系抽取失败!")
        return
    pos2 = search_start + rel

    # 3. 提取中间内容
    middle = text[pos1 + len(entity1):pos2]

    # 4. 构造待匹配串: A[中间内容]B
    built = "A" + middle + "B"

    # 5. 与预定义规则集逐字符全等比较
    matched_relation = None
    for relation, pattern, has_placeholder in RELATION_RULES:
        hit = (built == pattern) if has_placeholder else (middle == pattern)
        if hit:
            matched_relation = relation
            break

    # 6. 判定与输出
    if matched_relation:
        print(f"\n{entity1} - {matched_relation} - {entity2}")
    else:
        print("\n两实体之间不存在对应关系, 无法抽取有效关系")


# ==================== 第16关 基于邻接表的股票知识图谱构建 ====================

# 关系类型名称数组(下标对应关系类型)
RELATIONSHIP_NAMES = [
    "所属机构", "上市于", "具有行业编码", "属于行业大类", "属于行业二级类",
    "位于省份", "位于城市", "具有注册地址", "主营业务为",
]

# 实体类型名称数组(下标对应实体类型)
ENTITY_TYPE_NAMES = [
    "股票简称", "机构", "上市交易所", "行业编码", "行业大类",
    "行业二级类", "省份", "城市", "地址", "主营业务",
]

# 关系类型 -> 尾实体类型
TAIL_TYPE_OF_RELATIONSHIP = {
    0: 1,  # 所属机构 -> 机构
    1: 2,  # 上市于 -> 上市交易所
    2: 3,  # 具有行业编码 -> 行业编码
    3: 4,  # 属于行业大类 -> 行业大类
    4: 5,  # 属于行业二级类 -> 行业二级类
    5: 6,  # 位于省份 -> 省份
    6: 7,  # 位于城市 -> 城市
    7: 8,  # 具有注册地址 -> 地址
    8: 9,  # 主营业务为 -> 主营业务
}


class ArcNode:
    """边结点"""
    __slots__ = ("adjvex", "relationship", "nextarc")

    def __init__(self, adjvex, relationship):
        self.adjvex = adjvex
        self.relationship = relationship
        self.nextarc = None


class VNode:
    """顶点结点"""
    __slots__ = ("entity", "info", "firstarc")

    def __init__(self, entity, info):
        self.entity = entity
        self.info = info
        self.firstarc = None


class ALGraph:
    """邻接表图结构"""

    def __init__(self):
        self.vertices = []  # list[VNode]
        self.vexnum = 0
        self.arcnum = 0

    def find_vertex(self, info, entity_type):
        for i in range(self.vexnum):
            v = self.vertices[i]
            if v.entity == entity_type and v.info == info:
                return i
        return -1

    def add_vertex(self, info, entity_type):
        idx = self.find_vertex(info, entity_type)
        if idx >= 0:
            return idx
        self.vertices.append(VNode(entity_type, info))
        self.vexnum += 1
        return self.vexnum - 1

    def add_arc(self, frm, to, rel):
        """添加一条有向边 from -> to (头插法)"""
        if frm < 0 or to < 0:
            return
        node = ArcNode(to, rel)
        node.nextarc = self.vertices[frm].firstarc
        self.vertices[frm].firstarc = node
        self.arcnum += 1

    def print_adjacency_list(self):
        for i in range(self.vexnum):
            v = self.vertices[i]
            etype = ENTITY_TYPE_NAMES[v.entity] if 0 <= v.entity < len(ENTITY_TYPE_NAMES) else "未知类型"
            line = f"顶点{i} [{etype}] {v.info}"
            p = v.firstarc
            while p is not None:
                line += f"  ->( {RELATIONSHIP_NAMES[p.relationship]} ){self.vertices[p.adjvex].info}"
                p = p.nextarc
            print(line)


# 全局知识图谱, 供第17关直接调用
g_stock_graph = ALGraph()


def load_entities_from_db():
    """从MySQL读取实体: [(info, entity_type)]"""
    conn = get_conn()
    cur = conn.cursor()
    try:
        cur.execute("SELECT entity_name, entity_type FROM entities")
        rows = []
        for name, etype in cur.fetchall():
            if etype in ENTITY_TYPE_NAMES:
                rows.append((name, ENTITY_TYPE_NAMES.index(etype)))
        return rows
    finally:
        cur.close()
        conn.close()


def load_relations_from_db():
    """从MySQL读取三元组: [(head, relation, tail)]"""
    conn = get_conn()
    cur = conn.cursor()
    try:
        cur.execute("SELECT head_entity, relation, tail_entity FROM relations")
        return [(h, r, t) for h, r, t in cur.fetchall()]
    finally:
        cur.close()
        conn.close()


def build_knowledge_graph():
    """读取 entity/relation 数据, 构建双向邻接表知识图谱"""
    g = ALGraph()
    g.vexnum = 0
    g.arcnum = 0

    # 1. 读取实体, 初始化顶点
    entity_lines = 0
    for info, etype in load_entities_from_db():
        g.add_vertex(info, etype)
        entity_lines += 1
    print(f"已从 entity 数据读取 {entity_lines} 行实体, 当前顶点数={g.vexnum}")

    # 2. 读取三元组, 正向+反向双向建边
    relation_lines = 0
    for head_str, rel_name, tail_str in load_relations_from_db():
        if rel_name not in RELATIONSHIP_NAMES:
            continue
        rel = RELATIONSHIP_NAMES.index(rel_name)
        head = g.find_vertex(head_str, 0)
        if head < 0:
            head = g.add_vertex(head_str, 0)
        expected_tail_type = TAIL_TYPE_OF_RELATIONSHIP.get(rel, -1)
        tail = g.find_vertex(tail_str, expected_tail_type)
        if tail < 0:
            tail = g.add_vertex(tail_str, expected_tail_type)
        g.add_arc(head, tail, rel)   # 正向: 头实体 -> 尾实体
        g.add_arc(tail, head, rel)   # 反向: 尾实体 -> 头实体
        relation_lines += 1

    print(f"已从 relation 数据读取 {relation_lines} 行三元组(每条双向各建一条边)")
    print(f"知识图谱构建完成: 顶点数={g.vexnum}, 边数={g.arcnum}")
    g.print_adjacency_list()
    return g


def level16():
    """第16关: 基于邻接表的股票知识图谱构建"""
    global g_stock_graph
    g_stock_graph = build_knowledge_graph()


# ==================== 第17关 基于知识图谱的股票推荐 ====================

# 推荐输出顺序(关系类型下标, 与任务书规定的属性输出顺序一致)
RECOMMEND_ORDER = [8, 7, 6, 5, 4, 3, 2, 1, 0]
RECOMMEND_LABELS = {
    8: "主营业务", 7: "注册地址", 6: "所在城市", 5: "位于省份", 4: "行业二级类",
    3: "行业大类", 2: "行业编码", 1: "上市交易所", 0: "所属机构名称",
}


def get_arc_target(g, v, rel):
    """从顶点v出发, 沿邻接边查找指定关系类型rel指向的第一个顶点, 返回下标, 未找到返回-1"""
    p = g.vertices[v].firstarc
    while p is not None:
        if p.relationship == rel:
            return p.adjvex
        p = p.nextarc
    return -1


def level17():
    """第17关: 基于知识图谱的股票推荐"""
    global g_stock_graph
    g = g_stock_graph
    if g.vexnum == 0:
        print("知识图谱尚未构建, 正在自动构建...")
        g_stock_graph = build_knowledge_graph()
        g = g_stock_graph

    input_str = input("请输入股票相关信息(如行业编码、股票简称、机构名称等):").strip()
    if not input_str:
        print("输入不能为空!")
        return

    # 1. 节点定位: 在顶点数组中查找 info 与输入匹配的顶点
    start = -1
    for i in range(g.vexnum):
        if g.vertices[i].info == input_str:
            start = i
            break
    if start < 0:
        print(f"图谱中未找到与 \"{input_str}\" 匹配的顶点!")
        return

    # 2. 利用双向边收集关联的股票(股票简称类型)顶点
    stocks_idx = []
    if g.vertices[start].entity == 0:  # 0=股票简称: 起点本身即股票
        stocks_idx.append(start)
    else:
        p = g.vertices[start].firstarc
        while p is not None:
            if g.vertices[p.adjvex].entity == 0:  # 反向边指向的股票
                if p.adjvex not in stocks_idx:
                    stocks_idx.append(p.adjvex)
            p = p.nextarc

    if len(stocks_idx) == 0:
        print(f"未找到与 \"{input_str}\" 关联的股票信息!")
        return

    # 3. 信息聚合与输出: 每支股票按固定顺序输出属性, 缺失属性直接跳过
    print(f"\n输入 \"{input_str}\" 关联到 {len(stocks_idx)} 支股票, 推荐信息序列如下:")
    for i, s in enumerate(stocks_idx, 1):
        print(f"\n【第{i}支股票】{g.vertices[s].info}")
        for rel in RECOMMEND_ORDER:
            t = get_arc_target(g, s, rel)
            if t >= 0:
                print(f"{RECOMMEND_LABELS[rel]}: {g.vertices[t].info}")


if __name__ == "__main__":
    choice = input("请输入关卡(14-17):").strip()
    if choice == "14":
        level14()
    elif choice == "15":
        level15()
    elif choice == "16":
        level16()
    elif choice == "17":
        level17()
