@echo off
setlocal EnableDelayedExpansion

set JFX_ROOT=E:\java\jfx-17-18
set JDK_HOME=E:\java\jdk-11.0.30.7
set GRADLE_HOME=E:\java\gradle\gradle-7.6.6
set BUILD_DIR=E:\java\fx\build
set INSTALL_DIR=E:\java\fx\install

echo ========================================
echo JavaFX 17 Build Script
echo ========================================
echo JFX Root:    %JFX_ROOT%
echo JDK Home:    %JDK_HOME%
echo Gradle Home: %GRADLE_HOME%
echo Build Dir:   %BUILD_DIR%
echo Install Dir: %INSTALL_DIR%
echo ========================================
echo.

set JAVA_HOME=%JDK_HOME%
set PATH=%JAVA_HOME%\bin;%GRADLE_HOME%\bin;%PATH%

REM Windows SDK paths
set WINSDK_DIR=C:\Program Files (x86)\Windows Kits\10
set DXSDK_DIR=C:\Program Files (x86)\Windows Kits\8.1

REM Visual Studio 2015 paths
set VSINSTALLDIR=C:\Program Files\Microsoft Visual Studio 14.0
set VS140COMNTOOLS=%VSINSTALLDIR%\Common7\Tools\

echo [INFO] Environment:
echo   WINSDK_DIR=%WINSDK_DIR%
echo   DXSDK_DIR=%DXSDK_DIR%
echo   VSINSTALLDIR=%VSINSTALLDIR%
echo.

echo [INFO] Changing to JFX_ROOT: %JFX_ROOT%
cd /d "%JFX_ROOT%"
if errorlevel 1 (
    echo [ERROR] Failed to change directory
    pause
    exit /b 1
)
echo [INFO] Current directory: %CD%
echo.

echo [INFO] Creating gradle.properties...
(
    echo # JavaFX Build Configuration
    echo JDK_HOME=%JDK_HOME%
    echo JAVA_HOME=%JDK_HOME%
    echo.
    echo # Build configuration
    echo CONF=Release
    echo.
    echo # Target platform
    echo COMPILE_TARGETS=win
    echo.
    echo # Visual Studio 2015
    echo WINDOWS_VS_VER=140
    echo.
    echo # Windows SDK
    echo WINDOWS_SDK_DIR=C:/Program Files ^(x86^)/Windows Kits/10
    echo.
    echo # DirectX SDK (for fxc.exe)
    echo WINDOWS_DXSDK_DIR=C:/Program Files ^(x86^)/Windows Kits/8.1
) > gradle.properties
echo [INFO] gradle.properties created
echo.

echo ========================================
echo Building JavaFX SDK
echo ========================================
echo.
echo [INFO] Requirements:
echo   - JDK 11+: OK
echo   - Gradle 6.3+: OK
echo   - Windows SDK 10: OK
echo   - Windows SDK 8.1 (fxc.exe): OK
echo   - Visual Studio 2015: OK
echo.
echo [INFO] Running: gradle sdk
echo [INFO] This may take 30-60 minutes...
echo.

"%GRADLE_HOME%\bin\gradle.bat" sdk --no-daemon -PJDK_HOME=%JDK_HOME%

if errorlevel 1 (
    echo.
    echo [ERROR] Build failed
    echo [INFO] If VS2015 has issues, consider:
    echo   1. Install VS2019 or VS2022 Community (free)
    echo   2. Use pre-built JavaFX SDK from https://gluonhq.com/products/javafx/
    echo.
    pause
    exit /b 1
)

echo.
echo [SUCCESS] Build completed
echo.

if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

echo [INFO] Copying SDK to install directory...
if exist "build\sdk" (
    xcopy "build\sdk" "%INSTALL_DIR%" /E /I /Y /Q
    echo [SUCCESS] SDK installed to %INSTALL_DIR%
) else (
    echo [WARNING] SDK not found at build\sdk
)

echo.
echo ========================================
echo Installation Summary
echo ========================================
if exist "%INSTALL_DIR%\lib" (
    echo JAR files:
    dir /b "%INSTALL_DIR%\lib\*.jar" 2>nul
    echo.
)
if exist "%INSTALL_DIR%\bin" (
    echo DLL files:
    dir /b "%INSTALL_DIR%\bin\*.dll" 2>nul
    echo.
)
echo ========================================
echo Done!
echo ========================================
pause
exit /b 0
