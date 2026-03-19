# PowerShell Build Script
param(
    [string]$Configuration = "Release",
    [switch]$SkipTests
)

$ErrorActionPreference = "Stop"
$SCRIPT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Path
$PROJECT_ROOT = Split-Path -Parent $SCRIPT_DIR

Write-Host "=== Building Project ===" -ForegroundColor Green

Push-Location $PROJECT_ROOT

try {
    Write-Host "1. Building C++ core library..." -ForegroundColor Yellow
    
    $buildDir = Join-Path $PROJECT_ROOT "build"
    if (-not (Test-Path $buildDir)) {
        New-Item -ItemType Directory -Path $buildDir | Out-Null
    }
    
    Push-Location $buildDir
    try {
        cmake .. -DCMAKE_BUILD_TYPE=$Configuration -DBUILD_TESTS=$(if ($SkipTests) { "OFF" } else { "ON" })
        cmake --build . --config $Configuration -j $env:NUMBER_OF_PROCESSORS
        
        Write-Host "2. Copying native library..." -ForegroundColor Yellow
        $libDir = Join-Path $PROJECT_ROOT "lib"
        if (-not (Test-Path $libDir)) {
            New-Item -ItemType Directory -Path $libDir | Out-Null
        }
        
        Copy-Item "src/core/$Configuration/project_core.dll" $libDir -ErrorAction SilentlyContinue
        Copy-Item "src/bridge/$Configuration/project_bridge.dll" $libDir -ErrorAction SilentlyContinue
    }
    finally {
        Pop-Location
    }
    
    Write-Host "3. Building Java wrapper..." -ForegroundColor Yellow
    Push-Location (Join-Path $PROJECT_ROOT "src/java")
    try {
        mvn clean install -DskipTests
    }
    finally {
        Pop-Location
    }
    
    Write-Host "4. Building application..." -ForegroundColor Yellow
    Push-Location (Join-Path $PROJECT_ROOT "src/app")
    try {
        mvn clean package $(if ($SkipTests) { "-DskipTests" })
    }
    finally {
        Pop-Location
    }
    
    Write-Host "=== Build Complete ===" -ForegroundColor Green
    Write-Host "Native libraries: $PROJECT_ROOT\lib\" -ForegroundColor Cyan
    Write-Host "Application JAR: $PROJECT_ROOT\src\app\target\*.jar" -ForegroundColor Cyan
}
finally {
    Pop-Location
}
