param(
    [string]$BuildType = "Release",
    [string]$ABI = "arm64-v8a",
    [string]$NDKPath = "D:\program\android\ndk-r25c",
    [string]$SDKPath = "",
    [string]$OutputDir = ".\build_android",
    [switch]$Clean = $false,
    [switch]$Verbose = $false
)

$ErrorActionPreference = "Stop"

function Write-Log {
    param([string]$Message, [string]$Level = "INFO")
    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $color = switch ($Level) {
        "INFO" { "Green" }
        "WARN" { "Yellow" }
        "ERROR" { "Red" }
        default { "White" }
    }
    Write-Host "[$timestamp] [$Level] $Message" -ForegroundColor $color
}

function Test-Command {
    param([string]$Command)
    try {
        Get-Command $Command -ErrorAction Stop | Out-Null
        return $true
    } catch {
        return $false
    }
}

function Test-PathExists {
    param([string]$Path, [string]$Description)
    if (-not (Test-Path $Path)) {
        Write-Log "$Description 不存在: $Path" -Level "ERROR"
        return $false
    }
    Write-Log "$Description 存在: $Path" -Level "INFO"
    return $true
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  OGC Chart Android 构建脚本" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

Write-Log "构建类型: $BuildType"
Write-Log "目标ABI: $ABI"
Write-Log "NDK路径: $NDKPath"
Write-Log "输出目录: $OutputDir"
Write-Host ""

Write-Log "检查构建环境..." -Level "INFO"

$checksPassed = $true

if (-not (Test-PathExists $NDKPath "NDK")) {
    $checksPassed = $false
}

$toolchainFile = Join-Path $NDKPath "build\cmake\android.toolchain.cmake"
if (-not (Test-PathExists $toolchainFile "NDK工具链文件")) {
    $checksPassed = $false
}

if (-not (Test-Command "cmake")) {
    Write-Log "CMake未安装或不在PATH中" -Level "ERROR"
    $checksPassed = $false
} else {
    $cmakeVersion = (cmake --version | Select-Object -First 1)
    Write-Log "CMake版本: $cmakeVersion" -Level "INFO"
}

if (-not (Test-Command "ninja")) {
    Write-Log "Ninja未安装或不在PATH中，将使用默认生成器" -Level "WARN"
    $generator = "Unix Makefiles"
} else {
    $generator = "Ninja"
    Write-Log "使用Ninja构建工具" -Level "INFO"
}

if (-not $checksPassed) {
    Write-Log "环境检查失败，请安装缺失的工具" -Level "ERROR"
    exit 1
}

$env:ANDROID_NDK_HOME = $NDKPath
if ($SDKPath) {
    $env:ANDROID_SDK_ROOT = $SDKPath
    Write-Log "设置ANDROID_SDK_ROOT: $SDKPath" -Level "INFO"
}

if ($Clean -and (Test-Path $OutputDir)) {
    Write-Log "清理构建目录: $OutputDir" -Level "WARN"
    Remove-Item -Path $OutputDir -Recurse -Force
}

if (-not (Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir | Out-Null
    Write-Log "创建构建目录: $OutputDir" -Level "INFO"
}

$projectRoot = Split-Path -Parent $PSScriptRoot
$modulePath = Join-Path $projectRoot "android_adapter"

if (-not (Test-Path $modulePath)) {
    $modulePath = $projectRoot
}

Write-Log "项目路径: $projectRoot" -Level "INFO"
Write-Log "模块路径: $modulePath" -Level "INFO"
Write-Host ""

$CMakeArgs = @(
    "-G", $generator,
    "-DCMAKE_TOOLCHAIN_FILE=$toolchainFile",
    "-DANDROID_ABI=$ABI",
    "-DANDROID_PLATFORM=android-31",
    "-DANDROID_STL=c++_static",
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DCMAKE_INSTALL_PREFIX=$OutputDir\install",
    "-DANDROID_NDK=$NDKPath"
)

if ($Verbose) {
    $CMakeArgs += "-DCMAKE_VERBOSE_MAKEFILE=ON"
}

$CMakeArgs += @("-S", $modulePath, "-B", $OutputDir)

Write-Log "配置CMake..." -Level "INFO"
Write-Host "CMake参数:" -ForegroundColor Gray
Write-Host "  $($CMakeArgs -join ' ')" -ForegroundColor Gray
Write-Host ""

& cmake $CMakeArgs

if ($LASTEXITCODE -ne 0) {
    Write-Log "CMake配置失败 (退出码: $LASTEXITCODE)" -Level "ERROR"
    exit 1
}

Write-Host ""
Write-Log "开始构建..." -Level "INFO"

$buildArgs = @(
    "--build", $OutputDir,
    "--config", $BuildType,
    "--parallel"
)

& cmake $buildArgs

if ($LASTEXITCODE -ne 0) {
    Write-Log "构建失败 (退出码: $LASTEXITCODE)" -Level "ERROR"
    exit 1
}

Write-Host ""
Write-Log "安装库文件..." -Level "INFO"

& cmake --install $OutputDir

if ($LASTEXITCODE -ne 0) {
    Write-Log "安装失败 (退出码: $LASTEXITCODE)" -Level "WARN"
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  构建成功!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Log "输出目录: $OutputDir" -Level "INFO"

$soFiles = Get-ChildItem -Path $OutputDir -Filter "*.so" -Recurse -ErrorAction SilentlyContinue
if ($soFiles) {
    Write-Host ""
    Write-Log "生成的.so文件:" -Level "INFO"
    $soFiles | ForEach-Object {
        Write-Host "  $($_.FullName)" -ForegroundColor Cyan
    }
}

$aarFiles = Get-ChildItem -Path $OutputDir -Filter "*.aar" -Recurse -ErrorAction SilentlyContinue
if ($aarFiles) {
    Write-Host ""
    Write-Log "生成的.aar文件:" -Level "INFO"
    $aarFiles | ForEach-Object {
        Write-Host "  $($_.FullName)" -ForegroundColor Cyan
    }
}

Write-Host ""
Write-Log "构建完成时间: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')" -Level "INFO"
