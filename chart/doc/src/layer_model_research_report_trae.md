# 图层模型设计研究报告

**版本**: 1.8  
**日期**: 2026年3月17日  
**作者**: Trae AI Assistant  
**研究目标**: 为 CNLayer 及其派生类设计文档提供技术支撑

---

## 目录

1. [研究概述](#1-研究概述)
   - 1.1 [研究背景](#11-研究背景)
   - 1.2 [研究范围](#12-研究范围)
   - 1.3 [研究方法](#13-研究方法)
2. [OGC 图层标准与规范](#2-ogc-图层标准与规范)
   - 2.1 [OGC Simple Feature Access 标准](#21-ogc-simple-feature-access-标准)
   - 2.2 [OGC Web Feature Service (WFS) 规范](#22-ogc-web-feature-service-wfs-规范)
   - 2.3 [OGC 要素模型核心接口](#23-ogc-要素模型核心接口)
3. [GDAL OGRLayer 架构分析](#3-gdal-ogrlayer-架构分析)
   - 3.1 [GDAL/OGR 整体架构](#31-gdalogr-整体架构)
   - 3.2 [OGRLayer 核心接口详解](#32-ogrlayer-核心接口详解)
   - 3.3 [GDAL 数据源与驱动架构](#33-gdal-数据源与驱动架构)
   - 3.4 [OGRLayer 实现模式分析](#34-ogrlayer-实现模式分析)
4. [内存图层设计模式](#4-内存图层设计模式)
   - 4.1 [数据结构选择](#41-数据结构选择)
   - 4.2 [FID 管理策略](#42-fid-管理策略)
   - 4.3 [内存事务实现](#43-内存事务实现)
   - 4.4 [内存优化策略](#44-内存优化策略)
5. [矢量图层与文件格式](#5-矢量图层与文件格式)
   - 5.1 [格式抽象层设计](#51-格式抽象层设计)
   - 5.2 [空间过滤器实现](#52-空间过滤器实现)
   - 5.3 [属性过滤器与SQL解析](#53-属性过滤器与sql解析)
   - 5.4 [缓存管理策略](#54-缓存管理策略)
   - 5.5 [坐标参考系统(CRS)处理](#55-坐标参考系统crs处理)
   - 5.6 [空间关系谓词](#56-空间关系谓词)
   - 5.7 [大数据量处理策略](#57-大数据量处理策略)
   - 5.8 [图层组实现](#58-图层组实现)
   - 5.9 [几何有效性验证](#59-几何有效性验证)
   - 5.10 [拓扑操作](#510-拓扑操作)
   - 5.11 [元数据管理](#511-元数据管理)
   - 5.12 [图层样式定义](#512-图层样式定义)
   - 5.13 [内存映射文件支持](#513-内存映射文件支持)
   - 5.14 [异步 I/O 设计](#514-异步-io-设计)
   - 5.15 [插件架构设计](#515-插件架构设计)
   - 5.16 [几何操作算法](#516-几何操作算法)
   - 5.17 [空间分析功能](#517-空间分析功能)
   - 5.18 [数据格式注意事项](#518-数据格式注意事项)
   - 5.19 [性能基准测试](#519-性能基准测试)
   - 5.20 [版本控制与历史数据](#520-版本控制与历史数据)
   - 5.21 [三维数据支持](#521-三维数据支持)
   - 5.22 [瓦片服务支持](#522-瓦片服务支持)
   - 5.23 [大数据集成](#523-大数据集成)
   - 5.24 [数据序列化与压缩](#524-数据序列化与压缩)
   - 5.25 [权限与安全](#525-权限与安全)
   - 5.26 [国际化与本地化](#526-国际化与本地化)
   - 5.27 [数据质量评估](#527-数据质量评估)
   - 5.28 [网络服务客户端](#528-网络服务客户端)
   - 5.29 [空间统计分析](#529-空间统计分析)
   - 5.30 [数据备份与恢复](#530-数据备份与恢复)
   - 5.31 [GIS库对比分析](#531-gis库对比分析)
   - 5.32 [实际应用案例](#532-实际应用案例)
6. [栅格图层与波段管理](#6-栅格图层与波段管理)
   - 6.1 [栅格数据模型](#61-栅格数据模型)
   - 6.2 [地理参考系统](#62-地理参考系统)
   - 6.3 [波段管理](#63-波段管理)
   - 6.4 [金字塔机制](#64-金字塔机制)
   - 6.5 [重采样算法](#65-重采样算法)
   - 6.6 [块缓存机制](#66-块缓存机制)
7. [空间索引与查询优化](#7-空间索引与查询优化)
   - 7.1 [空间索引概述](#71-空间索引概述)
   - 7.2 [R树索引](#72-r树索引)
   - 7.3 [四叉树索引](#73-四叉树索引)
   - 7.4 [网格索引](#74-网格索引)
   - 7.5 [索引选择策略](#75-索引选择策略)
8. [多线程并发访问策略](#8-多线程并发访问策略)
   - 8.1 [线程安全设计原则](#81-线程安全设计原则)
   - 8.2 [读写锁实现](#82-读写锁实现)
   - 8.3 [图层线程安全实现](#83-图层线程安全实现)
   - 8.4 [并发事务处理](#84-并发事务处理)
   - 8.5 [连接池设计](#85-连接池设计)
   - 8.6 [图层连接（Join）支持](#86-图层连接join支持)
   - 8.7 [PostGIS 图层管理](#87-postgis-图层管理)
   - 8.8 [批量操作优化](#88-批量操作优化)
9. [现代 C++ 实现最佳实践](#9-现代-c-实现最佳实践)
   - 9.1 [智能指针应用](#91-智能指针应用)
   - 9.2 [移动语义](#92-移动语义)
   - 9.3 [PIMPL 模式](#93-pimpl-模式)
   - 9.4 [RAII 资源管理](#94-raii-资源管理)
   - 9.5 [迭代器模式](#95-迭代器模式)
   - 9.6 [错误处理与异常安全](#96-错误处理与异常安全)
   - 9.7 [跨平台兼容性](#97-跨平台兼容性)
   - 9.8 [日志与调试支持](#98-日志与调试支持)
   - 9.9 [单元测试策略](#99-单元测试策略)
10. [设计建议与结论](#10-设计建议与结论)
   - 10.1 [核心设计建议](#101-核心设计建议)
   - 10.2 [实现优先级](#102-实现优先级)
   - 10.3 [研究结论](#103-研究结论)
11. [参考链接](#11-参考链接)
   - 11.1 [OGC 标准](#111-ogc-标准)
   - 11.2 [GDAL 文档](#112-gdal-文档)
   - 11.3 [空间索引](#113-空间索引)
   - 11.4 [C++ 最佳实践](#114-c-最佳实践)
   - 11.5 [相关技术文章](#115-相关技术文章)
   - 11.6 [数据库相关](#116-数据库相关)
   - 11.7 [连接池与并发](#117-连接池与并发)
   - 11.8 [测试与调试](#118-测试与调试)
   - 11.9 [GIS 专业参考](#119-gis-专业参考)
   - 11.10 [几何验证与拓扑](#1110-几何验证与拓扑)
   - 11.11 [样式与渲染](#1111-样式与渲染)
   - 11.12 [性能与扩展](#1112-性能与扩展)
   - 11.13 [几何算法](#1113-几何算法)
   - 11.14 [空间分析](#1114-空间分析)
   - 11.15 [数据格式规范](#1115-数据格式规范)
   - 11.16 [时态 GIS](#1116-时态-gis)
   - 11.17 [三维与点云](#1117-三维与点云)
   - 11.18 [瓦片服务](#1118-瓦片服务)
   - 11.19 [大数据与分布式](#1119-大数据与分布式)
   - 11.20 [序列化与安全](#1120-序列化与安全)

---

## 1. 研究概述

### 1.1 研究背景

CNLayer 及其派生类构成了一套完整的、现代、类型安全、线程友好的 OGC 数据访问体系。本研究旨在深入分析相关技术标准和实现方案，为图层模型设计提供坚实的技术支撑。

### 1.2 研究范围

| 研究主题 | 关键技术点 |
|----------|-----------|
| OGC 标准与规范 | Simple Feature Access、WFS、图层定义 |
| GDAL OGRLayer | 架构设计、接口实现、能力测试 |
| 内存图层 | 数据结构选择、FID 管理、缓存策略 |
| 矢量图层 | 格式抽象、空间过滤、属性查询 |
| 栅格图层 | 金字塔、重采样、波段管理 |
| 空间索引 | R树、四叉树、网格索引 |
| 多线程安全 | 读写锁、事务、并发控制 |
| 现代 C++ | 智能指针、移动语义、RAII |

### 1.3 研究方法

- 文献调研：OGC 官方标准文档、GDAL 源码分析
- 技术对比：不同实现方案的优缺点分析
- 最佳实践：业界成熟方案总结

---

## 2. OGC 图层标准与规范

### 2.1 OGC Simple Feature Access 标准

#### 2.1.1 标准概述

OGC Simple Feature Access（简单要素访问）是一系列国际标准，定义了简单地理要素的存储、访问和操作规范。该标准分为两部分：

| 部分 | 名称 | 内容 |
|------|------|------|
| Part 1 | Common Architecture | 通用架构，定义几何类型和操作 |
| Part 2 | SQL Option | SQL 实现，定义数据库存储规范 |

**参考链接**:
- [OGC Simple Feature Access - Part 1](https://www.ogc.org/standards/sfa)
- [OGC Simple Feature Access - Part 2](https://www.ogc.org/standards/sfs)

#### 2.1.2 核心概念定义

**要素（Feature）**:
> 要素是由一个几何属性和若干个非几何属性共同描述的地理实体。

**图层（Layer）**:
> 图层是具有相同要素定义（Feature Definition）的要素集合，共享相同的几何类型和属性模式。

**关键特性**:

1. **统一模式**: 图层内所有要素共享相同的 OGRFeatureDefn
2. **几何类型约束**: 图层可指定允许的几何类型
3. **空间参考系统**: 图层关联统一的空间参考系统
4. **要素标识**: 每个要素在图层内有唯一的 FID

#### 2.1.3 几何类型层次

```
Geometry (抽象基类)
├── Point (点)
├── Curve (曲线 - 抽象)
│   ├── LineString (线串)
│   │   └── LinearRing (环)
│   ├── CircularString (圆弧)
│   └── CompoundCurve (复合曲线)
├── Surface (曲面 - 抽象)
│   ├── Polygon (多边形)
│   └── CurvePolygon (曲线多边形)
├── GeometryCollection (几何集合)
│   ├── MultiPoint (多点)
│   ├── MultiCurve (多曲线)
│   │   └── MultiLineString (多线串)
│   └── MultiSurface (多曲面)
│       └── MultiPolygon (多多边形)
├── PolyhedralSurface (多面体表面)
└── TIN (不规则三角网)
```

### 2.2 OGC Web Feature Service (WFS) 规范

#### 2.2.1 WFS 核心操作

| 操作 | 描述 | HTTP 方法 |
|------|------|-----------|
| GetCapabilities | 获取服务能力描述 | GET |
| DescribeFeatureType | 获取要素类型定义 | GET |
| GetFeature | 获取要素数据 | GET/POST |
| Transaction | 事务操作（增删改） | POST |
| LockFeature | 锁定要素 | POST |

**参考链接**:
- [OGC WFS 2.0 Standard](https://www.ogc.org/standards/wfs)

#### 2.2.2 图层在 WFS 中的映射

```
WFS 服务
└── FeatureType (对应 Layer)
    ├── Name (图层名称)
    ├── Title (标题)
    ├── Abstract (摘要)
    ├── Keywords (关键词)
    ├── CRS (坐标参考系统)
    ├── WGS84BoundingBox (范围)
    └── Metadata (元数据)
```

### 2.3 OGC 要素模型核心接口

根据 OGC 规范，图层应实现以下核心接口：

```cpp
// OGC 定义的图层核心能力
interface Layer {
    // 基本信息
    string getName();
    Envelope getExtent();
    CRS getSpatialReference();
    
    // 要素定义
    FeatureType getFeatureType();
    
    // 要素访问
    FeatureCollection getFeatures(Query query);
    Feature getFeatureById(Identifier id);
    long getFeatureCount(Filter filter);
    
    // 修改操作
    void insert(Feature feature);
    void update(Feature feature);
    void delete(Identifier id);
    
    // 过滤
    void setSpatialFilter(Geometry filter);
    void setAttributeFilter(string filter);
}
```

---

## 3. GDAL OGRLayer 架构分析

### 3.1 GDAL/OGR 整体架构

#### 3.1.1 架构层次

```
┌─────────────────────────────────────────────────────────┐
│                   应用程序层                              │
└───────────────────────────┬─────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────┐
│                   GDALDataset (数据集)                   │
│         表示文件、数据库或网络服务                        │
│         包含一个或多个 OGRLayer                          │
└───────────────────────────┬─────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────┐
│                   OGRLayer (图层)                        │
│         要素的有序集合，共享相同模式                      │
│         提供读写、过滤、事务等操作                        │
└───────────────────────────┬─────────────────────────────┘
                            │
┌───────────────────────────▼─────────────────────────────┐
│                   OGRFeature (要素)                      │
│         几何对象 + 属性字段 + FID                         │
└───────────────────────────┬─────────────────────────────┘
                            │
        ┌───────────────────┴───────────────────┐
        │                                       │
┌───────▼────────┐                    ┌────────▼────────┐
│  OGRGeometry   │                    │   OGRField      │
│  (几何对象)     │                    │   (属性字段)    │
└────────────────┘                    └─────────────────┘
```

**参考链接**:
- [GDAL Vector Data Model](https://www.osgeo.cn/gdal/user/vector_data_model.html)
- [GDAL OGRLayer API](https://gdal.org/api/ogrlayer_cpp.html)

#### 3.1.2 类关系图

```
GDALDriverManager (单例)
    │
    ├── GDALDriver (格式驱动)
    │       │
    │       └── GDALDataset (数据集实例)
    │               │
    │               ├── OGRLayer (图层)
    │               │       │
    │               │       ├── OGRFeatureDefn (要素定义)
    │               │       │       ├── OGRFieldDefn (字段定义)
    │               │       │       └── OGRGeomFieldDefn (几何字段定义)
    │               │       │
    │               │       └── OGRFeature (要素实例)
    │               │               ├── OGRGeometry (几何)
    │               │               └── OGRField[] (字段值)
    │               │
    │               └── OGRSpatialReference (空间参考)
    │
    └── GDALDriver (更多驱动...)
```

### 3.2 OGRLayer 核心接口详解

#### 3.2.1 基本信息

```cpp
class OGRLayer {
public:
    // 图层名称
    virtual const char* GetName() = 0;
    
    // 几何类型
    virtual OGRwkbGeometryType GetGeomType() = 0;
    
    // 空间参考系统
    virtual OGRSpatialReference* GetSpatialRef() = 0;
    
    // 要素定义
    virtual OGRFeatureDefn* GetLayerDefn() = 0;
    
    // 图层范围
    virtual OGRErr GetExtent(OGREnvelope* psExtent, int bForce = TRUE) = 0;
    
    // 要素数量
    virtual GIntBig GetFeatureCount(int bForce = TRUE) = 0;
};
```

#### 3.2.2 要素遍历机制

GDAL 采用游标模式进行要素遍历：

```cpp
// 标准遍历模式
void TraverseFeatures(OGRLayer* poLayer) {
    // 重置读取位置
    poLayer->ResetReading();
    
    // 循环获取要素
    OGRFeature* poFeature;
    while ((poFeature = poLayer->GetNextFeature()) != nullptr) {
        // 处理要素
        ProcessFeature(poFeature);
        
        // 销毁要素
        OGRFeature::DestroyFeature(poFeature);
    }
}
```

**关键点**:
- `ResetReading()`: 重置游标到起始位置
- `GetNextFeature()`: 获取下一个要素，返回 NULL 表示结束
- 调用者负责销毁返回的要素对象

#### 3.2.3 过滤器机制

**空间过滤器**:

```cpp
// 设置矩形空间过滤
void SetSpatialFilterRect(double dfMinX, double dfMinY, 
                          double dfMaxX, double dfMaxY);

// 设置几何空间过滤
void SetSpatialFilter(OGRGeometry* poGeom);

// 多几何字段支持
void SetSpatialFilter(int iGeomField, OGRGeometry* poGeom);
```

**属性过滤器**:

```cpp
// 设置 SQL WHERE 子句
OGRErr SetAttributeFilter(const char* pszQuery);

// 示例
poLayer->SetAttributeFilter("population > 1000000 AND country = 'China'");
```

#### 3.2.4 能力测试机制

```cpp
enum OGLayerCapability {
    // 读取能力
    OLCRandomRead,          // 随机读取
    OLCSequentialWrite,     // 顺序写入
    OLCRandomWrite,         // 随机写入
    OLCFastSpatialFilter,   // 快速空间过滤
    OLCFastFeatureCount,    // 快速要素计数
    OLCFastGetExtent,       // 快速获取范围
    
    // 事务能力
    OLCTransactions,        // 事务支持
    
    // 结构修改能力
    OLCCreateField,         // 创建字段
    OLCDeleteField,         // 删除字段
    OLCReorderFields,       // 重排字段
    OLCAlterFieldDefn,      // 修改字段定义
    
    // 删除能力
    OLCDeleteFeature,       // 删除要素
    
    // 其他能力
    OLCStringsAsUTF8,       // UTF-8 字符串
    OLCIgnoreFields         // 忽略字段
};

// 测试能力
virtual int TestCapability(const char* pszCap) = 0;
```

### 3.3 GDAL 数据源与驱动架构

#### 3.3.1 驱动注册机制

```cpp
// 注册所有驱动
GDALAllRegister();

// 打开数据源
GDALDataset* poDS = (GDALDataset*)GDALOpenEx(
    "path/to/file.shp",     // 文件路径
    GDAL_OF_VECTOR,         // 数据类型
    nullptr,                // 驱动列表
    nullptr,                // 打开选项
    nullptr                 // 兄弟文件
);

// 获取图层
OGRLayer* poLayer = poDS->GetLayerByName("layer_name");

// 关闭数据源
GDALClose(poDS);
```

#### 3.3.2 支持的矢量格式

| 格式 | 扩展名 | 特点 | 事务支持 |
|------|--------|------|----------|
| Shapefile | .shp | 最广泛支持，但有字段名限制 | 否 |
| GeoJSON | .geojson | Web 友好，文本格式 | 否 |
| GeoPackage | .gpkg | OGC 标准，支持多图层 | 是 |
| PostGIS | 数据库 | 企业级，完整事务 | 是 |
| SpatiaLite | .sqlite | 轻量级空间数据库 | 是 |
| GML | .gml | OGC 标准，XML 格式 | 否 |
| KML | .kml | Google Earth 格式 | 否 |
| FileGDB | .gdb | Esri 文件地理数据库 | 部分 |

### 3.4 OGRLayer 实现模式分析

#### 3.4.1 抽象基类设计

OGRLayer 作为抽象基类，定义了所有派生类必须实现的接口：

```cpp
// 必须实现的纯虚函数
virtual void ResetReading() = 0;
virtual OGRFeature* GetNextFeature() = 0;
virtual OGRFeatureDefn* GetLayerDefn() = 0;
virtual OGRSpatialReference* GetSpatialRef() = 0;
virtual int TestCapability(const char*) = 0;
```

#### 3.4.2 默认实现模式

对于可选功能，基类提供默认实现：

```cpp
// 默认实现：遍历所有要素计数
GIntBig OGRLayer::GetFeatureCount(int bForce) {
    if (!bForce && !TestCapability(OLCFastFeatureCount)) {
        return -1;  // 无法快速获取
    }
    
    GIntBig nCount = 0;
    ResetReading();
    while (GetNextFeature() != nullptr) {
        nCount++;
    }
    ResetReading();
    return nCount;
}

// 默认实现：遍历计算范围
OGRErr OGRLayer::GetExtent(OGREnvelope* psExtent, int bForce) {
    if (!bForce && !TestCapability(OLCFastGetExtent)) {
        return OGRERR_FAILURE;
    }
    
    // 遍历所有要素计算范围
    // ...
}
```

---

## 4. 内存图层设计模式

### 4.1 数据结构选择

#### 4.1.1 要素存储结构对比

| 数据结构 | 插入 | 查找(FID) | 删除 | 遍历 | 内存效率 |
|----------|------|-----------|------|------|----------|
| `std::vector<CNFeature*>` | O(1) 摊销 | O(n) | O(n) | O(1) | 高 |
| `std::map<int64_t, CNFeature*>` | O(log n) | O(log n) | O(log n) | O(log n) | 中 |
| `std::unordered_map<int64_t, CNFeature*>` | O(1) 平均 | O(1) 平均 | O(1) 平均 | O(n) | 中 |
| `std::vector` + `std::unordered_map` | O(1) | O(1) | O(1)* | O(1) | 中 |

**推荐方案**: 混合结构

```cpp
class CNMemoryLayer {
private:
    // 顺序存储：用于遍历
    std::vector<std::unique_ptr<CNFeature>> features_;
    
    // 索引：用于快速 FID 查找
    std::unordered_map<int64_t, size_t> fid_index_;
    
    // 当前读取位置
    size_t read_cursor_ = 0;
};
```

#### 4.1.2 性能分析

**遍历性能**:
```cpp
// vector 遍历：缓存友好
for (const auto& feature : features_) {
    ProcessFeature(feature.get());
}

// unordered_map 遍历：缓存不友好
for (const auto& pair : fid_index_) {
    ProcessFeature(features_[pair.second].get());
}
```

**FID 查找性能**:
```cpp
// 纯 vector：线性查找 O(n)
CNFeature* FindByFID_Vector(int64_t fid) {
    for (const auto& f : features_) {
        if (f->GetFID() == fid) return f.get();
    }
    return nullptr;
}

// 混合结构：哈希查找 O(1)
CNFeature* FindByFID_Hybrid(int64_t fid) {
    auto it = fid_index_.find(fid);
    if (it != fid_index_.end()) {
        return features_[it->second].get();
    }
    return nullptr;
}
```

### 4.2 FID 管理策略

#### 4.2.1 自动生成策略

```cpp
class CNMemoryLayer {
private:
    int64_t next_fid_ = 1;
    bool auto_fid_generation_ = true;
    
public:
    int64_t GenerateFID() {
        while (fid_index_.find(next_fid_) != fid_index_.end()) {
            next_fid_++;
        }
        return next_fid_++;
    }
    
    void SetAutoFIDGeneration(bool auto_gen) {
        auto_fid_generation_ = auto_gen;
    }
};
```

#### 4.2.2 FID 重用策略

```cpp
class CNMemoryLayer {
private:
    std::set<int64_t> deleted_fids_;  // 已删除的 FID 池
    
public:
    int64_t GenerateFID() {
        if (!deleted_fids_.empty()) {
            int64_t reused_fid = *deleted_fids_.begin();
            deleted_fids_.erase(deleted_fids_.begin());
            return reused_fid;
        }
        return next_fid_++;
    }
    
    CNStatus DeleteFeature(int64_t fid) {
        // ... 删除逻辑 ...
        deleted_fids_.insert(fid);  // 加入重用池
        return CNStatus::kSuccess;
    }
};
```

### 4.3 内存事务实现

#### 4.3.1 事务快照机制

```cpp
class CNMemoryLayer {
private:
    struct TransactionSnapshot {
        std::vector<std::unique_ptr<CNFeature>> features;
        std::unordered_map<int64_t, size_t> fid_index;
        int64_t next_fid;
    };
    
    std::stack<TransactionSnapshot> transaction_stack_;
    bool in_transaction_ = false;
    
public:
    CNStatus StartTransaction() {
        if (in_transaction_) {
            return CNStatus::kError;
        }
        
        // 创建当前状态快照
        TransactionSnapshot snapshot;
        snapshot.next_fid = next_fid_;
        snapshot.fid_index = fid_index_;
        
        // 深拷贝要素
        for (const auto& f : features_) {
            snapshot.features.push_back(f->Clone());
        }
        
        transaction_stack_.push(std::move(snapshot));
        in_transaction_ = true;
        return CNStatus::kSuccess;
    }
    
    CNStatus CommitTransaction() {
        if (!in_transaction_) {
            return CNStatus::kError;
        }
        
        transaction_stack_.pop();
        in_transaction_ = false;
        return CNStatus::kSuccess;
    }
    
    CNStatus RollbackTransaction() {
        if (!in_transaction_) {
            return CNStatus::kError;
        }
        
        // 恢复快照
        TransactionSnapshot snapshot = std::move(transaction_stack_.top());
        transaction_stack_.pop();
        
        features_ = std::move(snapshot.features);
        fid_index_ = std::move(snapshot.fid_index);
        next_fid_ = snapshot.next_fid;
        
        in_transaction_ = false;
        return CNStatus::kSuccess;
    }
};
```

### 4.4 内存优化策略

#### 4.4.1 预分配与收缩

```cpp
class CNMemoryLayer {
public:
    CNStatus Reserve(int64_t expected_count) {
        features_.reserve(expected_count);
        return CNStatus::kSuccess;
    }
    
    CNStatus ShrinkToFit() {
        features_.shrink_to_fit();
        return CNStatus::kSuccess;
    }
    
    CNStatus GetMemoryUsage(int64_t& feature_count, int64_t& memory_bytes) const {
        feature_count = features_.size();
        memory_bytes = features_.capacity() * sizeof(std::unique_ptr<CNFeature>);
        
        for (const auto& f : features_) {
            memory_bytes += f->GetMemoryUsage();
        }
        return CNStatus::kSuccess;
    }
};
```

#### 4.4.2 延迟加载字段

```cpp
class CNFeature {
private:
    std::vector<std::unique_ptr<CNFieldValue>> field_values_;
    std::bitset<kMaxFieldCount> field_loaded_;
    
public:
    const CNFieldValue* GetField(int index) {
        if (!field_loaded_[index]) {
            LoadField(index);  // 延迟加载
            field_loaded_[index] = true;
        }
        return field_values_[index].get();
    }
};
```

---

## 5. 矢量图层与文件格式

### 5.1 格式抽象层设计

#### 5.1.1 格式枚举定义

```cpp
enum class VectorFormat {
    kUnknown = 0,
    kShapefile,
    kGeoJSON,
    kGeoPackage,
    kPostGIS,
    kSpatiaLite,
    kGML,
    kKML,
    kFileGDB,
    kWFS,
    kMemory
};
```

#### 5.1.2 格式能力矩阵

| 格式 | 事务 | 多图层 | 64位FID | Unicode字段名 | 大文件支持 |
|------|------|--------|---------|---------------|-----------|
| Shapefile | 否 | 否 | 否 | 否 | 2GB 限制 |
| GeoJSON | 否 | 否 | 是 | 是 | 内存限制 |
| GeoPackage | 是 | 是 | 是 | 是 | 是 |
| PostGIS | 是 | 是 | 是 | 是 | 是 |
| SpatiaLite | 是 | 是 | 是 | 是 | 是 |
| FileGDB | 是 | 是 | 是 | 是 | 是 |

### 5.2 空间过滤器实现

#### 5.2.1 过滤器类型

```cpp
enum class SpatialFilterType {
    kNone,          // 无过滤
    kRectangular,   // 矩形过滤
    kGeometric,     // 几何过滤
    kCombined       // 组合过滤
};

struct SpatialFilter {
    SpatialFilterType type = SpatialFilterType::kNone;
    Envelope envelope;                    // 矩形范围
    std::unique_ptr<CNGeometry> geometry; // 几何形状
    int geom_field_index = 0;             // 几何字段索引
};
```

#### 5.2.2 过滤器应用流程

```
┌─────────────────┐
│ 设置空间过滤器   │
└────────┬────────┘
         │
┌────────▼────────┐
│ 检查索引可用性   │
└────────┬────────┘
         │
    ┌────┴────┐
    │         │
┌───▼───┐ ┌───▼────┐
│有索引 │ │ 无索引  │
└───┬───┘ └───┬────┘
    │         │
┌───▼───┐ ┌───▼────┐
│索引查询│ │遍历过滤│
└───┬───┘ └───┬────┘
    │         │
    └────┬────┘
         │
┌────────▼────────┐
│ 返回结果迭代器   │
└─────────────────┘
```

### 5.3 属性过滤器与 SQL 解析

#### 5.3.1 过滤器表达式

```cpp
// 支持的 SQL 子集
// 比较运算: =, !=, <, >, <=, >=
// 逻辑运算: AND, OR, NOT
// 空值判断: IS NULL, IS NOT NULL
// 字符串匹配: LIKE, NOT LIKE
// 范围判断: IN, BETWEEN
// 算术运算: +, -, *, /

// 示例
layer->SetAttributeFilter("population > 1000000 AND country = 'China'");
layer->SetAttributeFilter("name LIKE 'Beijing%'");
layer->SetAttributeFilter("id IN (1, 2, 3, 4, 5)");
```

#### 5.3.2 表达式解析树

```cpp
struct FilterNode {
    enum Type {
        kComparison,    // 比较运算
        kLogical,       // 逻辑运算
        kArithmetic,    // 算术运算
        kField,         // 字段引用
        kLiteral        // 字面量
    };
    
    Type type;
    std::string value;
    std::vector<std::unique_ptr<FilterNode>> children;
};

class FilterParser {
public:
    std::unique_ptr<FilterNode> Parse(const std::string& filter);
    bool Evaluate(const FilterNode* node, const CNFeature* feature);
};
```

### 5.4 缓存管理策略

#### 5.4.1 LRU 缓存实现

```cpp
template<typename Key, typename Value>
class LRUCache {
public:
    explicit LRUCache(size_t capacity) : capacity_(capacity) {}
    
    Value* Get(const Key& key) {
        auto it = cache_map_.find(key);
        if (it == cache_map_.end()) {
            return nullptr;
        }
        
        // 移动到最前
        cache_list_.splice(cache_list_.begin(), cache_list_, it->second);
        return &it->second->second;
    }
    
    void Put(const Key& key, Value value) {
        auto it = cache_map_.find(key);
        if (it != cache_map_.end()) {
            it->second->second = std::move(value);
            cache_list_.splice(cache_list_.begin(), cache_list_, it->second);
            return;
        }
        
        if (cache_map_.size() >= capacity_) {
            // 删除最久未使用
            auto last = cache_list_.back();
            cache_map_.erase(last.first);
            cache_list_.pop_back();
        }
        
        cache_list_.emplace_front(key, std::move(value));
        cache_map_[key] = cache_list_.begin();
    }
    
private:
    size_t capacity_;
    std::list<std::pair<Key, Value>> cache_list_;
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> cache_map_;
};
```

#### 5.4.2 图层缓存策略

```cpp
class CNVectorLayer {
private:
    LRUCache<int64_t, std::unique_ptr<CNFeature>> feature_cache_;
    
public:
    CNFeature* GetFeature(int64_t fid) {
        // 先查缓存
        auto cached = feature_cache_.Get(fid);
        if (cached && *cached) {
            return (*cached).get();
        }
        
        // 从数据源读取
        auto feature = ReadFeatureFromSource(fid);
        if (feature) {
            feature_cache_.Put(fid, std::move(feature));
            return feature_cache_.Get(fid)->get();
        }
        return nullptr;
    }
};
```

### 5.5 坐标参考系统(CRS)处理

#### 5.5.1 CRS 核心概念

坐标参考系统是 GIS 的核心基础，决定了空间数据的定位精度和互操作性。

```cpp
class CNSpatialReference {
public:
    // CRS 类型
    enum class CRSType {
        kGeographic,    // 地理坐标系 (经纬度)
        kProjected,     // 投影坐标系 (平面坐标)
        kGeocentric,    // 地心坐标系
        kVertical,      // 垂直坐标系
        kCompound,      // 复合坐标系
        kUnknown
    };
    
    // 基本信息
    std::string GetName() const;
    std::string GetAuthority() const;      // EPSG, ESRI, etc.
    std::string GetAuthorityCode() const;  // EPSG:4326 -> "4326"
    CRSType GetType() const;
    
    // WKT 表示
    std::string ToWKT() const;
    static std::unique_ptr<CNSpatialReference> FromWKT(const std::string& wkt);
    
    // PROJ 字符串
    std::string ToProjString() const;
    static std::unique_ptr<CNSpatialReference> FromProjString(const std::string& proj);
    
    // EPSG 代码
    static std::unique_ptr<CNSpatialReference> FromEPSG(int epsg_code);
    int GetEPSGCode() const;
    
    // 坐标系参数
    struct GeographicCRS {
        std::string datum;          // WGS84, NAD83, etc.
        std::string ellipsoid;      // GRS80, WGS84, etc.
        double semi_major_axis;     // 长半轴
        double inverse_flattening;  // 扁率倒数
        double prime_meridian;      // 本初子午线 (通常为 0)
    };
    
    struct ProjectedCRS {
        GeographicCRS base_crs;
        std::string projection;     // Mercator, UTM, etc.
        std::map<std::string, double> parameters;
        double false_easting;
        double false_northing;
        double central_meridian;
        double latitude_of_origin;
        double scale_factor;
    };
    
    // 坐标单位
    enum class Unit {
        kDegree,
        kMeter,
        kFoot,
        kUSFoot,
        kRadian,
        kUnknown
    };
    Unit GetLinearUnit() const;
    Unit GetAngularUnit() const;
    double GetUnitToMeter() const;  // 单位到米的转换系数
    
    // 轴顺序 (OGC 标准中重要概念)
    enum class AxisOrder {
        kXY,    // 东向, 北向 (大多数投影坐标系)
        kYX,    // 北向, 东向 (EPSG:4326 标准)
        kTraditional  // 传统顺序 (GDAL 默认)
    };
    AxisOrder GetAxisOrder() const;
    
    // 范围
    struct Extent {
        double west, south, east, north;
        std::string description;
    };
    bool GetExtent(Extent& extent) const;
    
    // 比较
    bool IsSame(const CNSpatialReference& other) const;
    bool IsGeographic() const { return type_ == CRSType::kGeographic; }
    bool IsProjected() const { return type_ == CRSType::kProjected; }
    
private:
    CRSType type_;
    std::string name_;
    std::string authority_;
    std::string authority_code_;
    void* proj_handle_;  // PROJ 库句柄
};
```

#### 5.5.2 坐标转换

```cpp
class CNCoordinateTransform {
public:
    // 创建转换器
    static std::unique_ptr<CNCoordinateTransform> Create(
        const CNSpatialReference& source_crs,
        const CNSpatialReference& target_crs
    );
    
    // 点转换
    CNStatus Transform(double& x, double& y, double& z = dummy_z_);
    CNStatus Transform(double* x, double* y, double* z, size_t count);
    
    // 几何对象转换
    CNStatus Transform(CNGeometry* geometry);
    
    // 批量转换 (高性能)
    CNStatus TransformBatch(
        const double* src_x, const double* src_y, 
        double* dst_x, double* dst_y, 
        size_t count
    );
    
    // 转换精度信息
    double GetAccuracy() const;
    bool IsPrecise() const;
    
    // 反向转换
    CNCoordinateTransform* Inverse() const;
    
    // 转换边界框 (注意：转换后的矩形可能不是矩形)
    CNStatus TransformBounds(
        double min_x, double min_y, double max_x, double max_y,
        double& out_min_x, double& out_min_y, double& out_max_x, double& out_max_y
    );
    
private:
    CNSpatialReference source_crs_;
    CNSpatialReference target_crs_;
    void* transform_handle_;  // PROJ PJ* 句柄
    static double dummy_z_;
};

// 图层级别的坐标转换
class CNLayer {
public:
    CNStatus TransformTo(const CNSpatialReference& target_crs) {
        auto transform = CNCoordinateTransform::Create(
            *GetSpatialReference(), target_crs);
        if (!transform) {
            return CNStatus::kError;
        }
        
        // 更新所有几何对象
        ResetReading();
        while (CNFeature* feature = GetNextFeature()) {
            CNStatus status = transform->Transform(
                feature->GetGeometry());
            if (status != CNStatus::kSuccess) {
                return status;
            }
        }
        
        // 更新图层 CRS
        spatial_ref_ = target_crs.Clone();
        return CNStatus::kSuccess;
    }
};
```

#### 5.5.3 轴顺序处理

OGC 标准与 EPSG 对轴顺序的定义存在差异，这是常见的互操作问题：

```cpp
// EPSG:4326 标准: 纬度(Y), 经度(X)
// 传统 GIS 软件: 经度(X), 纬度(Y)

class CNSpatialReference {
public:
    // 规范化坐标顺序 (遵循 OGC 标准)
    void NormalizeAxisOrder(bool strict_ogc = false) {
        if (strict_ogc && IsGeographic()) {
            // 地理坐标系: 纬度在前
            axis_order_ = AxisOrder::kYX;
        } else {
            // 传统顺序: 经度在前
            axis_order_ = AxisOrder::kTraditional;
        }
    }
    
    // 坐标顺序转换
    static void SwapCoordinates(double& x, double& y) {
        std::swap(x, y);
    }
};

// GDAL 风格的轴顺序处理
class CNLayer {
public:
    void SetAxisMappingStrategy(AxisMappingStrategy strategy) {
        axis_strategy_ = strategy;
    }
    
    enum class AxisMappingStrategy {
        kTraditionalGIS,  // 经度在前
        kOgcCompliant,    // 遵循 CRS 定义
        kAuto             // 自动检测
    };
    
private:
    AxisMappingStrategy axis_strategy_ = AxisMappingStrategy::kTraditionalGIS;
};
```

### 5.6 空间关系谓词

#### 5.6.1 OGC DE-9IM 模型

OGC 定义了基于维度扩展的九交模型(DE-9IM)来描述空间关系：

```cpp
// DE-9IM 矩阵
//     I(a)  B(a)  E(a)
// I(b)  II    IB    IE
// B(b)  BI    BB    BE
// E(b)  EI    EB    EE
//
// I = Interior (内部)
// B = Boundary (边界)
// E = Exterior (外部)

class CNSpatialRelation {
public:
    // 基本空间关系谓词
    static bool Equals(const CNGeometry* a, const CNGeometry* b);
    static bool Disjoint(const CNGeometry* a, const CNGeometry* b);
    static bool Intersects(const CNGeometry* a, const CNGeometry* b);
    static bool Touches(const CNGeometry* a, const CNGeometry* b);
    static bool Crosses(const CNGeometry* a, const CNGeometry* b);
    static bool Within(const CNGeometry* a, const CNGeometry* b);
    static bool Contains(const CNGeometry* a, const CNGeometry* b);
    static bool Overlaps(const CNGeometry* a, const CNGeometry* b);
    
    // DE-9IM 矩阵计算
    struct DE9IM {
        char matrix[9];  // 'T', 'F', '0', '1', '2', '*'
        
        // 矩阵元素
        char II() const { return matrix[0]; }  // 内部-内部
        char IB() const { return matrix[1]; }  // 内部-边界
        char IE() const { return matrix[2]; }  // 内部-外部
        char BI() const { return matrix[3]; }  // 边界-内部
        char BB() const { return matrix[4]; }  // 边界-边界
        char BE() const { return matrix[5]; }  // 边界-外部
        char EI() const { return matrix[6]; }  // 外部-内部
        char EB() const { return matrix[7]; }  // 外部-边界
        char EE() const { return matrix[8]; }  // 外部-外部
    };
    
    static DE9IM Relate(const CNGeometry* a, const CNGeometry* b);
    
    // 自定义关系匹配
    static bool RelatePattern(const CNGeometry* a, const CNGeometry* b, 
                              const std::string& pattern);
    
    // 距离计算
    static double Distance(const CNGeometry* a, const CNGeometry* b);
    
    // 缓冲区
    static std::unique_ptr<CNGeometry> Buffer(
        const CNGeometry* geom, double distance, int segments = 8);
    
    // 凸包
    static std::unique_ptr<CNGeometry> ConvexHull(const CNGeometry* geom);
    
    // 交集
    static std::unique_ptr<CNGeometry> Intersection(
        const CNGeometry* a, const CNGeometry* b);
    
    // 并集
    static std::unique_ptr<CNGeometry> Union(
        const CNGeometry* a, const CNGeometry* b);
    
    // 差集
    static std::unique_ptr<CNGeometry> Difference(
        const CNGeometry* a, const CNGeometry* b);
    
    // 对称差集
    static std::unique_ptr<CNGeometry> SymDifference(
        const CNGeometry* a, const CNGeometry* b);
};
```

#### 5.6.2 空间查询实现

```cpp
class CNLayer {
public:
    // 空间选择
    std::vector<CNFeature*> SelectByLocation(
        const CNGeometry* filter_geom,
        const std::string& predicate = "intersects"
    ) {
        std::vector<CNFeature*> results;
        
        // 使用空间索引加速
        if (spatial_index_ && TestCapability(OLCFastSpatialFilter)) {
            auto candidate_fids = spatial_index_->Query(filter_geom->GetEnvelope());
            for (int64_t fid : candidate_fids) {
                CNFeature* feature = GetFeature(fid);
                if (EvaluateSpatialPredicate(feature->GetGeometry(), 
                                              filter_geom, predicate)) {
                    results.push_back(feature);
                }
            }
        } else {
            // 全表扫描
            ResetReading();
            while (CNFeature* feature = GetNextFeature()) {
                if (EvaluateSpatialPredicate(feature->GetGeometry(), 
                                              filter_geom, predicate)) {
                    results.push_back(feature);
                }
            }
        }
        
        return results;
    }
    
private:
    bool EvaluateSpatialPredicate(
        const CNGeometry* geom, 
        const CNGeometry* filter,
        const std::string& predicate
    ) {
        if (predicate == "intersects") {
            return CNSpatialRelation::Intersects(geom, filter);
        } else if (predicate == "contains") {
            return CNSpatialRelation::Contains(filter, geom);
        } else if (predicate == "within") {
            return CNSpatialRelation::Within(geom, filter);
        } else if (predicate == "crosses") {
            return CNSpatialRelation::Crosses(geom, filter);
        } else if (predicate == "touches") {
            return CNSpatialRelation::Touches(geom, filter);
        } else if (predicate == "overlaps") {
            return CNSpatialRelation::Overlaps(geom, filter);
        } else if (predicate == "disjoint") {
            return CNSpatialRelation::Disjoint(geom, filter);
        }
        return false;
    }
};
```

### 5.7 大数据量处理策略

#### 5.7.1 分块加载

```cpp
class CNLayer {
public:
    struct TileRequest {
        int min_x, min_y, max_x, max_y;
        int level;  // LOD 级别
    };
    
    // 分块读取
    class TileIterator {
    public:
        TileIterator(CNLayer* layer, const TileRequest& request)
            : layer_(layer), request_(request) {}
        
        CNFeature* Next() {
            while (true) {
                // 当前块内遍历
                if (current_tile_features_ && 
                    tile_index_ < current_tile_features_->size()) {
                    return (*current_tile_features_)[tile_index_++];
                }
                
                // 加载下一块
                if (!LoadNextTile()) {
                    return nullptr;
                }
            }
        }
        
    private:
        bool LoadNextTile();
        
        CNLayer* layer_;
        TileRequest request_;
        std::vector<CNFeature*>* current_tile_features_ = nullptr;
        size_t tile_index_ = 0;
    };
    
    // 空间分块策略
    struct TilingScheme {
        int tile_width = 256;    // 像素
        int tile_height = 256;
        int min_level = 0;
        int max_level = 18;
        Envelope bounds;
    };
    
    void SetTilingScheme(const TilingScheme& scheme);
    TileIterator CreateTileIterator(const TileRequest& request);
};
```

#### 5.7.2 流式处理

```cpp
// 流式读取器 - 适用于大数据文件
class CNStreamingReader {
public:
    CNStreamingReader(const std::string& filename, size_t buffer_size = 10000)
        : buffer_size_(buffer_size) {}
    
    // 设置处理回调
    void SetFeatureCallback(
        std::function<bool(CNFeature*)> callback
    ) {
        feature_callback_ = std::move(callback);
    }
    
    // 执行流式处理
    CNStatus Process() {
        std::vector<std::unique_ptr<CNFeature>> buffer;
        buffer.reserve(buffer_size_);
        
        while (true) {
            CNFeature* feature = ReadNextFeature();
            if (!feature) break;
            
            buffer.emplace_back(feature);
            
            if (buffer.size() >= buffer_size_) {
                ProcessBuffer(buffer);
                buffer.clear();
            }
        }
        
        // 处理剩余数据
        if (!buffer.empty()) {
            ProcessBuffer(buffer);
        }
        
        return CNStatus::kSuccess;
    }
    
private:
    void ProcessBuffer(std::vector<std::unique_ptr<CNFeature>>& buffer) {
        for (auto& feature : buffer) {
            if (feature_callback_) {
                if (!feature_callback_(feature.get())) {
                    break;  // 用户请求停止
                }
            }
        }
    }
    
    CNFeature* ReadNextFeature();
    
    size_t buffer_size_;
    std::function<bool(CNFeature*)> feature_callback_;
};

// 使用示例：处理大型 Shapefile
void ProcessLargeShapefile(const std::string& filename) {
    CNStreamingReader reader(filename, 5000);
    
    int64_t count = 0;
    reader.SetFeatureCallback([&count](CNFeature* feature) {
        // 处理每个要素
        ProcessFeature(feature);
        count++;
        
        // 每 10000 个要素报告进度
        if (count % 10000 == 0) {
            std::cout << "Processed: " << count << " features" << std::endl;
        }
        
        return true;  // 继续处理
    });
    
    reader.Process();
}
```

#### 5.7.3 LOD (Level of Detail) 支持

```cpp
class CNLODLayer {
public:
    struct LODLevel {
        int level;
        double min_scale;
        double max_scale;
        double simplification_tolerance;
        std::unique_ptr<CNLayer> layer;
    };
    
    // 添加 LOD 级别
    void AddLODLevel(int level, double min_scale, double max_scale, 
                     double tolerance);
    
    // 根据比例尺获取合适的图层
    CNLayer* GetLayerForScale(double scale) {
        for (auto& lod : lod_levels_) {
            if (scale >= lod.min_scale && scale < lod.max_scale) {
                return lod.layer.get();
            }
        }
        return nullptr;
    }
    
    // 自动生成 LOD 级别
    CNStatus GenerateLODLevels(int levels, double base_tolerance = 0.0001) {
        for (int i = 0; i < levels; ++i) {
            double tolerance = base_tolerance * std::pow(2, i);
            double min_scale = std::pow(2, i) * 1000;
            double max_scale = (i == levels - 1) ? 
                std::numeric_limits<double>::max() : min_scale * 2;
            
            AddLODLevel(i, min_scale, max_scale, tolerance);
        }
        return CNStatus::kSuccess;
    }
    
private:
    std::vector<LODLevel> lod_levels_;
};
```

### 5.8 图层组实现

```cpp
class CNrGroupLaye : public CNLayer {
public:
    // 添加图层
    CNStatus AddLayer(std::unique_ptr<CNLayer> layer) {
        layers_.push_back(std::move(layer));
        return CNStatus::kSuccess;
    }
    
    // 移除图层
    CNStatus RemoveLayer(const std::string& name) {
        auto it = std::find_if(layers_.begin(), layers_.end(),
            [&name](const auto& l) { return l->GetName() == name; });
        if (it != layers_.end()) {
            layers_.erase(it);
            return CNStatus::kSuccess;
        }
        return CNStatus::kNotFound;
    }
    
    // 获取图层
    CNLayer* GetLayer(const std::string& name) {
        auto it = std::find_if(layers_.begin(), layers_.end(),
            [&name](const auto& l) { return l->GetName() == name; });
        return it != layers_.end() ? it->get() : nullptr;
    }
    
    // 图层顺序
    CNStatus MoveLayer(const std::string& name, int new_index);
    CNStatus MoveLayerUp(const std::string& name);
    CNStatus MoveLayerDown(const std::string& name);
    
    // 图层可见性
    void SetLayerVisible(const std::string& name, bool visible);
    bool IsLayerVisible(const std::string& name) const;
    
    // 合并所有图层要素
    std::vector<CNFeature*> GetAllFeatures() {
        std::vector<CNFeature*> all_features;
        for (auto& layer : layers_) {
            if (layer_visible_[layer->GetName()]) {
                layer->ResetReading();
                while (CNFeature* f = layer->GetNextFeature()) {
                    all_features.push_back(f);
                }
            }
        }
        return all_features;
    }
    
    // 重写基类方法
    int64_t GetFeatureCount(bool force = true) override {
        int64_t count = 0;
        for (const auto& layer : layers_) {
            if (layer_visible_[layer->GetName()]) {
                count += layer->GetFeatureCount(force);
            }
        }
        return count;
    }
    
    Envelope GetExtent(bool force = true) override {
        Envelope extent;
        for (const auto& layer : layers_) {
            if (layer_visible_[layer->GetName()]) {
                Envelope layer_extent = layer->GetExtent(force);
                extent.ExpandToInclude(layer_extent);
            }
        }
        return extent;
    }
    
private:
    std::vector<std::unique_ptr<CNLayer>> layers_;
    std::map<std::string, bool> layer_visible_;
};
```

### 5.9 几何有效性验证

#### 5.9.1 OGC 简单要素有效性规则

```cpp
class CNGeometryValidator {
public:
    struct ValidationResult {
        bool is_valid;
        std::string reason;
        CNGeometry* error_location;
        int error_code;
    };
    
    // OGC 简单要素有效性规则
    enum class ValidationRule {
        kOGCSimple,      // OGC 简单要素标准
        kOGCStrict,      // OGC 严格模式
        kESRI,           // ESRI 兼容模式
        kCustom          // 自定义规则
    };
    
    static ValidationResult Validate(const CNGeometry* geom, 
                                     ValidationRule rule = ValidationRule::kOGCSimple);
    
    // 点有效性
    static bool IsValidPoint(const CNPoint* point) {
        return std::isfinite(point->GetX()) && std::isfinite(point->GetY());
    }
    
    // 线有效性
    static ValidationResult IsValidLineString(const CNLineString* line) {
        ValidationResult result;
        result.is_valid = true;
        
        if (line->GetPointCount() < 2) {
            result.is_valid = false;
            result.reason = "LineString must have at least 2 points";
            result.error_code = 1;
            return result;
        }
        
        // 检查重复点
        for (size_t i = 1; i < line->GetPointCount(); ++i) {
            const CNPoint& p0 = line->GetPoint(i - 1);
            const CNPoint& p1 = line->GetPoint(i);
            if (p0.Equals(p1)) {
                result.is_valid = false;
                result.reason = "Duplicate consecutive points";
                result.error_code = 2;
                return result;
            }
        }
        
        return result;
    }
    
    // 多边形有效性
    static ValidationResult IsValidPolygon(const CNPolygon* polygon) {
        ValidationResult result;
        result.is_valid = true;
        
        // 外环必须有效
        const CNLinearRing* exterior = polygon->GetExteriorRing();
        if (!exterior || exterior->GetPointCount() < 4) {
            result.is_valid = false;
            result.reason = "Exterior ring must have at least 4 points";
            result.error_code = 3;
            return result;
        }
        
        // 外环必须闭合
        if (!exterior->IsClosed()) {
            result.is_valid = false;
            result.reason = "Exterior ring must be closed";
            result.error_code = 4;
            return result;
        }
        
        // 外环必须顺时针
        if (exterior->GetArea() < 0) {
            result.is_valid = false;
            result.reason = "Exterior ring must be clockwise";
            result.error_code = 5;
            return result;
        }
        
        // 检查内环
        for (int i = 0; i < polygon->GetInteriorRingCount(); ++i) {
            const CNLinearRing* interior = polygon->GetInteriorRing(i);
            
            // 内环必须逆时针
            if (interior->GetArea() > 0) {
                result.is_valid = false;
                result.reason = "Interior ring must be counter-clockwise";
                result.error_code = 6;
                return result;
            }
            
            // 内环必须在外环内部
            if (!exterior->Contains(interior)) {
                result.is_valid = false;
                result.reason = "Interior ring must be inside exterior ring";
                result.error_code = 7;
                return result;
            }
            
            // 内环之间不能相交
            for (int j = i + 1; j < polygon->GetInteriorRingCount(); ++j) {
                const CNLinearRing* other = polygon->GetInteriorRing(j);
                if (interior->Intersects(other)) {
                    result.is_valid = false;
                    result.reason = "Interior rings must not intersect";
                    result.error_code = 8;
                    return result;
                }
            }
        }
        
        return result;
    }
    
    // 自相交检测
    static bool HasSelfIntersection(const CNLineString* line);
    static bool HasSelfIntersection(const CNPolygon* polygon);
    
    // 几何修复
    static std::unique_ptr<CNGeometry> MakeValid(const CNGeometry* geom);
    static std::unique_ptr<CNGeometry> BufferZero(const CNGeometry* geom);
};
```

#### 5.9.2 图层级别验证

```cpp
class CNLayer {
public:
    struct ValidationReport {
        int64_t total_features;
        int64_t valid_features;
        int64_t invalid_features;
        std::vector<std::pair<int64_t, std::string>> errors;
    };
    
    ValidationReport ValidateGeometries(
        CNGeometryValidator::ValidationRule rule =
            CNGeometryValidator::ValidationRule::kOGCSimple,
        std::function<bool(double)> progress = nullptr
    ) {
        ValidationReport report;
        report.total_features = 0;
        report.valid_features = 0;
        report.invalid_features = 0;
        
        ResetReading();
        CNFeature* feature;
        while ((feature = GetNextFeature()) != nullptr) {
            report.total_features++;
            
            auto result = CNGeometryValidator::Validate(
                feature->GetGeometry(), rule);
            
            if (result.is_valid) {
                report.valid_features++;
            } else {
                report.invalid_features++;
                report.errors.emplace_back(
                    feature->GetFID(), result.reason);
            }
            
            if (progress && !progress(
                static_cast<double>(report.total_features) / 
                GetFeatureCount())) {
                break;
            }
        }
        
        return report;
    }
};
```

### 5.10 拓扑操作

#### 5.10.1 拓扑关系检查

```cpp
class CNTopologyValidator {
public:
    struct TopologyRule {
        enum Type {
            kMustNotOverlap,           // 不能重叠
            kMustNotHaveGaps,          // 不能有空隙
            kMustBeCoveredBy,          // 必须被覆盖
            kMustCover,                // 必须覆盖
            kMustNotIntersect,         // 不能相交
            kMustBeInside,             // 必须在内部
            kMustNotHaveDangles,       // 不能有悬挂
            kMustNotHavePseudoNodes,   // 不能有伪节点
            kMustBeSinglePart,         // 必须是单部分
            kMustNotSelfIntersect      // 不能自相交
        };
        
        Type type;
        CNLayer* layer1;
        CNLayer* layer2;  // 可选，某些规则需要两个图层
        double tolerance;
    };
    
    struct TopologyError {
        int64_t fid1;
        int64_t fid2;  // 可选
        TopologyRule::Type rule_type;
        std::unique_ptr<CNGeometry> error_geometry;
        std::string description;
    };
    
    // 执行拓扑检查
    static std::vector<TopologyError> Validate(
        const TopologyRule& rule,
        std::function<bool(double)> progress = nullptr
    );
    
    // 重叠检测
    static std::vector<TopologyError> CheckOverlap(
        CNLayer* layer, double tolerance = 0.0
    );
    
    // 空隙检测
    static std::vector<TopologyError> CheckGaps(
        CNLayer* layer, double tolerance = 0.0
    );
    
    // 悬挂节点检测
    static std::vector<TopologyError> CheckDangles(
        CNLayer* layer, double tolerance = 0.0
    );
};
```

#### 5.10.2 拓扑修复

```cpp
class CNTopologyFixer {
public:
    // 合并重叠区域
    static CNStatus MergeOverlaps(
        CNLayer* layer,
        const std::vector<CNTopologyValidator::TopologyError>& errors
    );
    
    // 填充空隙
    static CNStatus FillGaps(
        CNLayer* layer,
        const std::vector<CNTopologyValidator::TopologyError>& errors
    );
    
    // 延伸线到交点
    static CNStatus ExtendLines(
        CNLayer* layer,
        const std::vector<CNTopologyValidator::TopologyError>& errors
    );
    
    // 捕捉到几何
    static CNStatus SnapToGeometry(
        CNLayer* layer,
        const CNGeometry* reference,
        double tolerance
    );
    
    // 简化几何
    static CNStatus Simplify(
        CNLayer* layer,
        double tolerance,
        bool preserve_topology = true
    );
};
```

### 5.11 元数据管理

#### 5.11.1 ISO 19115 元数据标准

```cpp
class CNLayerMetadata {
public:
    // 标识信息
    struct IdentificationInfo {
        std::string title;
        std::string abstract;
        std::string purpose;
        std::string status;
        std::vector<std::string> keywords;
        std::string topic_category;
        std::vector<std::string> graphic_overview_urls;
    };
    
    // 数据质量信息
    struct DataQualityInfo {
        std::string scope;
        std::string lineage;
        std::vector<std::string> processing_steps;
        double positional_accuracy;
        double thematic_accuracy;
        double temporal_accuracy;
    };
    
    // 空间表示信息
    struct SpatialRepresentationInfo {
        std::string geometry_type;
        int coordinate_dimension;
        std::string spatial_resolution;
        std::string encoding;
        bool has_topology;
    };
    
    // 参考系统信息
    struct ReferenceSystemInfo {
        std::string crs_name;
        std::string crs_code;
        std::string datum;
        std::string ellipsoid;
        std::string projection;
    };
    
    // 范围信息
    struct ExtentInfo {
        double west_bound_longitude;
        double east_bound_longitude;
        double south_bound_latitude;
        double north_bound_latitude;
        std::string geographic_description;
    };
    
    // 分发信息
    struct DistributionInfo {
        std::string format_name;
        std::string format_version;
        std::string transfer_size;
        std::vector<std::string> online_resources;
    };
    
    // 获取/设置方法
    const IdentificationInfo& GetIdentificationInfo() const;
    void SetIdentificationInfo(const IdentificationInfo& info);
    
    const DataQualityInfo& GetDataQualityInfo() const;
    void SetDataQualityInfo(const DataQualityInfo& info);
    
    // XML 导入导出
    std::string ToXML() const;
    static std::unique_ptr<CNLayerMetadata> FromXML(const std::string& xml);
    
    // JSON 导入导出
    std::string ToJSON() const;
    static std::unique_ptr<CNLayerMetadata> FromJSON(const std::string& json);
    
private:
    IdentificationInfo identification_;
    DataQualityInfo quality_;
    SpatialRepresentationInfo spatial_;
    ReferenceSystemInfo reference_;
    ExtentInfo extent_;
    DistributionInfo distribution_;
};
```

### 5.12 图层样式定义

#### 5.12.1 SLD (Styled Layer Descriptor) 支持

```cpp
class CNLayerStyle {
public:
    // 符号类型
    enum class SymbolType {
        kPoint,
        kLine,
        kPolygon,
        kText,
        kRaster
    };
    
    // 点符号
    struct PointSymbol {
        std::string mark_type;  // circle, square, triangle, etc.
        double size;
        double rotation;
        CNColor fill_color;
        CNColor stroke_color;
        double stroke_width;
        std::string icon_url;  // 外部图标
    };
    
    // 线符号
    struct LineSymbol {
        CNColor color;
        double width;
        std::string dash_pattern;  // "5 2 1 2"
        std::string cap;          // butt, round, square
        std::string join;         // miter, round, bevel
        double offset;
    };
    
    // 多边形符号
    struct PolygonSymbol {
        CNColor fill_color;
        double fill_opacity;
        CNColor stroke_color;
        double stroke_width;
        std::string fill_pattern;  // solid, hatched, etc.
        double stroke_opacity;
    };
    
    // 文本符号
    struct TextSymbol {
        std::string field_name;
        std::string font_family;
        double font_size;
        CNColor color;
        bool bold;
        bool italic;
        double halo_radius;
        CNColor halo_color;
        std::string placement;  // point, line, interior
    };
    
    // 栅格符号
    struct RasterSymbol {
        double opacity;
        std::string color_ramp_name;
        std::vector<std::pair<double, CNColor>> color_map_entries;
        std::string resampling;  // nearest, bilinear, cubic
    };
    
    // 规则
    struct Rule {
        std::string name;
        std::string filter;  // OGC Filter 表达式
        double min_scale;
        double max_scale;
        std::vector<std::unique_ptr<Symbol>> symbols;
    };
    
    // 添加规则
    void AddRule(std::unique_ptr<Rule> rule);
    
    // 获取适用规则
    std::vector<const Rule*> GetApplicableRules(
        const CNFeature* feature,
        double scale
    ) const;
    
    // SLD 导入导出
    std::string ToSLD() const;
    static std::unique_ptr<CNLayerStyle> FromSLD(const std::string& sld);
    
    // QML 导入导出
    std::string ToQML() const;
    static std::unique_ptr<CNLayerStyle> FromQML(const std::string& qml);
    
private:
    std::string name_;
    std::string title_;
    std::string abstract_;
    std::vector<std::unique_ptr<Rule>> rules_;
};
```

#### 5.12.2 渲染器类型

```cpp
enum class CNRendererType {
    kSingleSymbol,      // 单一符号
    kCategorized,       // 分类渲染
    kGraduated,         // 分级渲染
    kRuleBased,         // 规则渲染
    kHeatmap,           // 热力图
    kInvertedPolygon,   // 反转多边形
    kPointDisplacement, // 点位移
    k25D                // 2.5D 渲染
};

class CNRenderer {
public:
    virtual ~CNRenderer() = default;
    
    virtual CNRendererType GetType() const = 0;
    virtual std::unique_ptr<CNLayerStyle::Rule> GetRuleForFeature(
        const CNFeature* feature) const = 0;
    
    // 分类渲染器
    static std::unique_ptr<CNRenderer> CreateCategorizedRenderer(
        const std::string& field_name,
        const std::vector<std::pair<std::string, CNLayerStyle::Symbol*>>& categories
    );
    
    // 分级渲染器
    static std::unique_ptr<CNRenderer> CreateGraduatedRenderer(
        const std::string& field_name,
        int classes,
        const std::string& classification_method,  // equal, quantile, jenks
        const CNColorRamp& color_ramp
    );
};
```

### 5.13 内存映射文件支持

#### 5.13.1 大文件处理

```cpp
class CNMemoryMappedFile {
public:
    enum class AccessMode {
        kReadOnly,
        kReadWrite,
        kCopyOnWrite
    };
    
    CNMemoryMappedFile(const std::string& filename, AccessMode mode);
    ~CNMemoryMappedFile();
    
    // 映射区域
    void* MapRegion(int64_t offset, int64_t size);
    void UnmapRegion(void* ptr, int64_t size);
    
    // 同步到磁盘
    void Sync();
    
    // 文件大小
    int64_t GetFileSize() const { return file_size_; }
    
    // 是否有效
    bool IsValid() const { return mapped_ptr_ != nullptr; }
    
private:
#ifdef CN_PLATFORM_WINDOWS
    HANDLE file_handle_;
    HANDLE mapping_handle_;
#else
    int file_descriptor_;
#endif
    void* mapped_ptr_;
    int64_t file_size_;
    AccessMode mode_;
};

// 内存映射图层
class CNMappedLayer : public CNVectorLayer {
public:
    CNStatus Open(const std::string& filename) {
        // 使用内存映射打开大文件
        mapped_file_ = std::make_unique<CNMemoryMappedFile>(
            filename, CNMemoryMappedFile::AccessMode::kReadOnly);
        
        if (!mapped_file_->IsValid()) {
            return CNStatus::kIOError;
        }
        
        // 解析文件头和索引
        return ParseHeaderAndIndex();
    }
    
    CNFeature* GetFeature(int64_t fid) override {
        // 从映射内存直接读取
        int64_t offset = feature_offset_index_[fid];
        return ParseFeatureFromMemory(
            static_cast<uint8_t*>(mapped_file_->MapRegion(offset, -1)));
    }
    
private:
    std::unique_ptr<CNMemoryMappedFile> mapped_file_;
    std::unordered_map<int64_t, int64_t> feature_offset_index_;
};
```

### 5.14 异步 I/O 设计

#### 5.14.1 异步读取器

```cpp
class CNAsyncReader {
public:
    struct ReadRequest {
        int64_t fid;
        std::function<void(CNFeature*, CNStatus)> callback;
    };
    
    CNAsyncReader(CNLayer* layer, int thread_count = 4);
    ~CNAsyncReader();
    
    // 异步读取单个要素
    std::future<CNFeature*> GetFeatureAsync(int64_t fid);
    
    // 批量异步读取
    void GetFeaturesAsync(
        const std::vector<int64_t>& fids,
        std::function<void(int64_t, CNFeature*, CNStatus)> callback
    );
    
    // 取消所有请求
    void CancelAll();
    
    // 等待所有请求完成
    void WaitAll();
    
private:
    CNLayer* layer_;
    std::vector<std::thread> worker_threads_;
    std::queue<ReadRequest> request_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::atomic<bool> shutdown_{false};
    
    void WorkerThread();
};

// 使用示例
void ProcessFeaturesAsync(CNLayer* layer) {
    CNAsyncReader reader(layer, 4);
    
    // 获取所有 FID
    std::vector<int64_t> fids = layer->GetAllFIDs();
    
    // 异步处理
    std::atomic<int> processed{0};
    reader.GetFeaturesAsync(fids, 
        [&processed](int64_t fid, CNFeature* feature, CNStatus status) {
            if (status == CNStatus::kSuccess) {
                ProcessFeature(feature);
            }
            processed++;
        });
    
    // 等待完成
    reader.WaitAll();
}
```

#### 5.14.2 异步写入器

```cpp
class CNAsyncWriter {
public:
    struct WriteRequest {
        std::unique_ptr<CNFeature> feature;
        std::promise<CNStatus> result_promise;
    };
    
    CNAsyncWriter(CNLayer* layer, int batch_size = 1000);
    ~CNAsyncWriter();
    
    // 异步写入
    std::future<CNStatus> WriteFeatureAsync(std::unique_ptr<CNFeature> feature);
    
    // 批量写入
    std::future<CNStatus> WriteFeaturesAsync(
        std::vector<std::unique_ptr<CNFeature>> features
    );
    
    // 刷新缓冲区
    void Flush();
    
private:
    CNLayer* layer_;
    int batch_size_;
    std::vector<std::unique_ptr<CNFeature>> batch_;
    std::thread writer_thread_;
    std::queue<WriteRequest> request_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    std::atomic<bool> shutdown_{false};
    
    void WriterThread();
};
```

### 5.15 插件架构设计

#### 5.15.1 驱动插件接口

```cpp
// 插件接口
class CNLayerDriverPlugin {
public:
    virtual ~CNLayerDriverPlugin() = default;
    
    // 驱动信息
    virtual std::string GetName() const = 0;
    virtual std::string GetDescription() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::vector<std::string> GetSupportedExtensions() const = 0;
    
    // 能力查询
    virtual bool CanOpen(const std::string& filename) const = 0;
    virtual bool CanCreate(const std::string& filename) const = 0;
    
    // 打开/创建图层
    virtual std::unique_ptr<CNVectorLayer> OpenVectorLayer(
        const std::string& filename,
        const std::string& layer_name,
        OpenMode mode
    ) = 0;
    
    virtual std::unique_ptr<CNRasterLayer> OpenRasterLayer(
        const std::string& filename,
        OpenMode mode
    ) = 0;
    
    virtual std::unique_ptr<CNVectorLayer> CreateVectorLayer(
        const std::string& filename,
        const std::string& layer_name,
        const CNLayerDefn& defn
    ) = 0;
};

// 插件管理器
class CNPluginManager {
public:
    static CNPluginManager& Instance() {
        static CNPluginManager instance;
        return instance;
    }
    
    // 加载插件
    CNStatus LoadPlugin(const std::string& plugin_path);
    
    // 卸载插件
    CNStatus UnloadPlugin(const std::string& plugin_name);
    
    // 注册驱动
    void RegisterDriver(std::unique_ptr<CNLayerDriverPlugin> driver);
    
    // 查找驱动
    CNLayerDriverPlugin* FindDriver(const std::string& filename) const;
    CNLayerDriverPlugin* FindDriverByName(const std::string& name) const;
    
    // 获取所有驱动
    std::vector<CNLayerDriverPlugin*> GetAllDrivers() const;
    
private:
    CNPluginManager() = default;
    
    std::map<std::string, std::unique_ptr<CNLayerDriverPlugin>> drivers_;
    std::map<std::string, void*> loaded_libraries_;  // 动态库句柄
    mutable std::mutex mutex_;
};
```

#### 5.15.2 插件加载机制

```cpp
CNStatus CNPluginManager::LoadPlugin(const std::string& plugin_path) {
#ifdef CN_PLATFORM_WINDOWS
    HMODULE handle = LoadLibraryA(plugin_path.c_str());
    if (!handle) {
        return CNStatus::kError;
    }
    
    // 获取插件创建函数
    typedef CNLayerDriverPlugin* (*CreatePluginFunc)();
    CreatePluginFunc create_func = reinterpret_cast<CreatePluginFunc>(
        GetProcAddress(handle, "CreatePlugin"));
    
    if (!create_func) {
        FreeLibrary(handle);
        return CNStatus::kError;
    }
#else
    void* handle = dlopen(plugin_path.c_str(), RTLD_NOW);
    if (!handle) {
        return CNStatus::kError;
    }
    
    typedef CNLayerDriverPlugin* (*CreatePluginFunc)();
    CreatePluginFunc create_func = reinterpret_cast<CreatePluginFunc>(
        dlsym(handle, "CreatePlugin"));
    
    if (!create_func) {
        dlclose(handle);
        return CNStatus::kError;
    }
#endif
    
    // 创建并注册插件
    CNLayerDriverPlugin* plugin = create_func();
    if (!plugin) {
        return CNStatus::kError;
    }
    
    std::string name = plugin->GetName();
    drivers_[name] = std::unique_ptr<CNLayerDriverPlugin>(plugin);
    loaded_libraries_[name] = handle;
    
    return CNStatus::kSuccess;
}
```

### 5.16 几何操作算法

#### 5.16.1 Douglas-Peucker 简化算法

```cpp
class CNGeometrySimplifier {
public:
    // Douglas-Peucker 算法实现
    static std::unique_ptr<CNLineString> SimplifyLineString(
        const CNLineString* line,
        double tolerance
    ) {
        if (line->GetPointCount() < 3) {
            return line->Clone();
        }
        
        std::vector<bool> keep(line->GetPointCount(), false);
        keep[0] = true;
        keep[line->GetPointCount() - 1] = true;
        
        SimplifyDP(line, 0, line->GetPointCount() - 1, tolerance, keep);
        
        // 构建简化后的线
        auto result = std::make_unique<CNLineString>();
        for (size_t i = 0; i < keep.size(); ++i) {
            if (keep[i]) {
                result->AddPoint(line->GetPoint(i));
            }
        }
        return result;
    }
    
private:
    static void SimplifyDP(
        const CNLineString* line,
        size_t first,
        size_t last,
        double tolerance,
        std::vector<bool>& keep
    ) {
        if (last <= first + 1) {
            return;
        }
        
        // 找到距离最远的点
        double max_dist = 0;
        size_t max_idx = first;
        
        const CNPoint& p1 = line->GetPoint(first);
        const CNPoint& p2 = line->GetPoint(last);
        
        for (size_t i = first + 1; i < last; ++i) {
            double dist = PerpendicularDistance(line->GetPoint(i), p1, p2);
            if (dist > max_dist) {
                max_dist = dist;
                max_idx = i;
            }
        }
        
        // 如果最大距离大于容差，递归简化
        if (max_dist > tolerance) {
            keep[max_idx] = true;
            SimplifyDP(line, first, max_idx, tolerance, keep);
            SimplifyDP(line, max_idx, last, tolerance, keep);
        }
    }
    
    static double PerpendicularDistance(
        const CNPoint& pt,
        const CNPoint& line_p1,
        const CNPoint& line_p2
    ) {
        double dx = line_p2.GetX() - line_p1.GetX();
        double dy = line_p2.GetY() - line_p1.GetY();
        
        double line_len_sq = dx * dx + dy * dy;
        if (line_len_sq == 0) {
            return pt.Distance(line_p1);
        }
        
        double t = ((pt.GetX() - line_p1.GetX()) * dx + 
                    (pt.GetY() - line_p1.GetY()) * dy) / line_len_sq;
        t = std::max(0.0, std::min(1.0, t));
        
        double proj_x = line_p1.GetX() + t * dx;
        double proj_y = line_p1.GetY() + t * dy;
        
        double dist_x = pt.GetX() - proj_x;
        double dist_y = pt.GetY() - proj_y;
        
        return std::sqrt(dist_x * dist_x + dist_y * dist_y);
    }
};

// 多边形简化（保持拓扑）
static std::unique_ptr<CNPolygon> SimplifyPolygon(
    const CNPolygon* polygon,
    double tolerance,
    bool preserve_topology = true
) {
    if (preserve_topology) {
        // 拓扑保持简化：确保环不自交
        auto simplified_exterior = SimplifyLineString(
            polygon->GetExteriorRing(), tolerance);
        
        // 验证简化后的环是否有效
        if (!simplified_exterior->IsValidRing()) {
            // 回退到更小的容差
            return SimplifyPolygon(polygon, tolerance * 0.5, true);
        }
        
        auto result = std::make_unique<CNPolygon>();
        result->SetExteriorRing(std::move(simplified_exterior));
        
        // 简化内环
        for (int i = 0; i < polygon->GetInteriorRingCount(); ++i) {
            auto simplified_interior = SimplifyLineString(
                polygon->GetInteriorRing(i), tolerance);
            if (simplified_interior->IsValidRing()) {
                result->AddInteriorRing(std::move(simplified_interior));
            }
        }
        
        return result;
    } else {
        // 简单简化：不保证拓扑
        auto result = std::make_unique<CNPolygon>();
        result->SetExteriorRing(SimplifyLineString(
            polygon->GetExteriorRing(), tolerance));
        return result;
    }
}
```

#### 5.16.2 Voronoi 图生成

```cpp
class CNVoronoiBuilder {
public:
    struct VoronoiCell {
        int64_t site_fid;           // 关联的站点 FID
        std::unique_ptr<CNPolygon> cell;  // Voronoi 多边形
    };
    
    // Fortune 算法实现
    static std::vector<VoronoiCell> BuildVoronoi(
        const CNLayer* point_layer,
        const Envelope& bounds
    ) {
        std::vector<VoronoiCell> cells;
        
        // 收集所有站点
        std::vector<std::pair<int64_t, CNPoint>> sites;
        point_layer->ResetReading();
        while (CNFeature* f = point_layer->GetNextFeature()) {
            CNPoint* pt = dynamic_cast<CNPoint*>(f->GetGeometry());
            if (pt) {
                sites.emplace_back(f->GetFID(), *pt);
            }
        }
        
        // 使用 Fortune 算法构建 Voronoi 图
        // 实际实现可使用 GEOS 或 Boost.Polygon
        // 这里提供接口定义
        
        return cells;
    }
    
    // 使用 GEOS 实现
    static std::vector<VoronoiCell> BuildVoronoiGEOS(
        const CNLayer* point_layer,
        const Envelope& bounds,
        double tolerance = 0.0
    );
};
```

#### 5.16.3 Delaunay 三角剖分

```cpp
class CNDelaunayTriangulator {
public:
    struct Triangle {
        CNPoint p1, p2, p3;
        int64_t fid1, fid2, fid3;  // 关联的原始点 FID
    };
    
    // Bowyer-Watson 算法实现
    static std::vector<Triangle> Triangulate(
        const std::vector<CNPoint>& points
    ) {
        std::vector<Triangle> triangles;
        
        if (points.size() < 3) {
            return triangles;
        }
        
        // 创建超级三角形
        Envelope bounds;
        for (const auto& pt : points) {
            bounds.ExpandToInclude(pt.GetX(), pt.GetY());
        }
        
        double dx = bounds.GetWidth() * 10;
        double dy = bounds.GetHeight() * 10;
        CNPoint super_p1(bounds.GetMinX() - dx, bounds.GetMinY() - dy);
        CNPoint super_p2(bounds.GetMaxX() + dx, bounds.GetMinY() - dy);
        CNPoint super_p3((bounds.GetMinX() + bounds.GetMaxX()) / 2,
                         bounds.GetMaxY() + dy);
        
        triangles.push_back({super_p1, super_p2, super_p3, -1, -1, -1});
        
        // 逐点插入
        for (const auto& pt : points) {
            std::vector<Triangle> bad_triangles;
            std::vector<std::pair<CNPoint, CNPoint>> polygon;
            
            // 找出所有外接圆包含该点的三角形
            for (const auto& tri : triangles) {
                if (IsPointInCircumcircle(pt, tri)) {
                    bad_triangles.push_back(tri);
                }
            }
            
            // 找出多边形边界
            for (const auto& tri : bad_triangles) {
                AddEdgeIfNotShared(polygon, tri.p1, tri.p2);
                AddEdgeIfNotShared(polygon, tri.p2, tri.p3);
                AddEdgeIfNotShared(polygon, tri.p3, tri.p1);
            }
            
            // 删除坏三角形
            triangles.erase(
                std::remove_if(triangles.begin(), triangles.end(),
                    [&bad_triangles](const Triangle& t) {
                        return std::find(bad_triangles.begin(), 
                                        bad_triangles.end(), t) != 
                               bad_triangles.end();
                    }),
                triangles.end());
            
            // 创建新三角形
            for (const auto& edge : polygon) {
                triangles.push_back({edge.first, edge.second, pt, -1, -1, -1});
            }
        }
        
        // 删除包含超级三角形顶点的三角形
        triangles.erase(
            std::remove_if(triangles.begin(), triangles.end(),
                [&super_p1, &super_p2, &super_p3](const Triangle& t) {
                    return t.p1.Equals(super_p1) || t.p1.Equals(super_p2) ||
                           t.p1.Equals(super_p3) ||
                           t.p2.Equals(super_p1) || t.p2.Equals(super_p2) ||
                           t.p2.Equals(super_p3) ||
                           t.p3.Equals(super_p1) || t.p3.Equals(super_p2) ||
                           t.p3.Equals(super_p3);
                }),
            triangles.end());
        
        return triangles;
    }
    
private:
    static bool IsPointInCircumcircle(const CNPoint& pt, const Triangle& tri) {
        double ax = tri.p1.GetX() - pt.GetX();
        double ay = tri.p1.GetY() - pt.GetY();
        double bx = tri.p2.GetX() - pt.GetX();
        double by = tri.p2.GetY() - pt.GetY();
        double cx = tri.p3.GetX() - pt.GetX();
        double cy = tri.p3.GetY() - pt.GetY();
        
        double det = (ax * ax + ay * ay) * (bx * cy - cx * by) -
                     (bx * bx + by * by) * (ax * cy - cx * ay) +
                     (cx * cx + cy * cy) * (ax * by - bx * ay);
        
        return det > 0;
    }
    
    static void AddEdgeIfNotShared(
        std::vector<std::pair<CNPoint, CNPoint>>& polygon,
        const CNPoint& p1, const CNPoint& p2
    ) {
        for (auto it = polygon.begin(); it != polygon.end(); ++it) {
            if ((it->first.Equals(p1) && it->second.Equals(p2)) ||
                (it->first.Equals(p2) && it->second.Equals(p1))) {
                polygon.erase(it);
                return;
            }
        }
        polygon.emplace_back(p1, p2);
    }
};
```

### 5.17 空间分析功能

#### 5.17.1 缓冲区分析

```cpp
class CNSpatialAnalysis {
public:
    // 缓冲区分析
    static std::unique_ptr<CNGeometry> Buffer(
        const CNGeometry* geom,
        double distance,
        int quadrant_segments = 8,
        BufferCapStyle cap_style = BufferCapStyle::kRound
    ) {
        switch (geom->GetGeometryType()) {
            case GeomType::kPoint:
                return BufferPoint(static_cast<const CNPoint*>(geom), 
                                   distance, quadrant_segments);
            case GeomType::kLineString:
                return BufferLineString(static_cast<const CNLineString*>(geom),
                                        distance, quadrant_segments, cap_style);
            case GeomType::kPolygon:
                return BufferPolygon(static_cast<const CNPolygon*>(geom),
                                     distance, quadrant_segments);
            default:
                return nullptr;
        }
    }
    
    enum class BufferCapStyle {
        kRound,    // 圆形端点
        kFlat,     // 平头端点
        kSquare    // 方形端点
    };
    
private:
    static std::unique_ptr<CNPolygon> BufferPoint(
        const CNPoint* point,
        double distance,
        int segments
    ) {
        auto ring = std::make_unique<CNLinearRing>();
        
        for (int i = 0; i < segments; ++i) {
            double angle = 2 * M_PI * i / segments;
            double x = point->GetX() + distance * std::cos(angle);
            double y = point->GetY() + distance * std::sin(angle);
            ring->AddPoint(CNPoint(x, y));
        }
        ring->CloseRings();
        
        auto polygon = std::make_unique<CNPolygon>();
        polygon->SetExteriorRing(std::move(ring));
        return polygon;
    }
    
    static std::unique_ptr<CNGeometry> BufferLineString(
        const CNLineString* line,
        double distance,
        int segments,
        BufferCapStyle cap_style
    );
};
```

#### 5.17.2 叠加分析

```cpp
class CNOverlayAnalysis {
public:
    enum class OverlayType {
        kIntersection,   // 交集
        kUnion,          // 并集
        kDifference,     // 差集
        kSymDifference,  // 对称差集
        kIdentity        // 标识
    };
    
    // 图层叠加分析
    static std::unique_ptr<CNLayer> Overlay(
        const CNLayer* layer1,
        const CNLayer* layer2,
        OverlayType type,
        std::function<bool(double)> progress = nullptr
    ) {
        auto result = std::make_unique<CNMemoryLayer>(
            "overlay_result", GeomType::kPolygon);
        
        // 使用空间索引加速
        auto index1 = BuildSpatialIndex(layer1);
        auto index2 = BuildSpatialIndex(layer2);
        
        layer1->ResetReading();
        int64_t total = layer1->GetFeatureCount();
        int64_t processed = 0;
        
        while (CNFeature* f1 = layer1->GetNextFeature()) {
            CNGeometry* g1 = f1->GetGeometry();
            Envelope env = g1->GetEnvelope();
            
            // 查找相交要素
            auto candidates = index2->Query(env);
            
            for (int64_t fid2 : candidates) {
                CNFeature* f2 = layer2->GetFeature(fid2);
                CNGeometry* g2 = f2->GetGeometry();
                
                // 精确相交检测
                if (g1->Intersects(g2)) {
                    // 执行叠加操作
                    std::unique_ptr<CNGeometry> result_geom;
                    switch (type) {
                        case OverlayType::kIntersection:
                            result_geom = g1->Intersection(g2);
                            break;
                        case OverlayType::kUnion:
                            result_geom = g1->Union(g2);
                            break;
                        case OverlayType::kDifference:
                            result_geom = g1->Difference(g2);
                            break;
                        case OverlayType::kSymDifference:
                            result_geom = g1->SymDifference(g2);
                            break;
                        default:
                            break;
                    }
                    
                    if (result_geom && !result_geom->IsEmpty()) {
                        // 创建结果要素，合并属性
                        auto result_feature = std::make_unique<CNFeature>();
                        result_feature->SetGeometry(std::move(result_geom));
                        MergeAttributes(result_feature.get(), f1, f2);
                        result->CreateFeature(result_feature.get());
                    }
                }
            }
            
            processed++;
            if (progress && !progress(
                static_cast<double>(processed) / total)) {
                break;
            }
        }
        
        return result;
    }
    
private:
    static void MergeAttributes(
        CNFeature* result,
        const CNFeature* f1,
        const CNFeature* f2
    ) {
        // 合并两个要素的属性
        // 字段名冲突时添加前缀
        for (int i = 0; i < f1->GetFieldCount(); ++i) {
            const CNFieldDefn* defn = f1->GetFieldDefn(i);
            result->SetField("layer1_" + defn->GetName(), f1->GetField(i));
        }
        for (int i = 0; i < f2->GetFieldCount(); ++i) {
            const CNFieldDefn* defn = f2->GetFieldDefn(i);
            result->SetField("layer2_" + defn->GetName(), f2->GetField(i));
        }
    }
};
```

#### 5.17.3 网络分析基础

```cpp
class CNNetworkAnalysis {
public:
    // 网络图结构
    struct NetworkGraph {
        struct Node {
            int64_t id;
            CNPoint location;
            std::vector<int64_t> edge_ids;
        };
        
        struct Edge {
            int64_t id;
            int64_t from_node;
            int64_t to_node;
            double length;
            double cost;
            bool is_directed;
        };
        
        std::map<int64_t, Node> nodes;
        std::map<int64_t, Edge> edges;
    };
    
    // 从线图层构建网络图
    static std::unique_ptr<NetworkGraph> BuildGraph(
        const CNLayer* line_layer,
        double tolerance = 0.0
    ) {
        auto graph = std::make_unique<NetworkGraph>();
        
        // 收集所有端点
        std::map<std::pair<double, double>, int64_t> endpoint_to_node;
        int64_t next_node_id = 1;
        int64_t next_edge_id = 1;
        
        line_layer->ResetReading();
        while (CNFeature* f = line_layer->GetNextFeature()) {
            CNLineString* line = dynamic_cast<CNLineString*>(f->GetGeometry());
            if (!line || line->GetPointCount() < 2) continue;
            
            // 获取起点和终点
            const CNPoint& start_pt = line->GetPoint(0);
            const CNPoint& end_pt = line->GetPoint(line->GetPointCount() - 1);
            
            // 创建或获取节点
            int64_t start_node = GetOrCreateNode(
                graph.get(), endpoint_to_node, start_pt, next_node_id, tolerance);
            int64_t end_node = GetOrCreateNode(
                graph.get(), endpoint_to_node, end_pt, next_node_id, tolerance);
            
            // 创建边
            NetworkGraph::Edge edge;
            edge.id = next_edge_id++;
            edge.from_node = start_node;
            edge.to_node = end_node;
            edge.length = line->GetLength();
            edge.cost = edge.length;
            edge.is_directed = false;
            
            graph->edges[edge.id] = edge;
            graph->nodes[start_node].edge_ids.push_back(edge.id);
            graph->nodes[end_node].edge_ids.push_back(edge.id);
        }
        
        return graph;
    }
    
    // Dijkstra 最短路径
    static std::vector<int64_t> ShortestPath(
        const NetworkGraph* graph,
        int64_t start_node,
        int64_t end_node
    ) {
        std::map<int64_t, double> dist;
        std::map<int64_t, int64_t> prev;
        std::priority_queue<std::pair<double, int64_t>,
                           std::vector<std::pair<double, int64_t>>,
                           std::greater<>> pq;
        
        // 初始化
        for (const auto& pair : graph->nodes) {
            dist[pair.first] = std::numeric_limits<double>::max();
        }
        dist[start_node] = 0;
        pq.push({0, start_node});
        
        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();
            
            if (u == end_node) break;
            if (d > dist[u]) continue;
            
            // 遍历相邻边
            for (int64_t edge_id : graph->nodes.at(u).edge_ids) {
                const auto& edge = graph->edges.at(edge_id);
                int64_t v = (edge.from_node == u) ? edge.to_node : edge.from_node;
                
                double alt = dist[u] + edge.cost;
                if (alt < dist[v]) {
                    dist[v] = alt;
                    prev[v] = u;
                    pq.push({alt, v});
                }
            }
        }
        
        // 重建路径
        std::vector<int64_t> path;
        if (dist[end_node] < std::numeric_limits<double>::max()) {
            for (int64_t at = end_node; at != start_node; at = prev[at]) {
                path.push_back(at);
            }
            path.push_back(start_node);
            std::reverse(path.begin(), path.end());
        }
        
        return path;
    }
    
private:
    static int64_t GetOrCreateNode(
        NetworkGraph* graph,
        std::map<std::pair<double, double>, int64_t>& endpoint_to_node,
        const CNPoint& pt,
        int64_t& next_node_id,
        double tolerance
    ) {
        // 简化：使用坐标作为键
        // 实际应使用空间索引和容差匹配
        auto key = std::make_pair(pt.GetX(), pt.GetY());
        
        auto it = endpoint_to_node.find(key);
        if (it != endpoint_to_node.end()) {
            return it->second;
        }
        
        int64_t node_id = next_node_id++;
        NetworkGraph::Node node;
        node.id = node_id;
        node.location = pt;
        graph->nodes[node_id] = node;
        endpoint_to_node[key] = node_id;
        
        return node_id;
    }
};
```

### 5.18 数据格式注意事项

#### 5.18.1 Shapefile 限制

```cpp
// Shapefile 格式限制和注意事项
struct ShapefileLimits {
    static constexpr size_t kMaxFileSize = 2147483647;  // 2GB
    static constexpr int kMaxFieldNameLength = 10;
    static constexpr int kMaxFieldCount = 255;
    static constexpr double kMaxCoordinateValue = 1e38;
    
    // 检查图层是否符合 Shapefile 限制
    static std::vector<std::string> ValidateForShapefile(const CNLayer* layer) {
        std::vector<std::string> warnings;
        
        // 检查字段名长度
        for (int i = 0; i < layer->GetLayerDefn()->GetFieldCount(); ++i) {
            const CNFieldDefn* defn = layer->GetLayerDefn()->GetFieldDefn(i);
            if (defn->GetName().length() > kMaxFieldNameLength) {
                warnings.push_back("Field name '" + defn->GetName() + 
                    "' will be truncated to 10 characters");
            }
        }
        
        // 检查几何类型
        GeomType geom_type = layer->GetGeomType();
        if (geom_type == GeomType::kCircularString ||
            geom_type == GeomType::kCompoundCurve ||
            geom_type == GeomType::kCurvePolygon) {
            warnings.push_back("Curve geometries will be converted to linear approximations");
        }
        
        // 检查混合几何类型
        if (geom_type == GeomType::kUnknown) {
            warnings.push_back("Mixed geometry types may cause issues");
        }
        
        return warnings;
    }
    
    // Shapefile 写入选项
    struct WriteOptions {
        std::string encoding = "UTF-8";  // 或 "ISO-8859-1"
        bool write_prj = true;           // 写入 PRJ 文件
        bool write_cpg = true;           // 写入 CPG 编码文件
        bool enforce_2d = false;         // 强制 2D
        double curve_tolerance = 0.0;    // 曲线转直线容差
    };
};
```

#### 5.18.2 GeoJSON 特性支持

```cpp
// GeoJSON 格式特性
struct GeoJSONFeatures {
    // 支持的几何类型
    static const std::set<GeomType> kSupportedGeomTypes;
    
    // GeoJSON 扩展选项
    struct WriteOptions {
        bool write_bbox = true;          // 写入边界框
        bool write_crs = false;          // 写入 CRS (非标准)
        int coordinate_precision = 7;    // 坐标精度
        bool write_id_field = true;      // 写入 id 字段
        std::string id_field_name = "id";
        bool pretty_print = false;       // 格式化输出
    };
    
    // RFC 7946 规范要求
    // 1. 坐标顺序: 经度在前，纬度在后 (lon, lat)
    // 2. 默认 CRS: WGS84 (EPSG:4326)
    // 3. 边界框: [west, south, east, north]
    
    static std::string FeatureToGeoJSON(const CNFeature* feature) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(7);
        
        ss << "{\"type\":\"Feature\"";
        
        // ID
        ss << ",\"id\":" << feature->GetFID();
        
        // 几何
        ss << ",\"geometry\":";
        GeometryToGeoJSON(ss, feature->GetGeometry());
        
        // 属性
        ss << ",\"properties\":{";
        for (int i = 0; i < feature->GetFieldCount(); ++i) {
            if (i > 0) ss << ",";
            const CNFieldDefn* defn = feature->GetFieldDefn(i);
            ss << "\"" << defn->GetName() << "\":";
            FieldValueToGeoJSON(ss, feature->GetField(i));
        }
        ss << "}";
        
        ss << "}";
        return ss.str();
    }
    
private:
    static void GeometryToGeoJSON(std::ostream& ss, const CNGeometry* geom);
    static void FieldValueToGeoJSON(std::ostream& ss, const CNFieldValue* value);
};
```

#### 5.18.3 GeoPackage 完整特性

```cpp
// GeoPackage 格式特性
struct GeoPackageFeatures {
    // GeoPackage 优势
    // 1. 单文件容器 (SQLite)
    // 2. 支持多图层
    // 3. 完整事务支持
    // 4. 64位 FID
    // 5. Unicode 字段名
    // 6. 空间索引 (RTree)
    // 7. 栅格瓦片支持
    
    struct LayerOptions {
        std::string table_name;
        std::string identifier;          // 元数据标识
        std::string description;
        bool has_z = false;
        bool has_m = false;
        std::string srs_id = "4326";     // 空间参考 ID
        bool create_spatial_index = true;
        std::string geometry_column_name = "geom";
    };
    
    // 创建 GeoPackage 图层
    static CNStatus CreateLayer(
        const std::string& filename,
        const LayerOptions& options,
        const CNLayerDefn& defn
    );
    
    // GeoPackage 扩展
    enum class Extension {
        kRTreeSpatialIndex,    // RTree 空间索引
        kZoomOther,            // 其他缩放级别
        kCRSWGS84,             // WGS84 CRS
        kSchema,               // 模式扩展
        kNonLinearGeometries,  // 非线性几何
        kTiledGriddedCoverage  // 栅格瓦片
    };
    
    // 检查扩展支持
    static bool HasExtension(const std::string& filename, Extension ext);
    
    // 启用扩展
    static CNStatus EnableExtension(
        const std::string& filename,
        Extension ext
    );
};
```

### 5.19 性能基准测试

#### 5.19.1 测试框架

```cpp
class CNLayerBenchmark {
public:
    struct BenchmarkResult {
        std::string test_name;
        int64_t feature_count;
        double elapsed_ms;
        double ops_per_second;
        double memory_mb;
        std::map<std::string, double> metrics;
    };
    
    // 运行基准测试
    static std::vector<BenchmarkResult> RunAllBenchmarks(
        CNLayer* layer,
        int iterations = 10
    ) {
        std::vector<BenchmarkResult> results;
        
        results.push_back(BenchmarkIterateAll(layer, iterations));
        results.push_back(BenchmarkRandomRead(layer, iterations));
        results.push_back(BenchmarkSpatialQuery(layer, iterations));
        results.push_back(BenchmarkAttributeQuery(layer, iterations));
        results.push_back(BenchmarkWrite(layer, iterations));
        
        return results;
    }
    
private:
    static BenchmarkResult BenchmarkIterateAll(CNLayer* layer, int iterations) {
        BenchmarkResult result;
        result.test_name = "IterateAll";
        result.feature_count = layer->GetFeatureCount();
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            layer->ResetReading();
            int64_t count = 0;
            while (layer->GetNextFeature()) {
                count++;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.elapsed_ms = std::chrono::duration<double, std::milli>(
            end - start).count() / iterations;
        result.ops_per_second = result.feature_count * 1000.0 / result.elapsed_ms;
        
        return result;
    }
    
    static BenchmarkResult BenchmarkRandomRead(CNLayer* layer, int iterations) {
        BenchmarkResult result;
        result.test_name = "RandomRead";
        result.feature_count = layer->GetFeatureCount();
        
        // 生成随机 FID 列表
        std::vector<int64_t> fids;
        layer->ResetReading();
        while (CNFeature* f = layer->GetNextFeature()) {
            fids.push_back(f->GetFID());
        }
        std::random_shuffle(fids.begin(), fids.end());
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            for (int64_t fid : fids) {
                CNFeature* f = layer->GetFeature(fid);
                (void)f;  // 防止优化掉
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.elapsed_ms = std::chrono::duration<double, std::milli>(
            end - start).count() / iterations;
        result.ops_per_second = result.feature_count * 1000.0 / result.elapsed_ms;
        
        return result;
    }
    
    static BenchmarkResult BenchmarkSpatialQuery(CNLayer* layer, int iterations);
    static BenchmarkResult BenchmarkAttributeQuery(CNLayer* layer, int iterations);
    static BenchmarkResult BenchmarkWrite(CNLayer* layer, int iterations);
};
```

#### 5.19.2 性能调优建议

```cpp
// 性能调优指南
struct PerformanceTuningGuide {
    // 内存图层优化
    struct MemoryLayerTuning {
        // 1. 预分配内存
        static constexpr int kInitialCapacity = 10000;
        
        // 2. 选择合适的索引
        // - < 1000 要素: 无索引
        // - 1000-10000: 网格索引
        // - > 10000: R树索引
        
        // 3. 缓存策略
        static constexpr int kDefaultCacheSize = 1000;
        
        // 4. 批量操作
        static constexpr int kBatchSize = 1000;
    };
    
    // 文件图层优化
    struct FileLayerTuning {
        // 1. 使用空间索引
        // 2. 延迟加载属性
        // 3. 批量读取
        // 4. 内存映射
    };
    
    // 数据库图层优化
    struct DatabaseLayerTuning {
        // 1. 连接池大小
        static constexpr int kDefaultPoolSize = 10;
        
        // 2. 使用空间索引 (PostGIS: GIST)
        // 3. 批量插入 (COPY 命令)
        // 4. 查询优化
    };
    
    // 性能问题诊断
    static std::vector<std::string> DiagnosePerformance(
        CNLayer* layer,
        const BenchmarkResult& result
    ) {
        std::vector<std::string> issues;
        
        // 检查迭代性能
        if (result.ops_per_second < 10000) {
            issues.push_back("Low iteration performance. Consider using spatial index.");
        }
        
        // 检查随机读取性能
        if (result.metrics["random_read_ratio"] > 10.0) {
            issues.push_back("Random read is slow. Consider enabling feature cache.");
        }
        
        return issues;
    }
};
```

### 5.20 版本控制与历史数据

#### 5.20.1 时态 GIS 支持

```cpp
class CNTemporalLayer {
public:
    // 时间类型
    enum class TemporalType {
        kInstant,    // 时间点
        kPeriod,     // 时间段
        kNone        // 非时态
    };
    
    // 时间字段配置
    struct TemporalFieldConfig {
        std::string start_field;
        std::string end_field;      // 可选，用于时间段
        TemporalType type;
        std::string time_format;    // 时间格式
    };
    
    // 设置时间字段
    void SetTemporalField(const TemporalFieldConfig& config);
    
    // 时间查询
    std::vector<CNFeature*> QueryByTime(
        const std::chrono::system_clock::time_point& time
    );
    
    std::vector<CNFeature*> QueryByTimeRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end
    );
    
    // 时间线分析
    struct TimeSeriesPoint {
        std::chrono::system_clock::time_point time;
        int64_t feature_count;
        double value;  // 可选统计值
    };
    
    std::vector<TimeSeriesPoint> GetTimeSeries(
        const std::string& field_name,
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end,
        std::chrono::seconds interval
    );
    
private:
    TemporalFieldConfig temporal_config_;
    std::unique_ptr<CNLayer> base_layer_;
};
```

#### 5.20.2 数据版本管理

```cpp
class CNVersionedLayer {
public:
    struct Version {
        int64_t version_id;
        std::chrono::system_clock::time_point timestamp;
        std::string author;
        std::string description;
        int64_t change_count;
    };
    
    // 创建版本快照
    CNStatus CreateVersion(const std::string& description);
    
    // 获取版本列表
    std::vector<Version> GetVersions() const;
    
    // 恢复到指定版本
    CNStatus RestoreVersion(int64_t version_id);
    
    // 获取指定版本的要素
    CNFeature* GetFeatureAtVersion(int64_t fid, int64_t version_id);
    
    // 变更追踪
    struct Change {
        enum Type { kInsert, kUpdate, kDelete };
        Type type;
        int64_t fid;
        std::unique_ptr<CNFeature> before;  // 更新/删除前
        std::unique_ptr<CNFeature> after;   // 插入/更新后
    };
    
    std::vector<Change> GetChangesSinceVersion(int64_t version_id);
    
    // 差异比较
    struct Diff {
        std::vector<int64_t> added;
        std::vector<int64_t> deleted;
        std::vector<std::pair<int64_t, std::string>> modified;
    };
    
    Diff CompareVersions(int64_t version1, int64_t version2);
    
private:
    std::vector<Version> versions_;
    std::map<int64_t, std::vector<Change>> version_changes_;
    std::unique_ptr<CNLayer> base_layer_;
};
```

### 5.21 三维数据支持

#### 5.21.1 三维几何类型

```cpp
// 三维几何类型扩展
enum class GeomType3D {
    kPointZ,           // 三维点 (X, Y, Z)
    kLineStringZ,      // 三维线
    kPolygonZ,         // 三维多边形
    kMultiPointZ,      // 三维多点
    kMultiLineStringZ, // 三维多线
    kMultiPolygonZ,    // 三维多面
    kPolyhedralSurface, // 多面体表面
    kTIN,              // 不规则三角网
    kTriangle,         // 三角形
    kSolid,            // 实体
    kMultiSolid        // 多实体
};

// 三维点
class CNPointZ : public CNGeometry {
public:
    CNPointZ(double x, double y, double z) 
        : x_(x), y_(y), z_(z) {}
    
    double GetX() const { return x_; }
    double GetY() const { return y_; }
    double GetZ() const { return z_; }
    
    // 三维距离计算
    double Distance3D(const CNPointZ& other) const {
        double dx = x_ - other.x_;
        double dy = y_ - other.y_;
        double dz = z_ - other.z_;
        return std::sqrt(dx*dx + dy*dy + dz*dz);
    }
    
    // 投影到二维
    CNPoint ToPoint2D() const { return CNPoint(x_, y_); }
    
private:
    double x_, y_, z_;
};

// TIN (不规则三角网)
class CNTIN : public CNGeometry {
public:
    struct Triangle {
        int v1, v2, v3;  // 顶点索引
    };
    
    void AddVertex(const CNPointZ& vertex);
    void AddTriangle(int v1, int v2, int v3);
    
    const std::vector<CNPointZ>& GetVertices() const { return vertices_; }
    const std::vector<Triangle>& GetTriangles() const { return triangles_; }
    
    // 三维范围
    struct Envelope3D {
        double min_x, max_x;
        double min_y, max_y;
        double min_z, max_z;
    };
    Envelope3D GetEnvelope3D() const;
    
    // 高程插值
    double InterpolateZ(double x, double y) const;
    
private:
    std::vector<CNPointZ> vertices_;
    std::vector<Triangle> triangles_;
};
```

#### 5.21.2 点云数据支持

```cpp
// 点云图层
class CNPointCloudLayer : public CNLayer {
public:
    struct PointCloudHeader {
        uint64_t point_count;
        double min_x, max_x;
        double min_y, max_y;
        double min_z, max_z;
        double scale_x, scale_y, scale_z;
        double offset_x, offset_y, offset_z;
        std::vector<std::string> dimensions;  // X, Y, Z, Intensity, ReturnNumber, etc.
    };
    
    struct Point {
        double x, y, z;
        uint16_t intensity;
        uint8_t return_number;
        uint8_t number_of_returns;
        uint8_t classification;
        uint16_t point_source_id;
        // LAS 1.4 扩展属性
        uint16_t red, green, blue;
        uint16_t nir;  // 近红外
    };
    
    // 点云读取
    CNStatus ReadPoints(uint64_t start, uint64_t count, 
                        std::vector<Point>& points);
    
    // 空间查询
    CNStatus QueryPoints(const Envelope3D& bounds, 
                         std::vector<Point>& points,
                         int max_points = -1);
    
    // 点云统计
    struct Statistics {
        double mean_z;
        double std_dev_z;
        double density;  // 点密度 (点/平方米)
        std::map<uint8_t, uint64_t> classification_counts;
    };
    Statistics ComputeStatistics();
    
    // 点云简化
    CNStatus Simplify(double tolerance, CNPointCloudLayer* output);
    
    // 地面点分类
    CNStatus ClassifyGroundPoints(double cell_size = 1.0);
    
private:
    PointCloudHeader header_;
    std::unique_ptr<OctreeIndex> spatial_index_;
};
```

### 5.22 瓦片服务支持

#### 5.22.1 矢量瓦片

```cpp
// 矢量瓦片图层
class CNVectorTileLayer : public CNLayer {
public:
    // 瓦片坐标
    struct TileCoord {
        int z;  // 缩放级别
        int x;  // 列号
        int y;  // 行号
    };
    
    // 瓦片范围计算
    static Envelope TileBounds(const TileCoord& coord) {
        double n = std::pow(2.0, coord.z);
        double min_x = coord.x / n * 360.0 - 180.0;
        double max_x = (coord.x + 1) / n * 360.0 - 180.0;
        double min_y = std::atan(std::sinh(M_PI * (1 - 2 * (coord.y + 1) / n))) 
                       * 180.0 / M_PI;
        double max_y = std::atan(std::sinh(M_PI * (1 - 2 * coord.y / n))) 
                       * 180.0 / M_PI;
        return Envelope(min_x, min_y, max_x, max_y);
    }
    
    // 瓦片编码格式
    enum class TileFormat {
        kMVT,      // Mapbox Vector Tile (PBF)
        kGeoJSON,  // GeoJSON
        kTopoJSON  // TopoJSON
    };
    
    // 获取瓦片
    std::vector<uint8_t> GetTile(const TileCoord& coord, TileFormat format);
    
    // 瓦片生成选项
    struct TileOptions {
        int extent = 4096;           // 瓦片坐标范围
        double simplify_tolerance;    // 简化容差
        int max_zoom = 14;           // 最大缩放级别
        int min_zoom = 0;            // 最小缩放级别
        std::vector<std::string> layer_names;  // 包含的图层
    };
    
    // 预生成瓦片
    CNStatus GenerateTiles(const std::string& output_dir,
                           const TileOptions& options,
                           std::function<bool(int, int)> progress = nullptr);
    
private:
    std::map<TileCoord, std::vector<uint8_t>> tile_cache_;
    TileOptions options_;
};

// MVT 编码器
class MVTLEncoder {
public:
    // 将要素编码为 MVT 格式
    std::vector<uint8_t> Encode(
        const std::vector<CNFeature*>& features,
        const TileCoord& coord,
        int extent = 4096
    );
    
private:
    // 几何坐标转换
    void TransformGeometry(const CNGeometry* geom, 
                          const TileCoord& coord,
                          int extent,
                          std::vector<uint32_t>& commands);
    
    // ZigZag 编码
    int32_t ZigZagEncode(int32_t n) {
        return (n << 1) ^ (n >> 31);
    }
};
```

#### 5.22.2 栅格瓦片

```cpp
// 栅格瓦片服务
class CNRasterTileService {
public:
    // 瓦片矩阵集 (OGC WMTS 标准)
    struct TileMatrix {
        std::string identifier;
        double scale_denominator;
        int tile_width;
        int tile_height;
        int matrix_width;
        int matrix_height;
        double top_left_x;
        double top_left_y;
    };
    
    struct TileMatrixSet {
        std::string identifier;
        std::string crs;
        std::vector<TileMatrix> matrices;
    };
    
    // 获取瓦片
    std::vector<uint8_t> GetTile(
        const TileCoord& coord,
        const std::string& format = "image/png",
        const std::string& style = "default"
    );
    
    // WMTS 能力文档
    std::string GetCapabilities(const std::string& service_url);
    
    // TMS 兼容
    std::string GetTMSUrl(const TileCoord& coord, 
                          const std::string& format = "png");
    
    // 瓦片缓存
    struct CacheConfig {
        std::string cache_dir;
        int64_t max_size_bytes;
        int expire_days;
    };
    void SetCache(const CacheConfig& config);
    
private:
    std::shared_ptr<CNRasterLayer> source_layer_;
    TileMatrixSet tile_matrix_set_;
    std::unique_ptr<TileCache> cache_;
};
```

### 5.23 大数据集成

#### 5.23.1 Apache Sedona 集成

```cpp
// Apache Sedona (原 GeoSpark) 集成
class CNSedonaAdapter {
public:
    // 将图层转换为 Sedona DataFrame
    static std::shared_ptr<DataFrame> ToSedonaDataFrame(
        const CNLayer* layer,
        SparkSession* spark
    ) {
        // 创建 Schema
        StructType schema;
        for (int i = 0; i < layer->GetLayerDefn()->GetFieldCount(); ++i) {
            const CNFieldDefn* defn = layer->GetLayerDefn()->GetFieldDefn(i);
            schema = schema.add(defn->GetName(), GetSparkType(defn->GetType()));
        }
        schema = schema.add("geometry", BinaryType());
        
        // 收集数据
        std::vector<Row> rows;
        layer->ResetReading();
        while (CNFeature* f = layer->GetNextFeature()) {
            std::vector<GenericValue> values;
            for (int i = 0; i < f->GetFieldCount(); ++i) {
                values.push_back(FieldValueToSpark(f->GetField(i)));
            }
            values.push_back(GeometryToWKB(f->GetGeometry()));
            rows.push_back(Row(values));
        }
        
        return spark->createDataFrame(rows, schema);
    }
    
    // Sedona 空间操作
    static std::unique_ptr<CNLayer> SpatialJoin(
        const CNLayer* left,
        const CNLayer* right,
        const std::string& predicate = "intersects"
    );
    
    static std::unique_ptr<CNLayer> SpatialAggregate(
        const CNLayer* layer,
        const std::string& operation  // "union", "intersection", etc.
    );
    
    // 空间分区
    static std::vector<std::unique_ptr<CNLayer>> SpatialPartition(
        const CNLayer* layer,
        int num_partitions,
        const std::string& partition_type = "quadtree"
    );
};
```

#### 5.23.2 分布式处理

```cpp
// 分布式图层处理
class CNDistributedLayerProcessor {
public:
    struct Config {
        int num_workers;
        int batch_size;
        std::string coordinator_url;
        int timeout_seconds;
    };
    
    // 并行处理
    template<typename Func>
    CNStatus ProcessParallel(
        CNLayer* layer,
        Func processor,
        const Config& config
    ) {
        // 分割数据
        auto partitions = PartitionData(layer, config.num_workers);
        
        // 启动工作进程
        std::vector<std::future<CNStatus>> futures;
        for (int i = 0; i < config.num_workers; ++i) {
            futures.push_back(std::async(std::launch::async, 
                [&, i]() {
                    return ProcessPartition(partitions[i].get(), processor);
                }
            ));
        }
        
        // 等待完成
        for (auto& f : futures) {
            CNStatus status = f.get();
            if (status != CNStatus::kSuccess) {
                return status;
            }
        }
        
        return CNStatus::kSuccess;
    }
    
    // MapReduce 模式
    template<typename Mapper, typename Reducer>
    std::unique_ptr<CNLayer> MapReduce(
        const CNLayer* input,
        Mapper mapper,
        Reducer reducer,
        const Config& config
    );
    
private:
    std::vector<std::unique_ptr<CNLayer>> PartitionData(
        CNLayer* layer, int num_partitions);
};
```

### 5.24 数据序列化与压缩

#### 5.24.1 高效序列化

```cpp
// 二进制序列化
class CNLayerSerializer {
public:
    // 序列化格式
    enum class Format {
        kBinary,     // 自定义二进制格式
        kWKB,        // Well-Known Binary
        kTWKB,       // Tiny WKB (压缩)
        kGeoPackage, // GeoPackage 二进制
        kFlatBuffer  // FlatBuffer 格式
    };
    
    // 序列化选项
    struct SerializeOptions {
        Format format = Format::kBinary;
        bool compress = true;
        int compression_level = 6;  // zlib 级别
        bool include_spatial_index = true;
        bool include_metadata = true;
    };
    
    // 序列化到文件
    CNStatus Serialize(
        const CNLayer* layer,
        const std::string& filename,
        const SerializeOptions& options = {}
    );
    
    // 从文件反序列化
    std::unique_ptr<CNLayer> Deserialize(
        const std::string& filename,
        const SerializeOptions& options = {}
    );
    
    // 序列化到内存
    std::vector<uint8_t> SerializeToMemory(
        const CNLayer* layer,
        const SerializeOptions& options = {}
    );
    
    // 增量序列化
    CNStatus SerializeDelta(
        const CNLayer* layer,
        const std::string& base_filename,
        const std::string& delta_filename
    );
};

// WKB 扩展
class CNWKBWriter {
public:
    // EWKB (Extended WKB) 支持 SRID 和 Z/M
    std::vector<uint8_t> WriteEWKB(const CNGeometry* geom, int srid = -1);
    
    // TWKB (Tiny WKB) 压缩格式
    std::vector<uint8_t> WriteTWKB(
        const CNGeometry* geom,
        int xy_precision = 6,
        int z_precision = 3,
        int m_precision = 3
    );
    
private:
    void WritePoint(std::vector<uint8_t>& buf, const CNPoint* pt);
    void WriteLineString(std::vector<uint8_t>& buf, const CNLineString* line);
    void WritePolygon(std::vector<uint8_t>& buf, const CNPolygon* poly);
};
```

#### 5.24.2 压缩策略

```cpp
// 图层压缩
class CNLayerCompressor {
public:
    // 压缩算法
    enum class Algorithm {
        kZlib,
        kLZ4,
        kZstd,
        kSnappy
    };
    
    // 压缩选项
    struct CompressOptions {
        Algorithm algorithm = Algorithm::kZstd;
        int level = 3;
        bool deduplicate_geometry = true;  // 几何去重
        bool delta_encode_coords = true;   // 坐标差分编码
        bool quantize_coords = false;      // 坐标量化
        double quantize_tolerance = 0.001;
    };
    
    // 压缩图层
    CNStatus Compress(
        const CNLayer* layer,
        const std::string& output_file,
        const CompressOptions& options = {}
    );
    
    // 解压图层
    std::unique_ptr<CNLayer> Decompress(const std::string& filename);
    
    // 压缩统计
    struct CompressStats {
        size_t original_size;
        size_t compressed_size;
        double compression_ratio;
        double compress_time_ms;
        double decompress_time_ms;
    };
    static CompressStats GetStats(const std::string& filename);
};
```

### 5.25 权限与安全

#### 5.25.1 访问控制

```cpp
// 图层权限管理
class CNLayerPermission {
public:
    // 权限类型
    enum class Permission {
        kNone = 0,
        kRead = 1,
        kWrite = 2,
        kDelete = 4,
        kAdmin = 8,
        kAll = kRead | kWrite | kDelete | kAdmin
    };
    
    // 用户角色
    struct Role {
        std::string name;
        uint32_t permissions;
        std::vector<std::string> allowed_layers;
        std::map<std::string, std::string> field_restrictions;
    };
    
    // 权限检查
    bool HasPermission(const std::string& user, Permission perm) const;
    
    // 字段级权限
    bool CanAccessField(const std::string& user, 
                        const std::string& field_name) const;
    
    // 几何区域限制
    struct SpatialRestriction {
        std::string user;
        std::unique_ptr<CNGeometry> allowed_area;
        bool clip_to_area;  // 是否裁剪到允许区域
    };
    
    void SetSpatialRestriction(const SpatialRestriction& restriction);
    
    // 审计日志
    struct AuditLog {
        std::string user;
        std::string action;
        int64_t fid;
        std::chrono::system_clock::time_point timestamp;
        std::string details;
    };
    
    void LogAccess(const AuditLog& log);
    std::vector<AuditLog> GetAuditLogs(
        const std::string& user = "",
        const std::chrono::system_clock::time_point& start = {},
        const std::chrono::system_clock::time_point& end = {}
    );
    
private:
    std::map<std::string, Role> roles_;
    std::vector<SpatialRestriction> spatial_restrictions_;
    std::vector<AuditLog> audit_logs_;
    mutable std::mutex mutex_;
};
```

#### 5.25.2 数据脱敏

```cpp
// 数据脱敏处理器
class CNDataMasker {
public:
    // 脱敏策略
    enum class MaskStrategy {
        kNone,
        kHash,        // 哈希脱敏
        kTruncate,    // 截断脱敏
        kSubstitute,  // 替换脱敏
        kShuffle,     // 打乱脱敏
        kGeneralize   // 泛化脱敏
    };
    
    // 字段脱敏规则
    struct MaskRule {
        std::string field_name;
        MaskStrategy strategy;
        std::string pattern;  // 用于替换的模板
        bool reversible;      // 是否可逆
    };
    
    // 应用脱敏
    void ApplyMask(CNFeature* feature, const std::vector<MaskRule>& rules);
    
    // 几何脱敏
    std::unique_ptr<CNGeometry> MaskGeometry(
        const CNGeometry* geom,
        double tolerance  // 简化容差，降低精度
    );
    
    // 坐标扰动
    CNPoint PerturbCoordinate(
        const CNPoint& pt,
        double max_offset
    );
    
    // 敏感字段检测
    static std::vector<std::string> DetectSensitiveFields(
        const CNLayer* layer,
        const std::vector<std::string>& patterns = {
            "password", "ssn", "credit", "phone", "email", "address"
        }
    );
};
```

### 5.26 国际化与本地化

#### 5.26.1 多语言支持架构

```cpp
class CNLocale {
public:
    static CNLocale& Instance() {
        static CNLocale instance;
        return instance;
    }
    
    void SetLocale(const std::string& locale_code) {
        locale_code_ = locale_code;
        LoadTranslations(locale_code);
    }
    
    const std::string& GetLocale() const { return locale_code_; }
    
    std::string Translate(const std::string& key) const {
        auto it = translations_.find(key);
        if (it != translations_.end()) {
            return it->second;
        }
        return key;
    }
    
    std::string FormatNumber(double value, int precision = -1) const {
        std::ostringstream oss;
        oss.imbue(GetLocaleObject());
        if (precision >= 0) {
            oss << std::fixed << std::setprecision(precision);
        }
        oss << value;
        return oss.str();
    }
    
    std::string FormatDate(const std::chrono::system_clock::time_point& tp) const;
    std::string FormatCoordinate(double value, bool is_latitude) const;
    
private:
    std::string locale_code_;
    std::map<std::string, std::string> translations_;
    
    void LoadTranslations(const std::string& locale);
    std::locale GetLocaleObject() const;
};

#define CN_TR(key) CNLocale::Instance().Translate(key)
```

#### 5.26.2 图层元数据本地化

```cpp
class CNLocalizedMetadata {
public:
    void SetTitle(const std::string& locale, const std::string& title) {
        titles_[locale] = title;
    }
    
    std::string GetTitle(const std::string& locale = "") const {
        if (locale.empty()) {
            return GetTitle(CNLocale::Instance().GetLocale());
        }
        auto it = titles_.find(locale);
        if (it != titles_.end()) {
            return it->second;
        }
        return titles_.empty() ? "" : titles_.begin()->second;
    }
    
    void SetDescription(const std::string& locale, const std::string& desc);
    std::string GetDescription(const std::string& locale = "") const;
    
    void SetKeywords(const std::string& locale, const std::vector<std::string>& keywords);
    std::vector<std::string> GetKeywords(const std::string& locale = "") const;
    
private:
    std::map<std::string, std::string> titles_;
    std::map<std::string, std::string> descriptions_;
    std::map<std::string, std::vector<std::string>> keywords_;
};

class CNLayer {
public:
    CNLocalizedMetadata& GetLocalizedMetadata() { return localized_metadata_; }
    const CNLocalizedMetadata& GetLocalizedMetadata() const { return localized_metadata_; }
    
private:
    CNLocalizedMetadata localized_metadata_;
};
```

#### 5.26.3 坐标格式与单位本地化

```cpp
class CNCoordinateFormatter {
public:
    enum class Format {
        kDecimalDegrees,      // 116.3975°
        kDegreesMinutes,      // 116°23.85'
        kDegreesMinutesSeconds, // 116°23'51"
        kUTM,                 // 50R 448251 4381164
        kMGRS                 // 50R MK 48251 81164
    };
    
    struct FormatOptions {
        Format format = Format::kDecimalDegrees;
        int decimal_places = 5;
        bool use_unicode_symbols = true;
        std::string degree_symbol = "°";
        std::string minute_symbol = "'";
        std::string second_symbol = "\"";
    };
    
    static std::string FormatLatitude(double lat, const FormatOptions& options = {});
    static std::string FormatLongitude(double lon, const FormatOptions& options = {});
    static std::string FormatCoordinate(double lon, double lat, const FormatOptions& options = {});
    
    static bool ParseCoordinate(const std::string& str, double& lon, double& lat);
    
    static std::string ToUTM(double lon, double lat, int& zone, char& band);
    static bool FromUTM(const std::string& utm, double& lon, double& lat);
    
    static std::string ToMGRS(double lon, double lat, int precision = 5);
    static bool FromMGRS(const std::string& mgrs, double& lon, double& lat);
};

class CNUnitConverter {
public:
    enum class LinearUnit {
        kMeter,
        kKilometer,
        kFoot,
        kYard,
        kMile,
        kNauticalMile,
        kChineseLi,
        kChineseZhang
    };
    
    static double Convert(double value, LinearUnit from, LinearUnit to);
    static std::string GetUnitName(LinearUnit unit, const std::string& locale = "");
    static std::string GetUnitSymbol(LinearUnit unit);
    
    enum class AreaUnit {
        kSquareMeter,
        kHectare,
        kSquareKilometer,
        kSquareFoot,
        kAcre,
        kSquareMile,
        kChineseMu,
        kChineseQing
    };
    
    static double ConvertArea(double value, AreaUnit from, AreaUnit to);
    static std::string FormatArea(double square_meters, AreaUnit preferred_unit, 
                                   const std::string& locale = "");
};
```

### 5.27 数据质量评估

#### 5.27.1 数据质量维度

```cpp
enum class DataQualityDimension {
    kCompleteness,      // 完整性
    kAccuracy,          // 准确性
    kConsistency,       // 一致性
    kTimeliness,        // 时效性
    kValidity,          // 有效性
    kUniqueness         // 唯一性
};

struct DataQualityReport {
    double completeness_score;
    double accuracy_score;
    double consistency_score;
    double timeliness_score;
    double validity_score;
    double uniqueness_score;
    double overall_score;
    
    std::vector<std::string> issues;
    std::map<std::string, int> issue_counts;
    std::chrono::system_clock::time_point assessment_time;
};
```

#### 5.27.2 数据质量检查器

```cpp
class CNDataQualityChecker {
public:
    DataQualityReport Assess(const CNLayer* layer) {
        DataQualityReport report;
        
        report.completeness_score = CheckCompleteness(layer);
        report.accuracy_score = CheckAccuracy(layer);
        report.consistency_score = CheckConsistency(layer);
        report.validity_score = CheckValidity(layer);
        report.uniqueness_score = CheckUniqueness(layer);
        
        report.overall_score = (report.completeness_score + 
                                 report.accuracy_score +
                                 report.consistency_score + 
                                 report.validity_score +
                                 report.uniqueness_score) / 5.0;
        
        report.assessment_time = std::chrono::system_clock::now();
        return report;
    }
    
private:
    double CheckCompleteness(const CNLayer* layer) {
        int64_t total = layer->GetFeatureCount();
        if (total == 0) return 100.0;
        
        int64_t complete = 0;
        layer->ResetReading();
        while (CNFeature* f = layer->GetNextFeature()) {
            if (IsFeatureComplete(f)) {
                complete++;
            }
        }
        return 100.0 * complete / total;
    }
    
    bool IsFeatureComplete(const CNFeature* feature) {
        if (!feature->GetGeometry()) return false;
        
        for (int i = 0; i < feature->GetFieldCount(); ++i) {
            if (feature->IsFieldSet(i) && !feature->IsFieldNull(i)) {
                return true;
            }
        }
        return false;
    }
    
    double CheckAccuracy(const CNLayer* layer) {
        int64_t total = layer->GetFeatureCount();
        if (total == 0) return 100.0;
        
        int64_t accurate = 0;
        layer->ResetReading();
        while (CNFeature* f = layer->GetNextFeature()) {
            if (IsGeometryValid(f->GetGeometry())) {
                accurate++;
            }
        }
        return 100.0 * accurate / total;
    }
    
    double CheckConsistency(const CNLayer* layer);
    double CheckValidity(const CNLayer* layer);
    double CheckUniqueness(const CNLayer* layer);
};
```

#### 5.27.3 拓扑一致性验证

```cpp
class CNTopologyValidator {
public:
    struct TopologyError {
        enum class Type {
            kSelfIntersection,      // 自相交
            kOverlap,               // 重叠
            kGap,                   // 间隙
            kDanglingNode,          // 悬挂节点
            kPseudoNode,            // 伪节点
            kDuplicateGeometry,     // 重复几何
            kInvalidRing            // 无效环
        };
        
        Type type;
        int64_t feature_fid1;
        int64_t feature_fid2;
        std::unique_ptr<CNGeometry> error_location;
        std::string description;
    };
    
    std::vector<TopologyError> Validate(
        const CNLayer* layer,
        const std::vector<TopologyError::Type>& rules
    ) {
        std::vector<TopologyError> errors;
        
        for (auto rule : rules) {
            auto rule_errors = ValidateRule(layer, rule);
            errors.insert(errors.end(), 
                          std::make_move_iterator(rule_errors.begin()),
                          std::make_move_iterator(rule_errors.end()));
        }
        
        return errors;
    }
    
private:
    std::vector<TopologyError> ValidateRule(
        const CNLayer* layer,
        TopologyError::Type rule
    );
    
    std::vector<TopologyError> CheckSelfIntersection(const CNLayer* layer);
    std::vector<TopologyError> CheckOverlap(const CNLayer* layer);
    std::vector<TopologyError> CheckGaps(const CNLayer* layer);
};

class CNTopologyRule {
public:
    static std::unique_ptr<CNTopologyRule> CreateMustNotOverlap(
        const CNLayer* layer
    );
    static std::unique_ptr<CNTopologyRule> CreateMustNotHaveGaps(
        const CNLayer* layer
    );
    static std::unique_ptr<CNTopologyRule> CreateMustBeCoveredBy(
        const CNLayer* layer1, const CNLayer* layer2
    );
    static std::unique_ptr<CNTopologyRule> CreateMustBeInside(
        const CNLayer* layer1, const CNLayer* layer2
    );
    
    virtual std::vector<TopologyError> Validate() = 0;
    virtual ~CNTopologyRule() = default;
};
```

### 5.28 网络服务客户端

#### 5.28.1 WFS 客户端实现

```cpp
class CNWFSClient {
public:
    struct WFSLayerInfo {
        std::string name;
        std::string title;
        std::string abstract;
        std::string crs;
        Envelope bbox;
        std::vector<std::string> keywords;
    };
    
    struct WFSCapabilities {
        std::string version;
        std::string service_title;
        std::string service_abstract;
        std::vector<WFSLayerInfo> layers;
        std::vector<std::string> supported_operations;
        std::vector<std::string> supported_formats;
        int max_features;
    };
    
    CNWFSClient(const std::string& service_url);
    
    CNStatus GetCapabilities(WFSCapabilities& capabilities);
    
    std::unique_ptr<CNLayer> GetFeature(
        const std::string& layer_name,
        const std::string& filter = "",
        const Envelope& bbox = Envelope(),
        int max_features = -1
    );
    
    CNStatus DescribeFeatureType(
        const std::string& layer_name,
        CNLayerDefn& defn
    );
    
    CNStatus Transaction(
        const std::vector<CNFeature*>& inserts,
        const std::vector<CNFeature*>& updates,
        const std::vector<int64_t>& deletes
    );
    
    void SetTimeout(int milliseconds) { timeout_ms_ = milliseconds; }
    void SetCredentials(const std::string& user, const std::string& password);
    void SetProxy(const std::string& proxy_url);
    
private:
    std::string service_url_;
    std::string username_;
    std::string password_;
    std::string proxy_url_;
    int timeout_ms_ = 30000;
    
    std::string BuildGetCapabilitiesURL();
    std::string BuildGetFeatureURL(const std::string& layer, const std::string& filter);
    std::unique_ptr<CNLayer> ParseGMLResponse(const std::string& gml);
};
```

#### 5.28.2 WMS 客户端实现

```cpp
class CNWMSClient {
public:
    struct WMSLayerInfo {
        std::string name;
        std::string title;
        std::string abstract;
        std::vector<std::string> crs_list;
        Envelope bbox;
        std::vector<std::string> styles;
        bool queryable;
    };
    
    struct WMSCapabilities {
        std::string version;
        std::string service_title;
        std::vector<WMSLayerInfo> layers;
        std::vector<std::string> supported_formats;
        std::vector<std::string> supported_crs;
    };
    
    struct GetMapRequest {
        std::vector<std::string> layers;
        std::vector<std::string> styles;
        std::string crs;
        Envelope bbox;
        int width;
        int height;
        std::string format;
        bool transparent;
    };
    
    CNWMSClient(const std::string& service_url);
    
    CNStatus GetCapabilities(WMSCapabilities& capabilities);
    
    std::vector<uint8_t> GetMap(const GetMapRequest& request);
    
    CNFeature* GetFeatureInfo(
        const std::string& layer,
        int x, int y,
        int width, int height,
        const Envelope& bbox,
        const std::string& info_format = "application/json"
    );
    
    void SetTimeout(int milliseconds);
    void SetCredentials(const std::string& user, const std::string& password);
    
private:
    std::string service_url_;
    int timeout_ms_ = 30000;
    
    std::string BuildGetMapURL(const GetMapRequest& request);
    std::string BuildGetFeatureInfoURL(const std::string& layer, int x, int y,
                                        int width, int height, const Envelope& bbox);
};
```

#### 5.28.3 OGC API Features 客户端

```cpp
class CNOGCAPIFeaturesClient {
public:
    CNOGCAPIFeaturesClient(const std::string& base_url);
    
    struct CollectionInfo {
        std::string id;
        std::string title;
        std::string description;
        std::vector<std::string> crs;
        Envelope extent;
        std::map<std::string, std::string> links;
    };
    
    std::vector<CollectionInfo> GetCollections();
    CollectionInfo GetCollection(const std::string& collection_id);
    
    std::unique_ptr<CNLayer> GetItems(
        const std::string& collection_id,
        int limit = 100,
        int offset = 0,
        const Envelope& bbox = Envelope(),
        const std::string& datetime = ""
    );
    
    CNFeature* GetItem(
        const std::string& collection_id,
        const std::string& feature_id
    );
    
    std::string GetConformance();
    
private:
    std::string base_url_;
    std::map<std::string, std::string> default_headers_;
    
    std::unique_ptr<CNLayer> ParseGeoJSONResponse(const std::string& json);
};
```

### 5.29 空间统计分析

#### 5.29.1 空间自相关分析

```cpp
class CNSpatialStatistics {
public:
    struct MoranResult {
        double moran_i;         // Moran's I 指数
        double expected_i;      // 期望值
        double variance;        // 方差
        double z_score;         // Z 得分
        double p_value;         // P 值
        bool significant;       // 是否显著
    };
    
    struct GearyResult {
        double geary_c;         // Geary's C 指数
        double expected_c;
        double variance;
        double z_score;
        double p_value;
        bool significant;
    };
    
    static MoranResult MoranI(
        const CNLayer* layer,
        const std::string& field,
        SpatialWeightType weight_type = SpatialWeightType::kQueen
    );
    
    static GearyResult GearyC(
        const CNLayer* layer,
        const std::string& field,
        SpatialWeightType weight_type = SpatialWeightType::kQueen
    );
    
    enum class SpatialWeightType {
        kRook,      // 车邻接
        kQueen,     // 后邻接
        kKNearest,  // K最近邻
        kDistance   // 距离阈值
    };
    
    static std::vector<std::vector<double>> BuildSpatialWeights(
        const CNLayer* layer,
        SpatialWeightType type,
        double threshold = 0.0,
        int k = 4
    );
};
```

#### 5.29.2 热点分析

```cpp
class CNHotspotAnalysis {
public:
    struct HotspotResult {
        double gi_star;         // Gi* 统计量
        double z_score;
        double p_value;
        int confidence_level;   // 90, 95, 99
        bool is_hotspot;
        bool is_coldspot;
    };
    
    static std::vector<HotspotResult> GetisOrdGiStar(
        const CNLayer* layer,
        const std::string& field,
        double distance_band
    );
    
    static std::unique_ptr<CNLayer> CreateHotspotLayer(
        const CNLayer* input_layer,
        const std::string& field,
        double distance_band
    );
    
    static std::unique_ptr<CNRasterLayer> KernelDensity(
        const CNLayer* point_layer,
        const std::string& weight_field,
        double bandwidth,
        double cell_size,
        const Envelope& extent
    );
};
```

#### 5.29.3 空间插值

```cpp
class CNSpatialInterpolation {
public:
    enum class Method {
        kIDW,           // 反距离加权
        kKriging,       // 克里金
        kSpline,        // 样条
        kNaturalNeighbor, // 自然邻域
        kNearest        // 最近邻
    };
    
    struct InterpolationOptions {
        Method method = Method::kIDW;
        double power = 2.0;              // IDW 幂参数
        int max_points = 12;             // 最大点数
        double search_radius = 0.0;      // 搜索半径 (0=可变)
        double cell_size = 0.0;          // 输出栅格单元大小
        Envelope extent;                 // 输出范围
        VariogramModel variogram;        // 克里金变异函数
    };
    
    struct VariogramModel {
        enum class Type {
            kSpherical,
            kExponential,
            kGaussian,
            kLinear
        };
        Type type = Type::kSpherical;
        double nugget = 0.0;
        double sill = 1.0;
        double range = 1.0;
    };
    
    static std::unique_ptr<CNRasterLayer> Interpolate(
        const CNLayer* point_layer,
        const std::string& value_field,
        const InterpolationOptions& options
    );
    
private:
    static double IDW(
        double x, double y,
        const std::vector<CNPoint>& points,
        const std::vector<double>& values,
        double power, int max_points
    );
    
    static double Kriging(
        double x, double y,
        const std::vector<CNPoint>& points,
        const std::vector<double>& values,
        const VariogramModel& variogram
    );
};
```

#### 5.29.4 空间聚类分析

```cpp
class CNSpatialClustering {
public:
    struct ClusterResult {
        int cluster_id;
        std::vector<int64_t> feature_fids;
        std::unique_ptr<CNGeometry> hull;
        CNPoint centroid;
        double area;
        int feature_count;
    };
    
    static std::vector<ClusterResult> DBSCAN(
        const CNLayer* layer,
        double epsilon,
        int min_points
    );
    
    static std::vector<ClusterResult> KMeans(
        const CNLayer* layer,
        int k,
        int max_iterations = 100
    );
    
    static std::vector<ClusterResult> HierarchicalClustering(
        const CNLayer* layer,
        int num_clusters,
        LinkageMethod method = LinkageMethod::kWard
    );
    
    enum class LinkageMethod {
        kSingle,
        kComplete,
        kAverage,
        kWard
    };
    
    static double OptimalEpsilon(
        const CNLayer* layer,
        int min_points
    );
};
```

### 5.30 数据备份与恢复

#### 5.30.1 备份策略

```cpp
class CNBackupManager {
public:
    enum class BackupType {
        kFull,          // 完全备份
        kIncremental,   // 增量备份
        kDifferential   // 差异备份
    };
    
    struct BackupOptions {
        BackupType type = BackupType::kFull;
        bool compress = true;
        int compression_level = 6;
        bool include_spatial_index = true;
        bool include_metadata = true;
        std::string encryption_key;
        std::vector<std::string> exclude_layers;
    };
    
    struct BackupInfo {
        std::string backup_id;
        std::chrono::system_clock::time_point timestamp;
        BackupType type;
        size_t size_bytes;
        std::string checksum;
        std::vector<std::string> included_layers;
        std::string base_backup_id;  // 增量备份的基础ID
    };
    
    CNStatus CreateBackup(
        const std::string& backup_path,
        const std::vector<CNLayer*>& layers,
        const BackupOptions& options = {}
    );
    
    std::vector<BackupInfo> ListBackups(const std::string& backup_dir);
    
    CNStatus RestoreBackup(
        const std::string& backup_path,
        const std::string& restore_dir,
        const std::vector<std::string>& layer_names = {}
    );
    
    CNStatus VerifyBackup(const std::string& backup_path);
    
    CNStatus DeleteBackup(const std::string& backup_id);
    
    void SetSchedule(const std::string& cron_expression, const BackupOptions& options);
    
private:
    std::string GenerateBackupId();
    std::string CalculateChecksum(const std::string& filepath);
    CNStatus WriteBackupManifest(const std::string& path, const BackupInfo& info);
};
```

#### 5.30.2 增量备份实现

```cpp
class CNIncrementalBackup {
public:
    struct ChangeRecord {
        enum class Type {
            kInsert,
            kUpdate,
            kDelete
        };
        
        Type type;
        int64_t fid;
        std::chrono::system_clock::time_point timestamp;
        std::unique_ptr<CNFeature> old_value;
        std::unique_ptr<CNFeature> new_value;
    };
    
    void RecordChange(const ChangeRecord& record) {
        std::lock_guard<std::mutex> lock(mutex_);
        changes_.push_back(record);
        
        if (changes_.size() >= batch_size_) {
            FlushChanges();
        }
    }
    
    CNStatus CreateIncrementalBackup(
        const std::string& base_backup_path,
        const std::string& output_path
    );
    
    CNStatus ApplyIncrementalBackup(
        const std::string& base_data_path,
        const std::string& incremental_backup_path
    );
    
    void SetBatchSize(size_t size) { batch_size_ = size; }
    
private:
    std::vector<ChangeRecord> changes_;
    std::mutex mutex_;
    size_t batch_size_ = 1000;
    std::chrono::system_clock::time_point last_backup_time_;
    
    void FlushChanges();
};
```

#### 5.30.3 灾难恢复

```cpp
class CNDisasterRecovery {
public:
    struct RecoveryPlan {
        std::string plan_id;
        std::string primary_backup_location;
        std::string secondary_backup_location;
        int recovery_point_objective_minutes;  // RPO
        int recovery_time_objective_minutes;   // RTO
        std::vector<std::string> critical_layers;
        std::map<std::string, std::string> layer_priorities;
    };
    
    CNStatus CreateRecoveryPlan(const RecoveryPlan& plan);
    
    CNStatus ExecuteRecovery(
        const std::string& plan_id,
        const std::string& target_location
    );
    
    struct RecoveryStatus {
        enum class State {
            kNotStarted,
            kInProgress,
            kCompleted,
            kFailed
        };
        
        State state;
        int progress_percent;
        std::string current_step;
        std::vector<std::string> completed_steps;
        std::vector<std::string> errors;
        std::chrono::system_clock::time_point start_time;
        std::chrono::system_clock::time_point end_time;
    };
    
    RecoveryStatus GetRecoveryStatus(const std::string& plan_id);
    
    CNStatus CancelRecovery(const std::string& plan_id);
    
    bool ValidateRecovery(const std::string& target_location);
    
private:
    std::map<std::string, RecoveryPlan> plans_;
    std::map<std::string, RecoveryStatus> active_recoveries_;
};
```

### 5.31 GIS库对比分析

#### 5.31.1 主要GIS库对比

| 特性 | CNLayer | GDAL/OGR | GeoTools | QGIS Core | ArcGIS Engine |
|------|---------|----------|----------|-----------|---------------|
| 语言 | C++11 | C++ | Java | C++ | C++/.NET |
| 开源 | 是 | 是 | 是 | 是 | 否 |
| OGC标准 | 完整 | 完整 | 完整 | 完整 | 完整 |
| 线程安全 | 设计支持 | 部分 | 部分 | 部分 | 是 |
| 内存管理 | 智能指针 | 手动 | GC | 手动 | 智能指针 |
| 格式支持 | 可扩展 | 200+ | 100+ | 80+ | 50+ |
| 空间索引 | R树/四叉树/网格 | R树 | R树/四叉树 | R树 | R树/网格 |
| 事务支持 | MVCC | 部分 | 部分 | 部分 | 完整 |
| 跨平台 | 是 | 是 | 是 | 是 | 部分 |

#### 5.31.2 架构对比

```cpp
// GDAL OGRLayer 架构特点
// - 成熟稳定，格式支持最广
// - C 风格 API，C++ 封装
// - 单线程设计，需外部同步
// - 手动内存管理

// GeoTools 架构特点
// - Java 生态，企业级应用
// - 完整的 OGC 标准实现
// - 插件式架构
// - GC 内存管理

// CNLayer 设计优势
// 1. 现代 C++11 特性
//    - 智能指针自动内存管理
//    - 移动语义高效数据传递
//    - RAII 资源管理

// 2. 线程安全设计
//    - 读写锁细粒度控制
//    - 线程局部存储
//    - MVCC 事务隔离

// 3. 类型安全
//    - 强类型枚举
//    - 编译期检查
//    - 类型推导

// 4. 可扩展性
//    - 插件驱动架构
//    - 工厂模式创建
//    - 能力测试机制
```

#### 5.31.3 性能对比建议

```cpp
// 性能测试场景建议
class CNPerformanceBenchmark {
public:
    struct BenchmarkResult {
        std::string operation;
        int64_t feature_count;
        double time_ms;
        double throughput;  // features/second
        size_t memory_peak_bytes;
    };
    
    static std::vector<BenchmarkResult> RunBenchmarks() {
        std::vector<BenchmarkResult> results;
        
        // 1. 要素遍历性能
        results.push_back(BenchmarkTraverse());
        
        // 2. 空间查询性能
        results.push_back(BenchmarkSpatialQuery());
        
        // 3. 批量写入性能
        results.push_back(BenchmarkBulkInsert());
        
        // 4. 并发读取性能
        results.push_back(BenchmarkConcurrentRead());
        
        // 5. 内存使用效率
        results.push_back(BenchmarkMemoryUsage());
        
        return results;
    }
    
private:
    static BenchmarkResult BenchmarkTraverse();
    static BenchmarkResult BenchmarkSpatialQuery();
    static BenchmarkResult BenchmarkBulkInsert();
    static BenchmarkResult BenchmarkConcurrentRead();
    static BenchmarkResult BenchmarkMemoryUsage();
};
```

### 5.32 实际应用案例

#### 5.32.1 城市规划应用

```cpp
// 城市规划图层管理案例
class UrbanPlanningApplication {
public:
    void Initialize() {
        // 创建规划图层组
        planning_group_ = std::make_unique<CNLayerGroup>("城市规划");
        
        // 添加基础图层
        planning_group_->AddLayer(CreateLandUseLayer());
        planning_group_->AddLayer(CreateRoadNetworkLayer());
        planning_group_->AddLayer(CreateBuildingLayer());
        planning_group_->AddLayer(CreateGreenSpaceLayer());
        
        // 设置图层依赖关系
        SetupLayerDependencies();
        
        // 初始化拓扑规则
        InitializeTopologyRules();
    }
    
    CNStatus AnalyzeLandUseChange(
        const CNLayer* old_layer,
        const CNLayer* new_layer,
        CNLayer* change_layer
    ) {
        // 空间叠加分析
        auto union_result = CNSpatialRelation::Union(
            old_layer->GetExtent(), new_layer->GetExtent());
        
        // 计算变化区域
        old_layer->ResetReading();
        while (CNFeature* old_feature = old_layer->GetNextFeature()) {
            new_layer->ResetReading();
            while (CNFeature* new_feature = new_layer->GetNextFeature()) {
                if (CNSpatialRelation::Intersects(
                        old_feature->GetGeometry(), 
                        new_feature->GetGeometry())) {
                    // 检测变化类型
                    auto intersection = CNSpatialRelation::Intersection(
                        old_feature->GetGeometry(),
                        new_feature->GetGeometry());
                    
                    if (intersection) {
                        CNFeature change_feature;
                        change_feature.SetGeometry(intersection.get());
                        change_feature.SetField("change_type", 
                            DetectChangeType(old_feature, new_feature));
                        change_layer->CreateFeature(&change_feature);
                    }
                }
            }
        }
        
        return CNStatus::kSuccess;
    }
    
private:
    std::unique_ptr<CNLayerGroup> planning_group_;
    
    std::unique_ptr<CNLayer> CreateLandUseLayer();
    std::unique_ptr<CNLayer> CreateRoadNetworkLayer();
    std::unique_ptr<CNLayer> CreateBuildingLayer();
    std::unique_ptr<CNLayer> CreateGreenSpaceLayer();
    void SetupLayerDependencies();
    void InitializeTopologyRules();
    std::string DetectChangeType(const CNFeature* old_f, const CNFeature* new_f);
};
```

#### 5.32.2 环境监测应用

```cpp
// 环境监测数据处理案例
class EnvironmentalMonitoring {
public:
    struct MonitoringStation {
        int64_t id;
        CNPoint location;
        std::string name;
        std::string type;
    };
    
    CNStatus ProcessSensorData(
        const std::vector<MonitoringStation>& stations,
        const std::map<int64_t, std::vector<SensorReading>>& readings
    ) {
        // 创建监测点图层
        auto station_layer = std::make_unique<CNMemoryLayer>(
            "监测站点", GeomType::kPoint);
        
        for (const auto& station : stations) {
            CNFeature feature;
            feature.SetGeometry(&station.location);
            feature.SetField("id", station.id);
            feature.SetField("name", station.name);
            feature.SetField("type", station.type);
            station_layer->CreateFeature(&feature);
        }
        
        // 空间插值生成等值线
        auto interpolated = CNSpatialInterpolation::Interpolate(
            station_layer.get(), "pm25_value", {
                .method = CNSpatialInterpolation::Method::kKriging,
                .cell_size = 100.0
            });
        
        // 热点分析
        auto hotspots = CNHotspotAnalysis::CreateHotspotLayer(
            station_layer.get(), "pm25_value", 5000.0);
        
        // 生成预警区域
        GenerateWarningZones(hotspots.get());
        
        return CNStatus::kSuccess;
    }
    
    CNStatus GeneratePollutionReport(
        const CNLayer* station_layer,
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end
    );
    
private:
    void GenerateWarningZones(const CNLayer* hotspots);
};
```

#### 5.32.3 物流配送应用

```cpp
// 物流配送路径规划案例
class LogisticsDelivery {
public:
    struct DeliveryPoint {
        int64_t id;
        CNPoint location;
        std::string address;
        int priority;
        double weight;
        std::chrono::minutes time_window_start;
        std::chrono::minutes time_window_end;
    };
    
    struct DeliveryRoute {
        int vehicle_id;
        std::vector<int64_t> delivery_sequence;
        double total_distance;
        std::chrono::minutes total_time;
        std::vector<CNPoint> path_points;
    };
    
    CNStatus OptimizeDeliveryRoutes(
        const CNLayer* road_network,
        const std::vector<DeliveryPoint>& points,
        int num_vehicles,
        std::vector<DeliveryRoute>& routes
    ) {
        // 构建路网图
        auto graph = BuildRoadNetworkGraph(road_network);
        
        // 计算配送点之间的最短路径
        auto distance_matrix = ComputeDistanceMatrix(points, graph);
        
        // 使用 VRP 算法优化路线
        routes = SolveVRP(points, distance_matrix, num_vehicles);
        
        // 生成实际路径
        for (auto& route : routes) {
            route.path_points = GenerateActualPath(
                route.delivery_sequence, points, graph);
        }
        
        return CNStatus::kSuccess;
    }
    
    std::unique_ptr<CNLayer> CreateServiceAreaLayer(
        const CNLayer* road_network,
        const CNPoint& depot,
        double max_distance
    ) {
        // 构建服务区多边形
        auto service_area = ComputeServiceArea(road_network, depot, max_distance);
        
        auto result = std::make_unique<CNMemoryLayer>(
            "服务区", GeomType::kPolygon);
        
        CNFeature feature;
        feature.SetGeometry(service_area.get());
        feature.SetField("depot_x", depot.GetX());
        feature.SetField("depot_y", depot.GetY());
        feature.SetField("max_distance", max_distance);
        result->CreateFeature(&feature);
        
        return result;
    }
    
private:
    std::unique_ptr<RoadNetworkGraph> BuildRoadNetworkGraph(const CNLayer* road_network);
    std::vector<std::vector<double>> ComputeDistanceMatrix(
        const std::vector<DeliveryPoint>& points,
        const std::unique_ptr<RoadNetworkGraph>& graph);
    std::vector<DeliveryRoute> SolveVRP(
        const std::vector<DeliveryPoint>& points,
        const std::vector<std::vector<double>>& distance_matrix,
        int num_vehicles);
    std::vector<CNPoint> GenerateActualPath(
        const std::vector<int64_t>& sequence,
        const std::vector<DeliveryPoint>& points,
        const std::unique_ptr<RoadNetworkGraph>& graph);
    std::unique_ptr<CNGeometry> ComputeServiceArea(
        const CNLayer* road_network,
        const CNPoint& depot,
        double max_distance);
};
```

#### 5.32.4 农业精准管理应用

```cpp
// 农业精准管理案例
class PrecisionAgriculture {
public:
    struct FieldZone {
        int64_t id;
        std::unique_ptr<CNPolygon> boundary;
        double area_hectares;
        std::string soil_type;
        double organic_matter;
        double ph_value;
    };
    
    CNStatus CreateManagementZones(
        const CNLayer* field_boundary,
        const std::vector<SoilSample>& soil_samples,
        const CNRasterLayer* yield_map,
        CNLayer* output_zones
    ) {
        // 1. 土壤属性插值
        auto soil_interpolated = InterpolateSoilProperties(soil_samples);
        
        // 2. 产量数据重采样
        auto yield_resampled = ResampleYieldData(yield_map, field_boundary);
        
        // 3. 多因子聚类分析
        auto clusters = MultiFactorClustering(
            soil_interpolated.get(), yield_resampled.get());
        
        // 4. 生成管理区边界
        for (const auto& cluster : clusters) {
            CNFeature zone_feature;
            zone_feature.SetGeometry(cluster.hull.get());
            zone_feature.SetField("zone_id", cluster.id);
            zone_feature.SetField("avg_yield", cluster.avg_yield);
            zone_feature.SetField("recommendation", 
                GenerateRecommendation(cluster));
            output_zones->CreateFeature(&zone_feature);
        }
        
        return CNStatus::kSuccess;
    }
    
    CNStatus GenerateVariableRateMap(
        const CNLayer* management_zones,
        const std::string& input_type,
        CNRasterLayer* output_map
    );
    
    CNStatus AnalyzeCropHealth(
        const CNRasterLayer* ndvi_image,
        const CNLayer* field_boundary,
        CNLayer* health_zones
    );
    
private:
    std::unique_ptr<CNRasterLayer> InterpolateSoilProperties(
        const std::vector<SoilSample>& samples);
    std::unique_ptr<CNRasterLayer> ResampleYieldData(
        const CNRasterLayer* yield_map, const CNLayer* boundary);
    std::vector<ClusterResult> MultiFactorClustering(
        const CNRasterLayer* soil, const CNRasterLayer* yield);
    std::string GenerateRecommendation(const ClusterResult& cluster);
};
```

---

## 6. 栅格图层与波段管理

### 6.1 栅格数据模型

#### 6.1.1 核心概念

```
┌─────────────────────────────────────────────────────────┐
│                   CNRasterLayer (栅格图层)               │
├─────────────────────────────────────────────────────────┤
│  属性:                                                  │
│  - 宽度、高度 (像素)                                    │
│  - 波段数量                                            │
│  - 数据类型 (Byte, UInt16, Float32, etc.)              │
│  - 地理变换 (GeoTransform)                             │
│  - 空间参考系统                                        │
├─────────────────────────────────────────────────────────┤
│  波段:                                                  │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐                   │
│  │ Band 1  │ │ Band 2  │ │ Band 3  │ ...               │
│  │ (Red)   │ │ (Green) │ │ (Blue)  │                   │
│  └─────────┘ └─────────┘ └─────────┘                   │
├─────────────────────────────────────────────────────────┤
│  金字塔:                                                │
│  ┌───────────────────────────────────┐                 │
│  │ Level 0 (原始分辨率)               │                 │
│  ├───────────────────────────────────┤                 │
│  │ Level 1 (1/2 分辨率)              │                 │
│  ├───────────────────────────────────┤                 │
│  │ Level 2 (1/4 分辨率)              │                 │
│  └───────────────────────────────────┘                 │
└─────────────────────────────────────────────────────────┘
```

#### 6.1.2 数据类型定义

```cpp
enum class CNRasterDataType {
    kUnknown = 0,
    kByte = 1,          // 8-bit unsigned integer
    kUInt16 = 2,        // 16-bit unsigned integer
    kInt16 = 3,         // 16-bit signed integer
    kUInt32 = 4,        // 32-bit unsigned integer
    kInt32 = 5,         // 32-bit signed integer
    kFloat32 = 6,       // 32-bit floating point
    kFloat64 = 7,       // 64-bit floating point
    kCInt16 = 8,        // Complex Int16
    kCInt32 = 9,        // Complex Int32
    kCFloat32 = 10,     // Complex Float32
    kCFloat64 = 11      // Complex Float64
};

inline int GetDataTypeSize(CNRasterDataType type) {
    static const int sizes[] = {0, 1, 2, 2, 4, 4, 4, 8, 4, 8, 8, 16};
    return sizes[static_cast<int>(type)];
}
```

### 6.2 地理参考系统

#### 6.2.1 仿射变换

```cpp
// GeoTransform 数组定义
// transform[0]: 左上角 X 坐标
// transform[1]: X 方向像素分辨率 (宽度)
// transform[2]: 旋转参数 (通常为 0)
// transform[3]: 左上角 Y 坐标
// transform[4]: 旋转参数 (通常为 0)
// transform[5]: Y 方向像素分辨率 (高度，通常为负值)

struct GeoTransform {
    double transform[6];
    
    // 像素坐标转地理坐标
    void PixelToWorld(double pixel_x, double pixel_y,
                      double& world_x, double& world_y) const {
        world_x = transform[0] + pixel_x * transform[1] + pixel_y * transform[2];
        world_y = transform[3] + pixel_x * transform[4] + pixel_y * transform[5];
    }
    
    // 地理坐标转像素坐标
    void WorldToPixel(double world_x, double world_y,
                      double& pixel_x, double& pixel_y) const {
        double det = transform[1] * transform[5] - transform[2] * transform[4];
        pixel_x = (transform[5] * (world_x - transform[0]) - 
                   transform[2] * (world_y - transform[3])) / det;
        pixel_y = (transform[1] * (world_y - transform[3]) - 
                   transform[4] * (world_x - transform[0])) / det;
    }
    
    // 获取分辨率
    double GetPixelWidth() const { return std::abs(transform[1]); }
    double GetPixelHeight() const { return std::abs(transform[5]); }
};
```

### 6.3 波段管理

#### 6.3.1 波段类设计

```cpp
class CNRasterBand {
public:
    // 基本信息
    int GetBandIndex() const { return band_index_; }
    CNRasterDataType GetDataType() const { return data_type_; }
    int GetXSize() const { return x_size_; }
    int GetYSize() const { return y_size_; }
    
    // 色彩解释
    enum class ColorInterpretation {
        kUndefined,
        kGrayIndex,
        kPaletteIndex,
        kRedBand,
        kGreenBand,
        kBlueBand,
        kAlphaBand,
        kHueBand,
        kSaturationBand,
        kLightnessBand,
        kCyanBand,
        kMagentaBand,
        kYellowBand,
        kBlackBand,
        kYCbCrYBand,
        kYCbCrCbBand,
        kYCbCrCrBand
    };
    
    ColorInterpretation GetColorInterpretation() const;
    void SetColorInterpretation(ColorInterpretation ci);
    
    // NoData 值
    bool HasNoDataValue() const;
    double GetNoDataValue() const;
    void SetNoDataValue(double value);
    
    // 块读写
    int GetBlockSizeX() const;
    int GetBlockSizeY() const;
    CNStatus ReadBlock(int x_block, int y_block, void* buffer);
    CNStatus WriteBlock(int x_block, int y_block, const void* buffer);
    
    // 栅格读写
    CNStatus ReadRaster(int x_off, int y_off, int x_size, int y_size,
                       void* buffer, int buf_x_size, int buf_y_size,
                       CNRasterDataType buf_type);
    CNStatus WriteRaster(int x_off, int y_off, int x_size, int y_size,
                        const void* buffer, int buf_x_size, int buf_y_size,
                        CNRasterDataType buf_type);
    
    // 统计信息
    struct Statistics {
        double min;
        double max;
        double mean;
        double std_dev;
        int64_t valid_count;
        int64_t null_count;
    };
    
    CNStatus GetStatistics(Statistics& stats, bool force = false);
    CNStatus ComputeStatistics(Statistics& stats, 
                               std::function<bool(double)> progress = nullptr);
    
    // 直方图
    CNStatus GetHistogram(int bucket_count, double min_val, double max_val,
                         std::vector<int64_t>& histogram, bool force = false);
    
private:
    int band_index_;
    CNRasterDataType data_type_;
    int x_size_, y_size_;
    int block_x_, block_y_;
    std::unique_ptr<BlockCache> block_cache_;
};
```

### 6.4 金字塔机制

#### 6.4.1 金字塔结构

```
原始影像 (Level 0)
├── 宽度: 10000 像素
├── 高度: 8000 像素
└── 分辨率: 1.0 米/像素

金字塔 Level 1
├── 宽度: 5000 像素
├── 高度: 4000 像素
└── 分辨率: 2.0 米/像素

金字塔 Level 2
├── 宽度: 2500 像素
├── 高度: 2000 像素
└── 分辨率: 4.0 米/像素

金字塔 Level 3
├── 宽度: 1250 像素
├── 高度: 1000 像素
└── 分辨率: 8.0 米/像素
```

#### 6.4.2 金字塔构建策略

```cpp
class CNRasterLayer {
public:
    struct PyramidLevel {
        int level;
        int width;
        int height;
        double resolution;
        std::string filename;  // 外部金字塔文件
    };
    
    CNStatus BuildPyramids(
        int levels = -1,  // -1 表示自动计算
        CNResampleMethod method = CNResampleMethod::kAverage,
        const std::vector<std::string>& options = {},
        std::function<bool(double)> progress = nullptr
    );
    
    CNStatus GetPyramidLevels(std::vector<PyramidLevel>& levels) const;
    
    int GetBestPyramidLevel(double target_resolution) const {
        double base_res = geo_transform_.GetPixelWidth();
        double ratio = target_resolution / base_res;
        
        int best_level = 0;
        double best_diff = std::abs(ratio - 1.0);
        
        for (int level = 1; level <= max_pyramid_level_; ++level) {
            double level_ratio = std::pow(2.0, level);
            double diff = std::abs(ratio - level_ratio);
            if (diff < best_diff) {
                best_diff = diff;
                best_level = level;
            }
        }
        return best_level;
    }
};
```

### 6.5 重采样算法

#### 6.5.1 重采样方法

```cpp
enum class CNResampleMethod {
    kNearest,       // 最近邻
    kBilinear,      // 双线性
    kCubic,         // 三次卷积
    kCubicSpline,   // 三次样条
    kLanczos,       // Lanczos
    kAverage,       // 平均值
    kMode,          // 众数
    kGauss          // 高斯
};
```

#### 6.5.2 算法实现

**最近邻插值**:
```cpp
double NearestInterpolate(const double* data, int width, int height,
                          double x, double y) {
    int px = static_cast<int>(std::round(x));
    int py = static_cast<int>(std::round(y));
    
    if (px < 0 || px >= width || py < 0 || py >= height) {
        return nodata_value_;
    }
    return data[py * width + px];
}
```

**双线性插值**:
```cpp
double BilinearInterpolate(const double* data, int width, int height,
                           double x, double y) {
    int x0 = static_cast<int>(std::floor(x));
    int y0 = static_cast<int>(std::floor(y));
    int x1 = x0 + 1;
    int y1 = y0 + 1;
    
    double dx = x - x0;
    double dy = y - y0;
    
    // 边界检查
    if (x0 < 0 || x1 >= width || y0 < 0 || y1 >= height) {
        return NearestInterpolate(data, width, height, x, y);
    }
    
    double v00 = data[y0 * width + x0];
    double v10 = data[y0 * width + x1];
    double v01 = data[y1 * width + x0];
    double v11 = data[y1 * width + x1];
    
    return (1 - dx) * (1 - dy) * v00 +
           dx * (1 - dy) * v10 +
           (1 - dx) * dy * v01 +
           dx * dy * v11;
}
```

**三次卷积插值**:
```cpp
double CubicInterpolate(const double* data, int width, int height,
                        double x, double y) {
    int x0 = static_cast<int>(std::floor(x)) - 1;
    int y0 = static_cast<int>(std::floor(y)) - 1;
    
    double dx = x - std::floor(x);
    double dy = y - std::floor(y);
    
    // 三次核函数
    auto cubic_weight = [](double d) -> double {
        double a = -0.5;  // 常用参数
        double abs_d = std::abs(d);
        if (abs_d <= 1) {
            return (a + 2) * abs_d * abs_d * abs_d - 
                   (a + 3) * abs_d * abs_d + 1;
        } else if (abs_d < 2) {
            return a * abs_d * abs_d * abs_d - 
                   5 * a * abs_d * abs_d + 
                   8 * a * abs_d - 4 * a;
        }
        return 0;
    };
    
    double result = 0;
    double weight_sum = 0;
    
    for (int j = 0; j < 4; ++j) {
        for (int i = 0; i < 4; ++i) {
            int px = x0 + i;
            int py = y0 + j;
            
            if (px >= 0 && px < width && py >= 0 && py < height) {
                double weight = cubic_weight(i - dx) * cubic_weight(j - dy);
                result += weight * data[py * width + px];
                weight_sum += weight;
            }
        }
    }
    
    return weight_sum > 0 ? result / weight_sum : nodata_value_;
}
```

### 6.6 块缓存机制

```cpp
class BlockCache {
public:
    explicit BlockCache(size_t max_size_bytes) 
        : max_size_(max_size_bytes), current_size_(0) {}
    
    struct BlockKey {
        int band_index;
        int x_block;
        int y_block;
        
        bool operator==(const BlockKey& other) const {
            return band_index == other.band_index &&
                   x_block == other.x_block &&
                   y_block == other.y_block;
        }
    };
    
    struct BlockKeyHash {
        size_t operator()(const BlockKey& k) const {
            return std::hash<int>()(k.band_index) ^
                   (std::hash<int>()(k.x_block) << 1) ^
                   (std::hash<int>()(k.y_block) << 2);
        }
    };
    
    void* GetBlock(const BlockKey& key) {
        auto it = cache_map_.find(key);
        if (it != cache_map_.end()) {
            // 移动到最前
            cache_list_.splice(cache_list_.begin(), cache_list_, it->second);
            return it->second->second.data.get();
        }
        return nullptr;
    }
    
    void PutBlock(const BlockKey& key, std::unique_ptr<uint8_t[]> data, 
                  size_t size) {
        // 清理空间
        while (current_size_ + size > max_size_ && !cache_list_.empty()) {
            auto& last = cache_list_.back();
            current_size_ -= last.size;
            cache_map_.erase(last.key);
            cache_list_.pop_back();
        }
        
        // 添加新块
        cache_list_.emplace_front(BlockEntry{key, std::move(data), size});
        cache_map_[key] = cache_list_.begin();
        current_size_ += size;
    }
    
private:
    struct BlockEntry {
        BlockKey key;
        std::unique_ptr<uint8_t[]> data;
        size_t size;
    };
    
    size_t max_size_;
    size_t current_size_;
    std::list<BlockEntry> cache_list_;
    std::unordered_map<BlockKey, typename std::list<BlockEntry>::iterator,
                       BlockKeyHash> cache_map_;
};
```

---

## 7. 空间索引与查询优化

### 7.1 空间索引概述

#### 7.1.1 索引类型对比

| 索引类型 | 构建时间 | 查询时间 | 更新时间 | 适用场景 |
|----------|----------|----------|----------|----------|
| R树 | O(n log n) | O(log n) | O(log n) | 动态数据、范围查询 |
| 四叉树 | O(n log n) | O(log n) | O(log n) | 点数据、均匀分布 |
| 网格索引 | O(n) | O(1) 平均 | O(1) | 静态数据、均匀分布 |
| kd树 | O(n log n) | O(log n) | O(n) | 高维数据、最近邻 |
| BSP树 | O(n log n) | O(log n) | O(n) | 计算机图形学 |

**参考链接**:
- [R-Tree与其他空间索引结构的对比](https://blog.csdn.net/qqrrjj2011/article/details/138186552)
- [空间索引简介](http://m.toutiao.com/group/7342386862985675301/)

### 7.2 R树索引

#### 7.2.1 R树结构

```
                    [根节点 MBR]
                    /          \
          [节点1 MBR]        [节点2 MBR]
          /    |    \        /    |    \
     [叶1]  [叶2]  [叶3]  [叶4]  [叶5]  [叶6]
       |      |      |      |      |      |
     要素1  要素2  要素3  要素4  要素5  要素6
```

#### 7.2.2 R树实现

```cpp
template<int MAX_ENTRIES = 16, int MIN_ENTRIES = 8>
class RTree {
public:
    struct Node {
        Envelope mbr;
        bool is_leaf;
        std::vector<std::unique_ptr<Node>> children;
        std::vector<int64_t> fids;  // 叶节点存储 FID
        
        Node() : is_leaf(false) {}
    };
    
    void Insert(int64_t fid, const Envelope& mbr) {
        InsertRecursive(root_.get(), fid, mbr);
    }
    
    std::vector<int64_t> Query(const Envelope& query_mbr) {
        std::vector<int64_t> results;
        QueryRecursive(root_.get(), query_mbr, results);
        return results;
    }
    
private:
    std::unique_ptr<Node> root_;
    
    void InsertRecursive(Node* node, int64_t fid, const Envelope& mbr) {
        if (node->is_leaf) {
            // 插入到叶节点
            node->fids.push_back(fid);
            node->mbr.ExpandToInclude(mbr);
            
            // 检查是否需要分裂
            if (node->fids.size() > MAX_ENTRIES) {
                SplitLeaf(node);
            }
        } else {
            // 选择最佳子节点
            Node* best_child = ChooseBestChild(node, mbr);
            InsertRecursive(best_child, fid, mbr);
            node->mbr.ExpandToInclude(mbr);
            
            // 检查是否需要分裂
            if (node->children.size() > MAX_ENTRIES) {
                SplitInternal(node);
            }
        }
    }
    
    void QueryRecursive(Node* node, const Envelope& query_mbr,
                        std::vector<int64_t>& results) {
        if (!node->mbr.Intersects(query_mbr)) {
            return;
        }
        
        if (node->is_leaf) {
            results.insert(results.end(), node->fids.begin(), node->fids.end());
        } else {
            for (const auto& child : node->children) {
                QueryRecursive(child.get(), query_mbr, results);
            }
        }
    }
    
    Node* ChooseBestChild(Node* node, const Envelope& mbr) {
        Node* best = nullptr;
        double min_enlargement = std::numeric_limits<double>::max();
        
        for (const auto& child : node->children) {
            Envelope enlarged = child->mbr;
            enlarged.ExpandToInclude(mbr);
            double enlargement = enlarged.GetArea() - child->mbr.GetArea();
            
            if (enlargement < min_enlargement) {
                min_enlargement = enlargement;
                best = child.get();
            }
        }
        return best;
    }
};
```

### 7.3 四叉树索引

#### 7.3.1 四叉树结构

```
                    [根节点]
                    /  |  \  \
                  NW  NE  SW  SE
                 /|   |\
                ...  ... ...
```

#### 7.3.2 四叉树实现

```cpp
template<int MAX_DEPTH = 16, int MAX_ITEMS = 16>
class QuadTree {
public:
    struct Node {
        Envelope bounds;
        int depth;
        std::vector<std::pair<int64_t, Envelope>> items;
        std::unique_ptr<Node> children[4];
        
        bool IsLeaf() const { return children[0] == nullptr; }
    };
    
    QuadTree(const Envelope& world_bounds) {
        root_ = std::make_unique<Node>();
        root_->bounds = world_bounds;
        root_->depth = 0;
    }
    
    void Insert(int64_t fid, const Envelope& mbr) {
        InsertRecursive(root_.get(), fid, mbr);
    }
    
    std::vector<int64_t> Query(const Envelope& query_mbr) {
        std::vector<int64_t> results;
        QueryRecursive(root_.get(), query_mbr, results);
        return results;
    }
    
private:
    std::unique_ptr<Node> root_;
    
    enum Quadrant { NW = 0, NE = 1, SW = 2, SE = 3 };
    
    Quadrant GetQuadrant(const Envelope& node_bounds, const Envelope& item_mbr) {
        double mid_x = (node_bounds.GetMinX() + node_bounds.GetMaxX()) / 2;
        double mid_y = (node_bounds.GetMinY() + node_bounds.GetMaxY()) / 2;
        
        bool north = item_mbr.GetMaxY() >= mid_y;
        bool south = item_mbr.GetMinY() < mid_y;
        bool west = item_mbr.GetMinX() < mid_x;
        bool east = item_mbr.GetMaxX() >= mid_x;
        
        if (north && west) return NW;
        if (north && east) return NE;
        if (south && west) return SW;
        return SE;
    }
    
    void Subdivide(Node* node) {
        double mid_x = (node->bounds.GetMinX() + node->bounds.GetMaxX()) / 2;
        double mid_y = (node->bounds.GetMinY() + node->bounds.GetMaxY()) / 2;
        
        // NW
        node->children[NW] = std::make_unique<Node>();
        node->children[NW]->bounds = Envelope(
            node->bounds.GetMinX(), mid_y,
            mid_x, node->bounds.GetMaxY()
        );
        node->children[NW]->depth = node->depth + 1;
        
        // NE
        node->children[NE] = std::make_unique<Node>();
        node->children[NE]->bounds = Envelope(
            mid_x, mid_y,
            node->bounds.GetMaxX(), node->bounds.GetMaxY()
        );
        node->children[NE]->depth = node->depth + 1;
        
        // SW
        node->children[SW] = std::make_unique<Node>();
        node->children[SW]->bounds = Envelope(
            node->bounds.GetMinX(), node->bounds.GetMinY(),
            mid_x, mid_y
        );
        node->children[SW]->depth = node->depth + 1;
        
        // SE
        node->children[SE] = std::make_unique<Node>();
        node->children[SE]->bounds = Envelope(
            mid_x, node->bounds.GetMinY(),
            node->bounds.GetMaxX(), mid_y
        );
        node->children[SE]->depth = node->depth + 1;
    }
    
    void InsertRecursive(Node* node, int64_t fid, const Envelope& mbr) {
        if (node->IsLeaf()) {
            if (node->items.size() < MAX_ITEMS || node->depth >= MAX_DEPTH) {
                node->items.emplace_back(fid, mbr);
            } else {
                Subdivide(node);
                // 重新分配现有项
                for (auto& item : node->items) {
                    Redistribute(node, item.first, item.second);
                }
                node->items.clear();
                Redistribute(node, fid, mbr);
            }
        } else {
            Redistribute(node, fid, mbr);
        }
    }
    
    void Redistribute(Node* node, int64_t fid, const Envelope& mbr) {
        // 简化：插入到所有相交的象限
        for (int i = 0; i < 4; ++i) {
            if (node->children[i]->bounds.Intersects(mbr)) {
                InsertRecursive(node->children[i].get(), fid, mbr);
            }
        }
    }
    
    void QueryRecursive(Node* node, const Envelope& query_mbr,
                        std::vector<int64_t>& results) {
        if (!node->bounds.Intersects(query_mbr)) {
            return;
        }
        
        for (const auto& item : node->items) {
            if (item.second.Intersects(query_mbr)) {
                results.push_back(item.first);
            }
        }
        
        if (!node->IsLeaf()) {
            for (int i = 0; i < 4; ++i) {
                QueryRecursive(node->children[i].get(), query_mbr, results);
            }
        }
    }
};
```

### 7.4 网格索引

```cpp
class GridIndex {
public:
    GridIndex(const Envelope& bounds, int cell_count_x, int cell_count_y)
        : bounds_(bounds), 
          cell_count_x_(cell_count_x), 
          cell_count_y_(cell_count_y) {
        cells_.resize(cell_count_x * cell_count_y);
    }
    
    void Insert(int64_t fid, const Envelope& mbr) {
        int min_x, min_y, max_x, max_y;
        GetCellRange(mbr, min_x, min_y, max_x, max_y);
        
        for (int y = min_y; y <= max_y; ++y) {
            for (int x = min_x; x <= max_x; ++x) {
                cells_[y * cell_count_x_ + x].push_back(fid);
            }
        }
    }
    
    std::vector<int64_t> Query(const Envelope& query_mbr) {
        std::unordered_set<int64_t> result_set;
        
        int min_x, min_y, max_x, max_y;
        GetCellRange(query_mbr, min_x, min_y, max_x, max_y);
        
        for (int y = min_y; y <= max_y; ++y) {
            for (int x = min_x; x <= max_x; ++x) {
                for (int64_t fid : cells_[y * cell_count_x_ + x]) {
                    result_set.insert(fid);
                }
            }
        }
        
        return std::vector<int64_t>(result_set.begin(), result_set.end());
    }
    
private:
    Envelope bounds_;
    int cell_count_x_, cell_count_y_;
    std::vector<std::vector<int64_t>> cells_;
    
    void GetCellRange(const Envelope& mbr, int& min_x, int& min_y,
                      int& max_x, int& max_y) {
        double cell_width = bounds_.GetWidth() / cell_count_x_;
        double cell_height = bounds_.GetHeight() / cell_count_y_;
        
        min_x = std::max(0, static_cast<int>(
            (mbr.GetMinX() - bounds_.GetMinX()) / cell_width));
        min_y = std::max(0, static_cast<int>(
            (mbr.GetMinY() - bounds_.GetMinY()) / cell_height));
        max_x = std::min(cell_count_x_ - 1, static_cast<int>(
            (mbr.GetMaxX() - bounds_.GetMinX()) / cell_width));
        max_y = std::min(cell_count_y_ - 1, static_cast<int>(
            (mbr.GetMaxY() - bounds_.GetMinY()) / cell_height));
    }
};
```

### 7.5 索引选择策略

```cpp
enum class SpatialIndexType {
    kNone,
    kRTree,
    kQuadTree,
    kGrid
};

class SpatialIndexFactory {
public:
    static std::unique_ptr<SpatialIndex> CreateIndex(
        SpatialIndexType type,
        const Envelope& bounds,
        int64_t estimated_count
    ) {
        switch (type) {
            case SpatialIndexType::kRTree:
                return std::make_unique<RTree<>>();
                
            case SpatialIndexType::kQuadTree:
                return std::make_unique<QuadTree<>>(bounds);
                
            case SpatialIndexType::kGrid: {
                // 根据数据量自动计算网格大小
                int cell_count = static_cast<int>(std::sqrt(estimated_count));
                return std::make_unique<GridIndex>(bounds, cell_count, cell_count);
            }
                
            default:
                return nullptr;
        }
    }
    
    static SpatialIndexType RecommendIndex(
        int64_t feature_count,
        bool is_dynamic,
        const std::string& query_type
    ) {
        if (feature_count < 1000) {
            return SpatialIndexType::kNone;  // 小数据量无需索引
        }
        
        if (is_dynamic) {
            return SpatialIndexType::kRTree;  // 动态数据用 R 树
        }
        
        if (query_type == "range") {
            return SpatialIndexType::kRTree;
        } else if (query_type == "point") {
            return SpatialIndexType::kQuadTree;
        }
        
        return SpatialIndexType::kGrid;
    }
};
```

---

## 8. 多线程并发访问策略

### 8.1 线程安全设计原则

#### 8.1.1 线程安全级别

| 级别 | 描述 | 实现难度 |
|------|------|----------|
| 不可变 | 对象创建后不可修改 | 低 |
| 无条件线程安全 | 所有操作都是线程安全的 | 中 |
| 有条件线程安全 | 部分操作需要外部同步 | 中 |
| 非线程安全 | 需要外部同步所有操作 | 低 |

**推荐**: 有条件线程安全
- 只读操作：线程安全
- 写操作：需要外部同步或内部锁

#### 8.1.2 同步原语选择

```cpp
// 互斥锁
std::mutex mutex_;

// 读写锁 (C++17: std::shared_mutex, C++11: 第三方库或自实现)
std::shared_mutex rw_lock_;

// 递归锁
std::recursive_mutex recursive_mutex_;

// 自旋锁 (适用于短临界区)
std::atomic_flag spin_lock_ = ATOMIC_FLAG_INIT;

// 条件变量
std::condition_variable cv_;
```

### 8.2 读写锁实现

#### 8.2.1 C++11 兼容的读写锁

```cpp
class ReadWriteLock {
public:
    ReadWriteLock() : reader_count_(0), writer_waiting_(0), writer_active_(false) {}
    
    void LockRead() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (writer_active_ || writer_waiting_ > 0) {
            read_cv_.wait(lock);
        }
        ++reader_count_;
    }
    
    void UnlockRead() {
        std::unique_lock<std::mutex> lock(mutex_);
        --reader_count_;
        if (reader_count_ == 0) {
            write_cv_.notify_one();
        }
    }
    
    void LockWrite() {
        std::unique_lock<std::mutex> lock(mutex_);
        ++writer_waiting_;
        while (reader_count_ > 0 || writer_active_) {
            write_cv_.wait(lock);
        }
        --writer_waiting_;
        writer_active_ = true;
    }
    
    void UnlockWrite() {
        std::unique_lock<std::mutex> lock(mutex_);
        writer_active_ = false;
        if (writer_waiting_ > 0) {
            write_cv_.notify_one();
        } else {
            read_cv_.notify_all();
        }
    }
    
private:
    std::mutex mutex_;
    std::condition_variable read_cv_;
    std::condition_variable write_cv_;
    int reader_count_;
    int writer_waiting_;
    bool writer_active_;
};

// RAII 包装器
class ReadLockGuard {
public:
    explicit ReadLockGuard(ReadWriteLock& lock) : lock_(lock) {
        lock_.LockRead();
    }
    ~ReadLockGuard() { lock_.UnlockRead(); }
private:
    ReadWriteLock& lock_;
};

class WriteLockGuard {
public:
    explicit WriteLockGuard(ReadWriteLock& lock) : lock_(lock) {
        lock_.LockWrite();
    }
    ~WriteLockGuard() { lock_.UnlockWrite(); }
private:
    ReadWriteLock& lock_;
};
```

### 8.3 图层线程安全实现

#### 8.3.1 细粒度锁策略

```cpp
class CNMemoryLayer {
private:
    // 数据锁：保护要素数据
    mutable ReadWriteLock data_lock_;
    
    // 过滤器锁：保护过滤器状态
    mutable std::mutex filter_lock_;
    
    // 事务锁：保护事务状态
    mutable std::mutex transaction_lock_;
    
    // 读取游标：线程局部存储
    thread_local static size_t tls_read_cursor_;
    
public:
    // 只读操作：读锁
    CNFeature* GetFeature(int64_t fid) {
        ReadLockGuard guard(data_lock_);
        return GetFeatureInternal(fid);
    }
    
    int64_t GetFeatureCount(bool force) {
        ReadLockGuard guard(data_lock_);
        return GetFeatureCountInternal(force);
    }
    
    // 写操作：写锁
    CNStatus CreateFeature(CNFeature* feature) {
        WriteLockGuard guard(data_lock_);
        return CreateFeatureInternal(feature);
    }
    
    CNStatus DeleteFeature(int64_t fid) {
        WriteLockGuard guard(data_lock_);
        return DeleteFeatureInternal(fid);
    }
    
    // 过滤器操作：独立锁
    CNStatus SetSpatialFilter(const Envelope& filter) {
        std::lock_guard<std::mutex> guard(filter_lock_);
        spatial_filter_ = filter;
        return CNStatus::kSuccess;
    }
    
    // 非阻塞尝试操作
    bool TryGetFeature(int64_t fid, CNFeature*& feature) noexcept {
        if (!data_lock_.TryLockRead()) {
            return false;
        }
        feature = GetFeatureInternal(fid);
        data_lock_.UnlockRead();
        return true;
    }
};
```

#### 8.3.2 线程局部存储

```cpp
class CNMemoryLayer {
private:
    // 每个线程独立的读取游标
    struct ThreadReadState {
        size_t cursor = 0;
        std::unique_ptr<SpatialFilter> filter_snapshot;
    };
    
    std::unordered_map<std::thread::id, ThreadReadState> thread_states_;
    std::mutex thread_states_lock_;
    
public:
    void ResetReading() {
        auto& state = GetThreadState();
        state.cursor = 0;
    }
    
    CNFeature* GetNextFeature() {
        auto& state = GetThreadState();
        
        ReadLockGuard guard(data_lock_);
        
        while (state.cursor < features_.size()) {
            CNFeature* feature = features_[state.cursor++].get();
            
            // 应用过滤器
            if (PassesFilter(feature, state.filter_snapshot.get())) {
                return feature;
            }
        }
        return nullptr;
    }
    
private:
    ThreadReadState& GetThreadState() {
        std::lock_guard<std::mutex> guard(thread_states_lock_);
        auto tid = std::this_thread::get_id();
        return thread_states_[tid];
    }
};
```

### 8.4 并发事务处理

#### 8.4.1 事务隔离级别

```cpp
enum class TransactionIsolation {
    kReadUncommitted,   // 读未提交
    kReadCommitted,     // 读已提交
    kRepeatableRead,    // 可重复读
    kSerializable       // 可串行化
};
```

#### 8.4.2 MVCC 实现

```cpp
class CNMemoryLayer {
private:
    struct VersionedFeature {
        int64_t fid;
        std::unique_ptr<CNFeature> feature;
        int64_t create_version;  // 创建版本号
        int64_t delete_version;  // 删除版本号（INT64_MAX 表示未删除）
    };
    
    std::atomic<int64_t> version_counter_{0};
    std::vector<VersionedFeature> features_;
    
public:
    CNFeature* GetFeature(int64_t fid, int64_t read_version) {
        ReadLockGuard guard(data_lock_);
        
        for (const auto& vf : features_) {
            if (vf.fid == fid) {
                // 检查版本可见性
                if (vf.create_version <= read_version &&
                    vf.delete_version > read_version) {
                    return vf.feature.get();
                }
            }
        }
        return nullptr;
    }
    
    CNStatus CreateFeature(CNFeature* feature) {
        WriteLockGuard guard(data_lock_);
        
        int64_t version = version_counter_.fetch_add(1) + 1;
        
        VersionedFeature vf;
        vf.fid = feature->GetFID();
        vf.feature.reset(feature);
        vf.create_version = version;
        vf.delete_version = INT64_MAX;
        
        features_.push_back(std::move(vf));
        return CNStatus::kSuccess;
    }
    
    CNStatus DeleteFeature(int64_t fid) {
        WriteLockGuard guard(data_lock_);
        
        int64_t version = version_counter_.fetch_add(1) + 1;
        
        for (auto& vf : features_) {
            if (vf.fid == fid && vf.delete_version == INT64_MAX) {
                vf.delete_version = version;
                return CNStatus::kSuccess;
            }
        }
        return CNStatus::kNotFound;
    }
};
```

### 8.5 连接池设计

#### 8.5.1 连接池架构

对于数据库图层（如 PostGIS、SpatiaLite），连接池是提高性能的关键组件：

```cpp
class CNConnectionPool {
public:
    struct ConnectionConfig {
        std::string host;
        int port;
        std::string database;
        std::string username;
        std::string password;
        int max_connections = 10;
        int min_connections = 2;
        int connection_timeout_ms = 5000;
        int idle_timeout_ms = 300000;  // 5分钟
    };
    
    explicit CNConnectionPool(const ConnectionConfig& config);
    ~CNConnectionPool();
    
    // 获取连接
    std::shared_ptr<CNConnection> AcquireConnection(int timeout_ms = -1);
    
    // 释放连接
    void ReleaseConnection(std::shared_ptr<CNConnection> conn);
    
    // 连接池状态
    int GetActiveConnectionCount() const;
    int GetIdleConnectionCount() const;
    
    // 健康检查
    CNStatus HealthCheck();
    
    // 关闭所有连接
    void CloseAll();
    
private:
    ConnectionConfig config_;
    std::queue<std::shared_ptr<CNConnection>> idle_connections_;
    std::set<std::shared_ptr<CNConnection>> active_connections_;
    mutable std::mutex pool_mutex_;
    std::condition_variable pool_cv_;
    std::thread cleanup_thread_;
    std::atomic<bool> shutdown_{false};
    
    void CleanupThread();
    std::shared_ptr<CNConnection> CreateConnection();
};
```

#### 8.5.2 连接池实现

```cpp
std::shared_ptr<CNConnection> CNConnectionPool::AcquireConnection(int timeout_ms) {
    std::unique_lock<std::mutex> lock(pool_mutex_);
    
    auto deadline = std::chrono::steady_clock::now() + 
                    std::chrono::milliseconds(timeout_ms);
    
    while (idle_connections_.empty() && 
           active_connections_.size() >= config_.max_connections) {
        if (timeout_ms >= 0) {
            if (pool_cv_.wait_until(lock, deadline) == std::cv_status::timeout) {
                return nullptr;  // 超时
            }
        } else {
            pool_cv_.wait(lock);
        }
    }
    
    std::shared_ptr<CNConnection> conn;
    
    if (!idle_connections_.empty()) {
        conn = idle_connections_.front();
        idle_connections_.pop();
    } else {
        conn = CreateConnection();
        if (!conn) {
            return nullptr;
        }
    }
    
    active_connections_.insert(conn);
    return conn;
}

void CNConnectionPool::ReleaseConnection(std::shared_ptr<CNConnection> conn) {
    std::lock_guard<std::mutex> lock(pool_mutex_);
    
    active_connections_.erase(conn);
    
    // 检查连接是否有效
    if (conn->IsValid() && idle_connections_.size() < config_.max_connections) {
        idle_connections_.push(conn);
    }
    
    pool_cv_.notify_one();
}

void CNConnectionPool::CleanupThread() {
    while (!shutdown_) {
        std::this_thread::sleep_for(std::chrono::minutes(1));
        
        std::lock_guard<std::mutex> lock(pool_mutex_);
        
        auto now = std::chrono::steady_clock::now();
        while (!idle_connections_.empty()) {
            auto& conn = idle_connections_.front();
            auto idle_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - conn->GetLastUsedTime()).count();
            
            if (idle_time > config_.idle_timeout_ms && 
                idle_connections_.size() > config_.min_connections) {
                idle_connections_.pop();
            } else {
                break;
            }
        }
    }
}
```

#### 8.5.3 RAII 连接守卫

```cpp
class CNConnectionGuard {
public:
    CNConnectionGuard(CNConnectionPool& pool, int timeout_ms = -1)
        : pool_(pool), 
          connection_(pool_.AcquireConnection(timeout_ms)) {}
    
    ~CNConnectionGuard() {
        if (connection_) {
            pool_.ReleaseConnection(connection_);
        }
    }
    
    CNConnectionGuard(const CNConnectionGuard&) = delete;
    CNConnectionGuard& operator=(const CNConnectionGuard&) = delete;
    
    CNConnection* Get() { return connection_.get(); }
    CNConnection* operator->() { return connection_.get(); }
    bool IsValid() const { return connection_ != nullptr; }
    
private:
    CNConnectionPool& pool_;
    std::shared_ptr<CNConnection> connection_;
};

// 使用示例
void QueryPostGIS(CNConnectionPool& pool) {
    CNConnectionGuard guard(pool, 5000);
    if (!guard.IsValid()) {
        throw std::runtime_error("Failed to acquire connection");
    }
    
    auto result = guard->ExecuteQuery("SELECT * FROM layers");
    // 处理结果...
}
```

### 8.6 图层连接（Join）支持

#### 8.6.1 连接类型定义

```cpp
enum class CNJoinType {
    kInner,      // 内连接
    kLeft,       // 左连接
    kRight,      // 右连接
    kFull        // 全连接
};

struct CNLayerJoinInfo {
    std::string target_field;           // 目标图层字段
    CNLayer* join_layer;                // 连接图层
    std::string join_field;             // 连接图层字段
    CNJoinType join_type;               // 连接类型
    std::vector<std::string> prefix_fields;  // 字段前缀
};
```

#### 8.6.2 连接实现

```cpp
class CNLayerJoin {
public:
    CNStatus ExecuteJoin(const CNLayerJoinInfo& join_info);
    
    CNFeature* GetJoinedFeature(int64_t fid) {
        CNFeature* base_feature = base_layer_->GetFeature(fid);
        if (!base_feature) return nullptr;
        
        // 获取连接字段的值
        auto target_value = base_feature->GetFieldValue(join_info_.target_field);
        
        // 在连接图层中查找匹配要素
        join_info_.join_layer->SetAttributeFilter(
            (join_info_.join_field + " = '" + target_value.ToString() + "'").c_str()
        );
        
        CNFeature* join_feature = join_info_.join_layer->GetNextFeature();
        
        // 合并属性
        return MergeFeatures(base_feature, join_feature);
    }
    
private:
    CNLayer* base_layer_;
    CNLayerJoinInfo join_info_;
    std::unordered_map<int64_t, CNFeature*> join_cache_;
    
    CNFeature* MergeFeatures(CNFeature* base, CNFeature* join);
};
```

### 8.7 PostGIS 图层管理

#### 8.7.1 PostGIS 特定功能

```cpp
class CNPostGISLayer : public CNVectorLayer {
public:
    // PostGIS 特有的空间索引操作
    CNStatus CreateGISTIndex(const std::string& geom_column);
    CNStatus DropGISTIndex(const std::string& index_name);
    
    // 空间分析函数
    CNLayer* ST_Buffer(double distance, int segments = 8);
    CNLayer* ST_Intersection(CNLayer* other);
    CNLayer* ST_Union(CNLayer* other);
    CNLayer* ST_Difference(CNLayer* other);
    
    // 空间关系查询
    CNLayer* ST_Within(CNGeometry* geom);
    CNLayer* ST_Contains(CNGeometry* geom);
    CNLayer* ST_Intersects(CNGeometry* geom);
    CNLayer* ST_DWithin(CNGeometry* geom, double distance);
    
    // 几何处理
    CNStatus ST_Transform(int target_srid);
    CNStatus ST_Simplify(double tolerance);
    CNStatus ST_SnapToGrid(double size_x, double size_y);
    
    // 元数据操作
    CNStatus UpdateGeometryColumns();
    CNStatus RegisterGeometryColumn(const std::string& column, 
                                    int srid, 
                                    const std::string& geom_type,
                                    int coord_dimension);
    
private:
    std::shared_ptr<CNConnection> connection_;
    std::string schema_name_;
    std::string table_name_;
};
```

#### 8.7.2 PostGIS 性能优化

```cpp
// 批量插入优化
CNStatus CNPostGISLayer::BulkInsert(const std::vector<CNFeature*>& features) {
    // 使用 COPY 命令批量导入
    std::string copy_sql = "COPY " + table_name_ + " FROM STDIN";
    
    auto copy_stream = connection_->BeginCopy(copy_sql);
    
    for (const auto& feature : features) {
        std::string row = FeatureToCSVRow(feature);
        copy_stream->Write(row + "\n");
    }
    
    copy_stream->End();
    return CNStatus::kSuccess;
}

// 空间查询优化
CNLayer* CNPostGISLayer::SpatialQueryOptimized(
    const std::string& geom_column,
    CNGeometry* filter_geom,
    const std::string& predicate
) {
    // 使用 && 操作符进行 MBR 过滤
    // 然后使用精确谓词进行二次过滤
    std::string sql = StringFormat(
        "SELECT * FROM %s WHERE %s && ST_GeomFromText('%s', %d) "
        "AND %s(%s, ST_GeomFromText('%s', %d))",
        table_name_.c_str(),
        geom_column.c_str(),
        filter_geom->ToWKT().c_str(),
        filter_geom->GetSRID(),
        predicate.c_str(),
        geom_column.c_str(),
        filter_geom->ToWKT().c_str(),
        filter_geom->GetSRID()
    );
    
    return connection_->ExecuteQuery(sql);
}
```

### 8.8 批量操作优化

```cpp
class CNBatchProcessor {
public:
    CNBatchProcessor(CNLayer* layer, int batch_size = 1000)
        : layer_(layer), batch_size_(batch_size) {}
    
    CNStatus AddFeature(CNFeature* feature) {
        std::lock_guard<std::mutex> guard(batch_lock_);
        
        batch_.push_back(feature);
        
        if (batch_.size() >= batch_size_) {
            return Flush();
        }
        return CNStatus::kSuccess;
    }
    
    CNStatus Flush() {
        if (batch_.empty()) {
            return CNStatus::kSuccess;
        }
        
        // 批量写入
        WriteLockGuard guard(layer_->GetWriteLock());
        
        for (CNFeature* feature : batch_) {
            CNStatus status = layer_->CreateFeatureInternal(feature);
            if (status != CNStatus::kSuccess) {
                batch_.clear();
                return status;
            }
        }
        
        batch_.clear();
        return CNStatus::kSuccess;
    }
    
    // 并行处理
    CNStatus ProcessParallel(
        const std::vector<CNFeature*>& features,
        std::function<CNStatus(CNFeature*)> processor,
        int thread_count = 4
    ) {
        std::vector<std::thread> threads;
        std::atomic<size_t> index{0};
        std::atomic<int> error_count{0};
        
        for (int t = 0; t < thread_count; ++t) {
            threads.emplace_back([&]() {
                while (true) {
                    size_t i = index.fetch_add(1);
                    if (i >= features.size() || error_count > 0) {
                        break;
                    }
                    
                    CNStatus status = processor(features[i]);
                    if (status != CNStatus::kSuccess) {
                        error_count.fetch_add(1);
                    }
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        return error_count > 0 ? CNStatus::kError : CNStatus::kSuccess;
    }
    
private:
    CNLayer* layer_;
    int batch_size_;
    std::vector<CNFeature*> batch_;
    std::mutex batch_lock_;
};
```

---

## 9. 现代 C++ 实现最佳实践

### 9.1 智能指针应用

#### 9.1.1 所有权语义

```cpp
// unique_ptr: 唯一所有权
class CNFeature {
private:
    std::unique_ptr<CNGeometry> geometry_;
    
public:
    void SetGeometry(std::unique_ptr<CNGeometry> geom) {
        geometry_ = std::move(geom);
    }
    
    std::unique_ptr<CNGeometry> ReleaseGeometry() {
        return std::move(geometry_);
    }
    
    const CNGeometry* GetGeometry() const {
        return geometry_.get();
    }
};

// shared_ptr: 共享所有权
class CNFeatureDefn {
private:
    std::shared_ptr<CNLayerDefn> layer_defn_;
    
public:
    CNFeatureDefn(std::shared_ptr<CNLayerDefn> defn)
        : layer_defn_(std::move(defn)) {}
};

// weak_ptr: 非拥有引用
class CNLayer {
private:
    std::weak_ptr<CNDataSource> data_source_;
    
public:
    std::shared_ptr<CNDataSource> GetDataSource() const {
        return data_source_.lock();
    }
};
```

#### 9.1.2 工厂模式与智能指针

```cpp
class CNLayerFactory {
public:
    static std::unique_ptr<CNVectorLayer> OpenVectorLayer(
        const std::string& filename,
        const std::string& layer_name = "",
        OpenMode mode = OpenMode::kReadOnly
    ) {
        auto layer = std::unique_ptr<CNVectorLayer>(new CNVectorLayer());
        if (layer->Open(filename, layer_name, mode)) {
            return layer;
        }
        return nullptr;
    }
    
    static std::unique_ptr<CNMemoryLayer> CreateMemoryLayer(
        const std::string& name, GeomType geom_type) {
        return std::unique_ptr<CNMemoryLayer>(new CNMemoryLayer(name, geom_type));
    }
};
```

### 9.2 移动语义

#### 9.2.1 移动构造与移动赋值

```cpp
class CNFeature {
public:
    // 移动构造
    CNFeature(CNFeature&& other) noexcept
        : fid_(other.fid_),
          geometry_(std::move(other.geometry_)),
          field_values_(std::move(other.field_values_)) {
        other.fid_ = -1;
    }
    
    // 移动赋值
    CNFeature& operator=(CNFeature&& other) noexcept {
        if (this != &other) {
            fid_ = other.fid_;
            geometry_ = std::move(other.geometry_);
            field_values_ = std::move(other.field_values_);
            other.fid_ = -1;
        }
        return *this;
    }
    
    // 禁止拷贝
    CNFeature(const CNFeature&) = delete;
    CNFeature& operator=(const CNFeature&) = delete;
};
```

### 9.3 PIMPL 模式

```cpp
// CNLayer.h
class CNLayer {
public:
    CNLayer();
    ~CNLayer();
    
    CNFeature* GetNextFeature();
    CNStatus CreateFeature(CNFeature* feature);
    // ...
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// CNLayer.cpp
class CNLayer::Impl {
public:
    std::vector<std::unique_ptr<CNFeature>> features_;
    size_t read_cursor_ = 0;
    ReadWriteLock data_lock_;
    // ... 私有实现细节
};

CNLayer::CNLayer() : impl_(std::unique_ptr<Impl>(new Impl())) {}
CNLayer::~CNLayer() = default;

CNFeature* CNLayer::GetNextFeature() {
    return impl_->GetNextFeature();
}
```

### 9.4 RAII 资源管理

```cpp
// 要素守卫
class CNFeatureGuard {
public:
    explicit CNFeatureGuard(CNFeature* feature) : feature_(feature) {}
    ~CNFeatureGuard() { if (feature_) delete feature_; }
    
    CNFeatureGuard(const CNFeatureGuard&) = delete;
    CNFeatureGuard& operator=(const CNFeatureGuard&) = delete;
    
    CNFeatureGuard(CNFeatureGuard&& other) noexcept : feature_(other.feature_) {
        other.feature_ = nullptr;
    }
    
    CNFeature* Get() const { return feature_; }
    CNFeature* Release() { 
        CNFeature* f = feature_; 
        feature_ = nullptr; 
        return f; 
    }
    
private:
    CNFeature* feature_;
};

// 使用示例
void ProcessLayer(CNLayer* layer) {
    layer->ResetReading();
    while (true) {
        CNFeatureGuard guard(layer->GetNextFeature());
        if (!guard.Get()) break;
        ProcessFeature(guard.Get());
    }
}
```

### 9.5 迭代器模式

```cpp
class CNLayerIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = CNFeature;
    using difference_type = std::ptrdiff_t;
    using pointer = CNFeature*;
    using reference = CNFeature&;
    
    CNLayerIterator(CNLayer* layer, bool end = false)
        : layer_(layer), is_end_(end) {
        if (!is_end_) {
            layer_->ResetReading();
            Advance();
        }
    }
    
    reference operator*() { return *current_; }
    pointer operator->() { return current_; }
    
    CNLayerIterator& operator++() {
        Advance();
        return *this;
    }
    
    bool operator==(const CNLayerIterator& other) const {
        return is_end_ == other.is_end_;
    }
    
    bool operator!=(const CNLayerIterator& other) const {
        return !(*this == other);
    }
    
private:
    void Advance() {
        current_ = layer_->GetNextFeature();
        if (!current_) {
            is_end_ = true;
        }
    }
    
    CNLayer* layer_;
    CNFeature* current_ = nullptr;
    bool is_end_;
};

// 使用示例
for (auto it = CNLayerIterator(layer); it != CNLayerIterator(layer, true); ++it) {
    ProcessFeature(*it);
}
```

### 9.6 错误处理与异常安全

#### 9.6.1 状态码设计

```cpp
enum class CNStatus {
    kSuccess = 0,
    kError = 1,
    kInvalidParameter = 2,
    kNotFound = 3,
    kAlreadyExists = 4,
    kOutOfMemory = 5,
    kIOError = 6,
    kFormatError = 7,
    kPermissionDenied = 8,
    kNotSupported = 9,
    kTimeout = 10,
    kTransactionError = 11,
    kConnectionError = 12,
    kGeometryError = 13
};

class CNResult {
public:
    CNResult(CNStatus status) : status_(status) {}
    CNResult(CNStatus status, const std::string& message) 
        : status_(status), message_(message) {}
    
    bool IsSuccess() const { return status_ == CNStatus::kSuccess; }
    CNStatus GetStatus() const { return status_; }
    const std::string& GetMessage() const { return message_; }
    
    static CNResult Success() { return CNResult(CNStatus::kSuccess); }
    static CNResult Error(const std::string& msg) { 
        return CNResult(CNStatus::kError, msg); 
    }
    
private:
    CNStatus status_;
    std::string message_;
};
```

#### 9.6.2 异常安全级别

```cpp
// 基本保证：操作失败时对象仍处于有效状态
class CNMemoryLayer {
public:
    CNStatus CreateFeature(CNFeature* feature) {
        WriteLockGuard guard(data_lock_);
        
        // 先验证
        if (!feature || !ValidateFeature(feature)) {
            return CNStatus::kInvalidParameter;
        }
        
        // 再执行
        try {
            features_.push_back(feature->Clone());
            return CNStatus::kSuccess;
        } catch (const std::bad_alloc&) {
            return CNStatus::kOutOfMemory;
        }
    }
};

// 强保证：操作失败时回滚到操作前状态
CNStatus CNMemoryLayer::BatchInsert(const std::vector<CNFeature*>& features) {
    WriteLockGuard guard(data_lock_);
    
    // 保存当前状态
    auto backup_size = features_.size();
    
    for (auto* feature : features) {
        try {
            features_.push_back(feature->Clone());
        } catch (...) {
            // 回滚
            while (features_.size() > backup_size) {
                features_.pop_back();
            }
            return CNStatus::kError;
        }
    }
    return CNStatus::kSuccess;
}
```

### 9.7 跨平台兼容性

#### 9.7.1 平台抽象层

```cpp
// 平台检测
#if defined(_WIN32) || defined(_WIN64)
    #define CN_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define CN_PLATFORM_LINUX
#elif defined(__APPLE__)
    #define CN_PLATFORM_MACOS
#endif

// 文件路径处理
class CNPath {
public:
    static std::string Join(const std::string& a, const std::string& b) {
#ifdef CN_PLATFORM_WINDOWS
        return a + "\\" + b;
#else
        return a + "/" + b;
#endif
    }
    
    static char GetSeparator() {
#ifdef CN_PLATFORM_WINDOWS
        return '\\';
#else
        return '/';
#endif
    }
    
    static std::string Normalize(const std::string& path) {
        std::string result = path;
#ifdef CN_PLATFORM_WINDOWS
        std::replace(result.begin(), result.end(), '/', '\\');
#else
        std::replace(result.begin(), result.end(), '\\', '/');
#endif
        return result;
    }
};
```

#### 9.7.2 字符编码处理

```cpp
class CNStringEncoding {
public:
#ifdef CN_PLATFORM_WINDOWS
    // Windows: UTF-16 <-> UTF-8 转换
    static std::wstring Utf8ToWide(const std::string& utf8) {
        if (utf8.empty()) return std::wstring();
        
        int len = MultiByteToWideChar(CP_UTF8, 0, 
            utf8.c_str(), -1, nullptr, 0);
        std::wstring wide(len - 1, 0);
        MultiByteToWideChar(CP_UTF8, 0, 
            utf8.c_str(), -1, &wide[0], len);
        return wide;
    }
    
    static std::string WideToUtf8(const std::wstring& wide) {
        if (wide.empty()) return std::string();
        
        int len = WideCharToMultiByte(CP_UTF8, 0, 
            wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
        std::string utf8(len - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, 
            wide.c_str(), -1, &utf8[0], len, nullptr, nullptr);
        return utf8;
    }
#else
    // Unix: 假设系统使用 UTF-8
    static std::string Utf8ToNative(const std::string& utf8) {
        return utf8;
    }
#endif
};
```

#### 9.7.3 共享库导出

```cpp
// 导出宏定义
#ifdef CN_PLATFORM_WINDOWS
    #ifdef CN_BUILDING_DLL
        #define CN_API __declspec(dllexport)
    #else
        #define CN_API __declspec(dllimport)
    #endif
#else
    #define CN_API __attribute__((visibility("default")))
#endif

// 使用示例
class CN_API CNLayer {
    // ...
};
```

### 9.8 日志与调试支持

#### 9.8.1 日志系统

```cpp
enum class CNLogLevel {
    kDebug,
    kInfo,
    kWarning,
    kError,
    kFatal
};

class CNLogger {
public:
    static CNLogger& Instance() {
        static CNLogger instance;
        return instance;
    }
    
    void SetLevel(CNLogLevel level) { level_ = level; }
    void SetOutput(std::ostream* output) { output_ = output; }
    
    void Log(CNLogLevel level, const char* file, int line, 
             const std::string& message) {
        if (level < level_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        const char* level_str[] = {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
        
        *output_ << "[" << level_str[static_cast<int>(level)] << "] "
                 << file << ":" << line << " - " 
                 << message << std::endl;
    }
    
private:
    CNLogger() : level_(CNLogLevel::kInfo), output_(&std::cerr) {}
    
    CNLogLevel level_;
    std::ostream* output_;
    std::mutex mutex_;
};

// 日志宏
#define CN_LOG(level, msg) \
    CNLogger::Instance().Log(level, __FILE__, __LINE__, msg)

#define CN_DEBUG(msg) CN_LOG(CNLogLevel::kDebug, msg)
#define CN_INFO(msg) CN_LOG(CNLogLevel::kInfo, msg)
#define CN_WARN(msg) CN_LOG(CNLogLevel::kWarning, msg)
#define CN_ERROR(msg) CN_LOG(CNLogLevel::kError, msg)
```

#### 9.8.2 性能追踪

```cpp
class CNProfiler {
public:
    struct ScopeTimer {
        ScopeTimer(const std::string& name) 
            : name_(name), start_(std::chrono::high_resolution_clock::now()) {}
        
        ~ScopeTimer() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
                end - start_).count();
            CNProfiler::Instance().Record(name_, duration);
        }
        
        std::string name_;
        std::chrono::high_resolution_clock::time_point start_;
    };
    
    static CNProfiler& Instance() {
        static CNProfiler instance;
        return instance;
    }
    
    void Record(const std::string& name, int64_t microseconds) {
        std::lock_guard<std::mutex> lock(mutex_);
        records_[name].push_back(microseconds);
    }
    
    void PrintStats() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& pair : records_) {
            auto& times = pair.second;
            int64_t total = 0;
            for (auto t : times) total += t;
            double avg = static_cast<double>(total) / times.size();
            
            std::cout << pair.first << ": avg=" << avg << "us, "
                      << "count=" << times.size() << std::endl;
        }
    }
    
private:
    std::mutex mutex_;
    std::unordered_map<std::string, std::vector<int64_t>> records_;
};

// 使用宏
#define CN_PROFILE(name) CNProfiler::ScopeTimer _timer_##name(#name)
```

### 9.9 单元测试策略

#### 9.9.1 测试框架集成

```cpp
// 假设使用 Google Test
#include <gtest/gtest.h>

class CNMemoryLayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        layer_ = std::make_unique<CNMemoryLayer>("test", GeomType::kPoint);
    }
    
    void TearDown() override {
        layer_.reset();
    }
    
    std::unique_ptr<CNMemoryLayer> layer_;
};

TEST_F(CNMemoryLayerTest, CreateFeature) {
    CNFeature feature;
    feature.SetFID(1);
    
    CNStatus status = layer_->CreateFeature(&feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    EXPECT_EQ(layer_->GetFeatureCount(), 1);
}

TEST_F(CNMemoryLayerTest, GetFeature) {
    CNFeature feature;
    feature.SetFID(42);
    layer_->CreateFeature(&feature);
    
    CNFeature* retrieved = layer_->GetFeature(42);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetFID(), 42);
}

TEST_F(CNMemoryLayerTest, ThreadSafety) {
    const int thread_count = 4;
    const int features_per_thread = 1000;
    
    std::vector<std::thread> threads;
    for (int t = 0; t < thread_count; ++t) {
        threads.emplace_back([&, t]() {
            for (int i = 0; i < features_per_thread; ++i) {
                CNFeature feature;
                feature.SetFID(t * features_per_thread + i);
                layer_->CreateFeature(&feature);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(layer_->GetFeatureCount(), 
              thread_count * features_per_thread);
}
```

#### 9.9.2 模拟对象

```cpp
class MockCNDataSource : public CNDataSource {
public:
    MOCK_METHOD(CNStatus, Open, (const std::string&), (override));
    MOCK_METHOD(CNLayer*, GetLayer, (int), (override));
    MOCK_METHOD(int, GetLayerCount, (), (const, override));
    MOCK_METHOD(void, Close, (), (override));
};

TEST(CNLayerManagerTest, OpenDataSource) {
    MockCNDataSource mock_ds;
    EXPECT_CALL(mock_ds, Open(::testing::_))
        .WillOnce(::testing::Return(CNStatus::kSuccess));
    
    CNLayerManager manager;
    CNStatus status = manager.OpenDataSource(&mock_ds, "test.gpkg");
    EXPECT_EQ(status, CNStatus::kSuccess);
}
```

---

## 10. 设计建议与结论

### 10.1 核心设计建议

#### 10.1.1 架构设计

| 建议项 | 说明 |
|--------|------|
| 抽象基类 | CNLayer 定义统一接口，派生类实现具体功能 |
| 接口分离 | 区分只读接口和读写接口 |
| 能力测试 | 使用 TestCapability 机制查询图层能力 |
| 工厂模式 | 统一图层创建入口 |

#### 10.1.2 性能优化

| 优化项 | 策略 |
|--------|------|
| 缓存 | LRU 缓存热点数据 |
| 索引 | 根据数据特征选择合适索引 |
| 批量操作 | 减少锁竞争，提高吞吐量 |
| 延迟加载 | 按需加载数据 |

#### 10.1.3 线程安全

| 场景 | 策略 |
|------|------|
| 只读操作 | 无锁或读锁 |
| 写操作 | 写锁保护 |
| 迭代器 | 线程局部存储 |
| 事务 | MVCC 或快照隔离 |

### 10.2 实现优先级

| 优先级 | 模块 | 说明 |
|--------|------|------|
| P0 | CNLayer 抽象基类 | 定义核心接口 |
| P0 | CNMemoryLayer | 内存实现，用于测试和临时数据 |
| P1 | CNVectorLayer | 文件格式支持 |
| P1 | 空间索引 | R树、四叉树 |
| P2 | CNRasterLayer | 栅格支持 |
| P2 | 高级特性 | 事务、批量操作 |

### 10.3 研究结论

本研究通过对 OGC 标准、GDAL OGRLayer 架构、空间索引、多线程安全等关键技术的深入分析，为 CNLayer 及其派生类的设计提供了坚实的技术支撑。

**核心发现**:

1. **OGC 标准遵循**: 图层设计应遵循 OGC Simple Feature Access 标准，确保与其他 GIS 系统的互操作性。

2. **GDAL 架构借鉴**: GDAL 的 OGRLayer 设计提供了成熟的参考，包括能力测试机制、过滤器实现、事务支持等。

3. **空间索引选择**: R树适合动态数据和范围查询，四叉树适合点数据，网格索引适合静态均匀分布数据。

4. **线程安全策略**: 读写锁 + 细粒度锁 + 线程局部存储的组合策略能够有效平衡性能和安全性。

5. **现代 C++ 实践**: 智能指针、移动语义、RAII 等特性能够显著提升代码质量和安全性。

---

## 11. 参考链接

### 11.1 OGC 标准

- [OGC Simple Feature Access - Part 1: Common Architecture](https://www.ogc.org/standards/sfa)
- [OGC Simple Feature Access - Part 2: SQL Option](https://www.ogc.org/standards/sfs)
- [OGC Web Feature Service (WFS)](https://www.ogc.org/standards/wfs)
- [OGC GeoPackage](https://www.ogc.org/standards/geopackage)

### 11.2 GDAL 文档

- [GDAL Vector Data Model](https://www.osgeo.cn/gdal/user/vector_data_model.html)
- [GDAL OGRLayer API](https://gdal.org/api/ogrlayer_cpp.html)
- [GDAL Raster Data Model](https://gdal.org/user/raster_data_model.html)
- [GDAL OGR Architecture](https://gdal.org/user/ogr_arch.html)

### 11.3 空间索引

- [R-Tree与其他空间索引结构的对比](https://blog.csdn.net/qqrrjj2011/article/details/138186552)
- [空间索引简介](http://m.toutiao.com/group/7342386862985675301/)
- [GeoSpark空间索引](https://juejin.cn/post/7369114828076498980)

### 11.4 C++ 最佳实践

- [C++ 智能指针详解](https://blog.csdn.net/zone_programming/article/details/47000647)
- [C++ 线程安全设计模式](https://blog.csdn.net/qq_21438461/article/details/135430152)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

### 11.5 相关技术文章

- [GDAL的初步介绍和使用](https://blog.csdn.net/kappan/article/details/140170201)
- [GDAL矢量数据读取与写入](https://blog.csdn.net/d8dongdong/article/details/141299117)
- [GDAL栅格数据处理教程](https://blog.csdn.net/weixin_44785184/article/details/129132836)

### 11.6 数据库相关

- [PostGIS 官方文档](https://postgis.net/documentation/)
- [PostGIS 空间索引](https://postgis.net/workshops/postgis-intro/indexing.html)
- [SpatiaLite 官方文档](https://www.gaia-gis.it/fossil/libspatialite/index)
- [MySQL 空间索引技术](https://blog.csdn.net/weixin_72610956/article/details/140959909)

### 11.7 连接池与并发

- [数据库连接池设计](https://blog.csdn.net/qq_21438461/article/details/130858058)
- [C++ 线程池实现](https://blog.csdn.net/sevenjoin/article/details/82187127)

### 11.8 测试与调试

- [Google Test 官方文档](https://google.github.io/googletest/)
- [Google Mock 使用指南](https://google.github.io/googletest/gmock_for_dummies.html)
- [C++ 异常安全编程](https://blog.csdn.net/m0_73879806/article/details/141639942)

### 11.9 GIS 专业参考

- [PROJ 坐标转换库](https://proj.org/)
- [EPSG 坐标参考系统数据库](https://epsg.org/)
- [OGC DE-9IM 空间关系模型](https://en.wikipedia.org/wiki/DE-9IM)
- [GEOS 几何引擎](https://libgeos.org/)
- [JTS 拓扑套件](https://locationtech.github.io/jts/)
- [ISO 19115 元数据标准](https://www.iso.org/standard/53798.html)

### 11.10 几何验证与拓扑

- [OGC Simple Features 规范](https://www.ogc.org/standards/sfa)
- [GEOS MakeValid 实现](https://libgeos.org/doxygen/classgeos_1_1geom_1_1Geometry.html)
- [PostGIS 拓扑模块](https://postgis.net/docs/manual-dev/Topology.html)
- [ArcGIS 拓扑规则](https://pro.arcgis.com/zh-cn/pro-app/latest/help/data/geodatabases/overview/topology-rules.htm)

### 11.11 样式与渲染

- [OGC SLD 规范](https://www.ogc.org/standards/sld)
- [QGIS 样式文档](https://docs.qgis.org/latest/en/docs/user_manual/working_with_vector/vector_properties.html)
- [GeoServer SLD 参考](https://docs.geoserver.org/stable/en/user/styling/sld/index.html)

### 11.12 性能与扩展

- [内存映射文件详解](https://blog.csdn.net/whatday/article/details/109276881)
- [C++ 异步 I/O 编程](https://blog.csdn.net/qq_21438461/article/details/130858058)
- [GDAL 驱动开发指南](https://gdal.org/tutorials/index.html)

### 11.13 几何算法

- [Douglas-Peucker 算法详解](https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm)
- [Voronoi 图算法](https://en.wikipedia.org/wiki/Voronoi_diagram)
- [Delaunay 三角剖分](https://en.wikipedia.org/wiki/Delaunay_triangulation)
- [GEOS 几何操作](https://libgeos.org/doxygen/classgeos_1_1geom_1_1Geometry.html)

### 11.14 空间分析

- [GIS 空间分析概述](https://pro.arcgis.com/zh-cn/pro-app/latest/help/analysis/spatial-analyst/)
- [PostGIS 空间函数](https://postgis.net/docs/reference.html)
- [缓冲区分析算法](https://en.wikipedia.org/wiki/Buffer_(GIS))

### 11.15 数据格式规范

- [Shapefile 技术描述](https://www.esri.com/content/dam/esrisites/sitecore-archive/Files/Pdfs/library/whitepapers/pdfs/shapefile.pdf)
- [GeoJSON RFC 7946](https://tools.ietf.org/html/rfc7946)
- [GeoPackage 规范](https://www.geopackage.org/spec/)
- [Spatialite 文档](https://www.gaia-gis.it/gaia-sins/)

### 11.16 时态 GIS

- [OGC TimeSeries 规范](https://www.ogc.org/standards/waterml2)
- [时态 GIS 概念](https://en.wikipedia.org/wiki/Temporal_GIS)
- [PostGIS 时态支持](https://postgis.net/docs/using_postgis_dbmanagement.html#Temporal)

### 11.17 三维与点云

- [OGC CityGML 标准](https://www.ogc.org/standards/citygml)
- [LAS 点云格式规范](https://www.asprs.org/divisions-committees/lidar-division/las-lidar-data-exchange-format)
- [Cesium 3D Tiles](https://github.com/CesiumGS/3d-tiles)
- [PDAL 点云处理库](https://pdal.io/)

### 11.18 瓦片服务

- [OGC WMTS 标准](https://www.ogc.org/standards/wmts)
- [Mapbox Vector Tile 规范](https://github.com/mapbox/vector-tile-spec)
- [TMS 规范](https://wiki.osgeo.org/wiki/Tile_Map_Service_Specification)

### 11.19 大数据与分布式

- [Apache Sedona](https://sedona.apache.org/)
- [GeoSpark 文档](https://geospark.datasyslab.org/)
- [Spark 空间数据处理](https://spark.apache.org/docs/latest/sql-ref-functions.html)

### 11.20 序列化与安全

- [WKB 规范](https://en.wikipedia.org/wiki/Well-known_text_representation_of_geometry)
- [TWKB 规范](https://github.com/TWKB/Specification)
- [GDAL 安全最佳实践](https://gdal.org/user/security.html)

---

**报告完成日期**: 2026年3月17日  
**文档版本**: 1.8

---

## 修改记录

| 版本 | 日期 | 修改者 | 修改内容 |
|------|------|--------|----------|
| 1.0 | 2026-03-17 | Trae AI | 初始版本：OGC标准、GDAL架构、内存图层、矢量图层、栅格图层、空间索引、多线程安全、现代C++实践 |
| 1.1 | 2026-03-17 | Trae AI | 补充：连接池设计(8.5)、图层Join支持(8.6)、PostGIS管理(8.7)、批量操作优化(8.8) |
| 1.2 | 2026-03-17 | Trae AI | 补充：错误处理与异常安全(9.6)、跨平台兼容性(9.7)、日志与调试(9.8)、单元测试(9.9) |
| 1.3 | 2026-03-17 | GIS专家 | 补充：CRS处理(5.5)、空间关系谓词(5.6)、大数据量处理(5.7)、图层组(5.8)、几何验证(5.9)、拓扑操作(5.10)、元数据(5.11)、样式定义(5.12)、内存映射(5.13)、异步I/O(5.14)、插件架构(5.15) |
| 1.4 | 2026-03-17 | GIS专家 | 修复：版本一致性、文档完整性检查 |
| 1.5 | 2026-03-17 | GIS专家 | 补充：几何算法(5.16)、空间分析(5.17)、格式注意事项(5.18)、性能测试(5.19)、时态GIS(5.20) |
| 1.6 | 2026-03-17 | GIS专家 | 补充：三维数据(5.21)、瓦片服务(5.22)、大数据集成(5.23)、序列化压缩(5.24)、权限安全(5.25) |
| 1.7 | 2026-03-17 | GIS专家 | 补充：国际化本地化(5.26)、数据质量评估(5.27)、网络服务客户端(5.28)、空间统计分析(5.29)、数据备份恢复(5.30)、GIS库对比分析(5.31)、实际应用案例(5.32) |
| 1.8 | 2026-03-17 | Markdown专家 | 修复：目录结构完整性、补充缺失子章节链接、版本一致性修复 |