@echo off
setlocal EnableDelayedExpansion

set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..\
set CODE_DIR=%SCRIPT_DIR%
set BUILD_DIR=%PROJECT_ROOT%build
set INSTALL_DIR=%PROJECT_ROOT%install

echo ========================================
echo C++ Modules Build Script v2.0
echo ========================================
echo Project Root: %PROJECT_ROOT%
echo Code Dir:     %CODE_DIR%
echo Build Dir:    %BUILD_DIR%
echo Install Dir:  %INSTALL_DIR%
echo ========================================
echo.

where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake not found in PATH
    echo Please install CMake and add to PATH
    exit /b 1
)

echo [INFO] CMake version:
cmake --version
echo.

if not exist "%BUILD_DIR%" (
    echo [INFO] Creating build directory: %BUILD_DIR%
    mkdir "%BUILD_DIR%"
)

if not exist "%INSTALL_DIR%" (
    echo [INFO] Creating install directory: %INSTALL_DIR%
    mkdir "%INSTALL_DIR%"
)

set SUCCESS_COUNT=0
set FAIL_COUNT=0
set TOTAL_COUNT=15
set FAILED_MODULES=

echo ========================================
echo Phase 1: CMake Configure
echo ========================================
echo.

cd /d "%CODE_DIR%"
cmake -B "%BUILD_DIR%" -S . ^
    -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DENABLE_TESTS=OFF ^
    -DBUILD_SHARED_LIBS=ON

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configure failed
    exit /b 1
)

echo.
echo [SUCCESS] CMake configure complete
echo.

echo ========================================
echo Phase 2: Build and Install Modules
echo ========================================
echo.
echo Build Order (by dependency):
echo   1. ogc_base        - Base library (no deps)
echo   2. ogc_geometry    - Geometry library (no deps)
echo   3. chart_parser    - Chart parser (ogc_base)
echo   4. ogc_feature     - Feature library (ogc_geometry)
echo   5. ogc_layer       - Layer library (ogc_geometry, ogc_feature)
echo   6. ogc_database    - Database library (ogc_geometry)
echo   7. ogc_draw        - Draw library (ogc_geometry)
echo   8. ogc_proj        - Projection library (ogc_geometry)
echo   9. ogc_cache       - Cache library (ogc_geometry)
echo  10. ogc_symbology   - Symbology library (ogc_geometry, ogc_feature, ogc_draw)
echo  11. ogc_graph       - Graph library (multiple deps)
echo  12. ogc_alert       - Alert library (ogc_geometry, ogc_graph, ogc_base)
echo  13. alert_system    - Alert system (ogc_alert, ogc_geometry)
echo  14. ogc_navi        - Navigation library (ogc_geometry)
echo  15. ogc_chart_c_api - C API (all modules)
echo.

echo [1/15] Building ogc_base...
call :BuildAndInstall ogc_base
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! ogc_base )

echo.
echo [2/15] Building ogc_geometry...
call :BuildAndInstall ogc_geometry
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! ogc_geometry )

echo.
echo [3/15] Building chart_parser...
call :BuildAndInstall chart_parser
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! chart_parser )

echo.
echo [4/15] Building ogc_feature...
call :BuildAndInstall ogc_feature
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! ogc_feature )

echo.
echo [5/15] Building ogc_layer...
call :BuildAndInstall ogc_layer
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! ogc_layer )

echo.
echo [6/15] Building ogc_database...
call :BuildAndInstall ogc_database
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! ogc_database )

echo.
echo [7/15] Building ogc_draw...
call :BuildAndInstall ogc_draw
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! ogc_draw )

echo.
echo [8/15] Building ogc_proj...
call :BuildAndInstall ogc_proj
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! ogc_proj )

echo.
echo [9/15] Building ogc_cache...
call :BuildAndInstall ogc_cache
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! ogc_cache )

echo.
echo [10/15] Building ogc_symbology...
call :BuildAndInstall ogc_symbology
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! ogc_symbology )

echo.
echo [11/15] Building ogc_graph...
call :BuildAndInstall ogc_graph
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! ogc_graph )

echo.
echo [12/15] Building ogc_alert...
call :BuildAndInstall ogc_alert
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! ogc_alert )

echo.
echo [13/15] Building alert_system...
call :BuildAndInstall alert_system
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! alert_system )

echo.
echo [14/15] Building ogc_navi...
call :BuildAndInstall ogc_navi
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! ogc_navi )

echo.
echo [15/15] Building ogc_chart_c_api...
call :BuildAndInstall ogc_chart_c_api
if !ERRORLEVEL! EQU 0 ( set /a SUCCESS_COUNT+=1 ) else ( set /a FAIL_COUNT+=1 && set FAILED_MODULES=!FAILED_MODULES! ogc_chart_c_api )

echo.
echo ========================================
echo Build Summary
echo ========================================
echo Success: %SUCCESS_COUNT% / %TOTAL_COUNT%
echo Failed:  %FAIL_COUNT% / %TOTAL_COUNT%
if not "%FAILED_MODULES%"=="" (
    echo Failed modules:%FAILED_MODULES%
)
echo ========================================

if %FAIL_COUNT% GTR 0 (
    echo [WARNING] Some modules failed, check error logs above
    exit /b 1
)

echo.
echo ========================================
echo Installation Summary
echo ========================================
echo.

if exist "%INSTALL_DIR%\bin" (
    echo DLL/EXE files in %INSTALL_DIR%\bin:
    dir /b "%INSTALL_DIR%\bin\*.dll" 2>nul
    dir /b "%INSTALL_DIR%\bin\*.exe" 2>nul
    echo.
)

if exist "%INSTALL_DIR%\lib" (
    echo LIB files in %INSTALL_DIR%\lib:
    dir /b "%INSTALL_DIR%\lib\*.lib" 2>nul
    echo.
)

if exist "%INSTALL_DIR%\include\ogc" (
    echo Header directories in %INSTALL_DIR%\include\ogc:
    dir /b /ad "%INSTALL_DIR%\include\ogc" 2>nul
    echo.
)

echo ========================================
echo All Done!
echo ========================================
echo Output directories:
echo   - DLL/EXE: %INSTALL_DIR%\bin\
echo   - LIB:     %INSTALL_DIR%\lib\
echo   - Headers: %INSTALL_DIR%\include\ogc\
echo ========================================
exit /b 0

:BuildAndInstall
set TARGET_NAME=%~1

echo [BUILD] %TARGET_NAME%

cmake --build "%BUILD_DIR%" --config Release --target %TARGET_NAME% -- /m /nologo

if %ERRORLEVEL% NEQ 0 (
    echo [FAILED] %TARGET_NAME% build failed
    exit /b 1
)

echo [INSTALL] %TARGET_NAME%

cmake --install "%BUILD_DIR%" --component %TARGET_NAME% 2>nul
if %ERRORLEVEL% NEQ 0 (
    cmake --install "%BUILD_DIR%" 2>nul
)

echo [SUCCESS] %TARGET_NAME% built and installed successfully
exit /b 0

endlocal
