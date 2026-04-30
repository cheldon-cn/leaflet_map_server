@echo off
setlocal EnableDelayedExpansion

set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..\..\
set GRADLE_CMD=E:\java\gradle\gradle-7.6.6\bin\gradle.bat
set INSTALL_DIR=%PROJECT_ROOT%install\bin\
set BUILD_ROOT=%PROJECT_ROOT%build\ecn\
set JAR_VERSION=1.2.1-alpha
set JAVA_HOME="E:\java\jdk-11.0.30.7"

set BUILD_MODE=all
set TARGET_LAYER=-1
set TARGET_MODULES=

call :ParseArgs %*

echo ========================================
echo EChart JAR Build Script v3.0
echo ========================================
echo Project Root: %PROJECT_ROOT%
echo Gradle Cmd: %GRADLE_CMD%
echo Install Dir: %INSTALL_DIR%
echo JAR Version: %JAR_VERSION%
echo JAVA_HOME: %JAVA_HOME%
echo Build Mode: %BUILD_MODE%
if defined TARGET_MODULES echo Target Modules: %TARGET_MODULES%
if %TARGET_LAYER% GTR -1 echo Min Layer: %TARGET_LAYER%
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
set TOTAL_COUNT=0

echo ========================================
echo Building modules (by dependency layer)
echo ========================================
echo.

call :BuildByLayer

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

:ParseArgs
:ParseLoop
if "%~1"=="" goto :ParseDone
if /i "%~1"=="-m" (
    set BUILD_MODE=module
    if not "%~2"=="" (
        if defined TARGET_MODULES (
            set TARGET_MODULES=%TARGET_MODULES% %~2
        ) else (
            set TARGET_MODULES=%~2
        )
        shift
    )
    shift
    goto :ParseLoop
)
if /i "%~1"=="--module" (
    set BUILD_MODE=module
    if not "%~2"=="" (
        if defined TARGET_MODULES (
            set TARGET_MODULES=%TARGET_MODULES% %~2
        ) else (
            set TARGET_MODULES=%~2
        )
        shift
    )
    shift
    goto :ParseLoop
)
if /i "%~1"=="-l" (
    set BUILD_MODE=layer
    if not "%~2"=="" (
        set TARGET_LAYER=%~2
        shift
    )
    shift
    goto :ParseLoop
)
if /i "%~1"=="--layer" (
    set BUILD_MODE=layer
    if not "%~2"=="" (
        set TARGET_LAYER=%~2
        shift
    )
    shift
    goto :ParseLoop
)
if /i "%~1"=="-h" goto :ShowHelp
if /i "%~1"=="--help" goto :ShowHelp
shift
goto :ParseLoop
:ParseDone
exit /b 0

:ShowHelp
echo.
echo Usage: compile_echart_jars.bat [OPTIONS]
echo.
echo Options:
echo   -m, --module MODULE   Build specific module(s), can be used multiple times
echo   -l, --layer N         Build modules at layer N and above (0-7)
echo   -h, --help            Show this help message
echo.
echo Examples:
echo   compile_echart_jars.bat                    Build all modules
echo   compile_echart_jars.bat -m echart-core     Build only echart-core
echo   compile_echart_jars.bat -m echart-core -m echart-ui  Build multiple modules
echo   compile_echart_jars.bat -l 5               Build layer 5, 6, 7 modules
echo.
echo Module Layers:
echo   Ribbon: fxribbon
echo   Layer 0: javawrapper, echart-core
echo   Layer 1: echart-i18n
echo   Layer 2: echart-render, echart-data
echo   Layer 3: echart-alarm, echart-ais, echart-route, echart-workspace
echo   Layer 4: echart-plugin, echart-ui-render, echart-theme
echo   Layer 5: echart-facade
echo   Layer 6: echart-ui
echo   Layer 7: echart-app
echo.
exit /b 0

:BuildByLayer
if "%BUILD_MODE%"=="module" (
    for %%M in (%TARGET_MODULES%) do (
        call :BuildSingleModule %%M
    )
    exit /b 0
)

if "%BUILD_MODE%"=="layer" (
    if %TARGET_LAYER% LEQ 0 call :BuildLayer0
    if %TARGET_LAYER% LEQ 1 call :BuildLayer1
    if %TARGET_LAYER% LEQ 2 call :BuildLayer2
    if %TARGET_LAYER% LEQ 3 call :BuildLayer3
    if %TARGET_LAYER% LEQ 4 call :BuildLayer4
    if %TARGET_LAYER% LEQ 5 call :BuildLayer5
    if %TARGET_LAYER% LEQ 6 call :BuildLayer6
    if %TARGET_LAYER% LEQ 7 call :BuildLayer7
    exit /b 0
)

call :BuildRibbon
call :BuildLayer0
call :BuildLayer1
call :BuildLayer2
call :BuildLayer3
call :BuildLayer4
call :BuildLayer5
call :BuildLayer6
call :BuildLayer7
exit /b 0

:BuildSingleModule
set MODULE_NAME=%~1
echo [Single] Building %MODULE_NAME% ...
call :BuildModule %MODULE_NAME% ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
exit /b 0

:BuildRibbon
echo [Ribbon] Building fxribbon ...
call :BuildModule fxribbon ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
exit /b 0

:BuildLayer0
echo.
echo [Layer 0] Building javawrapper ...
call :BuildModule javawrapper ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
echo [Layer 0] Building echart-core ...
call :BuildModule echart-core ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
exit /b 0

:BuildLayer1
echo.
echo [Layer 1] Building echart-i18n ...
call :BuildModule echart-i18n ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
exit /b 0

:BuildLayer2
echo.
echo [Layer 2] Building echart-render ...
call :BuildModule echart-render ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
echo.
echo [Layer 2] Building echart-data ...
call :BuildModule echart-data ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
exit /b 0

:BuildLayer3
echo.
echo [Layer 3] Building echart-alarm ...
call :BuildModule echart-alarm ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
echo.
echo [Layer 3] Building echart-ais ...
call :BuildModule echart-ais ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
echo.
echo [Layer 3] Building echart-route ...
call :BuildModule echart-route ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
echo.
echo [Layer 3] Building echart-workspace ...
call :BuildModule echart-workspace ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
exit /b 0

:BuildLayer4
echo.
echo [Layer 4] Building echart-plugin ...
call :BuildModule echart-plugin ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
echo.
echo [Layer 4] Building echart-ui-render ...
call :BuildModule echart-ui-render ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
echo.
echo [Layer 4] Building echart-theme ...
call :BuildModule echart-theme ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
exit /b 0

:BuildLayer5
echo.
echo [Layer 5] Building echart-facade ...
call :BuildModule echart-facade ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
exit /b 0

:BuildLayer6
echo.
echo [Layer 6] Building echart-ui ...
call :BuildModule echart-ui ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
exit /b 0

:BuildLayer7
echo.
echo [Layer 7] Building echart-app ...
call :BuildModule echart-app ecn
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 )
set /a TOTAL_COUNT+=1
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
call "%GRADLE_CMD%" -p "%MODULE_PATH%" build -x test --parallel
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
