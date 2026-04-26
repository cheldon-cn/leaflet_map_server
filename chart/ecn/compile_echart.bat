@echo off
setlocal EnableDelayedExpansion

echo ========================================
echo  ECN EChart Build Script v2.0
echo ========================================
echo.

set "JAVA_HOME=E:\java\jdk-11.0.30.7"
set "GRADLE_CMD=..\cycle\gradlew.bat"
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
    echo [ERROR] Gradle wrapper not found: %GRADLE_CMD%
    exit /b 1
)

echo ========================================
echo  Phase 1: Build All Modules
echo ========================================
echo.

echo [Ribbon] Building fxribbon...
call "%GRADLE_CMD%" -p ecn\fxribbon build -x test --no-daemon

echo.
echo [Layer 0] Building javawrapper, echart-core...
call "%GRADLE_CMD%" -p cycle\javawrapper build -x test --no-daemon
call "%GRADLE_CMD%" -p ecn\echart-core build -x test --no-daemon

echo.
echo [Layer 1] Building echart-i18n...
call "%GRADLE_CMD%" -p ecn\echart-i18n build -x test --no-daemon

echo.
echo [Layer 2] Building echart-render, echart-data...
call "%GRADLE_CMD%" -p ecn\echart-render build -x test --no-daemon
call "%GRADLE_CMD%" -p ecn\echart-data build -x test --no-daemon

echo.
echo [Layer 3] Building echart-alarm, echart-ais, echart-route...
call "%GRADLE_CMD%" -p ecn\echart-alarm build -x test --no-daemon
call "%GRADLE_CMD%" -p ecn\echart-ais build -x test --no-daemon
call "%GRADLE_CMD%" -p ecn\echart-route build -x test --no-daemon

echo.
echo [Layer 3] Building echart-workspace...
call "%GRADLE_CMD%" -p ecn\echart-workspace build -x test --no-daemon

echo.
echo [Layer 4] Building echart-plugin, echart-ui-render, echart-theme...
call "%GRADLE_CMD%" -p ecn\echart-plugin build -x test --no-daemon
call "%GRADLE_CMD%" -p ecn\echart-ui-render build -x test --no-daemon
call "%GRADLE_CMD%" -p ecn\echart-theme build -x test --no-daemon

echo.
echo [Layer 5] Building echart-facade...
call "%GRADLE_CMD%" -p ecn\echart-facade build -x test --no-daemon

echo.
echo [Layer 6] Building echart-ui...
call "%GRADLE_CMD%" -p ecn\echart-ui build -x test --no-daemon

echo.
echo [Layer 7] Building echart-app...
call "%GRADLE_CMD%" -p ecn\echart-app build -x test --no-daemon

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
for %%m in (javawrapper fxribbon echart-core echart-i18n echart-render echart-data echart-alarm echart-ais echart-route echart-workspace echart-plugin echart-ui-render echart-theme echart-facade echart-ui echart-app) do (
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

:end
endlocal
