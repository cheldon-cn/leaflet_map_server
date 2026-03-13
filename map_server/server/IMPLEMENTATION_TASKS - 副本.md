# Map Server 实施任务列表

> **基于文档**: DESIGN.md v2.0  
> **创建日期**: 2026-03-09  
> **目标**: 按优先级依次实现，达成项目目标

---

## 📋 任务优先级说明

根据模块依赖关系，任务按以下优先级排序：

**P0 (基础设施层)**: Logger、Config - 最底层，其他模块都依赖  
**P1 (数据访问层)**: Database 抽象层 - 数据存储基础  
**P2 (编码器层)**: Encoder 抽象层 - 图像编码基础  
**P3 (缓存层)**: Cache - 性能优化基础  
**P4 (渲染层)**: Renderer - 核心业务逻辑  
**P5 (服务层)**: MapService - 业务服务封装  
**P6 (HTTP服务层)**: HttpServer - 网络接口  
**P7 (主程序)**: main.cpp - 程序入口  
**P8 (测试与部署)**: 测试、文档、部署

---

## 🎯 阶段一：基础设施层 (P0)

### Task 1.1: 创建项目基础结构
**优先级**: P0  
**预计工时**: 2小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建目录结构（src/server、src/config、src/database、src/encoder、src/renderer、src/cache、src/utils）
- [ ] 创建 CMakeLists.txt（项目根目录）
- [ ] 创建 .gitignore 文件
- [ ] 创建 README.md（更新项目状态）

**验收标准**:
- 目录结构符合 DESIGN.md 2.3 节定义
- CMakeLists.txt 可以成功配置项目

---

### Task 1.2: 实现 Logger 模块
**优先级**: P0  
**预计工时**: 4小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/utils/logger.h
- [ ] 创建 src/utils/logger.cpp
- [ ] 实现日志级别（TRACE、DEBUG、INFO、WARN、ERROR）
- [ ] 实现日志文件输出
- [ ] 实现控制台输出
- [ ] 实现日志轮转（按大小和文件数）
- [ ] 实现线程安全的日志写入

**关键代码**:
```cpp
namespace cycle {

enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4
};

class Logger {
public:
    static void init(const LogConfig& config);
    static void trace(const std::string& message);
    static void debug(const std::string& message);
    static void info(const std::string& message);
    static void warn(const std::string& message);
    static void error(const std::string& message);
    
private:
    static void log(LogLevel level, const std::string& message);
    static void rotateLogs();
    
    static std::ofstream logFile_;
    static LogLevel minLevel_;
    static bool consoleOutput_;
};

} // namespace cycle
```

**验收标准**:
- 日志可以正常输出到文件和控制台
- 日志轮转功能正常
- 线程安全测试通过

---

### Task 1.3: 实现 Config 模块
**优先级**: P0  
**预计工时**: 6小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/config/config.h
- [ ] 创建 src/config/config.cpp
- [ ] 实现 DatabaseConfig 结构
- [ ] 实现 EncoderConfig 结构
- [ ] 实现 RangeLimitConfig 结构
- [ ] 实现 ServerConfig 结构
- [ ] 实现 CacheConfig 结构
- [ ] 实现 LogConfig 结构
- [ ] 实现 Config 类（加载、验证、保存）
- [ ] 创建 config.json 示例文件

**关键代码**:
```cpp
namespace cycle {

class Config {
public:
    DatabaseConfig database;
    EncoderConfig encoder;
    RangeLimitConfig range_limit;
    ServerConfig server;
    CacheConfig cache;
    LogConfig log;
    
    bool loadFromFile(const std::string& path);
    bool validate() const;
    bool saveToFile(const std::string& path) const;
    
private:
    std::string config_path_;
};

} // namespace cycle
```

**验收标准**:
- 可以成功加载 JSON 配置文件
- 配置验证功能正常
- 配置参数可以正确访问

---

## 🗄️ 阶段二：数据访问层 (P1)

### Task 2.1: 定义数据库抽象接口
**优先级**: P1  
**预计工时**: 4小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/database/idatabase.h
- [ ] 定义 SqlParameter 结构
- [ ] 定义 DatabaseRow 类
- [ ] 定义 ResultSet 类
- [ ] 定义 IDatabase 抽象接口
- [ ] 添加完整的 Doxygen 注释

**关键代码**:
```cpp
namespace cycle {
namespace database {

class IDatabase {
public:
    virtual ~IDatabase() = default;
    
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual bool isOpen() const = 0;
    
    virtual bool execute(const std::string& sql) = 0;
    virtual bool execute(const std::string& sql, 
                        const std::vector<SqlParameter>& params) = 0;
    
    virtual std::unique_ptr<ResultSet> query(const std::string& sql) = 0;
    virtual std::unique_ptr<ResultSet> query(
        const std::string& sql, 
        const std::vector<SqlParameter>& params) = 0;
    
    virtual bool beginTransaction() = 0;
    virtual bool commitTransaction() = 0;
    virtual bool rollbackTransaction() = 0;
    
    virtual std::string getLastError() const = 0;
    virtual int getLastErrorCode() const = 0;
    
    virtual std::unique_ptr<ResultSet> querySpatial(
        const std::string& table,
        const Envelope& envelope,
        const std::string& geometryColumn = "geometry") = 0;
};

} // namespace database
} // namespace cycle
```

**验收标准**:
- 接口定义完整且符合 C++11 标准
- 所有方法都有完整的文档注释

---

### Task 2.2: 实现 SQLite3 数据库适配器
**优先级**: P1  
**预计工时**: 8小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/database/sqlite_database.h
- [ ] 创建 src/database/sqlite_database.cpp
- [ ] 实现 SqliteDatabase 类
- [ ] 实现 SpatiaLite 扩展加载
- [ ] 实现空间查询功能
- [ ] 实现预处理语句
- [ ] 实现事务支持
- [ ] 编写单元测试

**关键代码**:
```cpp
namespace cycle {
namespace database {

class SqliteDatabase : public IDatabase {
public:
    explicit SqliteDatabase(const std::string& dbPath);
    ~SqliteDatabase() override;
    
    bool open() override;
    void close() override;
    bool isOpen() const override;
    
    std::unique_ptr<ResultSet> querySpatial(
        const std::string& table,
        const Envelope& envelope,
        const std::string& geometryColumn) override;
    
private:
    bool initSpatialite();
    
    sqlite3* db_;
    std::string dbPath_;
    bool hasSpatialite_;
};

} // namespace database
} // namespace cycle
```

**验收标准**:
- 可以成功连接 SQLite3 数据库
- SpatiaLite 扩展加载成功
- 空间查询功能正常
- 单元测试通过

---

### Task 2.3: 实现 PostgreSQL 数据库适配器
**优先级**: P1  
**预计工时**: 8小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/database/postgresql_database.h
- [ ] 创建 src/database/postgresql_database.cpp
- [ ] 实现 PostgresqlDatabase 类
- [ ] 实现 PostGIS 扩展初始化
- [ ] 实现空间查询功能
- [ ] 实现连接池
- [ ] 编写单元测试

**验收标准**:
- 可以成功连接 PostgreSQL 数据库
- PostGIS 扩展初始化成功
- 空间查询功能正常
- 连接池功能正常

---

### Task 2.4: 实现数据库工厂
**优先级**: P1  
**预计工时**: 3小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/database/database_factory.h
- [ ] 创建 src/database/database_factory.cpp
- [ ] 实现 DatabaseFactory 类
- [ ] 支持根据配置动态创建数据库实例

**验收标准**:
- 工厂可以根据配置创建正确的数据库实例
- 支持 SQLite3 和 PostgreSQL 两种类型

---

## 🎨 阶段三：编码器层 (P2)

### Task 3.1: 定义编码器抽象接口
**优先级**: P2  
**预计工时**: 3小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/encoder/iencoder.h
- [ ] 定义 ImageFormat 枚举
- [ ] 定义 EncodeOptions 结构
- [ ] 定义 RawImage 结构
- [ ] 定义 EncodedImage 结构
- [ ] 定义 IEncoder 抽象接口

**关键代码**:
```cpp
namespace cycle {
namespace encoder {

enum class ImageFormat {
    PNG8,
    PNG32,
    WEBP
};

class IEncoder {
public:
    virtual ~IEncoder() = default;
    
    virtual EncodedImage encode(const RawImage& image,
                                const EncodeOptions& options) = 0;
    
    virtual ImageFormat getSupportedFormat() const = 0;
    virtual std::string getMimeType() const = 0;
};

} // namespace encoder
} // namespace cycle
```

**验收标准**:
- 接口定义完整
- 支持多种图像格式

---

### Task 3.2: 实现 PNG 编码器
**优先级**: P2  
**预计工时**: 6小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/encoder/png_encoder.h
- [ ] 创建 src/encoder/png_encoder.cpp
- [ ] 实现 PngEncoder 类
- [ ] 支持 PNG8 和 PNG32 格式
- [ ] 实现调色板优化（PNG8）
- [ ] 实现 DPI 信息写入
- [ ] 编写单元测试

**验收标准**:
- PNG 编码功能正常
- 支持 PNG8 和 PNG32 两种格式
- DPI 信息正确写入

---

### Task 3.3: 实现 WebP 编码器
**优先级**: P2  
**预计工时**: 6小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/encoder/webp_encoder.h
- [ ] 创建 src/encoder/webp_encoder.cpp
- [ ] 实现 WebpEncoder 类
- [ ] 支持有损和无损压缩
- [ ] 实现质量参数配置
- [ ] 编写单元测试

**验收标准**:
- WebP 编码功能正常
- 支持有损和无损两种模式
- 质量参数生效

---

### Task 3.4: 实现编码器工厂
**优先级**: P2  
**预计工时**: 2小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/encoder/encoder_factory.h
- [ ] 创建 src/encoder/encoder_factory.cpp
- [ ] 实现 EncoderFactory 类
- [ ] 支持根据格式字符串创建编码器

**验收标准**:
- 工厂可以根据格式创建正确的编码器实例

---

## 💾 阶段四：缓存层 (P3)

### Task 4.1: 实现内存缓存
**优先级**: P3  
**预计工时**: 6小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/cache/cache.h
- [ ] 创建 src/cache/cache.cpp
- [ ] 实现 CacheKeyGenerator 类
- [ ] 实现 MemoryCache 类（LRU 算法）
- [ ] 实现线程安全的缓存操作
- [ ] 编写单元测试

**关键代码**:
```cpp
namespace cycle {

class MemoryCache {
public:
    explicit MemoryCache(size_t maxSizeBytes);
    
    bool get(const std::string& key, std::vector<uint8_t>& value);
    void put(const std::string& key, const std::vector<uint8_t>& value);
    void remove(const std::string& key);
    void clear();
    
    size_t getSize() const;
    size_t getMaxSize() const;
    
private:
    void evictIfNeeded();
    
    std::unordered_map<std::string, std::vector<uint8_t>> cache_;
    std::list<std::string> lruList_;
    std::unordered_map<std::string, std::list<std::string>::iterator> lruMap_;
    size_t currentSize_;
    size_t maxSize_;
    mutable std::mutex mutex_;
};

} // namespace cycle
```

**验收标准**:
- LRU 淘汰算法正常工作
- 线程安全测试通过
- 缓存命中率统计正确

---

### Task 4.2: 实现磁盘缓存
**优先级**: P3  
**预计工时**: 4小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 实现 DiskCache 类
- [ ] 实现缓存文件管理
- [ ] 实现 TTL 过期机制
- [ ] 实现缓存清理功能

**验收标准**:
- 磁盘缓存读写正常
- TTL 过期机制生效
- 缓存清理功能正常

---

## 🎨 阶段五：渲染层 (P4)

### Task 5.1: 实现渲染上下文
**优先级**: P4  
**预计工时**: 4小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/renderer/render_context.h
- [ ] 创建 src/renderer/render_context.cpp
- [ ] 实现 RenderContext 类
- [ ] 实现状态管理（压栈/出栈）
- [ ] 实现参数验证
- [ ] 实现坐标转换

**验收标准**:
- 渲染上下文管理正常
- 参数验证功能正常

---

### Task 5.2: 实现渲染引擎
**优先级**: P4  
**预计工时**: 12小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/renderer/renderer.h
- [ ] 创建 src/renderer/renderer.cpp
- [ ] 实现 Renderer 类
- [ ] 实现地图渲染逻辑
- [ ] 实现瓦片渲染逻辑
- [ ] 实现高 DPI 支持
- [ ] 实现渲染性能优化
- [ ] 编写单元测试

**关键代码**:
```cpp
namespace cycle {

class Renderer {
public:
    Renderer(std::shared_ptr<IDatabase> db,
             std::shared_ptr<IEncoder> encoder);
    
    std::vector<uint8_t> renderMap(const RenderRequest& request);
    std::vector<uint8_t> renderTile(int z, int x, int y);
    
private:
    bool renderLayer(const Layer& layer, ImageBuffer& buffer);
    void applyStyle(const StyleConfig& style, ImageBuffer& buffer);
    
    std::shared_ptr<IDatabase> database_;
    std::shared_ptr<IEncoder> encoder_;
};

} // namespace cycle
```

**验收标准**:
- 地图渲染功能正常
- 瓦片渲染功能正常
- 高 DPI 支持正常
- 性能指标达标（< 350ms）

---

## 🔧 阶段六：服务层 (P5)

### Task 6.1: 实现 MapService
**优先级**: P5  
**预计工时**: 8小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/service/map_service.h
- [ ] 创建 src/service/map_service.cpp
- [ ] 实现 MapService 类
- [ ] 实现瓦片请求处理
- [ ] 实现缓存集成
- [ ] 实现范围验证
- [ ] 编写单元测试

**验收标准**:
- MapService 功能完整
- 缓存集成正常
- 范围验证功能正常

---

## 🌐 阶段七：HTTP 服务层 (P6)

### Task 7.1: 实现 HTTP Server
**优先级**: P6  
**预计工时**: 10小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/server/http_server.h
- [ ] 创建 src/server/http_server.cpp
- [ ] 实现 HttpServer 类
- [ ] 实现路由设置
- [ ] 实现 WMTS 接口
- [ ] 实现健康检查接口
- [ ] 实现性能指标接口
- [ ] 编写单元测试

**关键代码**:
```cpp
namespace cycle {

class HttpServer {
public:
    HttpServer(const Config& config);
    ~HttpServer();
    
    bool start();
    void stop();
    bool isRunning() const;
    
private:
    void setupRoutes();
    void handleGetTile(const httplib::Request& req, httplib::Response& res);
    void handleGetCapabilities(const httplib::Request& req, httplib::Response& res);
    void handleHealth(const httplib::Request& req, httplib::Response& res);
    
    Config config_;
    std::unique_ptr<httplib::Server> server_;
    std::atomic<bool> running_;
};

} // namespace cycle
```

**验收标准**:
- HTTP 服务启动正常
- WMTS 接口功能完整
- 健康检查接口正常

---

## 🚀 阶段八：主程序与部署 (P7-P8)

### Task 8.1: 实现主程序
**优先级**: P7  
**预计工时**: 4小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/main.cpp
- [ ] 实现程序初始化
- [ ] 实现配置加载
- [ ] 实现信号处理（优雅退出）
- [ ] 实现错误处理

**验收标准**:
- 程序可以正常启动和退出
- 配置加载正常
- 信号处理正常

---

### Task 8.2: 编写单元测试
**优先级**: P8  
**预计工时**: 12小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 tests/ 目录
- [ ] 编写 Config 模块测试
- [ ] 编写 Database 模块测试
- [ ] 编写 Encoder 模块测试
- [ ] 编写 Cache 模块测试
- [ ] 编写 Renderer 模块测试
- [ ] 编写 MapService 模块测试
- [ ] 编写 HttpServer 模块测试

**验收标准**:
- 单元测试覆盖率 > 80%
- 所有测试通过

---

### Task 8.3: 编写部署文档
**优先级**: P8  
**预计工时**: 4小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 编写编译指南
- [ ] 编写部署指南
- [ ] 编写配置说明
- [ ] 编写运维指南

**验收标准**:
- 文档完整且准确
- 可以按照文档成功部署

---

## 📊 任务统计

### 总体统计
- **总任务数**: 22 个
- **预计总工时**: 约 120 小时（15 个工作日）
- **关键路径**: Logger → Config → Database → Encoder → Cache → Renderer → MapService → HttpServer → main.cpp

### 优先级分布
- **P0 (基础设施)**: 3 个任务，12 小时
- **P1 (数据访问)**: 4 个任务，23 小时
- **P2 (编码器)**: 4 个任务，17 小时
- **P3 (缓存)**: 2 个任务，10 小时
- **P4 (渲染)**: 2 个任务，16 小时
- **P5 (服务)**: 1 个任务，8 小时
- **P6 (HTTP)**: 1 个任务，10 小时
- **P7 (主程序)**: 1 个任务，4 小时
- **P8 (测试部署)**: 3 个任务，20 小时

---

## ✅ 验收标准

### 功能验收
- [ ] 所有核心功能实现完整
- [ ] 所有 API 接口功能正常
- [ ] 所有性能指标达标

### 质量验收
- [ ] 单元测试覆盖率 > 80%
- [ ] 无内存泄漏
- [ ] 无严重安全漏洞

### 文档验收
- [ ] 代码文档完整
- [ ] 部署文档完整
- [ ] API 文档完整

---

**文档创建时间**: 2026-03-09  
**最后更新时间**: 2026-03-09  
**维护者**: AI Assistant
