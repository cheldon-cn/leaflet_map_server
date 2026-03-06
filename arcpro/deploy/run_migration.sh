#!/bin/bash
# ArcGIS Pro到QGIS迁移工具启动脚本（Linux/Mac）
# 使用方法：./run_migration.sh [参数]

# 设置Python路径
export PYTHONPATH=../script

# 检查Python是否可用
if ! command -v python3 &> /dev/null; then
    echo "错误: 未找到Python3，请确保Python 3.8+已安装"
    exit 1
fi

# 检查依赖是否安装
python3 -c "import psycopg2" 2>/dev/null
if [ $? -ne 0 ]; then
    echo "警告: psycopg2未安装，尝试自动安装..."
    pip3 install -r requirements.txt
fi

echo ""
echo "========================================"
echo "ArcGIS Pro到QGIS迁移工具"
echo "========================================"
echo ""

# 如果没有参数，显示帮助
if [ $# -eq 0 ]; then
    python3 ../script/migration_tool.py --help
    exit 0
fi

# 传递所有参数给主程序
python3 ../script/migration_tool.py "$@"