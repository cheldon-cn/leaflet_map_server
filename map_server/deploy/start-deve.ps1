# Leaf 项目开发环境启动脚本 (PowerShell)
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "Leaf 项目开发环境启动" -ForegroundColor Cyan
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

Write-Host "1. 检查并构建后端可执行文件..." -ForegroundColor Green
$backendExe = "..\..\build\bin\map_server"
if (-not (Test-Path $backendExe)) {
    Write-Host "  后端可执行文件不存在，需要先构建后端" -ForegroundColor Yellow
    Write-Host "  请在项目根目录运行:" -ForegroundColor Yellow
    Write-Host "    mkdir build" -ForegroundColor Gray
    Write-Host "    cd build" -ForegroundColor Gray
    Write-Host "    cmake .. -DCMAKE_BUILD_TYPE=Release" -ForegroundColor Gray
    Write-Host "    cmake --build . --target map_server" -ForegroundColor Gray
    Write-Host ""
    
    $response = Read-Host "是否跳过后端构建直接启动？(y/N)"
    if ($response -notmatch '^[Yy]$') {
        Write-Host "请先构建后端再运行此脚本" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "  ? 后端可执行文件已存在" -ForegroundColor Green
}

Write-Host ""
Write-Host "2. 启动开发环境服务..." -ForegroundColor Green
Write-Host "   前端: http://localhost:3000" -ForegroundColor Cyan
Write-Host "   后端: http://localhost:8080" -ForegroundColor Cyan
Write-Host "   健康检查: http://localhost:8080/health" -ForegroundColor Cyan
Write-Host ""

# 启动服务
Write-Host "正在启动服务，请稍候..." -ForegroundColor Gray
Invoke-Expression "$composeCmd -f docker-compose.dev.yml up --build"

Write-Host ""
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "开发环境已停止" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan