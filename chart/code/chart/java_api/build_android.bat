@echo off
setlocal enabledelayedexpansion

REM ========================================
REM OGC Chart Android 构建脚本
REM ========================================

echo.
echo ========================================
echo   OGC Chart Android 构建脚本
echo ========================================
echo.

REM 设置默认参数
set BUILD_TYPE=Release
set ABI=arm64-v8a
set NDK_PATH=D:\program\android\ndk-r25c
set SDK_PATH=
set OUTPUT_DIR=.\build_android
set CLEAN=0
set VERBOSE=0

REM 解析命令行参数
:parse_args
if "%~1"=="" goto :end_parse
if /i "%~1"=="-h" goto :show_help
if /i "%~1"=="--help" goto :show_help
if /i "%~1"=="-t" set BUILD_TYPE=%~2& shift
if /i "%~1"=="--type" set BUILD_TYPE=%~2& shift
if /i "%~1"=="-a" set ABI=%~2& shift
if /i "%~1"=="--abi" set ABI=%~2& shift
if /i "%~1"=="-n" set NDK_PATH=%~2& shift
if /i "%~1"=="--ndk" set NDK_PATH=%~2& shift
if /i "%~1"=="-s" set SDK_PATH=%~2& shift
if /i "%~1"=="--sdk" set SDK_PATH=%~2& shift
if /i "%~1"=="-o" set OUTPUT_DIR=%~2& shift
if /i "%~1"=="--output" set OUTPUT_DIR=%~2& shift
if /i "%~1"=="-c" set CLEAN=1
if /i "%~1"=="--clean" set CLEAN=1
if /i "%~1"=="-v" set VERBOSE=1
if /i "%~1"=="--verbose" set VERBOSE=1
shift
goto :parse_args

:end_parse

REM 显示配置信息
echo [INFO] 构建类型: %BUILD_TYPE%
echo [INFO] 目标ABI: %ABI%
echo [INFO] NDK路径: %NDK_PATH%
echo [INFO] 输出目录: %OUTPUT_DIR%
echo.

REM 检查NDK路径
if not exist "%NDK_PATH%" (
    echo [ERROR] NDK路径不存在: %NDK_PATH%
    exit /b 1
)

set TOOLCHAIN_FILE=%NDK_PATH%\build\cmake\android.toolchain.cmake
if not exist "%TOOLCHAIN_FILE%" (
    echo [ERROR] NDK工具链文件不存在: %TOOLCHAIN_FILE%
    exit /b 1
)

echo [INFO] NDK工具链: %TOOLCHAIN_FILE%

REM 检查CMake
where cmake >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake未安装或不在PATH中
    exit /b 1
)

for /f "tokens=*" %%i in ('cmake --version 2^>^&1 ^| findstr /C:"cmake version"') do (
    echo [INFO] %%i
)

REM 检查Ninja
where ninja >nul 2>&1
if %ERRORLEVEL% equ 0 (
    set GENERATOR=Ninja
    echo [INFO] 使用Ninja构建工具
) else (
    set GENERATOR="Unix Makefiles"
    echo [WARN] Ninja未安装，使用默认生成器
)

REM 设置环境变量
set ANDROID_NDK_HOME=%NDK_PATH%
if not "%SDK_PATH%"=="" (
    set ANDROID_SDK_ROOT=%SDK_PATH%
    echo [INFO] 设置ANDROID_SDK_ROOT: %SDK_PATH%
)

REM 清理构建目录
if %CLEAN% equ 1 (
    if exist "%OUTPUT_DIR%" (
        echo [WARN] 清理构建目录: %OUTPUT_DIR%
        rmdir /s /q "%OUTPUT_DIR%"
    )
)

REM 创建构建目录
if not exist "%OUTPUT_DIR%" (
    mkdir "%OUTPUT_DIR%"
    echo [INFO] 创建构建目录: %OUTPUT_DIR%
)

REM 获取项目路径
set PROJECT_ROOT=%~dp0..
set MODULE_PATH=%PROJECT_ROOT%\android_adapter

if not exist "%MODULE_PATH%" (
    set MODULE_PATH=%PROJECT_ROOT%
)

echo [INFO] 项目路径: %PROJECT_ROOT%
echo [INFO] 模块路径: %MODULE_PATH%
echo.

REM 构建CMake参数
set CMAKE_ARGS=-G %GENERATOR%
set CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN_FILE%
set CMAKE_ARGS=%CMAKE_ARGS% -DANDROID_ABI=%ABI%
set CMAKE_ARGS=%CMAKE_ARGS% -DANDROID_PLATFORM=android-31
set CMAKE_ARGS=%CMAKE_ARGS% -DANDROID_STL=c++_static
set CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
set CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_INSTALL_PREFIX=%OUTPUT_DIR%\install
set CMAKE_ARGS=%CMAKE_ARGS% -DANDROID_NDK=%NDK_PATH%

if %VERBOSE% equ 1 (
    set CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_VERBOSE_MAKEFILE=ON
)

set CMAKE_ARGS=%CMAKE_ARGS% -S "%MODULE_PATH%" -B "%OUTPUT_DIR%"

REM 配置CMake
echo [INFO] 配置CMake...
if %VERBOSE% equ 1 (
    echo [DEBUG] CMake参数: %CMAKE_ARGS%
)
echo.

cmake %CMAKE_ARGS%

if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake配置失败 (退出码: %ERRORLEVEL%)
    exit /b 1
)

echo.
echo [INFO] 开始构建...

cmake --build "%OUTPUT_DIR%" --config %BUILD_TYPE% --parallel

if %ERRORLEVEL% neq 0 (
    echo [ERROR] 构建失败 (退出码: %ERRORLEVEL%)
    exit /b 1
)

echo.
echo [INFO] 安装库文件...

cmake --install "%OUTPUT_DIR%"

if %ERRORLEVEL% neq 0 (
    echo [WARN] 安装失败 (退出码: %ERRORLEVEL%)
)

echo.
echo ========================================
echo   构建成功!
echo ========================================
echo.
echo [INFO] 输出目录: %OUTPUT_DIR%

REM 列出生成的.so文件
if exist "%OUTPUT_DIR%\*.so" (
    echo.
    echo [INFO] 生成的.so文件:
    for %%f in ("%OUTPUT_DIR%\*.so") do (
        echo   %%f
    )
)

REM 列出生成的.aar文件
if exist "%OUTPUT_DIR%\*.aar" (
    echo.
    echo [INFO] 生成的.aar文件:
    for %%f in ("%OUTPUT_DIR%\*.aar") do (
        echo   %%f
    )
)

echo.
echo [INFO] 构建完成时间: %date% %time%
exit /b 0

:show_help
echo.
echo 用法: %~nx0 [选项]
echo.
echo 选项:
echo   -t, --type TYPE     构建类型 (Debug/Release, 默认: Release)
echo   -a, --abi ABI       目标ABI (arm64-v8a/armeabi-v7a, 默认: arm64-v8a)
echo   -n, --ndk PATH      NDK路径 (默认: D:\program\android\ndk-r25c)
echo   -s, --sdk PATH      SDK路径
echo   -o, --output DIR    输出目录 (默认: .\build_android)
echo   -c, --clean         清理构建目录
echo   -v, --verbose       详细输出
echo   -h, --help          显示帮助信息
echo.
echo 示例:
echo   %~nx0
echo   %~nx0 -t Debug -a armeabi-v7a
echo   %~nx0 -c -v
echo.
exit /b 0
