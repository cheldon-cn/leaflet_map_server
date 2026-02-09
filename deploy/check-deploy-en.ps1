# Deployment configuration check script
Write-Host "Leaf Project Containerized Deployment Configuration Check" -ForegroundColor Cyan
Write-Host "=========================================================" -ForegroundColor Cyan

# Check Docker
try {
    docker version 2>$null | Out-Null
    Write-Host "✅ Docker is installed" -ForegroundColor Green
} catch {
    Write-Host "❌ Docker is not installed or not available" -ForegroundColor Red
}

# Check Docker Compose
try {
    $composeVersion = docker-compose --version 2>$null
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✅ Docker Compose is installed ($composeVersion)" -ForegroundColor Green
    } else {
        $composeVersion = docker compose version 2>$null
        if ($LASTEXITCODE -eq 0) {
            Write-Host "✅ Docker Compose is installed (docker compose)" -ForegroundColor Green
        } else {
            Write-Host "❌ Docker Compose is not available" -ForegroundColor Red
        }
    }
} catch {
    Write-Host "❌ Docker Compose check failed" -ForegroundColor Red
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

foreach ($dir in $requiredDirs) {
    if (Test-Path $dir) {
        Write-Host "  ✅ $dir" -ForegroundColor Green
    } else {
        Write-Host "  ❌ $dir (missing)" -ForegroundColor Red
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

foreach ($file in $requiredFiles) {
    if (Test-Path $file) {
        $size = (Get-Item $file).Length
        Write-Host "  ✅ $file ($size bytes)" -ForegroundColor Green
    } else {
        Write-Host "  ❌ $file (missing)" -ForegroundColor Red
    }
}

# Check backend executable
Write-Host ""
Write-Host "Checking backend build..." -ForegroundColor Green
$backendExe = "..\..\build\bin\map_server"
if (Test-Path $backendExe) {
    Write-Host "  ✅ Backend executable exists" -ForegroundColor Green
} else {
    Write-Host "  ⚠️  Backend executable not found (need to build for dev mode)" -ForegroundColor Yellow
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
            Write-Host "  ✅ Backend production config port is correct (8080)" -ForegroundColor Green
        } else {
            Write-Host "  ⚠️  Backend production config port is not 8080" -ForegroundColor Yellow
        }
    } catch {
        Write-Host "  ❌ Backend production config JSON format error" -ForegroundColor Red
    }
} else {
    Write-Host "  ⚠️  Backend production config file not found" -ForegroundColor Yellow
}

# Check environment variable files
$envDev = Get-Content ".env.development" -ErrorAction SilentlyContinue
if ($envDev) {
    $hasApiUrl = $envDev | Where-Object { $_ -match "VITE_API_BASE_URL" }
    if ($hasApiUrl) {
        Write-Host "  ✅ Development environment variables include API config" -ForegroundColor Green
    } else {
        Write-Host "  ⚠️  Development environment variables missing API config" -ForegroundColor Yellow
    }
} else {
    Write-Host "  ⚠️  Development environment variables file not found" -ForegroundColor Yellow
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