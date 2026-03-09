@echo off
REM Leaf Map Server Startup Script for Windows
REM This script starts the backend server, frontend dev server, and opens browser

chcp 65001 >nul 2>&1

REM Get script directory
set "SCRIPT_ROOT=%~dp0"
set "SCRIPT_ROOT=%SCRIPT_ROOT:~0,-1%"

echo ================================================================
echo Leaf Map Server Startup Script (Windows Batch)
echo ================================================================
echo Script root: %SCRIPT_ROOT%
echo.

REM Default settings
set "SERVER_PORT=8080"
set "FRONTEND_PORT=3000"
set "SERVER_EXE_PATH=%SCRIPT_ROOT%\..\build\bin\server.exe"
set "CONFIG_PATH=%SCRIPT_ROOT%\..\build\config.example.json"
set "FRONTEND_DIR=%SCRIPT_ROOT%\..\web"
set "PID_FILE=%TEMP%\leaf_map_server_pids.txt"
set "BACKEND_PID=0"
set "FRONTEND_PID=0"

REM Check for -SkipBackend flag
set "SKIP_BACKEND=0"
if "%~1"=="-SkipBackend" (
    set "SKIP_BACKEND=1"
)

REM Start backend server
if "%SKIP_BACKEND%"=="1" goto :start_frontend

echo Starting backend server...

REM Check if server executable exists
if exist "%SERVER_EXE_PATH%" (
    echo Found backend: %SERVER_EXE_PATH%
) else if exist "%SCRIPT_ROOT%\..\build\bin\map_server.exe" (
    set "SERVER_EXE_PATH=%SCRIPT_ROOT%\..\build\bin\map_server.exe"
    echo Found backend: %SERVER_EXE_PATH%
) else (
    echo ERROR: Backend server not found
    echo Please build the project first
    exit /b 1
)

REM Check config file
if exist "%CONFIG_PATH%" (
    echo Using config: %CONFIG_PATH%
    start "" "%SERVER_EXE_PATH%" --config "%CONFIG_PATH%"
) else (
    echo WARNING: Config not found, using defaults
    start "" "%SERVER_EXE_PATH%"
)

echo Backend server started
echo Waiting for backend to be ready...
timeout /t 3 /nobreak >nul

REM Check if backend is responding
powershell -Command "try { $response = Invoke-WebRequest -Uri 'http://localhost:%SERVER_PORT%/health' -TimeoutSec 3 -UseBasicParsing -ErrorAction Stop; exit 0 } catch { exit 1 }"
if %errorlevel% equ 0 (
    echo Backend server is ready!
) else (
    echo WARNING: Backend health check failed, continuing anyway...
)

:start_frontend
echo.
echo Starting frontend development server...

REM Check frontend directory
if exist "%FRONTEND_DIR%\package.json" (
    echo Found frontend: %FRONTEND_DIR%
) else (
    echo ERROR: Frontend directory not found
    exit /b 1
)

REM Check npm
where npm >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: npm not found
    exit /b 1
)

REM Install dependencies if needed
if not exist "%FRONTEND_DIR%\node_modules" (
    echo Installing dependencies...
    pushd "%FRONTEND_DIR%"
    call npm install
    popd
)

REM Start frontend server
pushd "%FRONTEND_DIR%"
start /B cmd /C "npm run dev < nul > nul 2>&1"
popd

echo Frontend server started
timeout /t 5 /nobreak >nul

:open_browser
echo.
echo Opening browser...
start "" http://localhost:%FRONTEND_PORT%
echo Browser opened

echo.
echo ================================================================
echo Leaf Map Server System Successfully Started!
echo Backend API: http://localhost:%SERVER_PORT%
echo Frontend App: http://localhost:%FRONTEND_PORT%
echo Press Ctrl+C to stop all services
echo ================================================================

REM Cleanup function
:cleanup
echo.
echo Cleaning up processes...
if %BACKEND_PID% gtr 0 (
    echo Stopping backend server PID: %BACKEND_PID%
    taskkill /F /T /PID %BACKEND_PID% >nul 2>&1
)
if %FRONTEND_PID% gtr 0 (
    echo Stopping frontend server PID: %FRONTEND_PID%
    taskkill /F /T /PID %FRONTEND_PID% >nul 2>&1
)
if exist "%PID_FILE%" del "%PID_FILE%" >nul 2>&1
echo Cleanup completed
exit /b 0

REM Keep running
:keep_running
timeout /t 1 /nobreak >nul
goto keep_running
