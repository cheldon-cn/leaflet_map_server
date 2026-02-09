# Leaf 项目容器化部署详细方案

## 一、项目架构分析与设计目标

### 1.1 现状分析
- **后端**：C++17应用，依赖SQLite3（必需）和libpng（可选），使用cpp-httplib提供HTTP API
- **前端**：Node.js 16+应用，基于Vite构建，依赖Leaflet等库
- **数据层**：SQLite空间数据库（`spatial_data.db`）
- **输出目录**：生成的PNG文件保存位置

### 1.2 容器化设计目标
1. **环境一致性**：消除开发、测试、生产环境差异
2. **依赖隔离**：避免系统库版本冲突
3. **可移植性**：支持跨平台部署（Docker支持的所有平台）
4. **可扩展性**：支持水平扩展（前端）和垂直扩展（后端）
5. **运维友好**：集成监控、日志、健康检查

### 1.3 架构设计
```
┌─────────────────────────────────────────────────┐
│                外部访问 (80/443)                 │
│                    ↓                             │
│           ┌─────────────────┐                   │
│           │  Nginx反向代理  │                   │
│           │  (可选)         │                   │
│           └────────┬────────┘                   │
│                    │                             │
│        ┌───────────┼─────────────┐              │
│        ↓           ↓             ↓              │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐        │
│  │  前端    │ │  后端    │ │  静态    │        │
│  │容器:3000│ │容器:8080│ │文件服务│        │
│  └──────────┘ └──────────┘ └──────────┘        │
│        │           │             │              │
│        └───────────┼─────────────┘              │
│                    │                             │
│           ┌────────▼────────┐                   │
│           │  共享数据卷     │                   │
│           │  - 数据库       │                   │
│           │  - 输出文件     │                   │
│           │  - 配置         │                   │
│           └─────────────────┘                   │
└─────────────────────────────────────────────────┘
```

## 二、环境准备与前提条件

### 2.1 基础设施要求
- **Docker**：版本20.10+，支持BuildKit
- **Docker Compose**：版本2.0+（用于开发和生产编排）
- **磁盘空间**：至少2GB可用空间（包含基础镜像）
- **内存**：建议4GB+（C++编译需要内存）

### 2.2 开发环境准备
1. **安装Docker Desktop**（Windows/Mac）或Docker Engine（Linux）
2. **验证安装**：
   ```bash
   docker --version
   docker-compose --version
   ```
3. **启用BuildKit**（提升构建性能）：
   ```bash
   export DOCKER_BUILDKIT=1
   # 或永久配置：在/etc/docker/daemon.json添加"features": {"buildkit": true}
   ```

## 三、后端容器化详细步骤

### 3.1 Dockerfile设计（多阶段构建）

#### 阶段1：构建环境
```dockerfile
# 第一阶段：依赖安装和编译
FROM ubuntu:22.04 AS builder

# 安装构建依赖
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libsqlite3-dev \
    libpng-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /build

# 复制源码
COPY server/ ./server/
COPY CMakeLists.txt ./

# 创建构建目录并编译
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release && \
    cmake --build . --target map_server
```

#### 阶段2：运行时环境
```dockerfile
# 第二阶段：运行时镜像（更小体积）
FROM ubuntu:22.04 AS runtime

# 仅安装运行时依赖
RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    libpng16-16 \
    && rm -rf /var/lib/apt/lists/*

# 创建非root用户
RUN useradd -m -u 1000 appuser

# 设置工作目录
WORKDIR /app

# 从构建阶段复制可执行文件
COPY --from=builder /build/build/map_server ./map_server

# 复制配置文件
COPY build/config.example.json ./config.json

# 创建数据目录
RUN mkdir -p /data && chown -R appuser:appuser /data

# 切换用户
USER appuser

# 暴露端口
EXPOSE 8080

# 健康检查
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1

# 启动命令
CMD ["./map_server", "--config", "config.json"]
```

### 3.2 构建优化策略
1. **利用Docker缓存层**：
   - 将不经常变化的依赖安装放在前面
   - 将经常变化的源码复制放在后面

2. **减小镜像体积**：
   - 使用多阶段构建，仅复制必要文件到运行时镜像
   - 清理apt缓存和临时文件
   - 使用alpine基础镜像（可选，需测试兼容性）

### 3.3 配置文件管理
```bash
# 目录结构
docker/
├── backend/
│   ├── Dockerfile
│   ├── config.production.json  # 生产配置
│   └── config.development.json # 开发配置
```

## 四、前端容器化详细步骤

### 4.1 Dockerfile设计

#### 方案A：开发模式（支持热重载）
```dockerfile
FROM node:18-alpine AS development

WORKDIR /app

# 复制依赖文件
COPY web/package.json web/package-lock.json ./

# 安装依赖
RUN npm ci

# 复制源代码
COPY web/ ./

# 开发模式启动
CMD ["npm", "run", "dev"]
```

#### 方案B：生产构建
```dockerfile
# 第一阶段：构建
FROM node:18-alpine AS builder

WORKDIR /app

COPY web/package.json web/package-lock.json ./
RUN npm ci

COPY web/ ./
RUN npm run build

# 第二阶段：Nginx服务
FROM nginx:alpine

# 复制构建产物
COPY --from=builder /app/dist /usr/share/nginx/html

# 复制Nginx配置
COPY docker/frontend/nginx.conf /etc/nginx/conf.d/default.conf

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]
```

### 4.2 环境变量配置
```bash
# .env.production
VITE_API_BASE_URL=/api
NODE_ENV=production

# .env.development  
VITE_API_BASE_URL=http://localhost:8080
NODE_ENV=development
```

## 五、数据持久化方案

### 5.1 数据卷设计
```yaml
# 持久化数据卷
volumes:
  # 数据库文件
  leaf-db-data:
    driver: local
  
  # 生成的地图文件
  leaf-output-data:
    driver: local
  
  # 配置文件
  leaf-config-data:
    driver: local
```

### 5.2 数据库初始化
```bash
# 数据库初始化脚本
docker/init-db/
├── init.sql          # 表结构初始化
├── sample-data.sql   # 示例数据
└── run.sh           # 初始化脚本
```

## 六、编排部署方案（Docker Compose）

### 6.1 开发环境编排
```yaml
# docker-compose.dev.yml
version: '3.8'

services:
  # 后端服务
  backend:
    build:
      context: .
      dockerfile: docker/backend/Dockerfile.dev
    ports:
      - "8080:8080"
    volumes:
      - ./server:/app/server:ro
      - ./build/config.example.json:/app/config.json
      - leaf-db-data:/data/db
      - leaf-output-data:/data/output
    environment:
      - NODE_ENV=development
    restart: unless-stopped
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:8080/health"]
      interval: 30s
      timeout: 10s
      retries: 3

  # 前端服务（开发模式）
  frontend:
    build:
      context: .
      dockerfile: docker/frontend/Dockerfile.dev
    ports:
      - "3000:3000"
    volumes:
      - ./web:/app
      - /app/node_modules
    depends_on:
      - backend
    environment:
      - VITE_API_BASE_URL=http://backend:8080
    command: npm run dev
```

### 6.2 生产环境编排
```yaml
# docker-compose.prod.yml
version: '3.8'

services:
  # Nginx反向代理
  nginx:
    image: nginx:alpine
    ports:
      - "80:80"
      - "443:443"
    volumes:
      - ./docker/nginx/nginx.conf:/etc/nginx/nginx.conf:ro
      - ./docker/nginx/conf.d:/etc/nginx/conf.d:ro
      - ssl-certs:/etc/ssl/certs
    depends_on:
      - backend
      - frontend
    restart: always

  # 后端服务
  backend:
    build:
      context: .
      dockerfile: docker/backend/Dockerfile
    expose:
      - "8080"
    volumes:
      - leaf-db-data:/data/db
      - leaf-output-data:/data/output
      - ./docker/backend/config.production.json:/app/config.json:ro
    environment:
      - NODE_ENV=production
    restart: always
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:8080/health"]
      interval: 30s
      timeout: 10s
      retries: 3

  # 前端服务（静态文件）
  frontend:
    build:
      context: .
      dockerfile: docker/frontend/Dockerfile
    expose:
      - "80"
    restart: always
```

## 七、生产环境优化建议

### 7.1 安全性优化
1. **最小权限原则**：
   - 使用非root用户运行容器
   - 限制容器能力（`--cap-drop=ALL --cap-add=...`）

2. **镜像安全扫描**：
   ```bash
   # 使用Trivy扫描漏洞
   docker run --rm \
     -v /var/run/docker.sock:/var/run/docker.sock \
     aquasec/trivy:latest image leaf-backend:latest
   ```

3. **网络隔离**：
   ```yaml
   # 创建自定义网络
   networks:
     leaf-network:
       driver: bridge
       ipam:
         config:
           - subnet: 172.20.0.0/16
   ```

### 7.2 性能优化
1. **资源限制**：
   ```yaml
   backend:
     deploy:
       resources:
         limits:
           memory: 1G
           cpus: '1.0'
         reservations:
           memory: 512M
           cpus: '0.5'
   ```

2. **健康检查和就绪检查**：
   ```yaml
   healthcheck:
     test: ["CMD", "curl", "-f", "http://localhost:8080/health"]
     interval: 30s
     timeout: 10s
     retries: 3
     start_period: 60s
   ```

### 7.3 高可用部署
1. **后端无状态化**：
   - 将SQLite迁移到PostgreSQL/MySQL（支持多实例）
   - 使用共享存储（NFS/CEPH）或云存储服务

2. **前端水平扩展**：
   ```bash
   # 扩展前端实例
   docker-compose -f docker-compose.prod.yml up --scale frontend=3 -d
   ```

## 八、监控与日志方案

### 8.1 日志收集
```yaml
# 配置日志驱动
services:
  backend:
    logging:
      driver: "json-file"
      options:
        max-size: "10m"
        max-file: "3"
```

### 8.2 监控集成
1. **Prometheus指标暴露**：
   ```cpp
   // 在HttpServer中添加/metrics端点
   void HandleMetricsRequest(const httplib::Request& req, httplib::Response& res) {
       res.set_content(prometheus_client.GetMetrics(), "text/plain");
   }
   ```

2. **容器监控**：
   ```bash
   # 部署cAdvisor
   docker run \
     --volume=/:/rootfs:ro \
     --volume=/var/run:/var/run:ro \
     --volume=/sys:/sys:ro \
     --volume=/var/lib/docker/:/var/lib/docker:ro \
     --publish=8081:8080 \
     --detach=true \
     --name=cadvisor \
     google/cadvisor:latest
   ```

## 九、持续集成/部署（CI/CD）建议

### 9.1 GitHub Actions流水线
```yaml
# .github/workflows/docker.yml
name: Docker Build and Push

on:
  push:
    branches: [main]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up QEMU
      uses: docker/setup-qemu-action@v2
      
    - name: Set up Docker Buildx
      uses: docker/setup-buildx-action@v2
      
    - name: Login to DockerHub
      uses: docker/login-action@v2
      with:
        username: ${{ secrets.DOCKER_USERNAME }}
        password: ${{ secrets.DOCKER_TOKEN }}
        
    - name: Build and push backend
      uses: docker/build-push-action@v4
      with:
        context: .
        file: docker/backend/Dockerfile
        push: true
        tags: |
          ${{ secrets.DOCKER_USERNAME }}/leaf-backend:latest
          ${{ secrets.DOCKER_USERNAME }}/leaf-backend:${{ github.sha }}
          
    - name: Build and push frontend
      uses: docker/build-push-action@v4
      with:
        context: .
        file: docker/frontend/Dockerfile
        push: true
        tags: |
          ${{ secrets.DOCKER_USERNAME }}/leaf-frontend:latest
          ${{ secrets.DOCKER_USERNAME }}/leaf-frontend:${{ github.sha }}
```

### 9.2 部署策略
1. **蓝绿部署**：通过负载均衡器切换流量
2. **滚动更新**：逐步替换旧容器
3. **金丝雀发布**：先发布到少量实例验证

## 十、分阶段实施计划

### 阶段1：基础容器化（2-3天）
1. 创建Dockerfile和docker-compose配置
2. 实现开发环境容器化
3. 测试基本功能

### 阶段2：生产环境准备（3-5天）
1. 优化Dockerfile（多阶段构建）
2. 配置环境变量和配置文件管理
3. 实现健康检查和监控
4. 安全加固

### 阶段3：CI/CD集成（2-3天）
1. 设置自动化构建流水线
2. 配置镜像扫描和安全检查
3. 实现自动部署

### 阶段4：高级功能（5-7天）
1. 数据库迁移（SQLite → PostgreSQL）
2. 实现高可用部署
3. 配置日志聚合和告警

## 十一、注意事项与风险控制

### 11.1 技术风险
1. **C++运行时依赖**：确保libpng、SQLite3等库的ABI兼容性
2. **文件权限**：容器内用户与挂载卷的权限匹配
3. **网络配置**：避免端口冲突和网络隔离问题

### 11.2 运维风险
1. **数据备份**：定期备份数据库和配置文件
2. **版本控制**：为每个镜像打上明确的版本标签
3. **回滚策略**：保留旧版本镜像以便快速回滚

### 11.3 成本控制
1. **镜像大小优化**：使用多阶段构建减小镜像体积
2. **存储优化**：合理设置日志轮转和临时文件清理
3. **资源配额**：根据实际使用情况调整资源限制

## 十二、验证方案

### 12.1 功能验证清单
```bash
# 基础功能验证
1. curl http://localhost:8080/health      # 后端健康检查
2. curl http://localhost:3000            # 前端访问
3. POST /generate 生成地图图像          # 核心功能

# 容器特性验证
4. docker exec -it leaf-backend ps aux   # 进程运行状态
5. docker logs leaf-backend              # 日志输出
6. docker stats leaf-backend             # 资源使用情况
```

### 12.2 性能基准测试
```bash
# 压力测试（使用wrk）
wrk -t4 -c100 -d30s http://localhost:8080/health

# 内存泄漏检查
docker run --memory=512m leaf-backend:latest
```

## 总结

此方案提供了从开发到生产的完整容器化路线图，具有以下特点：

1. **模块化设计**：前后端分离，便于独立部署和扩展
2. **环境一致性**：通过Docker镜像保证环境一致性
3. **生产就绪**：包含健康检查、监控、日志等生产特性
4. **可扩展性**：支持水平扩展和高可用部署
5. **安全性**：最小权限原则、镜像扫描、网络隔离

实施时建议按阶段推进，每个阶段完成后进行充分测试，确保系统稳定性和功能完整性。

---
*文档版本：1.0*  
*最后更新：2025-02-05*  
*适用项目：Leaf 地图渲染服务系统*