# Leaf 项目容器化部署

本目录包含 Leaf 项目的完整容器化部署配置。

## 目录结构

```
deploy/
├── backend/                    # 后端服务配置
│   ├── Dockerfile            # 生产构建 Dockerfile
│   ├── Dockerfile.dev        # 开发构建 Dockerfile
│   ├── config.production.json  # 生产环境配置
│   └── config.development.json # 开发环境配置
├── frontend/                  # 前端服务配置
│   ├── Dockerfile            # 生产构建 Dockerfile
│   ├── Dockerfile.dev        # 开发构建 Dockerfile
│   └── nginx.conf            # Nginx 配置
├── nginx/                     # 反向代理配置
│   ├── nginx.conf            # 主 Nginx 配置
│   └── conf.d/
│       └── default.conf      # 站点配置
├── init-db/                   # 数据库初始化
│   ├── init.sql              # 表结构初始化
│   ├── sample-data.sql       # 示例数据
│   └── run.sh                # 初始化脚本
├── docker-compose.dev.yml     # 开发环境编排
├── docker-compose.prod.yml    # 生产环境编排
├── .env.development           # 开发环境变量
└── .env.production            # 生产环境变量
```

## 快速开始

### 开发环境

#### Windows (PowerShell)
1. **启动开发环境服务**：
   ```powershell
   cd deploy
   .\start-dev.ps1
   ```

2. **访问服务**：
   - 前端：http://localhost:3000
   - 后端 API：http://localhost:8080
   - 健康检查：http://localhost:8080/health

3. **停止服务**：
   ```powershell
   docker-compose -f docker-compose.dev.yml down
   ```

#### Linux/macOS (Bash)
1. **启动开发环境服务**：
   ```bash
   cd deploy
   chmod +x start-dev.sh
   ./start-dev.sh
   ```

2. **访问服务**：
   - 前端：http://localhost:3000
   - 后端 API：http://localhost:8080
   - 健康检查：http://localhost:8080/health

3. **停止服务**：
   ```bash
   docker-compose -f docker-compose.dev.yml down
   ```

#### 直接使用 Docker Compose
```bash
cd deploy
docker-compose -f docker-compose.dev.yml up --build
```

### 生产环境

#### Windows (PowerShell)
1. **启动生产环境服务**：
   ```powershell
   cd deploy
   .\start-prod.ps1
   ```

2. **访问服务**：
   - 网站：http://localhost (通过 Nginx 反向代理)
   - API：http://localhost/api/...

3. **查看服务状态**：
   ```powershell
   docker-compose -f docker-compose.prod.yml ps
   ```

4. **查看服务日志**：
   ```powershell
   docker-compose -f docker-compose.prod.yml logs -f
   ```

5. **停止服务**：
   ```powershell
   docker-compose -f docker-compose.prod.yml down
   ```

#### Linux/macOS (Bash)
1. **启动生产环境服务**：
   ```bash
   cd deploy
   chmod +x start-prod.sh
   ./start-prod.sh
   ```

2. **访问服务**：
   - 网站：http://localhost (通过 Nginx 反向代理)
   - API：http://localhost/api/...

3. **查看服务状态**：
   ```bash
   docker-compose -f docker-compose.prod.yml ps
   ```

4. **查看服务日志**：
   ```bash
   docker-compose -f docker-compose.prod.yml logs -f
   ```

5. **停止服务**：
   ```bash
   docker-compose -f docker-compose.prod.yml down
   ```

#### 直接使用 Docker Compose
```bash
cd deploy
docker-compose -f docker-compose.prod.yml up --build -d
```

## 环境配置

### 开发环境配置 (`docker-compose.dev.yml`)
- **后端服务**：使用开发配置，源码热重载
- **前端服务**：Vite 开发服务器，支持热更新
- **数据卷**：本地目录映射，便于开发调试
- **网络**：自定义 bridge 网络，服务间通过服务名通信

### 生产环境配置 (`docker-compose.prod.yml`)
- **Nginx 反向代理**：统一入口，负载均衡，SSL 支持
- **后端服务**：多阶段构建，最小化镜像体积
- **前端服务**：静态文件服务，Nginx 优化配置
- **健康检查**：自动监控服务状态
- **资源限制**：CPU/内存限制，防止资源耗尽

## 数据库初始化

### 初始化数据库
```bash
cd deploy/init-db
./run.sh
```

### 自定义数据库路径
```bash
./run.sh /path/to/your/database.db
```

### 数据库结构
- `layers`：图层定义表
- `features`：空间要素表
- `spatial_ref_sys`：空间参考系统表
- `metadata`：元数据表

## 部署选项

### 单机部署（开发/测试）
```bash
# 开发环境
docker-compose -f docker-compose.dev.yml up

# 生产环境
docker-compose -f docker-compose.prod.yml up -d
```

### 集群部署（生产）
1. **Docker Swarm**：
   ```bash
   docker stack deploy -c docker-compose.prod.yml leaf
   ```

2. **Kubernetes**：
   - 将 `docker-compose.prod.yml` 转换为 Kubernetes 资源文件
   - 使用 `kompose convert` 工具

## 监控与维护

### 查看服务状态
```bash
docker-compose -f docker-compose.prod.yml ps
docker-compose -f docker-compose.prod.yml logs [service-name]
```

### 健康检查
- 后端服务：`http://backend:8080/health`
- 前端服务：`http://frontend/health`
- Nginx：`http://nginx/health`

### 数据备份
```bash
# 备份数据库
docker exec leaf-backend cat /data/db/spatial_data.db > backup_$(date +%Y%m%d).db

# 备份生成的文件
tar -czf output_backup_$(date +%Y%m%d).tar.gz -C /path/to/volume leaf-output-data/
```

## 故障排除

### 常见问题

1. **端口冲突**：
   - 检查 80、443、3000、8080 端口是否被占用
   - 修改 `docker-compose.yml` 中的端口映射

2. **构建失败**：
   - 检查网络连接
   - 清理 Docker 缓存：`docker system prune -a`

3. **服务无法启动**：
   - 检查日志：`docker-compose logs [service-name]`
   - 检查配置文件路径和权限

4. **数据库连接失败**：
   - 确认数据库文件存在且可读写
   - 检查数据库文件权限：`chmod 644 /data/db/spatial_data.db`

### 获取帮助
- 查看详细部署指南：[../container-deployment-guide.md](../container-deployment-guide.md)
- 检查项目文档：[../README.md](../README.md)

## 更新与升级

### 更新容器
```bash
docker-compose -f docker-compose.prod.yml pull
docker-compose -f docker-compose.prod.yml up -d
```

### 重建服务
```bash
docker-compose -f docker-compose.prod.yml up --build -d
```

### 清理旧资源
```bash
docker system prune -a
docker volume prune
```

---

*最后更新：2025-02-05*  
*部署版本：1.0*