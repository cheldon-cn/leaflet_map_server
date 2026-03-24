<#
.SYNOPSIS
    OGC Geometry Library Installation Script
    
.DESCRIPTION
    This script compiles and installs the OGC Geometry library with proper
    directory structure:
      - include/  : Header files organized by module
      - lib/      : Static libraries and import libraries
      - bin/      : Dynamic libraries and executables
      - share/    : Documentation and CMake config files

.PARAMETER BuildType
    Build configuration (Release, Debug, RelWithDebInfo). Default: Release

.PARAMETER InstallPrefix
    Installation directory. Default: code/build/install

.PARAMETER BuildShared
    Build shared libraries. Default: true

.PARAMETER EnableTests
    Build unit tests. Default: true

.PARAMETER Clean
    Clean build directory before building

.EXAMPLE
    .\install.ps1 -BuildType Release -Clean
    
.EXAMPLE
    .\install.ps1 -BuildType Debug -InstallPrefix "C:\ogc" -EnableTests:$false
#>

param(
    [ValidateSet("Release", "Debug", "RelWithDebInfo")]
    [string]$BuildType = "Release",
    
    [string]$InstallPrefix = "",
    
    [bool]$BuildShared = $true,
    
    [bool]$EnableTests = $true,
    
    [bool]$Clean = $false
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$CodeDir = $ScriptDir
$BuildDir = Join-Path $CodeDir "build\temp"
$InstallDir = if ($InstallPrefix) { $InstallPrefix } else { Join-Path $CodeDir "build\install" }

function Write-Header {
    param([string]$Message)
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host " $Message" -ForegroundColor Cyan
    Write-Host "========================================" -ForegroundColor Cyan
    Write-Host ""
}

function Test-Command {
    param([string]$Command)
    $null = Get-Command $Command -ErrorAction SilentlyContinue
    return $?
}

function Invoke-Build {
    Write-Header "Configuring CMake"
    
    $CMakeArgs = @(
        "-S", $CodeDir,
        "-B", $BuildDir,
        "-G", "Visual Studio 17 2022",
        "-A", "x64",
        "-DCMAKE_BUILD_TYPE=$BuildType",
        "-DCMAKE_INSTALL_PREFIX=$InstallDir",
        "-DBUILD_SHARED_LIBS=$BuildShared",
        "-DENABLE_TESTS=$EnableTests"
    )
    
    Write-Host "CMake arguments:" -ForegroundColor Yellow
    $CMakeArgs | ForEach-Object { Write-Host "  $_" }
    
    & cmake $CMakeArgs
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed with exit code $LASTEXITCODE"
    }
    
    Write-Header "Building Project"
    
    & cmake --build $BuildDir --config $BuildType --parallel
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with exit code $LASTEXITCODE"
    }
}

function Invoke-Install {
    Write-Header "Installing to $InstallDir"
    
    & cmake --build $BuildDir --config $BuildType --target INSTALL
    if ($LASTEXITCODE -ne 0) {
        throw "Installation failed with exit code $LASTEXITCODE"
    }
}

function Copy-ThirdPartyDlls {
    Write-Header "Copying Third-Party DLLs"
    
    $BinDir = Join-Path $InstallDir "bin"
    if (-not (Test-Path $BinDir)) {
        New-Item -ItemType Directory -Path $BinDir -Force | Out-Null
    }
    
    $ThirdPartyPaths = @{
        "GEOS" = @(
            "F:\win\3rd\GEOS3.10\bin\geos.dll",
            "F:\win\3rd\GEOS3.10\bin\geos_c.dll"
        )
        "SQLite3" = @(
            "F:\win\3rd\sqlite3\sqlite3.dll"
        )
        "PostgreSQL" = @(
            "D:\program\PostgreSQL\13\bin\libpq.dll",
            "D:\program\PostgreSQL\13\bin\libcrypto-1_1-x64.dll",
            "D:\program\PostgreSQL\13\bin\libssl-1_1-x64.dll",
            "D:\program\PostgreSQL\13\bin\libintl-9.dll"
        )
        "GoogleTest" = @(
            "F:\win\3rd\googletest\bin\gtest.dll",
            "F:\win\3rd\googletest\bin\gtest_main.dll"
        )
    }
    
    foreach ($category in $ThirdPartyPaths.Keys) {
        Write-Host "Checking $category DLLs..." -ForegroundColor Yellow
        foreach ($dllPath in $ThirdPartyPaths[$category]) {
            if (Test-Path $dllPath) {
                $dllName = Split-Path $dllPath -Leaf
                Copy-Item $dllPath -Destination $BinDir -Force
                Write-Host "  Copied: $dllName" -ForegroundColor Green
            }
        }
    }
}

function New-InstallDirectories {
    $dirs = @(
        Join-Path $InstallDir "include",
        Join-Path $InstallDir "lib",
        Join-Path $InstallDir "bin",
        Join-Path $InstallDir "share\doc",
        Join-Path $InstallDir "share\cmake"
    )
    
    foreach ($dir in $dirs) {
        if (-not (Test-Path $dir)) {
            New-Item -ItemType Directory -Path $dir -Force | Out-Null
            Write-Host "Created: $dir" -ForegroundColor Green
        }
    }
}

function Show-InstallSummary {
    Write-Header "Installation Summary"
    
    Write-Host "Installation Directory: $InstallDir" -ForegroundColor Cyan
    Write-Host ""
    
    $IncludeDir = Join-Path $InstallDir "include"
    $LibDir = Join-Path $InstallDir "lib"
    $BinDir = Join-Path $InstallDir "bin"
    $ShareDir = Join-Path $InstallDir "share"
    
    if (Test-Path $IncludeDir) {
        $headerCount = (Get-ChildItem -Path $IncludeDir -Recurse -Filter "*.h").Count
        Write-Host "Headers:    $IncludeDir ($headerCount files)" -ForegroundColor Green
    }
    
    if (Test-Path $LibDir) {
        $libFiles = Get-ChildItem -Path $LibDir -Recurse -Include "*.lib","*.a"
        Write-Host "Libraries:  $LibDir ($($libFiles.Count) files)" -ForegroundColor Green
    }
    
    if (Test-Path $BinDir) {
        $binFiles = Get-ChildItem -Path $BinDir -Include "*.dll","*.exe"
        Write-Host "Binaries:   $BinDir ($($binFiles.Count) files)" -ForegroundColor Green
    }
    
    if (Test-Path $ShareDir) {
        Write-Host "Share:      $ShareDir" -ForegroundColor Green
    }
    
    Write-Host ""
    Write-Host "Installation completed successfully!" -ForegroundColor Green
}

function Main {
    Write-Header "OGC Geometry Library Installer"
    
    Write-Host "Configuration:" -ForegroundColor Yellow
    Write-Host "  Build Type:     $BuildType"
    Write-Host "  Install Prefix: $InstallDir"
    Write-Host "  Shared Libs:    $BuildShared"
    Write-Host "  Enable Tests:   $EnableTests"
    Write-Host "  Clean Build:    $Clean"
    
    if (-not (Test-Command "cmake")) {
        throw "CMake not found. Please install CMake and add it to PATH."
    }
    
    if ($Clean -and (Test-Path $BuildDir)) {
        Write-Header "Cleaning Build Directory"
        Remove-Item -Path $BuildDir -Recurse -Force
        Write-Host "Cleaned: $BuildDir" -ForegroundColor Green
    }
    
    New-InstallDirectories
    Invoke-Build
    Invoke-Install
    Copy-ThirdPartyDlls
    Show-InstallSummary
}

try {
    Main
}
catch {
    Write-Host ""
    Write-Host "ERROR: $_" -ForegroundColor Red
    Write-Host $_.ScriptStackTrace -ForegroundColor Red
    exit 1
}
