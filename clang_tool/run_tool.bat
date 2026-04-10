@echo off
REM SDK API Index and Validation Tool - Quick Start Script for Windows
REM This script runs the complete workflow

echo ========================================
echo SDK API Index and Validation Tool
echo ========================================
echo.

REM Check Python installation
python --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Python is not installed or not in PATH
    echo Please install Python 3.7+ from https://www.python.org/
    pause
    exit /b 1
)

REM Check libclang installation
python -c "from clang.cindex import Index" >nul 2>&1
if errorlevel 1 (
    echo ERROR: python-libclang is not installed
    echo Installing python-libclang...
    pip install libclang
    if errorlevel 1 (
        echo Failed to install libclang. Please install manually: pip install libclang
        pause
        exit /b 1
    )
)

echo Step 1: Building API Index...
python main.py --index-only
if errorlevel 1 (
    echo ERROR: Failed to build API index
    pause
    exit /b 1
)

echo.
echo Step 2: Extracting API Examples...
python example_extractor.py
if errorlevel 1 (
    echo ERROR: Failed to extract examples
    pause
    exit /b 1
)

echo.
echo Step 3: Validating APIs...
python main.py --validate-only --skip-index
if errorlevel 1 (
    echo ERROR: Failed to validate APIs
    pause
    exit /b 1
)

echo.
echo Step 4: Generating Report...
python main.py --report-only
if errorlevel 1 (
    echo ERROR: Failed to generate report
    pause
    exit /b 1
)

echo.
echo ========================================
echo Workflow Complete!
echo ========================================
echo.
echo Output files:
echo   - API Index: index\api_index.json
echo   - Examples:  index\api_examples.json
echo   - Results:   reports\validation_results.json
echo   - Report:    reports\api_validation_report.md
echo.

REM Open report
set /p OPEN_REPORT="Open report? (Y/N): "
if /i "%OPEN_REPORT%"=="Y" (
    start reports\api_validation_report.md
)

pause
