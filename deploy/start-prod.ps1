# Leaf 项目生产环境启动脚本 (PowerShell)
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Leaf 项目生产环境启动" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan

# 检查 Docker 是否运行
try {
    $dockerInfo = docker info 2>$null
    if ($LASTEXITCODE -ne 0) {
        Write-Host "错误: Docker 守护进程未运行" -ForegroundColor Red
        Write-Host "请启动 Docker Desktop 后重试" -ForegroundColor Yellow
        exit 1
    }
} catch {
    Write-Host "错误: Docker 未安装或不可用" -ForegroundColor Red
    Write-Host "请安装 Docker Desktop: https://www.docker.com/products/docker-desktop/" -ForegroundColor Yellow
    exit 1
}

# 检查 Docker Compose 是否可用
$composeCmd = "docker-compose"
try {
    $composeVersion = docker-compose --version 2>$null
    if ($LASTEXITCODE -ne 0) {
        Write-Host "警告: docker-compose 未找到，尝试使用 docker compose" -ForegroundColor Yellow
        $composeVersion = docker compose version 2>$null
        if ($LASTEXITCODE -eq 0) {
            $composeCmd = "docker compose"
        } else {
            Write-Host "错误: docker compose 也不可用" -ForegroundColor Red
            Write-Host "请安装 Docker Compose: https://docs.docker.com/compose/install/" -ForegroundColor Yellow
            exit 1
        }
    }
} catch {
    Write-Host "错误: Docker Compose 检查失败" -ForegroundColor Red
    exit 1
}

# 切换到脚本所在目录
Set-Location $PSScriptRoot

Write-Host "1. 检查环境配置..." -ForegroundColor Green
if (-not (Test-Path ".env.production")) {
    Write-Host "  ⚠️  生产环境配置文件不存在，将使用默认配置" -ForegroundColor Yellow
} else {
    Write-Host "  ✅ 生产环境配置文件已存在" -ForegroundColor Green
}

$backendConfig = "backend\config.production.json"
if (-not (Test-Path $backendConfig)) {
    Write-Host "  ❌ 后端生产配置文件不存在，请先创建" -ForegroundColor Red
    exit 1
} else {
    Write-Host "  ✅ 后端生产配置文件已存在" -ForegroundColor Green
}

Write-Host ""
Write-Host "2. 初始化数据库..." -ForegroundColor Green
$dbDir = "..\..\data\db"
$dbFile = "$dbDir\spatial_data.db"

if (-not (Test-Path $dbDir)) {
    Write-Host "  创建数据库目录..." -ForegroundColor Gray
    New-Item -ItemType Directory -Path $dbDir -Force | Out-Null
}

# 尝试使用 Docker 初始化数据库（跨平台方案）
Write-Host "  使用 Docker 初始化数据库..." -ForegroundColor Gray
$initSuccess = $false

try {
    # 使用 sqlite:alpine 镜像执行初始化脚本
    $initSqlPath = (Resolve-Path "init-db\init.sql").Path
    $sampleDataPath = (Resolve-Path "init-db\sample-data.sql").Path
    
    # 创建临时目录用于挂载
    $tempDir = "$env:TEMP\leaf-db-init-$(Get-Date -Format 'yyyyMMddHHmmss')"
    New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
    Copy-Item $initSqlPath "$tempDir\init.sql"
    Copy-Item $sampleDataPath "$tempDir\sample-data.sql"
    
    # 创建 Docker 命令初始化数据库
    $dockerCmd = @(
        "docker", "run", "--rm",
        "-v", "$($dbDir -replace '\\', '/'):/data/db",
        "-v", "$($tempDir -replace '\\', '/'):/scripts",
        "sqlite:alpine",
        "/bin/sh", "-c", @'
# 初始化数据库
sqlite3 /data/db/spatial_data.db < /scripts/init.sql
sqlite3 /data/db/spatial_data.db < /scripts/sample-data.sql
# 设置权限
chmod 644 /data/db/spatial_data.db
# 验证
echo "数据库初始化完成"
sqlite3 /data/db/spatial_data.db "SELECT '图层数量:' AS label, COUNT(*) AS count FROM layers;"
sqlite3 /data/db/spatial_data.db "SELECT '要素总数:' AS label, COUNT(*) AS count FROM features;"
'@
    )
    
    Write-Host "    执行数据库初始化命令..." -ForegroundColor Gray
    & $dockerCmd[0] $dockerCmd[1..($dockerCmd.Length-1)]
    
    if ($LASTEXITCODE -eq 0) {
        $initSuccess = $true
        Write-Host "  ✅ 数据库初始化完成" -ForegroundColor Green
    } else {
        Write-Host "  ⚠️  Docker 初始化失败，尝试其他方法..." -ForegroundColor Yellow
    }
    
    # 清理临时目录
    Remove-Item -Path $tempDir -Recurse -Force -ErrorAction SilentlyContinue
    
} catch {
    Write-Host "  ⚠️  Docker 初始化异常: $_" -ForegroundColor Yellow
}

# 如果 Docker 方法失败，尝试使用本地 sqlite3
if (-not $initSuccess) {
    Write-Host "  尝试使用本地 sqlite3 初始化..." -ForegroundColor Gray
    
    # 检查是否安装了 sqlite3
    $sqlite3Path = Get-Command "sqlite3" -ErrorAction SilentlyContinue
    if ($sqlite3Path) {
        try {
            # 执行初始化脚本
            sqlite3 $dbFile < "init-db\init.sql"
            sqlite3 $dbFile < "init-db\sample-data.sql"
            
            # 设置文件权限
            attrib -R $dbFile 2>$null
            Write-Host "  ✅ 使用本地 sqlite3 初始化完成" -ForegroundColor Green
            $initSuccess = $true
        } catch {
            Write-Host "  ❌ 本地 sqlite3 初始化失败: $_" -ForegroundColor Red
        }
    } else {
        Write-Host "  ⚠️  未找到本地 sqlite3，请手动初始化数据库" -ForegroundColor Yellow
        Write-Host "     安装方法:" -ForegroundColor Gray
        Write-Host "     • Windows: 从 https://sqlite.org/download.html 下载预编译二进制文件" -ForegroundColor Gray
        Write-Host "     • 或使用: choco install sqlite" -ForegroundColor Gray
    }
}

if (-not $initSuccess) {
    Write-Host "  ⚠️  数据库初始化未完成，服务可能无法正常工作" -ForegroundColor Yellow
    Write-Host "     请手动运行数据库初始化步骤" -ForegroundColor Gray
}

Write-Host ""
Write-Host "3. 构建生产镜像..." -ForegroundColor Green
Write-Host "   这可能需要几分钟时间，请耐心等待..." -ForegroundColor Gray
Invoke-Expression "$composeCmd -f docker-compose.prod.yml build"

Write-Host ""
Write-Host "4. 启动生产服务..." -ForegroundColor Green
Write-Host "   服务将在后台运行" -ForegroundColor Gray
Write-Host "   访问地址: http://localhost" -ForegroundColor Cyan
Write-Host "   API 地址: http://localhost/api" -ForegroundColor Cyan
Write-Host ""

Invoke-Expression "$composeCmd -f docker-compose.prod.yml up -d"

Write-Host ""
Write-Host "5. 查看服务状态..." -ForegroundColor Green
Invoke-Expression "$composeCmd -f docker-compose.prod.yml ps"

Write-Host ""
Write-Host "6. 查看服务日志（持续输出，按 Ctrl+C 退出）..." -ForegroundColor Green
Write-Host "   查看日志: docker-compose -f docker-compose.prod.yml logs -f" -ForegroundColor Gray
Write-Host "   停止服务: docker-compose -f docker-compose.prod.yml down" -ForegroundColor Gray
Write-Host ""

$response = Read-Host "是否立即查看日志？(y/N)"
if ($response -match '^[Yy]$') {
    Invoke-Expression "$composeCmd -f docker-compose.prod.yml logs -f"
}

Write-Host ""
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "生产环境启动完成" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "常用命令:" -ForegroundColor Green
Write-Host "  • 查看状态: docker-compose -f docker-compose.prod.yml ps" -ForegroundColor Gray
Write-Host "  • 查看日志: docker-compose -f docker-compose.prod.yml logs -f [service]" -ForegroundColor Gray
Write-Host "  • 停止服务: docker-compose -f docker-compose.prod.yml down" -ForegroundColor Gray
Write-Host "  • 重新构建: docker-compose -f docker-compose.prod.yml up --build -d" -ForegroundColor Gray
Write-Host "  • 清理资源: docker system prune -a" -ForegroundColor Gray
Write-Host "==========================================" -ForegroundColor Cyan