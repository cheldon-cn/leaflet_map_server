# 后端 Dockerfile - 多阶段构建

# 第一阶段：依赖安装和编译
FROM ubuntu:24.04 AS builder

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

# 第二阶段：运行时镜像（更小体积）
FROM ubuntu:24.04 AS runtime

# 仅安装运行时依赖（包括curl用于健康检查）
RUN apt-get update && apt-get install -y \
    libsqlite3-0 \
    libpng16-16 \
    curl \
    && rm -rf /var/lib/apt/lists/*

# 创建非root用户
RUN useradd -m -u 1000 appuser

# 设置工作目录
WORKDIR /app

# 从构建阶段复制可执行文件
COPY --from=builder /build/build/map_server ./map_server

# 复制配置文件（运行时可通过卷挂载覆盖）
COPY config.production.json ./config.json

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