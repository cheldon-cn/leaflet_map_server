# Android编译配置清单

> **目标平台**: Android 12 (API Level 31)  
> **更新日期**: 2026-04-03  
> **NDK版本**: r25c

---

## 一、环境要求

### 1.1 必需工具

| 工具 | 版本要求 | 用途 | 状态 |
|------|----------|------|------|
| Android NDK | r25c (25.2.9519653) | 原生代码编译 | ✅ 已知路径: `D:\program\android\ndk-r25c` |
| Android SDK | API 31+ | Android开发工具包 | ⬜ 待确认路径 |
| CMake | 3.16+ | 构建系统 | ⬜ 待确认版本 |
| Ninja | 1.10+ | 构建工具 | ⬜ 待确认 |
| Gradle | 7.5+ | 构建自动化 | ⬜ 待确认版本 |
| JDK | 11 或 17 | Java开发环境 | ⬜ 待确认版本 |

### 1.2 可选工具

| 工具 | 版本要求 | 用途 |
|------|----------|------|
| Android Studio | 2022.1+ | IDE开发环境 |
| LLDB | 默认NDK版本 | 原生代码调试 |

---

## 二、SDK/NDK配置

### 2.1 Android SDK配置

```powershell
# SDK路径 (待确认)
$ANDROID_SDK_ROOT = "C:\Users\[用户名]\AppData\Local\Android\Sdk"

# 或自定义路径
$ANDROID_SDK_ROOT = "D:\program\android\sdk"
```

**必需SDK组件**:
- [ ] Platform Tools
- [ ] Build Tools 33.0.0+
- [ ] SDK Platform 31 (Android 12)
- [ ] SDK Platform 33 (Android 13, 用于编译)

### 2.2 Android NDK配置

```powershell
# NDK路径 (已知)
$ANDROID_NDK_HOME = "D:\program\android\ndk-r25c"

# 或使用SDK内置NDK
$ANDROID_NDK_HOME = "$ANDROID_SDK_ROOT\ndk\25.2.9519653"
```

**NDK工具链验证**:
```powershell
# 验证NDK版本
& "$ANDROID_NDK_HOME\ndk-build" --version

# 验证工具链
& "$ANDROID_NDK_HOME\toolchains\llvm\prebuilt\windows-x86_64\bin\clang++.exe" --version
```

---

## 三、CMake配置

### 3.1 基础配置

```cmake
# CMake最低版本
cmake_minimum_required(VERSION 3.16)

# Android平台配置
set(ANDROID_PLATFORM android-31)  # Android 12
set(ANDROID_ABI arm64-v8a)        # 64位ARM
set(ANDROID_STL c++_static)       # 使用静态STL

# NDK工具链
set(CMAKE_TOOLCHAIN_FILE 
    "$ENV{ANDROID_NDK_HOME}/build/cmake/android.toolchain.cmake"
)
```

### 3.2 ABI配置选项

| ABI | 说明 | 推荐度 |
|-----|------|--------|
| `arm64-v8a` | 64位ARM设备 (推荐) | ⭐⭐⭐⭐⭐ |
| `armeabi-v7a` | 32位ARM设备 | ⭐⭐⭐⭐ |
| `x86_64` | 64位Intel模拟器 | ⭐⭐⭐ |
| `x86` | 32位Intel模拟器 | ⭐⭐ |

**多ABI构建**:
```cmake
# 支持多个ABI
set(ANDROID_ABI "arm64-v8a;armeabi-v7a")
```

### 3.3 构建类型配置

```cmake
# Debug配置
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")

# Release配置
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
```

---

## 四、Gradle配置

### 4.1 build.gradle配置

```groovy
android {
    namespace 'ogc.chart'
    compileSdkVersion 33
    
    defaultConfig {
        minSdkVersion 21        // Android 5.0
        targetSdkVersion 31     // Android 12
        
        // NDK配置
        ndk {
            abiFilters 'arm64-v8a', 'armeabi-v7a'
        }
        
        // CMake参数
        externalNativeBuild {
            cmake {
                arguments '-DANDROID_PLATFORM=android-31',
                          '-DANDROID_ABI=arm64-v8a',
                          '-DANDROID_STL=c++_static',
                          '-DCMAKE_BUILD_TYPE=Release'
                cFlags '-O3'
                cppFlags '-std=c++11', '-O3'
            }
        }
    }
    
    // CMake配置
    externalNativeBuild {
        cmake {
            path "src/main/cpp/CMakeLists.txt"
            version "3.16.0"
        }
    }
    
    // 构建类型
    buildTypes {
        debug {
            debuggable true
            jniDebuggable true
            minifyEnabled false
        }
        release {
            debuggable false
            jniDebuggable false
            minifyEnabled true
            proguardFiles getDefaultProguardFile('proguard-android-optimize.txt'), 
                          'proguard-rules.pro'
        }
    }
}
```

### 4.2 gradle.properties配置

```properties
# Android构建配置
android.useAndroidX=true
android.enableJetifier=true

# NDK配置
android.ndkVersion=25.2.9519653

# CMake配置
android.enableCMakeBuildCache=true

# 并行构建
org.gradle.parallel=true
org.gradle.caching=true
org.gradle.daemon=true
```

### 4.3 settings.gradle配置

```groovy
pluginManagement {
    repositories {
        google()
        mavenCentral()
        gradlePluginPortal()
    }
}

dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
    }
}

rootProject.name = "OGC-Chart-Android"
include ':app'
```

---

## 五、依赖库配置

### 5.1 原生依赖库

| 依赖库 | 版本 | 用途 | 状态 |
|--------|------|------|------|
| GDAL | 3.9.3 | 地理数据解析 | ⬜ 需编译Android版本 |
| libxml2 | 2.9+ | XML解析 | ⬜ 需编译Android版本 |
| SQLite | 3.35+ | 数据库 | ✅ Android内置 |
| SpatiaLite | 5.0+ | 空间数据库 | ⬜ 需编译Android版本 |
| PROJ | 9.0+ | 坐标转换 | ⬜ 需编译Android版本 |
| GoogleTest | 1.12+ | 单元测试 | ⬜ 需编译Android版本 |

### 5.2 Java依赖库

```groovy
dependencies {
    // AndroidX核心库
    implementation 'androidx.appcompat:appcompat:1.6.1'
    implementation 'androidx.core:core:1.10.0'
    implementation 'androidx.lifecycle:lifecycle-runtime:2.6.1'
    
    // 测试库
    testImplementation 'junit:junit:4.13.2'
    androidTestImplementation 'androidx.test.ext:junit:1.1.5'
    androidTestImplementation 'androidx.test.espresso:espresso-core:3.5.1'
}
```

### 5.3 依赖库编译脚本

**GDAL Android编译**:
```bash
# GDAL Android编译配置
./configure \
    --host=aarch64-linux-android \
    --with-sqlite3=yes \
    --with-spatialite=yes \
    --without-geos \
    --without-curl \
    CC=aarch64-linux-android21-clang \
    CXX=aarch64-linux-android21-clang++
```

---

## 六、签名配置

### 6.1 调试签名

```groovy
android {
    signingConfigs {
        debug {
            storeFile file('debug.keystore')
            storePassword 'android'
            keyAlias 'androiddebugkey'
            keyPassword 'android'
        }
    }
}
```

### 6.2 发布签名

```groovy
android {
    signingConfigs {
        release {
            storeFile file('release.keystore')
            storePassword System.getenv("KEYSTORE_PASSWORD")
            keyAlias System.getenv("KEY_ALIAS")
            keyPassword System.getenv("KEY_PASSWORD")
        }
    }
    
    buildTypes {
        release {
            signingConfig signingConfigs.release
        }
    }
}
```

**创建签名密钥**:
```powershell
# 创建调试密钥
keytool -genkey -v -keystore debug.keystore `
    -alias androiddebugkey `
    -storepass android `
    -keypass android `
    -keyalg RSA `
    -keysize 2048 `
    -validity 10000 `
    -dname "CN=Android Debug,O=Android,C=US"

# 创建发布密钥
keytool -genkey -v -keystore release.keystore `
    -alias ogc_chart `
    -keyalg RSA `
    -keysize 2048 `
    -validity 10000
```

---

## 七、ProGuard配置

### 7.1 proguard-rules.pro

```proguard
# OGC Chart ProGuard配置

# 保留JNI方法
-keepclasseswithmembernames class * {
    native <methods>;
}

# 保留所有native方法
-keep class ogc.chart.** {
    native <methods>;
}

# 保留Java API类
-keep public class ogc.chart.** {
    public protected *;
}

# 保留序列化类
-keepclassmembers class * implements java.io.Serializable {
    static final long serialVersionUID;
    private static final java.io.ObjectStreamField[] serialPersistentFields;
    private void writeObject(java.io.ObjectOutputStream);
    private void readObject(java.io.ObjectInputStream);
    java.lang.Object writeReplace();
    java.lang.Object readResolve();
}

# 移除日志 (Release构建)
-assumenosideeffects class android.util.Log {
    public static int v(...);
    public static int d(...);
    public static int i(...);
}
```

---

## 八、AndroidManifest配置

### 8.1 AndroidManifest.xml

```xml
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="ogc.chart">

    <!-- 权限声明 -->
    <uses-permission android:name="android.permission.INTERNET" />
    <uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" />
    <uses-permission android:name="android.permission.ACCESS_COARSE_LOCATION" />
    <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE" />
    <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" />

    <!-- OpenGL ES要求 -->
    <uses-feature android:glEsVersion="0x00020000" android:required="true" />

    <application
        android:allowBackup="true"
        android:icon="@mipmap/ic_launcher"
        android:label="@string/app_name"
        android:roundIcon="@mipmap/ic_launcher_round"
        android:supportsRtl="true"
        android:theme="@style/Theme.OGCChart">

        <!-- 主Activity -->
        <activity
            android:name=".ChartActivity"
            android:exported="true"
            android:configChanges="orientation|screenSize|keyboardHidden">
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>

    </application>

</manifest>
```

---

## 九、构建脚本

### 9.1 PowerShell构建脚本 (build_android.ps1)

```powershell
# Android构建脚本
param(
    [string]$BuildType = "Release",
    [string]$ABI = "arm64-v8a",
    [string]$NDKPath = "D:\program\android\ndk-r25c",
    [string]$SDKPath = "",
    [string]$OutputDir = ".\build_android"
)

# 设置环境变量
$env:ANDROID_NDK_HOME = $NDKPath
if ($SDKPath) {
    $env:ANDROID_SDK_ROOT = $SDKPath
}

# 创建构建目录
if (-not (Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir | Out-Null
}

# CMake配置
$CMakeArgs = @(
    "-G", "Ninja",
    "-DCMAKE_TOOLCHAIN_FILE=$NDKPath\build\cmake\android.toolchain.cmake",
    "-DANDROID_ABI=$ABI",
    "-DANDROID_PLATFORM=android-31",
    "-DANDROID_STL=c++_static",
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DCMAKE_INSTALL_PREFIX=$OutputDir\install",
    "-B", $OutputDir
)

Write-Host "配置CMake..." -ForegroundColor Green
& cmake $CMakeArgs

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake配置失败" -ForegroundColor Red
    exit 1
}

# 构建
Write-Host "构建项目..." -ForegroundColor Green
& cmake --build $OutputDir --config $BuildType --parallel

if ($LASTEXITCODE -ne 0) {
    Write-Host "构建失败" -ForegroundColor Red
    exit 1
}

Write-Host "构建成功! 输出目录: $OutputDir" -ForegroundColor Green
```

### 9.2 批处理构建脚本 (build_android.bat)

```batch
@echo off
setlocal

REM 设置NDK路径
set ANDROID_NDK_HOME=D:\program\android\ndk-r25c

REM 设置构建参数
set BUILD_TYPE=Release
set ABI=arm64-v8a
set OUTPUT_DIR=.\build_android

REM 创建构建目录
if not exist %OUTPUT_DIR% mkdir %OUTPUT_DIR%

REM CMake配置
cmake -G "Ninja" ^
    -DCMAKE_TOOLCHAIN_FILE=%ANDROID_NDK_HOME%\build\cmake\android.toolchain.cmake ^
    -DANDROID_ABI=%ABI% ^
    -DANDROID_PLATFORM=android-31 ^
    -DANDROID_STL=c++_static ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -B %OUTPUT_DIR%

if %ERRORLEVEL% neq 0 (
    echo CMake配置失败
    exit /b 1
)

REM 构建
cmake --build %OUTPUT_DIR% --config %BUILD_TYPE% --parallel

if %ERRORLEVEL% neq 0 (
    echo 构建失败
    exit /b 1
)

echo 构建成功! 输出目录: %OUTPUT_DIR%
endlocal
```

---

## 十、测试配置

### 10.1 单元测试配置

```groovy
android {
    defaultConfig {
        testInstrumentationRunner "androidx.test.runner.AndroidJUnitRunner"
    }
    
    testOptions {
        unitTests {
            includeAndroidResources = true
            returnDefaultValues = true
        }
    }
}
```

### 10.2 测试运行脚本

```powershell
# 运行单元测试
.\gradlew test

# 运行Android测试
.\gradlew connectedAndroidTest

# 生成测试报告
.\gradlew testDebugUnitTest --info
```

---

## 十一、常见问题解决

### 11.1 NDK版本不匹配

**问题**: NDK版本与项目要求不一致

**解决方案**:
```powershell
# 安装指定版本NDK
sdkmanager "ndk;25.2.9519653"

# 或在gradle.properties中指定
android.ndkVersion=25.2.9519653
```

### 11.2 CMake找不到

**问题**: CMake未安装或版本过低

**解决方案**:
```powershell
# 安装CMake
sdkmanager "cmake;3.22.1"

# 或手动安装
choco install cmake -y
```

### 11.3 ABI不兼容

**问题**: 模拟器无法运行原生库

**解决方案**:
```groovy
// 在build.gradle中添加x86_64支持
ndk {
    abiFilters 'arm64-v8a', 'x86_64'
}
```

### 11.4 依赖库缺失

**问题**: 找不到原生依赖库

**解决方案**:
1. 编译Android版本的依赖库
2. 将.so文件放入 `src/main/jniLibs/{abi}/` 目录
3. 在CMakeLists.txt中添加库路径

---

## 十二、环境变量配置

### 12.1 系统环境变量

```
ANDROID_SDK_ROOT=D:\program\android\sdk
ANDROID_NDK_HOME=D:\program\android\ndk-r25c
ANDROID_HOME=D:\program\android\sdk
PATH=%PATH%;%ANDROID_SDK_ROOT%\platform-tools
PATH=%PATH%;%ANDROID_SDK_ROOT%\tools
PATH=%PATH%;%ANDROID_NDK_HOME%
```

### 12.2 PowerShell配置文件

```powershell
# 添加到 $PROFILE
$env:ANDROID_SDK_ROOT = "D:\program\android\sdk"
$env:ANDROID_NDK_HOME = "D:\program\android\ndk-r25c"
$env:PATH += ";$env:ANDROID_SDK_ROOT\platform-tools"
$env:PATH += ";$env:ANDROID_NDK_HOME"
```

---

## 十三、检查清单

### 13.1 编译前检查

- [ ] NDK路径已配置 (`D:\program\android\ndk-r25c`)
- [ ] SDK路径已配置
- [ ] CMake已安装 (≥3.16)
- [ ] Ninja已安装
- [ ] JDK已安装 (11或17)
- [ ] Gradle已安装 (≥7.5)
- [ ] 依赖库已编译或准备就绪

### 13.2 编译时检查

- [ ] ABI配置正确 (`arm64-v8a`)
- [ ] 目标平台正确 (`android-31`)
- [ ] STL配置正确 (`c++_static`)
- [ ] 构建类型正确 (`Release`/`Debug`)
- [ ] 签名配置正确

### 13.3 编译后检查

- [ ] .so文件生成成功
- [ ] .aar文件生成成功
- [ ] 测试通过
- [ ] 签名验证通过

---

## 十四、版本信息

| 组件 | 当前版本 | 目标版本 | 状态 |
|------|----------|----------|------|
| Android NDK | r25c | r25c | ✅ |
| Android SDK | - | API 31+ | ⬜ |
| CMake | - | 3.16+ | ⬜ |
| Gradle | - | 7.5+ | ⬜ |
| JDK | - | 11/17 | ⬜ |
| Build Tools | - | 33.0.0+ | ⬜ |

---

## 十五、参考资源

- [Android NDK官方文档](https://developer.android.com/ndk/guides)
- [CMake Android交叉编译](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html#cross-compiling-for-android)
- [Gradle构建配置](https://developer.android.com/build)
- [GDAL Android编译](https://trac.osgeo.org/gdal/wiki/AndroidBuildSystem)

---

**文档版本**: v1.0  
**最后更新**: 2026-04-03
