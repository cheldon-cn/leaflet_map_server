# Map Server - C++11 Implementation

基于图片流的地图服务，使用cpp-httplib构建HTTP服务，集成PNG生成模块，支持实时渲染并返回PNG图像流。

## 🎯 项目目标

- **高性能地图渲染**：支持实时生成高质量PNG地图图像
- **RESTful API服务**：提供标准化的HTTP接口
- **多数据源支持**：集成SQLite空间数据库
- **可扩展架构**：支持插件式图层和渲染器
- **生产就绪**：线程安全、配置灵活、监控完善

## 功能特性

### 核心功能
- **HTTP REST API**：提供地图渲染端点，支持JSON请求格式
- **实时PNG生成**：集成Qt图像库生成高质量PNG图像
- **跨平台支持**：支持Windows/Linux/macOS部署
- **多线程安全**：线程安全的渲染引擎和缓存机制
- **可配置输出**：生成的文件保存到指定目录结构

### 高级特性
- **瓦片缓存**：支持内存和磁盘两级缓存
- **图层管理**：动态加载和卸载地图图层
- **空间查询**：基于SQLite的空间数据查询
- **样式配置**：可配置的地图样式和颜色方案
- **监控指标**：内置性能监控和健康检查

## 🚀 快速开始

### 前置条件

#### 必需依赖
- **CMake 3.10+**：构建系统
- **C++11兼容编译器**：GCC 7+/Clang 5+/MSVC 2017+
- **SQLite3开发库**：空间数据存储
- **libpng开发库**：PNG图像编码

#### 可选依赖
- **Qt 5.12+开发库**：推荐用于高质量图像渲染
- **cpp-httplib**：通过CMake自动下载

#### 各平台安装指南

**Ubuntu/Debian**
```bash
sudo apt-get update
sudo apt-get install cmake g++ libsqlite3-dev libpng-dev
# 可选：Qt5
sudo apt-get install qtbase5-dev libqt5gui5
```

**CentOS/RHEL**
```bash
sudo yum install cmake gcc-c++ sqlite-devel libpng-devel
# 可选：Qt5
sudo yum install qt5-qtbase-devel
```

**macOS**
```bash
brew install cmake sqlite libpng
# 可选：Qt5
brew install qt
```

**Windows**
- 安装 Visual Studio 2017+ 或 MinGW-w64
- 使用 vcpkg 安装依赖：
```bash
vcpkg install sqlite3 libpng
vcpkg install qt5-base
```

### 5分钟快速体验

1. **克隆项目**（假设已有源代码）
2. **配置环境变量**（可选）
3. **构建项目**
4. **启动服务**
5. **测试API**

详细步骤见后续章节。

## 构建步骤

**注意**：当前项目缺少 CMakeLists.txt 和源代码文件，以下为假设项目结构完整时的构建步骤：

```bash
# 在项目根目录
mkdir build
cd build
cmake ..
cmake --build . --target map_server
```

或者直接构建整个项目：
```bash
cmake --build .
```

**当前状态**：项目缺少 CMakeLists.txt 和源代码文件，无法构建。建议先创建这些文件。

## 运行服务器

```bash
# 默认配置（端口8080）
./map_server

# 指定端口
./map_server --port 9090

# 使用配置文件
./map_server --config config.json

# 指定输出目录
./map_server --output ./leaf/maps
```

## 📡 API 端点详解

### GET /health
**健康检查端点**，返回服务器状态信息。

**请求示例**
```bash
curl http://localhost:8080/health
```

**响应格式**
```json
{
  "status": "healthy",
  "timestamp": "2026-03-09T10:30:00Z",
  "version": "1.0.0",
  "uptime": "2h30m",
  "memory_usage": "45.2MB"
}
```

**状态码**
- `200 OK`：服务正常
- `503 Service Unavailable`：服务异常

---

### GET /layers
**获取可用图层列表**，返回所有已配置的地图图层信息。

**请求示例**
```bash
curl http://localhost:8080/layers
```

**响应格式**
```json
{
  "layers": [
    {
      "name": "roads",
      "type": "line",
      "description": "道路网络图层",
      "min_zoom": 0,
      "max_zoom": 18,
      "style": {
        "color": "#333333",
        "width": 2
      }
    },
    {
      "name": "buildings",
      "type": "polygon", 
      "description": "建筑物图层",
      "min_zoom": 12,
      "max_zoom": 18,
      "style": {
        "fill_color": "#f0f0f0",
        "stroke_color": "#cccccc",
        "stroke_width": 1
      }
    }
  ]
}
```

---

### POST /generate
**生成地图图像**，接收JSON格式的请求参数，返回PNG图像流。

**请求格式**
```json
{
  "bbox": [-74.006, 40.7128, -73.935, 40.8076],
  "width": 1024,
  "height": 768,
  "layers": ["roads", "buildings"],
  "background_color": "#ffffff",
  "filter": "name LIKE '%park%'",
  "zoom_level": 12,
  "format": "png",
  "quality": 90
}
```

**参数说明**
| 参数 | 类型 | 必选 | 默认值 | 说明 |
|------|------|------|--------|------|
| bbox | number[] | ✅ | - | 边界框 [minx, miny, maxx, maxy] |
| width | integer | ✅ | - | 图像宽度（像素） |
| height | integer | ✅ | - | 图像高度（像素） |
| layers | string[] | ❌ | 所有图层 | 要渲染的图层列表 |
| background_color | string | ❌ | "#ffffff" | 背景颜色（HEX格式） |
| filter | string | ❌ | - | SQL WHERE条件过滤 |
| zoom_level | integer | ❌ | 自动计算 | 缩放级别 |
| format | string | ❌ | "png" | 输出格式（目前仅支持png） |
| quality | integer | ❌ | 90 | 图像质量（1-100） |

**响应**
- `Content-Type: image/png`
- `Content-Length: <文件大小>`
- 直接返回PNG图像二进制流

**错误响应**
```json
{
  "error": "invalid_parameter",
  "message": "Width must be between 1 and 4096",
  "details": {
    "parameter": "width",
    "value": 5000,
    "constraint": "max_image_width"
  }
}
```

---

### GET /tile/{z}/{x}/{y}.png
**获取地图瓦片**，使用标准XYZ瓦片方案。

**URL格式**
```
/tile/{zoom}/{x}/{y}.png
```

**参数说明**
- `z`：缩放级别（0-18）
- `x`：瓦片X坐标
- `y`：瓦片Y坐标

**示例**
```bash
curl http://localhost:8080/tile/12/1204/1540.png
```

**响应**
- `Content-Type: image/png`
- `Cache-Control: max-age=3600`
- 返回256x256像素的PNG瓦片

## 请求示例

使用curl测试：

```bash
# 健康检查
curl http://localhost:8080/health

# 获取图层列表
curl http://localhost:8080/layers

# 生成地图图像
curl -X POST http://localhost:8080/generate \
  -H "Content-Type: application/json" \
  -d '{"bbox":[-180,-90,180,90],"width":800,"height":600}' \
  --output map.png
```

## 输出文件

生成的PNG文件将保存到配置的输出目录（默认：`./leaf/output`），文件名格式：
- `map_<timestamp>.png` - 通过/generate端点生成的地图
- `tile_<z>_<x>_<y>.png` - 瓦片缓存文件

## 配置文件示例

配置文件为JSON格式，包含以下字段：

```json
{
  "server": {
    "host": "0.0.0.0",
    "port": 8080,
    "worker_threads": 4,
    "max_request_size": 10485760,
    "timeout_seconds": 30
  },
  "database": {
    "path": "./spatial_data.db",
    "connection_pool_size": 5
  },
  "cache": {
    "memory_max_items": 100,
    "disk_cache_dir": "./tile_cache",
    "ttl_seconds": 30
  },
  "limits": {
    "max_image_width": 4096,
    "max_image_height": 4096,
    "max_features_per_request": 10000
  },
  "output_dir": "./leaf/output"
}
```

## 开发说明

- 命名空间：`cycle`
- 函数命名：首字符大写
- 成员变量：匈牙利命名法（m_前缀）
- 线程安全：使用互斥锁保护共享资源
- 错误处理：返回错误码和描述信息

## 依赖项

### 必需依赖
- **cpp-httplib**：轻量级HTTP服务器库（已包含在thirdparty目录）
- **SQLite3**：嵌入式数据库引擎
- **libpng**：PNG图像编码库
- **nlohmann/json**：JSON解析库（已包含在thirdparty目录）

### 可选依赖
- **Qt 5.12+**：高质量图像渲染（推荐）
- **SpatiaLite**：SQLite空间扩展（增强空间查询能力）

## 目录结构

**当前实际结构**：
```
server/
├── config/              # 配置文件
│   └── config.example.json
├── thirdparty/          # 第三方库
│   ├── httplib.h        # cpp-httplib HTTP服务器库
│   └── nlohmann/        # nlohmann/json JSON解析库
└── README.md           # 本文档
```

**目标结构（待实现）**：
```
server/
├── include/             # 头文件（待创建）
│   ├── Config.h
│   ├── HttpServer.h
│   ├── PngEncoder.h
│   └── RenderEngine.h
├── src/                # 源文件（待创建）
│   ├── Config.cpp
│   ├── HttpServer.cpp
│   ├── PngEncoder.cpp
│   ├── RenderEngine.cpp
│   └── main.cpp
├── config/             # 配置文件
│   └── config.example.json
├── thirdparty/         # 第三方库
│   ├── httplib.h
│   └── nlohmann/
└── README.md
```

## 注意事项

1. 确保输出目录有写入权限
2. **libpng为必需依赖**：PNG图像编码需要libpng库
3. 数据库文件路径需正确配置
4. 生产环境请配置适当的CORS策略和安全设置
5. **当前项目状态**：缺少CMakeLists.txt和源代码文件，无法构建运行

## 故障排除

- **端口占用**：修改端口号或停止占用端口的进程
- **权限不足**：确保对输出目录有写入权限
- **依赖缺失**：安装libpng和SQLite3开发包
- **配置错误**：检查JSON格式和路径设置

## 🛠️ 技术方案选择与适用场景

### 图像渲染方案对比

#### 方案一：Qt 图像渲染（推荐）
**优势**：
- ✅ 图像质量高，支持抗锯齿
- ✅ 跨平台兼容性好
- ✅ 丰富的图形处理功能
- ✅ 成熟的文档和社区支持

**劣势**：
- ❌ 依赖较大，部署包体积增加
- ❌ 商业使用需注意许可证

**适用场景**：
- 需要高质量地图渲染的企业应用
- 跨平台桌面应用集成
- 对图像质量要求较高的场景

#### 方案二：libpng 原生渲染
**优势**：
- ✅ 轻量级，依赖少
- ✅ 性能高，内存占用小
- ✅ 许可证友好（zlib/libpng许可证）

**劣势**：
- ❌ 图像处理功能相对基础
- ❌ 需要手动实现复杂图形操作

**适用场景**：
- 嵌入式系统或资源受限环境
- 高性能服务器端渲染
- 对部署包大小敏感的场景

#### 方案三：混合渲染模式
**优势**：
- ✅ 根据需求动态选择渲染器
- ✅ 兼顾质量和性能
- ✅ 灵活的部署选项

**劣势**：
- ❌ 实现复杂度较高
- ❌ 需要维护多套渲染逻辑

**适用场景**：
- 需要灵活配置的云服务
- 不同客户端有不同需求
- 渐进式功能升级

---

### 缓存策略选择

#### 内存缓存
**配置**：`memory_max_items: 100`
**优势**：访问速度快，适合热点数据
**劣势**：内存占用大，重启丢失

#### 磁盘缓存  
**配置**：`disk_cache_dir: "./tile_cache"`
**优势**：持久化存储，容量大
**劣势**：I/O性能瓶颈

#### 多级缓存（推荐）
**配置**：内存+磁盘组合
**优势**：兼顾速度和持久性
**劣势**：实现复杂度高

---

### 数据库方案选择

#### SQLite（当前方案）
**优势**：
- ✅ 零配置，单文件部署
- ✅ 支持空间扩展（SpatiaLite）
- ✅ 轻量级，性能良好

**适用场景**：
- 中小规模地图数据（<10GB）
- 单机部署
- 快速原型开发

#### PostgreSQL + PostGIS
**优势**：
- ✅ 强大的空间查询能力
- ✅ 支持并发访问
- ✅ 企业级可靠性

**适用场景**：
- 大规模地理数据（>10GB）
- 高并发生产环境
- 复杂空间分析需求

#### 云数据库
**优势**：
- ✅ 弹性扩展
- ✅ 免运维
- ✅ 高可用性

**适用场景**：
- 云原生部署
- 流量波动大的场景
- 无专职DBA团队

---

### 部署架构选择

#### 单机部署（推荐用于开发）
```yaml
架构：单一进程
优势：简单易部署
劣势：单点故障，扩展性差
```

#### 负载均衡集群
```yaml
架构：Nginx + 多个Map Server实例
优势：高可用，水平扩展
劣势：配置复杂，需要会话管理
```

#### 容器化部署（推荐用于生产）
```yaml
架构：Docker + Kubernetes
优势：弹性伸缩，易于管理
劣势：运维复杂度高
```

---

### 性能优化建议

#### 针对不同场景的配置优化

**高并发场景**
```json
{
  "worker_threads": 8,
  "connection_pool_size": 20,
  "memory_max_items": 500
}
```

**大图像渲染场景**
```json
{
  "max_image_width": 8192,
  "max_image_height": 8192,
  "timeout_seconds": 60
}
```

**内存敏感场景**
```json
{
  "memory_max_items": 50,
  "worker_threads": 2,
  "max_request_size": 5242880
}
```

---

## 🔧 开发指南

### IDE 配置

#### Visual Studio 2019+
- 安装 C++ 开发工具
- 配置 CMake 集成
- 设置调试环境变量

#### VS Code
```json
{
  "cmake.configureArgs": [
    "-DCMAKE_BUILD_TYPE=Debug",
    "-DUSE_QT=ON"
  ]
}
```

#### CLion
- 启用 CMake 项目支持
- 配置 Qt 工具链
- 设置运行配置

### 调试技巧

#### 性能分析
```bash
# 使用 gperftools 分析性能
LD_PRELOAD=/usr/lib/libprofiler.so CPUPROFILE=server.prof ./map_server
```

#### 内存检查
```bash
# 使用 Valgrind 检查内存泄漏
valgrind --leak-check=full ./map_server
```

---

## 🧪 测试指南

### 单元测试
使用 Google Test 框架：
```cpp
TEST(RenderEngineTest, BasicRendering) {
    RenderEngine engine;
    EXPECT_TRUE(engine.Initialize());
}
```

### 集成测试
```bash
# 启动测试服务器
./map_server --port 9090 --config test_config.json

# 运行 API 测试
python test_api.py
```

### 性能基准

#### 渲染性能指标
- 单张图像渲染时间：< 500ms
- 并发请求处理：> 100 req/s
- 内存使用：< 100MB 基础占用

#### 缓存命中率
- 内存缓存命中率：> 90%
- 磁盘缓存命中率：> 70%

---

## 📊 监控与日志

### 健康检查指标
- 服务可用性
- 内存使用率
- 请求处理时间
- 缓存命中率

### 日志级别说明
```json
{
  "log": {
    "level": 1,  // DEBUG
    "level": 2,  // INFO  
    "level": 3,  // WARN
    "level": 4   // ERROR
  }
}
```

---

## 许可证

本项目基于现有项目许可证。