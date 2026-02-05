# Leaf 地图渲染服务系统

基于 C++ 后端和 JavaScript 前端的全栈地图渲染服务，提供高性能的地图图像生成和交互式 Web 界面。

## 系统架构

本项目采用前后端分离架构：
- **后端**：C++11 实现的 HTTP 服务器，基于 cpp-httplib，提供 REST API 和 PNG 图像渲染
- **前端**：基于 Leaflet 和 Vite 的现代化 Web 应用，提供交互式地图界面
- **数据层**：SQLite3 空间数据库存储地理数据

## 功能特性

### 后端功能
- **HTTP REST API**：提供地图渲染端点
- **PNG编码**：集成 libpng 生成高质量 PNG 图像
- **跨平台**：支持 Windows/Linux/macOS
- **多线程安全**：线程安全的渲染和缓存
- **可配置输出**：生成的文件保存到 leaf 文件夹下的子目录
- **数据库集成**：连接 SQLite3 空间数据库查询图层数据

### 前端功能
- **交互式地图**：Leaflet 地图引擎，支持平滑平移和缩放
- **动态图像生成**：实时从后端请求地图图像
- **图层管理**：添加、移除和重排序地图图层
- **性能优化**：三级缓存策略（内存、localStorage、磁盘缓存）
- **响应式设计**：适配桌面、平板和移动设备
- **实时更新**：视口变化时自动更新地图

## 快速开始

### 环境要求
- **后端**：CMake 3.10+、C++11 编译器、SQLite3 开发库、libpng 开发库（可选）
- **前端**：Node.js 16+、npm 或 yarn

### 1. 构建后端
```bash
# 在项目根目录
mkdir build
cd build
cmake ..
cmake --build . --target map_server
```

### 2. 构建前端
```bash
cd web
npm install
```

### 3. 使用启动脚本（推荐）
项目提供了多种启动脚本，可同时启动前后端服务：

#### Windows PowerShell
```powershell
.\scripts\start-server.ps1
```

#### Windows 批处理
```bash
.\scripts\run-services00.bat
```

#### Linux/macOS
```bash
./scripts/start-server.sh
```

这些脚本会自动：
- 启动后端服务器（端口 8080）
- 启动前端开发服务器（端口 3000）
- 处理进程管理和错误恢复

### 4. 手动启动
```bash
# 启动后端（在 build 目录）
./map_server --config ../build/config.example.json

# 启动前端（在 web 目录）
npm run dev
```

## 详细文档

- [后端详细文档](./server/README.md) - C++ 后端架构和 API 文档
- [前端详细文档](./web/README.md) - Web 前端开发指南
- [前后端适配指南](./backend-frontend-adapter-guide.md) - 系统集成和接口适配说明

## API 文档

### 核心端点

#### GET /health
健康检查端点，返回服务器状态。

#### GET /layers
获取可用图层列表。

#### POST /generate
生成地图图像，接收 JSON 格式的请求参数：

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

返回：`image/png` 格式的 PNG 图像流。

#### GET /tile/{z}/{x}/{y}.png
获取地图瓦片（标准 XYZ 瓦片方案）。

### 新增端点（适配中）
- `GET /layers/{layerId}` - 获取图层详细信息
- `GET /capabilities` - 获取服务能力文档
- `POST /metrics` - 提交客户端性能指标

## 配置说明

### 后端配置
配置文件为 JSON 格式，位于 `build/config.example.json`：

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

### 前端配置
前端配置位于 `web/src/config/settings.js`，支持环境变量：

```env
VITE_API_BASE_URL=http://localhost:8080
```

## 项目结构

```
leaf/
├── build/                 # 构建输出目录
│   ├── bin/              # 可执行文件
│   ├── config.example.json # 配置文件示例
│   └── ... (其他构建文件)
├── output/               # 生成的 PNG 文件输出目录
├── scripts/              # 启动脚本
│   ├── run-services00.bat     # Windows 批处理脚本
│   ├── start-server.ps1       # PowerShell 脚本
│   ├── start-server.sh        # Shell 脚本
│   └── start-server0.ps1      # PowerShell 脚本（旧版）
├── server/               # C++ 后端源代码
│   ├── config/           # 配置文件
│   ├── include/          # 头文件
│   ├── src/              # 源文件
│   └── thirdparty/       # 第三方库
├── web/                  # JavaScript 前端项目
│   ├── public/           # 静态资源
│   ├── src/              # 前端源代码
│   ├── package.json      # 前端依赖配置
│   ├── vite.config.js    # Vite 构建配置
│   └── README.md         # 前端详细文档
├── backend-frontend-adapter-guide.md  # 前后端适配指南
└── README.md             # 本文档
```

## 开发指南

### 后端开发（C++）
- **命名空间**：`cycle`
- **函数命名**：首字符大写
- **成员变量**：匈牙利命名法（m_前缀）
- **线程安全**：使用互斥锁保护共享资源
- **错误处理**：返回错误码和描述信息

### 前端开发（JavaScript）
- **代码风格**：遵循 ESLint 和 Prettier 配置
- **组件设计**：模块化、可复用的组件结构
- **状态管理**：使用集中式状态存储
- **性能优化**：实现三级缓存策略

### 前后端适配状态
当前前后端适配正在进行中，详细进展请参考 [适配指南](./backend-frontend-adapter-guide.md)。主要适配任务包括：
1. API 端点统一
2. 响应格式协调
3. 参数验证增强
4. 数据库连接集成

## 故障排除

### 常见问题

#### 端口占用
- 修改配置文件中的端口号
- 停止占用端口的进程

#### 权限不足
- 确保对输出目录有写入权限
- 以管理员身份运行（Windows）

#### 依赖缺失
- 安装 libpng 和 SQLite3 开发包
- 运行 `npm install` 安装前端依赖

#### 配置错误
- 检查 JSON 格式和路径设置
- 确保数据库文件路径正确

#### 启动脚本问题
- **Ctrl+C 提示**：脚本已优化，无交互提示直接退出
- **浏览器打开两次**：已修复，Vite 配置禁用自动打开
- **控制台输出**：Vite 输出已重定向，保持控制台整洁

### 后端启动错误
```
Error: Unknown option E:\leaf\scripts\..\build\config.example.json
```
**解决方案**：使用正确的参数格式：`--config config.json`

## 部署说明

### 生产环境部署
1. **后端**：编译发布版本，配置 systemd 服务（Linux）或 Windows 服务
2. **前端**：运行 `npm run build`，将 `dist` 目录部署到 Web 服务器
3. **数据库**：确保 SQLite3 数据库文件可访问
4. **反向代理**：配置 Nginx/Apache 反向代理，统一前后端访问入口

### 容器化部署（计划中）
- 提供 Dockerfile 和 docker-compose 配置
- 支持 Kubernetes 部署

## 许可证

本项目基于现有项目许可证。详情请参考 LICENSE 文件（如有）。

## 支持与贡献

- **问题报告**：请提供详细的错误信息和复现步骤
- **功能建议**：欢迎提交功能请求
- **代码贡献**：请遵循现有代码风格，并添加相应测试

---
*最后更新：2025-02-05*  
*文档版本：2.0*