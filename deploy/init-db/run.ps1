# Leaf 项目数据库初始化脚本 (PowerShell)
# 用法: .\run.ps1 [数据库文件路径]
# 示例: .\run.ps1 ..\..\data\db\spatial_data.db

param(
    [string]$DbPath = "../../data/db/spatial_data.db"
)

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Leaf 项目数据库初始化 (PowerShell)" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host ""

# 解析数据库路径
$DbPath = Resolve-Path $DbPath -ErrorAction SilentlyContinue
if (-not $DbPath) {
    $DbPath = "$PSScriptRoot/../../data/db/spatial_data.db"
    $DbDir = Split-Path $DbPath -Parent
    
    if (-not (Test-Path $DbDir)) {
        Write-Host "创建数据库目录: $DbDir" -ForegroundColor Gray
        New-Item -ItemType Directory -Path $DbDir -Force | Out-Null
    }
}

Write-Host "数据库路径: $DbPath" -ForegroundColor Cyan

# 备份现有数据库（如果存在）
if (Test-Path $DbPath) {
    $backupPath = "$DbPath.backup.$(Get-Date -Format 'yyyyMMdd_HHmmss')"
    Write-Host "备份现有数据库: $backupPath" -ForegroundColor Gray
    Copy-Item $DbPath $backupPath -Force
}

# 方法1: 使用 Docker（推荐，跨平台）
function Initialize-WithDocker {
    Write-Host "尝试使用 Docker 初始化..." -ForegroundColor Gray
    
    try {
        # 创建临时目录用于挂载脚本
        $tempDir = "$env:TEMP\leaf-db-init-$(Get-Date -Format 'yyyyMMddHHmmss')"
        New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
        
        # 复制 SQL 脚本到临时目录
        Copy-Item "$PSScriptRoot\init.sql" "$tempDir\init.sql"
        Copy-Item "$PSScriptRoot\sample-data.sql" "$tempDir\sample-data.sql"
        
        # 获取数据库目录
        $dbDir = Split-Path $DbPath -Parent
        
        # Docker 命令
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
        
        Write-Host "执行 Docker 命令..." -ForegroundColor Gray
        & $dockerCmd[0] $dockerCmd[1..($dockerCmd.Length-1)]
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✅ 数据库初始化完成 (Docker)" -ForegroundColor Green
            return $true
        }
    } catch {
        Write-Host "Docker 初始化失败: $_" -ForegroundColor Yellow
    } finally {
        # 清理临时目录
        if (Test-Path $tempDir) {
            Remove-Item -Path $tempDir -Recurse -Force -ErrorAction SilentlyContinue
        }
    }
    
    return $false
}

# 方法2: 使用本地 sqlite3
function Initialize-WithLocalSqlite {
    Write-Host "尝试使用本地 sqlite3..." -ForegroundColor Gray
    
    # 检查 sqlite3 是否可用
    $sqlite3Cmd = Get-Command "sqlite3" -ErrorAction SilentlyContinue
    if (-not $sqlite3Cmd) {
        Write-Host "未找到 sqlite3 命令" -ForegroundColor Yellow
        return $false
    }
    
    try {
        # 执行初始化脚本
        Write-Host "执行 init.sql..." -ForegroundColor Gray
        sqlite3 $DbPath < "$PSScriptRoot\init.sql"
        
        Write-Host "执行 sample-data.sql..." -ForegroundColor Gray
        sqlite3 $DbPath < "$PSScriptRoot\sample-data.sql"
        
        Write-Host "✅ 数据库初始化完成 (本地 sqlite3)" -ForegroundColor Green
        return $true
    } catch {
        Write-Host "本地 sqlite3 初始化失败: $_" -ForegroundColor Red
        return $false
    }
}

# 主初始化流程
Write-Host "开始数据库初始化..." -ForegroundColor Green

$success = $false

# 先尝试 Docker 方法
$success = Initialize-WithDocker

# 如果 Docker 失败，尝试本地方法
if (-not $success) {
    Write-Host "Docker 方法失败，尝试本地 sqlite3..." -ForegroundColor Yellow
    $success = Initialize-WithLocalSqlite
}

# 如果都失败，提供手动指导
if (-not $success) {
    Write-Host ""
    Write-Host "❌ 数据库初始化失败" -ForegroundColor Red
    Write-Host "请手动执行以下步骤:" -ForegroundColor Yellow
    Write-Host "1. 安装 SQLite3:" -ForegroundColor Gray
    Write-Host "   • Windows: 从 https://sqlite.org/download.html 下载" -ForegroundColor Gray
    Write-Host "   • 或使用: choco install sqlite" -ForegroundColor Gray
    Write-Host "2. 运行初始化命令:" -ForegroundColor Gray
    Write-Host "   sqlite3 `"$DbPath`" < `"$PSScriptRoot\init.sql`"" -ForegroundColor Gray
    Write-Host "   sqlite3 `"$DbPath`" < `"$PSScriptRoot\sample-data.sql`"" -ForegroundColor Gray
    Write-Host ""
    exit 1
}

# 验证数据库文件
if (Test-Path $DbPath) {
    $fileSize = (Get-Item $DbPath).Length
    Write-Host ""
    Write-Host "✅ 数据库文件已创建" -ForegroundColor Green
    Write-Host "   文件路径: $DbPath" -ForegroundColor Gray
    Write-Host "   文件大小: $($fileSize) bytes" -ForegroundColor Gray
} else {
    Write-Host "⚠️  警告: 数据库文件可能未正确创建" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "数据库初始化流程完成" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan