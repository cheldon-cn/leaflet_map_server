@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo  ECN EChart Build Script v1.0
echo ========================================
echo.

set "JAVA_HOME=F:\enc\java"
set "GRADLE_CMD=gradle\gradle-4.5.1\bin\gradle.bat"
set "BUILD_ROOT=build\ecn"
set "INSTALL_DIR=install\bin"

echo [Environment]
echo JAVA_HOME: %JAVA_HOME%
echo Gradle: %GRADLE_CMD%
echo Build Dir: %BUILD_ROOT%
echo Install Dir: %INSTALL_DIR%
echo.

if not exist "%JAVA_HOME%\bin\java.exe" (
    echo [ERROR] Invalid JAVA_HOME: %JAVA_HOME%
    exit /b 1
)

if not exist "%GRADLE_CMD%" (
    echo [ERROR] Gradle not found: %GRADLE_CMD%
    exit /b 1
)

echo ========================================
echo  Phase 1: Build All Modules
echo ========================================
echo.

echo [Layer 0] Building echart-core...
call "%GRADLE_CMD%" -p ecn\echart-core build -x test
if errorlevel 1 goto :build_error

echo.
echo [Layer 1] Building echart-event, echart-i18n...
call "%GRADLE_CMD%" -p ecn\echart-event build -x test
call "%GRADLE_CMD%" -p ecn\echart-i18n build -x test

echo.
echo [Layer 2] Building echart-render, echart-data...
call "%GRADLE_CMD%" -p ecn\echart-render build -x test
call "%GRADLE_CMD%" -p ecn\echart-data build -x test

echo.
echo [Layer 3] Building echart-alarm, echart-ais, echart-route, echart-workspace...
call "%GRADLE_CMD%" -p ecn\echart-alarm build -x test
call "%GRADLE_CMD%" -p ecn\echart-ais build -x test
call "%GRADLE_CMD%" -p ecn\echart-route build -x test
call "%GRADLE_CMD%" -p ecn\echart-workspace build -x test

echo.
echo [Layer 4] Building echart-ui...
call "%GRADLE_CMD%" -p ecn\echart-ui build -x test

echo.
echo [Layer 5] Building echart-ui-render, echart-theme, echart-plugin...
call "%GRADLE_CMD%" -p ecn\echart-ui-render build -x test
call "%GRADLE_CMD%" -p ecn\echart-theme build -x test
call "%GRADLE_CMD%" -p ecn\echart-plugin build -x test

echo.
echo [Layer 6] Building echart-facade...
call "%GRADLE_CMD%" -p ecn\echart-facade build -x test

echo.
echo [Layer 7] Building echart-app...
call "%GRADLE_CMD%" -p ecn\echart-app build -x test

echo.
echo ========================================
echo  Phase 2: Install to %INSTALL_DIR%
echo ========================================
echo.

if not exist "%INSTALL_DIR%" (
    mkdir "%INSTALL_DIR%"
    echo Created install directory: %INSTALL_DIR%
)

echo Installing JAR files...
for %%m in (echart-core echart-event echart-i18n echart-render echart-data echart-alarm echart-ais echart-route echart-workspace echart-ui echart-ui-render echart-theme echart-plugin echart-facade echart-app) do (
    if exist "%BUILD_ROOT%\%%m\libs\%%m-*.jar" (
        for %%f in ("%BUILD_ROOT%\%%m\libs\%%m-*.jar") do (
            echo %%~nf | findstr /C:"-sources" >nul
            if errorlevel 1 (
                copy /Y "%%f" "%INSTALL_DIR%\" >nul
                echo   Installed: %%~nxf
            )
        )
    ) else (
        echo   Skipped: %%m - No build output
    )
)

echo.
echo ========================================
echo  Build Complete
echo ========================================
echo JAR files installed to: %INSTALL_DIR%
dir /b "%INSTALL_DIR%\*.jar" 2>nul
echo.

goto :end

:build_error
echo.
echo [ERROR] Build failed
exit /b 1

:end
endlocal
