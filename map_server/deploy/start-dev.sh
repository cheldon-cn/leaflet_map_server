#!/bin/bash
# Leaf 项目开发环境启动脚本

set -e

echo "=========================================="
echo "Leaf 项目开发环境启动"
echo "=========================================="

# 检查 Docker 是否运行
if ! docker info > /dev/null 2>&1; then
    echo "错误: Docker 守护进程未运行"
    echo "请启动 Docker 服务后重试"
    exit 1
fi

# 检查 Docker Compose 是否可用
if ! command -v docker-compose &> /dev/null; then
    echo "警告: docker-compose 未安装，尝试使用 docker compose"
    if ! docker compose version &> /dev/null; then
        echo "错误: docker compose 也不可用"
        echo "请安装 Docker Compose: https://docs.docker.com/compose/install/"
        exit 1
    fi
    COMPOSE_CMD="docker compose"
else
    COMPOSE_CMD="docker-compose"
fi

# 切换到脚本所在目录
cd "$(dirname "$0")"

echo "1. 检查并构建后端可执行文件..."
if [ ! -f "../../build/bin/map_server" ]; then
    echo "  后端可执行文件不存在，需要先构建后端"
    echo "  请在项目根目录运行:"
    echo "    mkdir -p build && cd build"
    echo "    cmake .. -DCMAKE_BUILD_TYPE=Release"
    echo "    cmake --build . --target map_server"
    echo ""
    read -p "是否跳过后端构建直接启动？(y/N): " -n 1 -r
    echo ""
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "请先构建后端再运行此脚本"
        exit 1
    fi
else
    echo "  ✅ 后端可执行文件已存在"
fi

echo ""
echo "2. 启动开发环境服务..."
echo "   前端: http://localhost:3000"
echo "   后端: http://localhost:8080"
echo "   健康检查: http://localhost:8080/health"
echo ""

# 启动服务
$COMPOSE_CMD -f docker-compose.dev.yml up --build

echo ""
echo "=========================================="
echo "开发环境已停止"
echo "=========================================="