#!/bin/bash
# Leaf 项目生产环境启动脚本

set -e

echo "=========================================="
echo "Leaf 项目生产环境启动"
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

echo "1. 检查环境配置..."
if [ ! -f ".env.production" ]; then
    echo "  ⚠️  生产环境配置文件不存在，将使用默认配置"
else
    echo "  ✅ 生产环境配置文件已存在"
fi

if [ ! -f "backend/config.production.json" ]; then
    echo "  ❌ 后端生产配置文件不存在，请先创建"
    exit 1
else
    echo "  ✅ 后端生产配置文件已存在"
fi

echo ""
echo "2. 初始化数据库..."
DB_PATH="../../data/db/spatial_data.db"
DB_DIR="$(dirname "$DB_PATH")"

# 创建数据库目录
if [ ! -d "$DB_DIR" ]; then
    echo "  创建数据库目录: $DB_DIR"
    mkdir -p "$DB_DIR"
fi

echo "  运行数据库初始化脚本..."

# 方法1: 使用本地 run.sh 脚本
if [ -f "init-db/run.sh" ]; then
    chmod +x init-db/run.sh
    if init-db/run.sh "$DB_PATH"; then
        echo "  ✅ 数据库初始化完成 (本地脚本)"
    else
        echo "  ⚠️  本地脚本初始化失败，尝试 Docker 方法..."
        # 方法2: 使用 Docker
        if command -v docker &> /dev/null; then
            echo "    使用 Docker 初始化..."
            TEMP_DIR="/tmp/leaf-db-init-$(date +%Y%m%d%H%M%S)"
            mkdir -p "$TEMP_DIR"
            cp init-db/init.sql "$TEMP_DIR/"
            cp init-db/sample-data.sql "$TEMP_DIR/"
            
            if docker run --rm \
                -v "$(realpath "$DB_DIR"):/data/db" \
                -v "$TEMP_DIR:/scripts" \
                sqlite:alpine \
                /bin/sh -c '
                    sqlite3 /data/db/spatial_data.db < /scripts/init.sql
                    sqlite3 /data/db/spatial_data.db < /scripts/sample-data.sql
                    chmod 644 /data/db/spatial_data.db
                    echo "数据库初始化完成"
                    sqlite3 /data/db/spatial_data.db "SELECT \"图层数量:\" AS label, COUNT(*) AS count FROM layers;"
                    sqlite3 /data/db/spatial_data.db "SELECT \"要素总数:\" AS label, COUNT(*) AS count FROM features;"
                '; then
                echo "  ✅ 数据库初始化完成 (Docker)"
                rm -rf "$TEMP_DIR"
            else
                echo "  ❌ Docker 初始化也失败"
                rm -rf "$TEMP_DIR"
                echo "  请手动初始化数据库:"
                echo "    sqlite3 \"$DB_PATH\" < init-db/init.sql"
                echo "    sqlite3 \"$DB_PATH\" < init-db/sample-data.sql"
                exit 1
            fi
        else
            echo "  ❌ Docker 不可用，无法初始化数据库"
            echo "  请手动安装 sqlite3 或 Docker，然后运行:"
            echo "    sqlite3 \"$DB_PATH\" < init-db/init.sql"
            echo "    sqlite3 \"$DB_PATH\" < init-db/sample-data.sql"
            exit 1
        fi
    fi
else
    echo "  ⚠️  数据库初始化脚本不存在，跳过"
    echo "  请手动创建数据库:"
    echo "    mkdir -p ../../data/db"
    echo "    sqlite3 ../../data/db/spatial_data.db < init-db/init.sql"
    echo "    sqlite3 ../../data/db/spatial_data.db < init-db/sample-data.sql"
fi

echo ""
echo "3. 构建生产镜像..."
echo "   这可能需要几分钟时间，请耐心等待..."
$COMPOSE_CMD -f docker-compose.prod.yml build

echo ""
echo "4. 启动生产服务..."
echo "   服务将在后台运行"
echo "   访问地址: http://localhost"
echo "   API 地址: http://localhost/api"
echo ""

$COMPOSE_CMD -f docker-compose.prod.yml up -d

echo ""
echo "5. 查看服务状态..."
$COMPOSE_CMD -f docker-compose.prod.yml ps

echo ""
echo "6. 查看服务日志（持续输出，按 Ctrl+C 退出）..."
echo "   查看日志: docker-compose -f docker-compose.prod.yml logs -f"
echo "   停止服务: docker-compose -f docker-compose.prod.yml down"
echo ""

read -p "是否立即查看日志？(y/N): " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Yy]$ ]]; then
    $COMPOSE_CMD -f docker-compose.prod.yml logs -f
fi

echo ""
echo "=========================================="
echo "生产环境启动完成"
echo "=========================================="
echo ""
echo "常用命令:"
echo "  • 查看状态: docker-compose -f docker-compose.prod.yml ps"
echo "  • 查看日志: docker-compose -f docker-compose.prod.yml logs -f [service]"
echo "  • 停止服务: docker-compose -f docker-compose.prod.yml down"
echo "  • 重新构建: docker-compose -f docker-compose.prod.yml up --build -d"
echo "  • 清理资源: docker system prune -a"
echo "=========================================="