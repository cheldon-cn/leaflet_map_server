# Deployment configuration check script
# Use ASCII encoding only to avoid encoding issues

# Set console output encoding to UTF-8
try {
    [Console]::OutputEncoding = [System.Text.Encoding]::UTF8
} catch {
    # Ignore if not supported
}

Write-Host "Leaf Project Containerized Deployment Configuration Check" -ForegroundColor Cyan
Write-Host "=========================================================" -ForegroundColor Cyan

# Check Docker
try {
    $null = docker version 2>$null
    Write-Host "[OK] Docker is installed" -ForegroundColor Green
} catch {
    Write-Host "[ERROR] Docker is not installed or not available" -ForegroundColor Red
}

# Check Docker Compose
try {
    $composeVersion = docker-compose --version 2>$null
    if ($LASTEXITCODE -eq 0) {
        Write-Host "[OK] Docker Compose is installed ($composeVersion)" -ForegroundColor Green
    } else {
        $composeVersion = docker compose version 2>$null
        if ($LASTEXITCODE -eq 0) {
            Write-Host "[OK] Docker Compose is installed (docker compose)" -ForegroundColor Green
        } else {
            Write-Host "[ERROR] Docker Compose is not available" -ForegroundColor Red
        }
    }
} catch {
    Write-Host "[ERROR] Docker Compose check failed" -ForegroundColor Red
}

# Check directory structure
Write-Host ""
Write-Host "Checking directory structure..." -ForegroundColor Green

$requiredDirs = @(
    "backend",
    "frontend", 
    "nginx",
    "init-db",
    "nginx\conf.d"
)

$dirsOK = $true
foreach ($dir in $requiredDirs) {
    if (Test-Path $dir) {
        Write-Host "  [OK] $dir" -ForegroundColor Green
    } else {
        Write-Host "  [ERROR] $dir (missing)" -ForegroundColor Red
        $dirsOK = $false
    }
}

# Check required files
Write-Host ""
Write-Host "Checking required files..." -ForegroundColor Green

$requiredFiles = @(
    "backend\Dockerfile",
    "backend\Dockerfile.dev",
    "backend\config.production.json",
    "backend\config.development.json",
    "frontend\Dockerfile",
    "frontend\Dockerfile.dev",
    "frontend\nginx.conf",
    "nginx\nginx.conf",
    "nginx\conf.d\default.conf",
    "init-db\init.sql",
    "init-db\sample-data.sql",
    "init-db\run.sh",
    "docker-compose.dev.yml",
    "docker-compose.prod.yml",
    ".env.development",
    ".env.production"
)

$filesOK = $true
foreach ($file in $requiredFiles) {
    if (Test-Path $file) {
        $size = (Get-Item $file).Length
        Write-Host "  [OK] $file ($size bytes)" -ForegroundColor Green
    } else {
        Write-Host "  [ERROR] $file (missing)" -ForegroundColor Red
        $filesOK = $false
    }
}

# Check backend executable
Write-Host ""
Write-Host "Checking backend build..." -ForegroundColor Green
$backendExe = "..\..\build\bin\map_server"
if (Test-Path $backendExe) {
    Write-Host "  [OK] Backend executable exists" -ForegroundColor Green
} else {
    Write-Host "  [WARN] Backend executable not found (need to build for dev mode)" -ForegroundColor Yellow
}

# Check configuration files content
Write-Host ""
Write-Host "Checking configuration file content..." -ForegroundColor Green

# Check backend production config
$backendProdConfig = Get-Content "backend\config.production.json" -Raw -ErrorAction SilentlyContinue
if ($backendProdConfig) {
    try {
        $configObj = $backendProdConfig | ConvertFrom-Json
        if ($configObj.server.port -eq 8080) {
            Write-Host "  [OK] Backend production config port is correct (8080)" -ForegroundColor Green
        } else {
            Write-Host "  [WARN] Backend production config port is not 8080" -ForegroundColor Yellow
        }
    } catch {
        Write-Host "  [ERROR] Backend production config JSON format error" -ForegroundColor Red
    }
} else {
    Write-Host "  [WARN] Backend production config file not found" -ForegroundColor Yellow
}

# Check environment variable files
$envDev = Get-Content ".env.development" -ErrorAction SilentlyContinue
if ($envDev) {
    $hasApiUrl = $envDev | Where-Object { $_ -match "VITE_API_BASE_URL" }
    if ($hasApiUrl) {
        Write-Host "  [OK] Development environment variables include API config" -ForegroundColor Green
    } else {
        Write-Host "  [WARN] Development environment variables missing API config" -ForegroundColor Yellow
    }
} else {
    Write-Host "  [WARN] Development environment variables file not found" -ForegroundColor Yellow
}

# Summary
Write-Host ""
Write-Host "=========================================================" -ForegroundColor Cyan
Write-Host "Check completed" -ForegroundColor Cyan
Write-Host "=========================================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Green
Write-Host "1. Development environment: .\start-dev.ps1" -ForegroundColor Gray
Write-Host "2. Production environment: .\start-prod.ps1" -ForegroundColor Gray
Write-Host "3. View detailed guide: HOWTO-DEPLOY.md" -ForegroundColor Gray
Write-Host "=========================================================" -ForegroundColor Cyan

# Return exit code
if (-not $dirsOK -or -not $filesOK) {
    exit 1
} else {
    exit 0
}