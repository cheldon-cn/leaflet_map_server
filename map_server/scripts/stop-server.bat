@echo off
REM Leaf Map Server Stop Script for Windows
REM This script stops the backend server and frontend dev server

chcp 65001 >nul 2>&1

echo ================================================================
echo Leaf Map Server Stop Script (Windows Batch)
echo ================================================================
echo.

REM Default settings
set "SERVER_PORT=8080"
set "FRONTEND_PORT=3000"

echo Stopping backend server...

REM Find and kill backend server processes
echo Checking for backend processes on port %SERVER_PORT%...
for /f "tokens=5" %%a in ('netstat -ano ^| findstr ":%SERVER_PORT%"') do (
    echo Found backend process PID: %%a
    taskkill /F /PID %%a >nul 2>&1
    if !errorlevel! equ 0 (
        echo Backend process stopped successfully
    ) else (
        echo Failed to stop backend process, trying alternative method...
        taskkill /F /T /PID %%a >nul 2>&1
    )
)

echo.
echo Stopping frontend server...

REM Find and kill frontend server processes
echo Checking for frontend processes on port %FRONTEND_PORT%...
for /f "tokens=5" %%a in ('netstat -ano ^| findstr ":%FRONTEND_PORT%"') do (
    echo Found frontend process PID: %%a
    taskkill /F /T /PID %%a >nul 2>&1
    if !errorlevel! equ 0 (
        echo Frontend process stopped successfully
    )
)

REM Additional cleanup: kill npm and node processes that might be left
echo.
echo Cleaning up npm and node processes...
for /f "tokens=2" %%p in ('tasklist ^| findstr /i "node.exe"') do (
    taskkill /F /PID %%p >nul 2>&1
)

for /f "tokens=2" %%p in ('tasklist ^| findstr /i "npm.exe"') do (
    taskkill /F /PID %%p >nul 2>&1
)

REM Kill server.exe/map_server.exe processes
echo Cleaning up server processes...
for /f "tokens=2" %%p in ('tasklist ^| findstr /i "server.exe"') do (
    taskkill /F /PID %%p >nul 2>&1
)

for /f "tokens=2" %%p in ('tasklist ^| findstr /i "map_server.exe"') do (
    taskkill /F /PID %%p >nul 2>&1
)

echo.
echo ================================================================
echo Cleanup completed!
echo All server processes have been stopped
echo ================================================================
exit /b 0
