# Android编译快速参考

> **详细配置**: 参见 [android_build_config.md](./android_build_config.md)

---

## 一、快速开始

### 1.1 环境检查

```powershell
# 检查NDK
Test-Path "D:\program\android\ndk-r25c"

# 检查CMake
cmake --version

# 检查Ninja (可选)
ninja --version
```

### 1.2 一键构建

**PowerShell**:
```powershell
.\build_android.ps1
```

**批处理**:
```batch
build_android.bat
```

---

## 二、常用命令

### 2.1 Debug构建

```powershell
.\build_android.ps1 -BuildType Debug
```

```batch
build_android.bat -t Debug
```

### 2.2 指定ABI

```powershell
# 64位ARM (推荐)
.\build_android.ps1 -ABI arm64-v8a

# 32位ARM
.\build_android.ps1 -ABI armeabi-v7a

# x86模拟器
.\build_android.ps1 -ABI x86_64
```

### 2.3 清理构建

```powershell
.\build_android.ps1 -Clean
```

```batch
build_android.bat -c
```

### 2.4 详细输出

```powershell
.\build_android.ps1 -Verbose
```

```batch
build_android.bat -v
```

### 2.5 完整示例

```powershell
.\build_android.ps1 `
    -BuildType Release `
    -ABI arm64-v8a `
    -NDKPath "D:\program\android\ndk-r25c" `
    -OutputDir ".\build_android_release" `
    -Clean `
    -Verbose
```

---

## 三、参数说明

### 3.1 PowerShell脚本参数

| 参数 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `-BuildType` | String | Release | 构建类型 (Debug/Release) |
| `-ABI` | String | arm64-v8a | 目标ABI |
| `-NDKPath` | String | D:\program\android\ndk-r25c | NDK路径 |
| `-SDKPath` | String | - | SDK路径 |
| `-OutputDir` | String | .\build_android | 输出目录 |
| `-Clean` | Switch | $false | 清理构建目录 |
| `-Verbose` | Switch | $false | 详细输出 |

### 3.2 批处理脚本参数

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `-t, --type` | Release | 构建类型 |
| `-a, --abi` | arm64-v8a | 目标ABI |
| `-n, --ndk` | D:\program\android\ndk-r25c | NDK路径 |
| `-s, --sdk` | - | SDK路径 |
| `-o, --output` | .\build_android | 输出目录 |
| `-c, --clean` | - | 清理构建目录 |
| `-v, --verbose` | - | 详细输出 |

---

## 四、输出文件

### 4.1 构建产物

| 文件类型 | 位置 | 说明 |
|----------|------|------|
| `.so` | `build_android\*.so` | 动态库 |
| `.a` | `build_android\*.a` | 静态库 |
| `.aar` | `build_android\*.aar` | Android库 |

### 4.2 安装目录

```
build_android/
├── install/
│   ├── lib/
│   │   └── libogc_android_adapter.so
│   └── include/
│       └── android_adapter/
└── CMakeCache.txt
```

---

## 五、常见问题

### 5.1 NDK路径错误

**错误**: `NDK路径不存在`

**解决**:
```powershell
# 检查NDK路径
Test-Path "D:\program\android\ndk-r25c"

# 或指定正确路径
.\build_android.ps1 -NDKPath "C:\正确的\ndk\路径"
```

### 5.2 CMake未找到

**错误**: `CMake未安装或不在PATH中`

**解决**:
```powershell
# 安装CMake
choco install cmake -y

# 或下载安装
# https://cmake.org/download/
```

### 5.3 Ninja未找到

**警告**: `Ninja未安装或不在PATH中`

**解决**:
```powershell
# 安装Ninja
choco install ninja -y

# 或使用默认生成器 (Unix Makefiles)
# 脚本会自动降级
```

### 5.4 ABI不兼容

**问题**: 模拟器无法运行

**解决**:
```powershell
# 使用x86_64 ABI
.\build_android.ps1 -ABI x86_64
```

---

## 六、环境变量

### 6.1 必需环境变量

```powershell
# NDK路径
$env:ANDROID_NDK_HOME = "D:\program\android\ndk-r25c"

# SDK路径 (可选)
$env:ANDROID_SDK_ROOT = "D:\program\android\sdk"
```

### 6.2 添加到系统环境变量

```
ANDROID_NDK_HOME=D:\program\android\ndk-r25c
ANDROID_SDK_ROOT=D:\program\android\sdk
PATH=%PATH%;%ANDROID_NDK_HOME%
```

---

## 七、下一步

1. **配置依赖库**: 编译GDAL、libxml2等依赖库的Android版本
2. **集成到项目**: 将生成的.so文件集成到Android项目
3. **运行测试**: 在Android设备或模拟器上运行测试
4. **性能优化**: 根据实际设备调整编译选项

---

## 八、参考链接

- [详细配置文档](./android_build_config.md)
- [Android NDK官方文档](https://developer.android.com/ndk/guides)
- [CMake Android工具链](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html#cross-compiling-for-android)

---

**文档版本**: v1.0  
**最后更新**: 2026-04-03
