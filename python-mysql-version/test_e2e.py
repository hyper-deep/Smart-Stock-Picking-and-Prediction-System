# -*- coding: utf-8 -*-
"""端到端回归测试: 依次调用1-20关并捕获结果(测试各关卡函数可直接运行)"""
import sys
import io

sys.path.insert(0, r"D:\creative\Smart Stock Picking and Prediction System(python+mysql)")

from unittest.mock import patch
import stock_maintain as m
import stock_seek as s
import stock_statistical as st
import stock_choice as c
import stock_prediction as p


def run_with_input(func, inputs):
    """用给定输入序列运行关卡函数, 返回(返回值, 输出文本)"""
    out = io.StringIO()
    with patch("builtins.input", side_effect=inputs), patch("sys.stdout", out):
        func()
    return out.getvalue()


def main():
    results = {}

    # 1: 位置3 = 宁德时代
    results["L1"] = run_with_input(m.level1, ["3"])
    assert "宁德时代" in results["L1"], results["L1"]

    # 2: 删除 万科发展
    results["L2"] = run_with_input(m.level2, ["万科发展"])
    assert "万科发展已删除" in results["L2"], results["L2"]

    # 3: 修改 平安银行
    results["L3"] = run_with_input(m.level3, ["平安银行", "新名称测试"])
    assert "已修改" in results["L3"], results["L3"]

    # 4: 折半查找
    results["L4"] = run_with_input(s.level4, ["Kweichow Moutai"])
    assert "查找成功" in results["L4"], results["L4"]

    # 5: BST
    results["L5"] = run_with_input(s.level5, ["Kweichow Moutai"])
    assert "原始序列BST" in results["L5"] and "排序后BST" in results["L5"]

    # 6: 字典树前缀
    results["L6"] = run_with_input(s.level6, ["CAT"])
    assert "共找到" in results["L6"], results["L6"]

    # 7: 开放地址散列
    results["L7"] = run_with_input(s.level7, ["CATL"])
    assert "查找成功" in results["L7"], results["L7"]

    # 8: 链地址散列
    results["L8"] = run_with_input(s.level8, ["CATL"])
    assert "查找成功" in results["L8"], results["L8"]

    # 9: 链表涨跌额
    results["L9"] = run_with_input(st.level9, ["C39", "2026-07-22"])
    assert "涨跌额排序" in results["L9"], results["L9"]

    # 10: 并查集换手率
    results["L10"] = run_with_input(st.level10, ["C39", "2026-07-22"])
    assert "换手率之和" in results["L10"], results["L10"]

    # 11: 堆排序Top3
    results["L11"] = run_with_input(st.level11, ["2026-07-22", "C39"])
    assert "Top3" in results["L11"], results["L11"]

    # 12: 归并排序成交量
    results["L12"] = run_with_input(st.level12, ["C39"])
    assert "汇总统计" in results["L12"], results["L12"]

    # 13: 决策树
    results["L13"] = run_with_input(st.level13, ["计算机、通信和其他电子设备制造业", ">0", ">1000万股", "海康威视"])
    assert "决策树构建完成" in results["L13"], results["L13"]

    # 14: BF实体识别
    results["L14"] = run_with_input(c.level14, ["****隶属于", "乐鑫科技隶属于乐鑫信息科技股份有限公司"])
    assert "乐鑫科技" in results["L14"], results["L14"]

    # 15: BF关系抽取
    results["L15"] = run_with_input(c.level15, ["同惠电子", "电子测量仪器的研发、生产与销售", "同惠电子专注于电子测量仪器的研发、生产与销售"])
    assert "主营业务为" in results["L15"], results["L15"]

    # 16: 邻接表知识图谱
    results["L16"] = run_with_input(c.level16, [])
    assert "知识图谱构建完成" in results["L16"], results["L16"]

    # 17: 图谱推荐(自动构建)
    results["L17"] = run_with_input(c.level17, ["C39"])
    assert "关联到" in results["L17"], results["L17"]

    # 18: 皮尔逊
    results["L18"] = run_with_input(p.level18, ["cn_600519"])
    assert "前3个指标" in results["L18"], results["L18"]

    # 19: 朴素贝叶斯
    results["L19"] = run_with_input(p.level19, ["cn_600519"])
    assert "预测结论" in results["L19"], results["L19"]

    # 20: KNN
    results["L20"] = run_with_input(p.level20, ["cn_600519"])
    assert "预测次日涨跌幅" in results["L20"], results["L20"]

    # 汇总输出(只显示关键行)
    print("=" * 60)
    print("端到端回归测试结果(1-20关全部通过)")
    print("=" * 60)
    for k in sorted(results.keys()):
        text = results[k]
        lines = [ln.strip() for ln in text.splitlines() if ln.strip()]
        key_lines = [ln for ln in lines if any(w in ln for w in ("成功", "完成", "删除", "修改", "Top", "换手率之和", "汇总", "关联到", "预测", "前3", "共找到", "实体", "三元组"))]
        print(f"\n[{k}] {key_lines[0] if key_lines else lines[0][:60]}")
        for ln in key_lines[1:3]:
            print(f"     {ln[:90]}")


if __name__ == "__main__":
    main()
    print("\n全部端到端回归测试通过!")
