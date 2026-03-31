# 海图显示系统设计文档

**版本**: v1.0  
**日期**: 2026-03-31  
**状态**: 设计阶段

---

## 1. 项目概述

### 1.1 项目目标

海图显示系统是整个海图导航平台的核心基础模块，负责海图数据的解析、存储、渲染和交互显示。系统需支持S57、S101及其他S100系列数据标准，实现跨平台（桌面端、移动端、Web端）统一渲染内核。

**核心目标指标**：

| 目标 | 具体指标 |
|-----|---------|
| 渲染性能 | 百万级要素渲染时间<300ms |
| 数据支持 | S57、S101、S100系列数据格式 |
| 平台覆盖 | Windows、Linux、Android、Web |
| 离线能力 | 各端支持离线海图显示 |
| 显示精度 | 支持高精度地图显示（定位精度<1m） |

### 1.2 技术栈

#### 1.2.1 核心技术架构

| 分类 | 技术选型 | 版本 | 用途 |
|------|----------|------|------|
| 核心内核 | C++ | 11 | 核心业务逻辑、数据处理 |
| 跨平台桥接 | JNI | - | C++与Java层交互 |
| Java封装 | JAR | - | Java API封装 |
| 桌面应用 | JavaFX | 11+ | 跨平台Java桌面应用框架 |
| 跨平台绘制 | Qt | 5.12+ | 跨平台图形绘制 |

#### 1.2.2 数据存储

| 分类 | 技术选型 | 版本 | 用途 |
|------|----------|------|------|
| 本地存储 | SQLite | 3.x | 离线数据存储 |
| 空间扩展 | SpatiaLite | 5.x | 空间数据索引 |
| 企业数据库 | PostgreSQL | 13+ | 企业级数据存储 |
| 空间扩展 | PostGIS | 3.x | PostgreSQL空间扩展 |
| 扩展支持 | Oracle/MySQL | - | 预留扩展接口 |

#### 1.2.3 数据处理

| 分类 | 技术选型 | 版本 | 用途 |
|------|----------|------|------|
| 数据解析 | GDAL/OGR | 3.x | S57/S101数据解析 |
| 坐标转换 | PROJ | 8.x | 坐标系转换 |

#### 1.2.4 技术架构图

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           技术架构层次图                                 │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                        应用层 (Application)                       │   │
│  │  ┌───────────┐ ┌───────────┐ ┌───────────┐                      │   │
│  │  │JavaFX桌面 │ │  Web应用  │ │  桌面应用  │                      │   │
│  │  └───────────┘ └───────────┘ └───────────┘                      │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                    │                                    │
│                                    ▼                                    │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                        封装层 (Wrapper)                           │   │
│  │  ┌───────────┐ ┌───────────┐                                     │   │
│  │  │  JAR封装  │ │  JNI桥接  │                                     │   │
│  │  └───────────┘ └───────────┘                                     │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                    │                                    │
│                                    ▼                                    │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                        核心层 (Core - C++11)                      │   │
│  │  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐       │   │
│  │  │   geom    │ │ database  │ │  feature  │ │   layer   │       │   │
│  │  └───────────┘ └───────────┘ └───────────┘ └───────────┘       │   │
│  │  ┌───────────┐ ┌───────────┐                                     │   │
│  │  │   draw    │ │   graph   │                                     │   │
│  │  └───────────┘ └───────────┘                                     │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                    │                                    │
│                                    ▼                                    │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                        平台层 (Platform)                          │   │
│  │  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐       │   │
│  │  │    Qt     │ │   GDAL    │ │   PROJ    │ │  SQLite   │       │   │
│  │  └───────────┘ └───────────┘ └───────────┘ └───────────┘       │   │
│  │  ┌───────────┐ ┌───────────┐                                     │   │
│  │  │PostgreSQL │ │  PostGIS  │                                     │   │
│  │  └───────────┘ └───────────┘                                     │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 1.3 目标用户

| 用户类型 | 使用场景 | 功能需求 |
|----------|----------|----------|
| 普通用户 | 内河航行导航 | 基础海图显示、定位导航 |
| VIP用户 | 专业航行导航 | 高精度定位、实时更新 |
| 高级VIP | 专业海事作业 | 厘米级定位、专业分析 |

### 1.4 适用范围

- 内河航道（长江、珠江、京杭运河等）
- 湖泊水域（洞庭湖、太湖等）
- 入海口区域
- 近海区域

---

## 2. 系统架构

### 2.1 整体架构图

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           海图显示系统架构                                │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                        应用层 (Application)                       │   │
│  │  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐       │   │
│  │  │JavaFX桌面 │ │  Web应用  │ │ 桌面端应用 │ │  命令行工具 │       │   │
│  │  └───────────┘ └───────────┘ └───────────┘ └───────────┘       │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                    │                                    │
│                                    ▼ (JNI/JAR)                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                        核心层 (Core - C++11)                      │   │
│  │  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐       │   │
│  │  │   graph   │ │   draw    │ │   layer   │ │  feature  │       │   │
│  │  │  图形交互  │ │  绘制引擎  │ │  图层管理  │ │  要素属性  │       │   │
│  │  └───────────┘ └───────────┘ └───────────┘ └───────────┘       │   │
│  │  ┌───────────┐ ┌───────────┐                                     │   │
│  │  │   geom    │ │ database  │                                     │   │
│  │  │  几何数据  │ │  数据存储  │                                     │   │
│  │  └───────────┘ └───────────┘                                     │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                    │                                    │
│                                    ▼                                    │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                        平台层 (Platform)                          │   │
│  │  ┌───────────┐ ┌───────────┐ ┌───────────┐ ┌───────────┐       │   │
│  │  │    Qt     │ │   GDAL    │ │   PROJ    │ │  SQLite   │       │   │
│  │  │ 跨平台绘制 │ │ 数据解析  │ │ 坐标转换  │ │ 本地存储  │       │   │
│  │  └───────────┘ └───────────┘ └───────────┘ └───────────┘       │   │
│  │  ┌───────────┐ ┌───────────┐                                     │   │
│  │  │PostgreSQL │ │  PostGIS  │                                     │   │
│  │  │ 企业存储  │ │ 空间扩展  │                                     │   │
│  │  └───────────┘ └───────────┘                                     │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 2.2 核心模块划分

#### 2.2.1 模块概览

**核心基础模块**：

| 模块名称 | 英文名 | 职责 | 依赖模块 |
|----------|--------|------|----------|
| 几何模块 | geom | 几何类定义与操作 | 无 |
| 数据库模块 | database | 统一数据库访问接口 | 无 |
| 要素模块 | feature | 要素属性管理 | geom, database |
| 图层模块 | layer | 图层数据封装 | feature, geom |
| 绘制模块 | draw | 绘制显示与打印 | layer, geom |
| 图形模块 | graph | 图形交互与控制 | draw, layer |

**业务功能模块**：

| 模块名称 | 英文名 | 职责 | 依赖模块 |
|----------|--------|------|----------|
| 数据解析模块 | DataParser | S57/S101数据解析 | CoordTrans |
| 数据存储模块 | DataStorage | 持久化存储、增量更新 | database, DataParser |
| 坐标转换模块 | CoordTrans | 多坐标系转换 | PROJ库 |
| 海图显示模块 | ChartDisplay | S52样式、日/夜模式、比例尺自适应 | draw, layer |
| 定位显示模块 | Position | 定位数据接入、船位显示 | draw, CoordTrans |
| 图层管理模块 | LayerManager | 多图层协调管理 | layer, draw |
| 平台适配模块 | PlatformAdapt | 跨平台适配 | Qt, JavaFX |
| 离线服务模块 | OfflineSvc | 离线数据管理 | DataStorage, database |

#### 2.2.2 核心模块依赖关系图

```
┌─────────────────────────────────────────────────────────────┐
│                    模块依赖关系图                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│                    ┌─────────────┐                          │
│                    │    graph    │                          │
│                    │  图形交互   │                          │
│                    │ 定位/样式   │                          │
│                    └──────┬──────┘                          │
│                           │ 调用                            │
│                           ▼                                 │
│                    ┌─────────────┐                          │
│                    │    draw     │                          │
│                    │  绘制引擎   │                          │
│                    │ 显示/打印   │                          │
│                    └──────┬──────┘                          │
│                           │ 使用                            │
│                           ▼                                 │
│                    ┌─────────────┐                          │
│                    │    layer    │                          │
│                    │  图层管理   │                          │
│                    │ 数据封装    │                          │
│                    └──────┬──────┘                          │
│                           │ 包含                            │
│                           ▼                                 │
│                    ┌─────────────┐                          │
│                    │   feature   │                          │
│                    │  要素属性   │                          │
│                    │ 属性管理    │                          │
│                    └──────┬──────┘                          │
│                           │ 包含                            │
│                           ▼                                 │
│         ┌─────────────────┴─────────────────┐              │
│         ▼                                   ▼               │
│  ┌─────────────┐                    ┌─────────────┐        │
│  │    geom     │                    │  database   │        │
│  │  几何数据   │                    │  数据存储   │        │
│  │ 点/线/面    │                    │ SQLite/PG   │        │
│  └─────────────┘                    └─────────────┘        │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

#### 2.2.3 数据流程图

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           数据处理流程                                   │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────┐                                                        │
│  │ 海图数据文件│                                                        │
│  │ S57/S101   │                                                        │
│  └──────┬──────┘                                                        │
│         │ 解析                                                          │
│         ▼                                                               │
│  ┌─────────────┐     ┌─────────────┐                                   │
│  │ 几何数据    │     │ 属性数据    │                                   │
│  │ (geom模块)  │     │             │                                   │
│  └──────┬──────┘     └──────┬──────┘                                   │
│         │                   │                                           │
│         └─────────┬─────────┘                                           │
│                   ▼                                                     │
│           ┌─────────────┐                                               │
│           │  database   │                                               │
│           │  本地DB存储 │                                               │
│           │ SQLite/PG   │                                               │
│           └──────┬──────┘                                               │
│                  │ 读取                                                 │
│                  ▼                                                      │
│           ┌─────────────┐                                               │
│           │   feature   │                                               │
│           │  要素组织   │                                               │
│           │ 几何+属性   │                                               │
│           └──────┬──────┘                                               │
│                  │ 封装                                                 │
│                  ▼                                                      │
│           ┌─────────────┐                                               │
│           │    layer    │                                               │
│           │  图层封装   │                                               │
│           │ 高级数据    │                                               │
│           └──────┬──────┘                                               │
│                  │ 绘制                                                 │
│                  ▼                                                      │
│           ┌─────────────┐                                               │
│           │    draw     │                                               │
│           │  绘制显示   │                                               │
│           │ 打印输出    │                                               │
│           └──────┬──────┘                                               │
│                  │ 交互                                                 │
│                  ▼                                                      │
│           ┌─────────────┐                                               │
│           │    graph    │                                               │
│           │  图形交互   │                                               │
│           │ 定位/样式   │                                               │
│           └─────────────┘                                               │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

**关键路径**：海图数据解析 → database存储 → feature组织 → layer封装 → draw绘制 → graph交互

**并行任务识别**：
- [geom模块] 和 [database模块] 可并行开发（无依赖）
- [feature模块] 依赖 geom 和 database 完成后开发
- [layer模块] 依赖 feature 完成后开发
- [draw模块] 依赖 layer 完成后开发
- [graph模块] 依赖 draw 完成后开发

**并行任务识别**：
- [交互控制模块] 和 [定位显示模块] 可并行开发
- [离线服务模块] 和 [安全合规模块] 可并行开发

### 2.3 技术选型理由

| 技术 | 选型理由 |
|------|----------|
| C++11 | 高性能GIS核心处理，跨平台支持，成熟稳定 |
| JNI/JAR | 实现C++核心与Java应用的桥接，便于Java生态集成 |
| JavaFX | 跨平台Java桌面应用框架，支持多平台部署 |
| Qt 5.12+ | 跨平台图形绘制框架，支持多种渲染后端 |
| GDAL/OGR | GIS数据解析标准库，支持S57/S101等格式 |
| PROJ | 坐标转换标准库，精度高，支持多种坐标系 |
| SQLite + SpatiaLite | 轻量级嵌入式数据库，支持空间索引，离线场景友好 |
| PostgreSQL + PostGIS | 企业级数据库，支持空间数据，可扩展性强 |
| 通用数据库接口 | 预留Oracle/MySQL扩展接口，便于后续扩展 |

---

## 3. 核心模块设计

### 3.1 几何模块 (geom)

**功能范围**：
- 几何类型定义（点、线、面、多点、多线、多面）
- 几何操作（缓冲区、相交、合并、差集等）
- 几何计算（面积、长度、距离、角度等）
- 几何变换（平移、旋转、缩放）
- 坐标系统支持

**不包含**：
- 几何数据持久化（由database模块负责）
- 几何渲染（由draw模块负责）

**几何类型层次**：

```
┌─────────────────────────────────────────────────────────────┐
│                      几何类型层次结构                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│                    ┌─────────────┐                          │
│                    │  Geometry   │                          │
│                    │  (抽象基类)  │                          │
│                    └──────┬──────┘                          │
│         ┌─────────────────┼─────────────────┐              │
│         ▼                 ▼                 ▼               │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │    Point    │  │    Line     │  │    Area     │         │
│  │    点几何   │  │    线几何   │  │    面几何   │         │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘         │
│         │                 │                 │               │
│         ▼                 ▼                 ▼               │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │ MultiPoint  │  │ MultiLine   │  │ MultiArea   │         │
│  │   多点几何  │  │   多线几何  │  │   多面几何  │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**对外接口**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| Buffer | `Geometry Buffer(Geometry geom, double distance)` | 缓冲区分析 |
| Intersect | `Geometry Intersect(Geometry geom1, Geometry geom2)` | 相交运算 |
| Union | `Geometry Union(Geometry geom1, Geometry geom2)` | 合并运算 |
| Distance | `double Distance(Geometry geom1, Geometry geom2)` | 距离计算 |
| Area | `double Area(Geometry geom)` | 面积计算 |
| Length | `double Length(Geometry geom)` | 长度计算 |
| Transform | `Geometry Transform(Geometry geom, Matrix matrix)` | 几何变换 |

**依赖模块**：
- 无外部依赖（基础模块）

**技术实现要点**：
1. 使用C++11实现，保证跨平台兼容性
2. 支持WKT/WKB格式导入导出
3. 实现空间关系判断算法
4. 支持坐标系转换（调用PROJ库）

---

### 3.2 数据库模块 (database)

**功能范围**：
- 统一数据库访问接口
- 多数据库适配（SQLite+SpatiaLite、PostgreSQL+PostGIS）
- 空间索引支持
- 事务管理
- 连接池管理

**不包含**：
- 数据解析（由外部解析模块负责）
- 业务逻辑（由feature/layer模块负责）

**数据库适配架构**：

```
┌─────────────────────────────────────────────────────────────┐
│                    数据库适配架构                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│                    ┌─────────────┐                          │
│                    │  统一接口层  │                          │
│                    │ IDatabase   │                          │
│                    └──────┬──────┘                          │
│                           │                                 │
│         ┌─────────────────┼─────────────────┐              │
│         ▼                 ▼                 ▼               │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │ SQLite适配  │  │ PostgreSQL  │  │  Oracle     │         │
│  │ SQLiteAdapt │  │PostgresAdapt│  │ OracleAdapt │         │
│  │  (已实现)   │  │  (已实现)   │  │  (预留扩展)  │         │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘         │
│         │                 │                 │               │
│         ▼                 ▼                 ▼               │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │   SQLite    │  │ PostgreSQL  │  │   Oracle    │         │
│  │ SpatiaLite  │  │   PostGIS   │  │   Spatial   │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
│                                                             │
│  ┌─────────────┐                                           │
│  │   MySQL     │  (预留扩展)                               │
│  │ MySQLSpatial│                                           │
│  └─────────────┘                                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**统一数据库接口 (IDatabase)**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| Connect | `bool Connect(const string& connStr)` | 连接数据库 |
| Disconnect | `void Disconnect()` | 断开连接 |
| ExecuteQuery | `ResultSet ExecuteQuery(const string& sql)` | 执行查询 |
| ExecuteUpdate | `int ExecuteUpdate(const string& sql)` | 执行更新 |
| BeginTransaction | `void BeginTransaction()` | 开始事务 |
| Commit | `void Commit()` | 提交事务 |
| Rollback | `void Rollback()` | 回滚事务 |
| InsertFeature | `int InsertFeature(const Feature& feature)` | 插入要素 |
| QueryFeatures | `vector<Feature> QueryFeatures(const BBox& bbox)` | 空间查询 |
| CreateSpatialIndex | `bool CreateSpatialIndex(const string& table, const string& geomCol)` | 创建空间索引 |

**支持的数据库特性对比**：

| 特性 | SQLite+SpatiaLite | PostgreSQL+PostGIS | Oracle(预留) | MySQL(预留) |
|-----|-------------------|-------------------|--------------|-------------|
| 空间索引 | R树 | GiST | R树 | R树 |
| 事务支持 | ✅ | ✅ | ✅ | ✅ |
| 空间函数 | ✅ | ✅ | ✅ | ✅ |
| 并发访问 | 单写多读 | 多写多读 | 多写多读 | 多写多读 |
| 适用场景 | 本地离线 | 企业级 | 企业级 | 企业级 |

**依赖模块**：
- SQLite/SpatiaLite：本地存储
- PostgreSQL/PostGIS：企业级存储
- libpq：PostgreSQL客户端库

**技术实现要点**：
1. 抽象统一接口，屏蔽底层数据库差异
2. 使用工厂模式创建数据库适配器
3. 实现连接池管理，提升性能
4. 支持空间索引，优化空间查询
5. 使用SQLite作为本地存储引擎
6. SpatiaLite扩展提供空间索引能力
7. 实现LRU缓存策略，热点数据常驻内存

---

### 3.3 要素模块 (feature)

**功能范围**：
- 要素定义（几何+属性）
- 属性管理（属性字段、属性值）
- 要素类型管理
- 要素查询和过滤

**不包含**：
- 数据持久化（由database模块负责）
- 图层管理（由layer模块负责）

**要素数据结构**：

```
┌─────────────────────────────────────────────────────────────┐
│                      要素数据结构                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                      Feature                         │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │  - id: int                    // 要素ID              │   │
│  │  - geometry: Geometry         // 几何数据(来自geom)  │   │
│  │  - attributes: AttributeMap   // 属性数据            │   │
│  │  - featureType: FeatureType   // 要素类型            │   │
│  │  - style: Style               // 显示样式            │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                   AttributeMap                       │   │
│  ├─────────────────────────────────────────────────────┤   │
│  │  - fields: vector<Field>      // 字段定义            │   │
│  │  - values: map<string, Value> // 属性值              │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**对外接口**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| GetGeometry | `Geometry GetGeometry()` | 获取几何数据 |
| SetGeometry | `void SetGeometry(const Geometry& geom)` | 设置几何数据 |
| GetAttribute | `Value GetAttribute(const string& name)` | 获取属性值 |
| SetAttribute | `void SetAttribute(const string& name, const Value& value)` | 设置属性值 |
| GetFeatureType | `FeatureType GetFeatureType()` | 获取要素类型 |
| Clone | `Feature Clone()` | 克隆要素 |

**要素类型定义**：

| 要素类型 | 说明 | 几何类型 |
|---------|-----|---------|
| SOUNDG | 水深点 | Point |
| LIGHTS | 灯光 | Point |
| BOYLAT | 浮标 | Point |
| DEPARE | 深度区域 | Area |
| COALNE | 海岸线 | Line |
| NAVLNE | 航道线 | Line |

**依赖模块**：
- geom模块：几何数据
- database模块：数据存储

**技术实现要点**：
1. 几何数据来自geom模块
2. 属性数据支持多种数据类型
3. 支持S57/S101要素类型映射
4. 实现要素缓存机制

---

### 3.4 图层模块 (layer)

**功能范围**：
- 图层数据封装（更高一级数据组织）
- 图层可见性控制
- 图层顺序管理
- 图层样式管理
- 图层过滤

**不包含**：
- 数据渲染（由draw模块负责）
- 用户交互（由graph模块负责）

**图层架构**：

```
┌─────────────────────────────────────────────────────────────┐
│                      图层架构                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│                    ┌─────────────┐                          │
│                    │   Layer     │                          │
│                    │  (图层基类)  │                          │
│                    └──────┬──────┘                          │
│         ┌─────────────────┼─────────────────┐              │
│         ▼                 ▼                 ▼               │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │ ChartLayer  │  │ TrackLayer  │  │ MarkLayer   │         │
│  │  海图图层   │  │  航迹图层   │  │  标记图层   │         │
│  └─────────────┘  └─────────────┘  └─────────────┘         │
│                                                             │
│  图层内容:                                                  │
│  ┌─────────────────────────────────────────────────────┐   │
│  │  - features: vector<Feature>   // 要素列表          │   │
│  │  - visible: bool               // 可见性            │   │
│  │  - opacity: float              // 透明度            │   │
│  │  - order: int                  // 图层顺序          │   │
│  │  - style: LayerStyle           // 图层样式          │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**数据分层架构**：

| 层级 | 数据类型 | 存储方式 | 更新频率 |
|-----|---------|---------|---------|
| 基础层 | 海岸线、陆地 | 本地存储 | 低频 |
| 航行层 | 航道、航标、障碍物 | 本地+增量更新 | 中频 |
| 动态层 | 水深、水位、气象 | 实时更新 | 高频 |
| 用户层 | 用户标记、航线 | 本地存储 | 按需 |

**对外接口**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| AddFeature | `void AddFeature(const Feature& feature)` | 添加要素 |
| RemoveFeature | `void RemoveFeature(int featureId)` | 移除要素 |
| GetFeatures | `vector<Feature> GetFeatures()` | 获取所有要素 |
| SetVisible | `void SetVisible(bool visible)` | 设置可见性 |
| SetOpacity | `void SetOpacity(float opacity)` | 设置透明度 |
| SetOrder | `void SetOrder(int order)` | 设置图层顺序 |

**依赖模块**：
- feature模块：要素数据
- geom模块：几何操作

**技术实现要点**：
1. 图层作为feature的更高一级封装
2. 支持图层分组管理
3. 实现图层可见性快速切换
4. 支持图层样式继承和覆盖

---

### 3.5 绘制模块 (draw)

**功能范围**：
- 海图绘制显示
- 打印输出
- 多设备支持（屏幕、打印机、图片）
- 渲染引擎管理
- S52显示样式支持

**不包含**：
- 用户交互（由graph模块负责）
- 数据管理（由layer模块负责）

**绘制架构**：

```
┌─────────────────────────────────────────────────────────────┐
│                      绘制引擎架构                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │ 样式管理器  │  │ 符号库管理  │  │ 渲染配置    │         │
│  │ StyleManager│  │ SymbolLib   │  │ RenderConfig│         │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘         │
│         │                │                │                │
│         └────────────────┼────────────────┘                │
│                          ▼                                  │
│                  ┌─────────────┐                           │
│                  │ 绘制调度器  │                           │
│                  │ DrawDispatch│                           │
│                  └──────┬──────┘                           │
│         ┌───────────────┼───────────────┐                  │
│         ▼               ▼               ▼                  │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐          │
│  │ 屏幕绘制器  │ │ 打印绘制器  │ │ 图片绘制器  │          │
│  │ScreenDrawer │ │PrintDrawer  │ │ImageDrawer  │          │
│  └─────────────┘ └─────────────┘ └─────────────┘          │
│                          │                                  │
│                          ▼                                  │
│                  ┌─────────────┐                           │
│                  │  Qt绘制API  │                           │
│                  │ QPainter    │                           │
│                  └─────────────┘                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**渲染性能指标**：

| 指标 | 目标值 | 测试条件 |
|-----|-------|---------|
| 首屏加载 | <300ms | 百万级要素 |
| 缩放响应 | <100ms | 单次缩放操作 |
| 平移响应 | <50ms | 单次平移操作 |
| 帧率 | ≥30fps | 交互状态 |

**显示样式支持**：

| 样式类型 | 说明 |
|---------|-----|
| S52样式 | 符合IHO S-52显示规范 |
| 自定义样式 | 支持用户自定义显示样式 |
| 日/夜模式 | 支持白天/夜晚显示模式 |
| 要素过滤 | 支持按类型、比例尺过滤显示 |

**对外接口**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| DrawLayer | `void DrawLayer(const Layer& layer, const Viewport& viewport)` | 绘制图层 |
| DrawFeature | `void DrawFeature(const Feature& feature, const Style& style)` | 绘制要素 |
| Print | `void Print(const vector<Layer>& layers, const PrintConfig& config)` | 打印输出 |
| ExportImage | `void ExportImage(const vector<Layer>& layers, const string& path, const ImageConfig& config)` | 导出图片 |
| SetStyle | `void SetStyle(StyleType style)` | 设置显示样式 |
| SetDayNightMode | `void SetDayNightMode(DayNightMode mode)` | 设置日/夜模式 |

**依赖模块**：
- layer模块：图层数据
- geom模块：几何数据
- Qt框架：跨平台绘制

**技术实现要点**：
1. 使用Qt QPainter作为统一绘制接口
2. 实现矢量渲染+栅格缓存混合模式
3. LOD技术优化大数据量渲染
4. 瓦片缓存机制提升响应速度

**示例代码 - 从SpatiaLite取数据绘制**：

```cpp
// ChartRenderer.h - 海图渲染器
#ifndef CHART_RENDERER_H
#define CHART_RENDERER_H

#include <QPainter>
#include <QPixmap>
#include <memory>
#include "SpatiaLiteStorage.h"
#include "Feature.h"
#include "StyleManager.h"
#include "SymbolLibrary.h"

namespace chart {

struct Viewport {
    double centerX;
    double centerY;
    double scale;
    double rotation;
    int width;
    int height;
    
    BoundingBox GetBoundingBox() const {
        double halfW = (width / scale) / 2.0;
        double halfH = (height / scale) / 2.0;
        return {centerX - halfW, centerY - halfH, 
                centerX + halfW, centerY + halfH};
    }
    
    Point ScreenToWorld(int screenX, int screenY) const {
        double worldX = centerX + (screenX - width / 2.0) / scale;
        double worldY = centerY - (screenY - height / 2.0) / scale;
        return {worldX, worldY};
    }
    
    Point WorldToScreen(double worldX, double worldY) const {
        int screenX = static_cast<int>((worldX - centerX) * scale + width / 2.0);
        int screenY = static_cast<int>((centerY - worldY) * scale + height / 2.0);
        return {static_cast<double>(screenX), static_cast<double>(screenY)};
    }
};

struct RenderConfig {
    bool enableAntialiasing = true;
    bool enableLOD = true;
    int lodThreshold = 10000;
    bool enableCache = true;
    int cacheSizeMB = 256;
};

class ChartRenderer {
public:
    ChartRenderer();
    ~ChartRenderer();
    
    bool Initialize(SpatiaLiteStorage* storage, const RenderConfig& config);
    
    void Render(QPainter* painter, const Viewport& viewport);
    void RenderLayer(QPainter* painter, const Layer& layer, const Viewport& viewport);
    void RenderFeature(QPainter* painter, const Feature& feature, const Viewport& viewport);
    
    QPixmap RenderToPixmap(const Viewport& viewport, const QSize& size);
    bool ExportImage(const std::string& path, const Viewport& viewport, 
                     const QSize& size, const std::string& format = "PNG");
    
    void SetDayNightMode(DayNightMode mode);
    void SetDisplayStyle(DisplayStyle style);
    
    void InvalidateCache();
    
private:
    void DrawPoint(QPainter* painter, const Point* point, const Style& style, 
                   const Viewport& viewport);
    void DrawLine(QPainter* painter, const Line* line, const Style& style,
                  const Viewport& viewport);
    void DrawArea(QPainter* painter, const Area* area, const Style& style,
                  const Viewport& viewport);
    
    void ApplyStyle(QPainter* painter, const Style& style);
    void DrawSymbol(QPainter* painter, const std::string& symbolId, 
                    const Point& pos, double scale);
    
    std::vector<Feature> QueryVisibleFeatures(const Viewport& viewport);
    void SortByPriority(std::vector<Feature>& features);
    
    SpatiaLiteStorage* m_storage;
    StyleManager* m_styleManager;
    SymbolLibrary* m_symbolLib;
    RenderConfig m_config;
    DayNightMode m_dayNightMode;
    DisplayStyle m_displayStyle;
    
    std::map<std::string, QPixmap> m_tileCache;
    std::mutex m_cacheMutex;
};

} // namespace chart

#endif // CHART_RENDERER_H
```

```cpp
// ChartRenderer.cpp - 海图渲染器实现
#include "ChartRenderer.h"
#include <QImage>
#include <QFile>
#include <spdlog/spdlog.h>
#include <chrono>
#include <algorithm>

namespace chart {

ChartRenderer::ChartRenderer() 
    : m_storage(nullptr)
    , m_styleManager(nullptr)
    , m_symbolLib(nullptr)
    , m_dayNightMode(DayNightMode::DAY)
    , m_displayStyle(DisplayStyle::S52) {
}

ChartRenderer::~ChartRenderer() {
    delete m_styleManager;
    delete m_symbolLib;
}

bool ChartRenderer::Initialize(SpatiaLiteStorage* storage, const RenderConfig& config) {
    m_storage = storage;
    m_config = config;
    
    m_styleManager = new StyleManager();
    if (!m_styleManager->LoadS52Styles()) {
        spdlog::error("加载S52样式失败");
        return false;
    }
    
    m_symbolLib = new SymbolLibrary();
    if (!m_symbolLib->LoadSymbols()) {
        spdlog::error("加载符号库失败");
        return false;
    }
    
    spdlog::info("海图渲染器初始化成功");
    return true;
}

void ChartRenderer::Render(QPainter* painter, const Viewport& viewport) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (m_config.enableAntialiasing) {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::TextAntialiasing, true);
    }
    
    painter->fillRect(0, 0, viewport.width, viewport.height, 
                      m_styleManager->GetBackgroundColor(m_dayNightMode));
    
    std::vector<Feature> features = QueryVisibleFeatures(viewport);
    spdlog::debug("查询到{}个可见要素", features.size());
    
    if (m_config.enableLOD && features.size() > m_config.lodThreshold) {
        SortByPriority(features);
        int visibleCount = static_cast<int>(features.size() * 0.7);
        features.resize(visibleCount);
    }
    
    SortByPriority(features);
    
    for (const auto& feature : features) {
        RenderFeature(painter, feature, viewport);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    spdlog::debug("渲染完成: 要素数={}, 耗时{:.2f}ms", features.size(), elapsedMs);
}

std::vector<Feature> ChartRenderer::QueryVisibleFeatures(const Viewport& viewport) {
    BoundingBox bbox = viewport.GetBoundingBox();
    QueryResult result = m_storage->QueryFeatures(bbox);
    
    if (!result.success) {
        spdlog::warn("查询要素失败: {}", result.errorMessage);
        return {};
    }
    
    std::vector<Feature> visibleFeatures;
    for (const auto& feature : result.features) {
        int minScale = feature.GetMinScale();
        int maxScale = feature.GetMaxScale();
        int currentScale = static_cast<int>(viewport.scale);
        
        if (minScale > 0 && currentScale < minScale) continue;
        if (maxScale > 0 && currentScale > maxScale) continue;
        
        visibleFeatures.push_back(feature);
    }
    
    return visibleFeatures;
}

void ChartRenderer::RenderFeature(QPainter* painter, const Feature& feature, 
                                   const Viewport& viewport) {
    const Geometry* geom = feature.GetGeometry();
    if (geom == nullptr) return;
    
    Style style = m_styleManager->GetStyle(feature.GetType(), m_dayNightMode);
    ApplyStyle(painter, style);
    
    switch (geom->GetType()) {
        case GeometryType::POINT:
        case GeometryType::MULTIPOINT:
            DrawPoint(painter, static_cast<const Point*>(geom), style, viewport);
            break;
        case GeometryType::LINE:
        case GeometryType::MULTILINE:
            DrawLine(painter, static_cast<const Line*>(geom), style, viewport);
            break;
        case GeometryType::AREA:
        case GeometryType::MULTIAREA:
            DrawArea(painter, static_cast<const Area*>(geom), style, viewport);
            break;
    }
}

void ChartRenderer::DrawPoint(QPainter* painter, const Point* point, 
                               const Style& style, const Viewport& viewport) {
    Point screenPos = viewport.WorldToScreen(point->GetX(), point->GetY());
    
    std::string symbolId = style.GetSymbolId();
    if (!symbolId.empty()) {
        DrawSymbol(painter, symbolId, screenPos, style.GetSymbolScale());
    } else {
        double radius = style.GetPointSize() / 2.0;
        painter->setBrush(QBrush(style.GetFillColor()));
        painter->setPen(QPen(style.GetStrokeColor(), style.GetStrokeWidth()));
        painter->drawEllipse(QPointF(screenPos.GetX(), screenPos.GetY()), 
                            radius, radius);
    }
}

void ChartRenderer::DrawLine(QPainter* painter, const Line* line, 
                              const Style& style, const Viewport& viewport) {
    const std::vector<Point>& points = line->GetPoints();
    if (points.empty()) return;
    
    QPolygonF polygon;
    for (const auto& pt : points) {
        Point screenPos = viewport.WorldToScreen(pt.GetX(), pt.GetY());
        polygon << QPointF(screenPos.GetX(), screenPos.GetY());
    }
    
    QPen pen(style.GetStrokeColor(), style.GetStrokeWidth());
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    
    if (style.IsDashed()) {
        QVector<qreal> dashes;
        dashes << style.GetDashPattern().first << style.GetDashPattern().second;
        pen.setDashPattern(dashes);
    }
    
    painter->setPen(pen);
    painter->drawPolyline(polygon);
}

void ChartRenderer::DrawArea(QPainter* painter, const Area* area, 
                              const Style& style, const Viewport& viewport) {
    const std::vector<Point>& points = area->GetPoints();
    if (points.size() < 3) return;
    
    QPolygonF polygon;
    for (const auto& pt : points) {
        Point screenPos = viewport.WorldToScreen(pt.GetX(), pt.GetY());
        polygon << QPointF(screenPos.GetX(), screenPos.GetY());
    }
    
    painter->setBrush(QBrush(style.GetFillColor()));
    painter->setPen(QPen(style.GetStrokeColor(), style.GetStrokeWidth()));
    painter->drawPolygon(polygon);
}

void ChartRenderer::ApplyStyle(QPainter* painter, const Style& style) {
    QPen pen(style.GetStrokeColor(), style.GetStrokeWidth());
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    
    QBrush brush(style.GetFillColor());
    painter->setBrush(brush);
}

void ChartRenderer::DrawSymbol(QPainter* painter, const std::string& symbolId,
                                const Point& pos, double scale) {
    QPixmap symbol = m_symbolLib->GetSymbol(symbolId, m_dayNightMode);
    if (symbol.isNull()) {
        spdlog::warn("符号未找到: {}", symbolId);
        return;
    }
    
    QSize targetSize = symbol.size() * scale;
    QPixmap scaledSymbol = symbol.scaled(targetSize, 
                                         Qt::KeepAspectRatio, 
                                         Qt::SmoothTransformation);
    
    int x = static_cast<int>(pos.GetX() - scaledSymbol.width() / 2.0);
    int y = static_cast<int>(pos.GetY() - scaledSymbol.height() / 2.0);
    
    painter->drawPixmap(x, y, scaledSymbol);
}

void ChartRenderer::SortByPriority(std::vector<Feature>& features) {
    std::sort(features.begin(), features.end(), 
              [](const Feature& a, const Feature& b) {
                  return a.GetDisplayPriority() < b.GetDisplayPriority();
              });
}

void ChartRenderer::SetDayNightMode(DayNightMode mode) {
    m_dayNightMode = mode;
    InvalidateCache();
    spdlog::info("切换显示模式: {}", mode == DayNightMode::DAY ? "白天" : "夜晚");
}

void ChartRenderer::SetDisplayStyle(DisplayStyle style) {
    m_displayStyle = style;
    InvalidateCache();
}

void ChartRenderer::InvalidateCache() {
    std::lock_guard<std::mutex> lock(m_cacheMutex);
    m_tileCache.clear();
}

QPixmap ChartRenderer::RenderToPixmap(const Viewport& viewport, const QSize& size) {
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    Viewport adjustedViewport = viewport;
    adjustedViewport.width = size.width();
    adjustedViewport.height = size.height();
    
    Render(&painter, adjustedViewport);
    return pixmap;
}

bool ChartRenderer::ExportImage(const std::string& path, const Viewport& viewport,
                                 const QSize& size, const std::string& format) {
    QPixmap pixmap = RenderToPixmap(viewport, size);
    return pixmap.save(QString::fromStdString(path), format.c_str());
}

} // namespace chart
```

**示例代码 - 完整渲染流程**：

```cpp
// main.cpp - 完整渲染流程示例
#include "S57Parser.h"
#include "SpatiaLiteStorage.h"
#include "ChartRenderer.h"
#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <spdlog/spdlog.h>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    spdlog::set_level(spdlog::level::debug);
    
    chart::S57Parser parser;
    chart::ParseResult parseResult = parser.ParseChart("CN12345.000");
    
    if (!parseResult.success) {
        spdlog::error("解析失败: {}", parseResult.errorMessage);
        return -1;
    }
    
    chart::SpatiaLiteStorage storage;
    chart::StorageConfig storageConfig;
    storageConfig.dbPath = "./charts.db";
    storageConfig.cacheSizeMB = 256;
    storageConfig.enableWAL = true;
    
    if (!storage.Initialize(storageConfig)) {
        spdlog::error("存储初始化失败");
        return -1;
    }
    
    int chartId = storage.StoreChart("CN12345", parseResult.features);
    if (chartId < 0) {
        spdlog::error("存储海图失败");
        return -1;
    }
    
    chart::ChartRenderer renderer;
    chart::RenderConfig renderConfig;
    renderConfig.enableAntialiasing = true;
    renderConfig.enableLOD = true;
    renderConfig.enableCache = true;
    
    if (!renderer.Initialize(&storage, renderConfig)) {
        spdlog::error("渲染器初始化失败");
        return -1;
    }
    
    QMainWindow window;
    QLabel* label = new QLabel(&window);
    window.setCentralWidget(label);
    window.resize(1024, 768);
    
    chart::Viewport viewport;
    viewport.centerX = 120.5;
    viewport.centerY = 31.2;
    viewport.scale = 5000;
    viewport.rotation = 0;
    viewport.width = 1024;
    viewport.height = 768;
    
    QPixmap pixmap = renderer.RenderToPixmap(viewport, QSize(1024, 768));
    label->setPixmap(pixmap);
    
    window.show();
    
    storage.Shutdown();
    
    return app.exec();
}
```

---

### 3.6 图形模块 (graph)

**功能范围**：
- 图形交互（平移、缩放、旋转）
- 定位显示
- 显示模式/样式切换
- 符号化显示
- 要素选择和查询

**不包含**：
- 绘制逻辑（由draw模块负责）
- 数据管理（由layer模块负责）

**图形交互架构**：

```
┌─────────────────────────────────────────────────────────────┐
│                      图形交互架构                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │
│  │ 交互控制器  │  │ 定位管理器  │  │ 样式管理器  │         │
│  │Interaction  │  │ PositionMgr │  │ StyleMgr    │         │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘         │
│         │                │                │                │
│         └────────────────┼────────────────┘                │
│                          ▼                                  │
│                  ┌─────────────┐                           │
│                  │ 图形管理器  │                           │
│                  │ GraphManager│                           │
│                  └──────┬──────┘                           │
│                          │ 调用                            │
│                          ▼                                  │
│                  ┌─────────────┐                           │
│                  │    draw     │                           │
│                  │  绘制模块   │                           │
│                  └─────────────┘                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**交互操作映射**：

| 功能 | 桌面端操作 | 移动端操作 |
|-----|-----------|-----------|
| 平移 | 鼠标拖拽 | 单指拖拽 |
| 缩放 | 滚轮/双击 | 双指捏合 |
| 旋转 | Shift+拖拽 | 双指旋转 |
| 要素选择 | 单击 | 单击 |
| 多选 | Ctrl+单击 | 长按+选择 |
| 框选 | 拖拽框选 | 两指框选 |

**显示模式支持**：

| 模式 | 说明 |
|-----|-----|
| 基础模式 | 显示基础航行要素 |
| 标准模式 | 显示标准航行要素 |
| 全部模式 | 显示所有要素 |
| 自定义模式 | 用户自定义显示内容 |

**对外接口**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| Pan | `void Pan(const Point& delta)` | 平移操作 |
| Zoom | `void Zoom(float scale, const Point& center)` | 缩放操作 |
| Rotate | `void Rotate(float angle)` | 旋转操作 |
| SelectFeature | `vector<Feature> SelectFeature(const Point& point)` | 选择要素 |
| SetDisplayMode | `void SetDisplayMode(DisplayMode mode)` | 设置显示模式 |
| SetDisplayStyle | `void SetDisplayStyle(DisplayStyle style)` | 设置显示样式 |
| UpdatePosition | `void UpdatePosition(const Position& pos)` | 更新定位 |
| Symbolize | `void Symbolize(Feature& feature)` | 符号化要素 |

**依赖模块**：
- draw模块：绘制功能（graph调用draw接口）
- layer模块：图层数据
- geom模块：几何操作

**技术实现要点**：
1. 统一事件抽象层，屏蔽平台差异
2. 手势识别和冲突处理
3. 操作惯性动画支持
4. 定位数据平滑处理
5. 样式切换无闪烁

---

### 3.7 数据解析模块 (DataParser)

**功能范围**：
- S57数据格式解析（.000文件）
- S101数据格式解析（GML/XML格式）
- S100系列数据解析（S-102/S-111/S-112等）
- 解析结果转换为内部数据结构

**不包含**：
- 数据存储（由DataStorage模块负责）
- 数据渲染（由draw模块负责）
- 数据校验（由安全合规模块负责）

**子模块划分**：

| 子模块 | 职责 | 说明 |
|--------|------|------|
| S57Parser | S57格式解析 | 解析.000文件，提取要素和属性 |
| S101Parser | S101格式解析 | 解析GML/XML格式数据 |
| S100Parser | S100系列解析 | 解析S-102/S-111等扩展格式 |
| DataConverter | 数据转换器 | 转换为内部统一数据结构 |

**对外接口**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| ParseChart | `ParseResult ParseChart(string filePath, ChartFormat format)` | 解析海图文件 |
| ParseFeature | `Feature ParseFeature(byte[] data, FeatureType type)` | 解析单个要素 |
| GetSupportedFormats | `List<ChartFormat> GetSupportedFormats()` | 获取支持的格式列表 |

**依赖模块**：
- GDAL/OGR库：底层解析支持
- CoordTrans模块：坐标系转换

**技术实现要点**：
1. 使用GDAL库作为底层解析引擎
2. 实现解析缓存机制，避免重复解析
3. 支持增量解析，按需加载要素
4. 解析错误处理和日志记录

**示例代码 - S57海图数据解析**：

```cpp
// S57Parser.h - S57数据解析器头文件
#ifndef S57_PARSER_H
#define S57_PARSER_H

#include <gdal/ogrsf_frmts.h>
#include <vector>
#include <memory>
#include "Geometry.h"
#include "Feature.h"
#include "CoordTrans.h"

namespace chart {

struct ParseResult {
    bool success;
    std::string errorMessage;
    int errorCode;
    std::vector<Feature> features;
    int totalFeatureCount;
    double parseTimeMs;
};

class S57Parser {
public:
    S57Parser();
    ~S57Parser();
    
    ParseResult ParseChart(const std::string& filePath);
    Feature ParseFeature(OGRFeature* ogrFeature);
    std::vector<ChartFormat> GetSupportedFormats() const;
    
private:
    Geometry* ParseGeometry(OGRGeometry* geom);
    AttributeMap ParseAttributes(OGRFeature* feature);
    FeatureType MapFeatureType(const std::string& typeName);
    
    std::unique_ptr<CoordTrans> m_coordTrans;
    std::map<std::string, FeatureType> m_typeMapping;
};

} // namespace chart

#endif // S57_PARSER_H
```

```cpp
// S57Parser.cpp - S57数据解析器实现
#include "S57Parser.h"
#include <chrono>
#include <spdlog/spdlog.h>

namespace chart {

ParseResult S57Parser::ParseChart(const std::string& filePath) {
    auto startTime = std::chrono::high_resolution_clock::now();
    ParseResult result;
    result.success = false;
    result.totalFeatureCount = 0;
    
    spdlog::info("开始解析S57文件: {}", filePath);
    
    GDALDataset* dataset = (GDALDataset*)GDALOpenEx(
        filePath.c_str(), 
        GDAL_OF_VECTOR, 
        nullptr, nullptr, nullptr
    );
    
    if (dataset == nullptr) {
        result.errorCode = ERR_FILE_NOT_FOUND;
        result.errorMessage = "无法打开文件: " + filePath;
        spdlog::error(result.errorMessage);
        return result;
    }
    
    int layerCount = dataset->GetLayerCount();
    spdlog::info("图层数量: {}", layerCount);
    
    for (int i = 0; i < layerCount; i++) {
        OGRLayer* layer = dataset->GetLayer(i);
        if (layer == nullptr) continue;
        
        layer->ResetReading();
        OGRFeature* feature;
        
        while ((feature = layer->GetNextFeature()) != nullptr) {
            Feature parsedFeature = ParseFeature(feature);
            if (parsedFeature.IsValid()) {
                result.features.push_back(parsedFeature);
                result.totalFeatureCount++;
            }
            OGRFeature::DestroyFeature(feature);
        }
    }
    
    GDALClose(dataset);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.parseTimeMs = std::chrono::duration<double, std::milli>(
        endTime - startTime
    ).count();
    
    result.success = true;
    spdlog::info("解析完成，共{}个要素，耗时{:.2f}ms", 
                 result.totalFeatureCount, result.parseTimeMs);
    
    return result;
}

Geometry* S57Parser::ParseGeometry(OGRGeometry* geom) {
    if (geom == nullptr) return nullptr;
    
    OGRwkbGeometryType type = geom->getGeometryType();
    
    switch (wkbFlatten(type)) {
        case wkbPoint: {
            OGRPoint* pt = geom->toPoint();
            return new Point(pt->getX(), pt->getY());
        }
        case wkbLineString: {
            OGRLineString* line = geom->toLineString();
            std::vector<Point> points;
            for (int i = 0; i < line->getNumPoints(); i++) {
                points.emplace_back(line->getX(i), line->getY(i));
            }
            return new Line(points);
        }
        case wkbPolygon: {
            OGRPolygon* poly = geom->toPolygon();
            OGRLinearRing* ring = poly->getExteriorRing();
            std::vector<Point> points;
            for (int i = 0; i < ring->getNumPoints(); i++) {
                points.emplace_back(ring->getX(i), ring->getY(i));
            }
            return new Area(points);
        }
        case wkbMultiPoint: {
            OGRMultiPoint* mpt = geom->toMultiPoint();
            std::vector<Point> points;
            for (int i = 0; i < mpt->getNumGeometries(); i++) {
                OGRPoint* pt = mpt->getGeometryRef(i)->toPoint();
                points.emplace_back(pt->getX(), pt->getY());
            }
            return new MultiPoint(points);
        }
        default:
            spdlog::warn("不支持的几何类型: {}", type);
            return nullptr;
    }
}

AttributeMap S57Parser::ParseAttributes(OGRFeature* feature) {
    AttributeMap attrs;
    
    OGRFeatureDefn* defn = feature->GetDefnRef();
    int fieldCount = defn->GetFieldCount();
    
    for (int i = 0; i < fieldCount; i++) {
        OGRFieldDefn* fieldDefn = defn->GetFieldDefn(i);
        std::string fieldName = fieldDefn->GetNameRef();
        
        if (feature->IsFieldSet(i)) {
            OGRFieldType fieldType = fieldDefn->GetType();
            
            switch (fieldType) {
                case OFTInteger:
                    attrs.SetValue(fieldName, feature->GetFieldAsInteger(i));
                    break;
                case OFTReal:
                    attrs.SetValue(fieldName, feature->GetFieldAsDouble(i));
                    break;
                case OFTString:
                    attrs.SetValue(fieldName, std::string(feature->GetFieldAsString(i)));
                    break;
                default:
                    attrs.SetValue(fieldName, std::string(feature->GetFieldAsString(i)));
            }
        }
    }
    
    return attrs;
}

Feature S57Parser::ParseFeature(OGRFeature* ogrFeature) {
    Feature feature;
    
    OGRGeometry* geom = ogrFeature->GetGeometryRef();
    if (geom == nullptr) {
        return feature;
    }
    
    Geometry* parsedGeom = ParseGeometry(geom);
    if (parsedGeom == nullptr) {
        return feature;
    }
    
    feature.SetGeometry(parsedGeom);
    feature.SetAttributes(ParseAttributes(ogrFeature));
    
    std::string featureType = ogrFeature->GetDefnRef()->GetName();
    feature.SetFeatureType(MapFeatureType(featureType));
    
    feature.SetValid(true);
    return feature;
}

FeatureType S57Parser::MapFeatureType(const std::string& typeName) {
    static const std::map<std::string, FeatureType> typeMap = {
        {"SOUNDG", FeatureType::SOUNDING},
        {"LIGHTS", FeatureType::LIGHT},
        {"BOYLAT", FeatureType::BUOY},
        {"DEPARE", FeatureType::DEPTH_AREA},
        {"COALNE", FeatureType::COASTLINE},
        {"NAVLNE", FeatureType::NAVLINE},
        {"WRECKS", FeatureType::WRECK},
        {"OBSTRN", FeatureType::OBSTRUCTION}
    };
    
    auto it = typeMap.find(typeName);
    if (it != typeMap.end()) {
        return it->second;
    }
    return FeatureType::UNKNOWN;
}

} // namespace chart
```

**示例代码 - 属性数据解析**：

```cpp
// AttributeParser.h - 属性解析器
namespace chart {

class AttributeParser {
public:
    enum class AttributeType {
        INTEGER,
        DOUBLE,
        STRING,
        LIST,
        UNKNOWN
    };
    
    struct AttributeValue {
        AttributeType type;
        int intValue;
        double doubleValue;
        std::string stringValue;
        std::vector<std::string> listValue;
    };
    
    using AttributeMap = std::map<std::string, AttributeValue>;
    
    static AttributeMap ParseS57Attributes(OGRFeature* feature);
    static std::string GetAttributeValue(const AttributeMap& attrs, 
                                         const std::string& name,
                                         const std::string& defaultValue = "");
    
    static int GetAttributeInt(const AttributeMap& attrs,
                               const std::string& name,
                               int defaultValue = 0);
    
    static double GetAttributeDouble(const AttributeMap& attrs,
                                     const std::string& name,
                                     double defaultValue = 0.0);
};

AttributeMap AttributeParser::ParseS57Attributes(OGRFeature* feature) {
    AttributeMap attrs;
    OGRFeatureDefn* defn = feature->GetDefnRef();
    
    for (int i = 0; i < defn->GetFieldCount(); i++) {
        OGRFieldDefn* fieldDefn = defn->GetFieldDefn(i);
        std::string fieldName = fieldDefn->GetNameRef();
        
        if (!feature->IsFieldSet(i)) continue;
        
        AttributeValue value;
        value.type = AttributeType::UNKNOWN;
        
        switch (fieldDefn->GetType()) {
            case OFTInteger:
                value.type = AttributeType::INTEGER;
                value.intValue = feature->GetFieldAsInteger(i);
                break;
            case OFTInteger64:
                value.type = AttributeType::INTEGER;
                value.intValue = static_cast<int>(feature->GetFieldAsInteger64(i));
                break;
            case OFTReal:
                value.type = AttributeType::DOUBLE;
                value.doubleValue = feature->GetFieldAsDouble(i);
                break;
            case OFTString:
                value.type = AttributeType::STRING;
                value.stringValue = feature->GetFieldAsString(i);
                break;
            case OFTStringList: {
                value.type = AttributeType::LIST;
                char** list = feature->GetFieldAsStringList(i);
                if (list) {
                    for (int j = 0; list[j] != nullptr; j++) {
                        value.listValue.push_back(list[j]);
                    }
                }
                break;
            }
            default:
                value.type = AttributeType::STRING;
                value.stringValue = feature->GetFieldAsString(i);
        }
        
        attrs[fieldName] = value;
    }
    
    return attrs;
}

std::string AttributeParser::GetAttributeValue(
    const AttributeMap& attrs,
    const std::string& name,
    const std::string& defaultValue) 
{
    auto it = attrs.find(name);
    if (it == attrs.end()) return defaultValue;
    
    switch (it->second.type) {
        case AttributeType::STRING:
            return it->second.stringValue;
        case AttributeType::INTEGER:
            return std::to_string(it->second.intValue);
        case AttributeType::DOUBLE:
            return std::to_string(it->second.doubleValue);
        default:
            return defaultValue;
    }
}

} // namespace chart
```

---

### 3.8 数据存储模块 (DataStorage)

**功能范围**：
- 海图数据持久化存储
- 增量更新支持
- 空间索引构建和维护
- 数据缓存管理

**与database模块的区别**：
- **database模块**：提供统一数据库访问接口，是底层基础设施
- **DataStorage模块**：基于database模块，提供海图数据的持久化存储、增量更新、缓存管理等业务层功能

**不包含**：
- 数据解析（由DataParser模块负责）
- 数据传输（由OfflineSvc模块负责）

**子模块划分**：

| 子模块 | 职责 | 说明 |
|--------|------|------|
| StorageManager | 存储管理器 | 管理数据库连接和事务 |
| SpatialIndex | 空间索引 | R树/四叉树索引管理 |
| CacheManager | 缓存管理 | 内存缓存和磁盘缓存 |
| UpdateManager | 更新管理 | 增量更新处理 |

**对外接口**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| StoreChart | `int StoreChart(ChartData data)` | 存储海图数据 |
| LoadChart | `ChartData LoadChart(int chartId)` | 加载海图数据 |
| QueryFeatures | `List<Feature> QueryFeatures(BoundingBox bbox, List<FeatureType> types)` | 空间查询 |
| UpdateChart | `bool UpdateChart(int chartId, UpdateData update)` | 增量更新 |

**依赖模块**：
- database模块：数据库访问接口
- DataParser模块：数据来源

**技术实现要点**：
1. 基于database模块的统一接口实现
2. 实现LRU缓存策略，热点数据常驻内存
3. 支持增量更新，减少数据传输量
4. 支持数据压缩存储，减少磁盘占用

**示例代码 - 海图数据存储到SpatiaLite**：

```cpp
// SpatiaLiteStorage.h - SpatiaLite存储管理器
#ifndef SPATIALITE_STORAGE_H
#define SPATIALITE_STORAGE_H

#include <sqlite3.h>
#include <spatialite/gaiageo.h>
#include <spatialite.h>
#include <vector>
#include <memory>
#include "Feature.h"
#include "Geometry.h"

namespace chart {

struct StorageConfig {
    std::string dbPath;
    int cacheSizeMB = 256;
    bool enableWAL = true;
    int pageSize = 4096;
};

struct QueryResult {
    bool success;
    std::string errorMessage;
    std::vector<Feature> features;
    int totalCount;
    double queryTimeMs;
};

class SpatiaLiteStorage {
public:
    SpatiaLiteStorage();
    ~SpatiaLiteStorage();
    
    bool Initialize(const StorageConfig& config);
    void Shutdown();
    
    int StoreChart(const std::string& chartName, 
                   const std::vector<Feature>& features);
    
    QueryResult QueryFeatures(const BoundingBox& bbox, 
                              const std::vector<FeatureType>& types = {});
    
    Feature LoadFeature(int featureId);
    bool UpdateFeature(const Feature& feature);
    bool DeleteFeature(int featureId);
    
    bool CreateSpatialIndex(const std::string& tableName);
    bool OptimizeStorage();
    
private:
    bool CreateTableSchema();
    bool InsertFeature(sqlite3_stmt* stmt, const Feature& feature);
    Geometry* ParseGeometryFromWKB(const unsigned char* wkb, int size);
    std::vector<unsigned char> GeometryToWKB(const Geometry* geom);
    
    sqlite3* m_db;
    void* m_spatialite;
    StorageConfig m_config;
    bool m_initialized;
};

} // namespace chart

#endif // SPATIALITE_STORAGE_H
```

```cpp
// SpatiaLiteStorage.cpp - SpatiaLite存储实现
#include "SpatiaLiteStorage.h"
#include <spdlog/spdlog.h>
#include <chrono>

namespace chart {

bool SpatiaLiteStorage::Initialize(const StorageConfig& config) {
    m_config = config;
    m_initialized = false;
    
    spatialite_init(0);
    
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    int rc = sqlite3_open_v2(config.dbPath.c_str(), &m_db, flags, nullptr);
    
    if (rc != SQLITE_OK) {
        spdlog::error("无法打开数据库: {}", sqlite3_errmsg(m_db));
        return false;
    }
    
    if (config.enableWAL) {
        char* errMsg = nullptr;
        rc = sqlite3_exec(m_db, "PRAGMA journal_mode=WAL;", nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            spdlog::warn("无法启用WAL模式: {}", errMsg);
            sqlite3_free(errMsg);
        }
    }
    
    std::string pageSizeSql = "PRAGMA page_size=" + std::to_string(config.pageSize) + ";";
    sqlite3_exec(m_db, pageSizeSql.c_str(), nullptr, nullptr, nullptr);
    
    std::string cacheSql = "PRAGMA cache_size=-" + std::to_string(config.cacheSizeMB * 1024) + ";";
    sqlite3_exec(m_db, cacheSql.c_str(), nullptr, nullptr, nullptr);
    
    if (!CreateTableSchema()) {
        spdlog::error("创建表结构失败");
        return false;
    }
    
    m_initialized = true;
    spdlog::info("SpatiaLite存储初始化成功: {}", config.dbPath);
    return true;
}

bool SpatiaLiteStorage::CreateTableSchema() {
    const char* createSql = R"(
        SELECT InitSpatialMetaData(1, 'XY');
        
        CREATE TABLE IF NOT EXISTS chart_files (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            file_path TEXT NOT NULL UNIQUE,
            file_name TEXT NOT NULL,
            format TEXT NOT NULL,
            version TEXT,
            parse_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            file_size INTEGER,
            checksum TEXT
        );
        
        CREATE TABLE IF NOT EXISTS charts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            chart_file_id INTEGER NOT NULL,
            chart_name TEXT NOT NULL,
            chart_number TEXT,
            scale INTEGER,
            crs TEXT DEFAULT 'EPSG:4326',
            FOREIGN KEY (chart_file_id) REFERENCES chart_files(id)
        );
        
        CREATE TABLE IF NOT EXISTS features (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            chart_id INTEGER NOT NULL,
            feature_id TEXT NOT NULL,
            feature_type TEXT NOT NULL,
            feature_name TEXT,
            geometry_type TEXT NOT NULL,
            attributes TEXT,
            s52_symbol TEXT,
            display_priority INTEGER DEFAULT 0,
            min_scale INTEGER,
            max_scale INTEGER,
            FOREIGN KEY (chart_id) REFERENCES charts(id)
        );
        
        SELECT AddGeometryColumn('features', 'geometry', 4326, 'GEOMETRY', 'XY');
        SELECT CreateSpatialIndex('features', 'geometry');
        
        CREATE INDEX IF NOT EXISTS idx_features_chart ON features(chart_id);
        CREATE INDEX IF NOT EXISTS idx_features_type ON features(feature_type);
        CREATE INDEX IF NOT EXISTS idx_features_priority ON features(display_priority);
    )";
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, createSql, nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        spdlog::error("创建表结构失败: {}", errMsg);
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

int SpatiaLiteStorage::StoreChart(const std::string& chartName,
                                   const std::vector<Feature>& features) {
    if (!m_initialized) return -1;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    int chartId = -1;
    sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
    
    const char* insertChartSql = "INSERT INTO charts (chart_name) VALUES (?);";
    sqlite3_stmt* chartStmt;
    sqlite3_prepare_v2(m_db, insertChartSql, -1, &chartStmt, nullptr);
    sqlite3_bind_text(chartStmt, 1, chartName.c_str(), -1, SQLITE_TRANSIENT);
    
    if (sqlite3_step(chartStmt) == SQLITE_DONE) {
        chartId = static_cast<int>(sqlite3_last_insert_rowid(m_db));
    }
    sqlite3_finalize(chartStmt);
    
    if (chartId < 0) {
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return -1;
    }
    
    const char* insertFeatureSql = R"(
        INSERT INTO features (chart_id, feature_id, feature_type, feature_name,
                             geometry_type, geometry, attributes, s52_symbol, 
                             display_priority, min_scale, max_scale)
        VALUES (?, ?, ?, ?, ?, MakePoint(?, ?, 4326), ?, ?, ?, ?, ?);
    )";
    
    sqlite3_stmt* featureStmt;
    sqlite3_prepare_v2(m_db, insertFeatureSql, -1, &featureStmt, nullptr);
    
    int insertedCount = 0;
    for (const auto& feature : features) {
        sqlite3_reset(featureStmt);
        sqlite3_clear_bindings(featureStmt);
        
        sqlite3_bind_int(featureStmt, 1, chartId);
        sqlite3_bind_text(featureStmt, 2, feature.GetId().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(featureStmt, 3, FeatureTypeToString(feature.GetType()).c_str(), 
                         -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(featureStmt, 4, feature.GetName().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(featureStmt, 5, GeometryTypeToString(feature.GetGeometryType()).c_str(),
                         -1, SQLITE_TRANSIENT);
        
        const Geometry* geom = feature.GetGeometry();
        if (geom && geom->GetType() == GeometryType::POINT) {
            const Point* pt = static_cast<const Point*>(geom);
            sqlite3_bind_double(featureStmt, 6, pt->GetX());
            sqlite3_bind_double(featureStmt, 7, pt->GetY());
        }
        
        std::string attrsJson = feature.AttributesToJson();
        sqlite3_bind_text(featureStmt, 8, attrsJson.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(featureStmt, 9, feature.GetS52Symbol().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(featureStmt, 10, feature.GetDisplayPriority());
        sqlite3_bind_int(featureStmt, 11, feature.GetMinScale());
        sqlite3_bind_int(featureStmt, 12, feature.GetMaxScale());
        
        if (sqlite3_step(featureStmt) == SQLITE_DONE) {
            insertedCount++;
        }
    }
    
    sqlite3_finalize(featureStmt);
    sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, nullptr);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    spdlog::info("存储海图完成: chartId={}, 要素数={}, 耗时{:.2f}ms", 
                 chartId, insertedCount, elapsedMs);
    
    return chartId;
}

QueryResult SpatiaLiteStorage::QueryFeatures(const BoundingBox& bbox,
                                              const std::vector<FeatureType>& types) {
    auto startTime = std::chrono::high_resolution_clock::now();
    QueryResult result;
    result.success = false;
    
    std::string sql = R"(
        SELECT id, feature_id, feature_type, feature_name, geometry_type,
               AsBinary(geometry) as geom_wkb, attributes, s52_symbol,
               display_priority, min_scale, max_scale
        FROM features
        WHERE rowid IN (
            SELECT rowid FROM SpatialIndex
            WHERE f_table_name = 'features'
            AND f_geometry_column = 'geometry'
            AND search_frame = BuildMbr(?, ?, ?, ?, 4326)
        )
    )";
    
    if (!types.empty()) {
        sql += " AND feature_type IN (";
        for (size_t i = 0; i < types.size(); i++) {
            if (i > 0) sql += ",";
            sql += "'" + FeatureTypeToString(types[i]) + "'";
        }
        sql += ")";
    }
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        result.errorMessage = sqlite3_errmsg(m_db);
        spdlog::error("查询失败: {}", result.errorMessage);
        return result;
    }
    
    sqlite3_bind_double(stmt, 1, bbox.minX);
    sqlite3_bind_double(stmt, 2, bbox.minY);
    sqlite3_bind_double(stmt, 3, bbox.maxX);
    sqlite3_bind_double(stmt, 4, bbox.maxY);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Feature feature;
        
        feature.SetId(std::to_string(sqlite3_column_int(stmt, 0)));
        feature.SetFeatureId(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        feature.SetType(StringToFeatureType(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))));
        feature.SetName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
        
        const void* wkbData = sqlite3_column_blob(stmt, 5);
        int wkbSize = sqlite3_column_bytes(stmt, 5);
        Geometry* geom = ParseGeometryFromWKB(static_cast<const unsigned char*>(wkbData), wkbSize);
        feature.SetGeometry(geom);
        
        const char* attrsJson = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        if (attrsJson) {
            feature.ParseAttributesFromJson(attrsJson);
        }
        
        feature.SetS52Symbol(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)));
        feature.SetDisplayPriority(sqlite3_column_int(stmt, 8));
        feature.SetMinScale(sqlite3_column_int(stmt, 9));
        feature.SetMaxScale(sqlite3_column_int(stmt, 10));
        
        result.features.push_back(feature);
        result.totalCount++;
    }
    
    sqlite3_finalize(stmt);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.queryTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    result.success = true;
    
    spdlog::debug("空间查询完成: 要素数={}, 耗时{:.2f}ms", 
                  result.totalCount, result.queryTimeMs);
    
    return result;
}

void SpatiaLiteStorage::Shutdown() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
    spatialite_cleanup();
    m_initialized = false;
    spdlog::info("SpatiaLite存储已关闭");
}

} // namespace chart
```

---

### 3.9 坐标转换模块 (CoordTrans)

**功能范围**：
- 多坐标系转换（WGS-84、CGCS2000、北京54等）
- 投影坐标与地理坐标转换
- 地方坐标系转换
- 坐标偏转处理（涉密区域）

**不包含**：
- 定位数据处理（由Position模块负责）
- 地图投影渲染（由draw模块负责）

**支持的坐标系**：

| 坐标系 | 说明 | 用途 |
|-------|-----|-----|
| WGS-84 | 全球定位坐标系 | GPS/北斗定位 |
| CGCS2000 | 中国大地坐标系 | 国内海图数据 |
| 北京54 | 旧版国内坐标系 | 历史数据兼容 |
| 地方坐标系 | 各地独立坐标系 | 特定区域数据 |

**坐标转换精度要求**：

| 转换类型 | 精度要求 |
|---------|---------|
| WGS-84 ↔ CGCS2000 | <0.1m |
| 投影坐标 ↔ 地理坐标 | <0.01m |
| 地方坐标 ↔ 国家坐标 | <0.5m |

**对外接口**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| Transform | `Point Transform(Point point, CoordSystem from, CoordSystem to)` | 坐标转换 |
| BatchTransform | `List<Point> BatchTransform(List<Point> points, CoordSystem from, CoordSystem to)` | 批量转换 |
| GetCoordSystems | `List<CoordSystem> GetCoordSystems()` | 获取支持的坐标系 |

**依赖模块**：
- PROJ库：底层坐标转换

**技术实现要点**：
1. 使用PROJ库作为底层转换引擎
2. 实现坐标转换缓存，避免重复计算
3. 支持自定义坐标系参数
4. 涉密区域坐标偏转处理

---

### 3.10 海图显示模块 (ChartDisplay)

**功能范围**：
- S52显示样式支持
- 日/夜模式切换
- 比例尺自适应显示
- 显示优先级管理
- 要素可见性控制

**与graph模块的区别**：
- **graph模块**：图形交互、定位显示、符号化，侧重用户交互
- **ChartDisplay模块**：S52样式、日/夜模式、比例尺自适应，侧重显示规则和样式管理等业务层逻辑

**不包含**：
- 用户交互处理（由graph模块负责）
- 图层管理（由LayerManager模块负责）

**显示样式支持**：

| 样式类型 | 说明 |
|---------|-----|
| S52样式 | 符合IHO S-52显示规范 |
| 自定义样式 | 支持用户自定义显示样式 |
| 日/夜模式 | 支持白天/夜晚显示模式 |
| 要素过滤 | 支持按类型、比例尺过滤显示 |

**显示比例尺级别**：

| 比例尺级别 | 显示内容 | 说明 |
|-----------|---------|-----|
| 1:10000以下 | 详细要素 | 港口、码头详细 |
| 1:10000-1:50000 | 航行要素 | 航道、航标、障碍物 |
| 1:50000-1:200000 | 概略要素 | 主要航道、岸线 |
| 1:200000以上 | 概览要素 | 海岸线、主要地标 |

**对外接口**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| SetDisplayStyle | `void SetDisplayStyle(DisplayStyle style)` | 设置显示样式 |
| SetDayNightMode | `void SetDayNightMode(DayNightMode mode)` | 设置日/夜模式 |
| GetDisplayConfig | `DisplayConfig GetDisplayConfig(int scale)` | 获取比例尺自适应配置 |
| SetDisplayPriority | `void SetDisplayPriority(FeatureType type, int priority)` | 设置显示优先级 |
| IsFeatureVisible | `bool IsFeatureVisible(FeatureType type, int scale)` | 判断要素是否可见 |

**依赖模块**：
- draw模块：绘制功能
- layer模块：图层数据

**技术实现要点**：
1. 实现S52显示规范
2. 日/夜模式颜色映射
3. 比例尺与显示级别映射
4. 显示优先级排序算法
5. 实现矢量渲染+栅格缓存混合模式
6. LOD技术优化大数据量渲染
7. 瓦片缓存机制提升响应速度
---

### 3.11 定位显示模块 (Position)

**功能范围**：
- 定位数据接入（手机定位、北斗设备、RTK设备）
- 船位实时显示
- 航向、航速显示
- 航行轨迹记录和回放

**不包含**：
- 定位数据处理（由外部设备/系统提供）
- 航线规划（由上层应用负责）

**定位数据源**：

| 数据源 | 接入方式 | 更新频率 | 适用用户 |
|-------|---------|---------|---------|
| 手机定位 | 系统API | 30秒/次 | 普通用户 |
| 北斗设备 | 串口/NMEA | 10秒/次 | VIP用户 |
| 差分定位 | RTK设备 | 1秒/次 | 高级VIP |

**定位精度等级**：

| 精度等级 | 定位方式 | 精度范围 | 适用场景 |
|---------|---------|---------|---------|
| 普通精度 | 手机GPS/北斗 | 5-10m | 一般导航 |
| 高精度 | 差分北斗 | 1-5m | 精细导航 |
| 厘米级 | RTK | <1m | 专业应用 |

**对外接口**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| UpdatePosition | `void UpdatePosition(Position position, Heading heading, Speed speed)` | 更新船位 |
| StartTrackRecording | `void StartTrackRecording()` | 开始轨迹记录 |
| StopTrackRecording | `TrackData StopTrackRecording()` | 停止轨迹记录 |
| PlaybackTrack | `void PlaybackTrack(TrackData track, PlaybackConfig config)` | 轨迹回放 |

**依赖模块**：
- draw模块：船位渲染
- CoordTrans模块：坐标转换

**技术实现要点**：
1. 多定位源抽象，统一接口
2. 定位数据平滑处理
3. 轨迹数据压缩存储
4. 轨迹回放动画支持

---

### 3.12 图层管理模块 (LayerManager)

**功能范围**：
- 图层开关控制
- 图层顺序调整
- 图层透明度调节
- 图层样式自定义
- 多图层协调管理

**与layer模块的区别**：
- **layer模块**：单个图层的数据封装，是数据结构层
- **LayerManager模块**：多图层的管理和协调，是管理层

**不包含**：
- 图层渲染（由draw模块负责）
- 图层数据管理（由DataStorage模块负责）

**数据分层架构**：

| 层级 | 数据类型 | 存储方式 | 更新频率 |
|-----|---------|---------|---------|
| 基础层 | 海岸线、陆地 | 本地存储 | 低频 |
| 航行层 | 航道、航标、障碍物 | 本地+增量更新 | 中频 |
| 动态层 | 水深、水位、气象 | 实时更新 | 高频 |
| 用户层 | 用户标记、航线 | 本地存储 | 按需 |

**对外接口**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| AddLayer | `void AddLayer(Layer layer)` | 添加图层 |
| RemoveLayer | `void RemoveLayer(string layerId)` | 移除图层 |
| SetLayerVisible | `void SetLayerVisible(string layerId, bool visible)` | 设置图层可见性 |
| SetLayerOpacity | `void SetLayerOpacity(string layerId, float opacity)` | 设置图层透明度 |
| ReorderLayer | `void ReorderLayer(string layerId, int index)` | 调整图层顺序 |
| GetActiveLayers | `List<Layer> GetActiveLayers()` | 获取活动图层列表 |

**依赖模块**：
- layer模块：图层数据结构
- draw模块：图层渲染

**技术实现要点**：
1. 图层状态管理
2. 图层渲染顺序控制
3. 图层样式继承和覆盖
4. 图层可见性优化

---

### 3.13 平台适配模块 (PlatformAdapt)

**功能范围**：
- 桌面端适配（Windows、Linux）
- 移动端适配（Android）
- Web端适配（Chrome、Firefox、Edge）
- 平台特性封装

**不包含**：
- 业务逻辑（由各业务模块负责）

**平台特性对比**：

| 功能 | 桌面端 | 移动端 | Web端 |
|-----|--------|--------|-------|
| 多窗口 | ✅ 支持 | ❌ 不支持 | ❌ 不支持 |
| 分屏显示 | ✅ 支持 | ❌ 不支持 | ✅ 支持 |
| 快捷键 | ✅ 支持 | ❌ 不支持 | ✅ 部分支持 |
| 打印输出 | ✅ 支持 | ❌ 不支持 | ✅ 支持 |
| 触摸优化 | ❌ 不支持 | ✅ 支持 | ✅ 支持 |
| 省电模式 | ❌ 不支持 | ✅ 支持 | ❌ 不支持 |
| 离线优先 | ✅ 支持 | ✅ 支持 | ✅ 支持 |
| 语音播报 | ✅ 支持 | ✅ 支持 | ✅ 支持 |

**对外接口**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| GetPlatform | `PlatformType GetPlatform()` | 获取当前平台类型 |
| GetScreenInfo | `ScreenInfo GetScreenInfo()` | 获取屏幕信息 |
| OpenFile | `string OpenFile(FileFilter filter)` | 打开文件对话框 |
| SaveFile | `string SaveFile(FileFilter filter)` | 保存文件对话框 |

**依赖模块**：
- Qt框架：跨平台GUI
- JavaFX：桌面应用框架

**技术实现要点**：
1. 平台抽象层设计
2. 条件编译控制平台特性
3. 平台特定优化
4. 平台兼容性测试

---

### 3.14 离线服务模块 (OfflineSvc)

**功能范围**：
- 离线数据存储管理
- 数据压缩和解压
- 数据清理和迁移
- 离线功能支持

**不包含**：
- 数据下载（由外部服务负责）
- 数据解析（由DataParser模块负责）

**离线功能支持**：

| 功能 | 离线支持 | 说明 |
|-----|---------|-----|
| 海图显示 | ✅ 支持 | 完全离线可用 |
| 要素查询 | ✅ 支持 | 本地数据查询 |
| 航线规划 | ✅ 支持 | 基于离线数据规划 |
| 定位显示 | ✅ 支持 | 使用本地定位 |
| 实时预警 | ❌ 不支持 | 需要网络连接 |
| 数据更新 | ❌ 不支持 | 需要网络连接 |

**对外接口**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| GetStorageInfo | `StorageInfo GetStorageInfo()` | 获取存储信息 |
| ClearCache | `void ClearCache()` | 清除缓存 |
| CompactData | `void CompactData()` | 压缩数据 |
| ExportData | `void ExportData(string path)` | 导出数据 |

**依赖模块**：
- DataStorage模块：数据存储
- database模块：数据库访问

**技术实现要点**：
1. 数据压缩算法选择（LZ4/ZSTD）
2. 存储空间管理
3. 数据迁移策略
4. 离线数据版本管理

### 3.15 安全合规模块 (Security)

**功能范围**：
- 数据加密存储
- 传输加密
- 访问控制
- 地理信息合规处理

**不包含**：
- 用户认证（由上层应用负责）
- 权限管理（由上层应用负责）

**安全需求**：

| 需求项 | 说明 | 实现方式 |
|-------|-----|---------|
| 数据加密 | 支持数据加密存储 | AES-256加密 |
| 传输加密 | HTTPS加密传输 | TLS 1.3 |
| 访问控制 | 基于角色的数据访问控制 | RBAC模型 |

**合规要求**：

| 要求 | 说明 | 实现方式 |
|-----|-----|---------|
| 地理信息保密 | 符合国家地理信息保密规定 | 数据分级管理 |
| 坐标偏转 | 涉密区域坐标偏转处理 | GCJ-02/BD-09偏转 |
| 数据脱敏 | 敏感数据脱敏处理 | 数据脱敏规则 |

**对外接口**：

| 接口名称 | 方法签名 | 说明 |
|----------|----------|------|
| EncryptData | `byte[] EncryptData(byte[] data)` | 加密数据 |
| DecryptData | `byte[] DecryptData(byte[] data)` | 解密数据 |
| ObfuscateCoord | `Point ObfuscateCoord(Point point)` | 坐标偏转 |
| CheckPermission | `bool CheckPermission(string resource, string action)` | 权限检查 |

**依赖模块**：
- 无外部依赖

**技术实现要点**：
1. 加密算法选择和密钥管理
2. 坐标偏转算法实现
3. 数据脱敏规则配置
4. 安全审计日志
---

## 4. 数据模型

### 4.1 实体关系图

```
┌─────────────────────────────────────────────────────────────┐
│                      实体关系图                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  [海图文件] ──1:N──> [图幅]                                │
│     chart_files        charts                              │
│                          │                                  │
│                          └──1:N──> [要素]                  │
│                                     features                │
│                                       │                      │
│                     ┌─────────────────┼─────────────────┐   │
│                     ▼                 ▼                 ▼   │
│                [点要素]          [线要素]          [面要素] │
│               point_feats      line_feats       area_feats  │
│                     │                 │                 │   │
│                     └─────────────────┼─────────────────┘   │
│                                       ▼                      │
│                                 [属性数据]                  │
│                                 attributes                  │
│                                                             │
│  [空间索引] ──N:1──> [要素]                                │
│  spatial_index          features                           │
│                                                             │
│  [轨迹数据] ──1:N──> [轨迹点]                              │
│    tracks              track_points                        │
│                                                             │
│  [用户标记] ──N:1──> [海图文件]                            │
│  user_marks            chart_files                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 数据库表设计

#### 4.2.1 海图文件表 (chart_files)

| 字段名 | 类型 | 约束 | 说明 |
|--------|------|------|------|
| id | INTEGER | PK | 主键 |
| file_path | TEXT | NN, UQ | 文件路径 |
| file_name | TEXT | NN | 文件名称 |
| format | TEXT | NN | 格式(S57/S101) |
| version | TEXT | | 数据版本 |
| parse_time | DATETIME | NN | 解析时间 |
| update_time | DATETIME | | 更新时间 |
| file_size | INTEGER | | 文件大小(字节) |
| checksum | TEXT | | 校验和 |

#### 4.2.2 图幅表 (charts)

| 字段名 | 类型 | 约束 | 说明 |
|--------|------|------|------|
| id | INTEGER | PK | 主键 |
| chart_file_id | INTEGER | FK, NN | 所属海图文件 |
| chart_name | TEXT | NN | 图幅名称 |
| chart_number | TEXT | | 图号 |
| scale | INTEGER | | 比例尺 |
| min_x | REAL | NN | 最小经度 |
| max_x | REAL | NN | 最大经度 |
| min_y | REAL | NN | 最小纬度 |
| max_y | REAL | NN | 最大纬度 |
| crs | TEXT | NN | 坐标系 |

#### 4.2.3 要素表 (features)

| 字段名 | 类型 | 约束 | 说明 |
|--------|------|------|------|
| id | INTEGER | PK | 主键 |
| chart_id | INTEGER | FK, NN | 所属图幅 |
| feature_id | TEXT | NN | 要素唯一标识 |
| feature_type | TEXT | NN | 要素类型代码 |
| feature_name | TEXT | | 要素名称 |
| geometry_type | TEXT | NN | 几何类型(POINT/LINE/AREA) |
| geometry | BLOB | NN | 几何数据(WKB格式) |
| attributes | TEXT | | 属性数据(JSON格式) |
| s52_symbol | TEXT | | S52符号代码 |
| display_priority | INTEGER | | 显示优先级 |
| min_scale | INTEGER | | 最小显示比例尺 |
| max_scale | INTEGER | | 最大显示比例尺 |

#### 4.2.4 空间索引表 (spatial_index)

| 字段名 | 类型 | 约束 | 说明 |
|--------|------|------|------|
| id | INTEGER | PK | 主键 |
| feature_id | INTEGER | FK, NN | 要素ID |
| min_x | REAL | NN | 最小X坐标 |
| max_x | REAL | NN | 最大X坐标 |
| min_y | REAL | NN | 最小Y坐标 |
| max_y | REAL | NN | 最大Y坐标 |
| tile_x | INTEGER | NN | 瓦片X索引 |
| tile_y | INTEGER | NN | 瓦片Y索引 |
| zoom_level | INTEGER | NN | 缩放级别 |

#### 4.2.5 轨迹表 (tracks)

| 字段名 | 类型 | 约束 | 说明 |
|--------|------|------|------|
| id | INTEGER | PK | 主键 |
| track_name | TEXT | NN | 轨迹名称 |
| start_time | DATETIME | NN | 开始时间 |
| end_time | DATETIME | | 结束时间 |
| start_point | TEXT | | 起点坐标(JSON) |
| end_point | TEXT | | 终点坐标(JSON) |
| total_distance | REAL | | 总距离(海里) |
| total_time | INTEGER | | 总时长(秒) |
| point_count | INTEGER | | 轨迹点数量 |

#### 4.2.6 轨迹点表 (track_points)

| 字段名 | 类型 | 约束 | 说明 |
|--------|------|------|------|
| id | INTEGER | PK | 主键 |
| track_id | INTEGER | FK, NN | 所属轨迹 |
| sequence | INTEGER | NN | 序号 |
| longitude | REAL | NN | 经度 |
| latitude | REAL | NN | 纬度 |
| timestamp | DATETIME | NN | 时间戳 |
| speed | REAL | | 速度(节) |
| heading | REAL | | 航向(度) |
| accuracy | REAL | | 精度(米) |

#### 4.2.7 用户标记表 (user_marks)

| 字段名 | 类型 | 约束 | 说明 |
|--------|------|------|------|
| id | INTEGER | PK | 主键 |
| chart_file_id | INTEGER | FK | 关联海图 |
| mark_type | TEXT | NN | 标记类型 |
| mark_name | TEXT | NN | 标记名称 |
| longitude | REAL | NN | 经度 |
| latitude | REAL | NN | 纬度 |
| description | TEXT | | 描述 |
| create_time | DATETIME | NN | 创建时间 |
| update_time | DATETIME | | 更新时间 |

### 4.3 索引策略

| 索引名 | 表 | 字段 | 类型 | 说明 |
|--------|------|------|------|------|
| idx_features_chart | features | chart_id | B-Tree | 按图幅查询要素 |
| idx_features_type | features | feature_type | B-Tree | 按类型查询要素 |
| idx_spatial_bbox | spatial_index | min_x, max_x, min_y, max_y | R-Tree | 空间范围查询 |
| idx_spatial_tile | spatial_index | tile_x, tile_y, zoom_level | B-Tree | 瓦片索引查询 |
| idx_tracks_time | tracks | start_time, end_time | B-Tree | 按时间查询轨迹 |
| idx_track_points_track | track_points | track_id, sequence | B-Tree | 按轨迹查询点 |

---

## 5. 接口设计

### 5.1 API列表

| 模块 | 接口名称 | 方法签名 | 说明 |
|------|----------|----------|------|
| DataParser | ParseChart | `ParseResult ParseChart(string filePath, ChartFormat format)` | 解析海图文件 |
| DataParser | ParseFeature | `Feature ParseFeature(byte[] data, FeatureType type)` | 解析单个要素 |
| DataParser | GetSupportedFormats | `List<ChartFormat> GetSupportedFormats()` | 获取支持的格式 |
| DataStorage | StoreChart | `int StoreChart(ChartData data)` | 存储海图数据 |
| DataStorage | LoadChart | `ChartData LoadChart(int chartId)` | 加载海图数据 |
| DataStorage | QueryFeatures | `List<Feature> QueryFeatures(BoundingBox bbox, List<FeatureType> types)` | 空间查询 |
| DataStorage | UpdateChart | `bool UpdateChart(int chartId, UpdateData update)` | 增量更新 |
| CoordTrans | Transform | `Point Transform(Point point, CoordSystem from, CoordSystem to)` | 坐标转换 |
| CoordTrans | BatchTransform | `List<Point> BatchTransform(List<Point> points, CoordSystem from, CoordSystem to)` | 批量转换 |
| RenderEngine | RenderChart | `void RenderChart(ChartData data, RenderConfig config, Viewport viewport)` | 渲染海图 |
| RenderEngine | SetStyle | `void SetStyle(StyleType style)` | 设置显示样式 |
| RenderEngine | SetDayNightMode | `void SetDayNightMode(DayNightMode mode)` | 设置日/夜模式 |
| Interaction | OnPan | `void OnPan(Point delta)` | 平移操作 |
| Interaction | OnZoom | `void OnZoom(float scale, Point center)` | 缩放操作 |
| Interaction | OnSelect | `List<Feature> OnSelect(Point point)` | 选择操作 |
| Position | UpdatePosition | `void UpdatePosition(Position position, Heading heading, Speed speed)` | 更新船位 |
| Position | StartTrackRecording | `void StartTrackRecording()` | 开始轨迹记录 |
| Position | PlaybackTrack | `void PlaybackTrack(TrackData track, PlaybackConfig config)` | 轨迹回放 |
| LayerManager | AddLayer | `void AddLayer(Layer layer)` | 添加图层 |
| LayerManager | SetLayerVisible | `void SetLayerVisible(string layerId, bool visible)` | 设置图层可见性 |
| Security | EncryptData | `byte[] EncryptData(byte[] data)` | 加密数据 |
| Security | ObfuscateCoord | `Point ObfuscateCoord(Point point)` | 坐标偏转 |

### 5.2 接口详情

#### 5.2.1 数据解析接口

**接口名称**: ParseChart

**方法签名**: `ParseResult ParseChart(string filePath, ChartFormat format)`

**输入参数**:

| 参数名 | 类型 | 必需 | 说明 |
|--------|------|------|------|
| filePath | string | 是 | 海图文件路径 |
| format | ChartFormat | 是 | 数据格式(S57/S101/S102等) |

**输出/返回值**:

| 字段名 | 类型 | 说明 |
|--------|------|------|
| success | bool | 是否成功 |
| chartData | ChartData | 解析后的海图数据 |
| errorCode | int | 错误码(失败时) |
| errorMessage | string | 错误信息(失败时) |

**异常处理**:

| 错误码 | 说明 | 处理建议 |
|--------|------|----------|
| ERR_FILE_NOT_FOUND | 文件不存在 | 检查文件路径 |
| ERR_FORMAT_UNSUPPORT | 格式不支持 | 使用支持的格式 |
| ERR_PARSE_FAILED | 解析失败 | 检查文件完整性 |
| ERR_FILE_CORRUPTED | 文件损坏 | 重新获取文件 |

**性能要求**:
- S57文件解析 < 500ms
- S101文件解析 < 1000ms

---

#### 5.2.2 空间查询接口

**接口名称**: QueryFeatures

**方法签名**: `List<Feature> QueryFeatures(BoundingBox bbox, List<FeatureType> types)`

**输入参数**:

| 参数名 | 类型 | 必需 | 说明 |
|--------|------|------|------|
| bbox | BoundingBox | 是 | 查询范围(经纬度) |
| types | List<FeatureType> | 否 | 要素类型过滤列表 |

**输出/返回值**:

| 字段名 | 类型 | 说明 |
|--------|------|------|
| features | List<Feature> | 要素列表 |
| totalCount | int | 总数量 |

**异常处理**:

| 错误码 | 说明 | 处理建议 |
|--------|------|----------|
| ERR_INVALID_BBOX | 无效的查询范围 | 检查坐标范围 |
| ERR_QUERY_TIMEOUT | 查询超时 | 缩小查询范围 |

**性能要求**:
- 空间查询 < 100ms

---

#### 5.2.3 渲染接口

**接口名称**: RenderChart

**方法签名**: `void RenderChart(ChartData data, RenderConfig config, Viewport viewport)`

**输入参数**:

| 参数名 | 类型 | 必需 | 说明 |
|--------|------|------|------|
| data | ChartData | 是 | 海图数据 |
| config | RenderConfig | 是 | 渲染配置 |
| viewport | Viewport | 是 | 视口信息 |

**RenderConfig 结构**:

| 字段名 | 类型 | 说明 |
|--------|------|------|
| styleType | StyleType | 样式类型(S52/CUSTOM) |
| dayNightMode | DayNightMode | 日/夜模式 |
| scale | int | 比例尺 |
| visibleLayers | List<string> | 可见图层列表 |
| opacity | Dictionary<string, float> | 图层透明度 |

**Viewport 结构**:

| 字段名 | 类型 | 说明 |
|--------|------|------|
| center | Point | 中心点坐标 |
| zoom | float | 缩放级别 |
| rotation | float | 旋转角度(度) |
| width | int | 视口宽度(像素) |
| height | int | 视口高度(像素) |

**性能要求**:
- 百万级要素渲染 < 300ms
- 交互帧率 ≥ 30fps

---

#### 5.2.4 定位更新接口

**接口名称**: UpdatePosition

**方法签名**: `void UpdatePosition(Position position, Heading heading, Speed speed)`

**输入参数**:

| 参数名 | 类型 | 必需 | 说明 |
|--------|------|------|------|
| position | Position | 是 | 经纬度坐标 |
| heading | Heading | 否 | 航向(度) |
| speed | Speed | 否 | 速度(节) |

**Position 结构**:

| 字段名 | 类型 | 说明 |
|--------|------|------|
| longitude | double | 经度 |
| latitude | double | 纬度 |
| altitude | double | 高度(米) |
| accuracy | double | 精度(米) |
| timestamp | datetime | 时间戳 |

**性能要求**:
- 定位更新响应 < 50ms

---

## 6. 非功能性需求

### 6.1 性能指标

| 指标分类 | 指标项 | 目标值 | 测试条件 |
|----------|--------|--------|----------|
| 渲染性能 | 首屏加载 | <300ms | 百万级要素 |
| 渲染性能 | 缩放响应 | <100ms | 单次缩放操作 |
| 渲染性能 | 平移响应 | <50ms | 单次平移操作 |
| 渲染性能 | 要素选择 | <50ms | 单击选择 |
| 渲染性能 | 图层切换 | <200ms | 切换图层显示 |
| 渲染性能 | 帧率 | ≥30fps | 交互状态 |
| 数据性能 | 数据加载 | <1s | 单个海图文件 |
| 数据性能 | 数据解析 | <500ms | S57文件 |
| 数据性能 | 空间查询 | <100ms | 范围查询 |
| 数据性能 | 增量更新 | <30s | 增量更新包 |
| 内存占用 | 桌面端 | <1GB | 正常使用 |
| 内存占用 | 移动端 | <500MB | 正常使用 |
| 启动时间 | 冷启动 | <3s | 首次启动 |
| 启动时间 | 热启动 | <1s | 后续启动 |

### 6.2 安全要求

| 要求 | 说明 | 实现方式 |
|------|------|----------|
| 数据加密 | 支持数据加密存储 | AES-256加密 |
| 传输加密 | HTTPS加密传输 | TLS 1.3 |
| 访问控制 | 基于角色的数据访问控制 | RBAC模型 |
| 审计日志 | 操作日志记录 | 日志模块 |
| 数据备份 | 数据备份恢复 | 备份模块 |

### 6.3 可扩展性要求

| 要求 | 说明 |
|------|------|
| 模块化设计 | 各模块独立，可单独升级 |
| 插件机制 | 支持第三方插件扩展 |
| 配置化 | 样式、规则可配置 |
| API开放 | 提供开放API供第三方调用 |

### 6.4 可用性要求

| 要求 | 说明 |
|------|------|
| 离线可用 | 核心功能支持离线使用 |
| 容错处理 | 异常情况优雅降级 |
| 数据恢复 | 支持数据恢复 |
| 多语言支持 | 支持中英文界面 |

---

## 7. 风险与约束

### 7.1 技术风险

| 风险 | 影响 | 可能性 | 应对措施 |
|------|------|--------|----------|
| S101标准仍在演进，可能存在兼容性问题 | 高 | 中 | 关注IHO标准更新，预留扩展接口 |
| OpenGL ES/WebGL跨平台渲染差异 | 中 | 高 | 封装渲染抽象层，隔离平台差异 |
| 大数据量渲染性能瓶颈 | 高 | 高 | 采用LOD技术、瓦片缓存、增量渲染 |
| 移动端内存限制 | 中 | 中 | 内存优化、数据分块加载 |
| 坐标转换精度问题 | 中 | 低 | 使用高精度转换算法，验证转换结果 |

### 7.2 资源约束

| 约束 | 说明 | 影响 |
|------|------|------|
| 开发人力 | 团队规模有限 | 影响开发进度 |
| 测试设备 | 多平台测试设备不足 | 影响测试覆盖 |
| 海图数据 | 测试数据获取受限 | 影响功能验证 |

### 7.3 时间约束

| 里程碑 | 时间节点 | 交付物 |
|--------|----------|--------|
| M1: 核心模块完成 | T+2月 | 数据解析、存储、渲染模块 |
| M2: 功能模块完成 | T+4月 | 交互、定位、图层模块 |
| M3: 平台适配完成 | T+5月 | 桌面端、移动端、Web端 |
| M4: 测试验收完成 | T+6月 | 测试报告、验收文档 |

### 7.4 外部依赖

| 依赖 | 说明 | 风险 |
|------|------|------|
| GDAL库 | 数据解析依赖 | 版本兼容性 |
| PROJ库 | 坐标转换依赖 | 版本兼容性 |
| Qt框架 | GUI框架依赖 | 许可证合规 |
| IHO标准 | 数据标准依赖 | 标准变更 |

---

## 8. 附录

### 8.1 术语表

| 术语 | 英文 | 说明 |
|------|------|------|
| S57 | IHO S-57 | IHO电子海图数据标准 |
| S101 | IHO S-101 | IHO新一代电子海图数据标准 |
| S52 | IHO S-52 | IHO电子海图显示规范 |
| S100 | IHO S-100 | IHO通用海图数据框架 |
| ENC | Electronic Navigational Chart | 电子航海图 |
| WGS-84 | World Geodetic System 1984 | 全球定位坐标系 |
| CGCS2000 | China Geodetic Coordinate System 2000 | 中国大地坐标系 |
| RTK | Real-Time Kinematic | 实时动态差分定位 |
| LOD | Level of Detail | 细节层次 |
| R树 | R-Tree | 空间索引数据结构 |

### 8.2 参考资料

| 资料 | 说明 |
|------|------|
| IHO S-57标准 | 电子海图数据标准 |
| IHO S-101标准 | 新一代电子海图数据标准 |
| IHO S-52标准 | 电子海图显示规范 |
| IHO S-100标准 | 通用海图数据框架 |
| GDAL文档 | GDAL库使用文档 |
| PROJ文档 | PROJ库使用文档 |
| Qt文档 | Qt框架使用文档 |
| OpenGL ES规范 | OpenGL ES API规范 |
| WebGL规范 | WebGL API规范 |

### 8.3 S57主要物标分类

| 类别 | 物标代码 | 物标名称 | 说明 |
|-----|---------|---------|-----|
| 测量数据 | DEPARE | 深度区域 | 水深数据 |
| 测量数据 | DEPCNT | 等深线 | 水深等值线 |
| 测量数据 | SOUNDG | 水深点 | 离散水深值 |
| 航行障碍 | WRECKS | 沉船 | 沉船位置信息 |
| 航行障碍 | OBSTRN | 障碍物 | 水下障碍物 |
| 航行障碍 | UWTROC | 水下岩石 | 岩石位置 |
| 航标设施 | BCNCAR | 基点浮标 | 航道标记 |
| 航标设施 | LIGHTS | 灯光 | 灯塔、灯桩 |
| 航标设施 | BOYCAR | 浮标 | 各类浮标 |
| 人工设施 | BRIDGE | 桥梁 | 跨江桥梁 |
| 人工设施 | PONTON | 浮码头 | 浮动设施 |
| 航道信息 | NAVLNE | 航道线 | 推荐航线 |

### 8.4 S100系列数据标准

| 标准 | 用途 | 说明 |
|-----|-----|-----|
| S-100 | 通用海图数据框架 | 基础标准 |
| S-101 | 电子航海图 | 替代S57 |
| S-102 | 高密度水深数据 | 高精度水深 |
| S-111 | 表层海流数据 | 海流信息 |
| S-112 | 水位数据 | 潮汐水位 |
| S-124 | 航行通告 | 航道通告 |

---

## 9. 测试计划

### 9.1 测试策略概述

**测试目标**：
- 确保各模块功能正确性
- 验证系统性能满足需求
- 保证跨平台兼容性
- 验证数据安全合规性

**测试框架选型**：

| 测试类型 | 框架/工具 | 用途 |
|---------|----------|------|
| 单元测试 | Google Test (gtest) | C++模块单元测试 |
| 单元测试 | JUnit 5 | Java/JNI层测试 |
| 集成测试 | Google Mock + gtest | 模块集成测试 |
| 性能测试 | Google Benchmark | 性能基准测试 |
| UI测试 | Qt Test | Qt界面测试 |
| 覆盖率 | gcov/lcov | 代码覆盖率统计 |

### 9.2 单元测试计划

#### 9.2.1 几何模块 (geom) 单元测试

| 测试类 | 测试用例 | 预期结果 | 优先级 |
|--------|----------|----------|--------|
| PointTest | CreatePoint | 正确创建点对象 | P0 |
| PointTest | DistanceCalculation | 两点距离计算正确 | P0 |
| PointTest | CoordinateTransform | 坐标转换正确 | P1 |
| LineTest | CreateLine | 正确创建线对象 | P0 |
| LineTest | LengthCalculation | 线长度计算正确 | P0 |
| LineTest | Interpolation | 线上插值点正确 | P1 |
| AreaTest | CreateArea | 正确创建面对象 | P0 |
| AreaTest | AreaCalculation | 面积计算正确 | P0 |
| AreaTest | ContainsPoint | 点包含判断正确 | P1 |
| GeometryOpTest | BufferOperation | 缓冲区分析正确 | P1 |
| GeometryOpTest | IntersectOperation | 相交运算正确 | P1 |
| GeometryOpTest | UnionOperation | 合并运算正确 | P1 |

```cpp
// geom_test.cpp - 几何模块单元测试示例
#include <gtest/gtest.h>
#include "Geometry.h"

namespace chart {
namespace test {

class PointTest : public ::testing::Test {
protected:
    void SetUp() override {
        point1 = new Point(120.5, 31.2);
        point2 = new Point(120.6, 31.3);
    }
    
    void TearDown() override {
        delete point1;
        delete point2;
    }
    
    Point* point1;
    Point* point2;
};

TEST_F(PointTest, CreatePoint) {
    EXPECT_DOUBLE_EQ(point1->GetX(), 120.5);
    EXPECT_DOUBLE_EQ(point1->GetY(), 31.2);
}

TEST_F(PointTest, DistanceCalculation) {
    double distance = point1->Distance(*point2);
    EXPECT_NEAR(distance, 14321.5, 10.0);
}

TEST_F(PointTest, CoordinateTransform) {
    Point transformed = point1->Transform(CoordSystem::WGS84, CoordSystem::CGCS2000);
    EXPECT_NEAR(transformed.GetX(), 120.5, 0.0001);
    EXPECT_NEAR(transformed.GetY(), 31.2, 0.0001);
}

class LineTest : public ::testing::Test {
protected:
    Line* line;
    
    void SetUp() override {
        std::vector<Point> points = {
            Point(0, 0), Point(100, 0), Point(100, 100)
        };
        line = new Line(points);
    }
    
    void TearDown() override {
        delete line;
    }
};

TEST_F(LineTest, CreateLine) {
    EXPECT_EQ(line->GetPointCount(), 3);
}

TEST_F(LineTest, LengthCalculation) {
    double length = line->Length();
    EXPECT_DOUBLE_EQ(length, 200.0);
}

TEST_F(LineTest, Interpolation) {
    Point midPoint = line->Interpolate(0.5);
    EXPECT_DOUBLE_EQ(midPoint.GetX(), 100.0);
    EXPECT_DOUBLE_EQ(midPoint.GetY(), 50.0);
}

class AreaTest : public ::testing::Test {
protected:
    Area* area;
    
    void SetUp() override {
        std::vector<Point> points = {
            Point(0, 0), Point(100, 0), Point(100, 100), Point(0, 100)
        };
        area = new Area(points);
    }
    
    void TearDown() override {
        delete area;
    }
};

TEST_F(AreaTest, CreateArea) {
    EXPECT_EQ(area->GetPointCount(), 4);
}

TEST_F(AreaTest, AreaCalculation) {
    double areaValue = area->Area();
    EXPECT_DOUBLE_EQ(areaValue, 10000.0);
}

TEST_F(AreaTest, ContainsPoint) {
    Point inside(50, 50);
    Point outside(150, 150);
    
    EXPECT_TRUE(area->Contains(inside));
    EXPECT_FALSE(area->Contains(outside));
}

} // namespace test
} // namespace chart
```

#### 9.2.2 数据解析模块 (DataParser) 单元测试

| 测试类 | 测试用例 | 预期结果 | 优先级 |
|--------|----------|----------|--------|
| S57ParserTest | ParseValidFile | 正确解析有效S57文件 | P0 |
| S57ParserTest | ParseInvalidFile | 返回错误码 | P0 |
| S57ParserTest | ParseGeometry | 几何数据正确转换 | P0 |
| S57ParserTest | ParseAttributes | 属性数据正确解析 | P0 |
| S57ParserTest | ParseLargeFile | 大文件解析不超时 | P1 |
| S101ParserTest | ParseValidFile | 正确解析有效S101文件 | P0 |
| S101ParserTest | ParseGMLGeometry | GML几何正确转换 | P1 |

```cpp
// DataParser_test.cpp - 数据解析模块单元测试示例
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "S57Parser.h"
#include "AttributeParser.h"

namespace chart {
namespace test {

class S57ParserTest : public ::testing::Test {
protected:
    S57Parser parser;
    std::string testFilePath = "test_data/CN12345.000";
};

TEST_F(S57ParserTest, ParseValidFile) {
    ParseResult result = parser.ParseChart(testFilePath);
    
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.totalFeatureCount, 0);
    EXPECT_LT(result.parseTimeMs, 500.0);
}

TEST_F(S57ParserTest, ParseInvalidFile) {
    ParseResult result = parser.ParseChart("nonexistent.000");
    
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.errorCode, ERR_FILE_NOT_FOUND);
}

TEST_F(S57ParserTest, ParseGeometry) {
    ParseResult result = parser.ParseChart(testFilePath);
    ASSERT_TRUE(result.success);
    
    for (const auto& feature : result.features) {
        const Geometry* geom = feature.GetGeometry();
        ASSERT_NE(geom, nullptr);
        
        GeometryType type = geom->GetType();
        EXPECT_TRUE(type == GeometryType::POINT ||
                    type == GeometryType::LINE ||
                    type == GeometryType::AREA ||
                    type == GeometryType::MULTIPOINT ||
                    type == GeometryType::MULTILINE ||
                    type == GeometryType::MULTIAREA);
    }
}

TEST_F(S57ParserTest, ParseAttributes) {
    ParseResult result = parser.ParseChart(testFilePath);
    ASSERT_TRUE(result.success);
    
    bool hasAttributes = false;
    for (const auto& feature : result.features) {
        const AttributeMap& attrs = feature.GetAttributes();
        if (!attrs.IsEmpty()) {
            hasAttributes = true;
            break;
        }
    }
    EXPECT_TRUE(hasAttributes);
}

TEST_F(S57ParserTest, ParseLargeFile) {
    std::string largeFilePath = "test_data/LARGE_CHART.000";
    ParseResult result = parser.ParseChart(largeFilePath);
    
    EXPECT_TRUE(result.success);
    EXPECT_LT(result.parseTimeMs, 5000.0);
}

} // namespace test
} // namespace chart
```

#### 9.2.3 数据存储模块 (DataStorage) 单元测试

| 测试类 | 测试用例 | 预期结果 | 优先级 |
|--------|----------|----------|--------|
| SpatiaLiteTest | Initialize | 数据库初始化成功 | P0 |
| SpatiaLiteTest | StoreChart | 海图数据存储成功 | P0 |
| SpatiaLiteTest | QueryFeatures | 空间查询返回正确结果 | P0 |
| SpatiaLiteTest | CreateSpatialIndex | 空间索引创建成功 | P0 |
| SpatiaLiteTest | TransactionRollback | 事务回滚正确 | P1 |
| SpatiaLiteTest | ConcurrentAccess | 并发访问正确处理 | P1 |

```cpp
// DataStorage_test.cpp - 数据存储模块单元测试示例
#include <gtest/gtest.h>
#include "SpatiaLiteStorage.h"
#include <thread>
#include <vector>

namespace chart {
namespace test {

class SpatiaLiteTest : public ::testing::Test {
protected:
    SpatiaLiteStorage storage;
    StorageConfig config;
    
    void SetUp() override {
        config.dbPath = ":memory:";
        config.cacheSizeMB = 64;
        config.enableWAL = false;
        
        ASSERT_TRUE(storage.Initialize(config));
    }
    
    void TearDown() override {
        storage.Shutdown();
    }
    
    std::vector<Feature> CreateTestFeatures(int count) {
        std::vector<Feature> features;
        for (int i = 0; i < count; i++) {
            Feature f;
            f.SetId(std::to_string(i));
            f.SetType(FeatureType::SOUNDING);
            f.SetGeometry(new Point(120.0 + i * 0.001, 31.0 + i * 0.001));
            features.push_back(f);
        }
        return features;
    }
};

TEST_F(SpatiaLiteTest, Initialize) {
    EXPECT_TRUE(storage.IsInitialized());
}

TEST_F(SpatiaLiteTest, StoreChart) {
    auto features = CreateTestFeatures(100);
    int chartId = storage.StoreChart("TestChart", features);
    
    EXPECT_GT(chartId, 0);
}

TEST_F(SpatiaLiteTest, QueryFeatures) {
    auto features = CreateTestFeatures(1000);
    storage.StoreChart("TestChart", features);
    
    BoundingBox bbox = {120.0, 31.0, 120.5, 31.5};
    QueryResult result = storage.QueryFeatures(bbox);
    
    EXPECT_TRUE(result.success);
    EXPECT_GT(result.totalCount, 0);
    EXPECT_LT(result.queryTimeMs, 100.0);
}

TEST_F(SpatiaLiteTest, CreateSpatialIndex) {
    auto features = CreateTestFeatures(100);
    storage.StoreChart("TestChart", features);
    
    EXPECT_TRUE(storage.CreateSpatialIndex("features"));
}

TEST_F(SpatiaLiteTest, TransactionRollback) {
    auto features = CreateTestFeatures(10);
    int chartId = storage.StoreChart("TestChart", features);
    
    Feature invalidFeature;
    invalidFeature.SetGeometry(nullptr);
    
    bool result = storage.UpdateFeature(invalidFeature);
    EXPECT_FALSE(result);
    
    QueryResult queryResult = storage.QueryFeatures({120.0, 31.0, 121.0, 32.0});
    EXPECT_EQ(queryResult.totalCount, 10);
}

TEST_F(SpatiaLiteTest, ConcurrentAccess) {
    auto features = CreateTestFeatures(1000);
    storage.StoreChart("TestChart", features);
    
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&]() {
            BoundingBox bbox = {120.0, 31.0, 120.5, 31.5};
            QueryResult result = storage.QueryFeatures(bbox);
            if (result.success) {
                successCount++;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    EXPECT_EQ(successCount, 10);
}

} // namespace test
} // namespace chart
```

#### 9.2.4 绘制模块 (draw) 单元测试

| 测试类 | 测试用例 | 预期结果 | 优先级 |
|--------|----------|----------|--------|
| ChartRendererTest | Initialize | 渲染器初始化成功 | P0 |
| ChartRendererTest | RenderEmptyViewport | 空视口渲染正确 | P0 |
| ChartRendererTest | RenderFeatures | 要素渲染正确 | P0 |
| ChartRendererTest | DayNightMode | 日夜模式切换正确 | P1 |
| ChartRendererTest | LODRendering | LOD渲染正确 | P1 |
| ChartRendererTest | ExportImage | 图片导出成功 | P1 |

### 9.3 集成测试计划

#### 9.3.1 模块集成测试

| 测试场景 | 测试内容 | 验证点 | 优先级 |
|---------|----------|--------|--------|
| 解析-存储集成 | S57解析后存储到数据库 | 数据完整性、一致性 | P0 |
| 存储-渲染集成 | 从数据库读取数据渲染 | 渲染正确性、性能 | P0 |
| 解析-存储-渲染全流程 | 完整数据处理流程 | 端到端正确性 | P0 |
| 多图层集成 | 多图层叠加渲染 | 图层顺序、透明度 | P1 |
| 定位-显示集成 | 定位数据实时显示 | 实时性、准确性 | P1 |

```cpp
// Integration_test.cpp - 集成测试示例
#include <gtest/gtest.h>
#include "S57Parser.h"
#include "SpatiaLiteStorage.h"
#include "ChartRenderer.h"

namespace chart {
namespace test {

class IntegrationTest : public ::testing::Test {
protected:
    S57Parser* parser;
    SpatiaLiteStorage* storage;
    ChartRenderer* renderer;
    
    void SetUp() override {
        parser = new S57Parser();
        
        storage = new SpatiaLiteStorage();
        StorageConfig config;
        config.dbPath = ":memory:";
        storage->Initialize(config);
        
        renderer = new ChartRenderer();
        RenderConfig renderConfig;
        renderer->Initialize(storage, renderConfig);
    }
    
    void TearDown() override {
        delete renderer;
        storage->Shutdown();
        delete storage;
        delete parser;
    }
};

TEST_F(IntegrationTest, ParseStoreRenderFlow) {
    ParseResult parseResult = parser->ParseChart("test_data/CN12345.000");
    ASSERT_TRUE(parseResult.success);
    
    int chartId = storage->StoreChart("CN12345", parseResult.features);
    ASSERT_GT(chartId, 0);
    
    Viewport viewport;
    viewport.centerX = 120.5;
    viewport.centerY = 31.2;
    viewport.scale = 5000;
    viewport.width = 1024;
    viewport.height = 768;
    
    QPixmap pixmap = renderer->RenderToPixmap(viewport, QSize(1024, 768));
    EXPECT_FALSE(pixmap.isNull());
}

TEST_F(IntegrationTest, DataIntegrity) {
    ParseResult parseResult = parser->ParseChart("test_data/CN12345.000");
    ASSERT_TRUE(parseResult.success);
    
    int originalCount = parseResult.totalFeatureCount;
    
    int chartId = storage->StoreChart("CN12345", parseResult.features);
    ASSERT_GT(chartId, 0);
    
    BoundingBox bbox = {120.0, 31.0, 121.0, 32.0};
    QueryResult queryResult = storage->QueryFeatures(bbox);
    
    EXPECT_EQ(queryResult.totalCount, originalCount);
}

TEST_F(IntegrationTest, PerformanceBenchmark) {
    ParseResult parseResult = parser->ParseChart("test_data/LARGE_CHART.000");
    ASSERT_TRUE(parseResult.success);
    EXPECT_LT(parseResult.parseTimeMs, 1000.0);
    
    auto storeStart = std::chrono::high_resolution_clock::now();
    int chartId = storage->StoreChart("LARGE", parseResult.features);
    auto storeEnd = std::chrono::high_resolution_clock::now();
    double storeTime = std::chrono::duration<double, std::milli>(storeEnd - storeStart).count();
    EXPECT_LT(storeTime, 5000.0);
    
    auto queryStart = std::chrono::high_resolution_clock::now();
    QueryResult queryResult = storage->QueryFeatures({120.0, 31.0, 121.0, 32.0});
    auto queryEnd = std::chrono::high_resolution_clock::now();
    double queryTime = std::chrono::duration<double, std::milli>(queryEnd - queryStart).count();
    EXPECT_LT(queryTime, 100.0);
}

} // namespace test
} // namespace chart
```

#### 9.3.2 平台兼容性测试

| 平台 | 测试项 | 验证点 |
|------|--------|--------|
| Windows 10/11 | 功能测试 | 所有功能正常 |
| Windows 10/11 | 性能测试 | 满足性能指标 |
| Linux (Ubuntu 20.04+) | 功能测试 | 所有功能正常 |
| Linux (Ubuntu 20.04+) | 性能测试 | 满足性能指标 |
| Android 10+ | 功能测试 | 核心功能正常 |
| Android 10+ | 性能测试 | 移动端性能指标 |

### 9.4 性能测试计划

#### 9.4.1 性能基准测试

| 测试项 | 测试方法 | 目标值 | 验收标准 |
|--------|----------|--------|----------|
| S57文件解析 | BenchmarkParse | <500ms | P95 < 600ms |
| 数据存储 | BenchmarkStore | <100ms/1000条 | P95 < 150ms |
| 空间查询 | BenchmarkQuery | <100ms | P95 < 150ms |
| 首屏渲染 | BenchmarkRender | <300ms | P95 < 400ms |
| 缩放响应 | BenchmarkZoom | <100ms | P95 < 150ms |
| 平移响应 | BenchmarkPan | <50ms | P95 < 80ms |
| 内存占用 | BenchmarkMemory | <500MB | 峰值 < 600MB |

```cpp
// Performance_test.cpp - 性能基准测试示例
#include <benchmark/benchmark.h>
#include "S57Parser.h"
#include "SpatiaLiteStorage.h"
#include "ChartRenderer.h"

static void BM_ParseChart(benchmark::State& state) {
    S57Parser parser;
    for (auto _ : state) {
        ParseResult result = parser.ParseChart("test_data/CN12345.000");
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_ParseChart)->Unit(benchmark::kMillisecond);

static void BM_StoreFeatures(benchmark::State& state) {
    SpatiaLiteStorage storage;
    StorageConfig config;
    config.dbPath = ":memory:";
    storage.Initialize(config);
    
    std::vector<Feature> features;
    for (int i = 0; i < 1000; i++) {
        Feature f;
        f.SetGeometry(new Point(120.0 + i * 0.001, 31.0 + i * 0.001));
        features.push_back(f);
    }
    
    for (auto _ : state) {
        int chartId = storage.StoreChart("Test", features);
        benchmark::DoNotOptimize(chartId);
    }
}
BENCHMARK(BM_StoreFeatures)->Unit(benchmark::kMillisecond);

static void BM_QueryFeatures(benchmark::State& state) {
    SpatiaLiteStorage storage;
    StorageConfig config;
    config.dbPath = ":memory:";
    storage.Initialize(config);
    
    std::vector<Feature> features;
    for (int i = 0; i < 10000; i++) {
        Feature f;
        f.SetGeometry(new Point(120.0 + i * 0.001, 31.0 + i * 0.001));
        features.push_back(f);
    }
    storage.StoreChart("Test", features);
    
    BoundingBox bbox = {120.0, 31.0, 120.5, 31.5};
    for (auto _ : state) {
        QueryResult result = storage.QueryFeatures(bbox);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_QueryFeatures)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
```

### 9.5 测试覆盖率要求

| 模块 | 行覆盖率要求 | 分支覆盖率要求 | 关键路径覆盖 |
|------|-------------|---------------|-------------|
| geom | ≥80% | ≥70% | 100% |
| database | ≥85% | ≥75% | 100% |
| feature | ≥80% | ≥70% | 100% |
| layer | ≥80% | ≥70% | 100% |
| draw | ≥75% | ≥65% | 100% |
| graph | ≥75% | ≥65% | 100% |
| DataParser | ≥85% | ≥75% | 100% |
| DataStorage | ≥85% | ≥75% | 100% |
| CoordTrans | ≥90% | ≥80% | 100% |

### 9.6 测试执行计划

| 阶段 | 时间 | 测试内容 | 通过标准 |
|------|------|----------|----------|
| 单元测试 | 开发过程中 | 各模块单元测试 | 覆盖率达标，全部通过 |
| 集成测试 | 模块完成后 | 模块集成测试 | 全部通过 |
| 系统测试 | 功能完成后 | 端到端测试 | 全部通过 |
| 性能测试 | 系统稳定后 | 性能基准测试 | 满足性能指标 |
| 验收测试 | 发布前 | 验收测试 | 满足验收标准 |

---

## 10. 验收标准

### 9.1 功能验收

| 验收项 | 验收标准 | 验收方法 |
|-------|---------|---------|
| 数据解析 | 正确解析S57/S101数据 | 单元测试+集成测试 |
| 渲染显示 | 符合S52显示规范 | 视觉检查+自动化测试 |
| 交互操作 | 平移/缩放/旋转流畅 | 性能测试 |
| 定位显示 | 正确显示船位和轨迹 | 功能测试 |
| 多端一致 | 各端功能一致 | 兼容性测试 |
| 离线功能 | 核心功能离线可用 | 离线测试 |

### 9.2 性能验收

| 验收项 | 验收标准 | 测试环境 |
|-------|---------|---------|
| 渲染性能 | 百万级要素<300ms | Windows: i5/16GB/2GB显卡 |
| 内存占用 | 移动端<500MB | Android: 8核/6GB |
| 启动时间 | <3s | 标准配置 |
| 响应时间 | 交互响应<100ms | 标准配置 |

### 9.3 安全验收

| 验收项 | 验收标准 | 验收方法 |
|-------|---------|---------|
| 数据加密 | 敏感数据加密存储 | 安全审计 |
| 传输加密 | HTTPS传输 | 安全扫描 |
| 访问控制 | 权限正确控制 | 权限测试 |
| 合规性 | 符合地理信息保密规定 | 合规审计 |

---

## 11. 环境配置说明

### 11.1 开发环境配置

#### 11.1.1 硬件要求

| 配置项 | 最低要求 | 推荐配置 |
|--------|----------|----------|
| CPU | Intel i5 4核 / AMD Ryzen 5 | Intel i7 8核 / AMD Ryzen 7 |
| 内存 | 16GB | 32GB |
| 硬盘 | 256GB SSD | 512GB NVMe SSD |
| 显卡 | 集成显卡 | NVIDIA GTX 1660 / AMD RX 560 |
| 显示器 | 1920x1080 | 2560x1440 双显示器 |

#### 11.1.2 软件环境

| 软件 | 版本要求 | 用途 |
|------|----------|------|
| 操作系统 | Windows 10/11 或 Ubuntu 20.04+ | 开发平台 |
| IDE | Visual Studio 2022 / CLion 2023+ | C++开发 |
| IDE | IntelliJ IDEA 2023+ | Java开发 |
| JDK | OpenJDK 11+ | Java运行环境 |
| CMake | 3.16+ | 构建工具 |
| Qt | 5.12+ (推荐5.15) | GUI框架 |
| GDAL | 3.4+ | 数据解析 |
| PROJ | 8.0+ | 坐标转换 |
| SQLite | 3.35+ | 本地数据库 |
| SpatiaLite | 5.0+ | 空间扩展 |
| Git | 2.30+ | 版本控制 |

#### 11.1.3 开发工具配置

```yaml
# 开发环境配置示例 (dev-config.yaml)
environment: development

compiler:
  cpp: 
    standard: C++11
    warnings: -Wall -Wextra -Werror
    debug: -g -O0
    coverage: --coverage
    
build:
  generator: Ninja
  build_type: Debug
  parallel_jobs: 8
  
dependencies:
  qt:
    path: C:/Qt/5.15.2/msvc2019_64
    modules: core, gui, widgets, sql
  gdal:
    path: C:/vcpkg/installed/x64-windows
    version: 3.6.0
  proj:
    path: C:/vcpkg/installed/x64-windows
    version: 9.2.0
    
database:
  sqlite:
    path: ./data/dev_charts.db
    cache_size_mb: 64
    enable_wal: true
  postgresql:
    host: localhost
    port: 5432
    database: chart_dev
    user: chart_dev
    password: dev_password
    
logging:
  level: DEBUG
  console: true
  file: ./logs/dev_chart.log
  max_size_mb: 100
  rotation_count: 5
  
testing:
  test_data_dir: ./test_data
  coverage_report: ./coverage
  benchmark_iterations: 100
```

#### 11.1.4 构建配置

```cmake
# CMakeLists.txt - 开发环境构建配置
cmake_minimum_required(VERSION 3.16)
project(ChartDisplay VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_TESTS "Enable unit tests" ON)
option(ENABLE_COVERAGE "Enable code coverage" ON)
option(ENABLE_BENCHMARK "Enable benchmark tests" ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-Wall -Wextra -g -O0)
    if(ENABLE_COVERAGE)
        add_compile_options(--coverage)
        add_link_options(--coverage)
    endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -DNDEBUG)
endif()

find_package(Qt5 COMPONENTS Core Gui Widgets Sql REQUIRED)
find_package(GDAL REQUIRED)
find_package(PROJ REQUIRED)
find_package(SQLite3 REQUIRED)

add_subdirectory(src)
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
```

### 11.2 测试环境配置

#### 11.2.1 硬件要求

| 配置项 | 最低要求 | 推荐配置 |
|--------|----------|----------|
| CPU | Intel i5 4核 | Intel i7 6核 |
| 内存 | 16GB | 32GB |
| 硬盘 | 512GB SSD | 1TB NVMe SSD |
| 显卡 | 集成显卡 | NVIDIA GTX 1060 |

#### 11.2.2 软件环境

| 软件 | 版本要求 | 用途 |
|------|----------|------|
| 操作系统 | Windows 10/11 或 Ubuntu 20.04+ | 测试平台 |
| Docker | 20.10+ | 容器化测试 |
| Jenkins | 2.400+ | CI/CD |
| PostgreSQL | 13+ | 企业数据库测试 |
| PostGIS | 3.2+ | 空间扩展测试 |

#### 11.2.3 测试环境配置

```yaml
# 测试环境配置示例 (test-config.yaml)
environment: test

compiler:
  cpp:
    standard: C++11
    warnings: -Wall -Wextra
    debug: -g -O1
    
build:
  generator: Ninja
  build_type: RelWithDebInfo
  parallel_jobs: 4
    
database:
  sqlite:
    path: :memory:
    cache_size_mb: 128
  postgresql:
    host: test-db.internal
    port: 5432
    database: chart_test
    user: chart_test
    password: ${POSTGRES_PASSWORD}
    
logging:
  level: INFO
  console: true
  file: ./logs/test_chart.log
  max_size_mb: 200
  rotation_count: 10
    
testing:
  test_data_dir: /data/test_charts
  coverage_threshold: 80
  benchmark_iterations: 1000
  performance_baseline: ./baseline/perf_baseline.json
  
ci:
  jenkins_url: https://jenkins.internal
  job_name: chart-display-test
  notify_on_failure: true
  artifacts_dir: ./artifacts
```

#### 11.2.4 CI/CD配置

```yaml
# Jenkinsfile - CI/CD流水线配置
pipeline {
    agent any
    
    environment {
        QT_PATH = 'C:/Qt/5.15.2/msvc2019_64'
        BUILD_TYPE = 'RelWithDebInfo'
    }
    
    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }
        
        stage('Setup') {
            steps {
                bat "cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
            }
        }
        
        stage('Build') {
            steps {
                bat "cmake --build build --config ${BUILD_TYPE} --parallel 8"
            }
        }
        
        stage('Unit Tests') {
            steps {
                bat "cd build && ctest -C ${BUILD_TYPE} --output-on-failure"
            }
            post {
                always {
                    junit 'build/tests/**/*.xml'
                }
            }
        }
        
        stage('Coverage Report') {
            steps {
                bat "lcov --capture --directory build --output-file coverage.info"
                bat "genhtml coverage.info --output-directory coverage_report"
            }
        }
        
        stage('Performance Tests') {
            steps {
                bat "cd build && ./benchmark_tests --benchmark_format=json --benchmark_out=perf_results.json"
            }
        }
        
        stage('Archive') {
            steps {
                archiveArtifacts artifacts: 'build/bin/**/*', fingerprint: true
                archiveArtifacts artifacts: 'coverage_report/**/*', fingerprint: true
            }
        }
    }
    
    post {
        always {
            cleanWs()
        }
        failure {
            emailext subject: 'Build Failed: ${JOB_NAME} #${BUILD_NUMBER}',
                     body: 'Check console output at ${BUILD_URL}',
                     to: 'team@company.com'
        }
    }
}
```

### 11.3 生产环境配置

#### 11.3.1 硬件要求

**桌面端服务器**：

| 配置项 | 最低要求 | 推荐配置 |
|--------|----------|----------|
| CPU | Intel Xeon 8核 | Intel Xeon 16核 |
| 内存 | 32GB | 64GB |
| 硬盘 | 1TB SSD | 2TB NVMe RAID |
| 网络 | 1Gbps | 10Gbps |

**移动端服务器**：

| 配置项 | 最低要求 | 推荐配置 |
|--------|----------|----------|
| CPU | Intel Xeon 4核 | Intel Xeon 8核 |
| 内存 | 16GB | 32GB |
| 硬盘 | 512GB SSD | 1TB NVMe SSD |

#### 11.3.2 软件环境

| 软件 | 版本要求 | 用途 |
|------|----------|------|
| 操作系统 | Windows Server 2019+ / Ubuntu 22.04 LTS | 生产平台 |
| PostgreSQL | 14+ | 企业数据库 |
| PostGIS | 3.3+ | 空间扩展 |
| Nginx | 1.20+ | 反向代理 |
| Redis | 6.0+ | 缓存服务 |

#### 11.3.3 生产环境配置

```yaml
# 生产环境配置示例 (prod-config.yaml)
environment: production

compiler:
  cpp:
    standard: C++11
    optimizations: -O3 -march=native -DNDEBUG
    
build:
  generator: Ninja
  build_type: Release
  strip_symbols: true
    
database:
  sqlite:
    path: /data/charts/chart_data.db
    cache_size_mb: 512
    enable_wal: true
    journal_mode: WAL
    synchronous: NORMAL
  postgresql:
    host: prod-db.internal
    port: 5432
    database: chart_production
    user: chart_prod
    password: ${POSTGRES_PASSWORD}
    pool_size: 20
    connection_timeout: 30
    
cache:
  redis:
    host: redis.internal
    port: 6379
    database: 0
    max_connections: 50
    default_ttl: 3600
    
logging:
  level: WARNING
  console: false
  file: /var/log/chart/chart.log
  max_size_mb: 500
  rotation_count: 20
  syslog: true
    
security:
  encryption:
    algorithm: AES-256-GCM
    key_rotation_days: 90
  tls:
    cert_path: /etc/ssl/certs/chart.crt
    key_path: /etc/ssl/private/chart.key
    min_version: TLSv1.3
    
monitoring:
  prometheus:
    enabled: true
    port: 9090
    metrics_path: /metrics
  health_check:
    enabled: true
    port: 8080
    path: /health
```

#### 11.3.4 部署配置

```yaml
# docker-compose.yml - 容器化部署配置
version: '3.8'

services:
  chart-api:
    image: chart-display:latest
    build:
      context: .
      dockerfile: Dockerfile
    ports:
      - "8080:8080"
    environment:
      - ENVIRONMENT=production
      - LOG_LEVEL=WARNING
      - DB_HOST=postgres
      - DB_PORT=5432
      - DB_NAME=chart_production
      - REDIS_HOST=redis
    volumes:
      - chart_data:/data/charts
      - chart_logs:/var/log/chart
    depends_on:
      - postgres
      - redis
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:8080/health"]
      interval: 30s
      timeout: 10s
      retries: 3
    deploy:
      resources:
        limits:
          cpus: '4'
          memory: 8G
        reservations:
          cpus: '2'
          memory: 4G
          
  postgres:
    image: postgis/postgis:14-3.3
    environment:
      - POSTGRES_DB=chart_production
      - POSTGRES_USER=chart_prod
      - POSTGRES_PASSWORD=${POSTGRES_PASSWORD}
    volumes:
      - postgres_data:/var/lib/postgresql/data
    healthcheck:
      test: ["CMD-SHELL", "pg_isready -U chart_prod"]
      interval: 10s
      timeout: 5s
      retries: 5
      
  redis:
    image: redis:7-alpine
    volumes:
      - redis_data:/data
    command: redis-server --maxmemory 2gb --maxmemory-policy allkeys-lru
    
  nginx:
    image: nginx:1.24-alpine
    ports:
      - "443:443"
    volumes:
      - ./nginx.conf:/etc/nginx/nginx.conf:ro
      - ./ssl:/etc/nginx/ssl:ro
    depends_on:
      - chart-api

volumes:
  chart_data:
  chart_logs:
  postgres_data:
  redis_data:
```

### 11.4 环境差异对比

| 配置项 | 开发环境 | 测试环境 | 生产环境 |
|--------|----------|----------|----------|
| 构建类型 | Debug | RelWithDebInfo | Release |
| 日志级别 | DEBUG | INFO | WARNING |
| 数据库 | SQLite本地 | PostgreSQL测试库 | PostgreSQL生产库 |
| 缓存 | 无 | 本地缓存 | Redis集群 |
| 性能监控 | 无 | 基准测试 | Prometheus |
| 安全加密 | 无 | 测试证书 | 生产证书 |
| 高可用 | 单机 | 单机 | 集群部署 |

---

## 12. 日志系统与性能评估

### 12.1 日志系统设计

#### 12.1.1 日志框架选型

| 框架 | 语言 | 特点 | 适用场景 |
|------|------|------|----------|
| spdlog | C++ | 高性能、异步、header-only | C++核心模块 |
| Log4j2 | Java | 功能丰富、异步日志 | Java/JNI层 |
| Qt Logging | C++/Qt | Qt集成、分类日志 | Qt界面模块 |

#### 12.1.2 日志级别定义

| 级别 | 说明 | 使用场景 |
|------|------|----------|
| TRACE | 详细跟踪信息 | 调试时开启，生产环境关闭 |
| DEBUG | 调试信息 | 开发测试环境 |
| INFO | 一般信息 | 关键操作记录 |
| WARNING | 警告信息 | 潜在问题提示 |
| ERROR | 错误信息 | 错误但可恢复 |
| CRITICAL | 严重错误 | 系统崩溃级别错误 |

#### 12.1.3 日志配置

```cpp
// LogManager.h - 日志管理器
#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <string>

namespace chart {

enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARNING = 3,
    ERROR = 4,
    CRITICAL = 5
};

struct LogConfig {
    LogLevel level = LogLevel::INFO;
    bool consoleOutput = true;
    std::string filePath = "./logs/chart.log";
    size_t maxFileSize = 100 * 1024 * 1024;  // 100MB
    int rotationCount = 5;
    bool asyncMode = true;
    std::string pattern = "[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] [%s:%#] %v";
};

class LogManager {
public:
    static LogManager& Instance();
    
    void Initialize(const LogConfig& config);
    void Shutdown();
    
    void SetLevel(LogLevel level);
    void SetPattern(const std::string& pattern);
    
    std::shared_ptr<spdlog::logger> GetLogger(const std::string& name);
    
    template<typename... Args>
    void Trace(const char* fmt, Args&&... args) {
        m_logger->trace(fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void Debug(const char* fmt, Args&&... args) {
        m_logger->debug(fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void Info(const char* fmt, Args&&... args) {
        m_logger->info(fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void Warning(const char* fmt, Args&&... args) {
        m_logger->warn(fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void Error(const char* fmt, Args&&... args) {
        m_logger->error(fmt, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    void Critical(const char* fmt, Args&&... args) {
        m_logger->critical(fmt, std::forward<Args>(args)...);
    }
    
private:
    LogManager() = default;
    ~LogManager() = default;
    
    std::shared_ptr<spdlog::logger> m_logger;
    LogConfig m_config;
};

#define LOG_TRACE(...)    chart::LogManager::Instance().Trace(__VA_ARGS__)
#define LOG_DEBUG(...)    chart::LogManager::Instance().Debug(__VA_ARGS__)
#define LOG_INFO(...)     chart::LogManager::Instance().Info(__VA_ARGS__)
#define LOG_WARNING(...)  chart::LogManager::Instance().Warning(__VA_ARGS__)
#define LOG_ERROR(...)    chart::LogManager::Instance().Error(__VA_ARGS__)
#define LOG_CRITICAL(...) chart::LogManager::Instance().Critical(__VA_ARGS__)

} // namespace chart

#endif // LOG_MANAGER_H
```

```cpp
// LogManager.cpp - 日志管理器实现
#include "LogManager.h"
#include <spdlog/async.h>

namespace chart {

LogManager& LogManager::Instance() {
    static LogManager instance;
    return instance;
}

void LogManager::Initialize(const LogConfig& config) {
    m_config = config;
    
    std::vector<spdlog::sink_ptr> sinks;
    
    if (config.consoleOutput) {
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(spdlog::level::trace);
        sinks.push_back(consoleSink);
    }
    
    if (!config.filePath.empty()) {
        auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            config.filePath, config.maxFileSize, config.rotationCount);
        fileSink->set_level(spdlog::level::trace);
        sinks.push_back(fileSink);
    }
    
    if (config.asyncMode) {
        spdlog::init_thread_pool(8192, 1);
        m_logger = std::make_shared<spdlog::async_logger>(
            "chart", sinks.begin(), sinks.end(),
            spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    } else {
        m_logger = std::make_shared<spdlog::logger>("chart", sinks.begin(), sinks.end());
    }
    
    m_logger->set_pattern(config.pattern);
    SetLevel(config.level);
    
    spdlog::register_logger(m_logger);
    spdlog::set_default_logger(m_logger);
}

void LogManager::SetLevel(LogLevel level) {
    m_config.level = level;
    m_logger->set_level(static_cast<spdlog::level::level_enum>(level));
}

void LogManager::Shutdown() {
    spdlog::shutdown();
}

} // namespace chart
```

### 12.2 各阶段性能评估指标

#### 12.2.1 数据解析阶段

| 指标 | 说明 | 目标值 | 监控方法 |
|------|------|--------|----------|
| parse_time_ms | 单文件解析耗时 | <500ms | 日志记录 |
| feature_count | 解析要素数量 | - | 日志记录 |
| parse_throughput | 解析吞吐量(要素/秒) | >2000 | 计算 |
| memory_peak_mb | 解析内存峰值 | <200MB | 内存监控 |
| error_count | 解析错误数 | 0 | 错误日志 |

```cpp
// 性能监控宏定义
#define PERF_START(tag) \
    auto _perf_start_##tag = std::chrono::high_resolution_clock::now()

#define PERF_END(tag, operation) \
    do { \
        auto _perf_end = std::chrono::high_resolution_clock::now(); \
        double _perf_ms = std::chrono::duration<double, std::milli>( \
            _perf_end - _perf_start_##tag).count(); \
        LOG_INFO("[PERF] {} 耗时: {:.2f}ms", operation, _perf_ms); \
    } while(0)

#define PERF_RECORD(stage, metric, value) \
    LOG_INFO("[PERF] [{}] {} = {}", stage, metric, value)
```

#### 12.2.2 数据存储阶段

| 指标 | 说明 | 目标值 | 监控方法 |
|------|------|--------|----------|
| store_time_ms | 存储耗时 | <100ms/1000条 | 日志记录 |
| query_time_ms | 查询耗时 | <100ms | 日志记录 |
| index_build_time_ms | 索引构建耗时 | <1000ms | 日志记录 |
| db_size_mb | 数据库大小 | - | 定期统计 |
| cache_hit_rate | 缓存命中率 | >80% | 计算 |

#### 12.2.3 渲染显示阶段

| 指标 | 说明 | 目标值 | 监控方法 |
|------|------|--------|----------|
| render_time_ms | 首屏渲染耗时 | <300ms | 日志记录 |
| frame_time_ms | 帧渲染耗时 | <33ms | 日志记录 |
| fps | 帧率 | ≥30fps | 计算 |
| visible_features | 可见要素数 | - | 日志记录 |
| lod_skip_count | LOD跳过数 | - | 日志记录 |

#### 12.2.4 交互响应阶段

| 指标 | 说明 | 目标值 | 监控方法 |
|------|------|--------|----------|
| pan_response_ms | 平移响应时间 | <50ms | 日志记录 |
| zoom_response_ms | 缩放响应时间 | <100ms | 日志记录 |
| select_response_ms | 选择响应时间 | <50ms | 日志记录 |
| gesture_latency_ms | 手势延迟 | <16ms | 日志记录 |

### 12.3 性能监控实现

```cpp
// PerformanceMonitor.h - 性能监控器
#ifndef PERFORMANCE_MONITOR_H
#define PERFORMANCE_MONITOR_H

#include <chrono>
#include <map>
#include <vector>
#include <mutex>
#include <fstream>

namespace chart {

struct PerformanceMetric {
    std::string name;
    double value;
    std::string unit;
    std::string timestamp;
};

struct PerformanceStats {
    double min;
    double max;
    double avg;
    double p50;
    double p95;
    double p99;
    size_t count;
};

class PerformanceMonitor {
public:
    static PerformanceMonitor& Instance();
    
    void StartTimer(const std::string& name);
    double EndTimer(const std::string& name);
    
    void RecordMetric(const std::string& stage, const std::string& name, 
                      double value, const std::string& unit = "ms");
    
    PerformanceStats GetStats(const std::string& stage, const std::string& name);
    
    void ExportReport(const std::string& filePath);
    void Reset();
    
private:
    PerformanceMonitor() = default;
    
    std::map<std::string, std::chrono::high_resolution_clock::time_point> m_timers;
    std::map<std::string, std::vector<PerformanceMetric>> m_metrics;
    std::mutex m_mutex;
    
    double CalculatePercentile(std::vector<double> values, double percentile);
};

#define PERF_TIMER_START(name) \
    chart::PerformanceMonitor::Instance().StartTimer(name)

#define PERF_TIMER_END(name) \
    chart::PerformanceMonitor::Instance().EndTimer(name)

#define PERF_RECORD_METRIC(stage, name, value, unit) \
    chart::PerformanceMonitor::Instance().RecordMetric(stage, name, value, unit)

} // namespace chart

#endif // PERFORMANCE_MONITOR_H
```

```cpp
// PerformanceMonitor.cpp - 性能监控器实现
#include "PerformanceMonitor.h"
#include "LogManager.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace chart {

PerformanceMonitor& PerformanceMonitor::Instance() {
    static PerformanceMonitor instance;
    return instance;
}

void PerformanceMonitor::StartTimer(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_timers[name] = std::chrono::high_resolution_clock::now();
}

double PerformanceMonitor::EndTimer(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_timers.find(name);
    if (it == m_timers.end()) {
        LOG_WARNING("Timer not found: {}", name);
        return 0.0;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(end - it->second).count();
    
    m_timers.erase(it);
    
    LOG_DEBUG("[PERF] {} = {:.2f}ms", name, elapsed);
    return elapsed;
}

void PerformanceMonitor::RecordMetric(const std::string& stage, 
                                       const std::string& name,
                                       double value, 
                                       const std::string& unit) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    std::string key = stage + "." + name;
    m_metrics[key].push_back({name, value, unit, ss.str()});
    
    LOG_INFO("[PERF] [{}] {} = {:.2f}{}", stage, name, value, unit);
}

PerformanceStats PerformanceMonitor::GetStats(const std::string& stage, 
                                               const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::string key = stage + "." + name;
    auto it = m_metrics.find(key);
    if (it == m_metrics.end() || it->second.empty()) {
        return {0, 0, 0, 0, 0, 0, 0};
    }
    
    std::vector<double> values;
    for (const auto& m : it->second) {
        values.push_back(m.value);
    }
    
    std::sort(values.begin(), values.end());
    
    PerformanceStats stats;
    stats.min = values.front();
    stats.max = values.back();
    stats.count = values.size();
    
    double sum = 0;
    for (double v : values) sum += v;
    stats.avg = sum / stats.count;
    
    stats.p50 = CalculatePercentile(values, 0.50);
    stats.p95 = CalculatePercentile(values, 0.95);
    stats.p99 = CalculatePercentile(values, 0.99);
    
    return stats;
}

double PerformanceMonitor::CalculatePercentile(std::vector<double> values, 
                                                double percentile) {
    if (values.empty()) return 0;
    
    size_t index = static_cast<size_t>(values.size() * percentile);
    if (index >= values.size()) index = values.size() - 1;
    
    return values[index];
}

void PerformanceMonitor::ExportReport(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::ofstream file(filePath);
    file << "Stage,Metric,Min,Max,Avg,P50,P95,P99,Count,Unit\n";
    
    for (const auto& [key, metrics] : m_metrics) {
        if (metrics.empty()) continue;
        
        auto stats = GetStats(
            key.substr(0, key.find('.')), 
            key.substr(key.find('.') + 1)
        );
        
        file << key.substr(0, key.find('.')) << ","
             << key.substr(key.find('.') + 1) << ","
             << std::fixed << std::setprecision(2)
             << stats.min << ","
             << stats.max << ","
             << stats.avg << ","
             << stats.p50 << ","
             << stats.p95 << ","
             << stats.p99 << ","
             << stats.count << ","
             << metrics[0].unit << "\n";
    }
    
    file.close();
    LOG_INFO("性能报告已导出: {}", filePath);
}

void PerformanceMonitor::Reset() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_timers.clear();
    m_metrics.clear();
}

} // namespace chart
```

### 12.4 性能评估报告模板

```markdown
## 性能评估报告

**评估日期**: YYYY-MM-DD
**评估环境**: [开发/测试/生产]
**版本号**: vX.X.X

### 1. 数据解析性能

| 指标 | 目标值 | 实际值 | 状态 |
|------|--------|--------|------|
| S57解析耗时 | <500ms | XXX ms | ✅/❌ |
| 解析吞吐量 | >2000要素/秒 | XXX 要素/秒 | ✅/❌ |
| 内存峰值 | <200MB | XXX MB | ✅/❌ |

### 2. 数据存储性能

| 指标 | 目标值 | 实际值 | 状态 |
|------|--------|--------|------|
| 存储耗时 | <100ms/1000条 | XXX ms | ✅/❌ |
| 查询耗时 | <100ms | XXX ms | ✅/❌ |
| 缓存命中率 | >80% | XX% | ✅/❌ |

### 3. 渲染显示性能

| 指标 | 目标值 | 实际值 | 状态 |
|------|--------|--------|------|
| 首屏渲染 | <300ms | XXX ms | ✅/❌ |
| 帧率 | ≥30fps | XX fps | ✅/❌ |
| 帧渲染耗时 | <33ms | XXX ms | ✅/❌ |

### 4. 交互响应性能

| 指标 | 目标值 | 实际值 | 状态 |
|------|--------|--------|------|
| 平移响应 | <50ms | XXX ms | ✅/❌ |
| 缩放响应 | <100ms | XXX ms | ✅/❌ |
| 选择响应 | <50ms | XXX ms | ✅/❌ |

### 5. 资源占用

| 指标 | 目标值 | 实际值 | 状态 |
|------|--------|--------|------|
| 内存占用(桌面) | <1GB | XXX MB | ✅/❌ |
| 内存占用(移动) | <500MB | XXX MB | ✅/❌ |
| 启动时间 | <3s | XXX s | ✅/❌ |

### 6. 问题与建议

[列出发现的问题和改进建议]
```

---

**文档结束**
