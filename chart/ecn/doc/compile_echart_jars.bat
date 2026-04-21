@echo off
setlocal EnableDelayedExpansion

set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..\..\
set GRADLE_CMD=%PROJECT_ROOT%cycle\gradle\gradle-4.5.1\gradle-4.5.1\bin\gradle.bat
set INSTALL_DIR=%PROJECT_ROOT%install\bin\
set BUILD_ROOT=%PROJECT_ROOT%build\ecn\
set JAR_VERSION=1.2.1-alpha

echo ========================================
echo EChart JAR Build Script
echo ========================================
echo Project Root: %PROJECT_ROOT%
echo Gradle Cmd: %GRADLE_CMD%
echo Install Dir: %INSTALL_DIR%
echo JAR Version: %JAR_VERSION%
echo ========================================
echo.

if not exist "%GRADLE_CMD%" (
    echo [ERROR] Gradle not found: %GRADLE_CMD%
    exit /b 1
)

if "%JAVA_HOME%"=="" (
    echo [WARNING] JAVA_HOME not set
    echo Example: set JAVA_HOME=D:\Program Files\buttercookies\java
    exit /b 1
)

echo [INFO] JAVA_HOME: %JAVA_HOME%
echo.

if not exist "%INSTALL_DIR%" (
    echo [INFO] Creating install dir: %INSTALL_DIR%
    mkdir "%INSTALL_DIR%"
)

set SUCCESS_COUNT=0
set FAIL_COUNT=0
set TOTAL_COUNT=15

echo ========================================
echo Building modules (by dependency layer)
echo ========================================
echo.

echo [ribbon 0] Building fxribbon ...
call :BuildModule fxribbon
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo [Layer 0] Building echart-core ...
call :BuildModule echart-core
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo [Layer 1] Building echart-event ...
call :BuildModule echart-event
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo [Layer 1] Building echart-i18n ...
call :BuildModule echart-i18n
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo [Layer 2] Building echart-render ...
call :BuildModule echart-render
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo [Layer 2] Building echart-data ...
call :BuildModule echart-data
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo [Layer 3] Building echart-alarm ...
call :BuildModule echart-alarm
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo [Layer 3] Building echart-ais ...
call :BuildModule echart-ais
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo [Layer 3] Building echart-route ...
call :BuildModule echart-route
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo [Layer 3] Building echart-workspace ...
call :BuildModule echart-workspace
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo [Layer 4] Building echart-ui ...
call :BuildModule echart-ui
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo [Layer 5] Building echart-ui-render ...
call :BuildModule echart-ui-render
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo [Layer 5] Building echart-theme ...
call :BuildModule echart-theme
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo [Layer 5] Building echart-plugin ...
call :BuildModule echart-plugin
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo [Layer 6] Building echart-facade ...
call :BuildModule echart-facade
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo [Layer 7] Building echart-app ...
call :BuildModule echart-app
if !ERRORLEVEL! EQU 0 (
    set /a SUCCESS_COUNT+=1
) else (
    set /a FAIL_COUNT+=1
)

echo.
echo ========================================
echo Build Complete
echo ========================================
echo Success: %SUCCESS_COUNT% / %TOTAL_COUNT%
echo Failed: %FAIL_COUNT% / %TOTAL_COUNT%
echo ========================================

if %FAIL_COUNT% GTR 0 (
    echo [WARNING] Some modules failed, check error logs
    exit /b 1
)

echo.
echo ========================================
echo Copying JAR files to install directory
echo ========================================
echo.

call :CopyJars

echo.
echo ========================================
echo All Done!
echo ========================================
echo JAR files installed to: %INSTALL_DIR%
echo ========================================
exit /b 0

:BuildModule
set MODULE_NAME=%~1
set MODULE_PATH=%PROJECT_ROOT%ecn\%MODULE_NAME%
set JAR_FILE=%BUILD_ROOT%%MODULE_NAME%\libs\%MODULE_NAME%-%JAR_VERSION%.jar

echo [BUILD] %MODULE_NAME%
echo   Path: %MODULE_PATH%

if not exist "%MODULE_PATH%" (
    echo [ERROR] Module dir not found: %MODULE_PATH%
    exit /b 1
)

call "%GRADLE_CMD%" -p "%MODULE_PATH%" build -x test -q

if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] %MODULE_NAME% build complete
    if exist "%JAR_FILE%" (
        echo   Output: %JAR_FILE%
    )
) else (
    echo [FAILED] %MODULE_NAME% build failed
)
exit /b %ERRORLEVEL%

:CopyJars
set COPIED_COUNT=0

for %%M in (echart-core echart-event echart-i18n echart-render echart-data echart-alarm echart-ais echart-route echart-workspace echart-ui echart-ui-render echart-theme echart-plugin echart-facade echart-app) do (
    set JAR_SRC=%BUILD_ROOT%%%M\libs\%%M-%JAR_VERSION%.jar
    if exist "!JAR_SRC!" (
        copy /Y "!JAR_SRC!" "%INSTALL_DIR%" >nul
        if !ERRORLEVEL! EQU 0 (
            echo [COPY] %%M-%JAR_VERSION%.jar
            set /a COPIED_COUNT+=1
        ) else (
            echo [FAILED] Copy %%M-%JAR_VERSION%.jar failed
        )
    ) else (
        echo [SKIP] %%M-%JAR_VERSION%.jar not found
    )
)

echo.
echo Copied %COPIED_COUNT% JAR files to %INSTALL_DIR%
exit /b 0

endlocal
