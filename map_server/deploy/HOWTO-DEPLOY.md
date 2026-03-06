# Leaf 项目容器化部署操作指南

## 概述

本指南提供了 Leaf 项目容器化部署的逐步操作说明。项目包含：
- **后端**：C++ 地图渲染服务（端口 8080）
- **前端**：Leaflet Web 应用（端口 3000/80）
- **数据库**：SQLite 空间数据库
- **反向代理**：Nginx（可选，生产环境）

## 部署前准备

### 1. 环境检查
确保满足以下条件：
- Docker 20.10+ 已安装并运行
- Docker Compose 2.0+ 已安装
- 至少 2GB 可用磁盘空间
- 至少 4GB 可用内存

### 2. 验证安装
```bash
docker --version
docker-compose --version
```

### 3. 启用 BuildKit（可选，提升构建性能）
```bash
# Linux/macOS
export DOCKER_BUILDKIT=1

# Windows PowerShell
$env:DOCKER_BUILDKIT=1
```

## 快速部署

### 方案一：开发环境（推荐）

#### 步骤 1：启动开发环境
```bash
cd deploy

# 方法 A：使用 Docker Compose 直接启动
docker-compose -f docker-compose.dev.yml up --build

# 方法 B：使用提供的启动脚本（Linux/macOS）
chmod +x start-dev.sh
./start-dev.sh

# 方法 C：使用 PowerShell（Windows）
.\start-dev.ps1  # 如果创建了 PowerShell 脚本
```

#### 步骤 2：访问服务
- **前端应用**：http://localhost:3000
- **后端 API**：http://localhost:8080
- **健康检查**：http://localhost:8080/health

#### 步骤 3：开发工作流
- 修改后端代码：`server/` 目录下的文件
- 修改前端代码：`web/` 目录下的文件
- 服务会自动热重载

### 方案二：生产环境

#### 步骤 1：初始化数据库
```bash
cd deploy/init-db

# 运行初始化脚本
chmod +x run.sh
./run.sh ../../data/db/spatial_data.db
```

#### 步骤 2：启动生产环境
```bash
cd deploy

# 方法 A：使用 Docker Compose
docker-compose -f docker-compose.prod.yml up --build -d

# 方法 B：使用启动脚本
chmod +x start-prod.sh
./start-prod.sh
```

#### 步骤 3：访问服务
- **网站**：http://localhost (通过 Nginx)
- **API**：http://localhost/api/...

#### 步骤 4：验证部署
```bash
# 查看服务状态
docker-compose -f docker-compose.prod.yml ps

# 查看服务日志
docker-compose -f docker-compose.prod.yml logs -f

# 健康检查
curl http://localhost/api/health
```

## 详细配置说明

### 后端配置

#### 配置文件
- `backend/config.development.json` - 开发环境配置
- `backend/config.production.json` - 生产环境配置

#### 关键配置项
```json
{
  "server": {
    "host": "0.0.0.0",
    "port": 8080,
    "worker_threads": 4
  },
  "database": {
    "path": "/data/db/spatial_data.db"
  }
}
```

### 前端配置

#### 环境变量
- `.env.development` - 开发环境变量
- `.env.production` - 生产环境变量

#### 关键配置项
```env
VITE_API_BASE_URL=http://backend:8080  # 开发环境
VITE_API_BASE_URL=/api                  # 生产环境
```

## 运维管理

### 常用命令

#### 开发环境
```bash
# 启动服务
docker-compose -f docker-compose.dev.yml up

# 停止服务
docker-compose -f docker-compose.dev.yml down

# 重建服务
docker-compose -f docker-compose.dev.yml up --build

# 查看日志
docker-compose -f docker-compose.dev.yml logs -f [service]
```

#### 生产环境
```bash
# 启动服务
docker-compose -f docker-compose.prod.yml up -d

# 停止服务
docker-compose -f docker-compose.prod.yml down

# 查看状态
docker-compose -f docker-compose.prod.yml ps

# 查看日志
docker-compose -f docker-compose.prod.yml logs -f [service]

# 清理资源
docker system prune -a
```

### 监控与维护

#### 健康检查
```bash
# 后端健康检查
curl http://localhost:8080/health

# 前端健康检查（如果配置）
curl http://localhost:3000/health
```

#### 数据库备份
```bash
# 备份数据库
docker exec $(docker ps -q -f name=leaf-backend) \
  cat /data/db/spatial_data.db > backup_$(date +%Y%m%d).db

# 恢复数据库
cat backup.db | docker exec -i $(docker ps -q -f name=leaf-backend) \
  tee /data/db/spatial_data.db > /dev/null
```

#### 日志管理
```bash
# 查看服务日志
docker-compose logs -f [service]

# 导出日志
docker-compose logs [service] > service.log

# 清理日志
docker-compose logs --tail=0 [service]
```

## 故障排除

### 常见问题

#### 1. 端口冲突
**症状**：服务启动失败，提示端口已被占用

**解决方案**：
1. 检查端口占用情况：
   ```bash
   # Linux/macOS
   netstat -tulpn | grep :80
   lsof -i :8080
   
   # Windows
   netstat -ano | findstr :80
   ```
2. 修改配置文件中的端口映射：
   ```yaml
   # docker-compose.dev.yml
   ports:
     - "3001:3000"  # 改为其他端口
     - "8081:8080"
   ```

#### 2. 构建失败
**症状**：Docker 构建过程中出错

**解决方案**：
1. 清理 Docker 缓存：
   ```bash
   docker system prune -a
   docker builder prune --all
   ```
2. 检查网络连接：
   ```bash
   docker pull ubuntu:22.04  # 测试基础镜像拉取
   ```
3. 检查磁盘空间：
   ```bash
   df -h  # Linux/macOS
   ```

#### 3. 服务无法启动
**症状**：容器创建后立即退出

**解决方案**：
1. 查看容器日志：
   ```bash
   docker logs [container-id]
   ```
2. 检查配置文件路径和权限：
   ```bash
   ls -la deploy/backend/
   ```
3. 检查环境变量配置：
   ```bash
   cat deploy/.env.development
   ```

#### 4. 数据库连接失败
**症状**：后端服务无法连接数据库

**解决方案**：
1. 检查数据库文件是否存在：
   ```bash
   ls -la ../../data/db/
   ```
2. 检查文件权限：
   ```bash
   chmod 644 ../../data/db/spatial_data.db
   ```
3. 重新初始化数据库：
   ```bash
   cd deploy/init-db
   ./run.sh
   ```

### 高级故障排除

#### 检查容器状态
```bash
# 查看所有容器
docker ps -a

# 查看容器详细信息
docker inspect [container-id]

# 查看容器资源使用情况
docker stats
```

#### 进入容器调试
```bash
# 进入后端容器
docker exec -it [backend-container-id] /bin/bash

# 进入前端容器
docker exec -it [frontend-container-id] /bin/sh
```

#### 清理所有资源
```bash
# 停止所有容器
docker-compose -f docker-compose.dev.yml down
docker-compose -f docker-compose.prod.yml down

# 清理未使用的资源
docker system prune -a --volumes

# 删除所有镜像
docker rmi $(docker images -q)
```

## 扩展与优化

### 性能优化

#### 镜像优化
1. **使用多阶段构建**：减小最终镜像体积
2. **使用 Alpine 基础镜像**：更小的基础镜像
3. **清理构建缓存**：删除中间文件和缓存

#### 容器资源限制
```yaml
deploy:
  resources:
    limits:
      memory: "1G"
      cpus: "1.0"
    reservations:
      memory: "512M"
      cpus: "0.5"
```

### 安全性加固

#### 容器安全
1. **使用非 root 用户**：降低权限
2. **限制容器能力**：仅授予必要权限
3. **镜像安全扫描**：定期扫描漏洞

#### 网络安全
1. **使用自定义网络**：隔离服务网络
2. **配置 SSL/TLS**：加密通信
3. **限制端口暴露**：仅暴露必要端口

### 高可用部署

#### Docker Swarm 部署
```bash
# 初始化 Swarm
docker swarm init

# 部署服务栈
docker stack deploy -c docker-compose.prod.yml leaf
```

#### Kubernetes 部署
```bash
# 转换 Docker Compose 配置
kompose convert -f docker-compose.prod.yml

# 应用 Kubernetes 配置
kubectl apply -f kubernetes/
```

## 附录

### 文件结构参考
```
deploy/
├── backend/                 # 后端配置
│   ├── Dockerfile         # 生产构建
│   ├── Dockerfile.dev     # 开发构建
│   ├── config.production.json
│   └── config.development.json
├── frontend/               # 前端配置
│   ├── Dockerfile         # 生产构建
│   ├── Dockerfile.dev     # 开发构建
│   └── nginx.conf         # Nginx配置
├── nginx/                  # 反向代理配置
├── init-db/                # 数据库初始化
├── docker-compose.dev.yml  # 开发编排
├── docker-compose.prod.yml # 生产编排
├── .env.development        # 开发环境变量
├── .env.production         # 生产环境变量
├── start-dev.sh           # 开发启动脚本
├── start-prod.sh          # 生产启动脚本
└── HOWTO-DEPLOY.md        # 本文件
```

### 环境变量参考
```env
# 开发环境 (.env.development)
BACKEND_PORT=8080
VITE_API_BASE_URL=http://backend:8080
NODE_ENV=development

# 生产环境 (.env.production)
BACKEND_PORT=8080
VITE_API_BASE_URL=/api
NODE_ENV=production
```

### 端口映射参考
| 服务 | 容器端口 | 主机端口（开发） | 主机端口（生产） |
|------|----------|------------------|------------------|
| 前端 | 3000     | 3000             | 80（通过 Nginx） |
| 后端 | 8080     | 8080             | 8080（内部）     |
| Nginx| 80       | -                | 80               |
| Nginx| 443      | -                | 443（可选）      |

---

*文档版本：1.0*  
*最后更新：2025-02-05*  
*适用环境：开发/测试/生产*