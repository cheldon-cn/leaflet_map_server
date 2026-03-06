# Docker Compose 部署问题诊断与解决总结

## 一、问题背景

在完成 ArcGIS Pro 迁移工具 Web 管理界面的开发后，按照部署指南执行 `docker-compose -f web/docker-compose.yml up -d` 启动服务时，连续遇到两个关键问题，导致服务无法正常启动。

## 二、第一轮问题：Docker 守护进程未运行与配置问题

### 2.1 问题现象
执行命令后出现连接 Docker API 失败的错误：
```
failed to connect to the docker API at npipe:////./pipe/dockerDesktopLinuxEngine
```

### 2.2 问题诊断
1. **Docker Desktop 未运行**：系统托盘无 Docker 图标，Docker 守护进程未启动
2. **配置文件中存在潜在问题**：
   - Worker 服务引用了不存在的 `Dockerfile.worker`
   - 前端健康检查使用了未安装的 `curl` 命令
   - 迁移脚本路径配置不一致
   - 前端依赖安装要求 `package-lock.json` 文件不存在

### 2.3 解决方案与修复

#### 2.3.1 启动 Docker Desktop
- 确保 Docker Desktop 已启动并运行
- 验证 Docker 状态：`docker version` 应同时显示 Client 和 Server 版本

#### 2.3.2 配置文件修复
对 `web/docker-compose.yml` 及相关文件进行了以下关键修改：

1. **Worker 服务构建**：
   ```yaml
   # 原配置（存在问题）
   worker:
     build:
       context: ./backend
       dockerfile: Dockerfile.worker  # 此文件不存在
   
   # 修复后配置
   worker:
     build:
       context: ./backend
       dockerfile: Dockerfile  # 使用主后端 Dockerfile
     command: ["python", "-m", "app.workers.migration_worker"]  # 指定启动命令
   ```

2. **前端健康检查**：
   ```yaml
   # 原配置（存在问题）
   healthcheck:
     test: ["CMD", "curl", "-f", "http://localhost:80"]  # Nginx Alpine 无 curl
   
   # 修复后配置  
   healthcheck:
     test: ["CMD", "nginx", "-t"]  # 测试 Nginx 配置语法
   ```

3. **迁移脚本路径统一**：
   ```yaml
   # 原配置（路径不一致）
   MIGRATION_SCRIPT_PATH: /app/../../script  # 容器内路径与卷映射不匹配
   
   # 修复后配置
   MIGRATION_SCRIPT_PATH: /app/script  # 与卷映射路径统一
   volumes:
     - ../../script:/app/script:ro
   ```

4. **前端依赖安装**：
   ```dockerfile
   # 原 Dockerfile（存在问题）
   COPY package.json package-lock.json ./  # package-lock.json 不存在
   RUN npm ci --only=production
   
   # 修复后 Dockerfile
   COPY package.json ./
   RUN npm install --only=production
   ```

5. **可选服务注释**：
   - 注释掉了可选的 Nginx 反向代理服务（缺少相关配置文件）

### 2.4 验证步骤
1. 启动 Docker Desktop
2. 执行配置验证：`docker-compose -f web/docker-compose.yml config`
3. 构建并启动服务：`docker-compose -f web/docker-compose.yml up --build -d`

## 三、第二轮问题：镜像加速器域名无法解析

### 3.1 问题现象
执行 `docker-compose -f web\docker-compose.yml up --build -d` 时出现镜像拉取失败：
```
Error response from daemon: failed to resolve reference "docker.io/minio/minio:latest": 
failed to do request: Head "https://hub-mirror.c.163.com/v2/minio/minio/manifests/latest?ns=docker.io": 
dialing hub-mirror.c.163.com:443 container via direct connection because Docker Desktop has no HTTPS proxy: 
connecting to hub-mirror.c.163.com:443: dial tcp: lookup hub-mirror.c.163.com: no such host
```

### 3.2 问题诊断
1. **网络环境限制**：公司网络 DNS 服务器无法解析网易镜像加速器域名 `hub-mirror.c.163.com`
2. **Docker 配置问题**：Docker Desktop 默认配置了无法访问的镜像加速器
3. **配置缓存**：修改 `~/.docker/daemon.json` 后，Docker Desktop 可能仍使用缓存配置

### 3.3 网络诊断结果
测试多个常用镜像加速器的 DNS 解析情况：
- ❌ `hub-mirror.c.163.com`：无法解析（网易镜像加速器）
- ❌ `mirror.baidubce.com`：无法解析（百度云镜像加速器）  
- ❌ `docker.mirrors.ustc.edu.cn`：无法解析（中科大镜像加速器）
- ❌ `mirror.ccs.tencentyun.com`：无法解析（腾讯云镜像加速器）
- ✅ `registry.docker-cn.com`：可解析（Docker 中国官方镜像）
- ✅ `registry.cn-hangzhou.aliyuncs.com`：可解析（阿里云镜像加速器）

### 3.4 解决方案（按推荐顺序）

#### 方案一：修改 Docker Desktop 镜像加速器设置（推荐）
通过 Docker Desktop 图形界面操作：
1. 右键点击系统托盘 Docker 图标 → **Settings**
2. 进入 **Docker Engine** 选项卡
3. 修改 `registry-mirrors` 配置：
   ```json
   "registry-mirrors": [
     "https://registry.docker-cn.com"  // Docker 中国官方镜像
   ]
   ```
   或完全删除该配置项（留空数组 `[]`）
4. 点击 **Apply & Restart**

#### 方案二：配置网络代理（如果公司有代理服务器）
1. Docker Desktop Settings → Resources → Proxies
2. 填写 HTTP 和 HTTPS 代理地址及端口
3. 或通过环境变量配置：
   ```powershell
   $env:HTTP_PROXY = "http://公司代理地址:端口"
   $env:HTTPS_PROXY = "http://公司代理地址:端口"
   ```

#### 方案三：使用本地已缓存的镜像
1. 在可联网环境中导出镜像：
   ```bash
   docker save -o minio.tar minio/minio:latest
   docker save -o postgres.tar postgres:15-alpine
   docker save -o redis.tar redis:7-alpine
   ```
2. 在当前环境导入镜像：
   ```powershell
   docker load -i minio.tar
   docker load -i postgres.tar
   docker load -i redis.tar
   ```

#### 方案四：简化系统架构（临时方案）
如果只需快速验证核心功能，可暂时移除 MinIO 服务：
1. 修改 `web/docker-compose.yml`，注释掉 minio 服务
2. 修改 backend 服务，移除对 minio 的依赖
3. 调整后端配置使用本地文件存储

### 3.5 配置修改验证
1. **检查当前配置**：
   ```powershell
   # 检查 daemon.json 配置
   Get-Content "$env:USERPROFILE\.docker\daemon.json" | ConvertFrom-Json
   
   # 检查 Docker 信息中的镜像加速器
   docker info | findstr "Registry Mirrors"
   ```

2. **测试镜像拉取**：
   ```powershell
   docker pull alpine:latest
   docker pull registry.docker-cn.com/minio/minio:latest
   ```

## 四、当前服务架构与端口配置

### 4.1 Docker Compose 服务栈
```yaml
services:
  frontend:    # React 前端 (Nginx) - 端口 3000
  backend:     # FastAPI 后端 - 端口 8000  
  db:          # PostgreSQL 数据库 - 端口 5432
  redis:       # Redis 缓存 - 端口 6379
  minio:       # MinIO 对象存储 - 端口 9000, 9001
  worker:      # 迁移任务工作器
  # nginx:     # 反向代理（已注释）
```

### 4.2 关键环境变量配置
```yaml
# 后端服务关键配置
DATABASE_URL: postgresql+asyncpg://postgres:password@db:5432/migration_db
REDIS_URL: redis://redis:6379/0
MINIO_ENDPOINT: minio:9000
MIGRATION_SCRIPT_PATH: /app/script
```

## 五、完整启动流程

### 5.1 前提条件
1. Docker Desktop 已安装并运行
2. 镜像加速器配置正确或网络代理已设置
3. 所需端口未被占用（3000, 8000, 5432, 6379, 9000, 9001）

### 5.2 启动命令
```powershell
# 进入项目目录
cd e:\pro\search\arcpro

# 构建并启动所有服务
docker-compose -f web\docker-compose.yml up --build -d

# 检查服务状态
docker-compose -f web\docker-compose.yml ps

# 查看服务日志
docker-compose -f web\docker-compose.yml logs -f
```

### 5.3 服务访问
- **前端管理界面**：http://localhost:3000
- **后端 API 文档**：http://localhost:8000/docs
- **MinIO 管理控制台**：http://localhost:9001
- **默认登录账号**：`admin` / `admin123`

## 六、总结与建议

### 6.1 问题根源分析
1. **第一轮问题**：Docker 环境未正确初始化 + 配置文件存在细节问题
2. **第二轮问题**：公司网络环境限制导致 Docker 镜像加速器域名无法解析

### 6.2 经验教训
1. **环境验证**：部署前应先验证 Docker 环境状态 `docker version`
2. **网络适应性**：企业网络环境可能限制外部域名访问，需准备备用方案
3. **配置测试**：修改配置后应通过 `docker-compose config` 验证语法
4. **渐进式部署**：可先启动核心服务（db, redis, backend）验证基础功能

### 6.3 后续优化建议
1. **镜像源优化**：在 Dockerfile 中使用国内镜像源加速构建
2. **离线部署方案**：准备离线镜像包，支持无网络环境部署
3. **健康检查改进**：优化服务健康检查机制，提高自愈能力
4. **配置管理**：完善环境变量管理，支持多环境配置

### 6.4 快速验证路径
如果遇到持续的网络问题，建议按以下路径验证：
1. 使用方案一修改 Docker Desktop 镜像加速器为 `registry.docker-cn.com`
2. 如仍失败，尝试方案二配置公司网络代理
3. 紧急情况下使用方案四简化架构，先验证核心迁移功能
4. 长期解决方案：与 IT 部门协调，开通必要的镜像加速器域名访问权限

---
**文档更新日期**：2025年3月5日  
**相关文档**：  
- [implementation_process_summary.md](./implementation_process_summary.md) - Web 系统实现过程总结  
- [web/README.md](./web/README.md) - Web 系统详细使用指南  
- [user_manual.md](./user_manual.md) - 最终用户操作手册