@echo off
REM ================================================
REM Leaf Map Services Startup Script (Batch)
REM Functions:
REM   1) Start backend map server (server.exe)
REM   2) Check backend health status
REM   3) Start frontend dev server if not running
REM   4) Open browser to access the application
REM ================================================

REM Configuration
set BACKEND_EXE=..\build\bin\server.exe
set CONFIG_FILE=..\build\config.example.json
set BACKEND_PORT=8080
set FRONTEND_PORT=3000
set FRONTEND_DIR=..\web
set HEALTH_CHECK_TIMEOUT=30
set HEALTH_CHECK_INTERVAL=2

REM Colors (Windows 10+ supports ANSI escape codes)
if not defined IN_COLOR_SET (
    for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
        set "ESC=%%a"
    )
    set "IN_COLOR_SET=1"
)
REM 如果 ESC 未定义，设置为空字符串以避免错误
if not defined ESC set "ESC="
if "%ESC%"=="" (
    set "CYAN="
    set "GREEN="
    set "YELLOW="
    set "RED="
    set "GRAY="
    set "RESET="
) else (
    set "CYAN=%ESC%[36m"
    set "GREEN=%ESC%[32m"
    set "YELLOW=%ESC%[33m"
    set "RED=%ESC%[31m"
    set "GRAY=%ESC%[90m"
    set "RESET=%ESC%[0m"
)

echo %CYAN%================================================%RESET%
echo %CYAN%Leaf 地图服务系统启动脚本 (Batch)%RESET%
echo %CYAN%================================================%RESET%
echo %YELLOW%后端端口: %BACKEND_PORT%, 前端端口: %FRONTEND_PORT%%RESET%
echo.

REM Function: Check if a port is responding
:CheckPort
setlocal
set PORT=%1
set URL=http://localhost:%PORT%
powershell -Command "$response = try { Invoke-WebRequest -Uri '%URL%' -TimeoutSec 3 -ErrorAction Stop } catch { $null }; if ($response -and $response.StatusCode -eq 200) { exit 0 } else { exit 1 }"
if %ERRORLEVEL% equ 0 (endlocal & exit /b 0) else (endlocal & exit /b 1)

REM Step 1: Check backend server executable
echo %CYAN%检查后端服务器可执行文件...%RESET%

REM 检查可能的可执行文件路径
set "BACKEND_EXE_FOUND="
if exist "%BACKEND_EXE%" (
    set "BACKEND_EXE_FOUND=%BACKEND_EXE%"
) else (
    REM 尝试其他可能的名称和位置
    set "ALTERNATE_EXE=..\build\bin\map_server.exe"
    if exist "%ALTERNATE_EXE%" (
        set "BACKEND_EXE=%ALTERNATE_EXE%"
        set "BACKEND_EXE_FOUND=%ALTERNATE_EXE%"
    ) else (
        set "ALTERNATE_EXE2=..\build\x64\Debug\map_server.exe"
        if exist "%ALTERNATE_EXE2%" (
            set "BACKEND_EXE=%ALTERNATE_EXE2%"
            set "BACKEND_EXE_FOUND=%ALTERNATE_EXE2%"
        ) else (
            set "ALTERNATE_EXE3=E:\pro\search\leaf\build\bin\server.exe"
            if exist "%ALTERNATE_EXE3%" (
                set "BACKEND_EXE=%ALTERNATE_EXE3%"
                set "BACKEND_EXE_FOUND=%ALTERNATE_EXE3%"
            )
        )
    )
)

if not defined BACKEND_EXE_FOUND (
    echo %RED%错误: 找不到后端服务器可执行文件%RESET%
    echo %GRAY%检查的路径:%RESET%
    echo %GRAY%  - ..\build\bin\server.exe%RESET%
    echo %GRAY%  - ..\build\bin\map_server.exe%RESET%
    echo %GRAY%  - ..\build\x64\Debug\map_server.exe%RESET%
    echo %GRAY%  - E:\pro\search\leaf\build\bin\server.exe%RESET%
    echo %YELLOW%请先构建项目:%RESET%
    echo %GRAY%  cmake --build build --target map_server%RESET%
    echo %YELLOW%或手动构建 server.exe / map_server.exe 并放置到上述目录中%RESET%
    echo.
    pause
    exit /b 1
)

echo %GREEN%后端可执行文件存在%RESET%
echo %GRAY%路径: %BACKEND_EXE%%RESET%

echo %GREEN%后端可执行文件存在%RESET%
echo %GRAY%路径: %BACKEND_EXE%%RESET%

REM Step 2: Check backend server status
echo.
echo %CYAN%检查后端服务器状态...%RESET%
call :CheckPort %BACKEND_PORT%
if %ERRORLEVEL% equ 0 (
    echo %GREEN%后端服务器已在运行 (端口: %BACKEND_PORT%)%RESET%
    goto :CheckFrontend
)

echo %YELLOW%后端服务器未运行，尝试启动...%RESET%

REM Check config file
if not exist "%CONFIG_FILE%" (
    echo %YELLOW%警告: 找不到配置文件，使用默认配置运行%RESET%
    start "" "%BACKEND_EXE%"
) else (
    echo %GRAY%使用配置文件: %CONFIG_FILE%%RESET%
    start "" "%BACKEND_EXE%" "%CONFIG_FILE%"
)

echo %GREEN%后端服务器进程已启动%RESET%

REM Step 3: Wait for backend server to become ready
echo %CYAN%等待后端服务器启动...%RESET%
set TIMEOUT_COUNTER=0
:WaitForBackend
call :CheckPort %BACKEND_PORT%
if %ERRORLEVEL% equ 0 goto :BackendReady

set /a TIMEOUT_COUNTER+=%HEALTH_CHECK_INTERVAL%
if %TIMEOUT_COUNTER% geq %HEALTH_CHECK_TIMEOUT% (
    echo %RED%错误: 后端服务器在 %HEALTH_CHECK_TIMEOUT% 秒内未响应%RESET%
    echo %YELLOW%请检查服务器日志或构建输出%RESET%
    echo.
    pause
    exit /b 1
)

echo %GRAY%等待服务器响应... (%TIMEOUT_COUNTER%/%HEALTH_CHECK_TIMEOUT% 秒)%RESET%
timeout /t %HEALTH_CHECK_INTERVAL% /nobreak > nul
goto :WaitForBackend

:BackendReady
echo %GREEN%后端服务器已就绪!%RESET%

REM Step 4: Check frontend dev server
:CheckFrontend
echo.
echo %CYAN%检查前端开发服务器状态...%RESET%
call :CheckPort %FRONTEND_PORT%
if %ERRORLEVEL% equ 0 (
    echo %GREEN%前端开发服务器可能已在运行 (端口: %FRONTEND_PORT%)%RESET%
    goto :OpenBrowser
)

echo %YELLOW%前端开发服务器未运行，尝试启动...%RESET%

REM Check frontend directory
if not exist "%FRONTEND_DIR%" (
    echo %RED%错误: 找不到前端目录%RESET%
    echo %GRAY%路径: %FRONTEND_DIR%%RESET%
    echo %YELLOW%请确保 web/ 目录存在%RESET%
    echo.
    pause
    exit /b 1
)

REM Check package.json
if not exist "%FRONTEND_DIR%\package.json" (
    echo %RED%错误: 前端目录中找不到 package.json%RESET%
    echo %YELLOW%请确保前端项目已初始化%RESET%
    echo.
    pause
    exit /b 1
)

echo %GREEN%启动前端开发服务器...%RESET%
echo %GRAY%前端目录: %FRONTEND_DIR%%RESET%

REM Start npm run dev in a new window
start "Leaf Frontend Dev Server" /D "%FRONTEND_DIR%" cmd /c "npm run dev"

echo %GRAY%等待前端服务器启动... (5秒)%RESET%
timeout /t 5 /nobreak > nul

REM Verify frontend is running
call :CheckPort %FRONTEND_PORT%
if %ERRORLEVEL% equ 0 (
    echo %GREEN%前端开发服务器已启动%RESET%
) else (
    echo %YELLOW%警告: 前端服务器可能仍在启动中%RESET%
    echo %GRAY%请稍后手动访问 http://localhost:%FRONTEND_PORT%%RESET%
)

REM Step 5: Open browser
:OpenBrowser
echo.
echo %CYAN%在浏览器中打开应用...%RESET%
start http://localhost:%FRONTEND_PORT%

REM Display status summary
echo.
echo %CYAN%================================================%RESET%
echo %GREEN%Leaf 地图服务系统状态:%RESET%
echo %GRAY%后端API: http://localhost:%BACKEND_PORT%%RESET%
echo %GRAY%前端应用: http://localhost:%FRONTEND_PORT%%RESET%
echo %YELLOW%按任意键停止所有服务或关闭窗口继续运行%RESET%
echo %CYAN%================================================%RESET%
echo.

REM Wait for user input before exiting
pause > nul

REM Cleanup (optional - user can manually kill processes)
echo %YELLOW%提示: 后端和前端进程仍在后台运行%RESET%
echo %GRAY%如需停止，请手动结束相关进程:%RESET%
echo %GRAY%  - server.exe / map_server.exe (后端)%RESET%
echo %GRAY%  - npm / node (前端)%RESET%
echo.

exit /b 0

REM Helper function to check if a process is listening on a port
:IsPortListening
setlocal
set PORT=%1
netstat -ano | findstr ":%PORT% " | findstr "LISTENING" > nul
if %ERRORLEVEL% equ 0 (endlocal & exit /b 0) else (endlocal & exit /b 1)