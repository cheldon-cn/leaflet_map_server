@echo off
REM ArcGIS Pro到QGIS迁移工具启动脚本（Windows）
REM 使用方法：run_migration.bat [参数]

REM 设置Python路径
set PYTHONPATH=..\script

REM 检查Python是否可用
python --version >nul 2>&1
if errorlevel 1 (
    echo 错误: 未找到Python，请确保Python 3.8+已安装并添加到PATH
    pause
    exit /b 1
)

REM 检查依赖是否安装
python -c "import psycopg2" >nul 2>&1
if errorlevel 1 (
    echo 警告: psycopg2未安装，尝试自动安装...
    pip install -r requirements.txt
)

REM 显示帮助信息
echo.
echo ========================================
echo ArcGIS Pro到QGIS迁移工具
echo ========================================
echo.

REM 如果没有参数，显示帮助
if "%1"=="" (
    python ..\script\migration_tool.py --help
    pause
    exit /b 0
)

REM 传递所有参数给主程序
python ..\script\migration_tool.py %*