@echo off
setlocal EnableDelayedExpansion

set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..\..\
set GRADLE_CMD=E:\java\gradle\gradle-7.6.6\bin\gradle.bat
set INSTALL_DIR=%PROJECT_ROOT%install\bin\
set BUILD_ROOT=%PROJECT_ROOT%build\ecn\
set JAR_VERSION=1.2.1-alpha
set JAVA_HOME="E:\java\jdk-11.0.30.7"

echo ========================================
echo EChart JAR Build Script v2.0
echo ========================================
echo Project Root: %PROJECT_ROOT%
echo Gradle Cmd: %GRADLE_CMD%
echo Install Dir: %INSTALL_DIR%
echo JAR Version: %JAR_VERSION%
echo JAVA_HOME: %JAVA_HOME%
echo ========================================
echo.

if not exist "%GRADLE_CMD%" (
    echo [ERROR] Gradle wrapper not found: %GRADLE_CMD%
    exit /b 1
)

if not exist "%JAVA_HOME%\bin\java.exe" (
    echo [ERROR] Invalid JAVA_HOME: %JAVA_HOME%
    echo Please check JDK 11 installation
    exit /b 1
)

if not exist "%INSTALL_DIR%" (
    echo [INFO] Creating install dir: %INSTALL_DIR%
    mkdir "%INSTALL_DIR%"
)

set SUCCESS_COUNT=0
set FAIL_COUNT=0
set TOTAL_COUNT=16

echo ========================================
echo Building modules (by dependency layer)
echo ========================================
echo.

echo [Ribbon] Building fxribbon ...
call :BuildModule fxribbon ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo [Layer 0] Building javawrapper ...
call :BuildModule javawrapper ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo [Layer 0] Building echart-core ...
call :BuildModule echart-core ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo.
echo [Layer 1] Building echart-i18n ...
call :BuildModule echart-i18n ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo.
echo [Layer 2] Building echart-render ...
call :BuildModule echart-render ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo.
echo [Layer 2] Building echart-data ...
call :BuildModule echart-data ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo.
echo [Layer 3] Building echart-alarm ...
call :BuildModule echart-alarm ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo.
echo [Layer 3] Building echart-ais ...
call :BuildModule echart-ais ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo.
echo [Layer 3] Building echart-route ...
call :BuildModule echart-route ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo.
echo [Layer 3] Building echart-workspace ...
call :BuildModule echart-workspace ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo.
echo [Layer 4] Building echart-plugin ...
call :BuildModule echart-plugin ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo.
echo [Layer 4] Building echart-ui-render ...
call :BuildModule echart-ui-render ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo.
echo [Layer 4] Building echart-theme ...
call :BuildModule echart-theme ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo.
echo [Layer 5] Building echart-facade ...
call :BuildModule echart-facade ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo.
echo [Layer 6] Building echart-ui ...
call :BuildModule echart-ui ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

echo.
echo [Layer 7] Building echart-app ...
call :BuildModule echart-app ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )

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
set MODULE_DIR=%~2
set MODULE_PATH=%PROJECT_ROOT%%MODULE_DIR%\%MODULE_NAME%

echo [BUILD] %MODULE_NAME%
echo   Path: %MODULE_PATH%

if not exist "%MODULE_PATH%" (
    echo [ERROR] Module dir not found: %MODULE_PATH%
    exit /b 1
)

call "%GRADLE_CMD%" -p "%MODULE_PATH%" build -x test  --parallel

if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] %MODULE_NAME% build complete
) else (
    echo [FAILED] %MODULE_NAME% build failed
)
exit /b %ERRORLEVEL%

:CopyJars
set COPIED_COUNT=0

for %%M in (javawrapper fxribbon echart-core echart-i18n echart-render echart-data echart-alarm echart-ais echart-route echart-workspace echart-plugin echart-ui-render echart-theme echart-facade echart-ui echart-app) do (
    if "%%M"=="javawrapper" (
        set JAR_VER=2.1.0
    ) else if "%%M"=="fxribbon" (
        set JAR_VER=2.1.0
    ) else (
        set JAR_VER=%JAR_VERSION%
    )
    
    if exist "%BUILD_ROOT%%%M\libs\%%M-!JAR_VER!.jar" (
        copy /Y "%BUILD_ROOT%%%M\libs\%%M-!JAR_VER!.jar" "%INSTALL_DIR%" >nul
        echo [COPY] %%M-!JAR_VER!.jar
        set /a COPIED_COUNT+=1
    ) else if exist "%BUILD_ROOT%%%M\libs\%%M-*.jar" (
        for %%f in ("%BUILD_ROOT%%%M\libs\%%M-*.jar") do (
            echo %%~nf | findstr /C:"-sources" >nul
            if errorlevel 1 (
                copy /Y "%%f" "%INSTALL_DIR%" >nul
                echo [COPY] %%~nxf
                set /a COPIED_COUNT+=1
            )
        )
    )
)

echo.
echo Copied %COPIED_COUNT% JAR files to %INSTALL_DIR%
exit /b 0

endlocal
