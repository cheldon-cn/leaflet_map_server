@echo off
chcp 65001 >nul
echo ========================================
echo Trae Auto Continue 编译脚本
echo ========================================
echo.

set AHK_PATH=C:\Program Files\AutoHotkey
set SCRIPT_DIR=%~dp0root
set BIN_DIR=%~dp0bin

echo [1/4] 检查源文件...
if not exist "%SCRIPT_DIR%\TraeAutoContinue.ahk" (
    echo 错误: 找不到源文件 TraeAutoContinue.ahk
    pause
    exit /b 1
)

echo [2/4] 创建bin目录...
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"
if not exist "%BIN_DIR%\config" mkdir "%BIN_DIR%\config"
if not exist "%BIN_DIR%\lib\RapidOCR" mkdir "%BIN_DIR%\lib\RapidOCR"
if not exist "%BIN_DIR%\log" mkdir "%BIN_DIR%\log"

echo [3/4] 复制资源文件...
copy /Y "%SCRIPT_DIR%\config\config.ini" "%BIN_DIR%\config\" >nul
xcopy /E /I /Y "%SCRIPT_DIR%\lib\RapidOCR\*" "%BIN_DIR%\lib\RapidOCR\" >nul

echo [4/4] 编译主程序...

set COMPILER=
set BASE=

if exist "%AHK_PATH%\Compiler\Ahk2Exe.exe" (
    set COMPILER=%AHK_PATH%\Compiler\Ahk2Exe.exe
    set BASE=%AHK_PATH%\AutoHotkey.exe
) else if exist "%AHK_PATH%\v2\Compiler\Ahk2Exe.exe" (
    set COMPILER=%AHK_PATH%\v2\Compiler\Ahk2Exe.exe
    set BASE=%AHK_PATH%\v2\AutoHotkey.exe
)

if defined COMPILER (
    echo 使用编译器: %COMPILER%
    "%COMPILER%" /in "%SCRIPT_DIR%\TraeAutoContinue.ahk" /out "%BIN_DIR%\TraeAutoContinue.exe" /base "%BASE%"
) else (
    echo 警告: 未找到 Ahk2Exe.exe
    echo 请手动使用 AutoHotkey GUI 编译
    echo.
    echo 右键 TraeAutoContinue.ahk -^> Compile Script
    pause
    exit /b 0
)

if exist "%BIN_DIR%\TraeAutoContinue.exe" (
    echo.
    echo ========================================
    echo 编译成功！
    echo ========================================
    echo.
    echo 输出文件: %BIN_DIR%\TraeAutoContinue.exe
    echo 配置文件: %BIN_DIR%\config\config.ini
    echo OCR库: %BIN_DIR%\lib\RapidOCR\
    echo.
    echo 按任意键打开bin目录...
    pause >nul
    explorer "%BIN_DIR%"
) else (
    echo.
    echo ========================================
    echo 编译失败！
    echo ========================================
    pause
    exit /b 1
)
