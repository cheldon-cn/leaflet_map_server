#!/bin/bash
# Leaf 项目数据库初始化脚本
# 用法: ./run.sh [数据库文件路径]

set -e

# 默认数据库路径
DEFAULT_DB_PATH="/data/db/spatial_data.db"

# 使用参数或默认路径
DB_PATH="${1:-$DEFAULT_DB_PATH}"
DB_DIR=$(dirname "$DB_PATH")

echo "Leaf 项目数据库初始化"
echo "数据库路径: $DB_PATH"

# 创建数据库目录
if [ ! -d "$DB_DIR" ]; then
    echo "创建数据库目录: $DB_DIR"
    mkdir -p "$DB_DIR"
fi

# 检查 SQLite3 是否安装
if ! command -v sqlite3 &> /dev/null; then
    echo "错误: sqlite3 未安装"
    echo "请安装 SQLite3:"
    echo "  Ubuntu/Debian: sudo apt-get install sqlite3"
    echo "  CentOS/RHEL: sudo yum install sqlite3"
    echo "  macOS: brew install sqlite3"
    exit 1
fi

# 备份现有数据库（如果存在）
if [ -f "$DB_PATH" ]; then
    BACKUP_PATH="${DB_PATH}.backup.$(date +%Y%m%d_%H%M%S)"
    echo "备份现有数据库: $BACKUP_PATH"
    cp "$DB_PATH" "$BACKUP_PATH"
fi

# 执行初始化脚本
echo "执行表结构初始化..."
sqlite3 "$DB_PATH" < "$(dirname "$0")/init.sql"

# 插入示例数据
echo "插入示例数据..."
sqlite3 "$DB_PATH" < "$(dirname "$0")/sample-data.sql"

# 设置文件权限
echo "设置数据库文件权限..."
chmod 644 "$DB_PATH"

# 验证数据库
echo "验证数据库结构..."
sqlite3 "$DB_PATH" << 'EOF'
.mode column
.headers on
SELECT '图层表' AS table_name, COUNT(*) AS row_count FROM layers
UNION ALL
SELECT '要素表', COUNT(*) FROM features
UNION ALL
SELECT '空间参考系统', COUNT(*) FROM spatial_ref_sys
UNION ALL
SELECT '元数据表', COUNT(*) FROM metadata;
EOF

# 空间扩展检查（可选）
if sqlite3 "$DB_PATH" "SELECT load_extension('mod_spatialite');" 2>/dev/null; then
    echo "空间扩展支持: 已启用"
else
    echo "空间扩展支持: 未启用（可选）"
fi

echo ""
echo "✅ 数据库初始化完成"
echo "数据库文件: $DB_PATH"
echo "可以使用以下命令连接:"
echo "  sqlite3 $DB_PATH"