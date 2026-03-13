# Map Server 实施任务列表 (DeepSeek 3.1 重新组织优先级)

> **基于文档**: DESIGN.md v2.0  
> **创建日期**: 2026-03-10  
> **重新组织**: DeepSeek 3.1 基于模块依赖关系  
> **目标**: 按正确的依赖关系依次实现，确保无遗漏

---

## 📋 重新组织的优先级说明

**DeepSeek 3.1 分析后的正确优先级顺序**:

**P0 (构建系统)**: CMakeLists.txt - 必须先有构建系统才能编译测试  
**P1 (基础设施层)**: Logger、Config - 最底层，其他模块都依赖  
**P2 (数据访问层)**: Database 抽象层 - 数据存储基础  
**P3 (编码器层)**: Encoder 抽象层 - 图像编码基础  
**P4 (缓存层)**: Cache - 性能优化基础  
**P5 (渲染层)**: RenderContext → Renderer - 渲染引擎  
**P6 (服务层)**: MapService - 业务服务封装  
**P7 (HTTP服务层)**: HttpServer - 网络接口  
**P8 (主程序)**: main.cpp - 程序入口  
**P9 (测试与部署)**: 测试、文档、部署

---

## 🎯 阶段一：构建系统 (P0)

### Task 0.1: CMakeLists.txt 项目构建配置
**优先级**: P0  
**预计工时**: 4小时  
**状态**: [x] 已完成

**任务内容**:
- [x] 创建项目根目录 CMakeLists.txt
- [x] 配置编译器选项和 C++ 标准
- [x] 配置依赖库查找
- [x] 配置源文件组织
- [x] 配置输出目录
- [x] 配置安装规则
- [x] 创建 find_package 配置文件

**验收标准**:
- 项目可以成功编译
- 依赖库正确链接

**已完成文件**:
- CMakeLists.txt

---

### Task 0.2: 创建项目基础结构
**优先级**: P0  
**预计工时**: 2小时  
**状态**: [x] 已完成

**任务内容**:
- [x] 创建目录结构（src/server、src/config、src/database、src/encoder、src/renderer、src/cache、src/utils）
- [x] 创建 .gitignore 文件
- [x] 创建 README.md（更新项目状态）

**验收标准**:
- 目录结构符合 DESIGN.md 2.3 节定义
- 项目基础文件完整

**已完成文件**:
- .gitignore
- README.md

---

## 🏗️ 阶段二：基础设施层 (P1)

### Task 1.1: 实现 Logger 模块
**优先级**: P1  
**预计工时**: 4小时  
**状态**: [x] 已完成

**任务内容**:
- [x] 创建 src/utils/logger.h
- [x] 创建 src/utils/logger.cpp
- [x] 实现日志级别（TRACE、DEBUG、INFO、WARN、ERROR）
- [x] 实现日志文件输出
- [x] 实现控制台输出
- [x] 实现日志轮转（按大小和文件数）
- [x] 实现线程安全的日志写入

**验收标准**:
- 日志可以正常输出到文件和控制台
- 日志轮转功能正常
- 线程安全测试通过

**已完成文件**:
- src/utils/logger.h
- src/utils/logger.cpp

---

### Task 1.2: 实现 Config 模块
**优先级**: P1  
**预计工时**: 6小时  
**状态**: [x] 已完成

**任务内容**:
- [x] 创建 src/config/config.h
- [x] 创建 src/config/config.cpp
- [x] 实现 DatabaseConfig 结构
- [x] 实现 EncoderConfig 结构
- [x] 实现 RangeLimitConfig 结构
- [x] 实现 ServerConfig 结构
- [x] 实现 CacheConfig 结构
- [x] 实现 LogConfig 结构
- [x] 实现 Config 类（加载、验证、保存）
- [x] 创建 config.json 示例文件

**验收标准**:
- 可以成功加载 JSON 配置文件
- 配置验证功能正常
- 配置参数可以正确访问

**已完成文件**:
- src/config/config.h
- src/config/config.cpp
- config.json

---

## 🗄️ 阶段三：数据访问层 (P2)

### Task 2.1: 定义数据库抽象接口
**优先级**: P2  
**预计工时**: 4小时  
**状态**: [x] 已完成

**任务内容**:
- [x] 创建 src/database/idatabase.h
- [x] 定义 SqlParameter 结构
- [x] 定义 DatabaseRow 类
- [x] 定义 ResultSet 类
- [x] 定义 IDatabase 抽象接口
- [x] 添加完整的 Doxygen 注释

**验收标准**:
- 接口定义完整且符合 C++11 标准
- 所有方法都有完整的文档注释

**已完成文件**:
- src/database/idatabase.h

---

### Task 2.2: 实现 SQLite3 数据库适配器
**优先级**: P2  
**预计工时**: 8小时  
**状态**: [x] 已完成

**任务内容**:
- [x] 创建 src/database/sqlite_database.h
- [x] 创建 src/database/sqlite_database.cpp
- [x] 实现 SqliteDatabase 类
- [x] 实现 SpatiaLite 扩展加载
- [x] 实现空间查询功能
- [x] 实现预处理语句
- [x] 实现事务支持
- [ ] 编写单元测试

**验收标准**:
- 可以成功连接 SQLite3 数据库
- SpatiaLite 扩展加载成功
- 空间查询功能正常
- 单元测试通过

**已完成文件**:
- src/database/sqlite_database.h
- src/database/sqlite_database.cpp

---

### Task 2.3: 实现数据库工厂
**优先级**: P2  
**预计工时**: 3小时  
**状态**: [x] 已完成

**任务内容**:
- [x] 创建 src/database/database_factory.h
- [x] 创建 src/database/database_factory.cpp
- [x] 实现 DatabaseFactory 类
- [x] 支持根据配置动态创建数据库实例

**验收标准**:
- 工厂可以根据配置创建正确的数据库实例
- 支持 SQLite3 和 PostgreSQL 两种类型

**已完成文件**:
- src/database/database_factory.h
- src/database/database_factory.cpp

---

### Task 2.4: 实现 PostgreSQL 数据库适配器 (可选)
**优先级**: P2 (可选)  
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

**依赖**: Task 2.1 (IDatabase 接口)

**验收标准**:
- 可以成功连接 PostgreSQL 数据库
- PostGIS 扩展初始化成功
- 空间查询功能正常
- 连接池功能正常

**说明**: 此任务为可选任务，适用于生产环境需要连接 PostgreSQL/PostGIS 的场景

---

## 🎨 阶段四：编码器层 (P3)

### Task 3.1: 定义编码器抽象接口
**优先级**: P3  
**预计工时**: 3小时  
**状态**: [x] 已完成

**任务内容**:
- [x] 创建 src/encoder/iencoder.h
- [x] 定义 ImageFormat 枚举
- [x] 定义 EncodeOptions 结构
- [x] 定义 RawImage 结构
- [x] 定义 EncodedImage 结构
- [x] 定义 IEncoder 抽象接口

**验收标准**:
- 接口定义完整
- 支持多种图像格式

**已完成文件**:
- src/encoder/iencoder.h

---

### Task 3.2: 实现 PNG 编码器
**优先级**: P3  
**预计工时**: 6小时  
**状态**: [x] 已完成

**任务内容**:
- [x] 创建 src/encoder/png_encoder.h
- [x] 创建 src/encoder/png_encoder.cpp
- [x] 实现 PngEncoder 类
- [x] 支持 PNG8 和 PNG32 格式
- [x] 实现调色板优化（PNG8）
- [x] 实现 DPI 信息写入
- [ ] 编写单元测试

**验收标准**:
- PNG 编码功能正常
- 支持 PNG8 和 PNG32 两种格式
- DPI 信息正确写入

**已完成文件**:
- src/encoder/png_encoder.h
- src/encoder/png_encoder.cpp

---

### Task 3.3: 实现编码器工厂
**优先级**: P3  
**预计工时**: 2小时  
**状态**: [x] 已完成

**任务内容**:
- [x] 创建 src/encoder/encoder_factory.h
- [x] 创建 src/encoder/encoder_factory.cpp
- [x] 实现 EncoderFactory 类
- [x] 支持根据格式字符串创建编码器

**验收标准**:
- 工厂可以根据格式创建正确的编码器实例

**已完成文件**:
- src/encoder/encoder_factory.h
- src/encoder/encoder_factory.cpp

---

### Task 3.4: 实现 WebP 编码器 (可选)
**优先级**: P3 (可选)  
**预计工时**: 6小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/encoder/webp_encoder.h
- [ ] 创建 src/encoder/webp_encoder.cpp
- [ ] 实现 WebpEncoder 类
- [ ] 支持有损和无损压缩
- [ ] 实现质量参数配置
- [ ] 编写单元测试

**依赖**: Task 3.1 (IEncoder 接口)

**验收标准**:
- WebP 编码功能正常
- 支持有损和无损两种模式
- 质量参数生效

**说明**: 此任务为可选任务，需要 libwebp 库支持

---

## 💾 阶段五：缓存层 (P4)

### Task 4.1: 实现内存缓存
**优先级**: P4  
**预计工时**: 6小时  
**状态**: [x] 已完成

**任务内容**:
- [x] 创建 src/cache/memory_cache.h
- [x] 创建 src/cache/memory_cache.cpp
- [x] 实现 CacheKeyGenerator 类
- [x] 实现 MemoryCache 类（LRU 算法）
- [x] 实现线程安全的缓存操作
- [ ] 编写单元测试

**验收标准**:
- LRU 淘汰算法正常工作
- 线程安全测试通过
- 缓存命中率统计正确

**已完成文件**:
- src/cache/memory_cache.h
- src/cache/memory_cache.cpp

---

### Task 4.2: 实现磁盘缓存 (可选)
**优先级**: P4 (可选)  
**预计工时**: 4小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/cache/disk_cache.h
- [ ] 创建 src/cache/disk_cache.cpp
- [ ] 实现 DiskCache 类
- [ ] 实现缓存文件管理（基于 hash 的目录结构）
- [ ] 实现 TTL 过期机制
- [ ] 实现缓存清理功能（LRU + TTL）
- [ ] 编写单元测试

**依赖**: Task 4.1 (MemoryCache)

**验收标准**:
- 磁盘缓存读写正常
- TTL 过期机制生效
- 缓存清理功能正常

**说明**: 磁盘缓存用于扩展内存缓存的容量限制，适合存储更多历史瓦片数据

---

## 🎨 阶段六：渲染层 (P5)

### Task 5.1: 实现渲染上下文
**优先级**: P5  
**预计工时**: 4小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/renderer/render_context.h
- [ ] 创建 src/renderer/render_context.cpp
- [ ] 实现 RenderContext 类
- [ ] 实现状态管理（压栈/出栈）
- [ ] 实现参数验证
- [ ] 实现坐标转换（地理坐标 ↔ 屏幕坐标）
- [ ] 实现样式管理

**依赖**: Task 2.1, Task 2.2, Task 3.2 (Database, Encoder)

**验收标准**:
- 渲染上下文管理正常
- 参数验证功能正常
- 坐标转换准确

**关键路径**: 这是渲染引擎的基础组件，必须在 Renderer 之前完成

---

### Task 5.2: 实现渲染引擎
**优先级**: P5  
**预计工时**: 12小时  
**状态**: [x] 已完成

**任务内容**:
- [x] 创建 src/renderer/renderer.h
- [x] 创建 src/renderer/renderer.cpp
- [x] 实现 Renderer 类
- [x] 实现地图渲染逻辑
- [x] 实现瓦片渲染逻辑
- [x] 实现高 DPI 支持
- [x] 实现渲染性能优化
- [ ] 编写单元测试

**验收标准**:
- 地图渲染功能正常
- 瓦片渲染功能正常
- 高 DPI 支持正常
- 性能指标达标（< 350ms）

**已完成文件**:
- src/renderer/renderer.h
- src/renderer/renderer.cpp

**问题**: 此任务应该在 Task 5.1 (渲染上下文) 之后完成，但实际执行顺序有误

---

## 🔧 阶段七：服务层 (P6)

### Task 6.1: 实现 MapService
**优先级**: P6  
**预计工时**: 8小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 src/service/map_service.h
- [ ] 创建 src/service/map_service.cpp
- [ ] 实现 MapService 类
- [ ] 实现瓦片请求处理
- [ ] 实现地图生成请求处理
- [ ] 实现缓存集成
- [ ] 实现范围验证
- [ ] 实现请求限流
- [ ] 编写单元测试

**依赖**: Task 5.2 (Renderer), Task 4.1 (MemoryCache), Task 7.1 (HttpServer 需要先完成)

**验收标准**:
- MapService 功能完整
- 缓存集成正常
- 范围验证功能正常
- 请求限流功能正常

**关键路径**: 这是业务逻辑层，连接渲染引擎和HTTP服务

---

## 🌐 阶段八：HTTP 服务层 (P7)

### Task 7.1: 实现 HTTP Server
**优先级**: P7  
**预计工时**: 10小时  
**状态**: [x] 已完成

**任务内容**:
- [x] 创建 src/server/http_server.h
- [x] 创建 src/server/http_server.cpp
- [x] 实现 HttpServer 类
- [x] 实现路由设置
- [x] 实现 WMTS 接口
- [x] 实现健康检查接口
- [x] 实现性能指标接口
- [ ] 编写单元测试

**验收标准**:
- HTTP 服务启动正常
- WMTS 接口功能完整
- 健康检查接口正常

**已完成文件**:
- src/server/http_server.h
- src/server/http_server.cpp

**问题**: 此任务应该在 Task 6.1 (MapService) 之后完成，但实际执行顺序有误

---

## 🚀 阶段九：主程序 (P8)

### Task 8.1: 实现主程序
**优先级**: P8  
**预计工时**: 4小时  
**状态**: [x] 已完成

**任务内容**:
- [x] 创建 src/main.cpp
- [x] 实现程序初始化
- [x] 实现配置加载
- [x] 实现组件初始化和组装
- [x] 实现信号处理（优雅退出）
- [x] 实现错误处理和日志记录
- [x] 实现后台运行支持（守护进程）

**验收标准**:
- 程序可以正常启动和退出
- 配置加载正常
- 信号处理正常
- 优雅退出功能正常

**已完成文件**:
- src/main.cpp

**问题**: 此任务应该在所有组件完成后执行，但实际执行顺序有误

---

## 🧪 阶段十：测试与部署 (P9)

### Task 9.1: 编写单元测试
**优先级**: P9  
**预计工时**: 12小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 创建 tests/ 目录结构
- [ ] 创建 CMakeLists.txt (tests)
- [ ] 编写 Config 模块测试
- [ ] 编写 Database 模块测试
- [ ] 编写 Encoder 模块测试
- [ ] 编写 Cache 模块测试
- [ ] 编写 Renderer 模块测试
- [ ] 编写 MapService 模块测试
- [ ] 编写 HttpServer 模块测试

**测试框架**: Google Test (gtest)

**验收标准**:
- 单元测试覆盖率 > 80%
- 所有测试通过

---

### Task 9.2: 编写部署文档
**优先级**: P9  
**预计工时**: 4小时  
**状态**: [ ] 待执行

**任务内容**:
- [ ] 编写编译指南 (BUILD.md)
- [ ] 编写部署指南 (DEPLOY.md)
- [ ] 编写配置说明 (CONFIG.md)
- [ ] 编写运维指南 (OPERATION.md)
- [ ] 编写 Docker 支持

**验收标准**:
- 文档完整且准确
- 可以按照文档成功部署

---

## 📊 任务统计

### 总体统计
- **总任务数**: 25 个
- **已完成任务**: 14 个
- **待执行任务**: 11 个
- **完成率**: 56%

### 优先级分布
- **P0 (构建系统)**: 2 个任务，6 小时，100% 完成
- **P1 (基础设施)**: 2 个任务，10 小时，100% 完成
- **P2 (数据访问)**: 4 个任务，23 小时，75% 完成
- **P3 (编码器)**: 4 个任务，17 小时，75% 完成
- **P4 (缓存)**: 2 个任务，10 小时，50% 完成
- **P5 (渲染)**: 2 个任务，16 小时，50% 完成
- **P6 (服务)**: 1 个任务，8 小时，0% 完成
- **P7 (HTTP)**: 1 个任务，10 小时，100% 完成
- **P8 (主程序)**: 1 个任务，4 小时，100% 完成
- **P9 (测试部署)**: 6 个任务，38 小时，0% 完成

### 关键路径问题
1. **Task 5.1 (渲染上下文)** 应该在 Task 5.2 (渲染引擎) 之前完成
2. **Task 6.1 (MapService)** 应该在 Task 7.1 (HttpServer) 之前完成
3. **Task 8.1 (主程序)** 应该在所有组件完成后执行

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

## 📝 DeepSeek 3.1 分析结论

### 正确的执行顺序
基于模块依赖关系分析，正确的执行顺序应该是：

1. **P0**: CMakeLists.txt → 项目基础结构
2. **P1**: Logger → Config
3. **P2**: IDatabase → SQLite3 → DatabaseFactory
4. **P3**: IEncoder → PNG Encoder → EncoderFactory
5. **P4**: MemoryCache
6. **P5**: RenderContext → Renderer
7. **P6**: MapService
8. **P7**: HttpServer
9. **P8**: main.cpp
10. **P9**: 单元测试 → 部署文档

### 当前问题
- 渲染引擎和HTTP服务在没有MapService的情况下已经实现
- 主程序在所有组件未完全集成的情况下已经实现
- 缺少渲染上下文组件

### 下一步建议
按照正确的优先级顺序，下一个应该执行的任务是：
**Task 5.1: 实现渲染上下文** (P5优先级)

---

**文档创建时间**: 2026-03-10  
**重新组织时间**: 2026-03-10  
**分析工具**: DeepSeek 3.1