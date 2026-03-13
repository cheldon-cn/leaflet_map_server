# Cycle Map Server 部署指南

## 系统要求

### 硬件要求
- **内存**: 最低 2GB，推荐 4GB 或以上
- **存储**: 最低 10GB 可用空间（用于缓存和数据）
- **CPU**: 多核处理器，推荐 4 核或以上

### 软件要求
- **操作系统**: Linux (Ubuntu 18.04+, CentOS 7+), Windows 10+, macOS 10.14+
- **编译器**: GCC 7.0+, Clang 6.0+, MSVC 2019+
- **构建工具**: CMake 3.10+

### 依赖库
- **必需依赖**:
  - SQLite3 3.20+
  - libpng 1.6+
  - cpp-httplib 0.10+
  - nlohmann/json 3.7+

- **可选依赖**:
  - PostgreSQL 12+ (用于 PostgreSQL 支持)
  - PostGIS 3.0+ (用于空间数据支持)
  - libwebp 1.0+ (用于 WebP 编码支持)
  - OpenSSL 1.1+ (用于 HTTPS 支持)

## 编译安装

### Linux/Unix 系统

1. **安装依赖**
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y build-essential cmake \
    libsqlite3-dev libpng-dev libpq-dev postgis \
    libwebp-dev libssl-dev

# CentOS/RHEL
sudo yum install -y gcc-c++ cmake \
    sqlite-devel libpng-devel postgresql-devel \
    postgis libwebp-devel openssl-devel
```

2. **编译项目**
```bash
cd server
mkdir build && cd build
cmake ..
make -j$(nproc)
```

3. **运行测试**
```bash
make test
# 或直接运行测试程序
./cycle-map-server-tests
```

### Windows 系统

1. **安装依赖**
   - 安装 Visual Studio 2019 或更高版本
   - 安装 vcpkg 包管理器
   - 使用 vcpkg 安装依赖：
```powershell
vcpkg install sqlite3 libpng cpp-httplib nlohmann-json
vcpkg install libpq libwebp openssl  # 可选依赖
```

2. **编译项目**
```powershell
cd server
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake ..
cmake --build . --config Release
```

### macOS 系统

1. **安装依赖**
```bash
# 使用 Homebrew
brew install cmake sqlite libpng postgresql postgis \
    webp openssl
```

2. **编译项目**
```bash
cd server
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

## 配置说明

### 配置文件 (config.json)

```json
{
    "server": {
        "host": "0.0.0.0",
        "port": 8080,
        "thread_count": 4,
        "read_timeout": 30,
        "write_timeout": 30
    },
    "database": {
        "type": "sqlite",
        "sqlite_file": "./data/map.db",
        "pg_host": "localhost",
        "pg_port": 5432,
        "pg_database": "mapdb",
        "pg_username": "postgres",
        "pg_password": "password",
        "max_connections": 10,
        "connection_timeout": 30
    },
    "cache": {
        "enabled": true,
        "memory_size_mb": 512,
        "disk_cache_dir": "./cache",
        "disk_cache_size_mb": 1024,
        "ttl_seconds": 3600
    },
    "range_limit": {
        "enabled": true,
        "max_zoom": 18,
        "min_x": -180,
        "min_y": -90,
        "max_x": 180,
        "max_y": 90,
        "max_width": 4096,
        "max_height": 4096,
        "max_dpi": 300
    }
}
```

### 环境变量

```bash
# 服务器配置
export CYCLE_SERVER_HOST=0.0.0.0
export CYCLE_SERVER_PORT=8080

# 数据库配置
export CYCLE_DB_TYPE=sqlite
export CYCLE_DB_FILE=./data/map.db

# 缓存配置
export CYCLE_CACHE_ENABLED=true
export CYCLE_CACHE_SIZE_MB=512

# 日志配置
export CYCLE_LOG_LEVEL=INFO
```

## 运行服务

### 开发模式
```bash
cd server/build
./cycle-map-server
```

### 生产模式
```bash
# 使用 systemd 服务
sudo cp cycle-map-server.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable cycle-map-server
sudo systemctl start cycle-map-server

# 查看服务状态
sudo systemctl status cycle-map-server

# 查看日志
sudo journalctl -u cycle-map-server -f
```

### Docker 部署

1. **构建 Docker 镜像**
```dockerfile
FROM ubuntu:20.04

# 安装依赖
RUN apt-get update && apt-get install -y \
    build-essential cmake \
    libsqlite3-dev libpng-dev \
    && rm -rf /var/lib/apt/lists/*

# 复制应用文件
COPY . /app
WORKDIR /app

# 编译应用
RUN mkdir build && cd build && \
    cmake .. && make

# 暴露端口
EXPOSE 8080

# 启动服务
CMD ["./build/cycle-map-server"]
```

2. **运行容器**
```bash
docker build -t cycle-map-server .
docker run -d -p 8080:8080 \
    -v ./data:/app/data \
    -v ./cache:/app/cache \
    cycle-map-server
```

## 性能优化

### 数据库优化

1. **SQLite 优化**
```sql
-- 启用 WAL 模式
PRAGMA journal_mode=WAL;
PRAGMA synchronous=NORMAL;
PRAGMA cache_size=-64000;
PRAGMA temp_store=MEMORY;
```

2. **PostgreSQL 优化**
```sql
-- 调整连接池大小
SET max_connections = 100;
SET shared_buffers = '256MB';
SET work_mem = '16MB';
```

### 缓存优化

1. **内存缓存**
- 根据可用内存调整缓存大小
- 监控缓存命中率，优化 TTL 设置

2. **磁盘缓存**
- 使用 SSD 存储提高 IO 性能
- 定期清理过期缓存文件

### 网络优化

1. **连接池配置**
```json
{
    "server": {
        "thread_count": 8,
        "read_timeout": 60,
        "write_timeout": 60
    }
}
```

2. **负载均衡**
- 使用 Nginx 作为反向代理
- 配置多实例负载均衡

## 监控和日志

### 健康检查
```bash
# HTTP 健康检查
curl http://localhost:8080/health

# 服务状态
curl http://localhost:8080/metrics
```

### 日志配置

1. **日志级别**
- DEBUG: 详细调试信息
- INFO: 常规运行信息
- WARN: 警告信息
- ERROR: 错误信息

2. **日志轮转**
```bash
# 使用 logrotate
/var/log/cycle-map-server.log {
    daily
    rotate 7
    compress
    missingok
    notifempty
}
```

## 故障排除

### 常见问题

1. **端口被占用**
```bash
# 检查端口占用
netstat -tulpn | grep 8080

# 杀死占用进程
kill -9 <PID>
```

2. **依赖库缺失**
```bash
# 检查动态库依赖
ldd cycle-map-server

# 安装缺失的库
sudo apt-get install -y libsqlite3-dev libpng-dev
```

3. **权限问题**
```bash
# 检查文件权限
ls -la data/ cache/

# 修复权限
chown -R cycle:cycle data/ cache/
chmod 755 data/ cache/
```

### 性能监控

1. **系统监控**
```bash
# CPU 使用率
top -p $(pgrep cycle-map-server)

# 内存使用
ps -o pid,user,%mem,command ax | grep cycle-map-server

# 网络连接
netstat -an | grep 8080
```

2. **应用监控**
- 监控 HTTP 请求响应时间
- 跟踪缓存命中率
- 记录数据库查询性能

## 安全配置

### 网络安全

1. **防火墙配置**
```bash
# 只允许特定 IP 访问
ufw allow from 192.168.1.0/24 to any port 8080

# 或使用 iptables
iptables -A INPUT -p tcp --dport 8080 -j DROP
iptables -A INPUT -p tcp --dport 8080 -s 192.168.1.0/24 -j ACCEPT
```

2. **HTTPS 配置**
```json
{
    "server": {
        "ssl_cert": "/path/to/cert.pem",
        "ssl_key": "/path/to/key.pem",
        "ssl_port": 8443
    }
}
```

### 数据安全

1. **数据库安全**
- 使用强密码
- 定期备份数据
- 加密敏感配置

2. **文件安全**
- 限制缓存目录访问权限
- 定期清理临时文件
- 监控文件系统变化

## 备份和恢复

### 数据备份

1. **数据库备份**
```bash
# SQLite 备份
sqlite3 data/map.db ".backup backup/map.db"

# PostgreSQL 备份
pg_dump -h localhost -U postgres mapdb > backup/mapdb.sql
```

2. **配置文件备份**
```bash
cp config.json backup/config-$(date +%Y%m%d).json
```

### 恢复流程

1. **停止服务**
```bash
sudo systemctl stop cycle-map-server
```

2. **恢复数据**
```bash
# SQLite 恢复
cp backup/map.db data/map.db

# PostgreSQL 恢复
psql -h localhost -U postgres mapdb < backup/mapdb.sql
```

3. **重启服务**
```bash
sudo systemctl start cycle-map-server
```

## 版本升级

### 升级流程

1. **备份当前版本**
```bash
# 备份数据和配置
tar -czf backup-$(date +%Y%m%d).tar.gz data/ cache/ config.json
```

2. **停止服务**
```bash
sudo systemctl stop cycle-map-server
```

3. **更新代码**
```bash
git pull origin main
```

4. **重新编译**
```bash
cd server/build
cmake .. && make
```

5. **重启服务**
```bash
sudo systemctl start cycle-map-server
```

### 回滚流程

1. **停止服务**
```bash
sudo systemctl stop cycle-map-server
```

2. **恢复备份**
```bash
tar -xzf backup-20231201.tar.gz
```

3. **重启旧版本**
```bash
sudo systemctl start cycle-map-server
```

---

**注意**: 在生产环境部署前，请确保充分测试所有功能，并根据实际需求调整配置参数。