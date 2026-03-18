# OGC 几何对象数据库存储模型设计文档

**版本**: 1.3  
**日期**: 2026年3月18日  
**状态**: 设计中  
**C++标准**: C++11  
**设计目标**: 稳定、正确、高效、易扩展、跨平台、多线程安全

---

## 文档修订历史

| 版本 | 日期 | 修改内容 | 评审意见 |
|------|------|----------|----------|
| 1.0 | 2026-03-16 | 初始版本创建 | - |
| 1.1 | 2026-03-17 | 根据技术评审意见修订：新增MVCC和事务隔离级别设计、空间索引管理策略、完善WKB转换器、连接池健康检查、批量插入错误恢复、空间查询分页、坐标参考系转换集成 | 核心架构师评审 |
| 1.2 | 2026-03-18 | 根据核心架构师评审意见修订：拆分DbConnection接口为角色接口、完善批量插入错误恢复机制、增加连接池耗尽处理策略、WKB转换器SIMD优化路径、查询计划分析接口、数据库连接重连机制、SRID严格验证选项、工作量评估和里程碑规划、测试环境配置说明、API稳定性承诺 | 核心架构师评审 |
| 1.3 | 2026-03-18 | 根据多角色交叉评审意见修订：新增GeoJSON序列化设计、空间索引策略对比、SRID验证与坐标转换流水线、空间函数兼容性矩阵、连接池预热与性能指标、事务超时处理、SIMD运行时检测与回退、SQL注入防护增强、审计日志接口、ABI兼容性与版本控制、C API绑定、错误处理统一策略、连接字符串安全处理、异步API设计 | 多角色交叉评审 |

---

## 目录

1. [概述](#1-概述)
2. [存储架构设计](#2-存储架构设计)
3. [PostGIS 存储适配器](#3-postgis-存储适配器)
4. [SpatiaLite 存储适配器](#4-spatialite-存储适配器)
5. [数据库抽象层](#5-数据库抽象层)
6. [连接池和资源管理](#6-连接池和资源管理)
7. [性能优化策略](#7-性能优化策略)
8. [线程安全设计](#8-线程安全设计)
9. [跨平台支持](#9-跨平台支持)
10. [使用示例](#10-使用示例)
11. [编译和依赖](#11-编译和依赖)
12. [错误处理](#12-错误处理)
13. [测试策略](#13-测试策略)
14. [MVCC和事务隔离级别设计](#14-mvcc和事务隔离级别设计)
15. [空间索引管理策略](#15-空间索引管理策略)
16. [WKB转换器完善设计](#16-wkb转换器完善设计)
17. [坐标参考系转换集成](#17-坐标参考系转换集成)
18. [性能监控和诊断](#18-性能监控和诊断)
19. [角色接口设计](#19-角色接口设计) *(v1.2新增)*
20. [批量插入错误恢复机制](#20-批量插入错误恢复机制) *(v1.2新增)*
21. [连接池高级特性](#21-连接池高级特性) *(v1.2新增)*
22. [WKB转换器SIMD优化](#22-wkb转换器simd优化) *(v1.2新增)*
23. [查询计划分析接口](#23-查询计划分析接口) *(v1.2新增)*
24. [数据库连接重连机制](#24-数据库连接重连机制) *(v1.2新增)*
25. [SRID严格验证](#25-srid严格验证) *(v1.2新增)*
26. [工作量评估和里程碑规划](#26-工作量评估和里程碑规划) *(v1.2新增)*
27. [测试环境配置](#27-测试环境配置) *(v1.2新增)*
28. [API稳定性承诺](#28-api稳定性承诺) *(v1.2新增)*
29. [总结](#29-总结)

---

## 1. 概述

### 1.1 设计目标

本数据库存储模型旨在为 OGC 几何对象提供高效、可靠的持久化存储方案,支持 **PostGIS** 和 **SpatiaLite** 两种主流空间数据库。

**核心设计目标**:

| 目标 | 说明 | 实现策略 |
|------|------|----------|
| **稳定性** | 健壮的错误处理、资源管理 | RAII、异常安全、连接池 |
| **正确性** | 数据完整性、事务支持 | ACID、WKB/EWKB 格式标准化 |
| **高效性** | 批量操作、空间索引优化 | COPY、R-Tree、预编译语句 |
| **扩展性** | 易于添加新数据库类型 | 适配器模式、工厂模式 |
| **跨平台** | 支持 Windows/Linux/macOS | C++11、无平台依赖 |
| **线程安全** | 多线程并发访问 | 连接池、独立连接、原子操作 |

### 1.2 核心特性

- ✅ **双数据库支持**: PostGIS (PostgreSQL) 和 SpatiaLite (SQLite)
- ✅ **标准化格式**: WKB/EWKB/GeoJSON 序列化
- ✅ **空间索引**: GiST (PostGIS) 和 R-Tree (SpatiaLite)
- ✅ **批量操作**: COPY (PostGIS) 和事务批量插入 (SpatiaLite)
- ✅ **连接池**: 复用数据库连接,提升性能
- ✅ **线程安全**: 每线程独立连接或连接池模式
- ✅ **错误处理**: Result 模式 + 异常可选
- ✅ **性能监控**: 查询性能统计、慢查询日志

### 1.3 架构层次

```
┌─────────────────────────────────────────────────────────┐
│                    应用层 (Application)                   │
│          使用 Geometry 对象进行业务逻辑处理                │
└───────────────────────┬─────────────────────────────────┘
                        │
┌───────────────────────▼─────────────────────────────────┐
│              数据库抽象层 (Database Abstraction)          │
│   DbConnection, DbStatement, DbResultSet, DbTransaction  │
└───────────────────────┬─────────────────────────────────┘
                        │
        ┌───────────────┴───────────────┐
        │                               │
┌───────▼────────┐            ┌────────▼────────┐
│ PostGIS 适配器  │            │ SpatiaLite 适配器│
│  PostGISAdapter│            │SpatiaLiteAdapter│
│  EWKB 格式     │            │ BLOB 格式        │
│  libpq + GEOS  │            │ libspatialite   │
└────────────────┘            └─────────────────┘
        │                               │
┌───────▼────────┐            ┌────────▼────────┐
│  PostgreSQL    │            │    SQLite       │
│  PostGIS 扩展   │            │  SpatiaLite 扩展│
└────────────────┘            └─────────────────┘
```

---

## 2. 存储架构设计

### 2.1 核心接口设计

#### 2.1.1 数据库连接接口

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 数据库连接抽象接口
 * 
 * 设计原则：
 * 1. 线程安全：所有方法都是线程安全的
 * 2. RAII：资源自动管理
 * 3. 异常安全：强异常保证
 */
class DbConnection {
public:
    virtual ~DbConnection() = default;
    
    // ========== 连接管理 ==========
    
    /**
     * @brief 打开数据库连接
     * @param connection_string 连接字符串
     * @return 操作结果
     */
    virtual DbResult Connect(const std::string& connection_string) = 0;
    
    /**
     * @brief 关闭数据库连接
     */
    virtual void Disconnect() = 0;
    
    /**
     * @brief 检查连接是否有效
     */
    virtual bool IsConnected() const = 0;
    
    /**
     * @brief Ping 数据库服务器
     */
    virtual bool Ping() const = 0;
    
    // ========== 事务管理 ==========
    
    /**
     * @brief 开始事务
     */
    virtual DbResult BeginTransaction() = 0;
    
    /**
     * @brief 提交事务
     */
    virtual DbResult Commit() = 0;
    
    /**
     * @brief 回滚事务
     */
    virtual DbResult Rollback() = 0;
    
    /**
     * @brief 是否在事务中
     */
    virtual bool InTransaction() const = 0;
    
    // ========== SQL 执行 ==========
    
    /**
     * @brief 执行 SQL 语句（无返回结果）
     */
    virtual DbResult Execute(const std::string& sql) = 0;
    
    /**
     * @brief 执行查询语句
     * @param sql SQL 查询
     * @param result 输出结果集
     */
    virtual DbResult ExecuteQuery(const std::string& sql,
                                   std::unique_ptr<DbResultSet>& result) = 0;
    
    /**
     * @brief 创建预编译语句
     */
    virtual DbResult PrepareStatement(const std::string& name,
                                       const std::string& sql,
                                       std::unique_ptr<DbStatement>& stmt) = 0;
    
    // ========== 几何对象操作 ==========
    
    /**
     * @brief 插入几何对象
     */
    virtual DbResult InsertGeometry(const std::string& table,
                                     const std::string& geom_column,
                                     const CNGeometry* geometry,
                                     const std::map<std::string, std::string>& attributes,
                                     int64_t& out_id) = 0;
    
    /**
     * @brief 批量插入几何对象
     */
    virtual DbResult InsertGeometries(const std::string& table,
                                       const std::string& geom_column,
                                       const std::vector<CNGeometryPtr>& geometries,
                                       const std::vector<std::map<std::string, std::string>>& attributes,
                                       std::vector<int64_t>& out_ids) = 0;
    
    /**
     * @brief 查询几何对象
     */
    virtual DbResult SelectGeometries(const std::string& table,
                                       const std::string& geom_column,
                                       const std::string& where_clause,
                                       std::vector<CNGeometryPtr>& geometries) = 0;
    
    /**
     * @brief 空间查询
     */
    virtual DbResult SpatialQuery(const std::string& table,
                                   const std::string& geom_column,
                                   SpatialOperator op,
                                   const CNGeometry* query_geom,
                                   std::vector<CNGeometryPtr>& results) = 0;
    
    /**
     * @brief 更新几何对象
     */
    virtual DbResult UpdateGeometry(const std::string& table,
                                     const std::string& geom_column,
                                     int64_t id,
                                     const CNGeometry* geometry) = 0;
    
    /**
     * @brief 删除几何对象
     */
    virtual DbResult DeleteGeometry(const std::string& table,
                                     int64_t id) = 0;
    
    // ========== 元数据操作 ==========
    
    /**
     * @brief 创建空间表
     */
    virtual DbResult CreateSpatialTable(const std::string& table_name,
                                         const std::string& geom_column,
                                         GeomType geom_type,
                                         int srid,
                                         int coord_dimension) = 0;
    
    /**
     * @brief 创建空间索引
     */
    virtual DbResult CreateSpatialIndex(const std::string& table_name,
                                         const std::string& geom_column) = 0;
    
    /**
     * @brief 检查空间表是否存在
     */
    virtual bool SpatialTableExists(const std::string& table_name) const = 0;
    
    // ========== 数据库类型 ==========
    
    /**
     * @brief 获取数据库类型
     */
    virtual DatabaseType GetType() const = 0;
    
    /**
     * @brief 获取数据库版本
     */
    virtual std::string GetVersion() const = 0;
    
    /**
     * @brief 获取最后插入的 ID
     */
    virtual int64_t GetLastInsertId() const = 0;
    
    /**
     * @brief 获取影响的行数
     */
    virtual int64_t GetRowsAffected() const = 0;
};

} // namespace DB
} // namespace OGC
```

#### 2.1.2 数据库类型枚举

```cpp
/**
 * @brief 数据库类型枚举
 */
enum class DatabaseType {
    kUnknown = 0,
    kPostGIS = 1,      // PostgreSQL + PostGIS
    kSpatiaLite = 2,   // SQLite + SpatiaLite
    kOracleSpatial = 3,
    kSQLServerSpatial = 4
};

/**
 * @brief 空间操作符枚举
 */
enum class SpatialOperator {
    kIntersects = 0,   // 相交
    kContains = 1,     // 包含
    kWithin = 2,       // 被包含
    kTouches = 3,      // 接触
    kCrosses = 4,      // 交叉
    kOverlaps = 5,     // 重叠
    kDisjoint = 6,     // 相离
    kEquals = 7,       // 相等
    kDWithin = 8       // 距离内
};

/**
 * @brief 数据库操作结果枚举
 */
enum class DbResult {
    // 成功
    kSuccess = 0,
    
    // 连接错误 (1-99)
    kConnectionFailed = 1,
    kConnectionLost = 2,
    kConnectionTimeout = 3,
    
    // SQL 错误 (100-199)
    kSqlError = 100,
    kSyntaxError = 101,
    kTableNotFound = 102,
    kColumnNotFound = 103,
    kConstraintViolation = 104,
    
    // 几何错误 (200-299)
    kInvalidGeometry = 200,
    kGeometryParseError = 201,
    kSridMismatch = 202,
    
    // 事务错误 (300-399)
    kTransactionError = 300,
    kDeadlock = 301,
    kSerializationFailure = 302,
    
    // 资源错误 (400-499)
    kOutOfMemory = 400,
    kResourceExhausted = 401,
    
    // 其他
    kNotSupported = 999,
    kNotImplemented = 1000
};
```

---

## 3. PostGIS 存储适配器

### 3.1 WKB/EWKB 格式转换

#### 3.1.1 EWKB 格式规范

**Extended Well-Known Binary (EWKB)** 是 PostGIS 对标准 WKB 的扩展,增加了 SRID 和 Z/M 维度支持。

**EWKB 结构**:

| 字节偏移 | 长度 | 内容 | 说明 |
|---------|------|------|------|
| 0 | 1 | 字节序标记 | `0x00` (大端) 或 `0x01` (小端) |
| 1 | 4 | 几何类型 | 类型码 + 标志位 |
| 5 | 4 (可选) | SRID | 空间参考系 ID (当标志位设置时) |
| ... | ... | 坐标数据 | 根据 geometry type 变化 |

**几何类型标志位**:

```cpp
// 几何类型基础码 (OGC 标准)
constexpr uint32_t kWkbPoint = 1;
constexpr uint32_t kWkbLineString = 2;
constexpr uint32_t kWkbPolygon = 3;
constexpr uint32_t kWkbMultiPoint = 4;
constexpr uint32_t kWkbMultiLineString = 5;
constexpr uint32_t kWkbMultiPolygon = 6;
constexpr uint32_t kWkbGeometryCollection = 7;

// PostGIS 扩展标志位
constexpr uint32_t kWkbHasZ = 0x80000000;   // 第31位: 有 Z 坐标
constexpr uint32_t kWkbHasM = 0x40000000;   // 第30位: 有 M 坐标
constexpr uint32_t kWkbHasSRID = 0x20000000; // 第29位: 有 SRID
```

#### 3.1.2 WKB/EWKB 转换器

```cpp
namespace OGC {
namespace DB {

/**
 * @brief WKB/EWKB 格式转换器
 * 
 * 线程安全：所有方法都是线程安全的
 */
class WkbConverter {
public:
    // ========== 从几何对象转换为 WKB ==========
    
    /**
     * @brief 将几何对象转换为 WKB
     * @param geometry 几何对象
     * @param wkb 输出 WKB 字节数组
     * @param include_srid 是否包含 SRID (EWKB)
     * @return 操作结果
     */
    static DbResult GeometryToWkb(const CNGeometry* geometry,
                                   std::vector<uint8_t>& wkb,
                                   bool include_srid = true);
    
    /**
     * @brief 将几何对象转换为十六进制 WKB
     */
    static DbResult GeometryToHexWkb(const CNGeometry* geometry,
                                      std::string& hex_wkb,
                                      bool include_srid = true);
    
    // ========== 从 WKB 转换为几何对象 ==========
    
    /**
     * @brief 从 WKB 创建几何对象
     * @param wkb WKB 字节数组
     * @param geometry 输出几何对象
     * @return 操作结果
     */
    static DbResult WkbToGeometry(const std::vector<uint8_t>& wkb,
                                   CNGeometryPtr& geometry);
    
    /**
     * @brief 从十六进制 WKB 创建几何对象
     */
    static DbResult HexWkbToGeometry(const std::string& hex_wkb,
                                      CNGeometryPtr& geometry);
    
    // ========== EWKB 特有功能 ==========
    
    /**
     * @brief 检查 WKB 是否为 EWKB (包含 SRID)
     */
    static bool IsEwkb(const std::vector<uint8_t>& wkb);
    
    /**
     * @brief 提取 EWKB 中的 SRID
     */
    static bool ExtractSrid(const std::vector<uint8_t>& ewkb, int& srid);
    
private:
    // 字节序转换
    static uint16_t SwapEndian16(uint16_t value);
    static uint32_t SwapEndian32(uint32_t value);
    static uint64_t SwapEndian64(uint64_t value);
    
    // 写入坐标
    static void WriteCoordinate(std::vector<uint8_t>& buffer,
                                 const Coordinate& coord,
                                 bool has_z, bool has_m,
                                 bool little_endian);
    
    // 读取坐标
    static Coordinate ReadCoordinate(const uint8_t* data,
                                      bool has_z, bool has_m,
                                      bool little_endian);
};

} // namespace DB
} // namespace OGC
```

**实现示例**:

```cpp
DbResult WkbConverter::GeometryToWkb(const CNGeometry* geometry,
                                      std::vector<uint8_t>& wkb,
                                      bool include_srid) {
    if (!geometry || geometry->IsEmpty()) {
        return DbResult::kInvalidGeometry;
    }
    
    // 清空输出缓冲区
    wkb.clear();
    
    // 小端序标记
    const bool kLittleEndian = true;
    wkb.push_back(kLittleEndian ? 0x01 : 0x00);
    
    // 构造几何类型 (包含标志位)
    uint32_t type_code = static_cast<uint32_t>(geometry->GetGeometryType());
    
    // 添加 Z/M 标志
    if (geometry->Is3D()) {
        type_code |= kWkbHasZ;
    }
    if (geometry->IsMeasured()) {
        type_code |= kWkbHasM;
    }
    
    // 添加 SRID 标志
    int srid = geometry->GetSRID();
    if (include_srid && srid > 0) {
        type_code |= kWkbHasSRID;
    }
    
    // 写入几何类型 (4 字节)
    const uint8_t* type_bytes = reinterpret_cast<const uint8_t*>(&type_code);
    if (kLittleEndian) {
        wkb.insert(wkb.end(), type_bytes, type_bytes + 4);
    } else {
        // 大端序需要翻转
        for (int i = 3; i >= 0; --i) {
            wkb.push_back(type_bytes[i]);
        }
    }
    
    // 写入 SRID (如果需要)
    if (include_srid && srid > 0) {
        const uint8_t* srid_bytes = reinterpret_cast<const uint8_t*>(&srid);
        if (kLittleEndian) {
            wkb.insert(wkb.end(), srid_bytes, srid_bytes + 4);
        } else {
            for (int i = 3; i >= 0; --i) {
                wkb.push_back(srid_bytes[i]);
            }
        }
    }
    
    // 根据几何类型写入坐标数据
    switch (geometry->GetGeometryType()) {
        case GeomType::kPoint: {
            const CNPoint* point = dynamic_cast<const CNPoint*>(geometry);
            WriteCoordinate(wkb, point->GetCoordinate(), 
                           geometry->Is3D(), geometry->IsMeasured(),
                           kLittleEndian);
            break;
        }
        case GeomType::kLineString: {
            const CNLineString* line = dynamic_cast<const CNLineString*>(geometry);
            uint32_t num_points = static_cast<uint32_t>(line->GetNumPoints());
            // 写入点数
            const uint8_t* np_bytes = reinterpret_cast<const uint8_t*>(&num_points);
            wkb.insert(wkb.end(), np_bytes, np_bytes + 4);
            // 写入坐标
            for (size_t i = 0; i < num_points; ++i) {
                WriteCoordinate(wkb, line->GetPointN(i),
                               geometry->Is3D(), geometry->IsMeasured(),
                               kLittleEndian);
            }
            break;
        }
        // ... 其他几何类型类似
        default:
            return DbResult::kNotImplemented;
    }
    
    return DbResult::kSuccess;
}
```

### 3.2 PostGIS 连接实现

```cpp
namespace OGC {
namespace DB {

/**
 * @brief PostGIS 数据库连接实现
 * 
 * 基于 libpq (PostgreSQL C 客户端库) 和 GEOS
 * 
 * 线程安全：每个连接对象应该在单个线程中使用，或使用连接池
 */
class PostGISConnection : public DbConnection {
public:
    // ========== 构造和析构 ==========
    
    PostGISConnection();
    virtual ~PostGISConnection();
    
    // 禁止拷贝
    PostGISConnection(const PostGISConnection&) = delete;
    PostGISConnection& operator=(const PostGISConnection&) = delete;
    
    // 支持移动
    PostGISConnection(PostGISConnection&&) noexcept;
    PostGISConnection& operator=(PostGISConnection&&) noexcept;
    
    // ========== DbConnection 接口实现 ==========
    
    DbResult Connect(const std::string& connection_string) override;
    void Disconnect() override;
    bool IsConnected() const override;
    bool Ping() const override;
    
    DbResult BeginTransaction() override;
    DbResult Commit() override;
    DbResult Rollback() override;
    bool InTransaction() const override;
    
    DbResult Execute(const std::string& sql) override;
    DbResult ExecuteQuery(const std::string& sql,
                           std::unique_ptr<DbResultSet>& result) override;
    DbResult PrepareStatement(const std::string& name,
                               const std::string& sql,
                               std::unique_ptr<DbStatement>& stmt) override;
    
    // ========== 几何对象操作 ==========
    
    DbResult InsertGeometry(const std::string& table,
                             const std::string& geom_column,
                             const CNGeometry* geometry,
                             const std::map<std::string, std::string>& attributes,
                             int64_t& out_id) override {
        // 1. 将几何对象转换为 EWKB
        std::vector<uint8_t> ewkb;
        DbResult result = WkbConverter::GeometryToWkb(geometry, ewkb, true);
        if (result != DbResult::kSuccess) {
            return result;
        }
        
        // 2. 构造 INSERT SQL
        std::ostringstream sql;
        sql << "INSERT INTO " << table << " (" << geom_column;
        
        for (const auto& attr : attributes) {
            sql << ", " << attr.first;
        }
        sql << ") VALUES ($1";  // $1 = 几何参数
        
        int param_index = 2;
        for (size_t i = 0; i < attributes.size(); ++i) {
            sql << ", $" << param_index++;
        }
        sql << ") RETURNING id";
        
        // 3. 准备参数
        std::vector<const char*> param_values;
        std::vector<int> param_lengths;
        std::vector<int> param_formats;
        
        // 几何参数 (二进制格式)
        param_values.push_back(reinterpret_cast<const char*>(ewkb.data()));
        param_lengths.push_back(static_cast<int>(ewkb.size()));
        param_formats.push_back(1);  // 1 = binary
        
        // 属性参数 (文本格式)
        std::vector<std::string> attr_values;
        for (const auto& attr : attributes) {
            attr_values.push_back(attr.second);
            param_values.push_back(attr_values.back().c_str());
            param_lengths.push_back(static_cast<int>(attr_values.back().size()));
            param_formats.push_back(0);  // 0 = text
        }
        
        // 4. 执行 SQL
        PGresult* pg_result = PQexecParams(
            pg_connection_,
            sql.str().c_str(),
            static_cast<int>(param_values.size()),
            nullptr,  // param types (让 PostgreSQL 自动推断)
            param_values.data(),
            param_lengths.data(),
            param_formats.data(),
            0  // 结果格式 0 = text
        );
        
        // 5. 检查结果
        if (PQresultStatus(pg_result) != PGRES_TUPLES_OK) {
            std::string error = PQerrorMessage(pg_connection_);
            PQclear(pg_result);
            last_error_ = error;
            return DbResult::kSqlError;
        }
        
        // 6. 获取插入的 ID
        out_id = std::stoll(PQgetvalue(pg_result, 0, 0));
        PQclear(pg_result);
        
        return DbResult::kSuccess;
    }
    
    DbResult SpatialQuery(const std::string& table,
                           const std::string& geom_column,
                           SpatialOperator op,
                           const CNGeometry* query_geom,
                           std::vector<CNGeometryPtr>& results) override {
        if (!query_geom || !IsConnected()) {
            return DbResult::kInvalidGeometry;
        }
        
        // 1. 转换查询几何为 EWKB
        std::vector<uint8_t> ewkb;
        DbResult result = WkbConverter::GeometryToWkb(query_geom, ewkb, true);
        if (result != DbResult::kSuccess) {
            return result;
        }
        
        // 2. 构造空间查询 SQL
        std::string spatial_function;
        switch (op) {
            case SpatialOperator::kIntersects:
                spatial_function = "ST_Intersects";
                break;
            case SpatialOperator::kContains:
                spatial_function = "ST_Contains";
                break;
            case SpatialOperator::kWithin:
                spatial_function = "ST_Within";
                break;
            // ... 其他操作符
            default:
                return DbResult::kNotSupported;
        }
        
        std::ostringstream sql;
        sql << "SELECT " << geom_column << " FROM " << table
            << " WHERE " << geom_column << " && ST_GeomFromEWKB($1)"  // &&: 边界框相交
            << " AND " << spatial_function << "(" << geom_column 
            << ", ST_GeomFromEWKB($1))";
        
        // 3. 执行查询
        const char* param_values[1] = {reinterpret_cast<const char*>(ewkb.data())};
        int param_lengths[1] = {static_cast<int>(ewkb.size())};
        int param_formats[1] = {1};  // binary
        
        PGresult* pg_result = PQexecParams(
            pg_connection_,
            sql.str().c_str(),
            1,
            nullptr,
            param_values,
            param_lengths,
            param_formats,
            1  // 结果格式 1 = binary
        );
        
        if (PQresultStatus(pg_result) != PGRES_TUPLES_OK) {
            last_error_ = PQerrorMessage(pg_connection_);
            PQclear(pg_result);
            return DbResult::kSqlError;
        }
        
        // 4. 解析结果集
        int num_rows = PQntuples(pg_result);
        results.clear();
        results.reserve(num_rows);
        
        for (int i = 0; i < num_rows; ++i) {
            // 获取 EWKB 二进制数据
            const uint8_t* data = reinterpret_cast<const uint8_t*>(
                PQgetvalue(pg_result, i, 0));
            int data_len = PQgetlength(pg_result, i, 0);
            
            // 转换为几何对象
            std::vector<uint8_t> geom_ewkb(data, data + data_len);
            CNGeometryPtr geom;
            result = WkbConverter::WkbToGeometry(geom_ewkb, geom);
            
            if (result == DbResult::kSuccess) {
                results.push_back(std::move(geom));
            }
        }
        
        PQclear(pg_result);
        return DbResult::kSuccess;
    }
    
    // ========== 其他方法实现类似 ==========
    
    DatabaseType GetType() const override { return DatabaseType::kPostGIS; }
    
private:
    PGconn* pg_connection_ = nullptr;
    std::string last_error_;
    bool in_transaction_ = false;
    
    // GEOS 上下文 (用于几何运算)
    GEOSContextHandle_t geos_context_ = nullptr;
};

} // namespace DB
} // namespace OGC
```

---

## 4. SpatiaLite 存储适配器

### 4.1 SpatiaLite BLOB 格式

SpatiaLite 使用自定义 BLOB 格式存储几何数据,在标准 WKB 基础上增加了 SRID 和 MBR 元数据。

**BLOB 结构**:

| 字节偏移 | 长度 | 内容 | 说明 |
|---------|------|------|------|
| 0 | 1 | 起始标记 | `0x00` |
| 1 | 1 | 字节序 | `0x01` (小端) 或 `0x00` (大端) |
| 2-5 | 4 | SRID | 空间参考系 ID |
| 6-37 | 32 | MBR (Envelope) | 4 个双精度浮点数 (minX, maxX, minY, maxY) |
| 38 | 1 | MBR 分隔符 | `0x7C` |
| 39-42 | 4 | 几何类型 | 类型码 |
| 43+ | 可变 | 坐标数据 | 标准 WKB 格式 |
| 末尾 | 1 | 结束标记 | `0xFE` |

### 4.2 SpatiaLite 连接实现

```cpp
namespace OGC {
namespace DB {

/**
 * @brief SpatiaLite 数据库连接实现
 * 
 * 基于 SQLite3 和 libspatialite
 * 
 * 线程安全：每个连接对象独立,适合连接池模式
 */
class SpatiaLiteConnection : public DbConnection {
public:
    // ========== 构造和析构 ==========
    
    SpatiaLiteConnection();
    virtual ~SpatiaLiteConnection();
    
    // ========== DbConnection 接口实现 ==========
    
    DbResult Connect(const std::string& connection_string) override {
        // connection_string 格式: "file:path/to/database.db"
        
        // 1. 打开 SQLite 数据库
        int ret = sqlite3_open_v2(
            connection_string.c_str(),
            &sqlite_connection_,
            SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
            nullptr
        );
        
        if (ret != SQLITE_OK) {
            last_error_ = sqlite3_errmsg(sqlite_connection_);
            sqlite3_close(sqlite_connection_);
            sqlite_connection_ = nullptr;
            return DbResult::kConnectionFailed;
        }
        
        // 2. 初始化 SpatiaLite 扩展
        spatialite_cache_ = spatialite_alloc_connection();
        spatialite_init_ex(sqlite_connection_, spatialite_cache_, 0);
        
        // 3. 初始化空间元数据表
        char* error_msg = nullptr;
        ret = sqlite3_exec(
            sqlite_connection_,
            "SELECT InitSpatialMetaData(1)",
            nullptr,
            nullptr,
            &error_msg
        );
        
        if (ret != SQLITE_OK) {
            last_error_ = error_msg;
            sqlite3_free(error_msg);
            // 不是致命错误,数据库可能已初始化
        }
        
        return DbResult::kSuccess;
    }
    
    void Disconnect() override {
        if (spatialite_cache_) {
            spatialite_cleanup_ex(spatialite_cache_);
            spatialite_free_connection(spatialite_cache_);
            spatialite_cache_ = nullptr;
        }
        
        if (sqlite_connection_) {
            sqlite3_close(sqlite_connection_);
            sqlite_connection_ = nullptr;
        }
    }
    
    DbResult InsertGeometry(const std::string& table,
                             const std::string& geom_column,
                             const CNGeometry* geometry,
                             const std::map<std::string, std::string>& attributes,
                             int64_t& out_id) override {
        if (!geometry || !sqlite_connection_) {
            return DbResult::kInvalidGeometry;
        }
        
        // 1. 转换几何对象为 SpatiaLite BLOB
        gaiaGeomCollPtr gaia_geom = GeometryToGaia(geometry);
        if (!gaia_geom) {
            return DbResult::kGeometryParseError;
        }
        
        // 2. 转换为 BLOB
        unsigned char* blob = nullptr;
        size_t blob_size = 0;
        gaiaToSpatiaLiteBlob(gaia_geom, &blob, &blob_size);
        
        // 3. 构造 INSERT SQL
        std::ostringstream sql;
        sql << "INSERT INTO " << table << " (" << geom_column;
        
        for (const auto& attr : attributes) {
            sql << ", " << attr.first;
        }
        sql << ") VALUES (?";
        
        for (size_t i = 0; i < attributes.size(); ++i) {
            sql << ", ?";
        }
        sql << ")";
        
        // 4. 准备语句
        sqlite3_stmt* stmt = nullptr;
        int ret = sqlite3_prepare_v2(
            sqlite_connection_,
            sql.str().c_str(),
            -1,
            &stmt,
            nullptr
        );
        
        if (ret != SQLITE_OK) {
            last_error_ = sqlite3_errmsg(sqlite_connection_);
            gaiaFreeGeomColl(gaia_geom);
            return DbResult::kSqlError;
        }
        
        // 5. 绑定参数
        // 第一个参数: 几何 BLOB
        ret = sqlite3_bind_blob(stmt, 1, blob, static_cast<int>(blob_size), free);
        if (ret != SQLITE_OK) {
            last_error_ = sqlite3_errmsg(sqlite_connection_);
            sqlite3_finalize(stmt);
            gaiaFreeGeomColl(gaia_geom);
            return DbResult::kSqlError;
        }
        
        // 其他参数: 属性值
        int param_index = 2;
        for (const auto& attr : attributes) {
            sqlite3_bind_text(stmt, param_index++, 
                             attr.second.c_str(), -1, SQLITE_TRANSIENT);
        }
        
        // 6. 执行插入
        ret = sqlite3_step(stmt);
        if (ret != SQLITE_DONE) {
            last_error_ = sqlite3_errmsg(sqlite_connection_);
            sqlite3_finalize(stmt);
            gaiaFreeGeomColl(gaia_geom);
            return DbResult::kSqlError;
        }
        
        // 7. 获取插入的 ID
        out_id = sqlite3_last_insert_rowid(sqlite_connection_);
        
        // 8. 清理资源
        sqlite3_finalize(stmt);
        gaiaFreeGeomColl(gaia_geom);
        
        return DbResult::kSuccess;
    }
    
    DbResult SpatialQuery(const std::string& table,
                           const std::string& geom_column,
                           SpatialOperator op,
                           const CNGeometry* query_geom,
                           std::vector<CNGeometryPtr>& results) override {
        if (!query_geom || !sqlite_connection_) {
            return DbResult::kInvalidGeometry;
        }
        
        // 1. 转换查询几何
        gaiaGeomCollPtr gaia_query = GeometryToGaia(query_geom);
        if (!gaia_query) {
            return DbResult::kGeometryParseError;
        }
        
        // 2. 转换为 BLOB
        unsigned char* query_blob = nullptr;
        size_t query_blob_size = 0;
        gaiaToSpatiaLiteBlob(gaia_query, &query_blob, &query_blob_size);
        
        // 3. 构造空间查询 SQL
        std::string spatial_function;
        switch (op) {
            case SpatialOperator::kIntersects:
                spatial_function = "ST_Intersects";
                break;
            case SpatialOperator::kContains:
                spatial_function = "ST_Contains";
                break;
            // ... 其他操作符
            default:
                gaiaFreeGeomColl(gaia_query);
                return DbResult::kNotSupported;
        }
        
        // 使用 R-Tree 索引优化查询
        std::ostringstream sql;
        sql << "SELECT " << geom_column << " FROM " << table << " AS t"
            << " WHERE " << spatial_function << "(t." << geom_column 
            << ", GeomFromWKB(?)) = 1"
            << " AND t.ROWID IN ("
            << "   SELECT ROWID FROM SpatialIndex"
            << "   WHERE f_table_name = '" << table << "'"
            << "     AND f_geometry_column = '" << geom_column << "'"
            << "     AND search_frame = GeomFromWKB(?)"
            << ")";
        
        // 4. 准备语句
        sqlite3_stmt* stmt = nullptr;
        int ret = sqlite3_prepare_v2(
            sqlite_connection_,
            sql.str().c_str(),
            -1,
            &stmt,
            nullptr
        );
        
        if (ret != SQLITE_OK) {
            last_error_ = sqlite3_errmsg(sqlite_connection_);
            free(query_blob);
            gaiaFreeGeomColl(gaia_query);
            return DbResult::kSqlError;
        }
        
        // 5. 绑定参数 (两个相同的 BLOB)
        sqlite3_bind_blob(stmt, 1, query_blob, 
                         static_cast<int>(query_blob_size), SQLITE_TRANSIENT);
        sqlite3_bind_blob(stmt, 2, query_blob, 
                         static_cast<int>(query_blob_size), free);
        
        // 6. 执行查询
        results.clear();
        while ((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
            // 获取 BLOB 数据
            const void* blob_data = sqlite3_column_blob(stmt, 0);
            int blob_size = sqlite3_column_bytes(stmt, 0);
            
            // 解析为 gaiaGeomColl
            gaiaGeomCollPtr gaia_result = gaiaFromSpatiaLiteBlob(
                static_cast<const unsigned char*>(blob_data),
                blob_size
            );
            
            if (gaia_result) {
                // 转换为 CNGeometry
                CNGeometryPtr geom = GaiaToGeometry(gaia_result);
                if (geom) {
                    results.push_back(std::move(geom));
                }
                gaiaFreeGeomColl(gaia_result);
            }
        }
        
        sqlite3_finalize(stmt);
        gaiaFreeGeomColl(gaia_query);
        
        return DbResult::kSuccess;
    }
    
    // ========== 其他方法实现类似 ==========
    
    DatabaseType GetType() const override { return DatabaseType::kSpatiaLite; }
    
private:
    sqlite3* sqlite_connection_ = nullptr;
    void* spatialite_cache_ = nullptr;
    std::string last_error_;
    
    // ========== 几何对象转换 ==========
    
    /**
     * @brief 将 CNGeometry 转换为 gaiaGeomColl
     */
    gaiaGeomCollPtr GeometryToGaia(const CNGeometry* geometry) {
        if (!geometry || geometry->IsEmpty()) {
            return nullptr;
        }
        
        gaiaGeomCollPtr gaia_geom = gaiaAllocGeomColl();
        gaia_geom->Srid = geometry->GetSRID();
        
        switch (geometry->GetGeometryType()) {
            case GeomType::kPoint: {
                const CNPoint* point = dynamic_cast<const CNPoint*>(geometry);
                gaiaAddPointToGeomColl(gaia_geom, point->GetX(), point->GetY());
                break;
            }
            case GeomType::kLineString: {
                const CNLineString* line = dynamic_cast<const CNLineString*>(geometry);
                size_t num_points = line->GetNumPoints();
                gaiaLinestringPtr gaia_line = gaiaAddLinestringToGeomColl(
                    gaia_geom, static_cast<int>(num_points));
                
                for (size_t i = 0; i < num_points; ++i) {
                    Coordinate coord = line->GetPointN(i);
                    gaiaSetPoint(gaia_line->Coords, static_cast<int>(i),
                                coord.x, coord.y);
                }
                break;
            }
            // ... 其他几何类型
            default:
                gaiaFreeGeomColl(gaia_geom);
                return nullptr;
        }
        
        return gaia_geom;
    }
    
    /**
     * @brief 将 gaiaGeomColl 转换为 CNGeometry
     */
    CNGeometryPtr GaiaToGeometry(gaiaGeomCollPtr gaia_geom) {
        if (!gaia_geom || gaia_geom->First == nullptr) {
            return nullptr;
        }
        
        // SpatiaLite 几何集合中只有一个几何对象
        gaiaGeometryPtr geom = gaia_geom->First;
        int srid = gaia_geom->Srid;
        
        switch (geom->Type) {
            case GAIA_POINT: {
                gaiaPointPtr pt = geom->FirstPoint;
                auto point = CNPoint::Create(pt->X, pt->Y);
                point->SetSRID(srid);
                return point;
            }
            case GAIA_LINESTRING: {
                gaiaLinestringPtr line = geom->FirstLinestring;
                CoordinateList coords;
                coords.reserve(line->Points);
                
                for (int i = 0; i < line->Points; ++i) {
                    double x, y;
                    gaiaGetPoint(line->Coords, i, &x, &y);
                    coords.emplace_back(x, y);
                }
                
                auto linestring = CNLineString::Create(coords);
                linestring->SetSRID(srid);
                return linestring;
            }
            // ... 其他几何类型
            default:
                return nullptr;
        }
    }
};

} // namespace DB
} // namespace OGC
```

---

## 5. 数据库抽象层

### 5.1 连接工厂

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 数据库连接工厂
 * 
 * 线程安全：静态工厂方法
 */
class DbConnectionFactory {
public:
    /**
     * @brief 创建数据库连接
     * @param db_type 数据库类型
     * @param connection_string 连接字符串
     * @return 连接对象智能指针
     */
    static std::unique_ptr<DbConnection> CreateConnection(
        DatabaseType db_type,
        const std::string& connection_string = ""
    );
    
    /**
     * @brief 从配置文件创建连接
     */
    static std::unique_ptr<DbConnection> CreateFromConfig(
        const std::string& config_file
    );
};

// 实现示例
std::unique_ptr<DbConnection> DbConnectionFactory::CreateConnection(
    DatabaseType db_type,
    const std::string& connection_string
) {
    std::unique_ptr<DbConnection> connection;
    
    switch (db_type) {
        case DatabaseType::kPostGIS:
            connection.reset(new PostGISConnection());
            break;
        case DatabaseType::kSpatiaLite:
            connection.reset(new SpatiaLiteConnection());
            break;
        default:
            return nullptr;
    }
    
    if (!connection_string.empty()) {
        DbResult result = connection->Connect(connection_string);
        if (result != DbResult::kSuccess) {
            return nullptr;
        }
    }
    
    return connection;
}
```

### 5.2 预编译语句

```cpp
/**
 * @brief 数据库预编译语句接口
 */
class DbStatement {
public:
    virtual ~DbStatement() = default;
    
    // ========== 参数绑定 ==========
    
    virtual DbResult BindNull(int index) = 0;
    virtual DbResult BindInt(int index, int32_t value) = 0;
    virtual DbResult BindInt64(int index, int64_t value) = 0;
    virtual DbResult BindDouble(int index, double value) = 0;
    virtual DbResult BindText(int index, const std::string& value) = 0;
    virtual DbResult BindBlob(int index, const void* data, size_t size) = 0;
    virtual DbResult BindGeometry(int index, const CNGeometry* geometry) = 0;
    
    // ========== 执行 ==========
    
    virtual DbResult Execute() = 0;
    virtual DbResult ExecuteQuery(std::unique_ptr<DbResultSet>& result) = 0;
    
    // ========== 重置 ==========
    
    virtual DbResult Reset() = 0;
    virtual DbResult ClearBindings() = 0;
};
```

### 5.3 结果集

```cpp
/**
 * @brief 数据库结果集接口
 */
class DbResultSet {
public:
    virtual ~DbResultSet() = default;
    
    // ========== 游标操作 ==========
    
    virtual bool Next() = 0;
    virtual bool IsAfterLast() const = 0;
    virtual size_t GetRowCount() const = 0;
    virtual size_t GetColumnCount() const = 0;
    
    // ========== 字段访问 ==========
    
    virtual int GetInt(int column) const = 0;
    virtual int64_t GetInt64(int column) const = 0;
    virtual double GetDouble(int column) const = 0;
    virtual std::string GetText(int column) const = 0;
    virtual std::vector<uint8_t> GetBlob(int column) const = 0;
    
    /**
     * @brief 获取几何对象
     * @param column 列索引
     * @return 几何对象智能指针
     */
    virtual CNGeometryPtr GetGeometry(int column) const = 0;
    
    // ========== 元数据 ==========
    
    virtual std::string GetColumnName(int column) const = 0;
    virtual ColumnType GetColumnType(int column) const = 0;
    virtual bool IsNull(int column) const = 0;
};
```

---

## 6. 连接池和资源管理

### 6.1 连接池设计

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 数据库连接池
 * 
 * 线程安全：使用互斥锁保护内部状态
 */
class DbConnectionPool {
public:
    /**
     * @brief 构造连接池
     * @param db_type 数据库类型
     * @param connection_string 连接字符串
     * @param min_size 最小连接数
     * @param max_size 最大连接数
     */
    DbConnectionPool(DatabaseType db_type,
                     const std::string& connection_string,
                     size_t min_size = 5,
                     size_t max_size = 20);
    
    ~DbConnectionPool();
    
    // 禁止拷贝
    DbConnectionPool(const DbConnectionPool&) = delete;
    DbConnectionPool& operator=(const DbConnectionPool&) = delete;
    
    /**
     * @brief 获取连接
     * @param timeout_ms 超时时间(毫秒), -1 表示无限等待
     * @return 连接对象智能指针
     */
    std::unique_ptr<DbConnection> Acquire(int timeout_ms = -1);
    
    /**
     * @brief 归还连接
     */
    void Release(std::unique_ptr<DbConnection> connection);
    
    /**
     * @brief 获取可用连接数
     */
    size_t GetAvailableCount() const;
    
    /**
     * @brief 获取总连接数
     */
    size_t GetTotalCount() const;
    
    /**
     * @brief 关闭所有连接
     */
    void CloseAll();
    
private:
    DatabaseType db_type_;
    std::string connection_string_;
    size_t min_size_;
    size_t max_size_;
    
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::queue<std::unique_ptr<DbConnection>> available_connections_;
    size_t total_count_ = 0;
    
    /**
     * @brief 创建新连接
     */
    std::unique_ptr<DbConnection> CreateConnection();
};

// 实现示例
std::unique_ptr<DbConnection> DbConnectionPool::Acquire(int timeout_ms) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    // 等待可用连接或超时
    auto predicate = [this]() { 
        return !available_connections_.empty() || total_count_ < max_size_; 
    };
    
    if (timeout_ms >= 0) {
        if (!condition_.wait_for(lock, 
                                 std::chrono::milliseconds(timeout_ms),
                                 predicate)) {
            return nullptr;  // 超时
        }
    } else {
        condition_.wait(lock, predicate);
    }
    
    // 从池中获取连接
    if (!available_connections_.empty()) {
        auto connection = std::move(available_connections_.front());
        available_connections_.pop();
        
        // 检查连接是否有效
        if (connection && connection->IsConnected()) {
            return connection;
        } else {
            total_count_--;
            // 递归重试
            return Acquire(timeout_ms);
        }
    }
    
    // 创建新连接
    if (total_count_ < max_size_) {
        auto connection = CreateConnection();
        if (connection) {
            total_count_++;
            return connection;
        }
    }
    
    return nullptr;
}

void DbConnectionPool::Release(std::unique_ptr<DbConnection> connection) {
    if (!connection) {
        return;
    }
    
    std::unique_lock<std::mutex> lock(mutex_);
    
    // 检查连接是否有效
    if (connection->IsConnected() && total_count_ <= max_size_) {
        available_connections_.push(std::move(connection));
        condition_.notify_one();
    } else {
        total_count_--;
    }
}

} // namespace DB
} // namespace OGC
```

### 6.2 RAII 连接守卫

```cpp
/**
 * @brief 连接守卫 (RAII)
 * 
 * 自动归还连接到连接池
 */
class ScopedConnection {
public:
    explicit ScopedConnection(DbConnectionPool& pool)
        : pool_(pool), connection_(pool.Acquire()) {}
    
    ~ScopedConnection() {
        if (connection_) {
            pool_.Release(std::move(connection_));
        }
    }
    
    // 禁止拷贝
    ScopedConnection(const ScopedConnection&) = delete;
    ScopedConnection& operator=(const ScopedConnection&) = delete;
    
    // 支持移动
    ScopedConnection(ScopedConnection&&) noexcept = default;
    ScopedConnection& operator=(ScopedConnection&&) noexcept = default;
    
    /**
     * @brief 获取原始连接指针
     */
    DbConnection* Get() const { return connection_.get(); }
    
    /**
     * @brief 重载 -> 运算符
     */
    DbConnection* operator->() const { return connection_.get(); }
    
    /**
     * @brief 检查连接是否有效
     */
    explicit operator bool() const { return connection_ != nullptr; }
    
private:
    DbConnectionPool& pool_;
    std::unique_ptr<DbConnection> connection_;
};
```

---

## 7. 性能优化策略

### 7.1 PostGIS 性能优化

#### 批量插入 (COPY 命令)

```cpp
/**
 * @brief PostGIS 批量插入优化
 * 
 * 使用 COPY 命令实现高性能批量插入
 */
class PostGISBulkInserter {
public:
    PostGISBulkInserter(DbConnection* connection,
                        const std::string& table,
                        const std::string& geom_column,
                        const std::vector<std::string>& attribute_columns);
    
    ~PostGISBulkInserter();
    
    /**
     * @brief 添加一行数据
     */
    DbResult AddRow(const CNGeometry* geometry,
                    const std::vector<std::string>& attributes);
    
    /**
     * @brief 批量添加数据
     */
    DbResult AddRows(const std::vector<CNGeometryPtr>& geometries,
                     const std::vector<std::vector<std::string>>& attributes);
    
    /**
     * @brief 提交所有数据
     */
    DbResult Commit();
    
    /**
     * @brief 获取已添加的行数
     */
    size_t GetRowCount() const { return row_count_; }
    
private:
    DbConnection* connection_;
    std::string table_;
    std::string geom_column_;
    std::vector<std::string> attribute_columns_;
    size_t row_count_ = 0;
    size_t batch_size_ = 1000;  // 每批提交行数
    
    std::ostringstream buffer_;  // 缓存 COPY 数据
};
```

**性能对比**:

| 方法 | 100 万行耗时 | 说明 |
|------|-------------|------|
| 单条 INSERT | ~45 分钟 | 不推荐 |
| 批量 INSERT (1000行/批) | ~8 分钟 | 较好 |
| COPY (二进制) | **~45 秒** | 推荐 |

### 7.2 SpatiaLite 性能优化

#### 事务批量插入

```cpp
/**
 * @brief SpatiaLite 批量插入优化
 * 
 * 使用事务包裹批量插入
 */
class SpatiaLiteBulkInserter {
public:
    SpatiaLiteBulkInserter(DbConnection* connection,
                           const std::string& table,
                           const std::string& geom_column);
    
    ~SpatiaLiteBulkInserter();
    
    DbResult Begin();
    DbResult AddRow(const CNGeometry* geometry,
                    const std::map<std::string, std::string>& attributes);
    DbResult Commit();
    
private:
    DbConnection* connection_;
    std::string table_;
    std::string geom_column_;
    sqlite3_stmt* insert_stmt_ = nullptr;
    size_t batch_count_ = 0;
    const size_t kBatchSize = 1000;
};
```

### 7.3 空间索引优化

#### PostGIS GiST 索引

```sql
-- 创建 GiST 空间索引
CREATE INDEX idx_table_geom ON my_table USING GIST (geom);

-- 使用索引优化查询
SELECT * FROM my_table 
WHERE geom && ST_MakeEnvelope(xmin, ymin, xmax, ymax)  -- 边界框过滤
  AND ST_Intersects(geom, ST_MakePoint(x, y));        -- 精确几何判断
```

#### SpatiaLite R-Tree 索引

```sql
-- 创建 R-Tree 空间索引
SELECT CreateSpatialIndex('my_table', 'geom');

-- 使用索引优化查询
SELECT * FROM my_table AS t
WHERE t.ROWID IN (
    SELECT ROWID FROM SpatialIndex
    WHERE f_table_name = 'my_table'
      AND f_geometry_column = 'geom'
      AND search_frame = BuildMbr(xmin, ymin, xmax, ymax)
)
AND ST_Intersects(t.geom, GeomFromText('POINT(x y)'));
```

### 7.4 查询性能优化

#### 7.4.1 使用预编译语句缓存

```cpp
/**
 * @brief 预编译语句缓存
 * 
 * 避免重复解析 SQL,提升性能
 */
class StatementCache {
public:
    explicit StatementCache(DbConnection* connection) 
        : connection_(connection) {}
    
    /**
     * @brief 获取或创建预编译语句
     */
    DbStatement* GetOrCreate(const std::string& name, const std::string& sql) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = cache_.find(name);
        if (it != cache_.end()) {
            return it->second.get();
        }
        
        std::unique_ptr<DbStatement> stmt;
        DbResult result = connection_->PrepareStatement(name, sql, stmt);
        if (result != DbResult::kSuccess) {
            return nullptr;
        }
        
        DbStatement* raw_ptr = stmt.get();
        cache_[name] = std::move(stmt);
        return raw_ptr;
    }
    
    void Clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.clear();
    }
    
private:
    DbConnection* connection_;
    std::map<std::string, std::unique_ptr<DbStatement>> cache_;
    mutable std::mutex mutex_;
};
```

#### 7.4.2 几何对象缓存

```cpp
/**
 * @brief 几何对象缓存
 * 
 * 缓存常用几何对象,避免重复解析 WKB
 */
class GeometryCache {
public:
    /**
     * @brief 获取缓存的几何对象
     * @param id 几何对象 ID
     * @return 几何对象智能指针 (nullptr 表示未缓存)
     */
    CNGeometryPtr Get(int64_t id) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = cache_.find(id);
        if (it != cache_.end()) {
            return it->second->Clone();
        }
        return nullptr;
    }
    
    /**
     * @brief 缓存几何对象
     */
    void Put(int64_t id, const CNGeometry* geometry) {
        if (!geometry || cache_size_ >= max_size_) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // LRU 策略: 移除最旧的条目
        if (cache_size_ >= max_size_) {
            int64_t oldest_id = lru_list_.front();
            lru_list_.pop_front();
            cache_.erase(oldest_id);
            cache_size_--;
        }
        
        cache_[id] = geometry->Clone();
        lru_list_.push_back(id);
        cache_size_++;
    }
    
    void Clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        cache_.clear();
        lru_list_.clear();
        cache_size_ = 0;
    }
    
private:
    std::map<int64_t, CNGeometryPtr> cache_;
    std::list<int64_t> lru_list_;
    size_t cache_size_ = 0;
    size_t max_size_ = 1000;
    mutable std::mutex mutex_;
};
```

---

## 8. 线程安全设计

### 8.1 线程安全保证

**设计原则**:

1. **数据库连接**: 每个连接对象非线程安全,推荐以下两种模式:
   - **每线程独立连接**: 每个线程创建自己的连接
   - **连接池模式**: 从连接池获取连接,用完归还

2. **几何对象**: 所有只读操作线程安全,修改操作需要外部同步

3. **缓存和池**: 使用互斥锁保护,线程安全

### 8.2 多线程使用示例

#### 模式1: 每线程独立连接

```cpp
void WorkerThread(const std::string& db_path, const std::vector<CNGeometryPtr>& geometries) {
    // 每个线程创建自己的连接
    auto connection = DbConnectionFactory::CreateConnection(
        DatabaseType::kSpatiaLite, db_path);
    
    if (!connection) {
        std::cerr << "连接失败" << std::endl;
        return;
    }
    
    // 执行数据库操作
    for (const auto& geom : geometries) {
        int64_t id;
        DbResult result = connection->InsertGeometry(
            "points", "geom", geom.get(), {}, id);
        
        if (result != DbResult::kSuccess) {
            std::cerr << "插入失败" << std::endl;
        }
    }
    
    // RAII 自动关闭连接
}

// 启动多个工作线程
std::vector<std::thread> threads;
for (int i = 0; i < 10; ++i) {
    threads.emplace_back(WorkerThread, "test.db", std::ref(geometries[i]));
}

for (auto& t : threads) {
    t.join();
}
```

#### 模式2: 连接池

```cpp
// 创建连接池
DbConnectionPool pool(DatabaseType::kPostGIS, 
                      "host=localhost dbname=gis user=postgres",
                      5, 20);  // min=5, max=20

// 多线程使用连接池
std::vector<std::thread> threads;
for (int i = 0; i < 100; ++i) {
    threads.emplace_back([&pool, i]() {
        // 从连接池获取连接
        ScopedConnection conn(pool);
        if (!conn) {
            std::cerr << "获取连接失败" << std::endl;
            return;
        }
        
        // 执行查询
        std::vector<CNGeometryPtr> results;
        auto query_geom = CNPoint::Create(i * 10, i * 10);
        
        DbResult result = conn->SpatialQuery(
            "points", "geom",
            SpatialOperator::kIntersects,
            query_geom.get(),
            results);
        
        std::cout << "Thread " << i << " found " << results.size() 
                  << " geometries" << std::endl;
        
        // ScopedConnection 自动归还连接
    });
}

for (auto& t : threads) {
    t.join();
}
```

---

## 9. 跨平台支持

### 9.1 平台兼容性

**支持的编译器和平台**:

| 编译器 | 最低版本 | 平台 |
|--------|---------|------|
| MSVC | 2017 (19.1) | Windows |
| GCC | 5.0 | Linux, macOS |
| Clang | 3.5 | Linux, macOS |

**依赖库**:

| 库 | PostGIS | SpatiaLite |
|----|---------|------------|
| libpq | ✓ | - |
| PostgreSQL | ≥ 9.6 | - |
| PostGIS | ≥ 2.5 | - |
| SQLite | - | ≥ 3.8 |
| SpatiaLite | - | ≥ 5.0 |
| GEOS | ≥ 3.6 | ≥ 3.6 |
| PROJ | ≥ 6.0 | ≥ 6.0 |

### 9.2 编译选项

#### CMake 配置

```cmake
cmake_minimum_required(VERSION 3.10)
project(OGCDatabase VERSION 1.0.0 LANGUAGES CXX)

# C++11 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
option(USE_POSTGIS "Enable PostGIS support" ON)
option(USE_SPATIALITE "Enable SpatiaLite support" ON)
option(USE_GEOS "Enable GEOS for geometry operations" ON)
option(USE_PROJ "Enable PROJ for coordinate transformation" ON)
option(ENABLE_TESTS "Build unit tests" ON)

# 查找依赖
find_package(Threads REQUIRED)

if(USE_POSTGIS)
    find_package(PostgreSQL REQUIRED)
    find_package(GEOS REQUIRED)
    add_definitions(-DUSE_POSTGIS)
endif()

if(USE_SPATIALITE)
    find_package(SQLite3 REQUIRED)
    find_package(GEOS REQUIRED)
    find_package(PROJ REQUIRED)
    add_definitions(-DUSE_SPATIALITE)
endif()

# 库目标
add_library(ogc_database
    src/db_connection.cpp
    src/db_result_set.cpp
    src/db_statement.cpp
    src/db_connection_pool.cpp
    src/postgis_connection.cpp
    src/spatialite_connection.cpp
    src/wkb_converter.cpp
    src/geometry_io.cpp
)

target_include_directories(ogc_database
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ogc_database
    PUBLIC
        Threads::Threads
        ogc_geometry  # 几何类库
    PRIVATE
        $<$<BOOL:USE_POSTGIS>:PostgreSQL::PostgreSQL>
        $<$<BOOL:USE_POSTGIS>:GEOS::GEOS>
        $<$<BOOL:USE_SPATIALITE>:SQLite::SQLite3>
        $<$<BOOL:USE_SPATIALITE>:spatialite>
        $<$<BOOL:USE_PROJ>:PROJ::proj>
)

# 编译选项
target_compile_options(ogc_database
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Werror>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Werror>
)

# 测试
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 安装
install(TARGETS ogc_database
    EXPORT OGCDatabaseTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/OGC
    DESTINATION include
)
```

---

## 10. 使用示例

### 10.1 基础使用

```cpp
#include <OGC/db_connection_factory.h>
#include <OGC/cn_point.h>
#include <OGC/cn_polygon.h>

using namespace OGC;
using namespace OGC::DB;

int main() {
    // ========== PostGIS 示例 ==========
    
    // 1. 创建连接
    auto pg_conn = DbConnectionFactory::CreateConnection(
        DatabaseType::kPostGIS,
        "host=localhost port=5432 dbname=gisdb user=postgres password=secret"
    );
    
    if (!pg_conn || !pg_conn->IsConnected()) {
        std::cerr << "PostGIS 连接失败" << std::endl;
        return -1;
    }
    
    // 2. 创建空间表
    DbResult result = pg_conn->CreateSpatialTable(
        "cities", "geom", GeomType::kPoint, 4326, 2);
    
    if (result != DbResult::kSuccess) {
        std::cerr << "创建表失败" << std::endl;
    }
    
    // 3. 插入几何对象
    auto point = CNPoint::Create(116.4, 39.9);  // 北京
    point->SetSRID(4326);
    
    std::map<std::string, std::string> attrs = {
        {"name", "Beijing"},
        {"population", "21540000"}
    };
    
    int64_t id;
    result = pg_conn->InsertGeometry("cities", "geom", point.get(), attrs, id);
    
    if (result == DbResult::kSuccess) {
        std::cout << "插入成功, ID=" << id << std::endl;
    }
    
    // 4. 空间查询
    auto query_geom = CNPoint::Create(116.5, 39.95);
    query_geom->SetSRID(4326);
    
    std::vector<CNGeometryPtr> results;
    result = pg_conn->SpatialQuery(
        "cities", "geom",
        SpatialOperator::kDWithin,  // 距离内
        query_geom.get(),
        results
    );
    
    std::cout << "找到 " << results.size() << " 个城市" << std::endl;
    
    // ========== SpatiaLite 示例 ==========
    
    // 1. 创建连接
    auto sqlite_conn = DbConnectionFactory::CreateConnection(
        DatabaseType::kSpatiaLite,
        "file:/path/to/database.db"
    );
    
    if (!sqlite_conn || !sqlite_conn->IsConnected()) {
        std::cerr << "SpatiaLite 连接失败" << std::endl;
        return -1;
    }
    
    // 2. 创建空间表
    result = sqlite_conn->CreateSpatialTable(
        "regions", "geom", GeomType::kPolygon, 4326, 2);
    
    // 3. 创建空间索引
    result = sqlite_conn->CreateSpatialIndex("regions", "geom");
    
    // 4. 插入多边形
    auto ring = CNLinearRing::CreateRectangle(0, 0, 10, 10);
    auto polygon = CNPolygon::Create();
    polygon->SetExteriorRing(std::move(ring));
    polygon->SetSRID(4326);
    
    attrs = {{"name", "Region A"}};
    result = sqlite_conn->InsertGeometry("regions", "geom", polygon.get(), attrs, id);
    
    return 0;
}
```

### 10.2 批量操作

```cpp
// ========== PostGIS 批量插入 ==========
{
    PostGISBulkInserter bulk_inserter(pg_conn.get(), "points", "geom", {"name", "value"});
    
    // 开始批量插入
    for (int i = 0; i < 10000; ++i) {
        auto point = CNPoint::Create(i * 0.1, i * 0.1);
        point->SetSRID(4326);
        
        std::vector<std::string> attrs = {
            "Point_" + std::to_string(i),
            std::to_string(i * 100)
        };
        
        bulk_inserter.AddRow(point.get(), attrs);
    }
    
    // 提交
    DbResult result = bulk_inserter.Commit();
    std::cout << "插入 " << bulk_inserter.GetRowCount() << " 行" << std::endl;
}

// ========== SpatiaLite 批量插入 ==========
{
    SpatiaLiteBulkInserter bulk_inserter(sqlite_conn.get(), "points", "geom");
    
    bulk_inserter.Begin();
    
    for (int i = 0; i < 10000; ++i) {
        auto point = CNPoint::Create(i * 0.1, i * 0.1);
        point->SetSRID(4326);
        
        std::map<std::string, std::string> attrs = {
            {"name", "Point_" + std::to_string(i)},
            {"value", std::to_string(i * 100)}
        };
        
        bulk_inserter.AddRow(point.get(), attrs);
    }
    
    DbResult result = bulk_inserter.Commit();
}
```

### 10.3 连接池使用

```cpp
// 创建连接池
DbConnectionPool pool(
    DatabaseType::kPostGIS,
    "host=localhost dbname=gisdb user=postgres",
    5,   // 最小连接数
    20   // 最大连接数
);

// 多线程使用连接池
std::vector<std::thread> threads;
for (int i = 0; i < 100; ++i) {
    threads.emplace_back([&pool, i]() {
        // 从连接池获取连接 (RAII)
        ScopedConnection conn(pool);
        if (!conn) {
            std::cerr << "获取连接超时" << std::endl;
            return;
        }
        
        // 执行数据库操作
        auto point = CNPoint::Create(i * 10, i * 10);
        int64_t id;
        
        DbResult result = conn->InsertGeometry(
            "points", "geom", point.get(), {}, id);
        
        if (result == DbResult::kSuccess) {
            std::cout << "Thread " << i << " inserted ID=" << id << std::endl;
        }
        
        // ScopedConnection 析构时自动归还连接
    });
}

for (auto& t : threads) {
    t.join();
}

std::cout << "连接池统计: " << pool.GetAvailableCount() << "/" 
          << pool.GetTotalCount() << std::endl;
```

---

## 11. 编译和依赖

### 11.1 目录结构

```
ogc_database/
├── include/
│   └── OGC/
│       ├── db_common.h              # 公共定义
│       ├── db_connection.h          # 连接接口
│       ├── db_result_set.h          # 结果集接口
│       ├── db_statement.h           # 预编译语句接口
│       ├── db_connection_pool.h     # 连接池
│       ├── db_factory.h             # 连接工厂
│       ├── postgis_connection.h     # PostGIS 连接
│       ├── spatialite_connection.h  # SpatiaLite 连接
│       ├── wkb_converter.h          # WKB 转换器
│       └── geometry_io.h            # 几何 I/O
├── src/
│   ├── db_connection.cpp
│   ├── db_result_set.cpp
│   ├── db_statement.cpp
│   ├── db_connection_pool.cpp
│   ├── postgis_connection.cpp
│   ├── spatialite_connection.cpp
│   ├── wkb_converter.cpp
│   └── geometry_io.cpp
├── tests/
│   ├── test_postgis.cpp
│   ├── test_spatialite.cpp
│   └── test_connection_pool.cpp
├── CMakeLists.txt
└── README.md
```

### 11.2 依赖说明

| 依赖项 | 版本要求 | 用途 | 必需性 |
|--------|---------|------|--------|
| **C++ 编译器** | C++11 | 核心语言特性 | 必需 |
| **CMake** | ≥ 3.10 | 构建系统 | 必需 |
| **Threads** | - | 多线程支持 | 必需 |
| **PostgreSQL** | ≥ 9.6 | PostGIS 后端 | PostGIS 必需 |
| **PostGIS** | ≥ 2.5 | 空间扩展 | PostGIS 必需 |
| **SQLite** | ≥ 3.8 | SpatiaLite 后端 | SpatiaLite 必需 |
| **SpatiaLite** | ≥ 5.0 | 空间扩展 | SpatiaLite 必需 |
| **GEOS** | ≥ 3.6 | 几何运算 | 必需 |
| **PROJ** | ≥ 6.0 | 坐标转换 | 必需 |
| **Google Test** | ≥ 1.10 | 单元测试 | 可选 |

---

## 12. 错误处理

### 12.1 错误处理策略

**双重错误处理机制**:

1. **Result 模式 (推荐)**: 函数返回错误码,适合可恢复错误
2. **异常模式 (可选)**: 抛出异常,适合不可恢复错误

```cpp
// Result 模式示例
DbResult result = connection->InsertGeometry(...);
if (result != DbResult::kSuccess) {
    std::cerr << "错误: " << GetDbResultString(result) << std::endl;
    // 错误处理...
}

// 异常模式示例
namespace OGC {
namespace DB {
    enum class ErrorHandlingPolicy {
        kReturnCode,  // 默认: 返回错误码
        kThrow        // 抛出异常
    };
    
    void SetErrorHandlingPolicy(ErrorHandlingPolicy policy);
}
}

try {
    connection->InsertGeometry(...);
} catch (const DbException& e) {
    std::cerr << "数据库错误: " << e.what() << std::endl;
    std::cerr << "错误码: " << GetDbResultString(e.GetErrorCode()) << std::endl;
}
```

### 12.2 错误信息获取

```cpp
/**
 * @brief 获取错误码字符串表示
 */
const char* GetDbResultString(DbResult result);

/**
 * @brief 获取错误码详细描述
 */
std::string GetDbResultDescription(DbResult result);

/**
 * @brief 数据库异常类
 */
class DbException : public std::runtime_error {
public:
    explicit DbException(DbResult code, const std::string& message = "");
    
    DbResult GetErrorCode() const noexcept;
    const std::string& GetSqlState() const noexcept;
    
private:
    DbResult code_;
    std::string sql_state_;
};
```

---

## 13. 测试策略

### 13.1 单元测试

```cpp
#include <gtest/gtest.h>
#include <OGC/postgis_connection.h>
#include <OGC/cn_point.h>

// PostGIS 连接测试
class PostGISConnectionTest : public ::testing::Test {
protected:
    void SetUp() override {
        connection = OGC::DB::DbConnectionFactory::CreateConnection(
            OGC::DB::DatabaseType::kPostGIS,
            "host=localhost dbname=test_db user=test password=test"
        );
    }
    
    void TearDown() override {
        connection.reset();
    }
    
    std::unique_ptr<OGC::DB::DbConnection> connection;
};

TEST_F(PostGISConnectionTest, ConnectAndDisconnect) {
    ASSERT_TRUE(connection != nullptr);
    EXPECT_TRUE(connection->IsConnected());
    
    connection->Disconnect();
    EXPECT_FALSE(connection->IsConnected());
}

TEST_F(PostGISConnectionTest, InsertAndSelectGeometry) {
    // 创建空间表
    OGC::DB::DbResult result = connection->CreateSpatialTable(
        "test_points", "geom", OGC::GeomType::kPoint, 4326, 2);
    ASSERT_EQ(result, OGC::DB::DbResult::kSuccess);
    
    // 插入点
    auto point = OGC::CNPoint::Create(116.4, 39.9);
    point->SetSRID(4326);
    
    std::map<std::string, std::string> attrs = {{"name", "Beijing"}};
    int64_t id;
    result = connection->InsertGeometry("test_points", "geom", point.get(), attrs, id);
    ASSERT_EQ(result, OGC::DB::DbResult::kSuccess);
    EXPECT_GT(id, 0);
    
    // 查询点
    std::vector<OGC::CNGeometryPtr> results;
    result = connection->SelectGeometries("test_points", "geom", "name='Beijing'", results);
    ASSERT_EQ(result, OGC::DB::DbResult::kSuccess);
    EXPECT_EQ(results.size(), 1);
}

TEST_F(PostGISConnectionTest, SpatialQuery) {
    // 插入测试数据
    auto point1 = OGC::CNPoint::Create(0, 0);
    auto point2 = OGC::CNPoint::Create(10, 10);
    auto point3 = OGC::CNPoint::Create(20, 20);
    
    connection->CreateSpatialTable("spatial_test", "geom", OGC::GeomType::kPoint, 4326, 2);
    
    int64_t id;
    connection->InsertGeometry("spatial_test", "geom", point1.get(), {}, id);
    connection->InsertGeometry("spatial_test", "geom", point2.get(), {}, id);
    connection->InsertGeometry("spatial_test", "geom", point3.get(), {}, id);
    
    // 空间查询
    auto query_rect = OGC::CNPolygon::CreateRectangle(-1, -1, 11, 11);
    query_rect->SetSRID(4326);
    
    std::vector<OGC::CNGeometryPtr> results;
    OGC::DB::DbResult result = connection->SpatialQuery(
        "spatial_test", "geom",
        OGC::DB::SpatialOperator::kIntersects,
        query_rect.get(),
        results
    );
    
    ASSERT_EQ(result, OGC::DB::DbResult::kSuccess);
    EXPECT_EQ(results.size(), 2);  // point1 和 point2
}
```

### 13.2 性能测试

```cpp
#include <benchmark/benchmark.h>

// 批量插入性能测试
static void BM_BulkInsert(benchmark::State& state) {
    auto connection = OGC::DB::DbConnectionFactory::CreateConnection(
        OGC::DB::DatabaseType::kPostGIS, "...");
    
    connection->CreateSpatialTable("perf_test", "geom", OGC::GeomType::kPoint, 4326, 2);
    
    for (auto _ : state) {
        state.PauseTiming();
        connection->Execute("DELETE FROM perf_test");
        state.ResumeTiming();
        
        OGC::DB::PostGISBulkInserter inserter(connection.get(), "perf_test", "geom", {});
        
        for (int i = 0; i < state.range(0); ++i) {
            auto point = OGC::CNPoint::Create(i * 0.001, i * 0.001);
            inserter.AddRow(point.get(), {});
        }
        
        inserter.Commit();
    }
    
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_BulkInsert)->RangeMultiplier(10)->Range(100, 100000)->Complexity();

// 空间查询性能测试
static void BM_SpatialQuery(benchmark::State& state) {
    auto connection = OGC::DB::DbConnectionFactory::CreateConnection(
        OGC::DB::DatabaseType::kPostGIS, "...");
    
    // ... 准备测试数据 ...
    
    auto query_geom = OGC::CNPoint::Create(500, 500);
    
    for (auto _ : state) {
        std::vector<OGC::CNGeometryPtr> results;
        connection->SpatialQuery("perf_test", "geom",
                                 OGC::DB::SpatialOperator::kDWithin,
                                 query_geom.get(), results);
        benchmark::DoNotOptimize(results);
    }
    
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_SpatialQuery)->RangeMultiplier(10)->Range(100, 100000)->Complexity();
```

---

## 14. MVCC和事务隔离级别设计

### 14.1 事务隔离级别

#### 14.1.1 隔离级别定义

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 事务隔离级别枚举
 * 
 * 对应SQL标准的隔离级别
 */
enum class IsolationLevel {
    kReadUncommitted = 0,  // 读未提交
    kReadCommitted = 1,    // 读已提交 (PostgreSQL默认)
    kRepeatableRead = 2,   // 可重复读
    kSerializable = 3      // 可串行化 (SQLite默认)
};

/**
 * @brief 事务行为选项
 */
struct TransactionOptions {
    IsolationLevel isolation_level = IsolationLevel::kReadCommitted;
    bool read_only = false;
    int timeout_ms = 30000;  // 事务超时时间
    bool deferrable = false; // 可延迟事务 (仅PostgreSQL)
};

} // namespace DB
} // namespace OGC
```

#### 14.1.2 PostGIS事务隔离级别

```cpp
/**
 * @brief PostGIS事务隔离级别实现
 */
class PostGITransactionManager {
public:
    /**
     * @brief 开始指定隔离级别的事务
     */
    DbResult BeginTransaction(const TransactionOptions& options) {
        std::ostringstream sql;
        
        // 设置隔离级别
        sql << "START TRANSACTION ISOLATION LEVEL ";
        switch (options.isolation_level) {
            case IsolationLevel::kReadUncommitted:
                sql << "READ UNCOMMITTED";
                break;
            case IsolationLevel::kReadCommitted:
                sql << "READ COMMITTED";
                break;
            case IsolationLevel::kRepeatableRead:
                sql << "REPEATABLE READ";
                break;
            case IsolationLevel::kSerializable:
                sql << "SERIALIZABLE";
                if (options.deferrable) {
                    sql << ", DEFERRABLE";
                }
                break;
        }
        
        if (options.read_only) {
            sql << ", READ ONLY";
        }
        
        return Execute(sql.str());
    }
    
    /**
     * @brief 设置事务超时
     */
    DbResult SetTransactionTimeout(int timeout_ms) {
        return Execute("SET statement_timeout = " + std::to_string(timeout_ms));
    }
    
    /**
     * @brief 获取当前隔离级别
     */
    IsolationLevel GetCurrentIsolationLevel() const;
};
```

#### 14.1.3 SpatiaLite事务隔离级别

```cpp
/**
 * @brief SpatiaLite事务隔离级别实现
 * 
 * 注意: SQLite仅支持SERIALIZABLE和READ UNCOMMITTED
 */
class SpatiaLiteTransactionManager {
public:
    DbResult BeginTransaction(const TransactionOptions& options) {
        // SQLite默认是SERIALIZABLE
        // 可以通过PRAGMA read_uncommitted启用READ UNCOMMITTED
        
        if (options.isolation_level == IsolationLevel::kReadUncommitted) {
            Execute("PRAGMA read_uncommitted = ON");
        } else {
            Execute("PRAGMA read_uncommitted = OFF");
        }
        
        return Execute("BEGIN IMMEDIATE");  // 立即获取写锁
    }
    
    /**
     * @brief 设置繁忙超时 (SQLite的锁等待机制)
     */
    DbResult SetBusyTimeout(int timeout_ms) {
        return Execute("PRAGMA busy_timeout = " + std::to_string(timeout_ms));
    }
};
```

### 14.2 MVCC机制说明

#### 14.2.1 PostgreSQL MVCC特性

```cpp
/**
 * @brief PostgreSQL MVCC特性封装
 */
class PostGISMVCCSupport {
public:
    /**
     * @brief 获取事务快照
     */
    struct SnapshotInfo {
        int64_t xmin;  // 快照开始时的活跃事务ID
        int64_t xmax;  // 快照结束时的下一个事务ID
        std::vector<int64_t> active_txids;  // 活跃事务ID列表
    };
    
    SnapshotInfo GetCurrentSnapshot();
    
    /**
     * @brief 设置事务快照 (用于可重复读)
     */
    DbResult SetTransactionSnapshot(const SnapshotInfo& snapshot);
    
    /**
     * @brief 检查行是否可见
     */
    bool IsRowVisible(int64_t xmin, int64_t xmax, const SnapshotInfo& snapshot);
    
    /**
     * @brief 获取表的事务ID范围
     */
    struct TableTransactionRange {
        int64_t min_txid;
        int64_t max_txid;
    };
    TableTransactionRange GetTableTransactionRange(const std::string& table);
};
```

#### 14.2.2 事务可见性规则

```cpp
/**
 * @brief MVCC可见性判断规则
 * 
 * PostgreSQL规则:
 * 1. xmin已提交 且 xmin < snapshot.xmin → 可见
 * 2. xmin在active_txids中 → 不可见
 * 3. xmax已提交 且 xmax < snapshot.xmin → 不可见 (已被删除)
 * 4. xmax未提交 或 xmax在active_txids中 → 可见 (删除未生效)
 */
class MVCCVisibilityChecker {
public:
    enum class Visibility {
        kVisible,
        kInvisible,
        kDeleted
    };
    
    Visibility CheckVisibility(
        int64_t xmin, bool xmin_committed,
        int64_t xmax, bool xmax_committed,
        const PostGISMVCCSupport::SnapshotInfo& snapshot
    );
};
```

### 14.3 死锁检测和处理

#### 14.3.1 死锁检测机制

```cpp
/**
 * @brief 死锁检测和处理策略
 */
class DeadlockHandler {
public:
    /**
     * @brief 死锁处理策略
     */
    enum class Policy {
        kAbort,      // 立即中止 (默认)
        kRetry,      // 自动重试
        kWait        // 等待超时
    };
    
    struct DeadlockConfig {
        Policy policy = Policy::kRetry;
        int max_retries = 3;
        int retry_delay_ms = 100;
        int lock_timeout_ms = 5000;
    };
    
    /**
     * @brief 检测是否为死锁错误
     */
    static bool IsDeadlockError(DbResult result, const std::string& error_msg);
    
    /**
     * @brief 执行带死锁重试的操作
     */
    template<typename Func>
    DbResult ExecuteWithRetry(Func&& func, const DeadlockConfig& config) {
        int attempts = 0;
        while (attempts <= config.max_retries) {
            DbResult result = func();
            
            if (result == DbResult::kSuccess) {
                return result;
            }
            
            if (result == DbResult::kDeadlock || 
                result == DbResult::kSerializationFailure) {
                attempts++;
                if (attempts <= config.max_retries) {
                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(config.retry_delay_ms));
                    continue;
                }
            }
            
            return result;
        }
        return DbResult::kDeadlock;
    }
};
```

#### 14.3.2 锁等待超时配置

```cpp
/**
 * @brief 锁等待超时配置接口
 */
class LockTimeoutConfig {
public:
    // PostgreSQL
    static DbResult SetPostgreSQLLockTimeout(DbConnection* conn, int timeout_ms) {
        return conn->Execute(
            "SET lock_timeout = " + std::to_string(timeout_ms));
    }
    
    static DbResult SetPostgreSQLDeadlockTimeout(DbConnection* conn, int timeout_ms) {
        return conn->Execute(
            "SET deadlock_timeout = " + std::to_string(timeout_ms));
    }
    
    // SpatiaLite
    static DbResult SetSQLiteBusyTimeout(DbConnection* conn, int timeout_ms) {
        return conn->Execute(
            "PRAGMA busy_timeout = " + std::to_string(timeout_ms));
    }
};
```

### 14.4 事务最佳实践

#### 14.4.1 事务使用建议

```cpp
/**
 * @brief 事务使用最佳实践
 */
namespace TransactionBestPractices {

// 1. 短事务优先
void ShortTransactionExample(DbConnection* conn) {
    // 正确: 事务内只做必要的操作
    conn->BeginTransaction();
    conn->InsertGeometry(...);  // 快速插入
    conn->Commit();             // 立即提交
    
    // 错误: 事务内做耗时操作
    // conn->BeginTransaction();
    // ... 长时间计算 ...
    // conn->InsertGeometry(...);
    // conn->Commit();
}

// 2. 批量操作使用单事务
void BatchInsertExample(DbConnection* conn) {
    conn->BeginTransaction();
    
    for (int i = 0; i < 10000; ++i) {
        conn->InsertGeometry(...);
        
        // 每1000行检查一次
        if (i % 1000 == 0 && i > 0) {
            // 可选: 中间提交减少锁持有时间
            // conn->Commit();
            // conn->BeginTransaction();
        }
    }
    
    conn->Commit();
}

// 3. 设置合理的超时
void TimeoutExample(DbConnection* conn) {
    TransactionOptions options;
    options.isolation_level = IsolationLevel::kReadCommitted;
    options.timeout_ms = 30000;  // 30秒超时
    
    conn->BeginTransaction(options);
    // ...
    conn->Commit();
}

// 4. 使用RAII管理事务
class ScopedTransaction {
public:
    explicit ScopedTransaction(DbConnection* conn, 
                               const TransactionOptions& options = {})
        : conn_(conn), committed_(false) {
        conn_->BeginTransaction(options);
    }
    
    ~ScopedTransaction() {
        if (!committed_) {
            conn_->Rollback();  // 异常安全
        }
    }
    
    DbResult Commit() {
        DbResult result = conn_->Commit();
        if (result == DbResult::kSuccess) {
            committed_ = true;
        }
        return result;
    }
    
private:
    DbConnection* conn_;
    bool committed_;
};

} // namespace TransactionBestPractices
```

---

## 15. 空间索引管理策略

### 15.1 索引类型和选择

#### 15.1.1 索引类型定义

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 空间索引类型
 */
enum class SpatialIndexType {
    kGiST,      // Generalized Search Tree (PostgreSQL)
    kBRIN,      // Block Range Index (PostgreSQL, 适合大表)
    kRTree,     // R-Tree (SpatiaLite)
    kQBKD,      // Quadtree (自定义)
    kHash       // 空间哈希 (自定义)
};

/**
 * @brief 索引创建选项
 */
struct SpatialIndexOptions {
    SpatialIndexType type = SpatialIndexType::kGiST;
    bool fillfactor = 90;           // 填充因子 (PostgreSQL)
    int pages_per_range = 128;      // BRIN参数
    bool concurrent = false;        // 并发创建 (不阻塞写入)
    std::string tablespace;         // 表空间
};

} // namespace DB
} // namespace OGC
```

#### 15.1.2 索引选择指南

```cpp
/**
 * @brief 空间索引选择指南
 */
class SpatialIndexSelector {
public:
    /**
     * @brief 根据数据特征推荐索引类型
     */
    struct DataCharacteristics {
        size_t row_count;           // 数据行数
        double avg_geom_size;       // 平均几何对象大小
        bool is_time_series;        // 是否时间序列数据
        bool frequent_updates;      // 是否频繁更新
        double query_selectivity;   // 查询选择性 (0-1)
    };
    
    static SpatialIndexType RecommendIndex(
        const DataCharacteristics& chars,
        DatabaseType db_type
    ) {
        // PostGIS索引选择
        if (db_type == DatabaseType::kPostGIS) {
            // 大表 + 低更新频率 + 时间序列 → BRIN
            if (chars.row_count > 10000000 && 
                !chars.frequent_updates &&
                chars.is_time_series) {
                return SpatialIndexType::kBRIN;
            }
            // 默认 → GiST
            return SpatialIndexType::kGiST;
        }
        
        // SpatiaLite → R-Tree
        if (db_type == DatabaseType::kSpatiaLite) {
            return SpatialIndexType::kRTree;
        }
        
        return SpatialIndexType::kGiST;
    }
    
    /**
     * @brief 获取索引创建最佳时机建议
     */
    static std::string GetIndexTimingAdvice(size_t row_count) {
        if (row_count < 10000) {
            return "数据量小，可在导入前创建索引";
        } else if (row_count < 1000000) {
            return "中等数据量，建议导入后创建索引";
        } else {
            return "大数据量，强烈建议导入后创建索引，并使用CONCURRENT选项";
        }
    }
};
```

### 15.2 索引创建和管理接口

#### 15.2.1 索引创建接口扩展

```cpp
/**
 * @brief 扩展的DbConnection接口 - 索引管理
 */
class SpatialIndexManager {
public:
    /**
     * @brief 创建空间索引
     * @param table_name 表名
     * @param geom_column 几何列名
     * @param index_name 索引名 (空则自动生成)
     * @param options 索引选项
     * @return 操作结果
     */
    virtual DbResult CreateSpatialIndex(
        const std::string& table_name,
        const std::string& geom_column,
        const std::string& index_name = "",
        const SpatialIndexOptions& options = {}
    ) = 0;
    
    /**
     * @brief 并发创建索引 (不阻塞写入)
     * 
     * PostgreSQL: CREATE INDEX CONCURRENTLY
     * SpatiaLite: 不支持，回退到普通创建
     */
    virtual DbResult CreateSpatialIndexConcurrent(
        const std::string& table_name,
        const std::string& geom_column,
        const std::string& index_name = ""
    ) = 0;
    
    /**
     * @brief 删除空间索引
     */
    virtual DbResult DropSpatialIndex(
        const std::string& index_name
    ) = 0;
    
    /**
     * @brief 重建索引
     */
    virtual DbResult RebuildSpatialIndex(
        const std::string& index_name
    ) = 0;
    
    /**
     * @brief 分析表统计信息
     * 
     * 更新索引统计信息，优化查询计划
     */
    virtual DbResult AnalyzeTable(
        const std::string& table_name
    ) = 0;
    
    /**
     * @brief 检查索引是否存在
     */
    virtual bool SpatialIndexExists(
        const std::string& table_name,
        const std::string& geom_column
    ) const = 0;
    
    /**
     * @brief 获取索引信息
     */
    struct IndexInfo {
        std::string name;
        std::string table_name;
        std::string column_name;
        SpatialIndexType type;
        size_t size_bytes;
        bool is_valid;
        time_t created_time;
    };
    
    virtual std::vector<IndexInfo> GetSpatialIndexes(
        const std::string& table_name = ""
    ) const = 0;
};
```

#### 15.2.2 PostGIS索引管理实现

```cpp
/**
 * @brief PostGIS索引管理实现
 */
class PostGISIndexManager : public SpatialIndexManager {
public:
    DbResult CreateSpatialIndex(
        const std::string& table_name,
        const std::string& geom_column,
        const std::string& index_name,
        const SpatialIndexOptions& options
    ) override {
        std::string idx_name = index_name.empty() 
            ? "idx_" + table_name + "_" + geom_column 
            : index_name;
        
        std::ostringstream sql;
        sql << "CREATE INDEX " << idx_name
            << " ON " << table_name;
        
        switch (options.type) {
            case SpatialIndexType::kGiST:
                sql << " USING GIST (" << geom_column << ")";
                break;
            case SpatialIndexType::kBRIN:
                sql << " USING BRIN (" << geom_column << ")"
                    << " WITH (pages_per_range = " 
                    << options.pages_per_range << ")";
                break;
            default:
                return DbResult::kNotSupported;
        }
        
        if (!options.tablespace.empty()) {
            sql << " TABLESPACE " << options.tablespace;
        }
        
        return connection_->Execute(sql.str());
    }
    
    DbResult CreateSpatialIndexConcurrent(
        const std::string& table_name,
        const std::string& geom_column,
        const std::string& index_name
    ) override {
        std::string idx_name = index_name.empty() 
            ? "idx_" + table_name + "_" + geom_column 
            : index_name;
        
        std::ostringstream sql;
        sql << "CREATE INDEX CONCURRENTLY " << idx_name
            << " ON " << table_name
            << " USING GIST (" << geom_column << ")";
        
        return connection_->Execute(sql.str());
    }
    
    DbResult RebuildSpatialIndex(const std::string& index_name) override {
        return connection_->Execute("REINDEX INDEX " + index_name);
    }
    
    DbResult AnalyzeTable(const std::string& table_name) override {
        return connection_->Execute("ANALYZE " + table_name);
    }
    
private:
    DbConnection* connection_;
};
```

#### 15.2.3 SpatiaLite索引管理实现

```cpp
/**
 * @brief SpatiaLite索引管理实现
 */
class SpatiaLiteIndexManager : public SpatialIndexManager {
public:
    DbResult CreateSpatialIndex(
        const std::string& table_name,
        const std::string& geom_column,
        const std::string& index_name,
        const SpatialIndexOptions& options
    ) override {
        // SpatiaLite只支持R-Tree
        std::ostringstream sql;
        sql << "SELECT CreateSpatialIndex('" << table_name 
            << "', '" << geom_column << "')";
        
        return connection_->Execute(sql.str());
    }
    
    DbResult CreateSpatialIndexConcurrent(
        const std::string& table_name,
        const std::string& geom_column,
        const std::string& index_name
    ) override {
        // SpatiaLite不支持并发创建，回退到普通创建
        return CreateSpatialIndex(table_name, geom_column, index_name, {});
    }
    
    DbResult RebuildSpatialIndex(const std::string& index_name) override {
        // SpatiaLite需要删除重建
        // 解析索引名获取表名和列名
        return connection_->Execute(
            "SELECT DisableSpatialIndex('table', 'column');"
            "SELECT CreateSpatialIndex('table', 'column');");
    }
    
    DbResult AnalyzeTable(const std::string& table_name) override {
        return connection_->Execute("ANALYZE " + table_name);
    }
    
private:
    DbConnection* connection_;
};
```

### 15.3 索引维护策略

#### 15.3.1 索引健康检查

```cpp
/**
 * @brief 索引健康检查
 */
class SpatialIndexHealthChecker {
public:
    struct HealthReport {
        std::string index_name;
        bool is_valid;
        double bloat_ratio;      // 膨胀率
        size_t dead_tuples;      // 死元组数
        size_t index_size;       // 索引大小
        std::string recommendation;
    };
    
    /**
     * @brief 检查索引健康状态
     */
    std::vector<HealthReport> CheckIndexHealth(
        DbConnection* conn,
        const std::string& table_name
    );
    
    /**
     * @brief 检查是否需要重建索引
     */
    bool NeedsRebuild(const HealthReport& report) const {
        // 膨胀率超过30%建议重建
        return report.bloat_ratio > 0.3 || !report.is_valid;
    }
    
    /**
     * @brief 自动维护索引
     */
    void AutoMaintain(DbConnection* conn, const std::string& table_name) {
        auto reports = CheckIndexHealth(conn, table_name);
        
        for (const auto& report : reports) {
            if (NeedsRebuild(report)) {
                conn->Execute("REINDEX INDEX " + report.index_name);
            }
        }
    }
};
```

#### 15.3.2 索引维护调度

```cpp
/**
 * @brief 索引维护调度器
 */
class IndexMaintenanceScheduler {
public:
    struct MaintenanceTask {
        std::string table_name;
        std::string index_name;
        std::string task_type;  // "analyze", "rebuild", "check"
        std::string schedule;   // cron表达式
        bool enabled = true;
    };
    
    void AddTask(const MaintenanceTask& task);
    void RemoveTask(const std::string& task_id);
    void EnableTask(const std::string& task_id, bool enabled);
    
    /**
     * @brief 执行到期任务
     */
    void RunPendingTasks(DbConnection* conn);
    
    /**
     * @brief 获取任务执行历史
     */
    struct TaskHistory {
        std::string task_id;
        time_t executed_at;
        bool success;
        std::string message;
    };
    
    std::vector<TaskHistory> GetTaskHistory(const std::string& task_id = "");
    
private:
    std::map<std::string, MaintenanceTask> tasks_;
    std::vector<TaskHistory> history_;
};
```

---

## 16. WKB转换器完善设计

### 16.1 完整几何类型支持

#### 16.1.1 几何类型映射表

```cpp
namespace OGC {
namespace DB {

/**
 * @brief WKB几何类型码完整定义
 */
namespace WkbTypeCodes {
    // OGC标准类型
    constexpr uint32_t kPoint = 1;
    constexpr uint32_t kLineString = 2;
    constexpr uint32_t kPolygon = 3;
    constexpr uint32_t kMultiPoint = 4;
    constexpr uint32_t kMultiLineString = 5;
    constexpr uint32_t kMultiPolygon = 6;
    constexpr uint32_t kGeometryCollection = 7;
    
    // OGC扩展类型 (ISO 13249-3)
    constexpr uint32_t kCircularString = 8;
    constexpr uint32_t kCompoundCurve = 9;
    constexpr uint32_t kCurvePolygon = 10;
    constexpr uint32_t kMultiCurve = 11;
    constexpr uint32_t kMultiSurface = 12;
    constexpr uint32_t kPolyhedralSurface = 15;
    constexpr uint32_t kTIN = 16;
    constexpr uint32_t kTriangle = 17;
    
    // PostGIS扩展标志位
    constexpr uint32_t kHasZ = 0x80000000;
    constexpr uint32_t kHasM = 0x40000000;
    constexpr uint32_t kHasSRID = 0x20000000;
    
    // 辅助函数
    inline uint32_t BaseType(uint32_t type_code) {
        return type_code & 0x0FFFFFFF;
    }
    
    inline bool HasZ(uint32_t type_code) {
        return (type_code & kHasZ) != 0;
    }
    
    inline bool HasM(uint32_t type_code) {
        return (type_code & kHasM) != 0;
    }
    
    inline bool HasSRID(uint32_t type_code) {
        return (type_code & kHasSRID) != 0;
    }
}

} // namespace DB
} // namespace OGC
```

#### 16.1.2 完整WKB转换器实现

```cpp
/**
 * @brief 完整WKB转换器
 * 
 * 支持所有OGC几何类型和PostGIS扩展
 */
class WkbConverter {
public:
    // ========== 几何对象转WKB ==========
    
    /**
     * @brief 将几何对象转换为WKB
     * @param geometry 几何对象
     * @param wkb 输出WKB字节数组
     * @param format 输出格式
     * @return 操作结果
     */
    enum class WkbFormat {
        kWkb,      // 标准WKB
        kEwkb,     // PostGIS EWKB (含SRID)
        kHexWkb    // 十六进制WKB
    };
    
    static DbResult GeometryToWkb(
        const CNGeometry* geometry,
        std::vector<uint8_t>& wkb,
        WkbFormat format = WkbFormat::kEwkb
    );
    
    /**
     * @brief WKB转几何对象
     */
    static DbResult WkbToGeometry(
        const std::vector<uint8_t>& wkb,
        CNGeometryPtr& geometry
    );
    
    // ========== 特定类型转换 ==========
    
    static DbResult PointToWkb(const CNPoint* geom, std::vector<uint8_t>& wkb);
    static DbResult LineStringToWkb(const CNLineString* geom, std::vector<uint8_t>& wkb);
    static DbResult PolygonToWkb(const CNPolygon* geom, std::vector<uint8_t>& wkb);
    static DbResult MultiPointToWkb(const CNMultiPoint* geom, std::vector<uint8_t>& wkb);
    static DbResult MultiLineStringToWkb(const CNMultiLineString* geom, std::vector<uint8_t>& wkb);
    static DbResult MultiPolygonToWkb(const CNMultiPolygon* geom, std::vector<uint8_t>& wkb);
    static DbResult GeometryCollectionToWkb(const CNGeometryCollection* geom, std::vector<uint8_t>& wkb);
    
    // 曲线类型 (如果支持)
    static DbResult CircularStringToWkb(const CNCircularString* geom, std::vector<uint8_t>& wkb);
    static DbResult CompoundCurveToWkb(const CNCompoundCurve* geom, std::vector<uint8_t>& wkb);
    
    // ========== 有效性检查 ==========
    
    /**
     * @brief 检查几何对象是否可序列化
     */
    static bool CanSerialize(const CNGeometry* geometry);
    
    /**
     * @brief 验证WKB格式
     */
    struct WkbValidationResult {
        bool is_valid;
        std::string error_message;
        uint32_t geometry_type;
        int srid;
        bool has_z;
        bool has_m;
    };
    
    static WkbValidationResult ValidateWkb(const std::vector<uint8_t>& wkb);
    
    // ========== 错误处理 ==========
    
    /**
     * @brief 处理无效几何对象
     */
    enum class InvalidGeometryPolicy {
        kReject,     // 拒绝并返回错误
        kRepair,     // 尝试修复
        kNull        // 返回空几何
    };
    
    static void SetInvalidGeometryPolicy(InvalidGeometryPolicy policy);
    
private:
    static InvalidGeometryPolicy invalid_geometry_policy_;
    
    // 内部辅助方法
    static void WriteHeader(std::vector<uint8_t>& wkb, 
                            uint32_t type_code, int srid,
                            bool little_endian = true);
    static bool ReadHeader(const uint8_t*& data, 
                          uint32_t& type_code, int& srid,
                          bool& little_endian);
};
```

### 16.2 错误处理和恢复

#### 16.2.1 WKB解析错误处理

```cpp
/**
 * @brief WKB解析错误类型
 */
enum class WkbParseError {
    kSuccess = 0,
    kInvalidEndianness = 1,
    kInvalidGeometryType = 2,
    kInvalidCoordinateCount = 3,
    kInvalidRingCount = 4,
    kInsufficientData = 5,
    kInvalidSRID = 6,
    kUnsupportedType = 7,
    kMalformedData = 8
};

/**
 * @brief WKB解析异常
 */
class WkbParseException : public std::runtime_error {
public:
    explicit WkbParseException(WkbParseError error, const std::string& message)
        : std::runtime_error(message), error_(error) {}
    
    WkbParseError GetError() const noexcept { return error_; }
    
private:
    WkbParseError error_;
};

/**
 * @brief 安全的WKB解析器
 */
class SafeWkbParser {
public:
    /**
     * @brief 安全解析WKB
     * @param wkb WKB数据
     * @param geometry 输出几何对象
     * @param error 输出错误信息
     * @return 是否成功
     */
    static bool TryParse(
        const std::vector<uint8_t>& wkb,
        CNGeometryPtr& geometry,
        WkbParseError& error
    ) noexcept;
    
    /**
     * @brief 解析并尝试修复
     */
    static DbResult ParseWithRepair(
        const std::vector<uint8_t>& wkb,
        CNGeometryPtr& geometry,
        std::string& repair_log
    );
};
```

### 16.3 性能优化

#### 16.3.1 WKB缓冲区复用

```cpp
/**
 * @brief WKB缓冲区池
 * 
 * 复用内存缓冲区，减少分配开销
 */
class WkbBufferPool {
public:
    static WkbBufferPool& Instance() {
        static WkbBufferPool instance;
        return instance;
    }
    
    /**
     * @brief 获取缓冲区
     */
    std::vector<uint8_t>* Acquire(size_t min_size = 1024) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (auto& buf : buffers_) {
            if (buf.capacity() >= min_size && !buf_in_use_[&buf]) {
                buf_in_use_[&buf] = true;
                buf.clear();
                return &buf;
            }
        }
        
        // 创建新缓冲区
        buffers_.emplace_back();
        buffers_.back().reserve(min_size);
        buf_in_use_[&buffers_.back()] = true;
        return &buffers_.back();
    }
    
    /**
     * @brief 归还缓冲区
     */
    void Release(std::vector<uint8_t>* buffer) {
        std::lock_guard<std::mutex> lock(mutex_);
        buf_in_use_[buffer] = false;
    }
    
private:
    WkbBufferPool() = default;
    
    std::vector<std::vector<uint8_t>> buffers_;
    std::map<std::vector<uint8_t>*, bool> buf_in_use_;
    std::mutex mutex_;
};

/**
 * @brief RAII缓冲区守卫
 */
class ScopedWkbBuffer {
public:
    ScopedWkbBuffer() 
        : buffer_(WkbBufferPool::Instance().Acquire()) {}
    ~ScopedWkbBuffer() {
        WkbBufferPool::Instance().Release(buffer_);
    }
    
    std::vector<uint8_t>& Get() { return *buffer_; }
    
private:
    std::vector<uint8_t>* buffer_;
};
```

---

## 17. 坐标参考系转换集成

### 17.1 坐标转换服务接口

#### 17.1.1 坐标转换接口定义

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 坐标参考系信息
 */
struct CoordinateReferenceSystem {
    int srid;               // SRID
    std::string auth_name;  // 权威机构名 (EPSG, ESRI等)
    int auth_code;          // 权威代码
    std::string wkt;        // WKT表示
    std::string proj4;      // PROJ.4字符串
    bool is_geographic;     // 是否地理坐标系
    bool is_projected;      // 是否投影坐标系
};

/**
 * @brief 坐标转换选项
 */
struct TransformOptions {
    bool strict = true;         // 严格模式 (转换失败抛异常)
    double accuracy = 0.0;      // 目标精度 (米)
    std::string area_of_use;    // 使用区域
    bool allow_ballpark = true; // 允许近似转换
};

/**
 * @brief 坐标转换服务接口
 */
class CoordinateTransformService {
public:
    virtual ~CoordinateTransformService() = default;
    
    /**
     * @brief 初始化服务
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief 获取CRS信息
     */
    virtual bool GetCRSInfo(int srid, CoordinateReferenceSystem& crs) = 0;
    
    /**
     * @brief 转换单个坐标
     */
    virtual bool Transform(
        double& x, double& y, double& z,
        int source_srid, int target_srid,
        const TransformOptions& options = {}
    ) = 0;
    
    /**
     * @brief 批量转换坐标
     */
    virtual bool TransformBatch(
        std::vector<double>& x, std::vector<double>& y, std::vector<double>& z,
        int source_srid, int target_srid,
        const TransformOptions& options = {}
    ) = 0;
    
    /**
     * @brief 转换几何对象
     */
    virtual CNGeometryPtr TransformGeometry(
        const CNGeometry* geometry,
        int target_srid,
        const TransformOptions& options = {}
    ) = 0;
    
    /**
     * @brief 检查转换是否可用
     */
    virtual bool CanTransform(int source_srid, int target_srid) = 0;
    
    /**
     * @brief 获取转换精度
     */
    virtual double GetTransformAccuracy(int source_srid, int target_srid) = 0;
};

} // namespace DB
} // namespace OGC
```

#### 17.1.2 PROJ库集成实现

```cpp
/**
 * @brief 基于PROJ的坐标转换实现
 */
class ProjTransformService : public CoordinateTransformService {
public:
    ProjTransformService();
    ~ProjTransformService() override;
    
    bool Initialize() override;
    
    bool GetCRSInfo(int srid, CoordinateReferenceSystem& crs) override {
        // 使用proj_get_crs_info_list_from_database
        PJ* crs = proj_create_from_database(
            context_, "EPSG", std::to_string(srid).c_str(),
            PJ_CATEGORY_CRS, false, nullptr);
        
        if (!crs) return false;
        
        crs.srid = srid;
        crs.auth_name = "EPSG";
        crs.auth_code = srid;
        
        // 获取WKT
        const char* wkt = proj_as_wkt(context_, crs, PJ_WKT2_2019, nullptr);
        if (wkt) crs.wkt = wkt;
        
        // 获取PROJ字符串
        const char* proj_str = proj_as_proj_string(context_, crs, PJ_PROJ_5, nullptr);
        if (proj_str) crs.proj4 = proj_str;
        
        // 判断类型
        PJ_TYPE type = proj_get_type(crs);
        crs.is_geographic = (type == PJ_TYPE_GEOGRAPHIC_2D_CRS || 
                            type == PJ_TYPE_GEOGRAPHIC_3D_CRS);
        crs.is_projected = (type == PJ_TYPE_PROJECTED_CRS);
        
        proj_destroy(crs);
        return true;
    }
    
    bool Transform(
        double& x, double& y, double& z,
        int source_srid, int target_srid,
        const TransformOptions& options
    ) override {
        PJ* transform = GetOrCreateTransform(source_srid, target_srid);
        if (!transform) return false;
        
        PJ_COORD coord;
        coord.xyzt.x = x;
        coord.xyzt.y = y;
        coord.xyzt.z = z;
        coord.xyzt.t = 0;
        
        coord = proj_trans(transform, PJ_FWD, coord);
        
        if (coord.xyzt.x == HUGE_VAL) {
            return false;
        }
        
        x = coord.xyzt.x;
        y = coord.xyzt.y;
        z = coord.xyzt.z;
        return true;
    }
    
    CNGeometryPtr TransformGeometry(
        const CNGeometry* geometry,
        int target_srid,
        const TransformOptions& options
    ) override {
        if (!geometry) return nullptr;
        
        int source_srid = geometry->GetSRID();
        if (source_srid == target_srid) {
            return geometry->Clone();
        }
        
        // 遍历所有坐标并转换
        return TransformGeometryImpl(geometry, source_srid, target_srid, options);
    }
    
private:
    PJ_CONTEXT* context_ = nullptr;
    std::map<std::pair<int, int>, PJ*> transform_cache_;
    std::mutex cache_mutex_;
    
    PJ* GetOrCreateTransform(int source_srid, int target_srid);
    CNGeometryPtr TransformGeometryImpl(
        const CNGeometry* geom, int source_srid, int target_srid,
        const TransformOptions& options);
};
```

### 17.2 空间查询中的坐标转换

#### 17.2.1 自动坐标转换

```cpp
/**
 * @brief 支持坐标转换的空间查询
 */
class SpatialQueryWithTransform {
public:
    /**
     * @brief 执行空间查询，自动处理SRID差异
     */
    static DbResult Execute(
        DbConnection* conn,
        const std::string& table,
        const std::string& geom_column,
        SpatialOperator op,
        const CNGeometry* query_geom,
        std::vector<CNGeometryPtr>& results,
        CoordinateTransformService* transform_service = nullptr
    ) {
        // 1. 获取表的SRID
        int table_srid = GetTableSRID(conn, table, geom_column);
        int query_srid = query_geom->GetSRID();
        
        // 2. 检查SRID是否匹配
        if (query_srid != table_srid && query_srid > 0 && table_srid > 0) {
            if (transform_service && 
                transform_service->CanTransform(query_srid, table_srid)) {
                // 转换查询几何对象
                CNGeometryPtr transformed = transform_service->TransformGeometry(
                    query_geom, table_srid);
                if (transformed) {
                    return conn->SpatialQuery(table, geom_column, op, 
                                             transformed.get(), results);
                }
            }
            
            // 无法转换，返回错误
            return DbResult::kSridMismatch;
        }
        
        // 3. SRID匹配，直接查询
        return conn->SpatialQuery(table, geom_column, op, query_geom, results);
    }
    
private:
    static int GetTableSRID(DbConnection* conn, 
                           const std::string& table,
                           const std::string& geom_column);
};
```

### 17.3 数据库SRID管理

#### 17.3.1 SRID查询和设置

```cpp
/**
 * @brief 数据库SRID管理接口
 */
class SRIDManager {
public:
    /**
     * @brief 获取几何列的SRID
     */
    static int GetGeometryColumnSRID(
        DbConnection* conn,
        const std::string& table,
        const std::string& geom_column
    );
    
    /**
     * @brief 设置几何列的SRID
     */
    static DbResult SetGeometryColumnSRID(
        DbConnection* conn,
        const std::string& table,
        const std::string& geom_column,
        int srid
    );
    
    /**
     * @brief 查询数据库支持的SRID列表
     */
    static std::vector<int> GetSupportedSRIDs(DbConnection* conn);
    
    /**
     * @brief 检查SRID是否有效
     */
    static bool IsValidSRID(DbConnection* conn, int srid);
    
    /**
     * @brief 插入自定义SRID
     */
    static DbResult InsertCustomSRID(
        DbConnection* conn,
        int srid,
        const std::string& wkt,
        const std::string& proj4
    );
};

// PostGIS实现
class PostGISSRIDManager : public SRIDManager {
public:
    static int GetGeometryColumnSRID(
        DbConnection* conn,
        const std::string& table,
        const std::string& geom_column
    ) {
        std::ostringstream sql;
        sql << "SELECT Find_SRID('', '" << table << "', '" << geom_column << "')";
        
        std::unique_ptr<DbResultSet> result;
        conn->ExecuteQuery(sql.str(), result);
        
        if (result && result->Next()) {
            return result->GetInt(0);
        }
        return -1;
    }
    
    static std::vector<int> GetSupportedSRIDs(DbConnection* conn) {
        std::vector<int> srids;
        
        std::unique_ptr<DbResultSet> result;
        conn->ExecuteQuery("SELECT srid FROM spatial_ref_sys ORDER BY srid", result);
        
        while (result && result->Next()) {
            srids.push_back(result->GetInt(0));
        }
        
        return srids;
    }
};
```

---

## 18. 性能监控和诊断

### 18.1 性能统计接口

#### 18.1.1 查询性能统计

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 查询性能统计信息
 */
struct QueryStats {
    std::string query_id;
    std::string sql;
    int64_t execution_time_ms;
    int64_t rows_affected;
    int64_t rows_fetched;
    size_t bytes_transferred;
    bool used_index;
    std::string index_name;
    time_t executed_at;
};

/**
 * @brief 连接池统计信息
 */
struct ConnectionPoolStats {
    size_t total_connections;
    size_t available_connections;
    size_t active_connections;
    size_t waiting_threads;
    int64_t total_acquires;
    int64_t total_releases;
    int64_t acquire_timeouts;
    int64_t avg_acquire_time_ms;
};

/**
 * @brief 性能监控接口
 */
class PerformanceMonitor {
public:
    virtual ~PerformanceMonitor() = default;
    
    // ========== 查询统计 ==========
    
    /**
     * @brief 记录查询执行
     */
    virtual void RecordQuery(const QueryStats& stats) = 0;
    
    /**
     * @brief 获取慢查询列表
     */
    virtual std::vector<QueryStats> GetSlowQueries(
        int64_t threshold_ms = 1000,
        size_t limit = 100
    ) = 0;
    
    /**
     * @brief 获取查询统计摘要
     */
    virtual std::map<std::string, QueryStats> GetQuerySummary() = 0;
    
    // ========== 连接池监控 ==========
    
    /**
     * @brief 获取连接池统计
     */
    virtual ConnectionPoolStats GetPoolStats() = 0;
    
    // ========== 性能指标导出 ==========
    
    /**
     * @brief 导出Prometheus格式指标
     */
    virtual std::string ExportPrometheusMetrics() = 0;
    
    /**
     * @brief 重置统计
     */
    virtual void Reset() = 0;
};

} // namespace DB
} // namespace OGC
```

#### 18.1.2 性能监控实现

```cpp
/**
 * @brief 默认性能监控实现
 */
class DefaultPerformanceMonitor : public PerformanceMonitor {
public:
    void RecordQuery(const QueryStats& stats) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        query_history_.push_back(stats);
        
        // 检查是否为慢查询
        if (stats.execution_time_ms > slow_query_threshold_ms_) {
            slow_queries_.push_back(stats);
        }
        
        // 更新摘要
        std::string key = GetQueryKey(stats.sql);
        if (query_summary_.find(key) == query_summary_.end()) {
            query_summary_[key] = stats;
        } else {
            // 更新平均值
            auto& existing = query_summary_[key];
            existing.execution_time_ms = 
                (existing.execution_time_ms + stats.execution_time_ms) / 2;
        }
    }
    
    std::vector<QueryStats> GetSlowQueries(
        int64_t threshold_ms, size_t limit
    ) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<QueryStats> result;
        for (const auto& stats : slow_queries_) {
            if (stats.execution_time_ms >= threshold_ms) {
                result.push_back(stats);
                if (result.size() >= limit) break;
            }
        }
        return result;
    }
    
    std::string ExportPrometheusMetrics() override {
        std::ostringstream oss;
        
        // 查询计数
        oss << "# HELP db_queries_total Total number of queries\n";
        oss << "# TYPE db_queries_total counter\n";
        oss << "db_queries_total " << query_history_.size() << "\n";
        
        // 慢查询计数
        oss << "# HELP db_slow_queries_total Total number of slow queries\n";
        oss << "# TYPE db_slow_queries_total counter\n";
        oss << "db_slow_queries_total " << slow_queries_.size() << "\n";
        
        // 连接池指标
        auto pool_stats = GetPoolStats();
        oss << "# HELP db_pool_connections Database connection pool size\n";
        oss << "# TYPE db_pool_connections gauge\n";
        oss << "db_pool_connections{state=\"total\"} " << pool_stats.total_connections << "\n";
        oss << "db_pool_connections{state=\"available\"} " << pool_stats.available_connections << "\n";
        oss << "db_pool_connections{state=\"active\"} " << pool_stats.active_connections << "\n";
        
        return oss.str();
    }
    
private:
    std::mutex mutex_;
    std::vector<QueryStats> query_history_;
    std::vector<QueryStats> slow_queries_;
    std::map<std::string, QueryStats> query_summary_;
    int64_t slow_query_threshold_ms_ = 1000;
    
    std::string GetQueryKey(const std::string& sql);
};
```

### 18.2 慢查询日志

#### 18.2.1 慢查询日志配置

```cpp
/**
 * @brief 慢查询日志配置
 */
struct SlowQueryLogConfig {
    bool enabled = true;
    int64_t threshold_ms = 1000;     // 慢查询阈值
    bool log_explain = true;         // 是否记录执行计划
    bool log_stack_trace = false;    // 是否记录调用栈
    std::string log_file;            // 日志文件路径
    size_t max_file_size_mb = 100;   // 最大文件大小
    int max_files = 10;              // 最大文件数
};

/**
 * @brief 慢查询日志记录器
 */
class SlowQueryLogger {
public:
    explicit SlowQueryLogger(const SlowQueryLogConfig& config);
    
    /**
     * @brief 记录慢查询
     */
    void LogSlowQuery(const QueryStats& stats);
    
    /**
     * @brief 记录执行计划
     */
    void LogExplain(DbConnection* conn, const std::string& sql);
    
private:
    SlowQueryLogConfig config_;
    std::ofstream log_file_;
    std::mutex mutex_;
    
    void RotateLogIfNeeded();
};
```

### 18.3 连接池健康监控

#### 18.3.1 健康检查实现

```cpp
/**
 * @brief 连接池健康检查器
 */
class ConnectionPoolHealthChecker {
public:
    /**
     * @brief 启动后台健康检查
     */
    void Start(DbConnectionPool* pool, int check_interval_ms = 30000) {
        running_ = true;
        check_thread_ = std::thread([this, pool, check_interval_ms]() {
            while (running_) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(check_interval_ms));
                
                if (!running_) break;
                
                CheckPoolHealth(pool);
            }
        });
    }
    
    /**
     * @brief 停止健康检查
     */
    void Stop() {
        running_ = false;
        if (check_thread_.joinable()) {
            check_thread_.join();
        }
    }
    
private:
    std::atomic<bool> running_{false};
    std::thread check_thread_;
    
    void CheckPoolHealth(DbConnectionPool* pool) {
        // 获取连接并测试
        auto conn = pool->Acquire(1000);
        if (!conn) {
            // 记录警告：无法获取连接
            LogWarning("Connection pool exhausted");
            return;
        }
        
        // Ping测试
        if (!conn->Ping()) {
            // 连接失效，移除
            LogWarning("Invalid connection detected");
            // 不归还连接，让池创建新连接
            return;
        }
        
        // 归还连接
        pool->Release(std::move(conn));
    }
    
    void LogWarning(const std::string& message);
};
```

---

## 19. 角色接口设计

### 19.1 接口隔离原则应用

#### 19.1.1 问题背景

原DbConnection接口包含40+个虚函数，涵盖连接管理、事务管理、SQL执行、几何操作、元数据操作等所有功能，违反接口隔离原则（ISP）。

#### 19.1.2 角色接口拆分

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 连接管理接口 - 核心连接操作
 */
class DbConnectionCore {
public:
    virtual ~DbConnectionCore() = default;
    
    virtual DbResult Connect(const std::string& connection_string) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() const = 0;
    virtual bool Ping() const = 0;
    
    virtual std::string GetDatabaseName() const = 0;
    virtual std::string GetServerVersion() const = 0;
    virtual DatabaseType GetDatabaseType() const = 0;
};

/**
 * @brief 事务管理接口
 */
class DbTransactionManager {
public:
    virtual ~DbTransactionManager() = default;
    
    virtual DbResult BeginTransaction() = 0;
    virtual DbResult BeginTransaction(const TransactionOptions& options) = 0;
    virtual DbResult Commit() = 0;
    virtual DbResult Rollback() = 0;
    virtual bool InTransaction() const = 0;
    
    virtual DbResult SetSavepoint(const std::string& name) = 0;
    virtual DbResult ReleaseSavepoint(const std::string& name) = 0;
    virtual DbResult RollbackToSavepoint(const std::string& name) = 0;
};

/**
 * @brief SQL执行接口
 */
class DbSqlExecutor {
public:
    virtual ~DbSqlExecutor() = default;
    
    virtual DbResult Execute(const std::string& sql) = 0;
    virtual DbResult ExecuteQuery(const std::string& sql,
                                   std::unique_ptr<DbResultSet>& result) = 0;
    virtual DbResult PrepareStatement(const std::string& name,
                                       const std::string& sql,
                                       std::unique_ptr<DbStatement>& stmt) = 0;
    
    virtual int64_t GetLastInsertId() const = 0;
    virtual int64_t GetRowsAffected() const = 0;
};

/**
 * @brief 几何操作接口
 */
class DbGeometryOperations {
public:
    virtual ~DbGeometryOperations() = default;
    
    virtual DbResult InsertGeometry(const std::string& table,
                                     const std::string& geom_column,
                                     const CNGeometry* geometry,
                                     const std::map<std::string, std::string>& attributes,
                                     int64_t& out_id) = 0;
    
    virtual DbResult InsertGeometries(const std::string& table,
                                       const std::string& geom_column,
                                       const std::vector<CNGeometryPtr>& geometries,
                                       const std::vector<std::map<std::string, std::string>>& attributes,
                                       std::vector<int64_t>& out_ids) = 0;
    
    virtual DbResult SpatialQuery(const std::string& table,
                                   const std::string& geom_column,
                                   SpatialOperator op,
                                   const CNGeometry* query_geom,
                                   std::vector<CNGeometryPtr>& results) = 0;
    
    virtual DbResult SpatialQueryWithAttributes(
        const std::string& table,
        const std::string& geom_column,
        SpatialOperator op,
        const CNGeometry* query_geom,
        const std::string& where_clause,
        std::vector<std::pair<CNGeometryPtr, std::map<std::string, std::string>>>& results) = 0;
    
    virtual DbResult UpdateGeometry(const std::string& table,
                                     const std::string& geom_column,
                                     int64_t id,
                                     const CNGeometry* geometry) = 0;
    
    virtual DbResult DeleteGeometry(const std::string& table,
                                     int64_t id) = 0;
};

/**
 * @brief 元数据操作接口
 */
class DbMetadataOperations {
public:
    virtual ~DbMetadataOperations() = default;
    
    virtual std::vector<std::string> GetTables() const = 0;
    virtual std::vector<std::string> GetGeometryColumns() const = 0;
    virtual bool TableExists(const std::string& table_name) const = 0;
    virtual bool GeometryColumnExists(const std::string& table_name,
                                       const std::string& column_name) const = 0;
    
    virtual DbResult GetTableInfo(const std::string& table_name,
                                   TableInfo& info) const = 0;
    virtual DbResult GetGeometryColumnInfo(const std::string& table_name,
                                            const std::string& column_name,
                                            GeometryColumnInfo& info) const = 0;
    
    virtual int GetGeometryColumnSRID(const std::string& table_name,
                                       const std::string& column_name) const = 0;
};

/**
 * @brief 完整数据库连接接口 - 组合所有角色接口
 * 
 * 使用多重继承组合所有角色接口，保持向后兼容
 */
class DbConnection : public DbConnectionCore,
                     public DbTransactionManager,
                     public DbSqlExecutor,
                     public DbGeometryOperations,
                     public DbMetadataOperations {
public:
    virtual ~DbConnection() = default;
    
    /**
     * @brief 检查是否支持特定功能
     */
    virtual bool SupportsFeature(Feature feature) const = 0;
    
    /**
     * @brief 获取连接ID（用于调试）
     */
    virtual uint64_t GetConnectionId() const = 0;
};

/**
 * @brief 只读连接接口
 * 
 * 仅包含查询相关接口，用于只读场景
 */
class DbReadOnlyConnection : public DbConnectionCore,
                              public DbSqlExecutor,
                              public DbGeometryOperations,
                              public DbMetadataOperations {
public:
    virtual ~DbReadOnlyConnection() = default;
};

/**
 * @brief 功能支持枚举
 */
enum class Feature {
    kTransactions,
    kSavepoints,
    kPreparedStatements,
    kCopyFromStdin,
    kSpatialIndexGiST,
    kSpatialIndexBRIN,
    kSpatialIndexRTree,
    kMVCC,
    kExplainAnalyze
};

} // namespace DB
} // namespace OGC
```

### 19.2 接口使用示例

```cpp
void UseRoleInterfaces() {
    std::unique_ptr<DbConnection> conn = CreateConnection(DatabaseType::kPostGIS);
    
    // 使用特定角色接口
    DbConnectionCore* core = conn.get();
    core->Connect("host=localhost dbname=test");
    
    DbTransactionManager* tx_mgr = conn.get();
    tx_mgr->BeginTransaction();
    
    DbGeometryOperations* geom_ops = conn.get();
    geom_ops->InsertGeometry("points", "geom", point.get(), {}, id);
    
    tx_mgr->Commit();
    
    // 只读场景使用只读接口
    DbReadOnlyConnection* read_only = dynamic_cast<DbReadOnlyConnection*>(conn.get());
    if (read_only) {
        std::vector<CNGeometryPtr> results;
        read_only->SpatialQuery("points", "geom", SpatialOperator::kIntersects,
                                query_geom.get(), results);
    }
}
```

---

## 20. 批量插入错误恢复机制

### 20.1 问题分析

COPY命令失败后，部分数据可能已写入数据库，无法回滚。大批量数据导入失败后需要重新开始，生产环境存在数据一致性问题。

### 20.2 分段提交机制

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 批量插入配置选项
 */
struct BulkInsertOptions {
    bool use_transaction = true;        // 是否使用事务包裹
    size_t batch_size = 10000;          // 每批次行数
    bool enable_checkpoint = true;      // 是否启用检查点
    std::string checkpoint_table;       // 检查点表名
    bool continue_on_error = false;     // 遇到错误是否继续
    size_t max_errors = 100;            // 最大错误数
};

/**
 * @brief 批量插入进度信息
 */
struct BulkInsertProgress {
    size_t total_rows;          // 总行数
    size_t processed_rows;      // 已处理行数
    size_t committed_rows;      // 已提交行数
    size_t failed_rows;         // 失败行数
    size_t current_batch;       // 当前批次
    std::string last_error;     // 最后一个错误信息
    bool can_resume;            // 是否可以恢复
};

/**
 * @brief 批量插入结果
 */
struct BulkInsertResult {
    DbResult result;
    size_t total_inserted;
    size_t total_failed;
    std::vector<std::pair<size_t, std::string>> errors;  // 行号 -> 错误信息
    BulkInsertProgress final_progress;
};

/**
 * @brief 增强的PostGIS批量插入器
 */
class PostGISBulkInserterV2 {
public:
    PostGISBulkInserterV2(DbConnection* connection,
                          const std::string& table,
                          const std::string& geom_column,
                          const std::vector<std::string>& attribute_columns,
                          const BulkInsertOptions& options = {});
    
    ~PostGISBulkInserterV2();
    
    /**
     * @brief 添加单行数据
     */
    DbResult AddRow(const CNGeometry* geometry,
                    const std::vector<std::string>& attributes);
    
    /**
     * @brief 批量添加数据
     */
    DbResult AddRows(const std::vector<CNGeometryPtr>& geometries,
                     const std::vector<std::vector<std::string>>& attributes);
    
    /**
     * @brief 提交所有数据
     */
    BulkInsertResult Commit();
    
    /**
     * @brief 获取当前进度
     */
    BulkInsertProgress GetProgress() const;
    
    /**
     * @brief 从检查点恢复
     * @return 恢复成功返回true，无检查点返回false
     */
    bool ResumeFromCheckpoint();
    
    /**
     * @brief 清除检查点
     */
    void ClearCheckpoint();
    
    /**
     * @brief 取消插入
     */
    void Cancel();

private:
    DbConnection* connection_;
    std::string table_;
    std::string geom_column_;
    std::vector<std::string> attribute_columns_;
    BulkInsertOptions options_;
    
    size_t current_batch_count_;
    size_t total_processed_;
    size_t total_failed_;
    std::vector<std::pair<size_t, std::string>> errors_;
    
    bool in_transaction_;
    bool cancelled_;
    
    DbResult FlushBatch();
    DbResult SaveCheckpoint(size_t committed_rows);
    size_t LoadCheckpoint();
    void CreateCheckpointTable();
};

} // namespace DB
} // namespace OGC
```

### 20.3 分段提交实现

```cpp
BulkInsertResult PostGISBulkInserterV2::Commit() {
    BulkInsertResult result;
    
    if (options_.use_transaction && !in_transaction_) {
        DbResult tx_result = connection_->BeginTransaction();
        if (tx_result != DbResult::kSuccess) {
            result.result = tx_result;
            return result;
        }
        in_transaction_ = true;
    }
    
    // 刷新剩余数据
    DbResult flush_result = FlushBatch();
    
    if (options_.use_transaction && in_transaction_) {
        if (flush_result == DbResult::kSuccess) {
            connection_->Commit();
        } else {
            connection_->Rollback();
        }
        in_transaction_ = false;
    }
    
    // 清除检查点
    if (options_.enable_checkpoint) {
        ClearCheckpoint();
    }
    
    result.result = flush_result;
    result.total_inserted = total_processed_ - total_failed_;
    result.total_failed = total_failed_;
    result.errors = std::move(errors_);
    result.final_progress = GetProgress();
    
    return result;
}

DbResult PostGISBulkInserterV2::FlushBatch() {
    if (current_batch_count_ == 0) {
        return DbResult::kSuccess;
    }
    
    // 执行COPY
    DbResult result = ExecuteCopy();
    
    if (result == DbResult::kSuccess) {
        total_processed_ += current_batch_count_;
        
        // 保存检查点
        if (options_.enable_checkpoint) {
            SaveCheckpoint(total_processed_);
        }
    }
    
    current_batch_count_ = 0;
    return result;
}

bool PostGISBulkInserterV2::ResumeFromCheckpoint() {
    size_t checkpoint = LoadCheckpoint();
    if (checkpoint == 0) {
        return false;
    }
    
    total_processed_ = checkpoint;
    return true;
}
```

### 20.4 使用示例

```cpp
void BulkInsertWithRecovery(DbConnection* conn) {
    BulkInsertOptions options;
    options.use_transaction = true;
    options.batch_size = 50000;
    options.enable_checkpoint = true;
    
    PostGISBulkInserterV2 inserter(conn, "large_table", "geom", {"name", "value"}, options);
    
    // 尝试从检查点恢复
    if (inserter.ResumeFromCheckpoint()) {
        std::cout << "Resumed from checkpoint" << std::endl;
    }
    
    // 添加数据
    for (size_t i = 0; i < 1000000; ++i) {
        auto geom = CreatePoint(i, i);
        inserter.AddRow(geom.get(), {"name_" + std::to_string(i), std::to_string(i)});
        
        // 定期检查进度
        if (i % 100000 == 0) {
            auto progress = inserter.GetProgress();
            std::cout << "Progress: " << progress.processed_rows << "/" << progress.total_rows << std::endl;
        }
    }
    
    // 提交
    auto result = inserter.Commit();
    std::cout << "Inserted: " << result.total_inserted << std::endl;
    std::cout << "Failed: " << result.total_failed << std::endl;
}
```

---

## 21. 连接池高级特性

### 21.1 连接池耗尽处理策略

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 连接池耗尽处理策略
 */
enum class PoolExhaustedPolicy {
    kWait,          // 等待直到有可用连接
    kFailFast,      // 立即返回失败
    kGrow,          // 在max_size范围内动态扩容
    kCallback       // 调用用户回调
};

/**
 * @brief 连接池配置
 */
struct ConnectionPoolConfig {
    size_t min_size = 5;
    size_t max_size = 20;
    int acquire_timeout_ms = 30000;
    int idle_timeout_ms = 600000;       // 空闲连接超时
    int validation_interval_ms = 30000; // 连接验证间隔
    PoolExhaustedPolicy exhausted_policy = PoolExhaustedPolicy::kWait;
    bool enable_metrics = true;
};

/**
 * @brief 连接池状态
 */
struct PoolState {
    size_t total_connections;
    size_t available_connections;
    size_t active_connections;
    size_t waiting_threads;
    bool is_exhausted;
    double utilization_rate;
};

/**
 * @brief 连接池事件回调
 */
class ConnectionPoolCallback {
public:
    virtual ~ConnectionPoolCallback() = default;
    
    virtual void OnPoolExhausted(const PoolState& state) = 0;
    virtual void OnConnectionCreated(uint64_t conn_id) = 0;
    virtual void OnConnectionDestroyed(uint64_t conn_id) = 0;
    virtual void OnConnectionAcquired(uint64_t conn_id, int64_t wait_ms) = 0;
    virtual void OnConnectionReleased(uint64_t conn_id) = 0;
    virtual void OnConnectionValidationFailed(uint64_t conn_id) = 0;
};

/**
 * @brief 增强的连接池实现
 */
class DbConnectionPoolV2 {
public:
    DbConnectionPoolV2(DatabaseType db_type,
                       const std::string& connection_string,
                       const ConnectionPoolConfig& config);
    
    ~DbConnectionPoolV2();
    
    /**
     * @brief 获取连接
     * @return 连接指针，失败返回nullptr
     */
    std::unique_ptr<DbConnection> Acquire(int timeout_ms = -1);
    
    /**
     * @brief 归还连接
     */
    void Release(std::unique_ptr<DbConnection> connection);
    
    /**
     * @brief 尝试获取连接（非阻塞）
     */
    std::unique_ptr<DbConnection> TryAcquire();
    
    /**
     * @brief 获取连接池状态
     */
    PoolState GetState() const;
    
    /**
     * @brief 设置事件回调
     */
    void SetCallback(std::shared_ptr<ConnectionPoolCallback> callback);
    
    /**
     * @brief 动态调整连接池大小
     */
    DbResult Resize(size_t new_min_size, size_t new_max_size);
    
    /**
     * @brief 关闭所有连接
     */
    void CloseAll();
    
    /**
     * @brief 关闭空闲连接
     */
    void CloseIdleConnections();
    
    /**
     * @brief 启动后台维护线程
     */
    void StartMaintenance();
    
    /**
     * @brief 停止后台维护线程
     */
    void StopMaintenance();

private:
    DatabaseType db_type_;
    std::string connection_string_;
    ConnectionPoolConfig config_;
    
    std::queue<std::unique_ptr<DbConnection>> available_;
    std::set<DbConnection*> active_;
    std::map<DbConnection*, std::chrono::steady_clock::time_point> last_used_;
    
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    
    std::shared_ptr<ConnectionPoolCallback> callback_;
    std::thread maintenance_thread_;
    std::atomic<bool> running_{false};
    
    void MaintenanceLoop();
    void ValidateConnections();
    void RemoveIdleConnections();
    void GrowIfNeeded();
    std::unique_ptr<DbConnection> CreateConnection();
};

} // namespace DB
} // namespace OGC
```

### 21.2 连接池耗尽处理实现

```cpp
std::unique_ptr<DbConnection> DbConnectionPoolV2::Acquire(int timeout_ms) {
    std::unique_lock<std::mutex> lock(mutex_);
    
    auto deadline = std::chrono::steady_clock::now() + 
                    std::chrono::milliseconds(timeout_ms < 0 ? 
                        config_.acquire_timeout_ms : timeout_ms);
    
    while (available_.empty() && active_.size() >= config_.max_size) {
        // 连接池耗尽
        switch (config_.exhausted_policy) {
            case PoolExhaustedPolicy::kFailFast:
                if (callback_) {
                    callback_->OnPoolExhausted(GetState());
                }
                return nullptr;
                
            case PoolExhaustedPolicy::kGrow:
                if (active_.size() < config_.max_size) {
                    // 动态扩容
                    auto conn = CreateConnection();
                    if (conn) {
                        active_.insert(conn.get());
                        if (callback_) {
                            callback_->OnConnectionAcquired(conn->GetConnectionId(), 0);
                        }
                        return conn;
                    }
                }
                // 无法扩容，继续等待
                break;
                
            case PoolExhaustedPolicy::kCallback:
                if (callback_) {
                    callback_->OnPoolExhausted(GetState());
                }
                // 继续等待
                break;
                
            case PoolExhaustedPolicy::kWait:
            default:
                break;
        }
        
        // 等待连接可用
        if (cv_.wait_until(lock, deadline) == std::cv_status::timeout) {
            if (callback_) {
                callback_->OnPoolExhausted(GetState());
            }
            return nullptr;
        }
    }
    
    // 获取可用连接
    if (!available_.empty()) {
        auto conn = std::move(available_.front());
        available_.pop();
        active_.insert(conn.get());
        last_used_[conn.get()] = std::chrono::steady_clock::now();
        
        if (callback_) {
            callback_->OnConnectionAcquired(conn->GetConnectionId(), 0);
        }
        
        return conn;
    }
    
    // 创建新连接
    auto conn = CreateConnection();
    if (conn) {
        active_.insert(conn.get());
        if (callback_) {
            callback_->OnConnectionAcquired(conn->GetConnectionId(), 0);
        }
    }
    
    return conn;
}
```

---

## 22. WKB转换器SIMD优化

### 22.1 SIMD优化路径

```cpp
namespace OGC {
namespace DB {

/**
 * @brief CPU特性检测
 */
class CpuFeatures {
public:
    static bool HasSSE2() { return has_sse2_; }
    static bool HasSSE41() { return has_sse41_; }
    static bool HasAVX() { return has_avx_; }
    static bool HasAVX2() { return has_avx2_; }
    
    static void Detect() {
#ifdef __x86_64__
        int regs[4];
        __cpuid(0, regs[0], regs[1], regs[2], regs[3]);
        
        if (regs[0] >= 1) {
            __cpuid(1, regs[0], regs[1], regs[2], regs[3]);
            has_sse2_ = (regs[3] & (1 << 26)) != 0;
            has_sse41_ = (regs[2] & (1 << 19)) != 0;
        }
        
        if (regs[0] >= 7) {
            __cpuid_count(7, 0, regs[0], regs[1], regs[2], regs[3]);
            has_avx2_ = (regs[1] & (1 << 5)) != 0;
        }
#endif
    }
    
private:
    static bool has_sse2_;
    static bool has_sse41_;
    static bool has_avx_;
    static bool has_avx2_;
};

/**
 * @brief SIMD优化的坐标转换器
 */
class SimdCoordinateConverter {
public:
    /**
     * @brief 批量转换double坐标到WKB格式
     */
    static void ConvertCoordinatesToWkb(
        const double* coords,
        uint8_t* output,
        size_t count,
        bool has_z,
        bool has_m
    );
    
    /**
     * @brief 从WKB批量解析坐标
     */
    static void ConvertCoordinatesFromWkb(
        const uint8_t* input,
        double* coords,
        size_t count,
        bool has_z,
        bool has_m
    );
    
    /**
     * @brief 批量字节序转换
     */
    static void SwapEndianBatch(double* data, size_t count);
};

} // namespace DB
} // namespace OGC
```

### 22.2 SIMD实现

```cpp
#ifdef __SSE2__
#include <emmintrin.h>

void SimdCoordinateConverter::ConvertCoordinatesToWkb(
    const double* coords,
    uint8_t* output,
    size_t count,
    bool has_z,
    bool has_m
) {
    size_t dims = 2 + (has_z ? 1 : 0) + (has_m ? 1 : 0);
    size_t total_coords = count * dims;
    
    if (CpuFeatures::HasSSE2() && total_coords >= 4) {
        // SSE2优化路径
        size_t i = 0;
        for (; i + 1 < total_coords; i += 2) {
            __m128d val = _mm_loadu_pd(&coords[i]);
            _mm_storeu_pd(reinterpret_cast<double*>(output + i * sizeof(double)), val);
        }
        
        // 处理剩余元素
        for (; i < total_coords; ++i) {
            *reinterpret_cast<double*>(output + i * sizeof(double)) = coords[i];
        }
    } else {
        // 标量路径
        std::memcpy(output, coords, total_coords * sizeof(double));
    }
}

void SimdCoordinateConverter::SwapEndianBatch(double* data, size_t count) {
    if (CpuFeatures::HasSSE41() && count >= 2) {
        // SSE4.1优化路径
        const __m128i shuffle_mask = _mm_set_epi8(
            7, 6, 5, 4, 3, 2, 1, 0,
            15, 14, 13, 12, 11, 10, 9, 8
        );
        
        size_t i = 0;
        for (; i + 1 < count; i += 2) {
            __m128i val = _mm_loadu_si128(reinterpret_cast<__m128i*>(&data[i]));
            val = _mm_shuffle_epi8(val, shuffle_mask);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(&data[i]), val);
        }
        
        for (; i < count; ++i) {
            uint64_t* p = reinterpret_cast<uint64_t*>(&data[i]);
            *p = __builtin_bswap64(*p);
        }
    } else {
        for (size_t i = 0; i < count; ++i) {
            uint64_t* p = reinterpret_cast<uint64_t*>(&data[i]);
            *p = __builtin_bswap64(*p);
        }
    }
}
#endif

#ifdef __AVX__
#include <immintrin.h>

void SimdCoordinateConverter::ConvertCoordinatesToWkbAVX(
    const double* coords,
    uint8_t* output,
    size_t count,
    bool has_z,
    bool has_m
) {
    size_t dims = 2 + (has_z ? 1 : 0) + (has_m ? 1 : 0);
    size_t total_coords = count * dims;
    
    if (CpuFeatures::HasAVX() && total_coords >= 4) {
        // AVX优化路径 - 一次处理4个double
        size_t i = 0;
        for (; i + 3 < total_coords; i += 4) {
            __m256d val = _mm256_loadu_pd(&coords[i]);
            _mm256_storeu_pd(reinterpret_cast<double*>(output + i * sizeof(double)), val);
        }
        
        // 处理剩余元素
        for (; i < total_coords; ++i) {
            *reinterpret_cast<double*>(output + i * sizeof(double)) = coords[i];
        }
    } else {
        ConvertCoordinatesToWkb(coords, output, count, has_z, has_m);
    }
}
#endif
```

### 22.3 性能对比

```cpp
void BenchmarkWkbConversion() {
    const size_t coord_count = 1000000;
    std::vector<double> coords(coord_count * 2);
    std::vector<uint8_t> output(coord_count * 2 * sizeof(double));
    
    auto start = std::chrono::high_resolution_clock::now();
    
    SimdCoordinateConverter::ConvertCoordinatesToWkb(
        coords.data(), output.data(), coord_count, false, false);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "SIMD conversion: " << duration.count() << " us" << std::endl;
}
```

---

## 23. 查询计划分析接口

### 23.1 接口定义

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 查询计划节点
 */
struct QueryPlanNode {
    std::string node_type;              // 节点类型 (Seq Scan, Index Scan, etc.)
    std::string relation_name;          // 表名
    std::string index_name;             // 使用的索引名
    double startup_cost;                // 启动代价
    double total_cost;                  // 总代价
    int64_t plan_rows;                  // 预估行数
    int plan_width;                     // 预估行宽度
    double actual_time_ms;              // 实际执行时间
    int64_t actual_rows;                // 实际返回行数
    int64_t actual_loops;               // 循环次数
    std::vector<QueryPlanNode> children; // 子节点
};

/**
 * @brief 查询计划分析结果
 */
struct QueryPlanResult {
    bool success;
    std::string sql;
    QueryPlanNode root;
    double total_execution_time_ms;
    std::vector<std::string> used_indexes;
    std::vector<std::string> warnings;
    std::string raw_explain_output;
};

/**
 * @brief 查询优化建议
 */
struct QueryOptimizationSuggestion {
    std::string issue;
    std::string suggestion;
    std::string example_sql;
    int priority;  // 1-5, 5最高
};

/**
 * @brief 查询计划分析器接口
 */
class QueryPlanAnalyzer {
public:
    virtual ~QueryPlanAnalyzer() = default;
    
    /**
     * @brief 分析查询计划
     */
    virtual QueryPlanResult Analyze(const std::string& sql) = 0;
    
    /**
     * @brief 分析并执行查询
     */
    virtual QueryPlanResult AnalyzeAndExecute(const std::string& sql) = 0;
    
    /**
     * @brief 获取优化建议
     */
    virtual std::vector<QueryOptimizationSuggestion> GetOptimizationSuggestions(
        const QueryPlanResult& plan) = 0;
    
    /**
     * @brief 检查是否使用了空间索引
     */
    virtual bool UsesSpatialIndex(const QueryPlanResult& plan,
                                   const std::string& table,
                                   const std::string& geom_column) = 0;
};

} // namespace DB
} // namespace OGC
```

### 23.2 PostGIS实现

```cpp
class PostGISQueryPlanAnalyzer : public QueryPlanAnalyzer {
public:
    PostGISQueryPlanAnalyzer(DbConnection* conn) : connection_(conn) {}
    
    QueryPlanResult Analyze(const std::string& sql) override {
        QueryPlanResult result;
        result.sql = sql;
        
        std::string explain_sql = "EXPLAIN (ANALYZE, BUFFERS, FORMAT JSON) " + sql;
        
        std::unique_ptr<DbResultSet> rs;
        DbResult db_result = connection_->ExecuteQuery(explain_sql, rs);
        
        if (db_result != DbResult::kSuccess) {
            result.success = false;
            return result;
        }
        
        if (rs && rs->Next()) {
            std::string json_output = rs->GetString(0);
            result.raw_explain_output = json_output;
            result.root = ParsePlanNode(json_output);
            result.success = true;
            
            ExtractUsedIndexes(result.root, result.used_indexes);
            GenerateWarnings(result.root, result.warnings);
        }
        
        return result;
    }
    
    std::vector<QueryOptimizationSuggestion> GetOptimizationSuggestions(
        const QueryPlanResult& plan) override {
        
        std::vector<QueryOptimizationSuggestion> suggestions;
        
        // 检查全表扫描
        if (HasSequentialScan(plan.root)) {
            QueryOptimizationSuggestion s;
            s.issue = "检测到全表扫描 (Sequential Scan)";
            s.suggestion = "考虑添加适当的索引";
            s.priority = 5;
            suggestions.push_back(s);
        }
        
        // 检查空间索引使用
        if (!HasSpatialIndexScan(plan.root) && ContainsSpatialFunction(plan.sql)) {
            QueryOptimizationSuggestion s;
            s.issue = "空间查询未使用空间索引";
            s.suggestion = "确保几何列有空间索引，并使用 && 操作符进行边界框过滤";
            s.example_sql = "SELECT * FROM table WHERE geom && ST_MakeEnvelope(...)";
            s.priority = 5;
            suggestions.push_back(s);
        }
        
        // 检查高代价排序
        if (HasHighCostSort(plan.root)) {
            QueryOptimizationSuggestion s;
            s.issue = "排序操作代价较高";
            s.suggestion = "考虑在排序列上创建索引";
            s.priority = 4;
            suggestions.push_back(s);
        }
        
        return suggestions;
    }
    
    bool UsesSpatialIndex(const QueryPlanResult& plan,
                          const std::string& table,
                          const std::string& geom_column) override {
        return HasSpatialIndexScanForTable(plan.root, table, geom_column);
    }

private:
    DbConnection* connection_;
    
    QueryPlanNode ParsePlanNode(const std::string& json);
    void ExtractUsedIndexes(const QueryPlanNode& node, std::vector<std::string>& indexes);
    void GenerateWarnings(const QueryPlanNode& node, std::vector<std::string>& warnings);
    bool HasSequentialScan(const QueryPlanNode& node);
    bool HasSpatialIndexScan(const QueryPlanNode& node);
    bool HasSpatialIndexScanForTable(const QueryPlanNode& node,
                                      const std::string& table,
                                      const std::string& geom_column);
    bool HasHighCostSort(const QueryPlanNode& node);
    bool ContainsSpatialFunction(const std::string& sql);
};
```

### 23.3 使用示例

```cpp
void AnalyzeSpatialQuery(DbConnection* conn) {
    PostGISQueryPlanAnalyzer analyzer(conn);
    
    std::string sql = R"(
        SELECT * FROM parcels 
        WHERE ST_Intersects(geom, ST_MakeEnvelope(0, 0, 100, 100, 4326))
    )";
    
    auto result = analyzer.Analyze(sql);
    
    if (result.success) {
        std::cout << "Total execution time: " << result.total_execution_time_ms << " ms" << std::endl;
        std::cout << "Used indexes: ";
        for (const auto& idx : result.used_indexes) {
            std::cout << idx << " ";
        }
        std::cout << std::endl;
        
        bool uses_spatial_index = analyzer.UsesSpatialIndex(result, "parcels", "geom");
        std::cout << "Uses spatial index: " << (uses_spatial_index ? "Yes" : "No") << std::endl;
        
        auto suggestions = analyzer.GetOptimizationSuggestions(result);
        for (const auto& s : suggestions) {
            std::cout << "\nIssue: " << s.issue << std::endl;
            std::cout << "Suggestion: " << s.suggestion << std::endl;
            if (!s.example_sql.empty()) {
                std::cout << "Example: " << s.example_sql << std::endl;
            }
        }
    }
}
```

---

## 24. 数据库连接重连机制

### 24.1 自动重连设计

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 连接状态
 */
enum class ConnectionState {
    kDisconnected,
    kConnected,
    kConnecting,
    kReconnecting,
    kFailed
};

/**
 * @brief 重连配置
 */
struct ReconnectConfig {
    bool enable_auto_reconnect = true;
    int max_reconnect_attempts = 3;
    int reconnect_delay_ms = 1000;
    int reconnect_backoff_factor = 2;  // 指数退避因子
    int max_reconnect_delay_ms = 30000;
    bool reconnect_on_ping_failure = true;
};

/**
 * @brief 连接事件监听器
 */
class ConnectionEventListener {
public:
    virtual ~ConnectionEventListener() = default;
    
    virtual void OnConnected(uint64_t conn_id) = 0;
    virtual void OnDisconnected(uint64_t conn_id, const std::string& reason) = 0;
    virtual void OnReconnecting(uint64_t conn_id, int attempt) = 0;
    virtual void OnReconnectSuccess(uint64_t conn_id) = 0;
    virtual void OnReconnectFailed(uint64_t conn_id, int attempts) = 0;
};

/**
 * @brief 支持自动重连的连接包装器
 */
class ReconnectingDbConnection : public DbConnection {
public:
    ReconnectingDbConnection(std::unique_ptr<DbConnection> connection,
                             const std::string& connection_string,
                             const ReconnectConfig& config = {});
    
    ~ReconnectingDbConnection() override;
    
    // DbConnectionCore接口
    DbResult Connect(const std::string& connection_string) override;
    void Disconnect() override;
    bool IsConnected() const override;
    bool Ping() const override;
    
    // 事务管理接口
    DbResult BeginTransaction() override;
    DbResult Commit() override;
    DbResult Rollback() override;
    bool InTransaction() const override;
    
    // SQL执行接口
    DbResult Execute(const std::string& sql) override;
    DbResult ExecuteQuery(const std::string& sql,
                          std::unique_ptr<DbResultSet>& result) override;
    
    /**
     * @brief 设置事件监听器
     */
    void SetEventListener(std::shared_ptr<ConnectionEventListener> listener);
    
    /**
     * @brief 获取连接状态
     */
    ConnectionState GetState() const;
    
    /**
     * @brief 手动触发重连
     */
    DbResult Reconnect();
    
    /**
     * @brief 检查并恢复连接
     */
    bool EnsureConnected();

private:
    std::unique_ptr<DbConnection> connection_;
    std::string connection_string_;
    ReconnectConfig config_;
    std::shared_ptr<ConnectionEventListener> listener_;
    
    mutable std::mutex mutex_;
    ConnectionState state_;
    int reconnect_attempts_;
    
    DbResult TryReconnect();
    int CalculateDelay(int attempt) const;
    void NotifyReconnecting(int attempt);
    void NotifyReconnectSuccess();
    void NotifyReconnectFailed(int attempts);
};

} // namespace DB
} // namespace OGC
```

### 24.2 重连实现

```cpp
DbResult ReconnectingDbConnection::Execute(const std::string& sql) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!EnsureConnected()) {
        return DbResult::kConnectionLost;
    }
    
    DbResult result = connection_->Execute(sql);
    
    if (result == DbResult::kConnectionLost || 
        result == DbResult::kConnectionError) {
        // 尝试重连
        if (config_.enable_auto_reconnect) {
            DbResult reconnect_result = TryReconnect();
            if (reconnect_result == DbResult::kSuccess) {
                // 重连成功，重试执行
                result = connection_->Execute(sql);
            }
        }
    }
    
    return result;
}

DbResult ReconnectingDbConnection::TryReconnect() {
    reconnect_attempts_ = 0;
    
    while (reconnect_attempts_ < config_.max_reconnect_attempts) {
        reconnect_attempts_++;
        NotifyReconnecting(reconnect_attempts_);
        
        int delay = CalculateDelay(reconnect_attempts_);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        
        // 尝试重新连接
        connection_->Disconnect();
        DbResult result = connection_->Connect(connection_string_);
        
        if (result == DbResult::kSuccess) {
            state_ = ConnectionState::kConnected;
            NotifyReconnectSuccess();
            return DbResult::kSuccess;
        }
    }
    
    state_ = ConnectionState::kFailed;
    NotifyReconnectFailed(reconnect_attempts_);
    return DbResult::kConnectionLost;
}

int ReconnectingDbConnection::CalculateDelay(int attempt) const {
    int delay = config_.reconnect_delay_ms * 
                static_cast<int>(std::pow(config_.reconnect_backoff_factor, attempt - 1));
    return std::min(delay, config_.max_reconnect_delay_ms);
}

bool ReconnectingDbConnection::EnsureConnected() {
    if (state_ == ConnectionState::kConnected && connection_->IsConnected()) {
        return true;
    }
    
    if (config_.reconnect_on_ping_failure && !connection_->Ping()) {
        if (config_.enable_auto_reconnect) {
            return TryReconnect() == DbResult::kSuccess;
        }
        return false;
    }
    
    return state_ == ConnectionState::kConnected;
}
```

---

## 25. SRID严格验证

### 25.1 SRID验证策略

```cpp
namespace OGC {
namespace DB {

/**
 * @brief SRID验证策略
 */
enum class SridValidationPolicy {
    kDisabled,      // 不验证
    kWarning,       // 警告但不阻止
    kStrict,        // 严格验证，不匹配则拒绝
    kAutoConvert    // 自动转换到目标SRID
};

/**
 * @brief SRID验证配置
 */
struct SridValidationConfig {
    SridValidationPolicy policy = SridValidationPolicy::kWarning;
    int target_srid = -1;               // 目标SRID，-1表示使用表的SRID
    bool allow_unknown_srid = true;     // 是否允许未知SRID (0或-1)
    bool log_warnings = true;           // 是否记录警告日志
};

/**
 * @brief SRID验证结果
 */
struct SridValidationResult {
    bool valid;
    bool needs_conversion;
    int source_srid;
    int target_srid;
    std::string message;
};

/**
 * @brief SRID验证器
 */
class SridValidator {
public:
    explicit SridValidator(const SridValidationConfig& config);
    
    /**
     * @brief 验证几何对象的SRID
     */
    SridValidationResult Validate(const CNGeometry* geometry, int table_srid);
    
    /**
     * @brief 批量验证
     */
    std::vector<SridValidationResult> ValidateBatch(
        const std::vector<CNGeometryPtr>& geometries, int table_srid);
    
    /**
     * @brief 检查SRID是否有效
     */
    static bool IsValidSrid(int srid);
    
    /**
     * @brief 检查两个SRID是否兼容
     */
    static bool AreCompatible(int srid1, int srid2);

private:
    SridValidationConfig config_;
};

/**
 * @brief SRID一致性检查工具
 */
class SridConsistencyChecker {
public:
    /**
     * @brief 检查表中所有几何对象的SRID一致性
     */
    struct ConsistencyReport {
        bool is_consistent;
        int expected_srid;
        std::map<int, size_t> srid_distribution;  // SRID -> 数量
        std::vector<int64_t> inconsistent_row_ids;
    };
    
    static ConsistencyReport CheckTable(DbConnection* conn,
                                         const std::string& table,
                                         const std::string& geom_column);
    
    /**
     * @brief 修复SRID不一致问题
     */
    static DbResult FixInconsistentSrids(
        DbConnection* conn,
        const std::string& table,
        const std::string& geom_column,
        int target_srid,
        CoordinateTransformService* transform_service = nullptr);
};

} // namespace DB
} // namespace OGC
```

### 25.2 SRID验证实现

```cpp
SridValidationResult SridValidator::Validate(const CNGeometry* geometry, int table_srid) {
    SridValidationResult result;
    result.source_srid = geometry->GetSRID();
    result.target_srid = (config_.target_srid >= 0) ? config_.target_srid : table_srid;
    result.needs_conversion = false;
    result.valid = true;
    
    // 检查未知SRID
    if (!IsValidSrid(result.source_srid)) {
        if (!config_.allow_unknown_srid) {
            result.valid = false;
            result.message = "几何对象SRID无效: " + std::to_string(result.source_srid);
            return result;
        }
        
        if (config_.log_warnings) {
            result.message = "警告: 几何对象SRID未知，将使用目标SRID: " + 
                            std::to_string(result.target_srid);
        }
        return result;
    }
    
    // 检查SRID匹配
    if (result.source_srid != result.target_srid) {
        switch (config_.policy) {
            case SridValidationPolicy::kDisabled:
                result.valid = true;
                break;
                
            case SridValidationPolicy::kWarning:
                result.valid = true;
                result.message = "警告: SRID不匹配 (源: " + 
                                std::to_string(result.source_srid) + 
                                ", 目标: " + std::to_string(result.target_srid) + ")";
                if (config_.log_warnings) {
                    // 记录日志
                }
                break;
                
            case SridValidationPolicy::kStrict:
                result.valid = false;
                result.message = "SRID不匹配，操作被拒绝 (源: " + 
                                std::to_string(result.source_srid) + 
                                ", 目标: " + std::to_string(result.target_srid) + ")";
                break;
                
            case SridValidationPolicy::kAutoConvert:
                result.valid = true;
                result.needs_conversion = true;
                result.message = "将自动转换SRID: " + 
                                std::to_string(result.source_srid) + " -> " + 
                                std::to_string(result.target_srid);
                break;
        }
    }
    
    return result;
}

SridConsistencyChecker::ConsistencyReport SridConsistencyChecker::CheckTable(
    DbConnection* conn,
    const std::string& table,
    const std::string& geom_column
) {
    ConsistencyReport report;
    report.is_consistent = true;
    
    std::ostringstream sql;
    sql << "SELECT ST_SRID(" << geom_column << ") as srid, COUNT(*) as cnt "
        << "FROM " << table << " "
        << "WHERE " << geom_column << " IS NOT NULL "
        << "GROUP BY ST_SRID(" << geom_column << ")";
    
    std::unique_ptr<DbResultSet> result;
    conn->ExecuteQuery(sql.str(), result);
    
    while (result && result->Next()) {
        int srid = result->GetInt(0);
        size_t count = result->GetInt64(1);
        
        report.srid_distribution[srid] = count;
        
        if (report.expected_srid == 0) {
            report.expected_srid = srid;
        } else if (srid != report.expected_srid) {
            report.is_consistent = false;
        }
    }
    
    if (!report.is_consistent) {
        // 查找不一致的行
        std::ostringstream inconsistent_sql;
        inconsistent_sql << "SELECT id FROM " << table << " "
                        << "WHERE ST_SRID(" << geom_column << ") != " << report.expected_srid;
        
        conn->ExecuteQuery(inconsistent_sql.str(), result);
        while (result && result->Next()) {
            report.inconsistent_row_ids.push_back(result->GetInt64(0));
        }
    }
    
    return report;
}
```

---

## 26. 工作量评估和里程碑规划

### 26.1 模块工作量评估

| 模块 | 预估代码行数 | 预估工期 | 复杂度 | 依赖模块 |
|------|-------------|----------|--------|----------|
| **核心抽象层** | | | | |
| DbConnectionCore接口 | 300行 | 3天 | 中 | 无 |
| DbTransactionManager接口 | 200行 | 2天 | 中 | 无 |
| DbSqlExecutor接口 | 250行 | 2天 | 中 | 无 |
| DbGeometryOperations接口 | 400行 | 4天 | 高 | WKB转换器 |
| DbMetadataOperations接口 | 300行 | 3天 | 中 | 无 |
| **PostGIS适配器** | | | | |
| PostGISConnection | 800行 | 5天 | 高 | 核心抽象层 |
| PostGISBulkInserter | 600行 | 4天 | 高 | PostGISConnection |
| PostGISQueryPlanAnalyzer | 400行 | 3天 | 中 | PostGISConnection |
| **SpatiaLite适配器** | | | | |
| SpatiaLiteConnection | 700行 | 4天 | 高 | 核心抽象层 |
| SpatiaLiteBulkInserter | 500行 | 3天 | 中 | SpatiaLiteConnection |
| **WKB转换器** | | | | |
| 基础转换器 | 800行 | 5天 | 中 | 无 |
| SIMD优化路径 | 400行 | 3天 | 高 | 基础转换器 |
| **连接池** | | | | |
| DbConnectionPoolV2 | 600行 | 4天 | 高 | 核心抽象层 |
| ReconnectingDbConnection | 300行 | 2天 | 中 | DbConnectionPoolV2 |
| **事务管理** | | | | |
| TransactionManager | 400行 | 3天 | 高 | 核心抽象层 |
| DeadlockHandler | 200行 | 2天 | 中 | TransactionManager |
| **坐标转换** | | | | |
| ProjTransformService | 500行 | 4天 | 中 | PROJ库 |
| **性能监控** | | | | |
| PerformanceMonitor | 400行 | 3天 | 低 | 无 |
| SlowQueryLogger | 200行 | 2天 | 低 | 无 |
| **SRID验证** | | | | |
| SridValidator | 300行 | 2天 | 中 | 无 |
| SridConsistencyChecker | 200行 | 2天 | 中 | SridValidator |
| **测试** | | | | |
| 单元测试 | 3000行 | 3周 | 中 | 所有模块 |
| 性能测试 | 800行 | 1周 | 中 | 所有模块 |
| 集成测试 | 600行 | 1周 | 中 | 所有模块 |
| **文档与示例** | - | 2周 | 低 | - |
| **总计** | ~16300行 | **21周** | - | - |

### 26.2 里程碑规划

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           项目里程碑规划                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  M1: 核心框架完成 (第4周)                                                    │
│  ├── 核心抽象层接口定义                                                      │
│  ├── DbConnection角色接口拆分                                                │
│  ├── WKB基础转换器                                                           │
│  └── 基础单元测试框架                                                        │
│                                                                             │
│  M2: PostGIS适配器完成 (第8周)                                               │
│  ├── PostGISConnection实现                                                   │
│  ├── PostGISBulkInserter (含错误恢复)                                        │
│  ├── 连接池基础功能                                                          │
│  └── PostGIS相关测试                                                         │
│                                                                             │
│  M3: SpatiaLite适配器完成 (第11周)                                           │
│  ├── SpatiaLiteConnection实现                                                │
│  ├── SpatiaLiteBulkInserter                                                  │
│  └── SpatiaLite相关测试                                                      │
│                                                                             │
│  M4: 高级特性完成 (第15周)                                                   │
│  ├── SIMD优化路径                                                            │
│  ├── 查询计划分析                                                            │
│  ├── 自动重连机制                                                            │
│  ├── SRID严格验证                                                            │
│  └── 性能监控完善                                                            │
│                                                                             │
│  M5: 测试与文档完成 (第19周)                                                 │
│  ├── 完整单元测试覆盖                                                        │
│  ├── 性能基准测试                                                            │
│  ├── 集成测试                                                                │
│  └── API文档和示例                                                           │
│                                                                             │
│  M6: 发布准备 (第21周)                                                       │
│  ├── 代码审查                                                                │
│  ├── 文档完善                                                                │
│  ├── 发布包准备                                                              │
│  └── 发布说明                                                                │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 26.3 风险评估与缓解

| 风险 | 可能性 | 影响 | 缓解措施 | 负责人 |
|------|--------|------|----------|--------|
| PostGIS版本兼容问题 | 中 | 中 | 多版本测试矩阵 | 适配器开发 |
| SIMD跨平台兼容 | 中 | 低 | 运行时检测 + 回退 | 性能优化 |
| PROJ库版本差异 | 中 | 中 | 支持PROJ 6+和7+ | 坐标转换 |
| 性能不达标 | 低 | 高 | 持续性能基准测试 | 性能优化 |
| 内存泄漏 | 低 | 高 | RAII + Valgrind/ASan | 全员 |
| 线程死锁 | 中 | 高 | 代码审查 + ThreadSanitizer | 并发开发 |
| 跨平台编译问题 | 低 | 中 | CI/CD多平台测试 | 构建维护 |

---

## 27. 测试环境配置

### 27.1 Docker Compose配置

```yaml
version: '3.8'

services:
  postgres:
    image: postgis/postgis:15-3.3
    container_name: ogc_test_postgres
    environment:
      POSTGRES_USER: test
      POSTGRES_PASSWORD: test
      POSTGRES_DB: ogc_test
    ports:
      - "5432:5432"
    volumes:
      - postgres_data:/var/lib/postgresql/data
      - ./init/postgres:/docker-entrypoint-initdb.d
    healthcheck:
      test: ["CMD-SHELL", "pg_isready -U test -d ogc_test"]
      interval: 10s
      timeout: 5s
      retries: 5

  spatialite:
    image: osgeo/gdal:ubuntu-small-latest
    container_name: ogc_test_spatialite
    command: tail -f /dev/null
    volumes:
      - spatialite_data:/data
      - ./init/spatialite:/init
    working_dir: /data

volumes:
  postgres_data:
  spatialite_data:
```

### 27.2 测试数据库初始化脚本

```sql
-- init/postgres/01_schema.sql

-- 创建测试模式
CREATE SCHEMA IF NOT EXISTS test;

-- 创建空间扩展
CREATE EXTENSION IF NOT EXISTS postgis;
CREATE EXTENSION IF NOT EXISTS postgis_topology;

-- 创建测试表
CREATE TABLE test.points (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100),
    value DOUBLE PRECISION,
    geom GEOMETRY(Point, 4326)
);

CREATE TABLE test.lines (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100),
    geom GEOMETRY(LineString, 4326)
);

CREATE TABLE test.polygons (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100),
    area DOUBLE PRECISION,
    geom GEOMETRY(Polygon, 4326)
);

-- 创建空间索引
CREATE INDEX idx_points_geom ON test.points USING GIST (geom);
CREATE INDEX idx_lines_geom ON test.lines USING GIST (geom);
CREATE INDEX idx_polygons_geom ON test.polygons USING GIST (geom);

-- 插入测试数据
INSERT INTO test.points (name, value, geom)
SELECT 
    'point_' || i,
    random() * 100,
    ST_SetSRID(ST_MakePoint(random() * 180 - 90, random() * 90 - 45), 4326)
FROM generate_series(1, 10000) AS i;
```

### 27.3 CI/CD配置示例

```yaml
name: CI

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build-linux:
    runs-on: ubuntu-latest
    services:
      postgres:
        image: postgis/postgis:15-3.3
        env:
          POSTGRES_USER: test
          POSTGRES_PASSWORD: test
          POSTGRES_DB: ogc_test
        ports:
          - 5432:5432
        options: >-
          --health-cmd pg_isready
          --health-interval 10s
          --health-timeout 5s
          --health-retries 5

    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y \
          libpq-dev \
          libsqlite3-dev \
          libproj-dev \
          libgeos-dev \
          cmake \
          g++
    
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
    
    - name: Build
      run: cmake --build build --parallel 4
    
    - name: Run unit tests
      working-directory: build
      env:
        DB_TEST_POSTGRES_HOST: localhost
        DB_TEST_POSTGRES_PORT: 5432
        DB_TEST_POSTGRES_USER: test
        DB_TEST_POSTGRES_PASSWORD: test
        DB_TEST_POSTGRES_DATABASE: ogc_test
      run: ctest --output-on-failure

  build-windows:
    runs-on: windows-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install vcpkg
      run: |
        git clone https://github.com/Microsoft/vcpkg.git
        .\vcpkg\bootstrap-vcpkg.bat
    
    - name: Install dependencies
      run: |
        .\vcpkg\vcpkg install libpq sqlite3 proj4 geos --triplet x64-windows
    
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
    
    - name: Build
      run: cmake --build build --config Debug --parallel 4
    
    - name: Run unit tests
      working-directory: build
      run: ctest -C Debug --output-on-failure
```

### 27.4 测试配置文件

```cpp
namespace OGC {
namespace DB {
namespace Test {

struct TestConfig {
    struct Postgres {
        std::string host = "localhost";
        int port = 5432;
        std::string user = "test";
        std::string password = "test";
        std::string database = "ogc_test";
    } postgres;
    
    struct Spatialite {
        std::string db_path = ":memory:";
    } spatialite;
    
    static TestConfig LoadFromEnv() {
        TestConfig config;
        
        const char* host = std::getenv("DB_TEST_POSTGRES_HOST");
        if (host) config.postgres.host = host;
        
        const char* port = std::getenv("DB_TEST_POSTGRES_PORT");
        if (port) config.postgres.port = std::stoi(port);
        
        const char* user = std::getenv("DB_TEST_POSTGRES_USER");
        if (user) config.postgres.user = user;
        
        const char* password = std::getenv("DB_TEST_POSTGRES_PASSWORD");
        if (password) config.postgres.password = password;
        
        const char* database = std::getenv("DB_TEST_POSTGRES_DATABASE");
        if (database) config.postgres.database = database;
        
        return config;
    }
    
    std::string GetPostgresConnectionString() const {
        std::ostringstream oss;
        oss << "host=" << postgres.host
            << " port=" << postgres.port
            << " user=" << postgres.user
            << " password=" << postgres.password
            << " dbname=" << postgres.database;
        return oss.str();
    }
};

} // namespace Test
} // namespace DB
} // namespace OGC
```

---

## 28. API稳定性承诺

### 28.1 版本控制策略

采用**语义化版本控制 (Semantic Versioning 2.0.0)**:

```
MAJOR.MINOR.PATCH

MAJOR: 不兼容的API变更
MINOR: 向后兼容的功能新增
PATCH: 向后兼容的问题修复
```

### 28.2 API稳定性分级

| 级别 | 说明 | 变更策略 |
|------|------|----------|
| **Stable** | 稳定API，长期支持 | 仅在主版本升级时变更 |
| **Beta** | 测试API，可能变更 | 提前1个次版本通知废弃 |
| **Experimental** | 实验性API | 随时可能变更 |

### 28.3 接口稳定性声明

```cpp
namespace OGC {
namespace DB {

/**
 * @brief API稳定性级别
 */
enum class ApiStability {
    kStable,        // 稳定API
    kBeta,          // 测试API
    kExperimental   // 实验性API
};

/**
 * @brief API稳定性属性宏
 */
#if defined(__has_cpp_attribute)
    #if __has_cpp_attribute(deprecated)
        #define OGC_DEPRECATED(msg) [[deprecated(msg)]]
    #endif
#endif

#ifndef OGC_DEPRECATED
    #define OGC_DEPRECATED(msg)
#endif

/**
 * @brief 稳定API示例
 * @stable v1.0+
 */
class DbConnection {
    // 稳定接口...
};

/**
 * @brief 测试API示例
 * @beta v1.2+
 */
class QueryPlanAnalyzer {
    // 测试接口...
};

/**
 * @brief 实验性API示例
 * @experimental
 */
class DistributedTransactionManager {
    // 实验性接口...
};

/**
 * @brief 废弃API示例
 * @deprecated 使用DbConnectionPoolV2替代
 */
OGC_DEPRECATED("Use DbConnectionPoolV2 instead")
class DbConnectionPool {
    // 废弃接口...
};

} // namespace DB
} // namespace OGC
```

### 28.4 废弃API迁移指南

| 废弃API | 替代API | 废弃版本 | 移除版本 | 迁移说明 |
|---------|---------|----------|----------|----------|
| DbConnectionPool | DbConnectionPoolV2 | v1.2 | v2.0 | 接口兼容，直接替换类名 |
| PostGISBulkInserter | PostGISBulkInserterV2 | v1.2 | v2.0 | 新增错误恢复功能，构造函数参数不变 |
| DbConnection::InsertGeometry (批量版本) | DbGeometryOperations::InsertGeometries | v1.2 | v2.0 | 接口拆分到角色接口 |

### 28.5 版本兼容性矩阵

| 库版本 | PostgreSQL | PostGIS | SQLite | SpatiaLite | PROJ |
|--------|------------|---------|--------|------------|------|
| v1.0 | 12-15 | 3.0-3.3 | 3.35+ | 5.0+ | 6.0+ |
| v1.1 | 12-15 | 3.0-3.3 | 3.35+ | 5.0+ | 6.0+ |
| v1.2 | 12-16 | 3.0-3.4 | 3.35+ | 5.0+ | 6.0+, 7.0+ |

### 28.6 变更日志格式

```markdown
## [1.2.0] - 2026-03-18

### Added
- 角色接口设计 (DbConnectionCore, DbTransactionManager等)
- 批量插入错误恢复机制
- 连接池耗尽处理策略
- WKB转换器SIMD优化路径
- 查询计划分析接口
- 数据库连接重连机制
- SRID严格验证选项

### Changed
- DbConnection接口拆分为多个角色接口 (向后兼容)
- PostGISBulkInserter增强错误恢复功能

### Deprecated
- DbConnectionPool: 使用DbConnectionPoolV2替代
- PostGISBulkInserter: 使用PostGISBulkInserterV2替代

### Fixed
- 连接池耗尽时的处理策略
- 批量插入失败后的数据一致性问题

### Security
- SRID验证防止数据注入
```

---

## 29. GeoJSON序列化设计 *(v1.3新增)*

### 29.1 GeoJSON转换器接口

```cpp
namespace OGC {
namespace DB {

/**
 * @brief GeoJSON序列化配置
 */
struct GeoJsonConfig {
    int coordinate_precision = 6;       // 坐标小数位数
    bool include_srid = false;          // 是否包含SRID
    bool include_bbox = true;           // 是否包含边界框
    bool pretty_print = false;          // 是否美化输出
    bool strict_ogc = true;             // 是否严格遵循OGC规范
};

/**
 * @brief GeoJSON格式转换器
 */
class GeoJsonConverter {
public:
    /**
     * @brief 将几何对象转换为GeoJSON字符串
     */
    static DbResult GeometryToGeoJson(const CNGeometry* geometry,
                                       std::string& geojson,
                                       const GeoJsonConfig& config = {});
    
    /**
     * @brief 将GeoJSON字符串转换为几何对象
     */
    static DbResult GeoJsonToGeometry(const std::string& geojson,
                                       CNGeometryPtr& geometry);
    
    /**
     * @brief 将FeatureCollection转换为几何对象数组
     */
    static DbResult FeatureCollectionToGeometries(
        const std::string& geojson,
        std::vector<CNGeometryPtr>& geometries,
        std::vector<std::map<std::string, std::string>>& properties);
    
    /**
     * @brief 将几何对象数组转换为FeatureCollection
     */
    static DbResult GeometriesToFeatureCollection(
        const std::vector<CNGeometryPtr>& geometries,
        const std::vector<std::map<std::string, std::string>>& properties,
        std::string& geojson,
        const GeoJsonConfig& config = {});

private:
    static std::string FormatCoordinate(const Coordinate& coord, int precision);
    static std::string FormatCoordinates(const std::vector<Coordinate>& coords, 
                                          int precision, bool has_z);
};

} // namespace DB
} // namespace OGC
```

### 29.2 GeoJSON实现示例

```cpp
DbResult GeoJsonConverter::GeometryToGeoJson(const CNGeometry* geometry,
                                              std::string& geojson,
                                              const GeoJsonConfig& config) {
    if (!geometry || geometry->IsEmpty()) {
        return DbResult::kInvalidGeometry;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(config.coordinate_precision);
    
    oss << "{\"type\":\"";
    
    switch (geometry->GetGeometryType()) {
        case GeomType::kPoint:
            oss << "Point\",\"coordinates\":";
            FormatPointCoordinate(oss, dynamic_cast<const CNPoint*>(geometry), config);
            break;
        case GeomType::kLineString:
            oss << "LineString\",\"coordinates\":";
            FormatLineStringCoordinates(oss, dynamic_cast<const CNLineString*>(geometry), config);
            break;
        case GeomType::kPolygon:
            oss << "Polygon\",\"coordinates\":";
            FormatPolygonCoordinates(oss, dynamic_cast<const CNPolygon*>(geometry), config);
            break;
        // ... 其他类型
        default:
            return DbResult::kNotImplemented;
    }
    
    if (config.include_bbox) {
        Envelope env = geometry->GetEnvelope();
        oss << ",\"bbox\":[" 
            << env.GetMinX() << "," << env.GetMinY() << ","
            << env.GetMaxX() << "," << env.GetMaxY() << "]";
    }
    
    if (config.include_srid && geometry->GetSRID() > 0) {
        oss << ",\"crs\":{\"type\":\"name\",\"properties\":{\"name\":\"EPSG:" 
            << geometry->GetSRID() << "\"}}";
    }
    
    oss << "}";
    geojson = oss.str();
    return DbResult::kSuccess;
}
```

---

## 30. 空间索引策略对比 *(v1.3新增)*

### 30.1 索引类型对比

| 特性 | PostGIS GiST | PostGIS BRIN | SpatiaLite R-Tree |
|------|--------------|--------------|-------------------|
| **索引类型** | 通用搜索树 | 块范围索引 | R-Tree |
| **适用场景** | 通用空间查询 | 时序/有序数据 | 嵌入式场景 |
| **索引大小** | 较大 | 极小 | 中等 |
| **查询性能** | 优秀 | 良好(有序数据) | 良好 |
| **更新代价** | 中等 | 低 | 中等 |
| **创建速度** | 中等 | 极快 | 中等 |
| **并发支持** | 优秀 | 优秀 | 单写多读 |

### 30.2 索引选择指南

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 空间索引类型枚举
 */
enum class SpatialIndexType {
    kGiST,      // PostGIS默认
    kBRIN,      // PostGIS块范围索引
    kRTree,     // SpatiaLite默认
    kAuto       // 自动选择
};

/**
 * @brief 空间索引选择建议
 */
struct IndexRecommendation {
    SpatialIndexType recommended_type;
    std::string reason;
    std::string create_sql;
    double estimated_size_mb;
    bool supports_concurrent_write;
};

/**
 * @brief 索引选择器
 */
class SpatialIndexSelector {
public:
    /**
     * @brief 根据数据特征推荐索引类型
     */
    static IndexRecommendation Recommend(
        DatabaseType db_type,
        const std::string& table_name,
        const std::string& geom_column,
        int64_t estimated_rows,
        bool is_temporally_ordered = false,
        double update_frequency = 0.0);
    
    /**
     * @brief 获取索引创建SQL
     */
    static std::string GetCreateIndexSql(
        DatabaseType db_type,
        SpatialIndexType index_type,
        const std::string& table_name,
        const std::string& geom_column,
        const std::string& index_name = "");
};

} // namespace DB
} // namespace OGC
```

### 30.3 索引选择决策树

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        空间索引选择决策树                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  数据库类型?                                                                 │
│  ├── PostGIS                                                                │
│  │   ├── 数据是否时间有序?                                                   │
│  │   │   ├── YES → BRIN (节省空间,创建快)                                    │
│  │   │   └── NO  → 数据量?                                                  │
│  │   │       ├── < 100万 → GiST (查询性能最优)                               │
│  │   │       └── >= 100万 → GiST + 考虑分区                                  │
│  │   └── 更新频率高?                                                         │
│  │       ├── YES → BRIN (更新代价低)                                        │
│  │       └── NO  → GiST (查询性能最优)                                       │
│  │                                                                          │
│  └── SpatiaLite                                                             │
│      └── R-Tree (唯一选择,性能良好)                                          │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 31. SRID验证与坐标转换流水线 *(v1.3新增)*

### 31.1 统一数据流设计

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 几何数据处理流水线
 */
class GeometryDataPipeline {
public:
    /**
     * @brief 流水线配置
     */
    struct Config {
        SridValidationPolicy srid_policy = SridValidationPolicy::kWarning;
        int target_srid = -1;
        bool auto_convert = false;
        bool validate_geometry = true;
        size_t max_vertices = 1000000;  // 最大顶点数限制
    };
    
    explicit GeometryDataPipeline(const Config& config);
    
    /**
     * @brief 处理几何对象
     * @return 处理结果，包含验证、转换后的几何对象
     */
    struct ProcessResult {
        DbResult result;
        CNGeometryPtr geometry;
        bool was_converted;
        int original_srid;
        std::string warning_message;
    };
    
    ProcessResult Process(const CNGeometry* input);
    
    /**
     * @brief 批量处理
     */
    std::vector<ProcessResult> ProcessBatch(const std::vector<CNGeometryPtr>& inputs);
    
    /**
     * @brief 设置坐标转换服务
     */
    void SetTransformService(std::shared_ptr<CoordinateTransformService> service);

private:
    Config config_;
    std::shared_ptr<SridValidator> srid_validator_;
    std::shared_ptr<CoordinateTransformService> transform_service_;
    std::shared_ptr<GeometryValidator> geometry_validator_;
};

} // namespace DB
} // namespace OGC
```

### 31.2 流水线处理流程

```cpp
GeometryDataPipeline::ProcessResult GeometryDataPipeline::Process(const CNGeometry* input) {
    ProcessResult result;
    result.original_srid = input ? input->GetSRID() : 0;
    
    if (!input) {
        result.result = DbResult::kInvalidGeometry;
        return result;
    }
    
    // Step 1: 几何对象验证
    if (config_.validate_geometry) {
        auto validation = geometry_validator_->Validate(input);
        if (!validation.valid) {
            result.result = DbResult::kInvalidGeometry;
            result.warning_message = validation.message;
            return result;
        }
        
        // 检查顶点数限制
        if (input->GetNumPoints() > config_.max_vertices) {
            result.result = DbResult::kResourceExhausted;
            result.warning_message = "Geometry exceeds max vertices limit";
            return result;
        }
    }
    
    // Step 2: SRID验证
    int target_srid = config_.target_srid;
    if (target_srid < 0) {
        // 使用默认SRID
        target_srid = 4326;
    }
    
    auto srid_result = srid_validator_->Validate(input, target_srid);
    if (!srid_result.valid && config_.srid_policy == SridValidationPolicy::kStrict) {
        result.result = DbResult::kSridMismatch;
        result.warning_message = srid_result.message;
        return result;
    }
    
    // Step 3: 坐标转换
    result.geometry = input->Clone();
    if (srid_result.needs_conversion && config_.auto_convert) {
        if (transform_service_) {
            auto transform_result = transform_service_->Transform(
                result.geometry.get(), target_srid);
            if (transform_result.result == DbResult::kSuccess) {
                result.was_converted = true;
                result.geometry = std::move(transform_result.geometry);
            }
        }
    }
    
    result.result = DbResult::kSuccess;
    return result;
}
```

---

## 32. 空间函数兼容性矩阵 *(v1.3新增)*

### 32.1 OGC空间函数映射

| OGC函数 | PostGIS | SpatiaLite | 说明 |
|---------|---------|------------|------|
| ST_Intersects | ST_Intersects | ST_Intersects | 完全兼容 |
| ST_Contains | ST_Contains | ST_Contains | 完全兼容 |
| ST_Within | ST_Within | ST_Within | 完全兼容 |
| ST_Touches | ST_Touches | ST_Touches | 完全兼容 |
| ST_Crosses | ST_Crosses | ST_Crosses | 完全兼容 |
| ST_Overlaps | ST_Overlaps | ST_Overlaps | 完全兼容 |
| ST_Disjoint | ST_Disjoint | ST_Disjoint | 完全兼容 |
| ST_Equals | ST_Equals | ST_Equals | 完全兼容 |
| ST_DWithin | ST_DWithin | ST_Distance < n | 语法差异 |
| ST_Buffer | ST_Buffer | ST_Buffer | 参数差异 |
| ST_Union | ST_Union | ST_Union | 完全兼容 |
| ST_Intersection | ST_Intersection | ST_Intersection | 完全兼容 |
| ST_Difference | ST_Difference | ST_Difference | 完全兼容 |
| ST_SymDifference | ST_SymDifference | ST_SymDifference | 完全兼容 |
| ST_Distance | ST_Distance | ST_Distance | 完全兼容 |
| ST_Area | ST_Area | ST_Area | 完全兼容 |
| ST_Length | ST_Length | ST_Length | 完全兼容 |
| ST_Centroid | ST_Centroid | ST_Centroid | 完全兼容 |
| ST_Envelope | ST_Envelope | ST_Envelope | 完全兼容 |
| ST_Transform | ST_Transform | ST_Transform | 需PROJ支持 |

### 32.2 函数映射器

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 空间函数映射器
 */
class SpatialFunctionMapper {
public:
    /**
     * @brief 获取数据库特定的函数名
     */
    static std::string GetFunctionName(DatabaseType db_type, 
                                        SpatialOperator op);
    
    /**
     * @brief 构建空间查询SQL
     */
    static std::string BuildSpatialQuery(
        DatabaseType db_type,
        const std::string& table,
        const std::string& geom_column,
        SpatialOperator op,
        const std::string& query_geom_param,
        const std::string& where_clause = "");
    
    /**
     * @brief 获取函数支持状态
     */
    static bool IsFunctionSupported(DatabaseType db_type, 
                                     SpatialOperator op);
};

} // namespace DB
} // namespace OGC
```

---

## 33. 连接池预热与性能指标 *(v1.3新增)*

### 33.1 连接池预热接口

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 连接池预热配置
 */
struct WarmupConfig {
    bool enable = true;
    size_t warmup_connections = 0;      // 0表示使用min_size
    int warmup_timeout_ms = 30000;
    bool validate_connections = true;
    bool parallel_warmup = true;
};

/**
 * @brief 扩展的连接池接口
 */
class DbConnectionPoolV3 : public DbConnectionPoolV2 {
public:
    /**
     * @brief 预热连接池
     * @return 成功创建的连接数
     */
    size_t Warmup(const WarmupConfig& config = {});
    
    /**
     * @brief 异步预热
     */
    std::future<size_t> WarmupAsync(const WarmupConfig& config = {});
    
    /**
     * @brief 获取性能指标
     */
    struct PerformanceMetrics {
        // 连接获取指标
        double avg_acquire_time_ms;
        double max_acquire_time_ms;
        double p99_acquire_time_ms;
        size_t acquire_count;
        size_t acquire_timeout_count;
        
        // 连接使用指标
        double avg_connection_usage_time_ms;
        double max_connection_usage_time_ms;
        size_t total_queries_executed;
        
        // 连接池状态
        double utilization_rate;
        size_t peak_active_connections;
        size_t connection_create_count;
        size_t connection_destroy_count;
        
        // 等待指标
        size_t waiting_threads_peak;
        double avg_wait_time_ms;
    };
    
    PerformanceMetrics GetMetrics() const;
    
    /**
     * @brief 重置性能指标
     */
    void ResetMetrics();
    
    /**
     * @brief 导出Prometheus格式指标
     */
    std::string ExportPrometheusMetrics(const std::string& prefix = "ogc_db_pool") const;
};

} // namespace DB
} // namespace OGC
```

### 33.2 预热实现

```cpp
size_t DbConnectionPoolV3::Warmup(const WarmupConfig& config) {
    if (!config.enable) {
        return 0;
    }
    
    size_t target_count = config.warmup_connections > 0 
        ? config.warmup_connections 
        : config_.min_size;
    
    std::vector<std::future<std::unique_ptr<DbConnection>>> futures;
    
    if (config.parallel_warmup) {
        // 并行创建连接
        for (size_t i = 0; i < target_count; ++i) {
            futures.emplace_back(std::async(std::launch::async, [this]() {
                return CreateConnection();
            }));
        }
    }
    
    size_t created = 0;
    std::unique_lock<std::mutex> lock(mutex_);
    
    if (config.parallel_warmup) {
        for (auto& future : futures) {
            auto conn = future.get();
            if (conn) {
                if (!config.validate_connections || conn->Ping()) {
                    available_.push(std::move(conn));
                    created++;
                }
            }
        }
    } else {
        for (size_t i = 0; i < target_count; ++i) {
            auto conn = CreateConnection();
            if (conn && (!config.validate_connections || conn->Ping())) {
                available_.push(std::move(conn));
                created++;
            }
        }
    }
    
    total_count_ = created;
    return created;
}
```

---

## 34. 事务超时处理 *(v1.3新增)*

### 34.1 事务超时配置

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 事务超时行为
 */
enum class TransactionTimeoutBehavior {
    kRollback,          // 回滚事务
    kRollbackAndThrow,  // 回滚并抛出异常
    kRollbackAndRetry,  // 回滚并自动重试
    kCustom             // 调用自定义回调
};

/**
 * @brief 事务超时配置
 */
struct TransactionTimeoutConfig {
    int timeout_ms = 30000;
    TransactionTimeoutBehavior behavior = TransactionTimeoutBehavior::kRollback;
    int max_retries = 3;
    std::function<void(DbResult)> timeout_callback;
};

/**
 * @brief 带超时的事务管理器
 */
class TimeoutTransactionManager {
public:
    explicit TimeoutTransactionManager(DbConnection* connection,
                                        const TransactionTimeoutConfig& config = {});
    
    /**
     * @brief 开始事务
     */
    DbResult BeginTransaction();
    
    /**
     * @brief 提交事务
     */
    DbResult Commit();
    
    /**
     * @brief 执行操作(带超时检查)
     */
    template<typename Func>
    DbResult Execute(Func&& operation);
    
    /**
     * @brief 检查是否超时
     */
    bool IsTimedOut() const;
    
    /**
     * @brief 获取剩余时间
     */
    int GetRemainingTimeMs() const;

private:
    DbConnection* connection_;
    TransactionTimeoutConfig config_;
    std::chrono::steady_clock::time_point start_time_;
    bool in_transaction_;
};

} // namespace DB
} // namespace OGC
```

### 34.2 超时处理实现

```cpp
template<typename Func>
DbResult TimeoutTransactionManager::Execute(Func&& operation) {
    if (!in_transaction_) {
        return DbResult::kTransactionError;
    }
    
    // 检查是否已超时
    if (IsTimedOut()) {
        switch (config_.behavior) {
            case TransactionTimeoutBehavior::kRollback:
                connection_->Rollback();
                in_transaction_ = false;
                return DbResult::kConnectionTimeout;
                
            case TransactionTimeoutBehavior::kRollbackAndThrow:
                connection_->Rollback();
                in_transaction_ = false;
                throw DbException(DbResult::kConnectionTimeout, "Transaction timed out");
                
            case TransactionTimeoutBehavior::kCustom:
                if (config_.timeout_callback) {
                    config_.timeout_callback(DbResult::kConnectionTimeout);
                }
                connection_->Rollback();
                in_transaction_ = false;
                return DbResult::kConnectionTimeout;
                
            default:
                connection_->Rollback();
                in_transaction_ = false;
                return DbResult::kConnectionTimeout;
        }
    }
    
    // 执行操作
    return operation();
}
```

---

## 35. SIMD运行时检测与回退 *(v1.3新增)*

### 35.1 运行时CPU特性检测

```cpp
namespace OGC {
namespace DB {

/**
 * @brief CPU特性检测器(增强版)
 */
class CpuFeatureDetector {
public:
    enum class Feature {
        kSSE2,
        kSSE41,
        kAVX,
        kAVX2,
        kAVX512F,
        kNEON  // ARM NEON
    };
    
    /**
     * @brief 检测CPU特性(首次调用时自动检测)
     */
    static bool HasFeature(Feature feature);
    
    /**
     * @brief 获取最佳SIMD级别
     */
    static Feature GetBestSimdLevel();
    
    /**
     * @brief 获取特性描述字符串
     */
    static std::string GetFeatureString();
    
    /**
     * @brief 强制使用特定SIMD级别(用于测试)
     */
    static void ForceSimdLevel(Feature level);

private:
    static bool detected_;
    static bool has_sse2_;
    static bool has_sse41_;
    static bool has_avx_;
    static bool has_avx2_;
    static bool has_avx512f_;
    static bool has_neon_;
    static Feature forced_level_;
    
    static void Detect();
};

} // namespace DB
} // namespace OGC
```

### 35.2 SIMD函数选择器

```cpp
namespace OGC {
namespace DB {

/**
 * @brief SIMD函数选择器
 */
template<typename Func>
class SimdDispatcher {
public:
    /**
     * @brief 注册不同SIMD级别的实现
     */
    void RegisterScalar(Func func);
    void RegisterSSE2(Func func);
    void RegisterSSE41(Func func);
    void RegisterAVX(Func func);
    void RegisterAVX2(Func func);
    
    /**
     * @brief 获取最佳实现
     */
    Func GetBestImplementation() const {
        auto level = CpuFeatureDetector::GetBestSimdLevel();
        
        switch (level) {
            case CpuFeatureDetector::Feature::kAVX2:
                if (avx2_func_) return avx2_func_;
                // fallthrough
            case CpuFeatureDetector::Feature::kAVX:
                if (avx_func_) return avx_func_;
                // fallthrough
            case CpuFeatureDetector::Feature::kSSE41:
                if (sse41_func_) return sse41_func_;
                // fallthrough
            case CpuFeatureDetector::Feature::kSSE2:
                if (sse2_func_) return sse2_func_;
                // fallthrough
            default:
                return scalar_func_;
        }
    }
    
    /**
     * @brief 调用最佳实现
     */
    template<typename... Args>
    auto operator()(Args&&... args) -> decltype(std::declval<Func>()(std::forward<Args>(args)...)) {
        return GetBestImplementation()(std::forward<Args>(args)...);
    }

private:
    Func scalar_func_ = nullptr;
    Func sse2_func_ = nullptr;
    Func sse41_func_ = nullptr;
    Func avx_func_ = nullptr;
    Func avx2_func_ = nullptr;
};

} // namespace DB
} // namespace OGC
```

### 35.3 使用示例

```cpp
// 初始化SIMD分发器
SimdDispatcher<ConvertFunc> converter;

converter.RegisterScalar(ConvertCoordinatesScalar);
converter.RegisterSSE2(ConvertCoordinatesSSE2);
converter.RegisterAVX(ConvertCoordinatesAVX);

// 运行时自动选择最佳实现
converter(coords, output, count, has_z, has_m);
```

---

## 36. SQL注入防护增强 *(v1.3新增)*

### 36.1 标识符验证器

```cpp
namespace OGC {
namespace DB {

/**
 * @brief SQL标识符验证器
 */
class SqlIdentifierValidator {
public:
    /**
     * @brief 验证表名
     */
    static bool IsValidTableName(const std::string& name);
    
    /**
     * @brief 验证列名
     */
    static bool IsValidColumnName(const std::string& name);
    
    /**
     * @brief 转义标识符
     */
    static std::string EscapeIdentifier(const std::string& identifier,
                                         DatabaseType db_type);
    
    /**
     * @brief 验证并转义
     */
    static DbResult ValidateAndEscape(const std::string& identifier,
                                       std::string& escaped,
                                       DatabaseType db_type);

private:
    // SQL关键字黑名单
    static const std::set<std::string> sql_keywords_;
    // 允许的字符模式
    static const std::regex valid_pattern_;
};

} // namespace DB
} // namespace OGC
```

### 36.2 几何对象复杂度验证

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 几何对象验证器
 */
class GeometryValidator {
public:
    /**
     * @brief 验证配置
     */
    struct Config {
        size_t max_vertices = 1000000;          // 最大顶点数
        size_t max_rings = 1000;                // 最大环数
        size_t max_elements = 10000;            // 最大元素数(集合类型)
        double max_extent = 180.0;              // 最大范围(度)
        bool validate_topology = true;          // 是否验证拓扑
        bool allow_self_intersection = false;   // 是否允许自相交
    };
    
    /**
     * @brief 验证结果
     */
    struct ValidationResult {
        bool valid;
        std::string message;
        size_t vertex_count;
        size_t ring_count;
        size_t element_count;
        std::vector<std::string> warnings;
    };
    
    explicit GeometryValidator(const Config& config = {});
    
    /**
     * @brief 验证几何对象
     */
    ValidationResult Validate(const CNGeometry* geometry);
    
    /**
     * @brief 快速复杂度检查
     */
    bool CheckComplexity(const CNGeometry* geometry) const;

private:
    Config config_;
};

} // namespace DB
} // namespace OGC
```

---

## 37. 审计日志接口 *(v1.3新增)*

### 37.1 审计日志设计

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 审计事件类型
 */
enum class AuditEventType {
    kConnect,
    kDisconnect,
    kQuery,
    kInsert,
    kUpdate,
    kDelete,
    kTransactionBegin,
    kTransactionCommit,
    kTransactionRollback,
    kSpatialQuery,
    kBulkInsert,
    kSchemaChange
};

/**
 * @brief 审计日志记录
 */
struct AuditLogEntry {
    uint64_t connection_id;
    AuditEventType event_type;
    std::string table_name;
    std::string operation_detail;
    int64_t rows_affected;
    int64_t execution_time_ms;
    std::string user_identity;
    std::string client_ip;
    std::chrono::system_clock::time_point timestamp;
    DbResult result;
};

/**
 * @brief 审计日志接口
 */
class AuditLogger {
public:
    virtual ~AuditLogger() = default;
    
    /**
     * @brief 记录审计事件
     */
    virtual void Log(const AuditLogEntry& entry) = 0;
    
    /**
     * @brief 查询审计日志
     */
    virtual std::vector<AuditLogEntry> Query(
        const std::string& table_name = "",
        AuditEventType event_type = AuditEventType::kQuery,
        const std::chrono::system_clock::time_point& start = {},
        const std::chrono::system_clock::time_point& end = {}) = 0;
};

/**
 * @brief 文件审计日志实现
 */
class FileAuditLogger : public AuditLogger {
public:
    explicit FileAuditLogger(const std::string& filepath,
                              bool rotate_daily = true);
    
    void Log(const AuditLogEntry& entry) override;
    std::vector<AuditLogEntry> Query(...) override;

private:
    std::ofstream file_;
    std::mutex mutex_;
    std::string filepath_;
    bool rotate_daily_;
};

/**
 * @brief 数据库审计日志实现
 */
class DatabaseAuditLogger : public AuditLogger {
public:
    explicit DatabaseAuditLogger(DbConnection* connection,
                                  const std::string& table_name = "audit_log");
    
    void Log(const AuditLogEntry& entry) override;
    std::vector<AuditLogEntry> Query(...) override;
};

} // namespace DB
} // namespace OGC
```

---

## 38. ABI兼容性与版本控制 *(v1.3新增)*

### 38.1 ABI版本控制

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 库版本信息
 */
struct LibraryVersion {
    int major;
    int minor;
    int patch;
    int abi_version;
    std::string git_commit;
    std::string build_date;
    
    static LibraryVersion GetCurrent();
    static std::string GetVersionString();
};

/**
 * @brief ABI兼容性检查
 */
class AbiCompatibility {
public:
    /**
     * @brief 检查ABI兼容性
     */
    static bool IsCompatible(int compiled_abi_version, int runtime_abi_version);
    
    /**
     * @brief 获取ABI版本
     */
    static int GetAbiVersion();
};

/**
 * @brief PIMPL模式基类
 */
template<typename Impl>
class PimplBase {
protected:
    std::unique_ptr<Impl> impl_;
    
    PimplBase() : impl_(std::make_unique<Impl>()) {}
    ~PimplBase() = default;
    
    // 禁止拷贝
    PimplBase(const PimplBase&) = delete;
    PimplBase& operator=(const PimplBase&) = delete;
    
    // 支持移动
    PimplBase(PimplBase&&) noexcept = default;
    PimplBase& operator=(PimplBase&&) noexcept = default;
};

} // namespace DB
} // namespace OGC
```

### 38.2 接口版本控制

```cpp
/**
 * @brief 接口版本查询
 */
class InterfaceVersion {
public:
    /**
     * @brief 获取DbConnection接口版本
     */
    static int GetDbConnectionVersion();
    
    /**
     * @brief 获取DbConnectionPool接口版本
     */
    static int GetDbConnectionPoolVersion();
    
    /**
     * @brief 检查接口兼容性
     */
    static bool CheckCompatibility();
};

/**
 * @brief 版本化接口基类
 */
template<int Version>
class VersionedInterface {
public:
    static constexpr int GetInterfaceVersion() { return Version; }
    
    virtual bool IsVersionSupported(int version) const {
        return version <= Version;
    }
};

// 使用示例
class DbConnectionV2 : public VersionedInterface<2> {
    // v2接口定义
};
```

---

## 39. C API绑定 *(v1.3新增)*

### 39.1 C API定义

```cpp
#ifdef __cplusplus
extern "C" {
#endif

// 句柄类型定义
typedef struct OGC_DbConnection* OGC_DbConnectionH;
typedef struct OGC_DbResultSet* OGC_DbResultSetH;
typedef struct OGC_DbStatement* OGC_DbStatementH;
typedef struct OGC_Geometry* OGC_GeometryH;

// 版本查询
OGC_API const char* OGC_GetVersion(void);
OGC_API int OGC_GetAbiVersion(void);

// 连接管理
OGC_API OGC_DbConnectionH OGC_DbConnection_Create(int db_type);
OGC_API void OGC_DbConnection_Destroy(OGC_DbConnectionH conn);
OGC_API int OGC_DbConnection_Connect(OGC_DbConnectionH conn, const char* conn_str);
OGC_API void OGC_DbConnection_Disconnect(OGC_DbConnectionH conn);
OGC_API int OGC_DbConnection_IsConnected(OGC_DbConnectionH conn);

// 事务管理
OGC_API int OGC_DbConnection_BeginTransaction(OGC_DbConnectionH conn);
OGC_API int OGC_DbConnection_Commit(OGC_DbConnectionH conn);
OGC_API int OGC_DbConnection_Rollback(OGC_DbConnectionH conn);

// 几何操作
OGC_API int OGC_DbConnection_InsertGeometry(
    OGC_DbConnectionH conn,
    const char* table,
    const char* geom_column,
    OGC_GeometryH geometry,
    int64_t* out_id);

OGC_API int OGC_DbConnection_SpatialQuery(
    OGC_DbConnectionH conn,
    const char* table,
    const char* geom_column,
    int spatial_op,
    OGC_GeometryH query_geom,
    OGC_DbResultSetH* result);

// 结果集
OGC_API int OGC_DbResultSet_Next(OGC_DbResultSetH result);
OGC_API OGC_GeometryH OGC_DbResultSet_GetGeometry(OGC_DbResultSetH result, int column);
OGC_API int64_t OGC_DbResultSet_GetInt64(OGC_DbResultSetH result, int column);
OGC_API const char* OGC_DbResultSet_GetString(OGC_DbResultSetH result, int column);
OGC_API void OGC_DbResultSet_Destroy(OGC_DbResultSetH result);

// 错误处理
OGC_API const char* OGC_GetLastError(OGC_DbConnectionH conn);
OGC_API int OGC_GetLastErrorCode(OGC_DbConnectionH conn);

#ifdef __cplusplus
}
#endif
```

### 39.2 C API实现

```cpp
extern "C" {

OGC_DbConnectionH OGC_DbConnection_Create(int db_type) {
    try {
        auto conn = OGC::DB::DbConnectionFactory::CreateConnection(
            static_cast<OGC::DB::DatabaseType>(db_type));
        return reinterpret_cast<OGC_DbConnectionH>(conn.release());
    } catch (...) {
        return nullptr;
    }
}

void OGC_DbConnection_Destroy(OGC_DbConnectionH conn) {
    if (conn) {
        delete reinterpret_cast<OGC::DB::DbConnection*>(conn);
    }
}

int OGC_DbConnection_Connect(OGC_DbConnectionH conn, const char* conn_str) {
    if (!conn || !conn_str) return -1;
    auto* c = reinterpret_cast<OGC::DB::DbConnection*>(conn);
    return static_cast<int>(c->Connect(conn_str));
}

} // extern "C"
```

---

## 40. 错误处理统一策略 *(v1.3新增)*

### 40.1 统一错误处理设计

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 错误处理策略
 */
enum class ErrorHandlingStrategy {
    kResultCode,    // 返回错误码(推荐)
    kException,     // 抛出异常
    kBoth           // 设置错误码并可选抛出异常
};

/**
 * @brief 全局错误处理配置
 */
class ErrorHandlingConfig {
public:
    static ErrorHandlingStrategy GetStrategy();
    static void SetStrategy(ErrorHandlingStrategy strategy);
    
    static bool ShouldThrowOnError();
    static void SetThrowOnError(bool enabled);
    
    static std::function<void(DbResult, const std::string&)> GetErrorHandler();
    static void SetErrorHandler(std::function<void(DbResult, const std::string&)> handler);
};

/**
 * @brief 错误上下文
 */
class ErrorContext {
public:
    static ErrorContext& Instance();
    
    /**
     * @brief 设置最后错误
     */
    void SetLastError(DbResult code, const std::string& message);
    
    /**
     * @brief 获取最后错误码
     */
    DbResult GetLastErrorCode() const;
    
    /**
     * @brief 获取最后错误消息
     */
    std::string GetLastError() const;
    
    /**
     * @brief 清除错误
     */
    void ClearError();

private:
    thread_local static DbResult last_code_;
    thread_local static std::string last_message_;
};

/**
 * @brief 错误处理宏
 */
#define OGC_RETURN_ON_ERROR(expr) \
    do { \
        OGC::DB::DbResult _result = (expr); \
        if (_result != OGC::DB::DbResult::kSuccess) { \
            return _result; \
        } \
    } while(0)

#define OGC_THROW_ON_ERROR(expr) \
    do { \
        OGC::DB::DbResult _result = (expr); \
        if (_result != OGC::DB::DbResult::kSuccess) { \
            throw OGC::DB::DbException(_result, OGC::DB::ErrorContext::Instance().GetLastError()); \
        } \
    } while(0)

} // namespace DB
} // namespace OGC
```

---

## 41. 连接字符串安全处理 *(v1.3新增)*

### 41.1 连接字符串解析器

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 连接字符串解析器
 */
class ConnectionStringParser {
public:
    /**
     * @brief 解析连接字符串
     */
    struct ParsedConnection {
        std::string host;
        int port = 0;
        std::string database;
        std::string user;
        std::string password;
        std::map<std::string, std::string> extra_params;
        bool ssl_enabled = false;
    };
    
    static ParsedConnection Parse(const std::string& connection_string);
    
    /**
     * @brief 验证连接字符串
     */
    struct ValidationResult {
        bool valid;
        std::string error_message;
        std::vector<std::string> warnings;
    };
    
    static ValidationResult Validate(const std::string& connection_string);
    
    /**
     * @brief 脱敏连接字符串(隐藏密码)
     */
    static std::string Redact(const std::string& connection_string);
    
    /**
     * @brief 构建连接字符串
     */
    static std::string Build(const ParsedConnection& parsed);
};

} // namespace DB
} // namespace OGC
```

### 41.2 脱敏实现

```cpp
std::string ConnectionStringParser::Redact(const std::string& connection_string) {
    std::string result = connection_string;
    
    // 匹配并替换密码
    // 格式1: password=xxx
    // 格式2: pwd=xxx
    std::regex password_regex("(password|pwd)=[^\\s;]*", 
                               std::regex_constants::icase);
    result = std::regex_replace(result, password_regex, "$1=***");
    
    return result;
}
```

---

## 42. 异步API设计 *(v1.3新增)*

### 42.1 异步接口定义

```cpp
namespace OGC {
namespace DB {

/**
 * @brief 异步操作结果
 */
template<typename T>
using AsyncResult = std::future<T>;

/**
 * @brief 异步数据库连接接口
 */
class AsyncDbConnection {
public:
    virtual ~AsyncDbConnection() = default;
    
    /**
     * @brief 异步执行查询
     */
    virtual AsyncResult<std::vector<CNGeometryPtr>> SpatialQueryAsync(
        const std::string& table,
        const std::string& geom_column,
        SpatialOperator op,
        const CNGeometry* query_geom) = 0;
    
    /**
     * @brief 异步批量插入
     */
    virtual AsyncResult<BulkInsertResult> InsertGeometriesAsync(
        const std::string& table,
        const std::string& geom_column,
        const std::vector<CNGeometryPtr>& geometries) = 0;
    
    /**
     * @brief 异步执行SQL
     */
    virtual AsyncResult<DbResult> ExecuteAsync(const std::string& sql) = 0;
    
    /**
     * @brief 取消异步操作
     */
    virtual void CancelAsync(uint64_t operation_id) = 0;
};

/**
 * @brief 异步连接池
 */
class AsyncConnectionPool {
public:
    /**
     * @brief 获取异步连接
     */
    virtual AsyncResult<std::unique_ptr<AsyncDbConnection>> AcquireAsync(
        int timeout_ms = -1) = 0;
};

} // namespace DB
} // namespace OGC
```

### 42.2 异步实现示例

```cpp
AsyncResult<std::vector<CNGeometryPtr>> AsyncDbConnectionImpl::SpatialQueryAsync(
    const std::string& table,
    const std::string& geom_column,
    SpatialOperator op,
    const CNGeometry* query_geom) {
    
    return std::async(std::launch::async, [this, table, geom_column, op, query_geom]() {
        std::vector<CNGeometryPtr> results;
        DbResult result = connection_->SpatialQuery(
            table, geom_column, op, query_geom, results);
        
        if (result != DbResult::kSuccess) {
            throw DbException(result, "Spatial query failed");
        }
        
        return results;
    });
}
```

---

## 43. 总结

本数据库存储模型设计文档提供了一个**生产级**的空间数据持久化解决方案,具有以下核心优势:

### 核心特性

✅ **双数据库支持**: PostGIS 和 SpatiaLite,覆盖主流空间数据库场景  
✅ **标准化格式**: WKB/EWKB/BLOB 序列化,符合 OGC 标准  
✅ **高性能**: 批量插入 (COPY)、空间索引 (GiST/R-Tree)、连接池  
✅ **线程安全**: 每线程独立连接或连接池模式,多线程并发安全  
✅ **跨平台**: 纯 C++11 标准,支持 Windows/Linux/macOS  
✅ **错误处理**: Result 模式 + 异常可选,灵活应对不同场景  
✅ **RAII 资源管理**: 自动连接关闭、资源释放  
✅ **完整测试**: 单元测试、性能测试、集成测试  
✅ **MVCC支持**: 事务隔离级别控制、死锁检测与重试机制  
✅ **空间索引管理**: 索引创建、维护、健康检查完整方案  
✅ **坐标转换**: PROJ库集成,支持跨坐标系空间查询  
✅ **性能监控**: 查询统计、慢查询日志、Prometheus指标导出

### v1.2新增特性

✅ **角色接口设计**: DbConnection拆分为DbConnectionCore、DbTransactionManager、DbSqlExecutor、DbGeometryOperations、DbMetadataOperations  
✅ **批量插入错误恢复**: 分段提交机制、检查点恢复、失败重试  
✅ **连接池高级特性**: 耗尽处理策略、动态扩容、事件回调  
✅ **WKB转换器SIMD优化**: SSE2/SSE4.1/AVX优化路径、运行时CPU特性检测  
✅ **查询计划分析接口**: EXPLAIN ANALYZE解析、优化建议生成  
✅ **数据库连接重连机制**: 自动重连、指数退避、事件监听  
✅ **SRID严格验证**: 多种验证策略、一致性检查工具  
✅ **工作量评估和里程碑规划**: 模块工作量评估、6个里程碑、风险评估  
✅ **测试环境配置**: Docker Compose配置、CI/CD示例、测试配置文件  
✅ **API稳定性承诺**: 语义化版本控制、废弃API迁移指南、版本兼容性矩阵

### v1.3新增特性 *(多角色交叉评审)*

✅ **GeoJSON序列化设计**: GeoJsonConverter支持坐标精度控制、CRS处理、FeatureCollection转换  
✅ **空间索引策略对比**: GiST/BRIN/R-Tree对比表、索引选择决策树、SpatialIndexSelector  
✅ **SRID验证与坐标转换流水线**: GeometryDataPipeline统一数据流设计、验证-转换一体化  
✅ **空间函数兼容性矩阵**: OGC函数到PostGIS/SpatiaLite完整映射、SpatialFunctionMapper  
✅ **连接池预热与性能指标**: Warmup接口、PerformanceMetrics、Prometheus指标导出  
✅ **事务超时处理**: TimeoutTransactionManager、多种超时行为配置  
✅ **SIMD运行时检测与回退**: CpuFeatureDetector、SimdDispatcher函数选择器  
✅ **SQL注入防护增强**: SqlIdentifierValidator标识符验证、GeometryValidator复杂度限制  
✅ **审计日志接口**: AuditLogger、FileAuditLogger、DatabaseAuditLogger实现  
✅ **ABI兼容性与版本控制**: LibraryVersion、PimplBase、VersionedInterface  
✅ **C API绑定**: 完整C API封装、句柄类型定义、跨语言支持  
✅ **错误处理统一策略**: ErrorHandlingStrategy、ErrorContext、错误处理宏  
✅ **连接字符串安全处理**: ConnectionStringParser、密码脱敏、验证接口  
✅ **异步API设计**: AsyncDbConnection、AsyncConnectionPool、Future/Promise模式

### 关键设计

1. **适配器模式**: 统一的数据库抽象层,易于扩展新数据库类型
2. **连接池**: 复用数据库连接,减少连接创建开销,支持健康检查
3. **批量优化**: COPY (PostGIS) 和事务批量插入 (SpatiaLite)
4. **空间索引**: GiST/BRIN (PostGIS) 和 R-Tree (SpatiaLite) 加速空间查询
5. **预编译语句缓存**: 避免 SQL 重复解析
6. **MVCC事务管理**: 支持多种隔离级别,死锁自动重试
7. **坐标转换服务**: 自动处理空间查询中的SRID差异
8. **接口隔离原则**: 角色接口拆分,降低耦合度
9. **错误恢复机制**: 批量插入断点续传,连接自动重连

### 性能指标

| 操作 | PostGIS | SpatiaLite |
|------|---------|------------|
| 批量插入 (100万行) | ~45 秒 (COPY) | ~2 分钟 (事务) |
| 空间查询 (10万行) | ~50 ms (GiST) | ~80 ms (R-Tree) |
| 连接获取 (连接池) | < 1 ms | < 1 ms |
| 坐标转换 (1000点) | ~5 ms | ~5 ms |
| WKB转换 (SIMD) | ~2x 加速 | ~2x 加速 |

### 后续工作

1. 实现 Oracle Spatial 和 SQL Server Spatial 适配器
2. 添加连接池动态调整策略
3. 实现分布式事务支持
4. 优化大规模数据导入导出
5. 编写详细 API 文档和用户手册
6. 添加更多曲线几何类型支持

---

**文档版本**: v1.3  
**最后更新**: 2026年3月18日  
**维护者**: Database Storage Team
