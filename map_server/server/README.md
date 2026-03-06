# Map Server - C++11 Implementation

基于图片流的地图服务，使用cpp-httplib构建HTTP服务，集成PNG生成模块，支持实时渲染并返回PNG图像流。

## 功能特性

- **HTTP REST API**：提供地图渲染端点
- **PNG编码**：集成libpng生成高质量PNG图像
- **跨平台**：支持Windows/Linux/macOS
- **多线程安全**：线程安全的渲染和缓存
- **可配置输出**：生成的文件保存到leaf文件夹下的子目录

## 构建要求

- CMake 3.10+
- C++11兼容编译器
- SQLite3开发库
- libpng开发库（可选，推荐）
- cpp-httplib（通过CMake自动下载）

## 构建步骤

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

## API端点

### GET /health
健康检查端点，返回服务器状态。

### GET /layers
获取可用图层列表。

### POST /generate
生成地图图像，接收JSON格式的请求参数：

```json
{
  "bbox": [minx, miny, maxx, maxy],
  "width": 1024,
  "height": 768,
  "layers": ["roads", "buildings"],
  "background_color": "#ffffff",
  "filter": "name LIKE '%park%'"
}
```

返回：`image/png`格式的PNG图像流。

### GET /tile/{z}/{x}/{y}.png
获取地图瓦片（标准XYZ瓦片方案）。

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

- **cpp-httplib**：轻量级HTTP服务器库
- **libpng**：PNG图像编码库
- **SQLite3**：嵌入式数据库引擎
- **nlohmann/json**：可选的JSON解析库

## 目录结构

```
leaf/
├── server/
│   ├── include/          # 头文件
│   │   ├── Config.h
│   │   ├── HttpServer.h
│   │   ├── PngEncoder.h
│   │   └── RenderEngine.h
│   ├── src/             # 源文件
│   │   ├── Config.cpp
│   │   ├── HttpServer.cpp
│   │   ├── PngEncoder.cpp
│   │   ├── RenderEngine.cpp
│   │   └── main.cpp
│   └── config/          # 配置文件
│       └── config.example.json
├── output/              # 生成文件输出目录
└── README.md           # 本文档
```

## 注意事项

1. 确保输出目录有写入权限
2. libpng为可选依赖，但推荐安装以获得完整功能
3. 数据库文件路径需正确配置
4. 生产环境请配置适当的CORS策略和安全设置

## 故障排除

- **端口占用**：修改端口号或停止占用端口的进程
- **权限不足**：确保对输出目录有写入权限
- **依赖缺失**：安装libpng和SQLite3开发包
- **配置错误**：检查JSON格式和路径设置

## 许可证

本项目基于现有项目许可证。