# 模块列表

> **更新日期**: 2026-04-10  
> **项目**: OGCGeometry v2.3.0

---

## 模块概览

| 模块 | 路径 | 说明 |
|------|------|------|
| base | code/base | 基础模块：日志、性能监控、线程安全 |
| geom | code/geom | 几何模块：OGC标准几何类型、空间操作、空间索引 |
| chart/parser | code/chart/parser | 海图解析：S57/S100/S102格式解析、GDAL集成 |
| feature | code/feature | 要素模块：地理要素、属性管理 |
| layer | code/layer | 图层模块：图层管理、渲染控制 |
| database | code/database | 数据库模块：PostgreSQL、SQLite支持 |
| draw | code/draw | 绘制模块：多引擎渲染、设备抽象 |
| proj | code/proj | 投影模块：坐标转换、投影支持 |
| cache | code/cache | 缓存模块：瓦片缓存、磁盘缓存 |
| symbology | code/symbology | 符号化模块：S52样式、符号渲染 |
| alert | code/alert | 预警模块：航行预警、规则引擎 |
| alarm | code/alarm | 告警模块：告警系统、REST接口 |
| graph | code/graph | 图形模块：集成绘制、并发渲染 |
| navi | code/navi | 导航模块：航线规划、导航服务 |
| cycle/chart_c_api | code/cycle/chart_c_api | C API：跨语言接口 |

---

## 模块依赖关系

```
base (无依赖)
  ↓
geom → feature → layer → draw → graph
  ↓       ↓        ↓
proj   database  symbology
  ↓                ↓
cache            alert → alarm
                   ↓
                 navi
```

---

## 外部依赖

### 依赖库

| 依赖库 | 使用模块 | 说明 |
|--------|----------|------|
| GDAL | chart/parser | 地理数据抽象库 |
| libpq | database | PostgreSQL客户端库 |
| sqlite3 | database, graph | 嵌入式数据库 |
| libspatialite | database | 空间数据库扩展 |
| libcurl | graph | HTTP客户端库 |
| libxml2 | chart/parser | XML解析库（可选） |
| GoogleTest | 所有测试 | 单元测试框架 |

### 依赖路径配置

```cmake
set(POSTGRESQL_ROOT "D:/program/PostgreSQL/13" CACHE PATH "PostgreSQL root directory")
set(SQLITE3_ROOT "F:/win/3rd/sqlite3" CACHE PATH "SQLite3 root directory")
set(GEOS_ROOT "F:/win/3rd/GEOS3.10" CACHE PATH "GEOS root directory")
set(PROJ_ROOT "F:/win/3rd/PROJ" CACHE PATH "PROJ root directory")
set(CURL_ROOT "F:/win/3rd/libcurl_x64" CACHE PATH "cURL library root directory")
set(GTEST_ROOT "F:/win/3rd/googletest" CACHE PATH "GoogleTest root directory")
set(GDAL_ROOT "F:/win/3rd/gdal-3.9.3" CACHE PATH "GDAL root directory")
set(LIBXML2_ROOT "F:/win/3rd/libxml2" CACHE PATH "libxml2 root directory")
```

---

## 编译状态

| 状态 | 模块数 | 模块列表 |
|------|--------|----------|
| ✅ 成功 | 15 | base, geom, chart/parser, feature, layer, database, draw, proj, cache, symbology, alert, alarm, graph, navi, cycle/chart_c_api |
| ❌ 失败 | 0 | - |
| ⏭️ 跳过 | 0 | - |

### 测试统计

| 项目 | 测试文件数 | 用例数 |
|------|------------|--------|
| 总计 | 140 | 4523 |
| ✅ 通过 | 139 | 4522 |
| ❌ 失败 | 0 | 0 |
| ⏭️ 跳过 | 1 | 1 |

**文件通过率**: 99.3% | **用例通过率**: 99.98%

**详细报告**: [0_test_report.md](./0_test_report.md)
