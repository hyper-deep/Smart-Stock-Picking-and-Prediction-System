# -*- coding: utf-8 -*-
"""
智能股票分析与预测系统 —— 控制台主菜单
西南财经大学《数据结构（课程设计）》第1-20关
技术栈: Python 3 + MySQL (stock_information 库)
"""
import sys

try:
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")
except Exception:
    pass

from stock_maintain import level1, level2, level3
from stock_seek import level4, level5, level6, level7, level8
from stock_statistical import level9, level10, level11, level12, level13
from stock_choice import level14, level15, level16, level17
from stock_prediction import level18, level19, level20


def print_menu():
    print("=" * 56)
    print("     智能股票分析与预测系统 (Python + MySQL)")
    print("=" * 56)
    print("【模块一: 股票信息管理与分析】")
    print(" 1. 股票信息存储 (顺序表)   [第1关]")
    print(" 2. 股票信息删除           [第2关]")
    print(" 3. 股票信息修改           [第3关]")
    print(" 4. 折半查找股票信息       [第4关]")
    print(" 5. 二叉排序树查找         [第5关]")
    print(" 6. 字典树前缀查找         [第6关]")
    print(" 7. 开放地址散列查找       [第7关]")
    print(" 8. 链地址散列查找         [第8关]")
    print(" 9. 链表涨跌额分析         [第9关]")
    print("10. 并查集换手率分析       [第10关]")
    print("11. 堆排序成交量Top3       [第11关]")
    print("12. 归并排序成交量统计     [第12关]")
    print("13. 决策树分类统计         [第13关]")
    print("【模块二: 股票信息推荐与预测】")
    print("14. BF实体识别             [第14关]")
    print("15. BF关系抽取             [第15关]")
    print("16. 邻接表知识图谱构建     [第16关]")
    print("17. 知识图谱股票推荐       [第17关]")
    print("18. 皮尔逊相关系数分析     [第18关]")
    print("19. 朴素贝叶斯涨跌预测     [第19关·选做]")
    print("20. KNN涨跌幅预测          [第20关·选做]")
    print(" 0. 退出系统")
    print("=" * 56)


def main():
    while True:
        print_menu()
        choice = input("请输入功能编号(0-20):").strip()
        print()
        if choice == "1":
            level1()
        elif choice == "2":
            level2()
        elif choice == "3":
            level3()
        elif choice == "4":
            level4()
        elif choice == "5":
            level5()
        elif choice == "6":
            level6()
        elif choice == "7":
            level7()
        elif choice == "8":
            level8()
        elif choice == "9":
            level9()
        elif choice == "10":
            level10()
        elif choice == "11":
            level11()
        elif choice == "12":
            level12()
        elif choice == "13":
            level13()
        elif choice == "14":
            level14()
        elif choice == "15":
            level15()
        elif choice == "16":
            level16()
        elif choice == "17":
            level17()
        elif choice == "18":
            level18()
        elif choice == "19":
            level19()
        elif choice == "20":
            level20()
        elif choice == "0":
            print("感谢使用, 再见!")
            break
        else:
            print("输入无效, 请重新输入!")
        print()


if __name__ == "__main__":
    main()
