[简体中文](#zh) | [English](#en)

<a id="zh"></a>

# 智能股票分析与预测系统

> 西南财经大学《数据结构（课程设计）》课程项目 —— 第 1–20 关完整实现
> 同一个课题的 **两种独立语言实现**：C 语言（控制台 + Qt 6）与 Python + MySQL（控制台 + PySide6）

> ⚠️ 大一学生课程项目，仅供参考与学习交流。

本项目针对任务书要求的 20 个关卡，用两套技术栈各自**从零手写数据结构**完成了全部功能，未使用语言内置库替代算法，满足课程设计考核要求。

- **C 语言版** → [`c-version/`](c-version/) —— 文本文件存储，手写 C 数据结构，Qt 6 图形界面
- **Python + MySQL 版** → [`python-mysql-version/`](python-mysql-version/) —— MySQL 8.0 存储，手写 Python 数据结构，PySide6 图形界面

---

## 仓库结构

```text
.
├── c-version/                          # C 语言实现（控制台 + Qt 图形界面）
│   ├── main.c  stock.h  portable.h     # 入口 / 结构体 / 编译器兼容层
│   ├── stock_maintain.c/.h             # 第 1-3 关：顺序表存储 / 删除 / 修改
│   ├── stock_seek.c/.h                 # 第 4-8 关：折半 / BST / 字典树 / 散列
│   ├── stock_statistical_analysis.c/.h # 第 9-13 关：链表 / 并查集 / 堆 / 归并 / 决策树
│   ├── stock_choice.c/.h               # 第 14-17 关：实体识别 / 关系抽取 / 知识图谱
│   ├── stock_prediction.c/.h           # 第 18-20 关：皮尔逊 / 朴素贝叶斯 / KNN
│   ├── gui_qt/                         # Qt 6 图形界面（选做，5 个标签页）
│   ├── stocks.txt entity.txt relation.txt detail/   # 数据集
│   ├── build*.bat  *.ps1               # 编译 / 部署脚本
│   └── README.md                       # C 版详细说明
│
└── python-mysql-version/               # Python + MySQL 实现（控制台 + PySide6）
    ├── main.py  gui.py                 # 控制台入口 / Qt 图形界面（选做）
    ├── db.py  db_setup.py              # MySQL 连接与数据模型 / 建库建表导入
    ├── stock_maintain.py               # 第 1-3 关
    ├── stock_seek.py                   # 第 4-8 关
    ├── stock_statistical.py            # 第 9-13 关
    ├── stock_choice.py                 # 第 14-17 关
    ├── stock_prediction.py             # 第 18-20 关
    ├── test_levels.py  test_e2e.py     # 单元测试 / 端到端回归测试
    ├── test_gui.py  test_gui_launch.py # GUI 逻辑与启动测试
    ├── requirements.txt
    └── README.md                       # Python 版详细说明
```

## 两个版本对比

算法逻辑一一对应（`stock_maintain.c` ↔ `stock_maintain.py`，…，`gui_qt/mainwindow.cpp` ↔ `gui.py`），
区别主要在数据存储层与图形界面技术栈：

| 维度 | C 语言版 `c-version/` | Python + MySQL 版 `python-mysql-version/` |
|---|---|---|
| 数据存储 | 文本文件（`stocks.txt` / `entity.txt` / `relation.txt` / `detail/`） | MySQL 8.0（`stock_information` 库） |
| 数据读取 | `fopen` 直接读文件 | pymysql 查询后构造内存结构 |
| 核心算法 | 手写 C 数据结构（指针） | 手写 Python 数据结构（类） |
| 图形界面 | Qt 6.8（C++，MSVC + qmake 风格构建脚本） | PySide6（Qt 6.11，Python） |
| 控制台入口 | `main.c`（按关卡逐个调用） | `main.py`（文本菜单 1-20） |
| 编译器/编码 | MSVC 与 MinGW 双编译器兼容；控制台 GBK、文件 UTF-8 | 统一 UTF-8（Python 3 默认） |

## 功能总览（第 1–20 关）

### 模块一：股票信息管理与分析（第 1–13 关）

| 关卡 | 功能 | 数据结构 / 算法 |
|---|---|---|
| 1 | 股票信息存储：输入位置输出该位置股票全部信息 | 顺序表 |
| 2 | 股票信息删除：删除后写回 `new_stock.txt` | 顺序表 |
| 3 | 股票信息修改：修改公司全称后写回 | 顺序表 |
| 4 | 折半查找：先按英文名二路归并排序，再折半查找，输出 ASL | 归并排序 + 折半查找 |
| 5 | 二叉排序树查找：对比原始序列与排序后重建 BST 的 ASL | BST + 快速排序 |
| 6 | 字典树前缀查找：53 个子结点（26 小写 + 26 大写 + 空格） | Trie |
| 7 | 开放地址散列查找：线性探测，`Hash=(Σ int(cᵢ)·i²) mod 397`，表长 400 | 散列表 |
| 8 | 链地址散列查找：拉链法（头插法），输出行情明细 | 散列表 + 链 |
| 9 | 链表涨跌额分析：涨跌额 <0 前、=0 中、>0 后 | 单链表 |
| 10 | 并查集行业换手率分析：按行业编码 union，汇总换手率 | 并查集 |
| 11 | 堆排序成交量 Top3：指定日期 + 行业，从大到小 | 堆排序 |
| 12 | 归并排序成交量统计：按日期累加汇总，从小到大 | 归并排序 |
| 13 | 决策树分类统计：二级门类 → 涨跌幅 → 成交量三条件分裂 | 决策树（二叉树存储） |

### 模块二：股票信息推荐与预测（第 14–20 关）

| 关卡 | 功能 | 数据结构 / 算法 |
|---|---|---|
| 14 | 基于规则的实体识别：`*` 匹配任意中文字符 | BF 暴力匹配 |
| 15 | 基于规则的关系抽取：提取两实体间内容与规则集比对 | BF 匹配 |
| 16 | 邻接表知识图谱构建：entity + relation 数据建双向图 | 邻接表（图） |
| 17 | 基于知识图谱的股票推荐：按固定顺序输出关联股票属性 | 图遍历 |
| 18 | 皮尔逊相关系数分析：5 指标与次日涨跌相关性，输出 Top3 | 相关系数 |
| 19 | 朴素贝叶斯涨跌预测（选做）：图结构存高斯参数 | 高斯朴素贝叶斯 |
| 20 | KNN 涨跌幅预测（选做）：Min-Max 归一化 + 欧氏距离 + 有序链表维护 K=3 | KNN |

### 图形界面（两版均为选做项，5 个标签页）

| 标签页 | 覆盖关卡 |
|---|---|
| ① 信息维护 | 第 1–3 关：按位置输出 / 删除 / 修改 |
| ② 信息查找 | 第 4–8 关：折半、BST、字典树、开放地址散列、链地址散列 |
| ③ 统计分析 | 第 9–13 关：链表、并查集、堆排序、归并排序、决策树 |
| ④ 知识图谱 | 第 14–17 关：实体识别、关系抽取、图谱构建、图谱推荐 |
| ⑤ 股票预测 | 第 18–20 关：皮尔逊、朴素贝叶斯、KNN |

## 数据集

| 数据 | 内容 | 规模 |
|---|---|---|
| `stocks.txt` | 股票基本信息（代码 / 简称 / 英文名 / 行业 / 交易所 / 公司全称 / 上市日期 / 省份 / 城市 / 法人 / 地址 / 网址 / 邮箱 / 电话 / 主营业务 / 经营范围） | 300 条 |
| `entity.txt` | 知识图谱实体（10 种：股票简称、机构、上市交易所、行业编码、行业大类、行业二级类、省份、城市、地址、主营业务） | 1338 条 |
| `relation.txt` | 知识图谱三元组（9 种关系：所属机构、上市于、具有行业编码、属于行业大类、属于行业二级类、位于省份、位于城市、具有注册地址、主营业务为） | 2700 条 |
| `detail/` | 每支股票每日行情（日期 / 开盘 / 收盘 / 最高 / 最低 / 成交量 / 成交额 / 换手率 / 涨跌幅 / 涨跌额） | 300 个文件 |

## 快速开始

### C 语言版

```bash
cd c-version
build.bat                 # MinGW gcc 编译控制台程序 → output/stock_system.exe
```

Qt 图形界面（需要 Visual Studio MSVC x64 + Qt 6.8.0 msvc2022_64）：

```bash
build_gui.bat             # 或 pwsh build_gui.ps1，编译 GUI
pwsh deploy_gui.ps1       # 部署到 output/gui（含 Qt DLL 与数据文件）
```

要求 `gcc` 已加入 PATH；可执行文件需与数据文件（`stocks.txt`、`detail/` 等）位于同一目录。
更详细的关卡说明与注意事项见 **[c-version/README.md](c-version/README.md)**。

### Python + MySQL 版

```bash
cd python-mysql-version
python db_setup.py        # 首次执行：建库建表并从 txt 导入数据
python main.py            # 控制台文本菜单（输入 1-20 选择关卡，0 退出）
python gui.py             # PySide6 图形界面（20 关全部功能）
```

需要 MySQL 8.0（默认库 `stock_information`）与 `pymysql`；图形界面依赖 PySide6。
更详细的说明见 **[python-mysql-version/README.md](python-mysql-version/README.md)**。

## 说明与免责

- 两个版本均为**手写数据结构**实现（顺序表、BST、Trie、散列表、链表、并查集、堆、归并排序、决策树、邻接表图），未使用语言内置库替代算法。
- 数据集为教学演示数据，项目仅供课程学习与交流参考，不构成任何投资建议。

## 许可证

[MIT License](LICENSE) © 2026 hyper-deep

---

[简体中文](#zh) | [English](#en)

<a id="en"></a>

# Smart Stock Picking and Prediction System

> Course project for *Data Structures (Course Design)*, Southwestern University of Finance and Economics — a complete implementation of Levels 1–20.
> **Two independent implementations** of the same assignment: C (console + Qt 6) and Python + MySQL (console + PySide6).

> ⚠️ A freshman course project, provided for reference and learning only.

For all 20 levels required by the assignment, this repository implements every feature **twice**, each time with hand-written data structures rather than built-in library replacements, meeting the assessment requirements of the course.

- **C version** → [`c-version/`](c-version/) — text-file storage, hand-written C data structures, Qt 6 GUI
- **Python + MySQL version** → [`python-mysql-version/`](python-mysql-version/) — MySQL 8.0 storage, hand-written Python data structures, PySide6 GUI

---

## Repository layout

```text
.
├── c-version/                          # C implementation (console + Qt GUI)
│   ├── main.c  stock.h  portable.h     # entry / structs / compiler compatibility layer
│   ├── stock_maintain.c/.h             # Levels 1-3: sequential list storage / delete / update
│   ├── stock_seek.c/.h                 # Levels 4-8: binary search / BST / trie / hash
│   ├── stock_statistical_analysis.c/.h # Levels 9-13: list / union-find / heap / merge / decision tree
│   ├── stock_choice.c/.h               # Levels 14-17: entity recognition / relation extraction / KG
│   ├── stock_prediction.c/.h           # Levels 18-20: Pearson / Naive Bayes / KNN
│   ├── gui_qt/                         # Qt 6 GUI (optional task, 5 tabs)
│   ├── stocks.txt entity.txt relation.txt detail/   # datasets
│   ├── build*.bat  *.ps1               # build / deploy scripts
│   └── README.md                       # detailed docs (Chinese)
│
└── python-mysql-version/               # Python + MySQL implementation (console + PySide6)
    ├── main.py  gui.py                 # console entry / Qt GUI (optional task)
    ├── db.py  db_setup.py              # MySQL connection & models / schema setup + import
    ├── stock_maintain.py               # Levels 1-3
    ├── stock_seek.py                   # Levels 4-8
    ├── stock_statistical.py            # Levels 9-13
    ├── stock_choice.py                 # Levels 14-17
    ├── stock_prediction.py             # Levels 18-20
    ├── test_levels.py  test_e2e.py     # unit tests / end-to-end regression tests
    ├── test_gui.py  test_gui_launch.py # GUI logic and launch tests
    ├── requirements.txt
    └── README.md                       # detailed docs (Chinese)
```

## Version comparison

The algorithms map one-to-one (`stock_maintain.c` ↔ `stock_maintain.py`, …, `gui_qt/mainwindow.cpp` ↔ `gui.py`).
The differences are mainly the storage layer and the GUI stack:

| Aspect | C version `c-version/` | Python + MySQL version `python-mysql-version/` |
|---|---|---|
| Data storage | Text files (`stocks.txt` / `entity.txt` / `relation.txt` / `detail/`) | MySQL 8.0 (`stock_information` database) |
| Data access | Read files directly with `fopen` | Query with pymysql, then build in-memory structures |
| Core algorithms | Hand-written C data structures (pointers) | Hand-written Python data structures (classes) |
| GUI | Qt 6.8 (C++, MSVC) | PySide6 (Qt 6.11, Python) |
| Console entry | `main.c` (calls each level directly) | `main.py` (text menu 1-20) |
| Toolchain / encoding | MSVC and MinGW compatible; console GBK, files UTF-8 | UTF-8 throughout (Python 3 default) |

## Feature overview (Levels 1–20)

### Module 1: Stock information management and analysis (Levels 1–13)

| Level | Feature | Data structure / algorithm |
|---|---|---|
| 1 | Store stock info and print the full record at a given position | Sequential list |
| 2 | Delete a stock record and write back to `new_stock.txt` | Sequential list |
| 3 | Modify a stock's full company name and write back | Sequential list |
| 4 | Binary search by English name after two-way merge sort, output ASL | Merge sort + binary search |
| 5 | BST search: compare ASL of the original sequence vs. a rebuilt BST | BST + quicksort |
| 6 | Trie prefix search with 53 child nodes (26 lower + 26 upper + space) | Trie |
| 7 | Open-addressing hash search: linear probing, `Hash=(Σ int(cᵢ)·i²) mod 397`, table size 400 | Hash table |
| 8 | Chained hash search: separate chaining (head insertion), with daily quotes | Hash table + linked list |
| 9 | List-based change analysis: negative first, zero middle, positive last | Singly linked list |
| 10 | Union-find turnover analysis grouped by industry code | Union-find |
| 11 | Heap-sort Top 3 by volume for a given date and industry | Heap sort |
| 12 | Merge-sort volume statistics aggregated by date, ascending | Merge sort |
| 13 | Decision-tree classification: secondary sector → change % → volume | Decision tree (binary) |

### Module 2: Stock recommendation and prediction (Levels 14–20)

| Level | Feature | Data structure / algorithm |
|---|---|---|
| 14 | Rule-based entity recognition: `*` matches any Chinese character | Brute-force (BF) matching |
| 15 | Rule-based relation extraction: compare text between entities with rule set | BF matching |
| 16 | Knowledge graph construction with adjacency lists (bidirectional graph) | Adjacency list (graph) |
| 17 | Knowledge-graph based stock recommendation | Graph traversal |
| 18 | Pearson correlation of 5 indicators vs. next-day change, output Top 3 | Correlation coefficient |
| 19 | Naive Bayes up/down prediction (optional): Gaussian parameters in a graph | Gaussian Naive Bayes |
| 20 | KNN change prediction (optional): min-max scaling + Euclidean distance + sorted list, K=3 | KNN |

### GUI (optional task in both versions, 5 tabs)

| Tab | Levels covered |
|---|---|
| 1. Info maintenance | Levels 1–3: print by position / delete / modify |
| 2. Info search | Levels 4–8: binary search, BST, trie, open addressing, chaining |
| 3. Statistical analysis | Levels 9–13: list, union-find, heap sort, merge sort, decision tree |
| 4. Knowledge graph | Levels 14–17: entity recognition, relation extraction, KG build, recommendation |
| 5. Prediction | Levels 18–20: Pearson, Naive Bayes, KNN |

## Datasets

| Data | Content | Size |
|---|---|---|
| `stocks.txt` | Basic stock info (code / short name / English name / industry / exchange / full company name / listing date / province / city / legal representative / address / website / email / phone / main business / business scope) | 300 rows |
| `entity.txt` | Knowledge-graph entities (10 types: stock name, institution, exchange, industry code, sector, sub-sector, province, city, address, main business) | 1,338 rows |
| `relation.txt` | Knowledge-graph triples (9 relations: belongs to institution, listed on, has industry code, belongs to sector, belongs to sub-sector, located in province, located in city, has registered address, main business is) | 2,700 rows |
| `detail/` | Daily quotes per stock (date / open / close / high / low / volume / turnover / turnover rate / change % / change amount) | 300 files |

## Quick start

### C version

```bash
cd c-version
build.bat                 # compile the console app with MinGW gcc → output/stock_system.exe
```

Qt GUI (requires Visual Studio MSVC x64 + Qt 6.8.0 msvc2022_64):

```bash
build_gui.bat             # or: pwsh build_gui.ps1
pwsh deploy_gui.ps1       # deploy to output/gui (Qt DLLs + data files)
```

`gcc` must be on `PATH`; the executable must sit in the same directory as the data files
(`stocks.txt`, `detail/`, etc.). See **[c-version/README.md](c-version/README.md)** for details.

### Python + MySQL version

```bash
cd python-mysql-version
python db_setup.py        # first run: create the schema and import data from the txt files
python main.py            # console menu (enter 1-20 to pick a level, 0 to exit)
python gui.py             # PySide6 GUI (all 20 levels)
```

Requires MySQL 8.0 (database `stock_information` by default) and `pymysql`; the GUI needs PySide6.
See **[python-mysql-version/README.md](python-mysql-version/README.md)** for details.

## Notes and disclaimer

- Both versions implement data structures **by hand** (sequential list, BST, trie, hash table, linked list, union-find, heap, merge sort, decision tree, adjacency-list graph) instead of substituting library calls.
- The datasets are for teaching demonstrations only. This project is for course learning and reference and does not constitute investment advice.

## License

[MIT License](LICENSE) © 2026 hyper-deep

---

[简体中文](#zh) | [English](#en)
