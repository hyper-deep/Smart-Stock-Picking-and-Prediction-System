# -*- coding: utf-8 -*-
"""
股票预测(第18-20关): 皮尔逊相关系数 / 朴素贝叶斯 / KNN
参考C实现: stock_prediction.c
数据来源: MySQL (stock_information 库: stock_daily 表)
"""
import sys
import math

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

from db import get_conn

INDICATOR_COUNT = 5

# 从MySQL读取指定股票的日交易数据
def load_daily_from_db(stock_code):
    """返回 list[dict] 按日期升序: 日期/开盘/最高/最低/成交量/成交额/换手率/涨跌幅"""
    conn = get_conn()
    cur = conn.cursor()
    try:
        cur.execute("""
            SELECT trade_date, open_price, close_price, highest, lowest,
                   volume, turnover, change_rate, rise_fall_rate, rise_fall_amount
            FROM stock_daily WHERE stock_code=%s ORDER BY trade_date
        """, (stock_code,))
        rows = []
        for r in cur.fetchall():
            rows.append({
                "date": r[0], "open": float(r[1]), "close": float(r[2]),
                "high": float(r[3]), "low": float(r[4]), "volume": float(r[5]),
                "turnover": float(r[6]), "changerate": float(r[7]),
                "risefallrate": float(r[8]), "risefallamount": float(r[9]),
            })
        return rows
    finally:
        cur.close()
        conn.close()


def resolve_stock_input(user_input):
    """
    解析输入: 支持股票代码(cn_600519) 或 文件路径(detail/cn_600519.txt)
    返回数据行 list[dict]; 失败返回 None
    """
    code = user_input.strip()
    # 兼容文件路径输入: 提取其中的股票代码
    if "/" in code or "\\" in code or code.endswith(".txt"):
        # 取文件名去掉.txt后缀
        import os
        code = os.path.basename(code)
        if code.endswith(".txt"):
            code = code[:-4]
    rows = load_daily_from_db(code)
    if not rows:
        print(f"未找到股票代码[{user_input}]的行情数据, 或文件无法读取!")
        return None
    return rows


# ==================== 第18关 基于皮尔逊相关系数的股票指标相关性分析 ====================

def pearson(x, y, n):
    """计算皮尔逊相关系数 r = [n*Σxy - Σx*Σy] / sqrt([n*Σx²-(Σx)²]*[n*Σy²-(Σy)²])"""
    if n <= 0:
        return 0.0
    sx = sy = sxy = sxx = syy = 0.0
    for i in range(n):
        sx += x[i]
        sy += y[i]
        sxy += x[i] * y[i]
        sxx += x[i] * x[i]
        syy += y[i] * y[i]
    numerator = n * sxy - sx * sy
    denom = (n * sxx - sx * sx) * (n * syy - sy * sy)
    if denom <= 0.0:
        return 0.0
    return numerator / math.sqrt(denom)


def level18():
    """第18关: 皮尔逊相关系数指标重要性分析"""
    user_input = input("请输入股票详细数据文件路径或股票代码(如 detail/cn_600519.txt):").strip()
    if not user_input:
        print("输入不能为空!")
        return
    rows = resolve_stock_input(user_input)
    if rows is None:
        return
    if len(rows) < 2:
        print("有效交易数据不足(至少需要2行), 无法进行相关性分析!")
        return

    n = len(rows) - 1  # 有效样本量 = 总行数 - 1

    # 第t日的5个指标与第t+1日的涨跌标签对齐
    names = ["涨跌幅", "成交量", "振幅", "换手率", "成交额"]
    x = [[0.0] * n for _ in range(INDICATOR_COUNT)]
    yy = [0.0] * n
    for i in range(n):
        r = rows[i]
        x[0][i] = r["risefallrate"]                       # 涨跌幅(直接读取)
        x[1][i] = r["volume"] / 10000.0                    # 成交量: 股 -> 万股
        x[2][i] = (r["high"] - r["low"]) / r["open"] * 100.0 if r["open"] > 0 else 0.0  # 振幅
        x[3][i] = r["changerate"]                          # 换手率(直接读取)
        x[4][i] = r["turnover"] / 100000000.0              # 成交额: 元 -> 亿元
        yy[i] = 1.0 if rows[i + 1]["risefallrate"] > 0 else 0.0  # 次日涨跌标签

    # 计算各指标与标签的皮尔逊相关系数并取绝对值
    inds = []
    for k in range(INDICATOR_COUNT):
        r = pearson(x[k], yy, n)
        inds.append((names[k], abs(r)))

    # 按相关系数绝对值降序排序(选择排序)
    for a in range(INDICATOR_COUNT - 1):
        maxi = a
        for b in range(a + 1, INDICATOR_COUNT):
            if inds[b][1] > inds[maxi][1]:
                maxi = b
        if maxi != a:
            inds[a], inds[maxi] = inds[maxi], inds[a]

    # 输出: 降序排序结果
    print("\n各指标与次日涨跌的皮尔逊相关系数(绝对值)降序:")
    for k, (name, v) in enumerate(inds, 1):
        print(f"{k}. {name}: {v:.4f}")

    # 输出: 相关性最强的前3个指标
    top3 = [name for name, _ in inds[:3]]
    print(f"\n相关性最强的前3个指标: {'、'.join(top3)}")


# ==================== 第19关 基于朴素贝叶斯分类器的股票涨跌预测(选做) ====================

NB_FEATURE_COUNT = 3
NB_CLASS_COUNT = 2


def gaussian_logpdf(x, mu, var):
    """高斯分布对数概率密度: log f(x; μ, σ²)"""
    if var <= 0.0:
        var = 1e-9
    diff = x - mu
    return (-0.5 * math.log(2.0 * math.pi)
            - 0.5 * math.log(var)
            - (diff * diff) / (2.0 * var))


def level19():
    """第19关(选做): 朴素贝叶斯涨跌预测(用图结构存储高斯参数)"""
    user_input = input("请输入股票详细数据文件路径或股票代码(如 detail/cn_600519.txt):").strip()
    if not user_input:
        print("输入不能为空!")
        return
    rows = resolve_stock_input(user_input)
    if rows is None:
        return
    if len(rows) < 2:
        print("有效交易数据不足(至少需要2行), 无法进行预测!")
        return

    # 计算3个关键指标(涨跌幅/振幅/成交额)
    feat = {"涨跌幅": [], "振幅": [], "成交额": []}
    for r in rows:
        feat["涨跌幅"].append(r["risefallrate"])
        amp = (r["high"] - r["low"]) / r["open"] * 100.0 if r["open"] > 0 else 0.0
        feat["振幅"].append(amp)
        feat["成交额"].append(r["turnover"] / 100000000.0)

    train_n = len(rows) - 1  # 训练集: 除最新一条外的所有交易日
    pred_feat = [feat["涨跌幅"][-1], feat["振幅"][-1], feat["成交额"][-1]]  # 待预测样本

    # 1. 训练: 计算先验概率与各类别下的高斯参数(μ, σ²)
    cnt = [0, 0]
    for t in range(train_n):
        y = 1 if rows[t + 1]["risefallrate"] > 0 else 0
        cnt[y] += 1
    prior = [cnt[0] / train_n if train_n > 0 else 0.0,
             cnt[1] / train_n if train_n > 0 else 0.0]

    # 图结构: edges[f][c] = (mu, var), 特征->标签的条件概率参数
    edges = {}
    feat_names = ["涨跌幅", "振幅", "成交额"]
    for f, fname in enumerate(feat_names):
        for c in range(NB_CLASS_COUNT):
            vals = [feat[fname][t] for t in range(train_n)
                    if (1 if rows[t + 1]["risefallrate"] > 0 else 0) == c]
            mu = sum(vals) / cnt[c] if cnt[c] > 0 else 0.0
            var = sum((v - mu) ** 2 for v in vals) / cnt[c] if cnt[c] > 0 else 0.0
            edges[(f, c)] = (mu, var)

    # 2. 预测: 计算两类对数后验概率
    score = [0.0, 0.0]
    for c in range(NB_CLASS_COUNT):
        if cnt[c] == 0:
            score[c] = -1e300
            continue
        score[c] = math.log(prior[c])
        for f in range(NB_FEATURE_COUNT):
            mu, var = edges[(f, c)]
            score[c] += gaussian_logpdf(pred_feat[f], mu, var)
    predict = 1 if score[1] >= score[0] else 0

    # 3. 输出训练结果与预测结论
    print("\n===== 朴素贝叶斯训练结果(图结构) =====")
    print(f"训练样本数: {train_n} (涨:{cnt[1]}, 跌:{cnt[0]})")
    print(f"先验概率: P(涨)={prior[1]:.4f}, P(跌)={prior[0]:.4f}")
    print("类条件高斯参数(μ, σ²):")
    for f, fname in enumerate(feat_names):
        for c, label in enumerate(["跌", "涨"]):
            mu, var = edges[(f, c)]
            print(f"  {fname} | {label}: μ={mu:.4f}, σ²={var:.4f}")
    print(f"\n待预测样本(最新交易日): 涨跌幅={pred_feat[0]:.4f}, 振幅={pred_feat[1]:.4f}, 成交额={pred_feat[2]:.4f}")
    print(f"涨类对数后验概率: {score[1]:.4f}")
    print(f"跌类对数后验概率: {score[0]:.4f}")
    print(f"\n预测结论: 次日为{'涨' if predict == 1 else '跌'}")


# ==================== 第20关 基于K近邻算法的股票涨跌预测(选做) ====================

KNN_K = 3
KNN_FEATURE = 3
KNN_NAMES = ["涨跌幅", "振幅", "成交额"]


class KNNNode:
    """有序链表结点: 距离 + 次日涨跌幅数值"""
    __slots__ = ("dist", "value", "next")

    def __init__(self, dist, value):
        self.dist = dist
        self.value = value
        self.next = None


def knn_insert(head, dist, value, cnt):
    """按距离升序插入有序链表; 若长度超过K则剔除链表末尾(距离最远)的结点"""
    node = KNNNode(dist, value)
    if head is None or dist < head.dist:
        node.next = head
        head = node
    else:
        p = head
        while p.next is not None and p.next.dist <= dist:
            p = p.next
        node.next = p.next
        p.next = node
    cnt[0] += 1

    if cnt[0] > KNN_K:
        if head.next is None:
            head = None
            cnt[0] = 0
        else:
            q = head
            while q.next.next is not None:
                q = q.next
            q.next = None
            cnt[0] -= 1
    return head


def level20():
    """第20关(选做): KNN回归预测次日涨跌幅数值"""
    user_input = input("请输入股票详细数据文件路径或股票代码(如 detail/cn_600519.txt):").strip()
    if not user_input:
        print("输入不能为空!")
        return
    rows = resolve_stock_input(user_input)
    if rows is None:
        return
    if len(rows) < 2:
        print("有效交易数据不足(至少需要2行), 无法进行预测!")
        return

    # 3个关键指标
    feat = {"涨跌幅": [], "振幅": [], "成交额": []}
    for r in rows:
        feat["涨跌幅"].append(r["risefallrate"])
        amp = (r["high"] - r["low"]) / r["open"] * 100.0 if r["open"] > 0 else 0.0
        feat["振幅"].append(amp)
        feat["成交额"].append(r["turnover"] / 100000000.0)

    target_idx = len(rows) - 1  # 待预测目标: 最新交易日
    target = [feat[n][target_idx] for n in KNN_NAMES]
    hist_n = len(rows) - 1  # 历史样本数

    # 1. Min-Max归一化: 基于历史样本求各特征的最小值/最大值
    fmin, fmax = [], []
    for fname in KNN_NAMES:
        vals = feat[fname][:hist_n]
        fmin.append(min(vals))
        fmax.append(max(vals))

    # 归一化目标向量(使用历史样本的同一组min/max)
    tnorm = []
    for f in range(KNN_FEATURE):
        tnorm.append((target[f] - fmin[f]) / (fmax[f] - fmin[f]) if fmax[f] > fmin[f] else 0.0)

    # 2-3. 计算各历史样本与目标的欧氏距离, 用有序链表动态维护K个最近邻
    head = None
    cnt = [0]
    for t in range(hist_n):
        hnorm = []
        for f in range(KNN_FEATURE):
            hnorm.append((feat[KNN_NAMES[f]][t] - fmin[f]) / (fmax[f] - fmin[f])
                         if fmax[f] > fmin[f] else 0.0)
        dist = math.sqrt(sum((tnorm[f] - hnorm[f]) ** 2 for f in range(KNN_FEATURE)))
        next_val = rows[t + 1]["risefallrate"]  # 该历史样本的次日涨跌幅数值
        head = knn_insert(head, dist, next_val, cnt)

    # 4. 预测 = K个近邻次日涨跌幅的算术平均
    total = 0.0
    count = 0
    p = head
    neighbors = []
    while p is not None:
        total += p.value
        count += 1
        neighbors.append((p.dist, p.value))
        p = p.next
    prediction = total / count if count > 0 else 0.0

    # 输出
    print(f"\n===== K近邻(K={KNN_K})回归预测 =====")
    print(f"历史样本数: {hist_n}")
    print(f"目标日(最新交易日)特征: 涨跌幅={target[0]:.4f}, 振幅={target[1]:.4f}, 成交额={target[2]:.4f}")
    print("Min-Max归一化参数(基于历史样本):")
    for f in range(KNN_FEATURE):
        print(f"  {KNN_NAMES[f]}: min={fmin[f]:.4f}, max={fmax[f]:.4f}")
    print(f"距离最近的{count}个近邻(距离升序):")
    for d, v in neighbors:
        print(f"  距离={d:.4f}, 次日涨跌幅={v:.4f}%")
    print(f"\n预测次日涨跌幅: {prediction:.4f}%")


if __name__ == "__main__":
    choice = input("请输入关卡(18-20):").strip()
    if choice == "18":
        level18()
    elif choice == "19":
        level19()
    elif choice == "20":
        level20()
