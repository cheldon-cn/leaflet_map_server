@echo off
setlocal EnableDelayedExpansion

set SCRIPT_DIR=%~dp0
if "%SCRIPT_DIR:~-1%"=="\" set SCRIPT_DIR=%SCRIPT_DIR:~0,-1%

set JAVA_HOME=D:\Program Files\Java\jdk1.8.0_162

if not exist "%JAVA_HOME%\bin\java.exe" (
    echo Error: Java not found at %JAVA_HOME%
    echo Please set JAVA_HOME environment variable or modify this script.
    pause
    exit /b 1
)

set JAVA_EXE=%JAVA_HOME%\bin\java.exe

set CLASSPATH=%SCRIPT_DIR%\echart-app-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\echart-facade-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\echart-ui-render-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\echart-theme-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\echart-plugin-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\echart-ui-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\echart-workspace-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\echart-route-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\echart-ais-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\echart-alarm-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\echart-data-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\echart-render-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\echart-i18n-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\echart-event-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\echart-core-1.2.1-alpha.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\javawrapper-2.1.0.jar
set CLASSPATH=%CLASSPATH%;%SCRIPT_DIR%\fxribbon-2.1.0.jar

set PATH=%SCRIPT_DIR%;%PATH%

echo Starting E-Chart Display Alarm Application...
echo.

"%JAVA_EXE%" -Djava.library.path=%SCRIPT_DIR% -cp "%CLASSPATH%" cn.cycle.echart.app.EChartApp %*

endlocal
