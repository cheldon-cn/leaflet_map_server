# 图层模型设计文档

**版本**: 1.5  
**日期**: 2026年3月18日  
**状态**: 设计中  
**C++标准**: C++11  
**设计目标**: 稳定、正确、高效、易扩展、跨平台、多线程安全

---

## 文档修订历史

| 版本 | 日期 | 修改内容 | 评审意见 |
|------|------|----------|----------|
| 1.0 | 2026-03-17 | 初始版本创建 | - |
| 1.1 | 2026-03-17 | 根据技术评审意见修订：增加数据源抽象层、驱动注册机制、优化事务实现、GDAL互操作层、里程碑规划 | 架构师评审 |
| 1.2 | 2026-03-17 | 根据实战评审意见修订：统一栅格模型架构、明确依赖库、新增连接池/进度回调/异步接口、修正工作量评估 | 实战评审 |
| 1.3 | 2026-03-17 | 根据核心架构师评审意见修订：拆分CNLayer接口、新增事件通知机制、引用迭代接口、流式读取接口、WFS客户端设计、图层组设计、调整工作量评估 | 核心架构师评审 |
| 1.4 | 2026-03-18 | 修正版本一致性，明确v1.4改进项为待实现状态 | 核心架构师评审 |
| 1.5 | 2026-03-18 | 根据核心架构师评审意见修订：新增WFS错误处理和重试机制、图层组事务传播、流式读取背压机制、事件通知异步机制、WFS响应解析安全校验、更新工作量评估 | 核心架构师评审 |

---

## 目录

1. [概述](#1-概述)
   - 1.1 [文档目的](#11-文档目的)
   - 1.2 [范围](#12-范围)
   - 1.3 [参考文档](#13-参考文档)
   - 1.4 [架构层次](#14-架构层次)
   - 1.5 [类继承关系](#15-类继承关系)
2. [设计目标](#2-设计目标)
   - 2.1 [核心目标](#21-核心目标)
   - 2.2 [设计原则](#22-设计原则)
3. [核心概念](#3-核心概念)
   - 3.1 [OGC Simple Feature Access 标准](#31-ogc-simple-feature-access-标准)
   - 3.2 [图层能力模型](#32-图层能力模型)
   - 3.3 [与几何模型和要素模型的关系](#33-与几何模型和要素模型的关系)
4. [公共定义](#4-公共定义)
   - 4.1 [命名空间](#41-命名空间)
   - 4.2 [图层类型枚举](#42-图层类型枚举)
   - 4.3 [图层能力枚举](#43-图层能力枚举)
   - 4.4 [状态码定义](#44-状态码定义)
   - 4.5 [空间过滤器类型](#45-空间过滤器类型)
   - 4.6 [空间关系谓词](#46-空间关系谓词)
5. [CNLayer 抽象基类设计](#5-cnlayer-抽象基类设计)
   - 5.1 [类定义](#51-类定义)
   - 5.2 [核心接口详解](#52-核心接口详解)
   - 5.3 [能力测试实现](#53-能力测试实现)
6. [CNMemoryLayer 内存图层实现](#6-cnmemorylayer-内存图层实现)
   - 6.1 [类定义](#61-类定义)
   - 6.2 [数据结构设计](#62-数据结构设计)
   - 6.3 [事务实现](#63-事务实现)
   - 6.4 [空间索引集成](#64-空间索引集成)
7. [CNVectorLayer 矢量图层实现](#7-cnvectorlayer-矢量图层实现)
   - 7.1 [类定义](#71-类定义)
   - 7.2 [格式驱动架构](#72-格式驱动架构)
   - 7.3 [具体格式实现](#73-具体格式实现)
8. [CNRasterLayer 栅格图层实现](#8-cnrasterlayer-栅格图层实现)
   - 8.1 [类定义](#81-类定义)
   - 8.2 [波段管理](#82-波段管理)
   - 8.3 [金字塔机制](#83-金字塔机制)
9. [线程安全设计](#9-线程安全设计)
   - 9.1 [线程安全策略](#91-线程安全策略)
   - 9.2 [读写锁实现](#92-读写锁实现)
   - 9.3 [线程安全包装器](#93-线程安全包装器)
   - 9.4 [图层快照](#94-图层快照)
10. [性能优化策略](#10-性能优化策略)
    - 10.1 [空间索引优化](#101-空间索引优化)
    - 10.2 [缓存机制](#102-缓存机制)
    - 10.3 [批量操作优化](#103-批量操作优化)
11. [使用示例](#11-使用示例)
    - 11.1 [基本使用](#111-基本使用)
    - 11.2 [空间查询](#112-空间查询)
    - 11.3 [事务操作](#113-事务操作)
    - 11.4 [读取矢量文件](#114-读取矢量文件)
    - 11.5 [连接 PostGIS](#115-连接-postgis)
    - 11.6 [多线程处理](#116-多线程处理)
    - 11.7 [使用图层快照](#117-使用图层快照)
12. [编译和依赖](#12-编译和依赖)
    - 12.1 [编译要求](#121-编译要求)
    - 12.2 [依赖库](#122-依赖库)
    - 12.3 [CMake 配置](#123-cmake-配置)
13. [测试策略](#13-测试策略)
    - 13.1 [单元测试](#131-单元测试)
    - 13.2 [性能测试](#132-性能测试)
    - 13.3 [集成测试](#133-集成测试)
    - 13.4 [测试覆盖率要求](#134-测试覆盖率要求)
14. [附录](#14-附录)
    - 14.1 [OGC 标准参考](#141-ogc-标准参考)
    - 14.2 [GDAL 参考文档](#142-gdal-参考文档)
    - 14.3 [C++ 最佳实践参考](#143-c-最佳实践参考)
    - 14.4 [术语表](#144-术语表)
    - 14.5 [版本历史](#145-版本历史)
15. [总结](#15-总结)
    - 15.1 [设计亮点](#151-设计亮点)
    - 15.2 [实现优先级](#152-实现优先级)
    - 15.3 [后续工作](#153-后续工作)
16. [数据源抽象层设计](#16-数据源抽象层设计) *(新增)*
    - 16.1 [CNDataSource 数据源接口](#161-cndatasource-数据源接口)
    - 16.2 [CNDriver 驱动抽象](#162-cndriver-驱动抽象)
    - 16.3 [CNDriverManager 驱动注册器](#163-cndrivermanager-驱动注册器)
17. [栅格数据模型设计](#17-栅格数据模型设计) *(新增)*
    - 17.1 [CNRasterDataset 栅格数据集接口](#171-cnrasterdataset-栅格数据集接口)
    - 17.2 [与矢量模型的分离设计](#172-与矢量模型的分离设计)
18. [GDAL 互操作层设计](#18-gdal-互操作层设计) *(新增)*
    - 18.1 [CNGDALAdapter 适配器](#181-cngdaladapter-适配器)
    - 18.2 [数据转换接口](#182-数据转换接口)
19. [性能优化增强设计](#19-性能优化增强设计) *(新增)*
    - 19.1 [增量事务快照](#191-增量事务快照)
    - 19.2 [分段锁缓存](#192-分段锁缓存)
    - 19.3 [对象池设计](#193-对象池设计)
    - 19.4 [增量空间索引更新](#194-增量空间索引更新)
20. [错误处理策略](#20-错误处理策略) *(新增)*
    - 20.1 [异常安全保证](#201-异常安全保证)
    - 20.2 [跨模块错误处理](#202-跨模块错误处理)
21. [项目里程碑规划](#21-项目里程碑规划) *(新增)*
    - 21.1 [开发阶段划分](#211-开发阶段划分)
    - 21.2 [工作量评估](#212-工作量评估)
22. [连接池设计](#22-连接池设计) *(新增)*
    - 22.1 [CNConnectionPool 接口](#221-cnconnectionpool-接口)
23. [进度回调与异步接口](#23-进度回调与异步接口) *(新增)*
    - 23.1 [CNProgressCallback 进度回调](#231-cnprogresscallback-进度回调)
    - 23.2 [异步操作接口](#232-异步操作接口)
24. [属性过滤器规范](#24-属性过滤器规范) *(新增)*
    - 24.1 [过滤器语法规范](#241-过滤器语法规范)
    - 24.2 [内存图层表达式引擎](#242-内存图层表达式引擎)
    - 24.3 [各驱动支持情况](#243-各驱动支持情况)
25. [内存压力测试](#25-内存压力测试) *(新增)*
    - 25.1 [内存估算公式](#251-内存估算公式)
    - 25.2 [内存压力测试场景](#252-内存压力测试场景)
26. [错误处理策略统一](#26-错误处理策略统一) *(新增)*
    - 26.1 [错误处理规范](#261-错误处理规范)
    - 26.2 [统一错误处理示例](#262-统一错误处理示例)
27. [CNLayer接口拆分设计](#27-cnlayer接口拆分设计) *(新增)*
    - 27.1 [接口隔离原则应用](#271-接口隔离原则应用)
    - 27.2 [角色接口定义](#272-角色接口定义)
28. [图层事件通知机制](#28-图层事件通知机制) *(新增)*
    - 28.1 [CNLayerObserver观察者接口](#281-cnlayerobserver观察者接口)
    - 28.2 [事件类型定义](#282-事件类型定义)
29. [引用迭代与流式读取](#29-引用迭代与流式读取) *(新增)*
    - 29.1 [GetNextFeatureRef引用迭代](#291-getnextfeatureref引用迭代)
    - 29.2 [CNFeatureStream流式接口](#292-cnfeaturestream流式接口)
30. [WFS客户端设计](#30-wfs客户端设计) *(新增)*
    - 30.1 [CNWFSLayer实现](#301-cnwfsLayer实现)
    - 30.2 [WFS请求构建](#302-wfs请求构建)
31. [CNLayerGroup图层组设计](#31-cnlayergroup图层组设计) *(新增)*
    - 31.1 [图层组接口定义](#311-图层组接口定义)
    - 31.2 [树形结构管理](#312-树形结构管理)
32. [空间索引并发保护](#32-空间索引并发保护) *(新增)*
    - 32.1 [并发读写策略](#321-并发读写策略)
    - 32.2 [读写锁集成](#322-读写锁集成)

---

## 1. 概述

### 1.1 文档目的

本文档详细描述了 CNLayer 及其派生类的设计方案，这些类共同构成了一套完整的、现代、类型安全、线程友好的 OGC 数据访问体系，遵循 OGC Simple Feature Access 标准（ISO 19125）。

### 1.2 范围

本设计涵盖以下核心组件：

- **CNLayer**: 图层抽象基类
- **CNMemoryLayer**: 内存图层实现
- **CNVectorLayer**: 矢量图层实现
- **CNRasterLayer**: 栅格图层实现
- **CNLayerGroup**: 图层组实现
- **CNLayerIterator**: 图层迭代器
- **CNSpatialIndex**: 空间索引接口

### 1.3 参考文档

| 文档 | 说明 |
|------|------|
| `layer_model_research_report_trae.md` | 图层模型设计研究报告 |
| `geometry_design_glm.md` | OGC 几何类库设计文档 v2.0 |
| `feature_model_design.md` | 要素模型设计文档 |
| `layer.txt` | 图层类定义源文件 |

### 1.4 架构层次

```
┌─────────────────────────────────────────────────────────┐
│                    应用层 (Application)                   │
│          使用 CNLayer 进行业务逻辑处理                     │
└───────────────────────┬─────────────────────────────────┘
                        │
┌───────────────────────▼─────────────────────────────────┐
│                    图层模型层 (Layer Model)               │
│   CNLayer, CNMemoryLayer, CNVectorLayer, CNRasterLayer  │
└───────────────────────┬─────────────────────────────────┘
                        │
        ┌───────────────┼───────────────┐
        │               │               │
┌───────▼────────┐ ┌────▼────┐ ┌────────▼────────┐
│   几何模型层    │ │要素模型层│ │   数据存储层     │
│ CNGeometry 等   │ │CNFeature │ │ DbConnection 等  │
└────────────────┘ └─────────┘ └─────────────────┘
```

### 1.5 类继承关系

```
CNLayer (抽象基类 - 矢量数据模型)
    ├── CNMemoryLayer (内存图层)
    ├── CNVectorLayer (矢量图层)
    │   ├── CNShapefileLayer
    │   ├── CNGeoJSONLayer
    │   ├── CNGeoPackageLayer
    │   ├── CNPostGISLayer
    │   └── CNWFSLayer
    └── CNLayerGroup (图层组)

CNRasterDataset (栅格数据模型 - 独立接口)
    ├── CNGeoTIFFDataset
    ├── CNNetCDFDataset
    └── CNWMTSDataset

CNRasterLayer (栅格图层包装器 - 不继承CNLayer)
    └── 包装 CNRasterDataset 提供统一访问接口
```

**设计说明**:
- **矢量数据模型**: CNLayer及其派生类处理要素(Feature)数据
- **栅格数据模型**: CNRasterDataset独立接口处理像素网格数据
- **CNRasterLayer**: 作为轻量包装器，不继承CNLayer，避免违反Liskov替换原则

---

## 2. 设计目标

### 2.1 核心目标

| 目标 | 说明 | 实现策略 |
|------|------|----------|
| **稳定性** | 健壮的错误处理、内存安全、异常安全 | Result模式、RAII、智能指针 |
| **正确性** | 符合OGC标准、完整测试覆盖 | OGC CITE测试、交叉验证 |
| **高效性** | 优化的性能、智能缓存、批量操作 | 延迟计算、空间索引、SIMD优化 |
| **扩展性** | 易于添加新类型和操作 | 工厂模式、策略模式、插件架构 |
| **跨平台** | 支持Windows/Linux/macOS | 纯C++11标准、无平台依赖 |
| **线程安全** | 多线程环境下安全使用 | 读写锁、原子操作、线程局部存储 |

### 2.2 设计原则

#### 2.2.1 Google C++ 风格规范

遵循 Google C++ Style Guide 的关键规范：

**命名规范**:
- **类型名称**: 大驼峰命名（如 `CNLayer`, `CNMemoryLayer`）
- **函数名称**: 大驼峰命名（如 `GetFeatureCount`, `SetSpatialFilter`）
- **变量名称**: 小写下划线命名（如 `feature_count_`, `spatial_filter_`）
- **常量名称**: k前缀大驼峰（如 `kMaxFeatureCount`）
- **枚举值**: k前缀大驼峰（如 `kRandomRead`, `kFastSpatialFilter`）

**所有权管理**:
- 优先使用 `std::unique_ptr` 表示唯一所有权
- 仅在必要时使用 `std::shared_ptr` 共享所有权
- 明确所有权转移使用移动语义

#### 2.2.2 现代 C++11 特性

- **智能指针**: `std::unique_ptr`, `std::shared_ptr`, `std::weak_ptr`
- **移动语义**: 移动构造、移动赋值、`std::move`
- **RAII**: 资源获取即初始化
- **原子操作**: `std::atomic`, `std::mutex`, `std::condition_variable`
- **强类型枚举**: `enum class`

---

## 3. 核心概念

### 3.1 OGC Simple Feature Access 标准

根据 OGC Simple Feature Access - Part 1: Common Architecture（ISO 19125），图层定义为：

> **图层（Layer）** 是具有相同要素定义（Feature Definition）的要素集合，共享相同的几何类型和属性模式。

**核心组成部分**:

1. **要素定义（Feature Definition）**: 描述要素的结构（字段定义）
2. **空间参考系统（Spatial Reference System）**: 坐标系统定义
3. **几何类型（Geometry Type）**: 点、线、面等
4. **范围（Extent）**: 图层的空间范围

### 3.2 图层能力模型

```
┌─────────────────────────────────────────────────────────┐
│                    图层能力 (Capabilities)               │
├─────────────────────────────────────────────────────────┤
│  读取能力                                                 │
│  ├── RandomRead         随机读取要素                      │
│  ├── FastSpatialFilter  快速空间过滤                      │
│  ├── FastFeatureCount   快速要素计数                      │
│  └── FastGetExtent      快速获取范围                      │
├─────────────────────────────────────────────────────────┤
│  写入能力                                                 │
│  ├── SequentialWrite    顺序写入                          │
│  ├── RandomWrite        随机写入                          │
│  └── CreateFeature      创建要素                          │
├─────────────────────────────────────────────────────────┤
│  结构修改能力                                             │
│  ├── CreateField        创建字段                          │
│  ├── DeleteField        删除字段                          │
│  └── AlterFieldDefn     修改字段定义                      │
├─────────────────────────────────────────────────────────┤
│  事务能力                                                 │
│  ├── Transactions       事务支持                          │
│  └── DeleteFeature      删除要素                          │
└─────────────────────────────────────────────────────────┘
```

### 3.3 与几何模型和要素模型的关系

```
CNLayer (图层)
    │
    ├── CNFeatureDefn (要素定义 - 共享)
    │   ├── CNFieldDefn[] (字段定义列表)
    │   └── CNGeomFieldDefn (几何字段定义)
    │
    ├── CNFeature[] (要素集合)
    │   ├── FID (要素标识)
    │   ├── CNGeometry (几何对象)
    │   └── CNFieldValue[] (属性字段值)
    │
    └── CNSpatialReference (空间参考)
```

---

## 4. 公共定义

### 4.1 命名空间

```cpp
namespace OGC {
    // 所有图层类和工具在此命名空间
}
```

### 4.2 图层类型枚举

```cpp
/**
 * @brief 图层类型枚举
 */
enum class CNLayerType {
    kUnknown = 0,
    kMemory,        // 内存图层
    kVector,        // 矢量图层
    kRaster,        // 栅格图层
    kGroup,         // 图层组
    kWFS,           // WFS服务图层
    kWMS,           // WMS服务图层
    kWMTS           // WMTS服务图层
};
```

### 4.3 图层能力枚举

```cpp
/**
 * @brief 图层能力枚举
 * 
 * 参考 GDAL OGRLayer 能力测试机制
 */
enum class CNLayerCapability {
    // 读取能力
    kRandomRead,            // 随机读取: GetFeature(fid)
    kFastSpatialFilter,     // 快速空间过滤
    kFastFeatureCount,      // 快速要素计数
    kFastGetExtent,         // 快速获取范围
    
    // 写入能力
    kSequentialWrite,       // 顺序写入
    kRandomWrite,           // 随机写入
    kCreateFeature,         // 创建要素
    kDeleteFeature,         // 删除要素
    
    // 结构修改能力
    kCreateField,           // 创建字段
    kDeleteField,           // 删除字段
    kReorderFields,         // 重排字段
    kAlterFieldDefn,        // 修改字段定义
    
    // 事务能力
    kTransactions,          // 事务支持
    
    // 其他能力
    kStringsAsUTF8,         // UTF-8 字符串
    kIgnoreFields,          // 忽略字段能力
    kCurveGeometries        // 支持曲线几何
};
```

### 4.4 状态码定义

```cpp
/**
 * @brief 图层操作状态码
 */
enum class CNStatus {
    kSuccess = 0,
    
    // 一般错误 (1-99)
    kError = 1,
    kInvalidParameter = 2,
    kNullPointer = 3,
    kOutOfRange = 4,
    kNotSupported = 5,
    kNotImplemented = 6,
    
    // 数据错误 (100-199)
    kInvalidFeature = 100,
    kInvalidGeometry = 101,
    kInvalidFID = 102,
    kFeatureNotFound = 103,
    kDuplicateFID = 104,
    kFieldNotFound = 105,
    kTypeMismatch = 106,
    
    // I/O错误 (200-299)
    kIOError = 200,
    kFileNotFound = 201,
    kFileExists = 202,
    kPermissionDenied = 203,
    kDiskFull = 204,
    kCorruptData = 205,
    
    // 事务错误 (300-399)
    kNoTransaction = 300,
    kTransactionActive = 301,
    kCommitFailed = 302,
    kRollbackFailed = 303,
    
    // 线程错误 (400-499)
    kLockFailed = 400,
    kTimeout = 401,
    kDeadlock = 402,
    
    // 内存错误 (500-599)
    kOutOfMemory = 500,
    kBufferOverflow = 501
};

// 状态信息获取
const char* GetStatusString(CNStatus status);
std::string GetStatusDescription(CNStatus status);
bool IsSuccess(CNStatus status);
```

### 4.5 空间过滤器类型

```cpp
/**
 * @brief 空间过滤器类型
 */
enum class CNSpatialFilterType {
    kNone,          // 无过滤
    kRectangular,   // 矩形过滤 (MBR)
    kGeometric      // 几何过滤
};
```

### 4.6 空间关系谓词

```cpp
/**
 * @brief 空间关系谓词 (OGC DE-9IM)
 */
enum class CNSpatialRelation {
    kEquals,        // 相等
    kDisjoint,      // 相离
    kIntersects,    // 相交
    kTouches,       // 接触
    kCrosses,       // 穿越
    kWithin,        // 在内部
    kContains,      // 包含
    kOverlaps,      // 重叠
    kCovers,        // 覆盖
    kCoveredBy      // 被覆盖
};
```

---

## 5. CNLayer 抽象基类设计

### 5.1 类定义

```cpp
namespace OGC {

// 前向声明
class CNFeature;
class CNFeatureDefn;
class CNGeometry;
class CNSpatialReference;
class CNEnvelope;
class CNFieldDefn;

/**
 * @brief 图层抽象基类
 * 
 * 遵循 OGC Simple Feature Access 标准
 * 参考 GDAL OGRLayer 设计
 * 
 * 线程安全说明:
 * - 只读操作是线程安全的
 * - 写操作需要外部同步或使用事务
 * - 迭代器不是线程安全的
 */
class CNLayer {
public:
    // ========== 构造与析构 ==========
    
    CNLayer() = default;
    
    virtual ~CNLayer() = default;
    
    // 禁止拷贝
    CNLayer(const CNLayer&) = delete;
    CNLayer& operator=(const CNLayer&) = delete;
    
    // 允许移动
    CNLayer(CNLayer&&) = default;
    CNLayer& operator=(CNLayer&&) = default;
    
    // ========== 基本信息 ==========
    
    /**
     * @brief 获取图层名称
     * @return 图层名称
     */
    virtual const std::string& GetName() const = 0;
    
    /**
     * @brief 获取图层类型
     * @return 图层类型枚举值
     */
    virtual CNLayerType GetLayerType() const = 0;
    
    /**
     * @brief 获取要素定义
     * @return 要素定义指针，调用者不拥有所有权
     */
    virtual CNFeatureDefn* GetFeatureDefn() = 0;
    
    virtual const CNFeatureDefn* GetFeatureDefn() const = 0;
    
    /**
     * @brief 获取几何类型
     * @return 几何类型枚举值
     */
    virtual GeomType GetGeomType() const = 0;
    
    /**
     * @brief 获取空间参考系统
     * @return 空间参考指针，可能为 nullptr
     */
    virtual CNSpatialReference* GetSpatialRef() = 0;
    
    virtual const CNSpatialReference* GetSpatialRef() const = 0;
    
    /**
     * @brief 获取图层范围
     * @param extent 输出范围
     * @param force 是否强制计算（遍历所有要素）
     * @return 状态码
     */
    virtual CNStatus GetExtent(CNEnvelope& extent, bool force = true) const = 0;
    
    // ========== 要素计数 ==========
    
    /**
     * @brief 获取要素数量
     * @param force 是否强制计算
     * @return 要素数量，-1 表示无法获取
     */
    virtual int64_t GetFeatureCount(bool force = true) const = 0;
    
    // ========== 要素遍历 ==========
    
    /**
     * @brief 重置读取位置
     * 
     * 将内部游标重置到起始位置，
     * 后续 GetNextFeature() 调用将从第一个要素开始
     */
    virtual void ResetReading() = 0;
    
    /**
     * @brief 获取下一个要素
     * @return 要素指针，调用者拥有所有权；返回 nullptr 表示结束
     */
    virtual std::unique_ptr<CNFeature> GetNextFeature() = 0;
    
    /**
     * @brief 获取下一个要素（不转移所有权）
     * @return 要素指针，调用者不拥有所有权；返回 nullptr 表示结束
     * @note 仅适用于支持引用迭代的图层
     */
    virtual CNFeature* GetNextFeatureRef() {
        return nullptr;
    }
    
    // ========== 随机访问 ==========
    
    /**
     * @brief 根据 FID 获取要素
     * @param fid 要素标识符
     * @return 要素指针，调用者拥有所有权；返回 nullptr 表示未找到
     */
    virtual std::unique_ptr<CNFeature> GetFeature(int64_t fid) = 0;
    
    /**
     * @brief 设置当前要素（用于后续修改）
     * @param fid 要素标识符
     * @return 状态码
     */
    virtual CNStatus SetFeature(const CNFeature* feature) {
        return CNStatus::kNotSupported;
    }
    
    // ========== 写入操作 ==========
    
    /**
     * @brief 创建新要素
     * @param feature 要素指针，FID 将被自动分配
     * @return 状态码
     */
    virtual CNStatus CreateFeature(CNFeature* feature) {
        return CNStatus::kNotSupported;
    }
    
    /**
     * @brief 删除要素
     * @param fid 要素标识符
     * @return 状态码
     */
    virtual CNStatus DeleteFeature(int64_t fid) {
        return CNStatus::kNotSupported;
    }
    
    /**
     * @brief 批量创建要素
     * @param features 要素数组
     * @return 成功创建的要素数量
     */
    virtual int64_t CreateFeatureBatch(
        const std::vector<CNFeature*>& features) {
        int64_t count = 0;
        for (auto* f : features) {
            if (CreateFeature(f) == CNStatus::kSuccess) {
                count++;
            }
        }
        return count;
    }
    
    // ========== 字段操作 ==========
    
    /**
     * @brief 创建新字段
     * @param field_defn 字段定义
     * @param approx_ok 是否允许近似类型
     * @return 状态码
     */
    virtual CNStatus CreateField(
        const CNFieldDefn* field_defn, 
        bool approx_ok = false) {
        return CNStatus::kNotSupported;
    }
    
    /**
     * @brief 删除字段
     * @param field_index 字段索引
     * @return 状态码
     */
    virtual CNStatus DeleteField(int field_index) {
        return CNStatus::kNotSupported;
    }
    
    /**
     * @brief 重排字段
     * @param new_order 新顺序（字段索引数组）
     * @return 状态码
     */
    virtual CNStatus ReorderFields(const std::vector<int>& new_order) {
        return CNStatus::kNotSupported;
    }
    
    /**
     * @brief 修改字段定义
     * @param field_index 字段索引
     * @param new_defn 新字段定义
     * @param flags 修改标志
     * @return 状态码
     */
    virtual CNStatus AlterFieldDefn(
        int field_index,
        const CNFieldDefn* new_defn,
        int flags) {
        return CNStatus::kNotSupported;
    }
    
    // ========== 过滤器 ==========
    
    /**
     * @brief 设置空间过滤器（矩形）
     * @param min_x 最小 X
     * @param min_y 最小 Y
     * @param max_x 最大 X
     * @param max_y 最大 Y
     */
    virtual void SetSpatialFilterRect(
        double min_x, double min_y,
        double max_x, double max_y) {
        SetSpatialFilter(nullptr);
    }
    
    /**
     * @brief 设置空间过滤器（几何）
     * @param geometry 过滤几何，nullptr 清除过滤器
     */
    virtual void SetSpatialFilter(const CNGeometry* geometry) = 0;
    
    /**
     * @brief 获取空间过滤器
     * @return 过滤几何指针，可能为 nullptr
     */
    virtual const CNGeometry* GetSpatialFilter() const = 0;
    
    /**
     * @brief 设置属性过滤器
     * @param query SQL WHERE 子句格式的查询字符串
     * @return 状态码
     */
    virtual CNStatus SetAttributeFilter(const std::string& query) = 0;
    
    /**
     * @brief 清除属性过滤器
     */
    virtual void ClearAttributeFilter() {
        SetAttributeFilter("");
    }
    
    // ========== 事务支持 ==========
    
    /**
     * @brief 开始事务
     * @return 状态码
     */
    virtual CNStatus StartTransaction() {
        return CNStatus::kNotSupported;
    }
    
    /**
     * @brief 提交事务
     * @return 状态码
     */
    virtual CNStatus CommitTransaction() {
        return CNStatus::kNotSupported;
    }
    
    /**
     * @brief 回滚事务
     * @return 状态码
     */
    virtual CNStatus RollbackTransaction() {
        return CNStatus::kNotSupported;
    }
    
    // ========== 能力测试 ==========
    
    /**
     * @brief 测试图层能力
     * @param capability 能力枚举值
     * @return 是否支持该能力
     */
    virtual bool TestCapability(CNLayerCapability capability) const = 0;
    
    /**
     * @brief 获取所有支持的能力
     * @return 能力列表
     */
    virtual std::vector<CNLayerCapability> GetCapabilities() const {
        std::vector<CNLayerCapability> caps;
        for (int i = 0; i < static_cast<int>(CNLayerCapability::kCurveGeometries); ++i) {
            if (TestCapability(static_cast<CNLayerCapability>(i))) {
                caps.push_back(static_cast<CNLayerCapability>(i));
            }
        }
        return caps;
    }
    
    // ========== 元数据 ==========
    
    /**
     * @brief 获取元数据项
     * @param key 元数据键
     * @return 元数据值，空字符串表示不存在
     */
    virtual std::string GetMetadata(const std::string& key) const {
        return "";
    }
    
    /**
     * @brief 设置元数据项
     * @param key 元数据键
     * @param value 元数据值
     * @return 状态码
     */
    virtual CNStatus SetMetadata(
        const std::string& key, 
        const std::string& value) {
        return CNStatus::kNotSupported;
    }
    
    // ========== 同步操作 ==========
    
    /**
     * @brief 同步到存储（用于文件格式）
     * @return 状态码
     */
    virtual CNStatus SyncToDisk() {
        return CNStatus::kSuccess;
    }
    
    // ========== 克隆 ==========
    
    /**
     * @brief 克隆图层
     * @return 新图层指针
     */
    virtual std::unique_ptr<CNLayer> Clone() const = 0;
    
protected:
    // ========== 受保护方法 ==========
    
    /**
     * @brief 通知要素定义已修改
     */
    void OnFeatureDefnModified();
    
    /**
     * @brief 通知要素已修改
     */
    void OnFeatureModified();
};

} // namespace OGC
```

### 5.2 能力测试宏定义

```cpp
// 便捷能力测试宏
#define CN_LAYER_CAN(layer, cap) \
    (layer)->TestCapability(OGC::CNLayerCapability::cap)

#define CN_LAYER_CAN_RANDOM_READ(layer) \
    CN_LAYER_CAN(layer, kRandomRead)

#define CN_LAYER_CAN_FAST_SPATIAL_FILTER(layer) \
    CN_LAYER_CAN(layer, kFastSpatialFilter)

#define CN_LAYER_CAN_CREATE_FEATURE(layer) \
    CN_LAYER_CAN(layer, kCreateFeature)

#define CN_LAYER_CAN_DELETE_FEATURE(layer) \
    CN_LAYER_CAN(layer, kDeleteFeature)

#define CN_LAYER_CAN_TRANSACTIONS(layer) \
    CN_LAYER_CAN(layer, kTransactions)
```

---

## 6. CNMemoryLayer 内存图层实现

### 6.1 类定义

```cpp
namespace OGC {

/**
 * @brief 内存图层实现
 * 
 * 特点:
 * - 所有数据存储在内存中
 * - 支持完整的事务操作
 * - 支持快速空间过滤（使用空间索引）
 * - 适用于临时数据处理和测试
 * 
 * 线程安全:
 * - 读操作: 线程安全
 * - 写操作: 需要外部同步
 * - 迭代: 非线程安全
 */
class CNMemoryLayer : public CNLayer {
public:
    // ========== 构造与析构 ==========
    
    /**
     * @brief 构造函数
     * @param name 图层名称
     * @param geom_type 几何类型
     * @param srs 空间参考（可选）
     */
    CNMemoryLayer(
        const std::string& name,
        GeomType geom_type,
        CNSpatialReference* srs = nullptr);
    
    /**
     * @brief 从要素定义构造
     * @param name 图层名称
     * @param feature_defn 要素定义
     */
    CNMemoryLayer(
        const std::string& name,
        std::shared_ptr<CNFeatureDefn> feature_defn);
    
    ~CNMemoryLayer() override;
    
    // 禁止拷贝
    CNMemoryLayer(const CNMemoryLayer&) = delete;
    CNMemoryLayer& operator=(const CNMemoryLayer&) = delete;
    
    // 允许移动
    CNMemoryLayer(CNMemoryLayer&&) noexcept;
    CNMemoryLayer& operator=(CNMemoryLayer&&) noexcept;
    
    // ========== CNLayer 接口实现 ==========
    
    const std::string& GetName() const override { return name_; }
    
    CNLayerType GetLayerType() const override { 
        return CNLayerType::kMemory; 
    }
    
    CNFeatureDefn* GetFeatureDefn() override;
    const CNFeatureDefn* GetFeatureDefn() const override;
    
    GeomType GetGeomType() const override;
    
    CNSpatialReference* GetSpatialRef() override;
    const CNSpatialReference* GetSpatialRef() const override;
    
    CNStatus GetExtent(CNEnvelope& extent, bool force = true) const override;
    
    int64_t GetFeatureCount(bool force = true) const override;
    
    void ResetReading() override;
    std::unique_ptr<CNFeature> GetNextFeature() override;
    CNFeature* GetNextFeatureRef() override;
    
    std::unique_ptr<CNFeature> GetFeature(int64_t fid) override;
    CNStatus SetFeature(const CNFeature* feature) override;
    
    CNStatus CreateFeature(CNFeature* feature) override;
    CNStatus DeleteFeature(int64_t fid) override;
    int64_t CreateFeatureBatch(
        const std::vector<CNFeature*>& features) override;
    
    CNStatus CreateField(
        const CNFieldDefn* field_defn,
        bool approx_ok = false) override;
    CNStatus DeleteField(int field_index) override;
    CNStatus ReorderFields(const std::vector<int>& new_order) override;
    CNStatus AlterFieldDefn(
        int field_index,
        const CNFieldDefn* new_defn,
        int flags) override;
    
    void SetSpatialFilterRect(
        double min_x, double min_y,
        double max_x, double max_y) override;
    void SetSpatialFilter(const CNGeometry* geometry) override;
    const CNGeometry* GetSpatialFilter() const override;
    
    CNStatus SetAttributeFilter(const std::string& query) override;
    
    CNStatus StartTransaction() override;
    CNStatus CommitTransaction() override;
    CNStatus RollbackTransaction() override;
    
    bool TestCapability(CNLayerCapability capability) const override;
    
    std::unique_ptr<CNLayer> Clone() const override;
    
    // ========== 内存图层特有方法 ==========
    
    /**
     * @brief 预分配内存
     * @param expected_count 预期要素数量
     */
    void Reserve(int64_t expected_count);
    
    /**
     * @brief 收缩内存到实际大小
     */
    void ShrinkToFit();
    
    /**
     * @brief 清空所有要素
     */
    void Clear();
    
    /**
     * @brief 获取内存使用量
     * @return 内存使用字节数
     */
    int64_t GetMemoryUsage() const;
    
    /**
     * @brief 设置 FID 自动生成
     * @param auto_gen 是否自动生成
     */
    void SetAutoFIDGeneration(bool auto_gen);
    
    /**
     * @brief 设置 FID 重用
     * @param reuse 是否重用已删除的 FID
     */
    void SetFIDReuse(bool reuse);
    
    /**
     * @brief 构建空间索引
     * @param index_type 索引类型
     */
    void BuildSpatialIndex(SpatialIndexType index_type = SpatialIndexType::kRTree);
    
    /**
     * @brief 获取空间索引
     * @return 空间索引指针
     */
    CNSpatialIndex* GetSpatialIndex();
    
    /**
     * @brief 空间查询
     * @param geometry 查询几何
     * @param relation 空间关系
     * @return 匹配的要素 FID 列表
     */
    std::vector<int64_t> SpatialQuery(
        const CNGeometry* geometry,
        CNSpatialRelation relation = CNSpatialRelation::kIntersects);
    
private:
    // ========== 私有成员 ==========
    
    std::string name_;
    std::shared_ptr<CNFeatureDefn> feature_defn_;
    CNSpatialReference* spatial_ref_;
    
    // 要素存储
    std::vector<std::unique_ptr<CNFeature>> features_;
    std::unordered_map<int64_t, size_t> fid_index_;
    
    // FID 管理
    int64_t next_fid_ = 1;
    bool auto_fid_generation_ = true;
    bool fid_reuse_ = true;
    std::set<int64_t> deleted_fids_;
    
    // 迭代状态
    size_t read_cursor_ = 0;
    
    // 过滤器
    std::unique_ptr<CNGeometry> spatial_filter_;
    std::string attribute_filter_;
    CNEnvelope filter_extent_;
    
    // 空间索引
    std::unique_ptr<CNSpatialIndex> spatial_index_;
    bool index_dirty_ = true;
    
    // 事务
    struct TransactionSnapshot {
        std::vector<std::unique_ptr<CNFeature>> features;
        std::unordered_map<int64_t, size_t> fid_index;
        int64_t next_fid;
        std::set<int64_t> deleted_fids;
    };
    std::stack<TransactionSnapshot> transaction_stack_;
    bool in_transaction_ = false;
    
    // 范围缓存
    mutable CNEnvelope cached_extent_;
    mutable bool extent_valid_ = false;
    
    // ========== 私有方法 ==========
    
    int64_t GenerateFID();
    void UpdateExtent(const CNFeature* feature);
    void InvalidateExtent();
    void ApplySpatialFilter();
    bool PassesFilters(const CNFeature* feature) const;
};

} // namespace OGC
```

### 6.2 实现要点

#### 6.2.1 要素存储策略

```cpp
// 混合存储结构：顺序存储 + 哈希索引
std::vector<std::unique_ptr<CNFeature>> features_;      // 顺序存储，遍历高效
std::unordered_map<int64_t, size_t> fid_index_;         // FID 到索引的映射

// 查找要素
CNFeature* CNMemoryLayer::GetFeatureRef(int64_t fid) {
    auto it = fid_index_.find(fid);
    if (it != fid_index_.end()) {
        return features_[it->second].get();
    }
    return nullptr;
}

// 添加要素
CNStatus CNMemoryLayer::CreateFeature(CNFeature* feature) {
    if (!feature) return CNStatus::kNullPointer;
    
    int64_t fid = feature->GetFID();
    if (fid <= 0 || fid_index_.find(fid) != fid_index_.end()) {
        if (auto_fid_generation_) {
            fid = GenerateFID();
            feature->SetFID(fid);
        } else {
            return CNStatus::kInvalidFID;
        }
    }
    
    size_t index = features_.size();
    features_.push_back(feature->Clone());
    fid_index_[fid] = index;
    
    UpdateExtent(feature);
    index_dirty_ = true;
    
    return CNStatus::kSuccess;
}
```

#### 6.2.2 FID 管理策略

```cpp
int64_t CNMemoryLayer::GenerateFID() {
    if (fid_reuse_ && !deleted_fids_.empty()) {
        int64_t reused_fid = *deleted_fids_.begin();
        deleted_fids_.erase(deleted_fids_.begin());
        return reused_fid;
    }
    
    while (fid_index_.find(next_fid_) != fid_index_.end()) {
        next_fid_++;
    }
    return next_fid_++;
}

CNStatus CNMemoryLayer::DeleteFeature(int64_t fid) {
    auto it = fid_index_.find(fid);
    if (it == fid_index_.end()) {
        return CNStatus::kFeatureNotFound;
    }
    
    size_t index = it->second;
    
    // 交换并弹出（O(1) 删除）
    if (index != features_.size() - 1) {
        int64_t last_fid = features_.back()->GetFID();
        fid_index_[last_fid] = index;
        std::swap(features_[index], features_.back());
    }
    
    features_.pop_back();
    fid_index_.erase(it);
    
    if (fid_reuse_) {
        deleted_fids_.insert(fid);
    }
    
    InvalidateExtent();
    index_dirty_ = true;
    
    return CNStatus::kSuccess;
}
```

#### 6.2.3 事务实现

```cpp
CNStatus CNMemoryLayer::StartTransaction() {
    if (in_transaction_) {
        return CNStatus::kTransactionActive;
    }
    
    TransactionSnapshot snapshot;
    snapshot.next_fid = next_fid_;
    snapshot.fid_index = fid_index_;
    snapshot.deleted_fids = deleted_fids_;
    
    for (const auto& f : features_) {
        snapshot.features.push_back(f->Clone());
    }
    
    transaction_stack_.push(std::move(snapshot));
    in_transaction_ = true;
    
    return CNStatus::kSuccess;
}

CNStatus CNMemoryLayer::CommitTransaction() {
    if (!in_transaction_) {
        return CNStatus::kNoTransaction;
    }
    
    transaction_stack_.pop();
    in_transaction_ = !transaction_stack_.empty();
    
    return CNStatus::kSuccess;
}

CNStatus CNMemoryLayer::RollbackTransaction() {
    if (!in_transaction_) {
        return CNStatus::kNoTransaction;
    }
    
    auto snapshot = std::move(transaction_stack_.top());
    transaction_stack_.pop();
    
    features_ = std::move(snapshot.features);
    fid_index_ = std::move(snapshot.fid_index);
    next_fid_ = snapshot.next_fid;
    deleted_fids_ = std::move(snapshot.deleted_fids);
    
    in_transaction_ = !transaction_stack_.empty();
    InvalidateExtent();
    index_dirty_ = true;
    
    return CNStatus::kSuccess;
}
```

---

## 7. CNVectorLayer 矢量图层实现

### 7.1 类定义

```cpp
namespace OGC {

/**
 * @brief 矢量图层抽象基类
 * 
 * 提供矢量数据访问的通用接口
 * 派生类实现特定格式的读写
 */
class CNVectorLayer : public CNLayer {
public:
    // ========== 构造与析构 ==========
    
    CNVectorLayer() = default;
    ~CNVectorLayer() override = default;
    
    CNLayerType GetLayerType() const override { 
        return CNLayerType::kVector; 
    }
    
    // ========== 矢量图层特有方法 ==========
    
    /**
     * @brief 获取数据源路径
     * @return 数据源路径
     */
    virtual std::string GetDataSourcePath() const = 0;
    
    /**
     * @brief 获取格式名称
     * @return 格式名称
     */
    virtual std::string GetFormatName() const = 0;
    
    /**
     * @brief 是否为只读
     * @return true 表示只读
     */
    virtual bool IsReadOnly() const = 0;
    
    /**
     * @brief 获取编码
     * @return 编码名称
     */
    virtual std::string GetEncoding() const { return "UTF-8"; }
    
    /**
     * @brief 获取几何字段数量
     * @return 几何字段数量
     */
    virtual int GetGeomFieldCount() const { return 1; }
    
    /**
     * @brief 获取几何字段定义
     * @param index 字段索引
     * @return 几何字段定义
     */
    virtual const CNGeomFieldDefn* GetGeomFieldDefn(int index) const;
    
    // ========== 坐标转换 ==========
    
    /**
     * @brief 设置坐标转换
     * @param target_srs 目标空间参考
     * @return 状态码
     */
    virtual CNStatus SetCoordinateTransform(
        const CNSpatialReference* target_srs);
    
    /**
     * @brief 清除坐标转换
     */
    virtual void ClearCoordinateTransform();
};

/**
 * @brief Shapefile 图层实现
 */
class CNShapefileLayer : public CNVectorLayer {
public:
    static std::unique_ptr<CNVectorLayer> Open(
        const std::string& path,
        bool update = false);
    
    const std::string& GetName() const override;
    std::string GetDataSourcePath() const override;
    std::string GetFormatName() const override { return "ESRI Shapefile"; }
    bool IsReadOnly() const override;
    
    // ... 其他接口实现
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/**
 * @brief GeoJSON 图层实现
 */
class CNGeoJSONLayer : public CNVectorLayer {
public:
    static std::unique_ptr<CNVectorLayer> Open(
        const std::string& path,
        bool update = false);
    
    static std::unique_ptr<CNVectorLayer> Parse(
        const std::string& json_string);
    
    std::string GetFormatName() const override { return "GeoJSON"; }
    
    // ... 其他接口实现
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/**
 * @brief GeoPackage 图层实现
 */
class CNGeoPackageLayer : public CNVectorLayer {
public:
    static std::unique_ptr<CNVectorLayer> Open(
        const std::string& path,
        const std::string& layer_name,
        bool update = false);
    
    std::string GetFormatName() const override { return "GeoPackage"; }
    
    // ... 其他接口实现
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/**
 * @brief PostGIS 图层实现
 */
class CNPostGISLayer : public CNVectorLayer {
public:
    struct ConnectionParams {
        std::string host = "localhost";
        int port = 5432;
        std::string database;
        std::string user;
        std::string password;
        std::string schema = "public";
        std::string table;
        std::string geom_column = "geom";
    };
    
    static std::unique_ptr<CNVectorLayer> Open(
        const ConnectionParams& params,
        bool update = false);
    
    std::string GetFormatName() const override { return "PostGIS"; }
    
    // ... 其他接口实现
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace OGC
```

### 7.2 PIMPL 模式实现

```cpp
// CNShapefileLayer 使用 PIMPL 模式隐藏实现细节
class CNShapefileLayer::Impl {
public:
    std::string path_;
    std::string name_;
    bool read_only_ = true;
    
    std::shared_ptr<CNFeatureDefn> feature_defn_;
    CNSpatialReference* spatial_ref_ = nullptr;
    
    // Shapefile 特有数据
    void* shp_handle_ = nullptr;
    void* dbf_handle_ = nullptr;
    
    int64_t feature_count_ = 0;
    size_t read_cursor_ = 0;
    
    CNEnvelope extent_;
    
    // ... 实现细节
};

CNShapefileLayer::CNShapefileLayer() : impl_(new Impl()) {}
CNShapefileLayer::~CNShapefileLayer() = default;

const std::string& CNShapefileLayer::GetName() const {
    return impl_->name_;
}

std::string CNShapefileLayer::GetDataSourcePath() const {
    return impl_->path_;
}
```

---

## 8. CNRasterLayer 栅格图层实现

### 8.1 类定义

```cpp
namespace OGC {

/**
 * @brief 栅格图层抽象基类
 */
class CNRasterLayer : public CNLayer {
public:
    CNRasterLayer() = default;
    ~CNRasterLayer() override = default;
    
    CNLayerType GetLayerType() const override { 
        return CNLayerType::kRaster; 
    }
    
    // ========== 栅格基本信息 ==========
    
    /**
     * @brief 获取栅格宽度（像素）
     */
    virtual int GetWidth() const = 0;
    
    /**
     * @brief 获取栅格高度（像素）
     */
    virtual int GetHeight() const = 0;
    
    /**
     * @brief 获取波段数量
     */
    virtual int GetBandCount() const = 0;
    
    /**
     * @brief 获取像素宽度（地图单位）
     */
    virtual double GetPixelWidth() const = 0;
    
    /**
     * @brief 获取像素高度（地图单位）
     */
    virtual double GetPixelHeight() const = 0;
    
    /**
     * @brief 获取数据类型
     */
    virtual CNDataType GetDataType() const = 0;
    
    // ========== 地理变换 ==========
    
    /**
     * @brief 获取地理变换参数
     * @param transform 6个参数的数组
     *   transform[0]: 左上角 X 坐标
     *   transform[1]: X 方向像素分辨率
     *   transform[2]: 旋转参数（通常为 0）
     *   transform[3]: 左上角 Y 坐标
     *   transform[4]: 旋转参数（通常为 0）
     *   transform[5]: Y 方向像素分辨率（通常为负）
     */
    virtual void GetGeoTransform(double* transform) const = 0;
    
    /**
     * @brief 设置地理变换参数
     */
    virtual CNStatus SetGeoTransform(const double* transform);
    
    // ========== 波段访问 ==========
    
    /**
     * @brief 获取波段
     * @param band_index 波段索引（从 1 开始）
     * @return 波段指针
     */
    virtual CNRasterBand* GetBand(int band_index) = 0;
    
    // ========== 数据读写 ==========
    
    /**
     * @brief 读取栅格数据
     * @param x_offset 起始 X 偏移（像素）
     * @param y_offset 起始 Y 偏移（像素）
     * @param x_size 读取宽度（像素）
     * @param y_size 读取高度（像素）
     * @param buffer 数据缓冲区
     * @param buffer_x_size 缓冲区宽度
     * @param buffer_y_size 缓冲区高度
     * @param data_type 数据类型
     * @param band_list 波段列表
     * @param band_count 波段数量
     * @return 状态码
     */
    virtual CNStatus ReadRaster(
        int x_offset, int y_offset,
        int x_size, int y_size,
        void* buffer,
        int buffer_x_size, int buffer_y_size,
        CNDataType data_type,
        const int* band_list = nullptr,
        int band_count = 0) = 0;
    
    /**
     * @brief 写入栅格数据
     */
    virtual CNStatus WriteRaster(
        int x_offset, int y_offset,
        int x_size, int y_size,
        const void* buffer,
        int buffer_x_size, int buffer_y_size,
        CNDataType data_type,
        const int* band_list = nullptr,
        int band_count = 0);
    
    // ========== 金字塔 ==========
    
    /**
     * @brief 获取金字塔层级数量
     */
    virtual int GetOverviewCount() const { return 0; }
    
    /**
     * @brief 获取金字塔图层
     * @param index 金字塔索引
     */
    virtual CNRasterLayer* GetOverview(int index) { return nullptr; }
    
    /**
     * @brief 构建金字塔
     * @param levels 金字塔层级列表
     * @param resampling 重采样方法
     */
    virtual CNStatus BuildOverviews(
        const std::vector<int>& levels,
        const std::string& resampling = "AVERAGE");
    
    // ========== 坐标转换 ==========
    
    /**
     * @brief 地图坐标转像素坐标
     */
    void GeoToPixel(
        double geo_x, double geo_y,
        double& pixel_x, double& pixel_y) const;
    
    /**
     * @brief 像素坐标转地图坐标
     */
    void PixelToGeo(
        double pixel_x, double pixel_y,
        double& geo_x, double& geo_y) const;
};

/**
 * @brief 栅格波段类
 */
class CNRasterBand {
public:
    virtual ~CNRasterBand() = default;
    
    /**
     * @brief 获取波段索引
     */
    virtual int GetBandIndex() const = 0;
    
    /**
     * @brief 获取数据类型
     */
    virtual CNDataType GetDataType() const = 0;
    
    /**
     * @brief 获取宽度
     */
    virtual int GetXSize() const = 0;
    
    /**
     * @brief 获取高度
     */
    virtual int GetYSize() const = 0;
    
    /**
     * @brief 获取无数据值
     */
    virtual bool GetNoDataValue(double* value) const = 0;
    
    /**
     * @brief 设置无数据值
     */
    virtual CNStatus SetNoDataValue(double value);
    
    /**
     * @brief 获取颜色解释
     */
    virtual CNColorInterp GetColorInterpretation() const;
    
    /**
     * @brief 获取颜色表
     */
    virtual const CNColorTable* GetColorTable() const;
    
    /**
     * @brief 读取波段数据
     */
    virtual CNStatus Read(
        int x_offset, int y_offset,
        int x_size, int y_size,
        void* buffer,
        int buffer_x_size, int buffer_y_size,
        CNDataType data_type) = 0;
    
    /**
     * @brief 写入波段数据
     */
    virtual CNStatus Write(
        int x_offset, int y_offset,
        int x_size, int y_size,
        const void* buffer,
        int buffer_x_size, int buffer_y_size,
        CNDataType data_type);
    
    /**
     * @brief 计算统计信息
     */
    virtual CNStatus ComputeStatistics(
        bool approx_ok,
        double* min_val,
        double* max_val,
        double* mean_val,
        double* std_dev);
};

/**
 * @brief 数据类型枚举
 */
enum class CNDataType {
    kUnknown = 0,
    kByte = 1,
    kUInt16 = 2,
    kInt16 = 3,
    kUInt32 = 4,
    kInt32 = 5,
    kFloat32 = 6,
    kFloat64 = 7,
    kCInt16 = 8,
    kCInt32 = 9,
    kCFloat32 = 10,
    kCFloat64 = 11
};

/**
 * @brief 颜色解释枚举
 */
enum class CNColorInterp {
    kUndefined = 0,
    kGrayIndex = 1,
    kPaletteIndex = 2,
    kRedBand = 3,
    kGreenBand = 4,
    kBlueBand = 5,
    kAlphaBand = 6,
    kHueBand = 7,
    kSaturationBand = 8,
    kLightnessBand = 9,
    kCyanBand = 10,
    kMagentaBand = 11,
    kYellowBand = 12,
    kBlackBand = 13
};

} // namespace OGC
```

---

## 9. 线程安全设计

### 9.1 线程安全策略

```cpp
namespace OGC {

/**
 * @brief 线程安全策略枚举
 */
enum class CNThreadSafetyLevel {
    kNone,          // 无线程安全保证
    kReadOnly,      // 只读操作线程安全
    kReadWrite,     // 读写操作线程安全（使用锁）
    kLockFree       // 无锁线程安全
};

/**
 * @brief 读写锁包装器
 */
class CNReadWriteLock {
public:
    CNReadWriteLock() = default;
    
    void LockRead() {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        read_lock_ = std::move(lock);
    }
    
    void UnlockRead() {
        read_lock_.release();
    }
    
    void LockWrite() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        write_lock_ = std::move(lock);
    }
    
    void UnlockWrite() {
        write_lock_.release();
    }
    
    // RAII 包装器
    class ReadGuard {
    public:
        explicit ReadGuard(CNReadWriteLock& lock) : lock_(lock) {
            lock_.LockRead();
        }
        ~ReadGuard() { lock_.UnlockRead(); }
    private:
        CNReadWriteLock& lock_;
    };
    
    class WriteGuard {
    public:
        explicit WriteGuard(CNReadWriteLock& lock) : lock_(lock) {
            lock_.LockWrite();
        }
        ~WriteGuard() { lock_.UnlockWrite(); }
    private:
        CNReadWriteLock& lock_;
    };
    
private:
    mutable std::shared_mutex mutex_;
    std::shared_lock<std::shared_mutex> read_lock_;
    std::unique_lock<std::shared_mutex> write_lock_;
};

} // namespace OGC
```

### 9.2 线程安全图层包装器

```cpp
namespace OGC {

/**
 * @brief 线程安全图层包装器
 * 
 * 使用读写锁保护图层操作
 */
class CNThreadSafeLayer : public CNLayer {
public:
    explicit CNThreadSafeLayer(std::unique_ptr<CNLayer> layer)
        : layer_(std::move(layer)) {}
    
    // ========== 读操作（共享锁） ==========
    
    const std::string& GetName() const override {
        CNReadWriteLock::ReadGuard guard(lock_);
        return layer_->GetName();
    }
    
    int64_t GetFeatureCount(bool force = true) const override {
        CNReadWriteLock::ReadGuard guard(lock_);
        return layer_->GetFeatureCount(force);
    }
    
    std::unique_ptr<CNFeature> GetFeature(int64_t fid) override {
        CNReadWriteLock::ReadGuard guard(lock_);
        return layer_->GetFeature(fid);
    }
    
    // ========== 写操作（独占锁） ==========
    
    CNStatus CreateFeature(CNFeature* feature) override {
        CNReadWriteLock::WriteGuard guard(lock_);
        return layer_->CreateFeature(feature);
    }
    
    CNStatus DeleteFeature(int64_t fid) override {
        CNReadWriteLock::WriteGuard guard(lock_);
        return layer_->DeleteFeature(fid);
    }
    
    CNStatus StartTransaction() override {
        CNReadWriteLock::WriteGuard guard(lock_);
        return layer_->StartTransaction();
    }
    
    CNStatus CommitTransaction() override {
        CNReadWriteLock::WriteGuard guard(lock_);
        return layer_->CommitTransaction();
    }
    
    CNStatus RollbackTransaction() override {
        CNReadWriteLock::WriteGuard guard(lock_);
        return layer_->RollbackTransaction();
    }
    
    // ... 其他方法委托
    
private:
    std::unique_ptr<CNLayer> layer_;
    mutable CNReadWriteLock lock_;
};

} // namespace OGC
```

### 9.3 并发迭代器

```cpp
namespace OGC {

/**
 * @brief 线程安全的图层快照迭代器
 * 
 * 创建图层快照，支持并发迭代
 */
class CNLayerSnapshot {
public:
    explicit CNLayerSnapshot(const CNLayer* layer);
    
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = CNFeature;
        using difference_type = std::ptrdiff_t;
        using pointer = CNFeature*;
        using reference = CNFeature&;
        
        Iterator& operator++();
        Iterator operator++(int);
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        CNFeature& operator*();
        CNFeature* operator->();
        
    private:
        friend class CNLayerSnapshot;
        size_t index_;
        CNLayerSnapshot* snapshot_;
    };
    
    Iterator begin();
    Iterator end();
    
    size_t size() const { return features_.size(); }
    
private:
    std::vector<std::unique_ptr<CNFeature>> features_;
};

// 使用示例
void ProcessLayerConcurrently(const CNLayer* layer) {
    CNLayerSnapshot snapshot(layer);
    
    std::vector<std::future<void>> futures;
    size_t chunk_size = snapshot.size() / std::thread::hardware_concurrency();
    
    auto begin = snapshot.begin();
    auto end = snapshot.end();
    
    for (auto it = begin; it != end; ) {
        auto chunk_end = it;
        for (size_t i = 0; i < chunk_size && chunk_end != end; ++i, ++chunk_end) {}
        
        futures.push_back(std::async(std::launch::async, [&it, chunk_end]() {
            for (auto f = it; f != chunk_end; ++f) {
                ProcessFeature(*f);
            }
        }));
        
        it = chunk_end;
    }
    
    for (auto& f : futures) {
        f.wait();
    }
}

} // namespace OGC
```

---

## 10. 性能优化策略

### 10.1 空间索引

```cpp
namespace OGC {

/**
 * @brief 空间索引类型
 */
enum class SpatialIndexType {
    kNone,
    kRTree,         // R树索引
    kQuadTree,      // 四叉树索引
    kGrid,          // 网格索引
    kSTRtree        // STR树（排序瓦片R树）
};

/**
 * @brief 空间索引接口
 */
class CNSpatialIndex {
public:
    virtual ~CNSpatialIndex() = default;
    
    /**
     * @brief 插入要素
     */
    virtual void Insert(int64_t fid, const CNEnvelope& extent) = 0;
    
    /**
     * @brief 删除要素
     */
    virtual void Remove(int64_t fid, const CNEnvelope& extent) = 0;
    
    /**
     * @brief 范围查询
     */
    virtual std::vector<int64_t> Query(const CNEnvelope& extent) const = 0;
    
    /**
     * @brief 清空索引
     */
    virtual void Clear() = 0;
    
    /**
     * @brief 获取索引类型
     */
    virtual SpatialIndexType GetType() const = 0;
    
    /**
     * @brief 获取索引统计信息
     */
    virtual size_t GetMemoryUsage() const = 0;
};

/**
 * @brief R树索引实现
 */
class CNRTreeIndex : public CNSpatialIndex {
public:
    explicit CNRTreeIndex(size_t max_children = 16);
    
    void Insert(int64_t fid, const CNEnvelope& extent) override;
    void Remove(int64_t fid, const CNEnvelope& extent) override;
    std::vector<int64_t> Query(const CNEnvelope& extent) const override;
    void Clear() override;
    SpatialIndexType GetType() const override { return SpatialIndexType::kRTree; }
    size_t GetMemoryUsage() const override;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/**
 * @brief 四叉树索引实现
 */
class CNQuadTreeIndex : public CNSpatialIndex {
public:
    explicit CNQuadTreeIndex(
        const CNEnvelope& extent,
        int max_depth = 8,
        int max_items = 16);
    
    void Insert(int64_t fid, const CNEnvelope& extent) override;
    void Remove(int64_t fid, const CNEnvelope& extent) override;
    std::vector<int64_t> Query(const CNEnvelope& extent) const override;
    void Clear() override;
    SpatialIndexType GetType() const override { return SpatialIndexType::kQuadTree; }
    size_t GetMemoryUsage() const override;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace OGC
```

### 10.2 缓存策略

```cpp
namespace OGC {

/**
 * @brief LRU 缓存实现
 */
template<typename Key, typename Value>
class CNLRUCache {
public:
    explicit CNLRUCache(size_t max_size) : max_size_(max_size) {}
    
    std::shared_ptr<Value> Get(const Key& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = cache_.find(key);
        if (it == cache_.end()) {
            return nullptr;
        }
        
        // 移动到最前面
        lru_list_.splice(lru_list_.begin(), lru_list_, it->second.second);
        return it->second.first;
    }
    
    void Put(const Key& key, std::shared_ptr<Value> value) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            it->second.first = value;
            lru_list_.splice(lru_list_.begin(), lru_list_, it->second.second);
            return;
        }
        
        // 淘汰最久未使用的
        while (cache_.size() >= max_size_) {
            auto last = lru_list_.back();
            cache_.erase(last);
            lru_list_.pop_back();
        }
        
        lru_list_.push_front(key);
        cache_[key] = {value, lru_list_.begin()};
    }
    
    void Clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.clear();
        lru_list_.clear();
    }
    
    size_t Size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return cache_.size();
    }
    
private:
    size_t max_size_;
    mutable std::mutex mutex_;
    std::list<Key> lru_list_;
    std::unordered_map<Key, 
        std::pair<std::shared_ptr<Value>, typename std::list<Key>::iterator>> cache_;
};

/**
 * @brief 要素缓存
 */
class CNFeatureCache {
public:
    explicit CNFeatureCache(size_t max_features = 10000)
        : cache_(max_features) {}
    
    std::shared_ptr<CNFeature> GetFeature(int64_t fid) {
        return cache_.Get(fid);
    }
    
    void PutFeature(int64_t fid, std::shared_ptr<CNFeature> feature) {
        cache_.Put(fid, feature);
    }
    
    void Clear() { cache_.Clear(); }
    
private:
    CNLRUCache<int64_t, CNFeature> cache_;
};

} // namespace OGC
```

### 10.3 批量操作优化

```cpp
namespace OGC {

/**
 * @brief 批量操作上下文
 */
class CNBatchContext {
public:
    explicit CNBatchContext(CNLayer* layer, size_t batch_size = 1000)
        : layer_(layer), batch_size_(batch_size) {}
    
    ~CNBatchContext() {
        Flush();
    }
    
    CNStatus AddFeature(CNFeature* feature) {
        pending_features_.push_back(feature->Clone());
        
        if (pending_features_.size() >= batch_size_) {
            return Flush();
        }
        return CNStatus::kSuccess;
    }
    
    CNStatus Flush() {
        if (pending_features_.empty()) {
            return CNStatus::kSuccess;
        }
        
        CNStatus status = CNStatus::kSuccess;
        
        if (layer_->TestCapability(CNLayerCapability::kTransactions)) {
            layer_->StartTransaction();
        }
        
        for (auto& f : pending_features_) {
            status = layer_->CreateFeature(f.get());
            if (status != CNStatus::kSuccess) {
                break;
            }
        }
        
        if (layer_->TestCapability(CNLayerCapability::kTransactions)) {
            if (status == CNStatus::kSuccess) {
                layer_->CommitTransaction();
            } else {
                layer_->RollbackTransaction();
            }
        }
        
        pending_features_.clear();
        return status;
    }
    
private:
    CNLayer* layer_;
    size_t batch_size_;
    std::vector<std::unique_ptr<CNFeature>> pending_features_;
};

} // namespace OGC
```

---

## 11. 使用示例

### 11.1 基本使用

```cpp
#include "cn_layer.h"
#include "cn_memory_layer.h"
#include "cn_vector_layer.h"
#include "cn_feature.h"
#include "cn_geometry.h"

using namespace OGC;

void BasicUsage() {
    // 创建内存图层
    CNMemoryLayer layer("cities", GeomType::kPoint);
    
    // 添加字段
    CNFieldDefn name_field("name", CNFieldType::kString);
    name_field.SetWidth(100);
    layer.CreateField(&name_field);
    
    CNFieldDefn pop_field("population", CNFieldType::kInteger64);
    layer.CreateField(&pop_field);
    
    // 创建要素
    CNFeature feature(layer.GetFeatureDefn());
    feature.SetGeometry(CNPoint(116.4, 39.9));  // 北京
    feature.SetField("name", "Beijing");
    feature.SetField("population", 21540000);
    layer.CreateFeature(&feature);
    
    // 遍历要素
    layer.ResetReading();
    while (auto f = layer.GetNextFeature()) {
        std::cout << "City: " << f->GetFieldAsString("name") << std::endl;
        std::cout << "Population: " << f->GetFieldAsInteger64("population") << std::endl;
    }
}
```

### 11.2 空间查询

```cpp
void SpatialQuery() {
    CNMemoryLayer layer("pois", GeomType::kPoint);
    
    // 添加要素...
    
    // 构建空间索引
    layer.BuildSpatialIndex(SpatialIndexType::kRTree);
    
    // 设置空间过滤器
    CNEnvelope extent(116.0, 39.0, 117.0, 40.0);  // 北京区域
    layer.SetSpatialFilterRect(
        extent.GetMinX(), extent.GetMinY(),
        extent.GetMaxX(), extent.GetMaxY());
    
    // 遍历过滤后的要素
    layer.ResetReading();
    while (auto f = layer.GetNextFeature()) {
        // 处理要素
    }
    
    // 清除过滤器
    layer.SetSpatialFilter(nullptr);
}
```

### 11.3 事务操作

```cpp
void TransactionExample() {
    CNMemoryLayer layer("data", GeomType::kPolygon);
    
    // 开始事务
    if (layer.StartTransaction() == CNStatus::kSuccess) {
        // 批量添加要素
        for (int i = 0; i < 1000; ++i) {
            CNFeature feature(layer.GetFeatureDefn());
            // 设置要素属性...
            layer.CreateFeature(&feature);
        }
        
        // 提交或回滚
        if (SomeCondition()) {
            layer.CommitTransaction();
        } else {
            layer.RollbackTransaction();
        }
    }
}
```

### 11.4 读取矢量文件

```cpp
void ReadVectorFile() {
    // 打开 Shapefile
    auto layer = CNShapefileLayer::Open("path/to/file.shp");
    if (!layer) {
        std::cerr << "Failed to open file" << std::endl;
        return;
    }
    
    // 获取基本信息
    std::cout << "Layer: " << layer->GetName() << std::endl;
    std::cout << "Features: " << layer->GetFeatureCount() << std::endl;
    std::cout << "Geometry: " << GetGeomTypeName(layer->GetGeomType()) << std::endl;
    
    // 遍历要素
    layer->ResetReading();
    while (auto feature = layer->GetNextFeature()) {
        auto geom = feature->GetGeometry();
        if (geom) {
            std::cout << "Geometry: " << geom->ToWKT() << std::endl;
        }
    }
}
```

### 11.5 连接 PostGIS

```cpp
void PostGISExample() {
    CNPostGISLayer::ConnectionParams params;
    params.host = "localhost";
    params.port = 5432;
    params.database = "gis_db";
    params.user = "postgres";
    params.password = "password";
    params.table = "cities";
    
    auto layer = CNPostGISLayer::Open(params);
    if (!layer) {
        std::cerr << "Failed to connect to PostGIS" << std::endl;
        return;
    }
    
    // 使用图层...
}
```

### 11.6 多线程处理

```cpp
void MultiThreadProcessing() {
    CNMemoryLayer layer("data", GeomType::kPoint);
    
    // 添加要素...
    
    // 创建线程安全包装器
    CNThreadSafeLayer safe_layer(layer.Clone());
    
    // 并发读取
    std::vector<std::future<int64_t>> futures;
    for (int i = 0; i < 4; ++i) {
        futures.push_back(std::async(std::launch::async, [&safe_layer, i]() {
            int64_t count = 0;
            safe_layer.ResetReading();
            while (auto f = safe_layer.GetNextFeature()) {
                ProcessFeature(*f);
                count++;
            }
            return count;
        }));
    }
    
    for (auto& f : futures) {
        std::cout << "Processed: " << f.get() << std::endl;
    }
}
```

### 11.7 使用图层快照

```cpp
void SnapshotExample() {
    CNMemoryLayer layer("data", GeomType::kPolygon);
    
    // 添加要素...
    
    // 创建快照
    CNLayerSnapshot snapshot(&layer);
    
    // 并发处理快照
    std::vector<std::future<void>> futures;
    size_t chunk_size = snapshot.size() / std::thread::hardware_concurrency();
    
    for (size_t i = 0; i < snapshot.size(); i += chunk_size) {
        futures.push_back(std::async(std::launch::async, [&snapshot, i, chunk_size]() {
            auto it = snapshot.begin();
            std::advance(it, i);
            
            auto end = it;
            size_t count = 0;
            while (it != snapshot.end() && count < chunk_size) {
                ProcessFeature(*it);
                ++it;
                ++count;
            }
        }));
    }
    
    for (auto& f : futures) {
        f.wait();
    }
}
```

---

## 12. 编译和依赖

### 12.1 编译要求

- **C++ 标准**: C++11 或更高
- **编译器**: 
  - GCC 4.8+
  - Clang 3.4+
  - MSVC 2015+

### 12.2 依赖库

#### 12.2.1 核心依赖

| 依赖 | 版本 | 用途 | 必需 |
|------|------|------|------|
| PROJ | 6.0+ | 坐标转换 | 是 |
| Boost.Geometry | 1.71+ | 空间索引(R树)、几何算法 | 是 |

#### 12.2.2 可选依赖

| 依赖 | 版本 | 用途 | 条件编译宏 |
|------|------|------|------------|
| GDAL | 3.0+ | 文件格式支持、互操作 | CN_HAS_GDAL |
| SQLite | 3.0+ | GeoPackage/SpatiaLite | CN_HAS_SQLITE |
| libpq | 12+ | PostGIS连接 | CN_HAS_POSTGIS |
| Google Test | 1.8+ | 单元测试 | 仅测试时 |

#### 12.2.3 依赖说明

**Boost.Geometry 说明**:
- 用于实现R树空间索引（`bgi::rtree`）
- 提供几何算法（相交、包含、距离等）
- 仅依赖Boost头文件，无需编译Boost库

**GDAL 互操作说明**:
- 启用CN_HAS_GDAL后，CNGDALAdapter可用
- 支持OGRLayer与CNLayer双向转换
- 注意：GDAL采用MIT/X协议，商业友好

**PostGIS 连接说明**:
- 使用libpq原生API连接PostgreSQL
- 建议使用连接池管理连接
- 支持异步查询和批量操作

### 12.3 CMake 配置

```cmake
cmake_minimum_required(VERSION 3.10)
project(cn_layer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找依赖
find_package(PROJ REQUIRED)
find_package(GDAL OPTIONAL_COMPONENTS)
find_package(SQLite3 OPTIONAL_COMPONENTS)

# 源文件
set(LAYER_SOURCES
    src/cn_layer.cpp
    src/cn_memory_layer.cpp
    src/cn_vector_layer.cpp
    src/cn_raster_layer.cpp
    src/cn_spatial_index.cpp
)

# 创建库
add_library(cn_layer ${LAYER_SOURCES})

target_include_directories(cn_layer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJ_INCLUDE_DIRS}
)

target_link_libraries(cn_layer PUBLIC
    ${PROJ_LIBRARIES}
)

if(GDAL_FOUND)
    target_compile_definitions(cn_layer PUBLIC CN_HAS_GDAL)
    target_link_libraries(cn_layer PUBLIC GDAL::GDAL)
endif()

if(SQLite3_FOUND)
    target_compile_definitions(cn_layer PUBLIC CN_HAS_SQLITE)
    target_link_libraries(cn_layer PUBLIC SQLite::SQLite3)
endif()

# 安装
install(TARGETS cn_layer
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include
)
```

---

## 13. 测试策略

### 13.1 单元测试

```cpp
#include <gtest/gtest.h>
#include "cn_memory_layer.h"
#include "cn_feature.h"

using namespace OGC;

class CNMemoryLayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        layer_ = std::make_unique<CNMemoryLayer>("test", GeomType::kPoint);
    }
    
    std::unique_ptr<CNMemoryLayer> layer_;
};

TEST_F(CNMemoryLayerTest, CreateFeature) {
    CNFeature feature(layer_->GetFeatureDefn());
    feature.SetGeometry(CNPoint(0, 0));
    
    EXPECT_EQ(layer_->CreateFeature(&feature), CNStatus::kSuccess);
    EXPECT_EQ(layer_->GetFeatureCount(), 1);
}

TEST_F(CNMemoryLayerTest, GetFeature) {
    CNFeature feature(layer_->GetFeatureDefn());
    feature.SetGeometry(CNPoint(1, 2));
    layer_->CreateFeature(&feature);
    
    int64_t fid = feature.GetFID();
    auto retrieved = layer_->GetFeature(fid);
    
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetFID(), fid);
}

TEST_F(CNMemoryLayerTest, DeleteFeature) {
    CNFeature feature(layer_->GetFeatureDefn());
    feature.SetGeometry(CNPoint(0, 0));
    layer_->CreateFeature(&feature);
    
    int64_t fid = feature.GetFID();
    EXPECT_EQ(layer_->DeleteFeature(fid), CNStatus::kSuccess);
    EXPECT_EQ(layer_->GetFeatureCount(), 0);
}

TEST_F(CNMemoryLayerTest, Transaction) {
    CNFeature feature(layer_->GetFeatureDefn());
    feature.SetGeometry(CNPoint(0, 0));
    
    EXPECT_EQ(layer_->StartTransaction(), CNStatus::kSuccess);
    layer_->CreateFeature(&feature);
    EXPECT_EQ(layer_->GetFeatureCount(), 1);
    EXPECT_EQ(layer_->CommitTransaction(), CNStatus::kSuccess);
}

TEST_F(CNMemoryLayerTest, TransactionRollback) {
    CNFeature feature(layer_->GetFeatureDefn());
    feature.SetGeometry(CNPoint(0, 0));
    
    EXPECT_EQ(layer_->StartTransaction(), CNStatus::kSuccess);
    layer_->CreateFeature(&feature);
    EXPECT_EQ(layer_->GetFeatureCount(), 1);
    EXPECT_EQ(layer_->RollbackTransaction(), CNStatus::kSuccess);
    EXPECT_EQ(layer_->GetFeatureCount(), 0);
}

TEST_F(CNMemoryLayerTest, SpatialFilter) {
    for (int i = 0; i < 10; ++i) {
        CNFeature feature(layer_->GetFeatureDefn());
        feature.SetGeometry(CNPoint(i * 10.0, i * 10.0));
        layer_->CreateFeature(&feature);
    }
    
    layer_->SetSpatialFilterRect(0, 0, 50, 50);
    layer_->ResetReading();
    
    int count = 0;
    while (auto f = layer_->GetNextFeature()) {
        count++;
    }
    EXPECT_EQ(count, 6);
}

TEST_F(CNMemoryLayerTest, SpatialIndex) {
    for (int i = 0; i < 1000; ++i) {
        CNFeature feature(layer_->GetFeatureDefn());
        feature.SetGeometry(CNPoint(i * 0.1, i * 0.1));
        layer_->CreateFeature(&feature);
    }
    
    layer_->BuildSpatialIndex(SpatialIndexType::kRTree);
    
    std::vector<int64_t> ids = layer_->SpatialQuery(
        CNPoint(50, 50).Buffer(10.0).get(),
        CNSpatialRelation::kIntersects
    );
    
    EXPECT_GT(ids.size(), 0);
}

} // namespace OGC
```

### 13.2 性能测试

```cpp
#include <benchmark/benchmark.h>
#include "cn_memory_layer.h"

using namespace OGC;

static void BM_CreateFeature(benchmark::State& state) {
    CNMemoryLayer layer("perf", GeomType::kPoint);
    CNFieldDefn field("value", CNFieldType::kInteger);
    layer.CreateField(&field);
    
    int64_t fid = 0;
    for (auto _ : state) {
        CNFeature feature(layer.GetFeatureDefn());
        feature.SetGeometry(CNPoint(0, 0));
        feature.SetField("value", static_cast<int>(fid));
        layer.CreateFeature(&feature);
        fid++;
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_CreateFeature);

static void BM_SpatialQuery(benchmark::State& state) {
    CNMemoryLayer layer("perf", GeomType::kPoint);
    
    for (int i = 0; i < 10000; ++i) {
        CNFeature feature(layer.GetFeatureDefn());
        feature.SetGeometry(CNPoint(
            static_cast<double>(rand() % 1000),
            static_cast<double>(rand() % 1000)));
        layer.CreateFeature(&feature);
    }
    
    layer.BuildSpatialIndex(SpatialIndexType::kRTree);
    
    CNEnvelope query_extent(400, 400, 600, 600);
    
    for (auto _ : state) {
        layer.SetSpatialFilterRect(
            query_extent.GetMinX(), query_extent.GetMinY(),
            query_extent.GetMaxX(), query_extent.GetMaxY());
        layer.ResetReading();
        
        int count = 0;
        while (auto f = layer.GetNextFeature()) {
            count++;
        }
        benchmark::DoNotOptimize(count);
    }
}
BENCHMARK(BM_SpatialQuery);

static void BM_ThreadSafeRead(benchmark::State& state) {
    CNMemoryLayer layer("perf", GeomType::kPoint);
    
    for (int i = 0; i < 1000; ++i) {
        CNFeature feature(layer.GetFeatureDefn());
        feature.SetGeometry(CNPoint(i, i));
        layer.CreateFeature(&feature);
    }
    
    CNThreadSafeLayer safe_layer(layer.Clone());
    
    for (auto _ : state) {
        safe_layer.ResetReading();
        while (auto f = safe_layer.GetNextFeature()) {
            benchmark::DoNotOptimize(f);
        }
    }
}
BENCHMARK(BM_ThreadSafeRead)->Threads(4);
```

### 13.3 集成测试

```cpp
class CNLayerIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_dir_ = std::string(TEST_DATA_DIR) + "/";
    }
    
    std::string test_dir_;
};

TEST_F(CNLayerIntegrationTest, ShapefileReadWrite) {
    std::string filepath = test_dir_ + "test_output.shp";
    
    {
        auto layer = CNShapefileLayer::Create(filepath, GeomType::kPolygon);
        ASSERT_TRUE(layer != nullptr);
        
        CNFieldDefn name_field("name", CNFieldType::kString);
        name_field.SetWidth(50);
        layer->CreateField(&name_field);
        
        CNFeature feature(layer->GetFeatureDefn());
        feature.SetGeometry(CNPolygon({
            {0, 0}, {10, 0}, {10, 10}, {0, 10}, {0, 0}
        }));
        feature.SetField("name", "Test Polygon");
        layer->CreateFeature(&feature);
    }
    
    {
        auto layer = CNShapefileLayer::Open(filepath);
        ASSERT_TRUE(layer != nullptr);
        EXPECT_EQ(layer->GetFeatureCount(), 1);
        
        auto feature = layer->GetFeature(0);
        ASSERT_TRUE(feature != nullptr);
        EXPECT_EQ(feature->GetFieldAsString("name"), "Test Polygon");
    }
}

TEST_F(CNLayerIntegrationTest, GeoJSONRoundTrip) {
    std::string filepath = test_dir_ + "test_output.geojson";
    
    CNMemoryLayer source("test", GeomType::kPoint);
    CNFieldDefn id_field("id", CNFieldType::kInteger);
    source.CreateField(&id_field);
    
    for (int i = 0; i < 100; ++i) {
        CNFeature feature(source.GetFeatureDefn());
        feature.SetGeometry(CNPoint(i * 0.1, i * 0.2));
        feature.SetField("id", i);
        source.CreateFeature(&feature);
    }
    
    EXPECT_TRUE(CNGeoJSONWriter::Write(filepath, &source));
    
    auto loaded = CNGeoJSONLayer::Open(filepath);
    ASSERT_TRUE(loaded != nullptr);
    EXPECT_EQ(loaded->GetFeatureCount(), 100);
}

TEST_F(CNLayerIntegrationTest, CRSConversion) {
    CNMemoryLayer layer("test", GeomType::kPoint);
    
    auto wgs84 = CNSpatialReference::FromEPSG(4326);
    layer.SetSpatialRef(wgs84.get());
    
    CNFeature feature(layer.GetFeatureDefn());
    feature.SetGeometry(CNPoint(116.4, 39.9));
    layer.CreateFeature(&feature);
    
    auto utm = CNSpatialReference::FromEPSG(32650);
    auto transformed = layer.TransformTo(utm.get());
    
    ASSERT_TRUE(transformed != nullptr);
    
    auto transformed_feature = transformed->GetFeature(0);
    ASSERT_TRUE(transformed_feature != nullptr);
    
    auto geom = transformed_feature->GetGeometry();
    ASSERT_TRUE(geom != nullptr);
    
    auto point = dynamic_cast<const CNPoint*>(geom);
    ASSERT_TRUE(point != nullptr);
    
    EXPECT_NEAR(point->X(), 448251.0, 1000.0);
    EXPECT_NEAR(point->Y(), 4420837.0, 1000.0);
}
```

### 13.4 测试覆盖率要求

| 模块 | 行覆盖率要求 | 分支覆盖率要求 |
|------|-------------|---------------|
| CNLayer 基类 | ≥ 90% | ≥ 85% |
| CNMemoryLayer | ≥ 95% | ≥ 90% |
| CNVectorLayer | ≥ 85% | ≥ 80% |
| CNRasterLayer | ≥ 80% | ≥ 75% |
| 空间索引 | ≥ 90% | ≥ 85% |
| 线程安全组件 | ≥ 95% | ≥ 90% |

---

## 14. 附录

### 14.1 OGC 标准参考

| 标准 | 说明 | 链接 |
|------|------|------|
| OGC Simple Feature Access - Part 1 | 通用架构 | https://www.ogc.org/standards/sfa |
| OGC Simple Feature Access - Part 2 | SQL 实现 | https://www.ogc.org/standards/sfs |
| OGC WFS 2.0 | Web 要素服务 | https://www.ogc.org/standards/wfs |
| OGC WMTS 1.0 | 瓦片地图服务 | https://www.ogc.org/standards/wmts |
| OGC GeoPackage | 地理包格式 | https://www.ogc.org/standards/gpkg |

### 14.2 GDAL 参考文档

| 文档 | 说明 | 链接 |
|------|------|------|
| GDAL Vector Data Model | 矢量数据模型 | https://gdal.org/user/vector_data_model.html |
| OGRLayer API | 图层接口文档 | https://gdal.org/api/ogrlayer_cpp.html |
| OGR Feature API | 要素接口文档 | https://gdal.org/api/ogrfeature_cpp.html |
| GDAL Driver Implementation | 驱动实现指南 | https://gdal.org/api/driver_cpp.html |

### 14.3 C++ 最佳实践参考

| 文档 | 说明 | 链接 |
|------|------|------|
| Google C++ Style Guide | 编码规范 | https://google.github.io/styleguide/cppguide.html |
| C++ Core Guidelines | 核心指南 | https://isocpp.github.io/CppCoreGuidelines/ |
| Effective Modern C++ | 现代 C++ 实践 | Scott Meyers 著 |

### 14.4 术语表

| 术语 | 英文 | 说明 |
|------|------|------|
| 图层 | Layer | 具有相同要素定义的要素集合 |
| 要素 | Feature | 由几何属性和非几何属性描述的地理实体 |
| 要素定义 | Feature Definition | 描述要素结构的元数据 |
| 字段定义 | Field Definition | 描述单个字段的元数据 |
| 空间参考系统 | Spatial Reference System (SRS) | 坐标系统定义 |
| 外包矩形 | Envelope / MBR | 最小外包矩形 |
| 空间索引 | Spatial Index | 加速空间查询的数据结构 |
| 空间过滤器 | Spatial Filter | 用于筛选要素的空间范围 |
| 属性过滤器 | Attribute Filter | 用于筛选要素的属性条件 |
| FID | Feature ID | 要素唯一标识符 |
| WKT | Well-Known Text | 几何对象的文本表示格式 |
| WKB | Well-Known Binary | 几何对象的二进制表示格式 |
| CRS | Coordinate Reference System | 坐标参考系统 |
| SRID | Spatial Reference ID | 空间参考标识符 |
| EPSG | European Petroleum Survey Group | 坐标系统编码标准组织 |

### 14.5 版本历史

| 版本 | 日期 | 修改内容 | 作者 |
|------|------|----------|------|
| 1.0 | 2026-03-17 | 初始版本创建 | Trae AI |

---

## 15. 总结

本设计文档详细描述了 CNLayer 图层模型的设计方案，遵循以下核心原则：

### 15.1 设计亮点

1. **OGC 标准兼容**: 完全遵循 OGC Simple Feature Access 标准
2. **现代 C++ 设计**: 使用 C++11 特性，包括智能指针、移动语义、RAII
3. **线程安全**: 提供多种并发访问策略，支持读写分离
4. **高性能**: 空间索引、缓存机制、批量操作优化
5. **可扩展**: 抽象基类设计，支持多种数据源
6. **跨平台**: 纯 C++11 实现，无平台依赖

### 15.2 实现优先级

| 优先级 | 模块 | 说明 |
|--------|------|------|
| P0 | CNLayer 抽象基类 | 核心接口定义 |
| P0 | CNMemoryLayer | 内存图层实现 |
| P0 | CNFeature/CNFeatureDefn | 要素模型 |
| P1 | CNVectorLayer | 矢量图层实现 |
| P1 | 空间索引 | R树/四叉树 |
| P1 | 线程安全组件 | 读写锁、快照 |
| P2 | CNRasterLayer | 栅格图层实现 |
| P2 | CNLayerGroup | 图层组 |
| P3 | WFS/WMTS 客户端 | 网络服务 |

### 15.3 后续工作

1. **性能优化**: SIMD 加速、GPU 计算
2. **格式扩展**: 更多矢量/栅格格式支持
3. **分布式支持**: 大数据集成
4. **三维支持**: 3D 几何、点云数据
5. **时态支持**: 时态 GIS 功能

---

## 16. 数据源抽象层设计 *(新增)*

### 16.1 CNDataSource 数据源接口

**设计背景**: 原设计缺少数据源抽象层，无法支持多图层文件格式（如GeoPackage）的统一管理，导致资源管理混乱。

```cpp
namespace OGC {

/**
 * @brief 数据源抽象基类
 * 
 * 管理文件句柄、数据库连接等资源
 * 支持多图层文件格式
 * 
 * 设计参考: GDAL GDALDataset
 */
class CNDataSource {
public:
    virtual ~CNDataSource() = default;
    
    // ========== 基本信息 ==========
    
    /**
     * @brief 获取数据源名称
     */
    virtual const std::string& GetName() const = 0;
    
    /**
     * @brief 获取数据源路径
     */
    virtual std::string GetPath() const = 0;
    
    /**
     * @brief 是否为只读
     */
    virtual bool IsReadOnly() const = 0;
    
    /**
     * @brief 获取驱动名称
     */
    virtual const char* GetDriverName() const = 0;
    
    // ========== 图层管理 ==========
    
    /**
     * @brief 获取图层数量
     */
    virtual int GetLayerCount() const = 0;
    
    /**
     * @brief 根据索引获取图层
     * @param index 图层索引
     * @return 图层指针，调用者不拥有所有权
     */
    virtual CNLayer* GetLayer(int index) = 0;
    
    /**
     * @brief 根据名称获取图层
     * @param name 图层名称
     * @return 图层指针，调用者不拥有所有权
     */
    virtual CNLayer* GetLayerByName(const std::string& name) = 0;
    
    /**
     * @brief 创建新图层
     * @param name 图层名称
     * @param geom_type 几何类型
     * @param srs 空间参考系统
     * @return 图层指针，调用者不拥有所有权
     */
    virtual CNLayer* CreateLayer(
        const std::string& name,
        GeomType geom_type,
        CNSpatialReference* srs = nullptr) = 0;
    
    /**
     * @brief 删除图层
     * @param name 图层名称
     * @return 状态码
     */
    virtual CNStatus DeleteLayer(const std::string& name) {
        return CNStatus::kNotSupported;
    }
    
    // ========== 事务支持 ==========
    
    /**
     * @brief 开始事务（数据源级别）
     */
    virtual CNStatus StartTransaction() {
        return CNStatus::kNotSupported;
    }
    
    /**
     * @brief 提交事务
     */
    virtual CNStatus CommitTransaction() {
        return CNStatus::kNotSupported;
    }
    
    /**
     * @brief 回滚事务
     */
    virtual CNStatus RollbackTransaction() {
        return CNStatus::kNotSupported;
    }
    
    // ========== 工厂方法 ==========
    
    /**
     * @brief 打开数据源
     * @param path 数据源路径
     * @param update 是否以更新模式打开
     * @return 数据源指针
     */
    static std::unique_ptr<CNDataSource> Open(
        const std::string& path,
        bool update = false);
    
    /**
     * @brief 创建数据源
     * @param path 数据源路径
     * @param driver 驱动名称
     * @return 数据源指针
     */
    static std::unique_ptr<CNDataSource> Create(
        const std::string& path,
        const std::string& driver);
};

/**
 * @brief 文件数据源实现
 */
class CNFileDataSource : public CNDataSource {
public:
    const std::string& GetName() const override { return name_; }
    std::string GetPath() const override { return path_; }
    
    int GetLayerCount() const override;
    CNLayer* GetLayer(int index) override;
    CNLayer* GetLayerByName(const std::string& name) override;
    CNLayer* CreateLayer(const std::string& name, GeomType geom_type,
                          CNSpatialReference* srs) override;
    
protected:
    std::string path_;
    std::string name_;
    std::vector<std::unique_ptr<CNLayer>> layers_;
    std::map<std::string, size_t> layer_index_;
};

/**
 * @brief 数据库数据源实现
 */
class CNDatabaseDataSource : public CNDataSource {
public:
    struct ConnectionParams {
        std::string host;
        int port;
        std::string database;
        std::string user;
        std::string password;
        std::string schema;
    };
    
    CNStatus Connect(const ConnectionParams& params);
    CNStatus Disconnect();
    bool IsConnected() const;
    
    CNStatus StartTransaction() override;
    CNStatus CommitTransaction() override;
    CNStatus RollbackTransaction() override;
    
protected:
    ConnectionParams params_;
    void* connection_;  // 数据库连接句柄
    bool in_transaction_ = false;
};

} // namespace OGC
```

### 16.2 CNDriver 驱动抽象

```cpp
namespace OGC {

/**
 * @brief 驱动抽象基类
 * 
 * 定义格式驱动的通用接口
 * 支持运行时驱动注册和发现
 */
class CNDriver {
public:
    virtual ~CNDriver() = default;
    
    // ========== 基本信息 ==========
    
    /**
     * @brief 获取驱动名称
     */
    virtual const char* GetName() const = 0;
    
    /**
     * @brief 获取驱动描述
     */
    virtual const char* GetDescription() const {
        return GetName();
    }
    
    /**
     * @brief 获取支持的扩展名列表
     */
    virtual std::vector<std::string> GetExtensions() const {
        return {};
    }
    
    /**
     * @brief 获取支持的MIME类型列表
     */
    virtual std::vector<std::string> GetMimeTypes() const {
        return {};
    }
    
    // ========== 能力测试 ==========
    
    /**
     * @brief 测试是否可以打开指定路径
     * @param path 数据源路径
     * @return true 表示可以打开
     */
    virtual bool CanOpen(const std::string& path) const = 0;
    
    /**
     * @brief 测试是否可以创建指定路径
     * @param path 数据源路径
     * @return true 表示可以创建
     */
    virtual bool CanCreate(const std::string& path) const {
        return false;
    }
    
    /**
     * @brief 是否支持更新模式
     */
    virtual bool SupportsUpdate() const {
        return false;
    }
    
    /**
     * @brief 是否支持多图层
     */
    virtual bool SupportsMultipleLayers() const {
        return false;
    }
    
    /**
     * @brief 是否支持事务
     */
    virtual bool SupportsTransactions() const {
        return false;
    }
    
    // ========== 数据源操作 ==========
    
    /**
     * @brief 打开数据源
     * @param path 数据源路径
     * @param update 是否以更新模式打开
     * @return 数据源指针
     */
    virtual std::unique_ptr<CNDataSource> Open(
        const std::string& path,
        bool update = false) = 0;
    
    /**
     * @brief 创建数据源
     * @param path 数据源路径
     * @param options 创建选项
     * @return 数据源指针
     */
    virtual std::unique_ptr<CNDataSource> Create(
        const std::string& path,
        const std::map<std::string, std::string>& options = {}) {
        (void)path;
        (void)options;
        return nullptr;
    }
    
    /**
     * @brief 删除数据源
     * @param path 数据源路径
     * @return 状态码
     */
    virtual CNStatus Delete(const std::string& path) {
        (void)path;
        return CNStatus::kNotSupported;
    }
};

/**
 * @brief Shapefile 驱动实现
 */
class CNShapefileDriver : public CNDriver {
public:
    const char* GetName() const override { return "ESRI Shapefile"; }
    
    std::vector<std::string> GetExtensions() const override {
        return {"shp"};
    }
    
    bool CanOpen(const std::string& path) const override;
    
    bool SupportsUpdate() const override { return true; }
    bool SupportsMultipleLayers() const override { return false; }
    
    std::unique_ptr<CNDataSource> Open(
        const std::string& path,
        bool update) override;
    
    std::unique_ptr<CNDataSource> Create(
        const std::string& path,
        const std::map<std::string, std::string>& options) override;
};

/**
 * @brief GeoPackage 驱动实现
 */
class CNGeoPackageDriver : public CNDriver {
public:
    const char* GetName() const override { return "GeoPackage"; }
    
    std::vector<std::string> GetExtensions() const override {
        return {"gpkg"};
    }
    
    bool CanOpen(const std::string& path) const override;
    
    bool SupportsUpdate() const override { return true; }
    bool SupportsMultipleLayers() const override { return true; }
    bool SupportsTransactions() const override { return true; }
    
    std::unique_ptr<CNDataSource> Open(
        const std::string& path,
        bool update) override;
    
    std::unique_ptr<CNDataSource> Create(
        const std::string& path,
        const std::map<std::string, std::string>& options) override;
};

/**
 * @brief PostGIS 驱动实现
 */
class CNPostGISDriver : public CNDriver {
public:
    const char* GetName() const override { return "PostGIS"; }
    
    bool CanOpen(const std::string& path) const override;
    
    bool SupportsUpdate() const override { return true; }
    bool SupportsMultipleLayers() const override { return true; }
    bool SupportsTransactions() const override { return true; }
    
    std::unique_ptr<CNDataSource> Open(
        const std::string& path,
        bool update) override;
};

} // namespace OGC
```

### 16.3 CNDriverManager 驱动注册器

```cpp
namespace OGC {

/**
 * @brief 驱动管理器（单例）
 * 
 * 管理所有注册的驱动
 * 支持运行时驱动发现和加载
 */
class CNDriverManager {
public:
    /**
     * @brief 获取单例实例
     */
    static CNDriverManager& Instance() {
        static CNDriverManager instance;
        return instance;
    }
    
    // ========== 驱动注册 ==========
    
    /**
     * @brief 注册驱动
     * @param driver 驱动实例
     */
    void RegisterDriver(std::unique_ptr<CNDriver> driver) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string name = driver->GetName();
        drivers_[name] = std::move(driver);
    }
    
    /**
     * @brief 注销驱动
     * @param name 驱动名称
     */
    void DeregisterDriver(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        drivers_.erase(name);
    }
    
    /**
     * @brief 注册内置驱动
     */
    void RegisterBuiltinDrivers() {
        RegisterDriver(std::make_unique<CNShapefileDriver>());
        RegisterDriver(std::make_unique<CNGeoPackageDriver>());
        RegisterDriver(std::make_unique<CNGeoJSONDriver>());
        RegisterDriver(std::make_unique<CNPostGISDriver>());
    }
    
    // ========== 驱动查找 ==========
    
    /**
     * @brief 根据名称获取驱动
     * @param name 驱动名称
     * @return 驱动指针，不存在返回 nullptr
     */
    CNDriver* GetDriver(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = drivers_.find(name);
        return it != drivers_.end() ? it->second.get() : nullptr;
    }
    
    /**
     * @brief 根据路径自动识别驱动
     * @param path 数据源路径
     * @return 驱动指针，无法识别返回 nullptr
     */
    CNDriver* FindDriver(const std::string& path) const {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& pair : drivers_) {
            if (pair.second->CanOpen(path)) {
                return pair.second.get();
            }
        }
        return nullptr;
    }
    
    /**
     * @brief 获取所有驱动
     * @return 驱动列表
     */
    std::vector<CNDriver*> GetDrivers() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<CNDriver*> result;
        for (const auto& pair : drivers_) {
            result.push_back(pair.second.get());
        }
        return result;
    }
    
    // ========== 数据源打开 ==========
    
    /**
     * @brief 打开数据源（自动识别驱动）
     * @param path 数据源路径
     * @param update 是否以更新模式打开
     * @return 数据源指针
     */
    std::unique_ptr<CNDataSource> Open(
        const std::string& path,
        bool update = false) const {
        
        CNDriver* driver = FindDriver(path);
        if (!driver) {
            return nullptr;
        }
        return driver->Open(path, update);
    }
    
private:
    CNDriverManager() {
        RegisterBuiltinDrivers();
    }
    
    CNDriverManager(const CNDriverManager&) = delete;
    CNDriverManager& operator=(const CNDriverManager&) = delete;
    
    mutable std::mutex mutex_;
    std::map<std::string, std::unique_ptr<CNDriver>> drivers_;
};

} // namespace OGC
```

---

## 17. 栅格数据模型设计 *(新增)*

### 17.1 CNRasterDataset 栅格数据集接口

**设计背景**: 原设计中 CNRasterLayer 继承自 CNLayer，但栅格数据模型与矢量差异巨大（无Feature概念），违反Liskov替换原则。

```cpp
namespace OGC {

/**
 * @brief 栅格数据集接口（独立于CNLayer）
 * 
 * 栅格数据与矢量数据模型差异：
 * - 矢量：要素集合，每个要素有几何和属性
 * - 栅格：像素网格，按波段组织，无要素概念
 * 
 * 因此栅格数据不应继承 CNLayer
 */
class CNRasterDataset {
public:
    virtual ~CNRasterDataset() = default;
    
    // ========== 基本信息 ==========
    
    /**
     * @brief 获取数据集名称
     */
    virtual const std::string& GetName() const = 0;
    
    /**
     * @brief 获取数据源路径
     */
    virtual std::string GetPath() const = 0;
    
    /**
     * @brief 是否为只读
     */
    virtual bool IsReadOnly() const = 0;
    
    // ========== 栅格尺寸 ==========
    
    /**
     * @brief 获取栅格宽度（像素）
     */
    virtual int GetWidth() const = 0;
    
    /**
     * @brief 获取栅格高度（像素）
     */
    virtual int GetHeight() const = 0;
    
    /**
     * @brief 获取波段数量
     */
    virtual int GetBandCount() const = 0;
    
    // ========== 地理参考 ==========
    
    /**
     * @brief 获取空间参考系统
     */
    virtual CNSpatialReference* GetSpatialRef() = 0;
    
    /**
     * @brief 获取地理变换参数
     * @param transform 6个参数的数组
     *   transform[0]: 左上角 X 坐标
     *   transform[1]: X 方向像素分辨率
     *   transform[2]: 旋转参数
     *   transform[3]: 左上角 Y 坐标
     *   transform[4]: 旋转参数
     *   transform[5]: Y 方向像素分辨率（通常为负）
     */
    virtual void GetGeoTransform(double* transform) const = 0;
    
    /**
     * @brief 设置地理变换参数
     */
    virtual CNStatus SetGeoTransform(const double* transform);
    
    /**
     * @brief 获取像素宽度（地图单位）
     */
    virtual double GetPixelWidth() const;
    
    /**
     * @brief 获取像素高度（地图单位）
     */
    virtual double GetPixelHeight() const;
    
    // ========== 波段访问 ==========
    
    /**
     * @brief 获取波段
     * @param index 波段索引（从 1 开始）
     * @return 波段指针
     */
    virtual CNRasterBand* GetBand(int index) = 0;
    
    // ========== 数据读写 ==========
    
    /**
     * @brief 读取栅格数据
     */
    virtual CNStatus ReadRaster(
        int x_offset, int y_offset,
        int x_size, int y_size,
        void* buffer,
        int buffer_x_size, int buffer_y_size,
        CNDataType data_type,
        const int* band_list = nullptr,
        int band_count = 0) = 0;
    
    /**
     * @brief 写入栅格数据
     */
    virtual CNStatus WriteRaster(
        int x_offset, int y_offset,
        int x_size, int y_size,
        const void* buffer,
        int buffer_x_size, int buffer_y_size,
        CNDataType data_type,
        const int* band_list = nullptr,
        int band_count = 0);
    
    // ========== 金字塔 ==========
    
    /**
     * @brief 获取金字塔层级数量
     */
    virtual int GetOverviewCount() const { return 0; }
    
    /**
     * @brief 获取金字塔数据集
     */
    virtual CNRasterDataset* GetOverview(int index);
    
    /**
     * @brief 构建金字塔
     */
    virtual CNStatus BuildOverviews(
        const std::vector<int>& levels,
        const std::string& resampling = "AVERAGE");
    
    // ========== 坐标转换 ==========
    
    /**
     * @brief 地图坐标转像素坐标
     */
    void GeoToPixel(double geo_x, double geo_y,
                    double& pixel_x, double& pixel_y) const;
    
    /**
     * @brief 像素坐标转地图坐标
     */
    void PixelToGeo(double pixel_x, double pixel_y,
                    double& geo_x, double& geo_y) const;
    
    /**
     * @brief 获取范围
     */
    CNEnvelope GetExtent() const;
    
    // ========== 工厂方法 ==========
    
    static std::unique_ptr<CNRasterDataset> Open(
        const std::string& path,
        bool update = false);
};

/**
 * @brief 栅格波段类
 */
class CNRasterBand {
public:
    virtual ~CNRasterBand() = default;
    
    virtual int GetBandIndex() const = 0;
    virtual CNDataType GetDataType() const = 0;
    virtual int GetXSize() const = 0;
    virtual int GetYSize() const = 0;
    
    virtual bool GetNoDataValue(double* value) const = 0;
    virtual CNStatus SetNoDataValue(double value);
    
    virtual CNColorInterp GetColorInterpretation() const;
    virtual const CNColorTable* GetColorTable() const;
    
    virtual CNStatus Read(int x_offset, int y_offset,
                          int x_size, int y_size,
                          void* buffer,
                          int buffer_x_size, int buffer_y_size,
                          CNDataType data_type) = 0;
    
    virtual CNStatus Write(int x_offset, int y_offset,
                           int x_size, int y_size,
                           const void* buffer,
                           int buffer_x_size, int buffer_y_size,
                           CNDataType data_type);
    
    virtual CNStatus ComputeStatistics(
        bool approx_ok,
        double* min_val, double* max_val,
        double* mean_val, double* std_dev);
};

} // namespace OGC
```

### 17.2 与矢量模型的分离设计

```
┌─────────────────────────────────────────────────────────────────┐
│                        应用层                                    │
│         统一的空间数据访问接口（可选的适配层）                     │
└───────────────────────────┬─────────────────────────────────────┘
                            │
        ┌───────────────────┴───────────────────┐
        │                                       │
┌───────▼────────┐                    ┌────────▼────────┐
│   矢量数据模型  │                    │   栅格数据模型   │
│ CNDataSource   │                    │ CNRasterDataset │
│ CNLayer        │                    │ CNRasterBand    │
│ CNFeature      │                    │                 │
└───────┬────────┘                    └────────┬────────┘
        │                                      │
        └──────────────────┬───────────────────┘
                           │
                ┌──────────▼──────────┐
                │   共享基础设施       │
                │ CNSpatialReference  │
                │ CNEnvelope          │
                │ CNDriverManager     │
                └─────────────────────┘
```

**设计决策说明**:

| 方面 | 矢量模型 | 栅格模型 |
|------|----------|----------|
| 数据单元 | Feature（要素） | Pixel（像素） |
| 空间表示 | Geometry（几何） | Grid（网格） |
| 属性存储 | Field（字段） | Band（波段） |
| 空间查询 | 空间关系谓词 | 范围裁剪 |
| 典型操作 | 叠加分析、拓扑操作 | 重采样、波段运算 |

---

## 18. GDAL 互操作层设计 *(新增)*

### 18.1 CNGDALAdapter 适配器

**设计背景**: 缺少与GDAL的互操作层，难以复用现有GDAL生态。

```cpp
namespace OGC {

#ifdef CN_HAS_GDAL

/**
 * @brief GDAL适配器
 * 
 * 提供与GDAL/OGR的数据互操作能力
 * 
 * 性能说明:
 * - 要素转换需要深拷贝（OGRFeature与CNFeature内存布局不同）
 * - 几何转换需要深拷贝（OGRGeometry与CNGeometry结构不同）
 * - WrapLayer/WrapDataset为轻量包装，不复制数据
 * - 批量转换时建议使用CopyFeatures接口
 */
class CNGDALAdapter {
public:
    // ========== 图层适配 ==========
    
    /**
     * @brief 将OGRLayer包装为CNLayer
     * @param layer OGRLayer指针（不获取所有权）
     * @return CNLayer指针
     */
    static std::unique_ptr<CNLayer> WrapLayer(OGRLayer* layer);
    
    /**
     * @brief 将CNLayer解包为OGRLayer
     * @param layer CNLayer指针
     * @return OGRLayer指针（如果源是GDAL图层）
     */
    static OGRLayer* UnwrapLayer(CNLayer* layer);
    
    // ========== 数据源适配 ==========
    
    /**
     * @brief 将GDALDataset包装为CNDataSource
     */
    static std::unique_ptr<CNDataSource> WrapDataset(GDALDataset* dataset);
    
    /**
     * @brief 将CNDataSource解包为GDALDataset
     */
    static GDALDataset* UnwrapDataset(CNDataSource* datasource);
    
    // ========== 要素转换 ==========
    
    /**
     * @brief 将OGRFeature转换为CNFeature
     * @param feature OGRFeature指针
     * @param defn 目标要素定义（可选）
     * @return CNFeature指针
     */
    static std::unique_ptr<CNFeature> ConvertFeature(
        const OGRFeature* feature,
        CNFeatureDefn* defn = nullptr);
    
    /**
     * @brief 将CNFeature转换为OGRFeature
     * @param feature CNFeature指针
     * @param defn 目标要素定义（可选）
     * @return OGRFeature指针
     */
    static OGRFeature* ConvertFeature(
        const CNFeature* feature,
        OGRFeatureDefn* defn = nullptr);
    
    // ========== 几何转换 ==========
    
    /**
     * @brief 将OGRGeometry转换为CNGeometry
     */
    static std::unique_ptr<CNGeometry> ConvertGeometry(
        const OGRGeometry* geom);
    
    /**
     * @brief 将CNGeometry转换为OGRGeometry
     */
    static OGRGeometry* ConvertGeometry(
        const CNGeometry* geom);
    
    // ========== 空间参考转换 ==========
    
    /**
     * @brief 将OGRSpatialReference转换为CNSpatialReference
     */
    static std::unique_ptr<CNSpatialReference> ConvertSRS(
        const OGRSpatialReference* srs);
    
    /**
     * @brief 将CNSpatialReference转换为OGRSpatialReference
     */
    static OGRSpatialReference* ConvertSRS(
        const CNSpatialReference* srs);
    
    // ========== 批量转换 ==========
    
    /**
     * @brief 批量转换图层要素
     * @param src_layer 源图层
     * @param dst_layer 目标图层
     * @param count 转换数量（-1表示全部）
     * @return 实际转换数量
     */
    static int64_t CopyFeatures(
        OGRLayer* src_layer,
        CNLayer* dst_layer,
        int64_t count = -1);
    
    static int64_t CopyFeatures(
        CNLayer* src_layer,
        OGRLayer* dst_layer,
        int64_t count = -1);
};

/**
 * @brief GDAL图层包装器
 * 
 * 将OGRLayer包装为CNLayer接口
 * 支持零拷贝访问
 */
class CNGDALLayerWrapper : public CNLayer {
public:
    explicit CNGDALLayerWrapper(OGRLayer* layer, bool own = false);
    ~CNGDALLayerWrapper() override;
    
    const std::string& GetName() const override;
    CNLayerType GetLayerType() const override { return CNLayerType::kVector; }
    CNFeatureDefn* GetFeatureDefn() override;
    GeomType GetGeomType() const override;
    CNSpatialReference* GetSpatialRef() override;
    CNStatus GetExtent(CNEnvelope& extent, bool force = true) const override;
    
    int64_t GetFeatureCount(bool force = true) const override;
    void ResetReading() override;
    std::unique_ptr<CNFeature> GetNextFeature() override;
    std::unique_ptr<CNFeature> GetFeature(int64_t fid) override;
    
    CNStatus CreateFeature(CNFeature* feature) override;
    CNStatus DeleteFeature(int64_t fid) override;
    
    void SetSpatialFilter(const CNGeometry* geometry) override;
    void SetSpatialFilterRect(double min_x, double min_y,
                               double max_x, double max_y) override;
    CNStatus SetAttributeFilter(const std::string& query) override;
    
    CNStatus StartTransaction() override;
    CNStatus CommitTransaction() override;
    CNStatus RollbackTransaction() override;
    
    bool TestCapability(CNLayerCapability capability) const override;
    
    OGRLayer* GetOGRLayer() { return ogr_layer_; }
    
private:
    OGRLayer* ogr_layer_;
    bool own_layer_;
    std::string name_;
    std::shared_ptr<CNFeatureDefn> feature_defn_;
};

#endif // CN_HAS_GDAL

} // namespace OGC
```

### 18.2 数据转换接口

```cpp
namespace OGC {

/**
 * @brief 数据转换器接口
 */
class CNDataConverter {
public:
    virtual ~CNDataConverter() = default;
    
    /**
     * @brief 转换图层
     * @param src 源图层
     * @param dst 目标图层
     * @param options 转换选项
     * @return 转换的要素数量
     */
    virtual int64_t Convert(
        CNLayer* src,
        CNLayer* dst,
        const std::map<std::string, std::string>& options = {}) = 0;
    
    /**
     * @brief 转换数据源
     */
    virtual int64_t Convert(
        CNDataSource* src,
        CNDataSource* dst,
        const std::map<std::string, std::string>& options = {}) = 0;
};

/**
 * @brief 格式转换器
 */
class CNFormatConverter : public CNDataConverter {
public:
    /**
     * @brief 转换文件格式
     * @param src_path 源文件路径
     * @param dst_path 目标文件路径
     * @param dst_driver 目标驱动名称
     * @param options 转换选项
     * @return 状态码
     */
    static CNStatus ConvertFile(
        const std::string& src_path,
        const std::string& dst_path,
        const std::string& dst_driver,
        const std::map<std::string, std::string>& options = {});
    
    int64_t Convert(
        CNLayer* src,
        CNLayer* dst,
        const std::map<std::string, std::string>& options) override;
    
    int64_t Convert(
        CNDataSource* src,
        CNDataSource* dst,
        const std::map<std::string, std::string>& options) override;
};

} // namespace OGC
```

---

## 19. 性能优化增强设计 *(新增)*

### 19.1 增量事务快照

**设计背景**: 原事务实现复制所有要素，大数据量时内存翻倍。

```cpp
namespace OGC {

/**
 * @brief 增量事务快照
 * 
 * 仅记录变更，而非复制全部数据
 * 内存占用从 O(n) 降为 O(变更量)
 */
struct IncrementalSnapshot {
    std::set<int64_t> added_fids;              // 新增的FID
    std::set<int64_t> deleted_fids;            // 删除的FID
    std::map<int64_t, std::unique_ptr<CNFeature>> original_features;  // 修改前的副本
    int64_t snapshot_next_fid;                 // 快照时的next_fid_
    CNEnvelope snapshot_extent;                // 快照时的范围
    bool snapshot_extent_valid;
};

class CNMemoryLayer {
public:
    CNStatus StartTransaction() override {
        if (in_transaction_) {
            return CNStatus::kTransactionActive;
        }
        
        IncrementalSnapshot snapshot;
        snapshot.snapshot_next_fid = next_fid_;
        snapshot.snapshot_extent = cached_extent_;
        snapshot.snapshot_extent_valid = extent_valid_;
        
        transaction_snapshots_.push(std::move(snapshot));
        in_transaction_ = true;
        
        return CNStatus::kSuccess;
    }
    
    CNStatus CommitTransaction() override {
        if (!in_transaction_) {
            return CNStatus::kNoTransaction;
        }
        
        transaction_snapshots_.pop();
        in_transaction_ = !transaction_snapshots_.empty();
        
        return CNStatus::kSuccess;
    }
    
    CNStatus RollbackTransaction() override {
        if (!in_transaction_) {
            return CNStatus::kNoTransaction;
        }
        
        auto snapshot = std::move(transaction_snapshots_.top());
        transaction_snapshots_.pop();
        
        // 删除新增的要素
        for (int64_t fid : snapshot.added_fids) {
            auto it = fid_index_.find(fid);
            if (it != fid_index_.end()) {
                features_[it->second].reset();
                fid_index_.erase(it);
            }
        }
        
        // 恢复删除的要素
        for (int64_t fid : snapshot.deleted_fids) {
            auto it = snapshot.original_features.find(fid);
            if (it != snapshot.original_features.end()) {
                size_t index = features_.size();
                features_.push_back(std::move(it->second));
                fid_index_[fid] = index;
            }
        }
        
        // 恢复修改的要素
        for (auto& pair : snapshot.original_features) {
            auto it = fid_index_.find(pair.first);
            if (it != fid_index_.end()) {
                features_[it->second] = std::move(pair.second);
            }
        }
        
        next_fid_ = snapshot.snapshot_next_fid;
        cached_extent_ = snapshot.snapshot_extent;
        extent_valid_ = snapshot.snapshot_extent_valid;
        
        in_transaction_ = !transaction_snapshots_.empty();
        index_dirty_ = true;
        
        return CNStatus::kSuccess;
    }
    
private:
    std::stack<IncrementalSnapshot> transaction_snapshots_;
    
    void RecordModification(int64_t fid, CNFeature* original) {
        if (in_transaction_) {
            auto& snapshot = transaction_snapshots_.top();
            if (snapshot.added_fids.find(fid) == snapshot.added_fids.end() &&
                snapshot.original_features.find(fid) == snapshot.original_features.end()) {
                snapshot.original_features[fid] = original->Clone();
            }
        }
    }
};

} // namespace OGC
```

### 19.2 分段锁缓存

**设计背景**: 原LRU缓存每次操作都加全局锁，高并发下成为瓶颈。

```cpp
namespace OGC {

/**
 * @brief 分段LRU缓存
 * 
 * 将缓存分为多个段，每个段独立加锁
 * 大幅降低锁竞争
 * 
 * @tparam Key 键类型
 * @tparam Value 值类型
 * @tparam NumShards 分段数量（应为2的幂次）
 */
template<typename Key, typename Value, size_t NumShards = 16>
class CNShardedLRUCache {
public:
    explicit CNShardedLRUCache(size_t max_size_per_shard)
        : max_size_per_shard_(max_size_per_shard) {
        for (auto& shard : shards_) {
            shard = std::make_unique<Shard>(max_size_per_shard);
        }
    }
    
    std::shared_ptr<Value> Get(const Key& key) {
        size_t shard_index = GetShardIndex(key);
        return shards_[shard_index]->Get(key);
    }
    
    void Put(const Key& key, std::shared_ptr<Value> value) {
        size_t shard_index = GetShardIndex(key);
        shards_[shard_index]->Put(key, value);
    }
    
    void Clear() {
        for (auto& shard : shards_) {
            shard->Clear();
        }
    }
    
    size_t Size() const {
        size_t total = 0;
        for (const auto& shard : shards_) {
            total += shard->Size();
        }
        return total;
    }
    
private:
    size_t GetShardIndex(const Key& key) const {
        return std::hash<Key>{}(key) & (NumShards - 1);
    }
    
    struct Shard {
        explicit Shard(size_t max_size) : max_size(max_size) {}
        
        std::shared_ptr<Value> Get(const Key& key) {
            std::lock_guard<std::mutex> lock(mutex);
            auto it = cache.find(key);
            if (it == cache.end()) {
                return nullptr;
            }
            lru_list.splice(lru_list.begin(), lru_list, it->second.second);
            return it->second.first;
        }
        
        void Put(const Key& key, std::shared_ptr<Value> value) {
            std::lock_guard<std::mutex> lock(mutex);
            auto it = cache.find(key);
            if (it != cache.end()) {
                it->second.first = value;
                lru_list.splice(lru_list.begin(), lru_list, it->second.second);
                return;
            }
            
            while (cache.size() >= max_size) {
                auto last = lru_list.back();
                cache.erase(last);
                lru_list.pop_back();
            }
            
            lru_list.push_front(key);
            cache[key] = {value, lru_list.begin()};
        }
        
        void Clear() {
            std::lock_guard<std::mutex> lock(mutex);
            cache.clear();
            lru_list.clear();
        }
        
        size_t Size() const {
            std::lock_guard<std::mutex> lock(mutex);
            return cache.size();
        }
        
        size_t max_size;
        mutable std::mutex mutex;
        std::list<Key> lru_list;
        std::unordered_map<Key, 
            std::pair<std::shared_ptr<Value>, 
                      typename std::list<Key>::iterator>> cache;
    };
    
    size_t max_size_per_shard_;
    std::array<std::unique_ptr<Shard>, NumShards> shards_;
};

/**
 * @brief 高性能要素缓存
 */
class CNHighPerfFeatureCache {
public:
    explicit CNHighPerfFeatureCache(size_t max_features = 10000)
        : cache_((max_features + 15) / 16) {}
    
    std::shared_ptr<CNFeature> GetFeature(int64_t fid) {
        return cache_.Get(fid);
    }
    
    void PutFeature(int64_t fid, std::shared_ptr<CNFeature> feature) {
        cache_.Put(fid, feature);
    }
    
    void Clear() { cache_.Clear(); }
    
private:
    CNShardedLRUCache<int64_t, CNFeature, 16> cache_;
};

} // namespace OGC
```

### 19.3 对象池设计

**设计背景**: 频繁创建/销毁CNFeature对象，小对象分配开销大。

```cpp
namespace OGC {

/**
 * @brief 对象池
 * 
 * 复用对象，减少内存分配开销
 * 线程安全
 */
template<typename T>
class CNObjectPool {
public:
    explicit CNObjectPool(size_t max_size = 1024)
        : max_size_(max_size) {}
    
    /**
     * @brief 获取对象
     * @return 对象指针，使用自定义删除器归还到池
     */
    std::unique_ptr<T, std::function<void(T*)>> Acquire() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (pool_.empty()) {
            return std::unique_ptr<T, std::function<void(T*)>>(
                new T(),
                [this](T* p) { Return(p); });
        }
        
        T* obj = pool_.back();
        pool_.pop_back();
        return std::unique_ptr<T, std::function<void(T*)>>(
            obj, [this](T* p) { Return(p); });
    }
    
    /**
     * @brief 获取并初始化对象
     */
    template<typename... Args>
    std::unique_ptr<T, std::function<void(T*)>> Acquire(Args&&... args) {
        auto obj = Acquire();
        obj->Reset(std::forward<Args>(args)...);
        return obj;
    }
    
    /**
     * @brief 预热池
     */
    void WarmUp(size_t count) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (size_t i = pool_.size(); i < count && i < max_size_; ++i) {
            pool_.push_back(new T());
        }
    }
    
    size_t Size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return pool_.size();
    }
    
private:
    void Return(T* obj) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (pool_.size() < max_size_) {
            obj->Clear();
            pool_.push_back(obj);
        } else {
            delete obj;
        }
    }
    
    size_t max_size_;
    mutable std::mutex mutex_;
    std::vector<T*> pool_;
};

/**
 * @brief 要素对象池
 */
class CNFeaturePool : public CNObjectPool<CNFeature> {
public:
    static CNFeaturePool& Instance() {
        static CNFeaturePool instance(4096);
        return instance;
    }
    
private:
    explicit CNFeaturePool(size_t max_size) : CNObjectPool<CNFeature>(max_size) {}
};

} // namespace OGC
```

### 19.4 增量空间索引更新

**设计背景**: 原设计每次修改后标记索引脏，未实现增量更新。

```cpp
namespace OGC {

/**
 * @brief 支持增量更新的空间索引接口
 */
class CNIncrementalSpatialIndex : public CNSpatialIndex {
public:
    /**
     * @brief 插入要素（增量）
     */
    virtual void Insert(int64_t fid, const CNEnvelope& envelope) = 0;
    
    /**
     * @brief 删除要素（增量）
     */
    virtual void Remove(int64_t fid, const CNEnvelope& envelope) = 0;
    
    /**
     * @brief 更新要素（增量）
     */
    void Update(int64_t fid, 
                const CNEnvelope& old_envelope,
                const CNEnvelope& new_envelope) {
        Remove(fid, old_envelope);
        Insert(fid, new_envelope);
    }
};

/**
 * @brief 增量R树索引
 */
class CNIncrementalRTree : public CNIncrementalSpatialIndex {
public:
    void Insert(int64_t fid, const CNEnvelope& envelope) override {
        std::lock_guard<std::mutex> lock(mutex_);
        rtree_.insert(std::make_pair(envelope, fid));
    }
    
    void Remove(int64_t fid, const CNEnvelope& envelope) override {
        std::lock_guard<std::mutex> lock(mutex_);
        rtree_.remove(std::make_pair(envelope, fid));
    }
    
    std::vector<int64_t> Query(const CNEnvelope& envelope) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<int64_t> result;
        auto it = rtree_.qbegin(bgi::intersects(envelope));
        while (it != rtree_.qend()) {
            result.push_back(it->second);
            ++it;
        }
        return result;
    }
    
private:
    mutable std::mutex mutex_;
    bgi::rtree<std::pair<CNEnvelope, int64_t>, bgi::quadratic<16>> rtree_;
};

/**
 * @brief 内存图层增量索引支持
 */
class CNMemoryLayer {
public:
    void SetIncrementalIndex(bool enabled) {
        incremental_index_enabled_ = enabled;
        if (enabled && !spatial_index_) {
            spatial_index_ = std::make_unique<CNIncrementalRTree>();
        }
    }
    
    CNStatus CreateFeature(CNFeature* feature) override {
        // ... 原有逻辑 ...
        
        // 增量更新索引
        if (incremental_index_enabled_ && spatial_index_) {
            auto* idx = dynamic_cast<CNIncrementalSpatialIndex*>(spatial_index_.get());
            if (idx) {
                CNEnvelope env;
                feature->GetGeometry()->GetEnvelope(env);
                idx->Insert(fid, env);
            }
        } else {
            index_dirty_ = true;
        }
        
        return CNStatus::kSuccess;
    }
    
    CNStatus DeleteFeature(int64_t fid) override {
        // 获取旧范围
        CNEnvelope old_envelope;
        if (incremental_index_enabled_ && spatial_index_) {
            auto* f = GetFeatureRef(fid);
            if (f && f->GetGeometry()) {
                f->GetGeometry()->GetEnvelope(old_envelope);
            }
        }
        
        // ... 原有删除逻辑 ...
        
        // 增量更新索引
        if (incremental_index_enabled_ && spatial_index_) {
            auto* idx = dynamic_cast<CNIncrementalSpatialIndex*>(spatial_index_.get());
            if (idx) {
                idx->Remove(fid, old_envelope);
            }
        } else {
            index_dirty_ = true;
        }
        
        return CNStatus::kSuccess;
    }
    
private:
    bool incremental_index_enabled_ = false;
};

} // namespace OGC
```

---

## 20. 错误处理策略 *(新增)*

### 20.1 异常安全保证

```cpp
namespace OGC {

/**
 * @brief 异常安全级别
 */
enum class CNExceptionSafety {
    kNoGuarantee,      // 无保证
    kBasic,            // 基本保证：不泄漏资源，对象处于有效状态
    kStrong,           // 强保证：操作成功或回滚
    kNoThrow           // 不抛出保证
};

/**
 * @brief 异常类
 */
class CNLayerException : public std::runtime_error {
public:
    explicit CNLayerException(CNStatus code, const std::string& message = "")
        : std::runtime_error(message), code_(code) {}
    
    CNStatus GetStatusCode() const noexcept { return code_; }
    
private:
    CNStatus code_;
};

/**
 * @brief Result 类型（用于不使用异常的场景）
 */
template<typename T>
class CNResult {
public:
    CNResult(T value) : value_(std::move(value)), status_(CNStatus::kSuccess) {}
    CNResult(CNStatus status) : status_(status) {}
    
    bool IsSuccess() const { return status_ == CNStatus::kSuccess; }
    CNStatus GetStatus() const { return status_; }
    
    T& GetValue() & { 
        if (!IsSuccess()) throw CNLayerException(status_);
        return value_; 
    }
    
    T&& GetValue() && { 
        if (!IsSuccess()) throw CNLayerException(status_);
        return std::move(value_); 
    }
    
    const T& GetValue() const& { 
        if (!IsSuccess()) throw CNLayerException(status_);
        return value_; 
    }
    
    T GetValueOrDefault(T default_value) const {
        return IsSuccess() ? value_ : default_value;
    }
    
private:
    T value_;
    CNStatus status_;
};

/**
 * @brief 异常安全操作包装器
 */
template<typename Func>
auto CNNoThrowWrapper(Func&& func, CNStatus default_error = CNStatus::kError)
    -> CNResult<decltype(func())> {
    try {
        return func();
    } catch (const std::exception& e) {
        return default_error;
    } catch (...) {
        return default_error;
    }
}

} // namespace OGC
```

### 20.2 跨模块错误处理

```cpp
namespace OGC {

/**
 * @brief 错误上下文
 */
struct CNErrorContext {
    std::string file;
    int line;
    std::string function;
    std::string message;
    CNStatus status;
    std::exception_ptr inner_exception;
};

/**
 * @brief 错误处理器接口
 */
class CNErrorHandler {
public:
    virtual ~CNErrorHandler() = default;
    virtual void Handle(const CNErrorContext& context) = 0;
};

/**
 * @brief 全局错误管理器
 */
class CNErrorManager {
public:
    static CNErrorManager& Instance();
    
    void SetHandler(std::shared_ptr<CNErrorHandler> handler) {
        handler_ = handler;
    }
    
    void ReportError(const CNErrorContext& context) {
        if (handler_) {
            handler_->Handle(context);
        }
    }
    
    CNStatus GetLastError() const { return last_error_; }
    std::string GetLastErrorString() const { return last_error_string_; }
    
private:
    std::shared_ptr<CNErrorHandler> handler_;
    CNStatus last_error_ = CNStatus::kSuccess;
    std::string last_error_string_;
};

// 错误报告宏
#define CN_REPORT_ERROR(status, message) \
    OGC::CNErrorManager::Instance().ReportError({ \
        __FILE__, __LINE__, __func__, message, status, nullptr \
    })

#define CN_THROW(status, message) \
    throw OGC::CNLayerException(status, message)

} // namespace OGC
```

---

## 21. 项目里程碑规划 *(新增)*

### 21.1 开发阶段划分

| 里程碑 | 阶段名称 | 内容 | 预估工作量 | 依赖 | 交付物 |
|--------|----------|------|------------|------|--------|
| **M1** | 核心框架 | CNLayer基类、CNMemoryLayer、CNFeatureDefn、基础测试 | 4周 | - | 可编译的库、单元测试 |
| **M2** | 数据源抽象 | CNDataSource、CNDriver、CNDriverManager | 2周 | M1 | 驱动注册机制 |
| **M3** | 矢量格式支持 | Shapefile读写、GeoJSON读写 | 4周 | M2 | 文件格式支持 |
| **M4** | 数据库支持 | GeoPackage、SpatiaLite、PostGIS（含连接池） | 6周 | M2 | 数据库连接支持 |
| **M5** | 空间索引 | R树、四叉树、增量更新 | 3周 | M1 | 空间查询优化 |
| **M6** | 线程安全完善 | 读写锁、快照、分段锁缓存 | 2周 | M1, M5 | 并发测试通过 |
| **M7** | GDAL互操作 | CNGDALAdapter、格式转换器 | 2周 | M2, M3 | GDAL集成 |
| **M8** | 栅格支持 | CNRasterDataset、GeoTIFF | 4周 | M2 | 栅格数据支持 |
| **M9** | 性能优化 | 对象池、增量事务、SIMD | 3周 | M5, M6 | 性能基准测试 |
| **M10** | 文档与示例 | API文档、使用示例、最佳实践 | 2周 | M1-M9 | 完整文档 |

**总工作量**: 约32周（约8个月）

### 21.2 工作量评估

| 模块 | 代码行数估算 | 测试代码估算 | 复杂度 | 风险等级 |
|------|-------------|-------------|--------|----------|
| CNLayer基类 | 500 | 300 | 中 | 低 |
| CNMemoryLayer | 1500 | 800 | 高 | 中 |
| CNDataSource | 800 | 400 | 中 | 低 |
| CNDriverManager | 400 | 200 | 低 | 低 |
| Shapefile驱动 | 2000 | 600 | 中 | 低 |
| GeoPackage驱动 | 2500 | 800 | 高 | 中 |
| PostGIS驱动 | 4000 | 1200 | 高 | 高 |
| 连接池 | 800 | 400 | 中 | 中 |
| 空间索引 | 2000 | 1000 | 高 | 中 |
| 线程安全组件 | 1000 | 600 | 高 | 高 |
| GDAL适配器 | 1500 | 500 | 中 | 低 |
| 栅格数据集 | 2000 | 800 | 高 | 中 |

**风险评估**:

| 风险 | 可能性 | 影响 | 缓解措施 |
|------|--------|------|----------|
| PostGIS协议变更 | 中 | 高 | 使用libpq官方API、连接池隔离 |
| 线程死锁 | 中 | 高 | 代码审查、压力测试、死锁检测工具 |
| 内存泄漏 | 低 | 高 | RAII、智能指针、内存检测工具 |
| 性能不达标 | 中 | 中 | 性能基准测试、持续优化 |
| 跨平台兼容性 | 低 | 中 | CI/CD多平台测试 |
| Boost版本兼容 | 低 | 中 | 锁定Boost最低版本1.71 |

---

## 22. 连接池设计 *(新增)*

### 22.1 CNConnectionPool 接口

```cpp
namespace OGC {

/**
 * @brief 数据库连接池接口
 * 
 * 管理数据库连接的生命周期
 * 支持连接复用、超时回收、健康检查
 */
class CNConnectionPool {
public:
    virtual ~CNConnectionPool() = default;
    
    /**
     * @brief 获取连接
     * @param timeout_ms 超时时间（毫秒），-1表示无限等待
     * @return 连接指针，超时返回nullptr
     */
    virtual std::shared_ptr<CNDbConnection> Acquire(int timeout_ms = 5000) = 0;
    
    /**
     * @brief 归还连接
     * @param conn 连接指针
     */
    virtual void Release(std::shared_ptr<CNDbConnection> conn) = 0;
    
    /**
     * @brief 获取池大小
     */
    virtual size_t GetSize() const = 0;
    
    /**
     * @brief 获取空闲连接数
     */
    virtual size_t GetIdleCount() const = 0;
    
    /**
     * @brief 获取活跃连接数
     */
    virtual size_t GetActiveCount() const = 0;
    
    /**
     * @brief 健康检查
     * @return 健康连接数
     */
    virtual size_t HealthCheck() = 0;
    
    /**
     * @brief 清空连接池
     */
    virtual void Clear() = 0;
    
    /**
     * @brief 关闭连接池
     */
    virtual void Close() = 0;
};

/**
 * @brief 连接池配置
 */
struct CNConnectionPoolConfig {
    size_t min_size = 2;            // 最小连接数
    size_t max_size = 10;           // 最大连接数
    int idle_timeout_ms = 300000;   // 空闲超时（5分钟）
    int connection_timeout_ms = 5000; // 获取连接超时
    int health_check_interval_ms = 60000; // 健康检查间隔
    bool auto_reconnect = true;     // 自动重连
};

/**
 * @brief PostGIS连接池实现
 */
class CNPostGISConnectionPool : public CNConnectionPool {
public:
    static std::unique_ptr<CNConnectionPool> Create(
        const std::string& connection_string,
        const CNConnectionPoolConfig& config = {});
    
    std::shared_ptr<CNDbConnection> Acquire(int timeout_ms = 5000) override;
    void Release(std::shared_ptr<CNDbConnection> conn) override;
    size_t GetSize() const override;
    size_t GetIdleCount() const override;
    size_t GetActiveCount() const override;
    size_t HealthCheck() override;
    void Clear() override;
    void Close() override;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/**
 * @brief 连接池RAII包装器
 */
class CNConnectionGuard {
public:
    explicit CNConnectionGuard(CNConnectionPool& pool, int timeout_ms = 5000)
        : pool_(pool), conn_(pool_.Acquire(timeout_ms)) {}
    
    ~CNConnectionGuard() {
        if (conn_) {
            pool_.Release(conn_);
        }
    }
    
    CNDbConnection* Get() { return conn_.get(); }
    CNDbConnection* operator->() { return conn_.get(); }
    bool IsValid() const { return conn_ != nullptr; }
    
private:
    CNConnectionPool& pool_;
    std::shared_ptr<CNDbConnection> conn_;
};

} // namespace OGC
```

---

## 23. 进度回调与异步接口 *(新增)*

### 23.1 CNProgressCallback 进度回调

```cpp
namespace OGC {

/**
 * @brief 进度回调接口
 * 
 * 用于长时间操作的进度反馈
 */
class CNProgressCallback {
public:
    virtual ~CNProgressCallback() = default;
    
    /**
     * @brief 进度更新
     * @param progress 进度值 (0.0 - 1.0)
     * @param message 进度消息
     * @return true继续执行，false取消操作
     */
    virtual bool OnProgress(double progress, const std::string& message) = 0;
    
    /**
     * @brief 操作完成
     * @param success 是否成功
     * @param message 完成消息
     */
    virtual void OnComplete(bool success, const std::string& message) {}
};

/**
 * @brief 默认进度回调（控制台输出）
 */
class CNConsoleProgressCallback : public CNProgressCallback {
public:
    bool OnProgress(double progress, const std::string& message) override {
        int bar_width = 50;
        int pos = static_cast<int>(bar_width * progress);
        
        std::cout << "[";
        for (int i = 0; i < bar_width; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << static_cast<int>(progress * 100) << "% " << message << "\r";
        std::cout.flush();
        
        return !cancelled_;
    }
    
    void Cancel() { cancelled_ = true; }
    
private:
    bool cancelled_ = false;
};

/**
 * @brief 支持进度回调的操作示例
 */
class CNMemoryLayer : public CNLayer {
public:
    /**
     * @brief 批量导入要素（带进度）
     */
    int64_t ImportFromLayer(
        CNLayer* source,
        CNProgressCallback* callback = nullptr);
    
    /**
     * @brief 构建空间索引（带进度）
     */
    void BuildSpatialIndex(
        SpatialIndexType type,
        CNProgressCallback* callback = nullptr);
};

} // namespace OGC
```

### 23.2 异步操作接口

```cpp
namespace OGC {

/**
 * @brief 异步操作结果
 */
template<typename T>
class CNAsyncResult {
public:
    CNAsyncResult() : completed_(false) {}
    
    /**
     * @brief 等待完成
     */
    void Wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return completed_; });
    }
    
    /**
     * @brief 等待完成（带超时）
     * @param timeout_ms 超时时间
     * @return true表示完成，false表示超时
     */
    bool WaitFor(int timeout_ms) {
        std::unique_lock<std::mutex> lock(mutex_);
        return cv_.wait_for(lock, std::chrono::milliseconds(timeout_ms),
            [this] { return completed_; });
    }
    
    /**
     * @brief 获取结果
     */
    T GetResult() {
        Wait();
        return result_;
    }
    
    /**
     * @brief 是否完成
     */
    bool IsCompleted() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return completed_;
    }
    
    /**
     * @brief 设置结果
     */
    void SetResult(T result) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            result_ = std::move(result);
            completed_ = true;
        }
        cv_.notify_all();
    }
    
private:
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    T result_;
    bool completed_;
};

/**
 * @brief 异步图层操作
 */
class CNAsyncLayerOps {
public:
    /**
     * @brief 异步读取要素
     */
    static std::shared_ptr<CNAsyncResult<std::vector<std::unique_ptr<CNFeature>>>>
    GetFeaturesAsync(CNLayer* layer, const CNEnvelope& extent) {
        auto result = std::make_shared<CNAsyncResult<std::vector<std::unique_ptr<CNFeature>>>>();
        
        std::thread([layer, extent, result]() {
            std::vector<std::unique_ptr<CNFeature>> features;
            layer->SetSpatialFilterRect(
                extent.GetMinX(), extent.GetMinY(),
                extent.GetMaxX(), extent.GetMaxY());
            layer->ResetReading();
            while (auto f = layer->GetNextFeature()) {
                features.push_back(std::move(f));
            }
            result->SetResult(std::move(features));
        }).detach();
        
        return result;
    }
    
    /**
     * @brief 异步批量写入
     */
    static std::shared_ptr<CNAsyncResult<int64_t>>
    CreateFeaturesAsync(CNLayer* layer, std::vector<std::unique_ptr<CNFeature>> features) {
        auto result = std::make_shared<CNAsyncResult<int64_t>>();
        
        std::thread([layer, features = std::move(features), result]() mutable {
            int64_t count = 0;
            for (auto& f : features) {
                if (layer->CreateFeature(f.get()) == CNStatus::kSuccess) {
                    count++;
                }
            }
            result->SetResult(count);
        }).detach();
        
        return result;
    }
};

} // namespace OGC
```

---

## 24. 属性过滤器规范 *(新增)*

### 24.1 过滤器语法规范

**设计背景**: 不同数据源的属性过滤能力差异大，需要明确支持的语法范围。

#### 24.1.1 基础语法（所有驱动必须支持）

```
<filter> ::= <comparison> | <logical>
<comparison> ::= <field> <op> <value>
<op> ::= "=" | "!=" | "<" | ">" | "<=" | ">=" | "LIKE"
<logical> ::= <filter> "AND" <filter> | <filter> "OR" <filter> | "NOT" <filter>
<value> ::= <string> | <number> | "NULL"
```

**示例**:
```sql
name = 'Beijing'
population > 1000000
name LIKE 'New%' AND population > 500000
```

#### 24.1.2 扩展语法（支持SQL的驱动可选）

```sql
-- IN 子句
name IN ('Beijing', 'Shanghai', 'Guangzhou')

-- BETWEEN
population BETWEEN 1000000 AND 10000000

-- IS NULL / IS NOT NULL
name IS NOT NULL

-- 嵌套逻辑
(name = 'Beijing' OR name = 'Shanghai') AND population > 1000000
```

### 24.2 内存图层表达式引擎

```cpp
namespace OGC {

/**
 * @brief 表达式节点类型
 */
enum class CNExprType {
    kLiteral,       // 字面量
    kField,         // 字段引用
    kComparison,    // 比较运算
    kLogical,       // 逻辑运算
    kFunction       // 函数调用
};

/**
 * @brief 表达式节点
 */
struct CNExprNode {
    CNExprType type;
    std::string value;
    std::vector<std::unique_ptr<CNExprNode>> children;
    
    /**
     * @brief 计算表达式
     * @param feature 要素上下文
     * @return 表达式结果
     */
    bool Evaluate(const CNFeature* feature) const;
};

/**
 * @brief 表达式解析器
 */
class CNExprParser {
public:
    /**
     * @brief 解析过滤表达式
     * @param expr 表达式字符串
     * @return 表达式树根节点
     */
    static std::unique_ptr<CNExprNode> Parse(const std::string& expr);
    
    /**
     * @brief 验证表达式语法
     */
    static bool Validate(const std::string& expr, std::string& error);
};

/**
 * @brief 内存图层属性过滤实现
 */
class CNMemoryLayer {
public:
    CNStatus SetAttributeFilter(const std::string& query) override {
        if (query.empty()) {
            attr_filter_.reset();
            return CNStatus::kSuccess;
        }
        
        std::string error;
        if (!CNExprParser::Validate(query, error)) {
            return CNStatus::kInvalidParameter;
        }
        
        attr_filter_ = CNExprParser::Parse(query);
        return attr_filter_ ? CNStatus::kSuccess : CNStatus::kError;
    }
    
private:
    std::unique_ptr<CNExprNode> attr_filter_;
};

} // namespace OGC
```

### 24.3 各驱动支持情况

| 驱动 | 基础语法 | 扩展语法 | 实现方式 |
|------|----------|----------|----------|
| CNMemoryLayer | ✅ | ❌ | 内置表达式引擎 |
| CNShapefileLayer | ✅ | ❌ | 内置表达式引擎 |
| CNGeoPackageLayer | ✅ | ✅ | SQLite引擎 |
| CNPostGISLayer | ✅ | ✅ | PostgreSQL引擎 |
| CNGeoJSONLayer | ✅ | ❌ | 内置表达式引擎 |

---

## 25. 内存压力测试 *(新增)*

### 25.1 内存估算公式

```
内存占用 ≈ 基础开销 + 要素数量 × 单要素开销

其中:
- 基础开销 ≈ 1MB（图层元数据、索引结构）
- 单要素开销 = 几何大小 + 属性大小 + 管理开销
- 管理开销 ≈ 64字节（FID索引、指针等）

几何大小估算:
- Point: 16字节（x, y）
- LineString: 16 × 顶点数
- Polygon: 16 × 顶点数
- MultiPolygon: 较复杂，建议实测

属性大小估算:
- Integer: 4字节
- Integer64: 8字节
- Double: 8字节
- String: 字符串长度 + 8字节
```

### 25.2 内存压力测试场景

```cpp
namespace OGC {

/**
 * @brief 内存压力测试
 */
class CNMemoryStressTest {
public:
    /**
     * @brief 测试场景: 100万点要素
     */
    static void TestMillionPoints() {
        CNMemoryLayer layer("points", GeomType::kPoint);
        
        CNFieldDefn id_field("id", CNFieldType::kInteger64);
        layer.CreateField(&id_field);
        
        CNFieldDefn name_field("name", CNFieldType::kString);
        name_field.SetWidth(50);
        layer.CreateField(&name_field);
        
        // 预分配
        layer.Reserve(1000000);
        
        // 预期内存: ~1M × (16 + 8 + 50 + 64) ≈ 138MB
        
        for (int64_t i = 0; i < 1000000; ++i) {
            CNFeature feature(layer.GetFeatureDefn());
            feature.SetGeometry(CNPoint(i * 0.001, i * 0.001));
            feature.SetField("id", i);
            feature.SetField("name", "Feature_" + std::to_string(i));
            layer.CreateFeature(&feature);
        }
        
        std::cout << "Feature count: " << layer.GetFeatureCount() << std::endl;
        std::cout << "Memory usage: " << layer.GetMemoryUsage() / 1024 / 1024 << " MB" << std::endl;
    }
    
    /**
     * @brief 测试场景: 10万复杂多边形
     */
    static void TestComplexPolygons() {
        CNMemoryLayer layer("polygons", GeomType::kPolygon);
        
        // 每个多边形约100个顶点
        // 预期内存: ~100K × (16 × 100 + 64) ≈ 166MB
        
        for (int64_t i = 0; i < 100000; ++i) {
            CNFeature feature(layer.GetFeatureDefn());
            
            // 创建复杂多边形
            std::vector<CNPoint> ring;
            for (int j = 0; j < 100; ++j) {
                double angle = j * 2 * M_PI / 100;
                ring.emplace_back(cos(angle) * 100 + i, sin(angle) * 100 + i);
            }
            ring.push_back(ring[0]); // 闭合
            
            feature.SetGeometry(CNPolygon(ring));
            layer.CreateFeature(&feature);
        }
        
        std::cout << "Feature count: " << layer.GetFeatureCount() << std::endl;
        std::cout << "Memory usage: " << layer.GetMemoryUsage() / 1024 / 1024 << " MB" << std::endl;
    }
    
    /**
     * @brief 测试场景: 事务内存开销
     */
    static void TestTransactionMemory() {
        CNMemoryLayer layer("test", GeomType::kPoint);
        
        // 添加初始数据
        for (int i = 0; i < 100000; ++i) {
            CNFeature feature(layer.GetFeatureDefn());
            feature.SetGeometry(CNPoint(i, i));
            layer.CreateFeature(&feature);
        }
        
        int64_t base_memory = layer.GetMemoryUsage();
        
        // 开始事务
        layer.StartTransaction();
        
        // 修改10%的数据
        layer.ResetReading();
        int count = 0;
        while (auto f = layer.GetNextFeature()) {
            if (count++ % 10 == 0) {
                f->SetGeometry(CNPoint(f->GetFID() * 2, f->GetFID() * 2));
                layer.SetFeature(f.get());
            }
        }
        
        int64_t transaction_memory = layer.GetMemoryUsage();
        
        std::cout << "Base memory: " << base_memory / 1024 / 1024 << " MB" << std::endl;
        std::cout << "Transaction memory: " << transaction_memory / 1024 / 1024 << " MB" << std::endl;
        std::cout << "Overhead: " << (transaction_memory - base_memory) / 1024 / 1024 << " MB" << std::endl;
        
        layer.RollbackTransaction();
    }
};

} // namespace OGC
```

---

## 26. 错误处理策略统一 *(新增)*

### 26.1 错误处理规范

**设计原则**: 明确区分返回码和异常的使用场景。

| 场景 | 错误处理方式 | 说明 |
|------|-------------|------|
| 构造函数 | 抛出异常 | 构造失败无法返回状态码 |
| 析构函数 | 不抛异常 | 析构必须成功，错误记录日志 |
| 业务操作 | 返回CNStatus | 调用者可决定如何处理 |
| 资源获取失败 | 抛出异常 | 内存、文件句柄等关键资源 |
| 参数校验失败 | 返回CNStatus | 可预期的错误 |
| 内部不变量破坏 | 抛出异常 | 程序错误，不应继续执行 |

### 26.2 统一错误处理示例

```cpp
namespace OGC {

/**
 * @brief 业务操作统一返回状态码
 */
class CNMemoryLayer : public CNLayer {
public:
    CNStatus CreateFeature(CNFeature* feature) override {
        // 参数校验 - 返回状态码
        if (!feature) {
            return CNStatus::kNullPointer;
        }
        
        // 业务逻辑错误 - 返回状态码
        if (feature->GetGeometry() == nullptr && GetGeomType() != GeomType::kUnknown) {
            return CNStatus::kInvalidGeometry;
        }
        
        // 资源分配 - 可能抛异常，由调用者捕获
        try {
            features_.push_back(feature->Clone());
        } catch (const std::bad_alloc&) {
            return CNStatus::kOutOfMemory;
        }
        
        return CNStatus::kSuccess;
    }
};

/**
 * @brief 构造函数抛出异常
 */
class CNPostGISConnection {
public:
    CNPostGISConnection(const std::string& conn_str) {
        conn_ = PQconnectdb(conn_str.c_str());
        if (PQstatus(conn_) != CONNECTION_OK) {
            std::string error = PQerrorMessage(conn_);
            PQfinish(conn_);
            throw CNLayerException(CNStatus::kIOError, 
                "Failed to connect: " + error);
        }
    }
    
    ~CNPostGISConnection() noexcept {
        if (conn_) {
            PQfinish(conn_);  // 析构不抛异常
        }
    }
};

} // namespace OGC
```

---

## 27. CNLayer接口拆分设计 *(新增)*

### 27.1 接口隔离原则应用

**设计背景**: 原CNLayer接口包含40+个虚函数，违反接口隔离原则(ISP)，客户端被迫依赖不需要的方法。

**问题分析**:
- 只读客户端需要实现CreateFeature等写入方法
- 不支持事务的图层需要实现StartTransaction等方法
- 接口过于庞大，增加实现负担

### 27.2 角色接口定义

```cpp
namespace OGC {

/**
 * @brief 只读图层接口
 * 
 * 最小化接口，仅包含读取操作
 */
class CNReadOnlyLayer {
public:
    virtual ~CNReadOnlyLayer() = default;
    
    virtual const std::string& GetName() const = 0;
    virtual CNLayerType GetLayerType() const = 0;
    virtual CNFeatureDefn* GetFeatureDefn() = 0;
    virtual const CNFeatureDefn* GetFeatureDefn() const = 0;
    virtual GeomType GetGeomType() const = 0;
    virtual CNSpatialReference* GetSpatialRef() = 0;
    virtual const CNSpatialReference* GetSpatialRef() const = 0;
    virtual CNStatus GetExtent(CNEnvelope& extent, bool force = true) const = 0;
    virtual int64_t GetFeatureCount(bool force = true) const = 0;
    
    virtual void ResetReading() = 0;
    virtual std::unique_ptr<CNFeature> GetNextFeature() = 0;
    virtual CNFeature* GetNextFeatureRef() = 0;
    virtual std::unique_ptr<CNFeature> GetFeature(int64_t fid) = 0;
    
    virtual void SetSpatialFilter(const CNGeometry* geometry) = 0;
    virtual void SetSpatialFilterRect(double min_x, double min_y,
                                       double max_x, double max_y) = 0;
    virtual const CNGeometry* GetSpatialFilter() const = 0;
    virtual CNStatus SetAttributeFilter(const std::string& query) = 0;
    
    virtual bool TestCapability(CNLayerCapability capability) const = 0;
};

/**
 * @brief 可写图层接口
 */
class CNWritableLayer : public CNReadOnlyLayer {
public:
    virtual CNStatus SetFeature(const CNFeature* feature) {
        return CNStatus::kNotSupported;
    }
    
    virtual CNStatus CreateFeature(CNFeature* feature) {
        return CNStatus::kNotSupported;
    }
    
    virtual CNStatus DeleteFeature(int64_t fid) {
        return CNStatus::kNotSupported;
    }
    
    virtual int64_t CreateFeatureBatch(
        const std::vector<CNFeature*>& features) {
        int64_t count = 0;
        for (auto* f : features) {
            if (CreateFeature(f) == CNStatus::kSuccess) {
                count++;
            }
        }
        return count;
    }
    
    virtual CNStatus CreateField(const CNFieldDefn* field_defn, 
                                  bool approx_ok = false) {
        return CNStatus::kNotSupported;
    }
    
    virtual CNStatus DeleteField(int field_index) {
        return CNStatus::kNotSupported;
    }
};

/**
 * @brief 事务图层接口
 */
class CNTransactionalLayer : public CNWritableLayer {
public:
    virtual CNStatus StartTransaction() {
        return CNStatus::kNotSupported;
    }
    
    virtual CNStatus CommitTransaction() {
        return CNStatus::kNotSupported;
    }
    
    virtual CNStatus RollbackTransaction() {
        return CNStatus::kNotSupported;
    }
};

/**
 * @brief 完整图层接口（向后兼容）
 * 
 * 继承所有角色接口，保持与现有代码兼容
 */
class CNLayer : public CNTransactionalLayer {
public:
    virtual std::unique_ptr<CNLayer> Clone() const = 0;
    virtual CNStatus SyncToDisk() { return CNStatus::kSuccess; }
    virtual std::string GetMetadata(const std::string& key) const { return ""; }
    virtual CNStatus SetMetadata(const std::string& key, 
                                  const std::string& value) {
        return CNStatus::kNotSupported;
    }
};

} // namespace OGC
```

**接口层次结构**:
```
CNReadOnlyLayer (只读操作)
    └── CNWritableLayer (添加写入操作)
        └── CNTransactionalLayer (添加事务操作)
            └── CNLayer (添加克隆、元数据等)
```

---

## 28. 图层事件通知机制 *(新增)*

### 28.1 CNLayerObserver观察者接口

**设计背景**: 渲染层无法感知图层变化，需要轮询或手动刷新，效率低下。

```cpp
namespace OGC {

/**
 * @brief 图层事件类型
 */
enum class CNLayerEventType {
    kFeatureAdded,          // 要素添加
    kFeatureDeleted,        // 要素删除
    kFeatureModified,       // 要素修改
    kSchemaChanged,         // 模式变更（字段增删改）
    kSpatialFilterChanged,  // 空间过滤器变更
    kAttributeFilterChanged,// 属性过滤器变更
    kCleared,               // 图层清空
    kTransactionStarted,    // 事务开始
    kTransactionCommitted,  // 事务提交
    kTransactionRolledBack  // 事务回滚
};

/**
 * @brief 图层事件参数
 */
struct CNLayerEventArgs {
    CNLayerEventType type;
    int64_t fid;                    // 相关FID（-1表示不适用）
    int field_index;                // 相关字段索引（-1表示不适用）
    std::string message;            // 附加消息
    const CNFeature* feature;       // 相关要素（可能为nullptr）
};

/**
 * @brief 图层观察者接口
 */
class CNLayerObserver {
public:
    virtual ~CNLayerObserver() = default;
    
    /**
     * @brief 图层变更通知
     * @param layer 发生变更的图层
     * @param args 事件参数
     */
    virtual void OnLayerChanged(CNLayer* layer, 
                                 const CNLayerEventArgs& args) = 0;
    
    /**
     * @brief 图层即将变更通知
     * @param layer 即将变更的图层
     * @param args 事件参数
     * @return true允许变更，false取消变更
     */
    virtual bool OnLayerChanging(CNLayer* layer,
                                  const CNLayerEventArgs& args) {
        (void)layer;
        (void)args;
        return true;
    }
};

/**
 * @brief 可观察图层基类
 */
class CNObservableLayer : public CNLayer {
public:
    /**
     * @brief 添加观察者
     */
    void AddObserver(CNLayerObserver* observer) {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        observers_.insert(observer);
    }
    
    /**
     * @brief 移除观察者
     */
    void RemoveObserver(CNLayerObserver* observer) {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        observers_.erase(observer);
    }
    
    /**
     * @brief 清除所有观察者
     */
    void ClearObservers() {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        observers_.clear();
    }
    
protected:
    /**
     * @brief 通知观察者（变更后）
     */
    void NotifyObservers(CNLayerEventType type, 
                         int64_t fid = -1,
                         int field_index = -1,
                         const std::string& message = "",
                         const CNFeature* feature = nullptr) {
        CNLayerEventArgs args{type, fid, field_index, message, feature};
        
        std::lock_guard<std::mutex> lock(observers_mutex_);
        for (auto* observer : observers_) {
            observer->OnLayerChanged(this, args);
        }
    }
    
    /**
     * @brief 通知观察者（变更前）
     */
    bool NotifyObserversChanging(CNLayerEventType type,
                                  int64_t fid = -1,
                                  int field_index = -1,
                                  const std::string& message = "",
                                  const CNFeature* feature = nullptr) {
        CNLayerEventArgs args{type, fid, field_index, message, feature};
        
        std::lock_guard<std::mutex> lock(observers_mutex_);
        for (auto* observer : observers_) {
            if (!observer->OnLayerChanging(this, args)) {
                return false;
            }
        }
        return true;
    }
    
private:
    std::set<CNLayerObserver*> observers_;
    mutable std::mutex observers_mutex_;
};

} // namespace OGC
```

### 28.2 事件类型定义

| 事件类型 | 触发时机 | 典型用途 |
|----------|----------|----------|
| kFeatureAdded | CreateFeature成功后 | 更新渲染缓存 |
| kFeatureDeleted | DeleteFeature成功后 | 移除渲染元素 |
| kFeatureModified | SetFeature成功后 | 重绘要素 |
| kSchemaChanged | CreateField/DeleteField后 | 更新属性表UI |
| kSpatialFilterChanged | SetSpatialFilter后 | 重新查询渲染 |
| kCleared | Clear()后 | 清空渲染 |

---

## 29. 引用迭代与流式读取 *(新增)*

### 29.1 GetNextFeatureRef引用迭代

**设计背景**: GetNextFeature返回unique_ptr，每次迭代涉及内存分配/释放，大数据集性能开销大。

```cpp
namespace OGC {

/**
 * @brief 引用迭代接口
 * 
 * 返回要素引用而非拷贝，避免内存分配开销
 * 注意：返回的指针仅在下次迭代前有效
 */
class CNReadOnlyLayer {
public:
    /**
     * @brief 获取下一个要素（引用，不转移所有权）
     * @return 要素指针，调用者不拥有所有权；nullptr表示结束
     * @note 返回的指针仅在下次调用GetNextFeature/GetNextFeatureRef前有效
     */
    virtual CNFeature* GetNextFeatureRef() {
        return nullptr;
    }
    
    /**
     * @brief 获取当前迭代位置的要素引用
     * @return 要素指针，nullptr表示无效位置
     */
    virtual CNFeature* GetCurrentFeatureRef() {
        return nullptr;
    }
};

/**
 * @brief 内存图层引用迭代实现
 */
class CNMemoryLayer : public CNObservableLayer {
public:
    CNFeature* GetNextFeatureRef() override {
        ApplySpatialFilter();
        
        while (read_cursor_ < filtered_indices_.size()) {
            size_t idx = filtered_indices_[read_cursor_++];
            CNFeature* feature = features_[idx].get();
            
            if (PassesFilters(feature)) {
                current_feature_ = feature;
                return feature;
            }
        }
        
        current_feature_ = nullptr;
        return nullptr;
    }
    
    CNFeature* GetCurrentFeatureRef() override {
        return current_feature_;
    }
    
    std::unique_ptr<CNFeature> GetNextFeature() override {
        CNFeature* ref = GetNextFeatureRef();
        return ref ? ref->Clone() : nullptr;
    }
    
private:
    CNFeature* current_feature_ = nullptr;
    std::vector<size_t> filtered_indices_;
};

} // namespace OGC
```

**使用示例**:
```cpp
// 高性能迭代（零拷贝）
layer->ResetReading();
while (CNFeature* f = layer->GetNextFeatureRef()) {
    ProcessFeature(*f);  // 直接使用引用
}

// 需要持有要素时使用unique_ptr
layer->ResetReading();
while (auto f = layer->GetNextFeature()) {
    features.push_back(std::move(f));  // 转移所有权
}
```

### 29.2 CNFeatureStream流式接口

**设计背景**: 大数据集无法流式处理，内存压力大。

```cpp
namespace OGC {

/**
 * @brief 要素流式读取接口
 * 
 * 支持大数据集流式处理，避免一次性加载全部要素
 */
class CNFeatureStream {
public:
    virtual ~CNFeatureStream() = default;
    
    /**
     * @brief 读取下一批要素
     * @param batch_size 批次大小
     * @return 要素列表
     */
    virtual std::vector<std::unique_ptr<CNFeature>> ReadNextBatch(
        size_t batch_size = 1000) = 0;
    
    /**
     * @brief 是否到达末尾
     */
    virtual bool IsEndOfStream() const = 0;
    
    /**
     * @brief 获取已读取数量
     */
    virtual int64_t GetReadCount() const = 0;
    
    /**
     * @brief 重置流
     */
    virtual void Reset() = 0;
    
    /**
     * @brief 关闭流
     */
    virtual void Close() = 0;
};

/**
 * @brief 图层流式读取扩展
 */
class CNLayerStreamExtensions {
public:
    /**
     * @brief 创建要素流
     */
    static std::unique_ptr<CNFeatureStream> CreateStream(
        CNLayer* layer,
        size_t batch_size = 1000) {
        return std::make_unique<CNLayerFeatureStream>(layer, batch_size);
    }
    
    /**
     * @brief 流式处理要素
     */
    template<typename Processor>
    static void ProcessStream(CNLayer* layer, 
                               Processor processor,
                               size_t batch_size = 1000) {
        auto stream = CreateStream(layer, batch_size);
        while (!stream->IsEndOfStream()) {
            auto batch = stream->ReadNextBatch(batch_size);
            for (const auto& feature : batch) {
                processor(*feature);
            }
        }
    }
    
    /**
     * @brief 流式导出
     */
    static int64_t ExportToLayer(CNLayer* src, 
                                  CNLayer* dst,
                                  size_t batch_size = 1000) {
        int64_t count = 0;
        auto stream = CreateStream(src, batch_size);
        while (!stream->IsEndOfStream()) {
            auto batch = stream->ReadNextBatch(batch_size);
            for (auto& feature : batch) {
                if (dst->CreateFeature(feature.get()) == CNStatus::kSuccess) {
                    count++;
                }
            }
        }
        return count;
    }
};

/**
 * @brief 图层要素流实现
 */
class CNLayerFeatureStream : public CNFeatureStream {
public:
    CNLayerFeatureStream(CNLayer* layer, size_t batch_size)
        : layer_(layer), batch_size_(batch_size) {
        layer_->ResetReading();
    }
    
    std::vector<std::unique_ptr<CNFeature>> ReadNextBatch(
        size_t batch_size) override {
        std::vector<std::unique_ptr<CNFeature>> batch;
        batch.reserve(batch_size > 0 ? batch_size : batch_size_);
        
        size_t count = 0;
        size_t target = batch_size > 0 ? batch_size : batch_size_;
        
        while (count < target) {
            auto feature = layer_->GetNextFeature();
            if (!feature) {
                end_of_stream_ = true;
                break;
            }
            batch.push_back(std::move(feature));
            count++;
            read_count_++;
        }
        
        return batch;
    }
    
    bool IsEndOfStream() const override { return end_of_stream_; }
    int64_t GetReadCount() const override { return read_count_; }
    
    void Reset() override {
        layer_->ResetReading();
        read_count_ = 0;
        end_of_stream_ = false;
    }
    
    void Close() override {
        layer_ = nullptr;
    }
    
private:
    CNLayer* layer_;
    size_t batch_size_;
    int64_t read_count_ = 0;
    bool end_of_stream_ = false;
};

} // namespace OGC
```

---

## 30. WFS客户端设计 *(新增)*

### 30.1 CNWFSLayer实现

**设计背景**: 目录列出CNWFSLayer但无详细设计，缺少OGC WFS服务访问能力。

```cpp
namespace OGC {

/**
 * @brief WFS版本枚举
 */
enum class CNWFSVersion {
    k1_0_0,
    k1_1_0,
    k2_0_0
};

/**
 * @brief WFS连接参数
 */
struct CNWFSConnectionParams {
    std::string url;                    // WFS服务URL
    CNWFSVersion version = CNWFSVersion::k2_0_0;
    std::string username;               // 用户名（可选）
    std::string password;               // 密码（可选）
    std::string api_key;                // API密钥（可选）
    int timeout_ms = 30000;             // 请求超时
    int max_features = 10000;           // 单次请求最大要素数
    bool use_gml = false;               // 使用GML格式（默认GeoJSON）
    std::string srs_name;               // 输出坐标系（可选）
};

/**
 * @brief WFS图层实现
 */
class CNWFSLayer : public CNReadOnlyLayer {
public:
    /**
     * @brief 连接WFS服务
     */
    static std::unique_ptr<CNWFSLayer> Connect(
        const CNWFSConnectionParams& params,
        const std::string& type_name);
    
    /**
     * @brief 获取WFS能力文档
     */
    static std::string GetCapabilities(const std::string& url);
    
    /**
     * @brief 列出可用图层
     */
    static std::vector<std::string> ListLayers(const std::string& url);
    
    // ========== CNReadOnlyLayer接口实现 ==========
    
    const std::string& GetName() const override { return type_name_; }
    CNLayerType GetLayerType() const override { return CNLayerType::kWFS; }
    CNFeatureDefn* GetFeatureDefn() override;
    const CNFeatureDefn* GetFeatureDefn() const override;
    GeomType GetGeomType() const override;
    CNSpatialReference* GetSpatialRef() override;
    const CNSpatialReference* GetSpatialRef() const override;
    CNStatus GetExtent(CNEnvelope& extent, bool force = true) const override;
    int64_t GetFeatureCount(bool force = true) const override;
    
    void ResetReading() override;
    std::unique_ptr<CNFeature> GetNextFeature() override;
    CNFeature* GetNextFeatureRef() override;
    std::unique_ptr<CNFeature> GetFeature(int64_t fid) override;
    
    void SetSpatialFilter(const CNGeometry* geometry) override;
    void SetSpatialFilterRect(double min_x, double min_y,
                               double max_x, double max_y) override;
    const CNGeometry* GetSpatialFilter() const override;
    CNStatus SetAttributeFilter(const std::string& query) override;
    
    bool TestCapability(CNLayerCapability capability) const override;
    
    // ========== WFS特有方法 ==========
    
    /**
     * @brief 获取WFS服务URL
     */
    std::string GetServiceURL() const { return params_.url; }
    
    /**
     * @brief 获取类型名称
     */
    std::string GetTypeName() const { return type_name_; }
    
    /**
     * @brief 设置BBOX坐标系
     */
    void SetBBOXCRS(const std::string& crs);
    
    /**
     * @brief 设置排序
     */
    void SetSortBy(const std::string& property, bool ascending = true);
    
    /**
     * @brief 设置输出格式
     */
    void SetOutputFormat(const std::string& format);
    
    /**
     * @brief 预加载所有要素到内存
     */
    CNStatus PreloadAll();
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
    
    CNWFSConnectionParams params_;
    std::string type_name_;
};

} // namespace OGC
```

### 30.2 WFS请求构建

```cpp
namespace OGC {

/**
 * @brief WFS请求构建器
 */
class CNWFSRequestBuilder {
public:
    explicit CNWFSRequestBuilder(const std::string& base_url,
                                  CNWFSVersion version);
    
    /**
     * @brief 构建GetCapabilities请求
     */
    std::string BuildGetCapabilities() const;
    
    /**
     * @brief 构建DescribeFeatureType请求
     */
    std::string BuildDescribeFeatureType(const std::string& type_name) const;
    
    /**
     * @brief 构建GetFeature请求
     */
    std::string BuildGetFeature(
        const std::string& type_name,
        const CNEnvelope* bbox = nullptr,
        const std::string& filter = "",
        int max_features = -1,
        int start_index = 0) const;
    
    /**
     * @brief 设置输出格式
     */
    void SetOutputFormat(const std::string& format);
    
    /**
     * @brief 设置BBOX CRS
     */
    void SetBBOXCRS(const std::string& crs);
    
private:
    std::string BuildFilter(const std::string& attribute_filter,
                            const CNEnvelope* bbox) const;
    
    std::string base_url_;
    CNWFSVersion version_;
    std::string output_format_;
    std::string bbox_crs_;
};

/**
 * @brief WFS响应解析器
 */
class CNWFSResponseParser {
public:
    /**
     * @brief 解析GetCapabilities响应
     */
    static std::vector<std::pair<std::string, std::string>> 
    ParseCapabilities(const std::string& xml);
    
    /**
     * @brief 解析DescribeFeatureType响应
     */
    static std::unique_ptr<CNFeatureDefn>
    ParseFeatureType(const std::string& xml, const std::string& type_name);
    
    /**
     * @brief 解析GetFeature响应（GML）
     */
    static std::vector<std::unique_ptr<CNFeature>>
    ParseGMLFeatures(const std::string& xml, CNFeatureDefn* defn);
    
    /**
     * @brief 解析GetFeature响应（GeoJSON）
     */
    static std::vector<std::unique_ptr<CNFeature>>
    ParseGeoJSONFeatures(const std::string& json, CNFeatureDefn* defn);
};

} // namespace OGC
```

---

## 31. CNLayerGroup图层组设计 *(新增)*

### 31.1 图层组接口定义

**设计背景**: 目录列出CNLayerGroup但无详细设计，缺少图层组织管理能力。

```cpp
namespace OGC {

/**
 * @brief 图层组节点类型
 */
enum class CNLayerNodeType {
    kLayer,     // 叶子节点：图层
    kGroup      // 分支节点：图层组
};

/**
 * @brief 图层树节点基类
 */
class CNLayerNode {
public:
    virtual ~CNLayerNode() = default;
    
    virtual CNLayerNodeType GetNodeType() const = 0;
    virtual const std::string& GetName() const = 0;
    virtual void SetName(const std::string& name) = 0;
    
    virtual bool IsVisible() const = 0;
    virtual void SetVisible(bool visible) = 0;
    
    virtual CNLayerNode* GetParent() = 0;
    virtual const CNLayerNode* GetParent() const = 0;
    virtual void SetParent(CNLayerNode* parent) = 0;
};

/**
 * @brief 图层组（树形结构）
 */
class CNLayerGroup : public CNLayerNode {
public:
    CNLayerGroup() = default;
    explicit CNLayerGroup(const std::string& name) : name_(name) {}
    
    // ========== CNLayerNode接口 ==========
    
    CNLayerNodeType GetNodeType() const override { 
        return CNLayerNodeType::kGroup; 
    }
    
    const std::string& GetName() const override { return name_; }
    void SetName(const std::string& name) override { name_ = name; }
    
    bool IsVisible() const override { return visible_; }
    void SetVisible(bool visible) override { visible_ = visible; }
    
    CNLayerNode* GetParent() override { return parent_; }
    const CNLayerNode* GetParent() const override { return parent_; }
    void SetParent(CNLayerNode* parent) override { parent_ = parent; }
    
    // ========== 子节点管理 ==========
    
    /**
     * @brief 获取子节点数量
     */
    size_t GetChildCount() const { return children_.size(); }
    
    /**
     * @brief 获取子节点
     */
    CNLayerNode* GetChild(size_t index);
    const CNLayerNode* GetChild(size_t index) const;
    
    /**
     * @brief 添加子节点
     */
    void AddChild(std::unique_ptr<CNLayerNode> child);
    
    /**
     * @brief 插入子节点
     */
    void InsertChild(size_t index, std::unique_ptr<CNLayerNode> child);
    
    /**
     * @brief 移除子节点
     */
    std::unique_ptr<CNLayerNode> RemoveChild(size_t index);
    
    /**
     * @brief 查找子节点
     */
    CNLayerNode* FindChild(const std::string& name);
    
    /**
     * @brief 清空所有子节点
     */
    void Clear();
    
    /**
     * @brief 移动子节点
     */
    void MoveChild(size_t from_index, size_t to_index);
    
    // ========== 图层快捷方法 ==========
    
    /**
     * @brief 添加图层
     */
    void AddLayer(std::unique_ptr<CNLayer> layer);
    
    /**
     * @brief 获取图层
     */
    CNLayer* GetLayer(size_t index);

    /**
     * @brief 获取图层组数量
     */
    int GetGroupCount() const;

    /**
     * @brief 获取图层组
     */
    CNLayerGroup* GetGroup(size_t index);
    const CNLayerGroup* GetGroup(size_t index) const;

    /**
     * @brief 添加图层组
     */
    void AddGroup(std::unique_ptr<CNLayerGroup> group);
    
    /**
     * @brief 获取所有图层（递归）
     */
    std::vector<CNLayer*> GetAllLayers();
    
    /**
     * @brief 获取可见图层
     */
    std::vector<CNLayer*> GetVisibleLayers();
    
    // ========== 迭代器 ==========
    
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = CNLayerNode;
        using difference_type = std::ptrdiff_t;
        using pointer = CNLayerNode*;
        using reference = CNLayerNode&;
        
        Iterator& operator++();
        Iterator operator++(int);
        bool operator==(const Iterator& other) const;
        bool operator!=(const Iterator& other) const;
        reference operator*();
        pointer operator->();
        
    private:
        friend class CNLayerGroup;
        std::vector<CNLayerNode*> stack_;
    };
    
    Iterator begin();
    Iterator end();
    
private:
    std::string name_;
    bool visible_ = true;
    CNLayerNode* parent_ = nullptr;
    std::vector<std::unique_ptr<CNLayerWrapper>> layers_;
    std::vector<std::unique_ptr<CNLayerGroup>> groups_;
};

/**
 * @brief 图层包装节点
 */
class CNLayerWrapper : public CNLayerNode {
public:
    explicit CNLayerWrapper(std::unique_ptr<CNLayer> layer)
        : layer_(std::move(layer)) {}
    
    CNLayerNodeType GetNodeType() const override { 
        return CNLayerNodeType::kLayer; 
    }
    
    const std::string& GetName() const override { 
        return layer_->GetName(); 
    }
    void SetName(const std::string& name) override { 
        (void)name;  // 图层名称由图层自身管理
    }
    
    bool IsVisible() const override { return visible_; }
    void SetVisible(bool visible) override { visible_ = visible; }
    
    CNLayerNode* GetParent() override { return parent_; }
    const CNLayerNode* GetParent() const override { return parent_; }
    void SetParent(CNLayerNode* parent) override { parent_ = parent; }
    
    CNLayer* GetLayer() { return layer_.get(); }
    const CNLayer* GetLayer() const { return layer_.get(); }
    
private:
    std::unique_ptr<CNLayer> layer_;
    bool visible_ = true;
    CNLayerNode* parent_ = nullptr;
};

} // namespace OGC
```

### 31.2 树形结构管理

**设计说明**: CNLayerGroup 内部使用两个独立容器管理子节点：
- `layers_`: 存储图层包装节点 `std::vector<std::unique_ptr<CNLayerWrapper>>`
- `groups_`: 存储子图层组 `std::vector<std::unique_ptr<CNLayerGroup>>`

这种方式避免了 CNLayerNode 到 CNLayer 的类型转换问题，使代码更类型安全。

### 31.3 CNLayerNode、CNLayerWrapper、CNLayer关系详解

**类关系图**:

```
                    ┌─────────────────┐
                    │  CNLayerNode    │  (抽象基类)
                    │  - GetNodeType()│
                    │  - GetName()    │
                    │  - IsVisible()  │
                    │  - GetParent()  │
                    │  - SetParent()  │
                    └────────┬────────┘
                             │
            ┌────────────────┼────────────────┐
            │                │                │
            ▼                ▼                ▼
   ┌────────────────┐ ┌────────────────┐ ┌────────────────┐
   │ CNLayerWrapper │ │ CNLayerGroup   │ │   CNLayer      │
   │ (叶子节点)      │ │ (分支节点)      │ │ (抽象基类)     │
   │ - layer_       │ │ - layers_      │ │ - GetName()    │
   │ - visible_     │ │ - groups_      │ │ - GetExtent()  │
   │ - parent_      │ │ - parent_      │ │ - GetFeature() │
   └───────┬────────┘ └────────────────┘ │ - ...          │
           │                              └───────┬────────┘
           │ 包含                                  │ 继承
           ▼                                      ▼
   ┌────────────────┐                   ┌────────────────┐
   │   CNLayer      │                   │ CNMemoryLayer  │
   │   (被包装)     │                   │ CNVectorLayer  │
   └────────────────┘                   │ CNRasterLayer  │
                                        └────────────────┘
```

**设计原则**:

1. **CNLayerNode（抽象节点基类）**:
   - 定义图层树节点的统一接口
   - 支持两种节点类型：`kLayer`（叶子）和 `kGroup`（分支）
   - 提供父节点指针实现树形结构遍历
   - 提供可见性控制

2. **CNLayerWrapper（图层包装器）**:
   - **设计目的**: 将 CNLayer 包装为 CNLayerNode，使其能加入图层树
   - **为什么需要包装**: CNLayer 是独立的抽象基类，不继承 CNLayerNode
   - **组合模式**: 使用组合而非继承，避免 CNLayer 承担树节点职责
   - **职责分离**: 
     - CNLayer 专注于图层核心功能（要素管理、空间查询等）
     - CNLayerWrapper 专注于树节点功能（父节点、可见性）
   - **优点**:
     - 类型安全：避免 dynamic_cast 的运行时开销
     - 单一职责：每个类职责清晰
     - 灵活性：同一图层可被多个图层组引用（通过不同包装器）

3. **CNLayer（图层抽象基类）**:
   - 定义图层的核心功能接口
   - 不继承 CNLayerNode，保持独立性
   - 可被 CNMemoryLayer、CNVectorLayer、CNRasterLayer 等继承实现

**使用示例**:

```cpp
// 创建图层组
auto root_group = std::make_unique<CNLayerGroup>("Root");

// 创建图层
auto roads_layer = std::make_unique<CNMemoryLayer>("Roads", GeomType::kLineString);

// 方式1：直接添加图层（自动包装为CNLayerWrapper）
root_group->AddLayer(std::move(roads_layer));

// 方式2：添加子图层组
auto background_group = std::make_unique<CNLayerGroup>("Background");
root_group->AddGroup(std::move(background_group));

// 遍历图层树
for (size_t i = 0; i < root_group->GetChildCount(); ++i) {
    CNLayerNode* child = root_group->GetChild(i);
    if (child->GetNodeType() == CNLayerNodeType::kLayer) {
        CNLayerWrapper* wrapper = static_cast<CNLayerWrapper*>(child);
        CNLayer* layer = wrapper->GetLayer();
        // 使用图层...
    }
}

// 直接获取图层
CNLayer* layer = root_group->GetLayer(0);
```

**内存管理**:
- CNLayerGroup 持有 CNLayerWrapper 和 CNLayerGroup 的所有权
- CNLayerWrapper 持有 CNLayer 的所有权
- 删除图层组时，所有子节点自动释放

```
示例结构:
CNLayerGroup("Root")
├── CNLayerGroup("Background")
│   ├── CNLayerWrapper -> WorldMap (CNLayer*)
│   └── CNLayerWrapper -> Ocean (CNLayer*)
├── CNLayerGroup("Infrastructure")
│   ├── CNLayerWrapper -> Roads (CNLayer*)
│   ├── CNLayerWrapper -> Railways (CNLayer*)
│   └── CNLayerGroup("Airports")
│       ├── CNLayerWrapper -> AirportPoints (CNLayer*)
│       └── CNLayerWrapper -> AirportPolygons (CNLayer*)
└── CNLayerWrapper -> Labels (CNLayer*)
```

---

## 32. 空间索引并发保护 *(新增)*

### 32.1 并发读写策略

**设计背景**: 增量空间索引更新时未考虑并发读写，可能导致数据竞争。

```cpp
namespace OGC {

/**
 * @brief 线程安全空间索引包装器
 */
template<typename IndexType>
class CNThreadSafeSpatialIndex : public CNSpatialIndex {
public:
    explicit CNThreadSafeSpatialIndex(std::unique_ptr<IndexType> index)
        : index_(std::move(index)) {}
    
    void Insert(int64_t fid, const CNEnvelope& extent) override {
        CNReadWriteLock::WriteGuard guard(lock_);
        index_->Insert(fid, extent);
    }
    
    void Remove(int64_t fid, const CNEnvelope& extent) override {
        CNReadWriteLock::WriteGuard guard(lock_);
        index_->Remove(fid, extent);
    }
    
    std::vector<int64_t> Query(const CNEnvelope& extent) const override {
        CNReadWriteLock::ReadGuard guard(lock_);
        return index_->Query(extent);
    }
    
    void Clear() override {
        CNReadWriteLock::WriteGuard guard(lock_);
        index_->Clear();
    }
    
    SpatialIndexType GetType() const override {
        return index_->GetType();
    }
    
    size_t GetMemoryUsage() const override {
        CNReadWriteLock::ReadGuard guard(lock_);
        return index_->GetMemoryUsage();
    }
    
    /**
     * @brief 批量插入（减少锁获取次数）
     */
    void InsertBatch(const std::vector<std::pair<int64_t, CNEnvelope>>& items) {
        CNReadWriteLock::WriteGuard guard(lock_);
        for (const auto& item : items) {
            index_->Insert(item.first, item.second);
        }
    }
    
    /**
     * @brief 批量删除
     */
    void RemoveBatch(const std::vector<std::pair<int64_t, CNEnvelope>>& items) {
        CNReadWriteLock::WriteGuard guard(lock_);
        for (const auto& item : items) {
            index_->Remove(item.first, item.second);
        }
    }
    
    /**
     * @brief 获取底层索引（用于批量操作）
     * @param lock 返回的锁对象，生命周期内持有写锁
     */
    std::pair<IndexType*, std::unique_ptr<CNReadWriteLock::WriteGuard>>
    AcquireWriteAccess() {
        auto guard = std::make_unique<CNReadWriteLock::WriteGuard>(lock_);
        return {index_.get(), std::move(guard)};
    }
    
private:
    std::unique_ptr<IndexType> index_;
    mutable CNReadWriteLock lock_;
};

} // namespace OGC
```

### 32.2 读写锁集成

```cpp
namespace OGC {

/**
 * @brief 内存图层并发索引支持
 */
class CNMemoryLayer : public CNObservableLayer {
public:
    /**
     * @brief 设置并发索引模式
     */
    void SetConcurrentIndex(bool enabled) {
        concurrent_index_enabled_ = enabled;
        if (enabled && spatial_index_) {
            auto* raw_index = spatial_index_.get();
            
            if (auto* rtree = dynamic_cast<CNRTreeIndex*>(raw_index)) {
                spatial_index_ = std::make_unique<
                    CNThreadSafeSpatialIndex<CNRTreeIndex>>(
                    std::unique_ptr<CNRTreeIndex>(rtree));
            }
        }
    }
    
    /**
     * @brief 并发安全的空间查询
     */
    std::vector<int64_t> ConcurrentSpatialQuery(
        const CNEnvelope& extent) const {
        if (spatial_index_) {
            return spatial_index_->Query(extent);
        }
        return {};
    }
    
    /**
     * @brief 批量更新索引（高效）
     */
    void BatchUpdateIndex(
        const std::vector<std::pair<int64_t, CNEnvelope>>& additions,
        const std::vector<std::pair<int64_t, CNEnvelope>>& removals) {
        if (!spatial_index_) return;
        
        auto* ts_index = dynamic_cast<
            CNThreadSafeSpatialIndex<CNSpatialIndex>*>(spatial_index_.get());
        
        if (ts_index) {
            ts_index->RemoveBatch(removals);
            ts_index->InsertBatch(additions);
        } else {
            for (const auto& item : removals) {
                spatial_index_->Remove(item.first, item.second);
            }
            for (const auto& item : additions) {
                spatial_index_->Insert(item.first, item.second);
            }
        }
    }
    
private:
    bool concurrent_index_enabled_ = false;
};

} // namespace OGC
```

---

**文档结束**

---

## 33. WFS错误处理和重试机制 *(新增 v1.5)*

### 33.1 CNWFSRetryPolicy 重试策略

**设计背景**: WFS服务依赖网络，网络不稳定环境下查询失败率高，需要完善的错误处理和重试机制。

```cpp
namespace OGC {

/**
 * @brief WFS错误类型
 */
enum class CNWFSErrorType {
    kNone,                  // 无错误
    kNetworkError,          // 网络错误（连接超时、DNS解析失败等）
    kHttpError,             // HTTP错误（4xx、5xx）
    kParseError,            // 响应解析错误
    kServiceError,          // WFS服务错误（ExceptionReport）
    kAuthenticationError,   // 认证错误
    kRateLimitError,        // 限流错误
    kTimeoutError           // 请求超时
};

/**
 * @brief WFS错误信息
 */
struct CNWFSError {
    CNWFSErrorType type = CNWFSErrorType::kNone;
    int http_code = 0;
    std::string message;
    std::string service_code;       // WFS服务异常代码
    std::string service_message;    // WFS服务异常消息
    bool is_retryable = false;      // 是否可重试
};

/**
 * @brief WFS重试策略配置
 */
struct CNWFSRetryPolicy {
    int max_retries = 3;                    // 最大重试次数
    int initial_delay_ms = 1000;            // 初始延迟（毫秒）
    int max_delay_ms = 30000;               // 最大延迟（毫秒）
    double backoff_multiplier = 2.0;        // 退避乘数
    std::set<CNWFSErrorType> retryable_errors = {
        CNWFSErrorType::kNetworkError,
        CNWFSErrorType::kTimeoutError,
        CNWFSErrorType::kRateLimitError,
        CNWFSErrorType::kServiceError
    };
    
    /**
     * @brief 计算重试延迟（指数退避）
     */
    int CalculateDelay(int retry_count) const {
        int delay = static_cast<int>(
            initial_delay_ms * std::pow(backoff_multiplier, retry_count));
        return std::min(delay, max_delay_ms);
    }
    
    /**
     * @brief 判断错误是否可重试
     */
    bool IsRetryable(const CNWFSError& error) const {
        return error.is_retryable && 
               retryable_errors.find(error.type) != retryable_errors.end();
    }
};

/**
 * @brief WFS熔断器状态
 */
enum class CNWFSBreakerState {
    kClosed,        // 关闭状态（正常）
    kOpen,          // 打开状态（熔断）
    kHalfOpen      // 半开状态（尝试恢复）
};

/**
 * @brief WFS熔断器配置
 */
struct CNWFSBreakerConfig {
    int failure_threshold = 5;          // 失败阈值
    int success_threshold = 3;          // 恢复阈值
    int open_timeout_ms = 60000;        // 熔断超时时间
};

/**
 * @brief WFS熔断器
 */
class CNWFSBreaker {
public:
    explicit CNWFSBreaker(const CNWFSBreakerConfig& config = {});
    
    /**
     * @brief 是否允许请求
     */
    bool AllowRequest();
    
    /**
     * @brief 记录成功
     */
    void RecordSuccess();
    
    /**
     * @brief 记录失败
     */
    void RecordFailure();
    
    /**
     * @brief 获取当前状态
     */
    CNWFSBreakerState GetState() const { return state_; }
    
    /**
     * @brief 获取统计信息
     */
    struct Stats {
        int total_requests = 0;
        int successful_requests = 0;
        int failed_requests = 0;
        int rejected_requests = 0;
    };
    Stats GetStats() const;

private:
    CNWFSBreakerConfig config_;
    CNWFSBreakerState state_ = CNWFSBreakerState::kClosed;
    int failure_count_ = 0;
    int success_count_ = 0;
    std::chrono::steady_clock::time_point open_time_;
    mutable std::mutex mutex_;
    Stats stats_;
};

/**
 * @brief WFS请求执行器（带重试和熔断）
 */
class CNWFSRequestExecutor {
public:
    CNWFSRequestExecutor(
        const CNWFSRetryPolicy& retry_policy = {},
        const CNWFSBreakerConfig& breaker_config = {});
    
    /**
     * @brief 执行请求（带重试和熔断）
     * @param request_func 请求函数
     * @return 响应内容，失败返回空
     */
    std::string Execute(
        std::function<std::pair<std::string, CNWFSError>()> request_func);
    
    /**
     * @brief 获取熔断器
     */
    CNWFSBreaker& GetBreaker() { return breaker_; }
    
    /**
     * @brief 获取最后一次错误
     */
    const CNWFSError& GetLastError() const { return last_error_; }

private:
    CNWFSError ParseError(const std::string& response, int http_code);
    
    CNWFSRetryPolicy retry_policy_;
    CNWFSBreaker breaker_;
    CNWFSError last_error_;
};

} // namespace OGC
```

### 33.2 CNWFSLayer错误处理集成

```cpp
namespace OGC {

class CNWFSLayer : public CNReadOnlyLayer {
public:
    /**
     * @brief 设置重试策略
     */
    void SetRetryPolicy(const CNWFSRetryPolicy& policy);
    
    /**
     * @brief 设置熔断器配置
     */
    void SetBreakerConfig(const CNWFSBreakerConfig& config);
    
    /**
     * @brief 获取熔断器状态
     */
    CNWFSBreakerState GetBreakerState() const;
    
    /**
     * @brief 获取最后一次错误
     */
    CNWFSError GetLastError() const;
    
    /**
     * @brief 手动重置熔断器
     */
    void ResetBreaker();
    
private:
    std::unique_ptr<CNWFSRequestExecutor> executor_;
};

} // namespace OGC
```

---

## 34. 图层组事务传播机制 *(新增 v1.5)*

### 34.1 事务传播行为定义

**设计背景**: 图层组内多图层事务操作需要明确的传播行为，保证跨图层操作的原子性。

```cpp
namespace OGC {

/**
 * @brief 事务传播行为
 */
enum class CNTransactionPropagation {
    kRequired,          // 需要事务：有则加入，无则新建
    kRequiresNew,       // 必须新事务：挂起当前事务，新建事务
    kSupports,          // 支持事务：有则加入，无则非事务执行
    kNotSupported,      // 不支持事务：挂起当前事务，非事务执行
    kMandatory,         // 强制事务：必须有事务，否则抛异常
    kNever,             // 禁止事务：必须无事务，否则抛异常
    kNested            // 嵌套事务：创建嵌套事务
};

/**
 * @brief 事务状态
 */
enum class CNTransactionState {
    kActive,            // 活跃状态
    kCommitted,         // 已提交
    kRolledBack,        // 已回滚
    kCompleted,         // 已完成（提交或回滚）
    kUnknown           // 未知状态
};

/**
 * @brief 图层事务上下文
 */
struct CNLayerTransactionContext {
    CNLayer* layer;
    CNTransactionState state = CNTransactionState::kActive;
    std::vector<CNStatus> operation_results;
};

/**
 * @brief 图层组事务协调器
 */
class CNLayerGroupTransactionCoordinator {
public:
    /**
     * @brief 开始图层组事务
     * @param group 图层组
     * @param propagation 传播行为
     * @return 状态码
     */
    CNStatus BeginTransaction(
        CNLayerGroup* group,
        CNTransactionPropagation propagation = CNTransactionPropagation::kRequired);
    
    /**
     * @brief 提交图层组事务
     * @param group 图层组
     * @return 状态码
     */
    CNStatus CommitTransaction(CNLayerGroup* group);
    
    /**
     * @brief 回滚图层组事务
     * @param group 图层组
     * @return 状态码
     */
    CNStatus RollbackTransaction(CNLayerGroup* group);
    
    /**
     * @brief 获取事务状态
     */
    CNTransactionState GetTransactionState(const CNLayerGroup* group) const;
    
    /**
     * @brief 是否在事务中
     */
    bool IsInTransaction(const CNLayerGroup* group) const;

private:
    std::map<CNLayerGroup*, std::vector<CNLayerTransactionContext>> transactions_;
    mutable std::mutex mutex_;
    
    std::vector<CNLayer*> CollectLayers(CNLayerGroup* group);
    CNStatus CommitAll(std::vector<CNLayerTransactionContext>& contexts);
    CNStatus RollbackAll(std::vector<CNLayerTransactionContext>& contexts);
};

} // namespace OGC
```

### 34.2 CNLayerGroup事务支持

```cpp
namespace OGC {

class CNLayerGroup : public CNLayerNode {
public:
    // ========== 事务支持 ==========
    
    /**
     * @brief 开始事务（所有子图层）
     * @param propagation 传播行为
     * @return 状态码
     */
    CNStatus StartTransaction(
        CNTransactionPropagation propagation = CNTransactionPropagation::kRequired);
    
    /**
     * @brief 提交事务
     * @return 状态码
     * 
     * 如果任一图层提交失败，将回滚所有已提交的图层
     */
    CNStatus CommitTransaction();
    
    /**
     * @brief 回滚事务
     * @return 状态码
     */
    CNStatus RollbackTransaction();
    
    /**
     * @brief 是否支持事务
     */
    bool SupportsTransactions() const;
    
    /**
     * @brief 是否在事务中
     */
    bool IsInTransaction() const;
    
    /**
     * @brief 设置事务协调器
     */
    void SetTransactionCoordinator(
        std::shared_ptr<CNLayerGroupTransactionCoordinator> coordinator);

private:
    std::shared_ptr<CNLayerGroupTransactionCoordinator> transaction_coordinator_;
    bool in_transaction_ = false;

    std::vector<std::unique_ptr<CNLayer>> layers_;
    std::vector<std::unique_ptr<CNLayerGroup>> groups_;
};

} // namespace OGC
```

### 34.3 两阶段提交实现

```cpp
namespace OGC {

/**
 * @brief 两阶段提交协调器
 */
class CNTwoPhaseCommitCoordinator {
public:
    /**
     * @brief 第一阶段：准备
     * @param layers 参与事务的图层列表
     * @return 所有图层是否都准备好
     */
    bool Prepare(const std::vector<CNLayer*>& layers);
    
    /**
     * @brief 第二阶段：提交
     * @param layers 参与事务的图层列表
     * @return 状态码
     */
    CNStatus Commit(const std::vector<CNLayer*>& layers);
    
    /**
     * @brief 回滚
     * @param layers 参与事务的图层列表
     * @return 状态码
     */
    CNStatus Rollback(const std::vector<CNLayer*>& layers);

private:
    struct ParticipantState {
        CNLayer* layer;
        bool prepared = false;
        bool committed = false;
    };
    std::vector<ParticipantState> participants_;
};

} // namespace OGC
```

---

## 35. 流式读取背压机制 *(新增 v1.5)*

### 35.1 背压策略定义

**设计背景**: CNFeatureStream消费者速度慢于生产者时可能导致内存积压，需要背压机制。

```cpp
namespace OGC {

/**
 * @brief 背压策略类型
 */
enum class CNBackpressureStrategy {
    kNone,              // 无背压（原有行为）
    kBlock,             // 阻塞策略：生产者等待消费者
    kDrop,              // 丢弃策略：丢弃超出缓冲的数据
    kBuffer,            // 缓冲策略：使用固定大小缓冲区
    kThrottle           // 节流策略：动态调整生产速度
};

/**
 * @brief 背压配置
 */
struct CNBackpressureConfig {
    CNBackpressureStrategy strategy = CNBackpressureStrategy::kBlock;
    size_t max_buffer_size = 10000;         // 最大缓冲要素数
    int block_timeout_ms = 5000;            // 阻塞超时时间
    double throttle_factor = 0.8;           // 节流因子（0-1）
    bool enable_monitoring = true;          // 启用监控
};

/**
 * @brief 背压状态
 */
struct CNBackpressureStatus {
    size_t current_buffer_size = 0;         // 当前缓冲大小
    size_t total_produced = 0;              // 总生产数量
    size_t total_consumed = 0;              // 总消费数量
    size_t total_dropped = 0;               // 总丢弃数量
    size_t total_blocked = 0;               // 总阻塞次数
    double production_rate = 0.0;           // 生产速率（要素/秒）
    double consumption_rate = 0.0;          // 消费速率（要素/秒）
    bool is_backpressured = false;          // 是否处于背压状态
};

/**
 * @brief 背压控制器
 */
class CNBackpressureController {
public:
    explicit CNBackpressureController(const CNBackpressureConfig& config = {});
    
    /**
     * @brief 生产者请求许可
     * @param count 请求生产的数量
     * @return 实际允许生产的数量（可能小于请求）
     */
    size_t RequestProduce(size_t count);
    
    /**
     * @brief 生产者通知完成
     * @param count 实际生产的数量
     */
    void NotifyProduced(size_t count);
    
    /**
     * @brief 消费者通知消费
     * @param count 消费的数量
     */
    void NotifyConsumed(size_t count);
    
    /**
     * @brief 获取当前状态
     */
    CNBackpressureStatus GetStatus() const;
    
    /**
     * @brief 重置状态
     */
    void Reset();
    
    /**
     * @brief 设置策略
     */
    void SetStrategy(CNBackpressureStrategy strategy);
    
    /**
     * @brief 设置缓冲大小
     */
    void SetMaxBufferSize(size_t size);

private:
    CNBackpressureConfig config_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    size_t buffer_size_ = 0;
    CNBackpressureStatus status_;
    std::chrono::steady_clock::time_point last_measure_time_;
};

} // namespace OGC
```

### 35.2 CNFeatureStream背压集成

```cpp
namespace OGC {

/**
 * @brief 带背压的要素流
 */
class CNBackpressuredFeatureStream : public CNFeatureStream {
public:
    CNBackpressuredFeatureStream(
        CNLayer* layer,
        const CNBackpressureConfig& config = {});
    
    /**
     * @brief 读取下一批要素（带背压控制）
     * @param batch_size 批次大小
     * @return 要素列表
     */
    std::vector<std::unique_ptr<CNFeature>> ReadNextBatch(
        size_t batch_size = 1000) override;
    
    /**
     * @brief 是否到达末尾
     */
    bool IsEndOfStream() const override;
    
    /**
     * @brief 获取已读取数量
     */
    int64_t GetReadCount() const override;
    
    /**
     * @brief 重置流
     */
    void Reset() override;
    
    /**
     * @brief 关闭流
     */
    void Close() override;
    
    // ========== 背压相关接口 ==========
    
    /**
     * @brief 获取背压控制器
     */
    CNBackpressureController& GetBackpressureController();
    
    /**
     * @brief 获取背压状态
     */
    CNBackpressureStatus GetBackpressureStatus() const;
    
    /**
     * @brief 设置背压策略
     */
    void SetBackpressureStrategy(CNBackpressureStrategy strategy);
    
    /**
     * @brief 设置消费者回调（用于反馈消费速度）
     */
    void SetConsumerCallback(std::function<void(size_t)> callback);

private:
    CNLayer* layer_;
    CNBackpressureController backpressure_;
    int64_t read_count_ = 0;
    bool end_of_stream_ = false;
    std::function<void(size_t)> consumer_callback_;
};

/**
 * @brief 流式处理辅助函数
 */
class CNStreamProcessor {
public:
    /**
     * @brief 带背压的流式处理
     */
    template<typename Processor>
    static int64_t ProcessWithBackpressure(
        CNLayer* layer,
        Processor processor,
        const CNBackpressureConfig& config = {},
        size_t batch_size = 1000) {
        
        CNBackpressuredFeatureStream stream(layer, config);
        int64_t total_processed = 0;
        
        while (!stream.IsEndOfStream()) {
            auto batch = stream.ReadNextBatch(batch_size);
            
            for (auto& feature : batch) {
                processor(*feature);
                total_processed++;
            }
            
            // 通知消费完成
            stream.GetBackpressureController().NotifyConsumed(batch.size());
        }
        
        return total_processed;
    }
    
    /**
     * @brief 并行流式处理（多消费者）
     */
    template<typename Processor>
    static int64_t ParallelProcess(
        CNLayer* layer,
        Processor processor,
        size_t num_consumers = 4,
        const CNBackpressureConfig& config = {});
};

} // namespace OGC
```

---

## 36. 事件通知异步机制 *(新增 v1.5)*

### 36.1 异步事件分发器

**设计背景**: CNLayerObserver::OnLayerChanged同步调用可能阻塞图层操作，需要异步机制。

```cpp
namespace OGC {

/**
 * @brief 事件优先级
 */
enum class CNEventPriority {
    kLow = 0,
    kNormal = 1,
    kHigh = 2,
    kCritical = 3
};

/**
 * @brief 异步事件项
 */
struct CNAsyncEvent {
    CNLayerEventType type;
    CNLayer* layer;
    CNLayerEventArgs args;
    CNEventPriority priority;
    std::chrono::steady_clock::time_point timestamp;
};

/**
 * @brief 异步事件分发器配置
 */
struct CNAsyncEventDispatcherConfig {
    size_t queue_size = 10000;              // 事件队列大小
    size_t worker_threads = 2;              // 工作线程数
    int event_timeout_ms = 5000;            // 事件处理超时
    bool drop_on_overflow = false;          // 队列满时是否丢弃
    CNEventPriority min_priority = CNEventPriority::kLow;  // 最低处理优先级
};

/**
 * @brief 异步事件分发器
 */
class CNAsyncEventDispatcher {
public:
    explicit CNAsyncEventDispatcher(
        const CNAsyncEventDispatcherConfig& config = {});
    ~CNAsyncEventDispatcher();
    
    /**
     * @brief 启动分发器
     */
    void Start();
    
    /**
     * @brief 停止分发器
     */
    void Stop();
    
    /**
     * @brief 是否运行中
     */
    bool IsRunning() const;
    
    /**
     * @brief 分发事件（异步）
     * @param layer 图层
     * @param args 事件参数
     * @param priority 优先级
     * @return 是否成功加入队列
     */
    bool Dispatch(
        CNLayer* layer,
        const CNLayerEventArgs& args,
        CNEventPriority priority = CNEventPriority::kNormal);
    
    /**
     * @brief 分发事件（同步，等待处理完成）
     */
    bool DispatchSync(
        CNLayer* layer,
        const CNLayerEventArgs& args,
        CNEventPriority priority = CNEventPriority::kNormal);
    
    /**
     * @brief 获取队列大小
     */
    size_t GetQueueSize() const;
    
    /**
     * @brief 获取统计信息
     */
    struct Stats {
        size_t total_dispatched = 0;
        size_t total_processed = 0;
        size_t total_dropped = 0;
        size_t total_timeouts = 0;
        double avg_processing_time_ms = 0.0;
    };
    Stats GetStats() const;

private:
    void WorkerThread();
    
    CNAsyncEventDispatcherConfig config_;
    std::priority_queue<CNAsyncEvent> event_queue_;
    mutable std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::vector<std::thread> workers_;
    std::atomic<bool> running_{false};
    Stats stats_;
    mutable std::mutex stats_mutex_;
};

} // namespace OGC
```

### 36.2 CNObservableLayer异步支持

```cpp
namespace OGC {

/**
 * @brief 可观察图层基类（支持异步通知）
 */
class CNObservableLayer : public CNLayer {
public:
    /**
     * @brief 设置异步分发器
     */
    void SetAsyncDispatcher(std::shared_ptr<CNAsyncEventDispatcher> dispatcher);
    
    /**
     * @brief 设置是否使用异步通知
     */
    void SetAsyncNotification(bool enabled);
    
    /**
     * @brief 是否使用异步通知
     */
    bool IsAsyncNotification() const;
    
protected:
    /**
     * @brief 通知观察者（自动选择同步/异步）
     */
    void NotifyObservers(
        CNLayerEventType type,
        int64_t fid = -1,
        int field_index = -1,
        const std::string& message = "",
        const CNFeature* feature = nullptr,
        CNEventPriority priority = CNEventPriority::kNormal);
    
    /**
     * @brief 通知观察者（变更前，始终同步）
     */
    bool NotifyObserversChanging(
        CNLayerEventType type,
        int64_t fid = -1,
        int field_index = -1,
        const std::string& message = "",
        const CNFeature* feature = nullptr);

private:
    std::shared_ptr<CNAsyncEventDispatcher> async_dispatcher_;
    bool async_notification_ = false;
};

} // namespace OGC
```

---

## 37. WFS响应解析安全校验 *(新增 v1.5)*

### 37.1 安全解析器

**设计背景**: 解析外部WFS响应可能存在XXE攻击、JSON注入等安全风险。

```cpp
namespace OGC {

/**
 * @brief 解析安全配置
 */
struct CNParseSecurityConfig {
    bool disable_xxe = true;                // 禁用XML外部实体
    bool disable_dtd = true;                // 禁用DTD
    size_t max_xml_depth = 100;             // XML最大嵌套深度
    size_t max_json_depth = 50;             // JSON最大嵌套深度
    size_t max_string_length = 10000000;    // 最大字符串长度（10MB）
    size_t max_array_size = 1000000;        // 最大数组大小
    size_t max_attributes = 1000;           // 最大属性数量
    bool validate_utf8 = true;              // 验证UTF-8编码
    std::set<std::string> allowed_protocols = {"http", "https"};  // 允许的协议
};

/**
 * @brief 安全XML解析器
 */
class CNSecureXMLParser {
public:
    explicit CNSecureXMLParser(const CNParseSecurityConfig& config = {});
    
    /**
     * @brief 安全解析XML
     * @param xml_string XML字符串
     * @return 解析结果，失败返回nullptr
     */
    std::unique_ptr<CNXMLDocument> Parse(const std::string& xml_string);
    
    /**
     * @brief 验证XML安全性
     * @param xml_string XML字符串
     * @param error 错误信息
     * @return 是否安全
     */
    bool Validate(const std::string& xml_string, std::string& error);

private:
    CNParseSecurityConfig config_;
    
    bool CheckXXE(const std::string& xml);
    bool CheckDepth(const std::string& xml);
    bool CheckSize(const std::string& xml);
};

/**
 * @brief 安全JSON解析器
 */
class CNSecureJSONParser {
public:
    explicit CNSecureJSONParser(const CNParseSecurityConfig& config = {});
    
    /**
     * @brief 安全解析JSON
     * @param json_string JSON字符串
     * @return 解析结果，失败返回nullptr
     */
    std::unique_ptr<CNJSONDocument> Parse(const std::string& json_string);
    
    /**
     * @brief 验证JSON安全性
     */
    bool Validate(const std::string& json_string, std::string& error);

private:
    CNParseSecurityConfig config_;
};

} // namespace OGC
```

### 37.2 安全WFS响应解析器

```cpp
namespace OGC {

/**
 * @brief 安全WFS响应解析器
 */
class CNSecureWFSResponseParser {
public:
    explicit CNSecureWFSResponseParser(
        const CNParseSecurityConfig& config = {});
    
    /**
     * @brief 安全解析GetCapabilities响应
     */
    std::vector<std::pair<std::string, std::string>>
    ParseCapabilities(const std::string& response);
    
    /**
     * @brief 安全解析DescribeFeatureType响应
     */
    std::unique_ptr<CNFeatureDefn>
    ParseFeatureType(const std::string& response, const std::string& type_name);
    
    /**
     * @brief 安全解析GetFeature响应（GML）
     */
    std::vector<std::unique_ptr<CNFeature>>
    ParseGMLFeatures(const std::string& response, CNFeatureDefn* defn);
    
    /**
     * @brief 安全解析GetFeature响应（GeoJSON）
     */
    std::vector<std::unique_ptr<CNFeature>>
    ParseGeoJSONFeatures(const std::string& response, CNFeatureDefn* defn);
    
    /**
     * @brief 获取最后一次安全警告
     */
    std::string GetLastWarning() const { return last_warning_; }
    
    /**
     * @brief 获取安全配置
     */
    const CNParseSecurityConfig& GetConfig() const { return config_; }

private:
    CNParseSecurityConfig config_;
    CNSecureXMLParser xml_parser_;
    CNSecureJSONParser json_parser_;
    std::string last_warning_;
    
    bool ValidateGeometry(const CNGeometry* geom);
    bool ValidateFeature(const CNFeature* feature);
};

/**
 * @brief CNWFSLayer安全解析集成
 */
class CNWFSLayer : public CNReadOnlyLayer {
public:
    /**
     * @brief 设置解析安全配置
     */
    void SetParseSecurityConfig(const CNParseSecurityConfig& config);
    
    /**
     * @brief 获取安全解析器
     */
    CNSecureWFSResponseParser& GetSecureParser();

private:
    std::unique_ptr<CNSecureWFSResponseParser> secure_parser_;
};

} // namespace OGC
```

---

## 38. 工作量评估更新 *(新增 v1.5)*

### 38.1 更新后的里程碑规划

| 里程碑 | 阶段名称 | 内容 | 预估工作量 | 依赖 | 交付物 |
|--------|----------|------|------------|------|--------|
| **M1** | 核心框架 | CNLayer基类、CNMemoryLayer、CNFeatureDefn、基础测试 | 4周 | - | 可编译的库、单元测试 |
| **M2** | 数据源抽象 | CNDataSource、CNDriver、CNDriverManager | 2周 | M1 | 驱动注册机制 |
| **M3** | 矢量格式支持 | Shapefile读写、GeoJSON读写 | 4周 | M2 | 文件格式支持 |
| **M4** | 数据库支持 | GeoPackage、SpatiaLite、PostGIS（含连接池） | 6周 | M2 | 数据库连接支持 |
| **M5** | 空间索引 | R树、四叉树、增量更新、并发保护 | 4周 | M1 | 空间查询优化 |
| **M6** | 线程安全完善 | 读写锁、快照、分段锁缓存 | 2周 | M1, M5 | 并发测试通过 |
| **M7** | GDAL互操作 | CNGDALAdapter、格式转换器 | 2周 | M2, M3 | GDAL集成 |
| **M8** | 栅格支持 | CNRasterDataset、GeoTIFF | 4周 | M2 | 栅格数据支持 |
| **M9** | 性能优化 | 对象池、增量事务、SIMD | 3周 | M5, M6 | 性能基准测试 |
| **M10** | WFS客户端 | CNWFSLayer、错误处理、安全解析 | 6周 | M2 | OGC服务支持 |
| **M11** | 图层组 | CNLayerGroup、事务传播、持久化 | 4周 | M1 | 图层组织管理 |
| **M12** | 高级特性 | 事件异步、流式背压 | 3周 | M6, M10 | 高级功能 |
| **M13** | 文档与示例 | API文档、使用示例、最佳实践 | 2周 | M1-M12 | 完整文档 |

**总工作量**: 约46周（约11.5个月）

### 38.2 模块工作量明细

| 模块 | 代码行数估算 | 测试代码估算 | 复杂度 | 风险等级 | 预估工期 |
|------|-------------|-------------|--------|----------|----------|
| CNLayer基类 | 500 | 300 | 中 | 低 | 1周 |
| CNMemoryLayer | 1500 | 800 | 高 | 中 | 3周 |
| CNDataSource | 800 | 400 | 中 | 低 | 2周 |
| CNDriverManager | 400 | 200 | 低 | 低 | 1周 |
| Shapefile驱动 | 2000 | 600 | 中 | 低 | 2周 |
| GeoPackage驱动 | 2500 | 800 | 高 | 中 | 3周 |
| PostGIS驱动 | 4000 | 1200 | 高 | 高 | 4周 |
| 连接池 | 800 | 400 | 中 | 中 | 1周 |
| 空间索引 | 2500 | 1200 | 高 | 中 | 3周 |
| 并发空间索引 | 500 | 300 | 中 | 中 | 1周 |
| 线程安全组件 | 1000 | 600 | 高 | 高 | 2周 |
| GDAL适配器 | 1500 | 500 | 中 | 低 | 2周 |
| 栅格数据集 | 2000 | 800 | 高 | 中 | 4周 |
| WFS客户端 | 3000 | 1000 | 高 | 中 | 4周 |
| WFS错误处理 | 800 | 400 | 中 | 低 | 1周 |
| WFS安全解析 | 600 | 300 | 中 | 低 | 1周 |
| 图层组 | 1500 | 600 | 中 | 低 | 2周 |
| 事务传播 | 800 | 400 | 高 | 中 | 2周 |
| 事件异步 | 600 | 300 | 中 | 低 | 1周 |
| 流式背压 | 500 | 300 | 中 | 低 | 1周 |
| 文档与示例 | - | - | 低 | 低 | 2周 |

### 38.3 风险评估更新

| 风险 | 可能性 | 影响 | 缓解措施 | 应急预案 |
|------|--------|------|----------|----------|
| PostGIS协议变更 | 中 | 高 | 使用libpq官方API、连接池隔离 | 延长M4工期 |
| WFS版本兼容 | 中 | 中 | 多版本测试、适配层 | 延长M10工期 |
| 线程死锁 | 中 | 高 | 代码审查、压力测试、死锁检测工具 | 延长M6工期 |
| 内存泄漏 | 低 | 高 | RAII、智能指针、内存检测工具 | 增加测试时间 |
| 性能不达标 | 中 | 中 | 性能基准测试、持续优化 | 延长M9工期 |
| 跨平台兼容性 | 低 | 中 | CI/CD多平台测试 | 增加测试时间 |
| 安全漏洞 | 低 | 高 | 安全审计、渗透测试 | 延长M10工期 |
| 文档版本不一致 | 低 | 中 | 版本控制、评审流程 | 立即修正 |

---

## 39. v1.6改进项：高优先级问题解决 *(新增 v1.6)*

### 39.1 CNConnectionPool连接泄漏检测机制

**问题**: CNConnectionPool缺少连接泄漏检测机制，可能导致连接未正确归还。

```cpp
namespace OGC {

/**
 * @brief 连接泄漏检测配置
 */
struct CNLeakDetectionConfig {
    bool enabled = true;                    // 是否启用泄漏检测
    int leak_threshold_ms = 60000;          // 泄漏阈值（毫秒）
    bool auto_reclaim = true;               // 自动回收泄漏连接
    int reclaim_interval_ms = 30000;        // 回收检查间隔
    bool log_leaks = true;                  // 记录泄漏日志
};

/**
 * @brief 连接包装器（带泄漏检测）
 */
class CNTrackedConnection {
public:
    CNTrackedConnection(
        std::shared_ptr<CNDbConnection> conn,
        CNConnectionPool* pool,
        int64_t track_id)
        : conn_(conn), pool_(pool), track_id_(track_id),
          acquire_time_(std::chrono::steady_clock::now()),
          released_(false) {}
    
    ~CNTrackedConnection() {
        if (!released_ && conn_) {
            if (pool_) {
                pool_->ReportLeak(track_id_, GetHoldDuration());
                if (pool_->GetLeakDetectionConfig().auto_reclaim) {
                    pool_->Release(conn_);
                }
            }
        }
    }
    
    CNDbConnection* Get() { return conn_.get(); }
    CNDbConnection* operator->() { return conn_.get(); }
    
    void MarkReleased() { released_ = true; }
    
    int64_t GetHoldDuration() const {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            now - acquire_time_).count();
    }
    
    int64_t GetTrackId() const { return track_id_; }

private:
    std::shared_ptr<CNDbConnection> conn_;
    CNConnectionPool* pool_;
    int64_t track_id_;
    std::chrono::steady_clock::time_point acquire_time_;
    bool released_;
};

/**
 * @brief 连接池泄漏检测扩展
 */
class CNConnectionPoolWithLeakDetection : public CNConnectionPool {
public:
    void SetLeakDetectionConfig(const CNLeakDetectionConfig& config);
    CNLeakDetectionConfig GetLeakDetectionConfig() const;
    
    /**
     * @brief 获取泄漏的连接列表
     */
    std::vector<int64_t> GetLeakedConnections() const;
    
    /**
     * @brief 报告连接泄漏
     */
    void ReportLeak(int64_t track_id, int64_t hold_duration_ms);
    
    /**
     * @brief 获取泄漏统计
     */
    struct LeakStats {
        int total_leaks = 0;
        int auto_reclaimed = 0;
        int64_t max_hold_time_ms = 0;
        std::vector<std::string> leak_stack_traces;
    };
    LeakStats GetLeakStats() const;

private:
    CNLeakDetectionConfig leak_config_;
    std::map<int64_t, std::chrono::steady_clock::time_point> active_connections_;
    LeakStats leak_stats_;
    mutable std::mutex leak_mutex_;
};

} // namespace OGC
```

### 39.2 CNAccessControl字段级权限控制

**问题**: CNAccessControl缺少字段级权限控制，无法实现细粒度访问控制。

```cpp
namespace OGC {

/**
 * @brief 字段权限类型
 */
enum class CNFieldPermission {
    kNone = 0,          // 无权限
    kRead = 1,          // 只读
    kWrite = 2,         // 只写
    kReadWrite = 3,     // 读写
    kAdmin = 7          // 管理员权限
};

/**
 * @brief 字段权限规则
 */
struct CNFieldPermissionRule {
    std::string field_name;             // 字段名称（支持通配符）
    CNFieldPermission permission;       // 权限类型
    std::string condition;              // 条件表达式（可选）
    std::set<std::string> roles;        // 适用角色
};

/**
 * @brief 字段级访问控制器
 */
class CNFieldLevelAccessControl {
public:
    /**
     * @brief 添加字段权限规则
     */
    void AddPermissionRule(const CNFieldPermissionRule& rule);
    
    /**
     * @brief 检查字段权限
     * @param field_name 字段名称
     * @param user_roles 用户角色列表
     * @param permission 需要的权限
     * @return 是否有权限
     */
    bool CheckPermission(
        const std::string& field_name,
        const std::set<std::string>& user_roles,
        CNFieldPermission permission) const;
    
    /**
     * @brief 过滤要素字段（移除无权限字段）
     * @param feature 要素
     * @param user_roles 用户角色列表
     * @param permission 需要的权限
     * @return 过滤后的要素
     */
    std::unique_ptr<CNFeature> FilterFields(
        const CNFeature* feature,
        const std::set<std::string>& user_roles,
        CNFieldPermission permission) const;
    
    /**
     * @brief 获取可见字段列表
     */
    std::vector<std::string> GetVisibleFields(
        const CNFeatureDefn* defn,
        const std::set<std::string>& user_roles) const;
    
    /**
     * @brief 设置字段脱敏规则
     */
    void SetFieldMaskingRule(
        const std::string& field_name,
        const std::string& masking_pattern);

private:
    std::vector<CNFieldPermissionRule> permission_rules_;
    std::map<std::string, std::string> masking_rules_;
    mutable std::mutex mutex_;
    
    bool MatchFieldName(const std::string& pattern, const std::string& name) const;
};

/**
 * @brief 带字段级权限的图层包装器
 */
class CNFieldSecuredLayer : public CNReadOnlyLayer {
public:
    CNFieldSecuredLayer(
        std::unique_ptr<CNLayer> layer,
        std::shared_ptr<CNFieldLevelAccessControl> access_control,
        const std::set<std::string>& user_roles);
    
    std::unique_ptr<CNFeature> GetNextFeature() override;
    std::unique_ptr<CNFeature> GetFeature(int64_t fid) override;
    
private:
    std::unique_ptr<CNLayer> layer_;
    std::shared_ptr<CNFieldLevelAccessControl> access_control_;
    std::set<std::string> user_roles_;
};

} // namespace OGC
```

### 39.3 API废弃策略和迁移指南

**问题**: 缺少API废弃策略和迁移指南，用户升级风险高。

```cpp
namespace OGC {

/**
 * @brief API废弃级别
 */
enum class CNDeprecationLevel {
    kStable,            // 稳定API
    kDeprecated,        // 已废弃，仍可用
    kLegacy,            // 遗留API，计划移除
    kRemoved            // 已移除
};

/**
 * @brief API废弃信息
 */
struct CNDeprecationInfo {
    std::string api_name;               // API名称
    CNDeprecationLevel level;           // 废弃级别
    std::string since_version;          // 废弃起始版本
    std::string removed_version;        // 计划移除版本
    std::string replacement;            // 替代API
    std::string migration_guide;        // 迁移指南
    std::string reason;                 // 废弃原因
};

/**
 * @brief API废弃管理器
 */
class CNDeprecationManager {
public:
    static CNDeprecationManager& Instance();
    
    /**
     * @brief 注册废弃API
     */
    void RegisterDeprecated(const CNDeprecationInfo& info);
    
    /**
     * @brief 检查API状态
     */
    CNDeprecationInfo GetDeprecationInfo(const std::string& api_name) const;
    
    /**
     * @brief 获取所有废弃API列表
     */
    std::vector<CNDeprecationInfo> GetAllDeprecated() const;
    
    /**
     * @brief 获取指定版本的移除列表
     */
    std::vector<std::string> GetRemovedInVersion(
        const std::string& version) const;
    
    /**
     * @brief 设置废弃警告处理器
     */
    void SetWarningHandler(std::function<void(const CNDeprecationInfo&)> handler);

private:
    std::map<std::string, CNDeprecationInfo> deprecated_apis_;
    std::function<void(const CNDeprecationInfo&)> warning_handler_;
    mutable std::mutex mutex_;
};

/**
 * @brief 废弃警告宏
 */
#define CN_DEPRECATED(api_name, replacement, version) \
    CNDeprecationManager::Instance().RegisterDeprecated({ \
        api_name, CNDeprecationLevel::kDeprecated, \
        CN_VERSION, version, replacement, "", "" \
    })

#define CN_DEPRECATED_MSG(msg) \
    []() { \
        CNDeprecationInfo info; \
        /* fill info */ \
        CNDeprecationManager::Instance().GetWarningHandler()(info); \
    }()

} // namespace OGC
```

### 39.4 API迁移指南

| 旧API | 新API | 废弃版本 | 移除版本 | 迁移说明 |
|-------|-------|----------|----------|----------|
| `CNLayer::GetNextFeature()` | `CNLayer::GetNextFeatureRef()` | v1.5 | v2.0 | 使用引用迭代避免内存分配 |
| `CNMemoryLayer::TransactionSnapshot` | `IncrementalSnapshot` | v1.5 | v2.0 | 使用增量快照减少内存占用 |
| `CNLRUCache` | `CNShardedLRUCache` | v1.5 | v2.0 | 使用分段锁缓存提升并发性能 |
| `CNLayer` (单接口) | `CNReadOnlyLayer`/`CNWritableLayer` | v1.5 | v2.0 | 使用角色接口提升灵活性 |

### 39.5 版本兼容性承诺

| 版本系列 | 兼容性承诺 | 支持周期 |
|----------|-----------|----------|
| v1.x | 向后兼容，废弃API保留至少2个版本 | 2026-2028 |
| v2.x | 可能引入破坏性变更，提供迁移工具 | 2028-2030 |

---

**文档版本**: v1.6  
**最后更新**: 2026年3月18日  
**维护者**: Layer Model Team