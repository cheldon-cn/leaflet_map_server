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

set CLASSPATH=%SCRIPT_DIR%\javafx-app-1.0.0.jar;%SCRIPT_DIR%\javawrapper-1.0.0.jar

set PATH=%SCRIPT_DIR%;%PATH%

"%JAVA_EXE%" -Djava.library.path=%SCRIPT_DIR% -cp "%CLASSPATH%" cn.cycle.app.MainApp %*

endlocal
