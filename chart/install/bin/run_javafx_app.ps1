$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

$JavaHome = "D:\Program Files\Java\jdk1.8.0_162"
$JavaExe = Join-Path $JavaHome "bin\java.exe"

if (-not (Test-Path $JavaExe)) {
    Write-Host "Error: Java not found at $JavaHome"
    Write-Host "Please set JAVA_HOME environment variable or modify this script."
    Read-Host "Press Enter to exit"
    exit 1
}

$Classpath = (Join-Path $ScriptDir "javafx-app-1.0.0.jar") + ";" + (Join-Path $ScriptDir "javawrapper-1.0.0.jar")

$env:PATH = "$ScriptDir;$env:PATH"

& $JavaExe -D"java.library.path=$ScriptDir" -cp $Classpath cn.cycle.app.MainApp $args
