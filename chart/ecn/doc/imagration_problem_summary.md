# Java 11 迁移问题汇总

**创建日期**: 2026-04-25  
**最后更新**: 2026-04-25 20:30  
**状态**: ✅ 全部完成

---

## 一、已解决问题

### 1.1 javawrapper 模块架构问题

| 问题ID | 问题描述 | 发现时间 | 解决方案 | 状态 |
|--------|----------|----------|----------|------|
| P001 | javawrapper 包含 JavaFX 依赖，与文档描述不符 | 2026-04-25 16:20 | 将 JavaFX 相关类移到独立模块 javawrapper-adapter | ✅ 已解决 |

**详细说明**:
- **发现过程**: 编译 javawrapper 时发现 45 个编译错误，提示 `javafx.scene.input` 包不存在
- **影响文件**: `CanvasAdapter.java`, `ChartMouseEvent.java`, `ChartScrollEvent.java`, `ChartKeyEvent.java`, `ChartEventHandler.java`, `PanHandler.java`, `ZoomHandler.java`
- **解决方案**: 
  1. 创建新模块 `ecn/javawrapper-adapter`
  2. 将 JavaFX 相关类移动到新模块
  3. `ImageDevice.java` 保留在 javawrapper（不依赖 JavaFX）

---

### 1.2 Gradle 版本兼容性问题

| 问题ID | 问题描述 | 发现时间 | 解决方案 | 状态 |
|--------|----------|----------|----------|------|
| P002 | Gradle 4.5.1 不支持 Java 11 | 2026-04-25 16:15 | 升级 Gradle 到 7.6.6 | ✅ 已解决 |

**详细说明**:
- **错误信息**: `Could not determine java version from '11.0.30'`
- **解决方案**: 更新 `gradle-wrapper.properties` 使用 `E:/java/gradle/gradle-7.6.6-all.zip`

---

### 1.3 Gradle 插件兼容性问题

| 问题ID | 问题描述 | 发现时间 | 解决方案 | 状态 |
|--------|----------|----------|----------|------|
| P003 | Gradle 7.6.6 移除了 `maven` 插件 | 2026-04-25 16:30 | 使用 `maven-publish` 插件替代 | ✅ 已解决 |

**详细说明**:
- **错误信息**: `Plugin with id 'maven' not found`
- **解决方案**: 将 `apply plugin: 'maven'` 改为 `apply plugin: 'maven-publish'`

---

### 1.4 Gradle 配置语法变更

| 问题ID | 问题描述 | 发现时间 | 解决方案 | 状态 |
|--------|----------|----------|----------|------|
| P004 | `compile` 配置已废弃 | 2026-04-25 17:00 | 使用 `implementation` 替代 | ✅ 已解决 |
| P005 | `baseName` 属性已废弃 | 2026-04-25 17:00 | 使用 `archiveBaseName.set()` 替代 | ✅ 已解决 |
| P006 | `classifier` 属性已废弃 | 2026-04-25 17:00 | 使用 `archiveClassifier.set()` 替代 | ✅ 已解决 |

---

### 1.5 JavaFX 模块依赖问题

| 问题ID | 问题描述 | 发现时间 | 解决方案 | 状态 |
|--------|----------|----------|----------|------|
| P007 | echart-theme 缺少 JavaFX 依赖 | 2026-04-25 17:30 | 添加 JavaFX 17 依赖 | ✅ 已解决 |
| P008 | echart-ui-render 缺少 JavaFX 依赖 | 2026-04-25 17:45 | 添加 JavaFX 17 依赖 | ✅ 已解决 |
| P009 | echart-ui 缺少 JavaFX FXML 依赖 | 2026-04-25 18:00 | 添加 javafx-fxml 依赖 | ✅ 已解决 |
| P010 | echart-ui 缺少 JavaFX Swing 依赖 | 2026-04-25 18:05 | 添加 javafx-swing 依赖 | ✅ 已解决 |
| P011 | echart-ui 缺少 JavaFX Media 依赖 | 2026-04-25 18:10 | 添加 javafx-media 依赖 | ✅ 已解决 |
| P012 | echart-app 缺少 JavaFX 依赖 | 2026-04-25 18:15 | 添加完整 JavaFX 17 依赖 | ✅ 已解决 |

---

### 1.6 业务模块依赖问题

| 问题ID | 问题描述 | 发现时间 | 解决方案 | 状态 |
|--------|----------|----------|----------|------|
| P013 | echart-ui 缺少业务模块依赖 | 2026-04-25 18:00 | 添加 echart-data, echart-route, echart-alarm, echart-workspace 依赖 | ✅ 已解决 |

**详细说明**:
- **发现过程**: echart-ui 编译失败，找不到 ChartFile, Route, Waypoint, Workspace 等类
- **解决方案**: 在 echart-ui 的 build.gradle 中添加对业务模块 JAR 的直接依赖

---

### 1.7 javawrapper 依赖问题

| 问题ID | 问题描述 | 发现时间 | 解决方案 | 状态 |
|--------|----------|----------|----------|------|
| P014 | echart-alarm 缺少 javawrapper 依赖 | 2026-04-25 19:00 | 添加 javawrapper-2.1.0.jar 依赖 | ✅ 已解决 |
| P015 | echart-ais 缺少 javawrapper 和 echart-alarm 依赖 | 2026-04-25 19:00 | 添加 javawrapper 和 echart-alarm 依赖 | ✅ 已解决 |
| P016 | echart-facade 缺少 echart-workspace 和 javawrapper 依赖 | 2026-04-25 19:10 | 添加 echart-workspace 和 javawrapper 依赖 | ✅ 已解决 |

**详细说明**:
- **发现过程**: 编译 echart-alarm 时发现找不到 Alert、Type、Severity 类，这些类在 javawrapper 模块的 cn.cycle.chart.api.alert 包中
- **影响模块**: echart-alarm, echart-ais, echart-facade
- **解决方案**: 在各模块的 build.gradle 中添加对 javawrapper-2.1.0.jar 的依赖

---

### 1.8 JavaFX 源码编译问题（新增）

| 问题ID | 问题描述 | 发现时间 | 解决方案 | 状态 |
|--------|----------|----------|----------|------|
| P017 | WINSDK_DIR 环境变量未定义 | 2026-04-25 19:45 | 在 gradle.properties 中设置 WINDOWS_SDK_DIR | ✅ 已解决 |
| P018 | DXSDK_DIR 环境变量未定义 | 2026-04-25 19:50 | 设置 WINDOWS_DXSDK_DIR 指向 Windows Kits 8.1 | ✅ 已解决 |
| P019 | VS2015 兼容性问题 | 2026-04-25 19:40 | 设置 WINDOWS_VS_VER=140 | ⚠️ 部分支持 |

**详细说明**:
- **背景**: 尝试使用 JDK 11 编译 JavaFX 17 源码（E:\java\jfx-17-18）
- **错误信息**: `FAIL: WINSDK_DIR not defined` 和 `FAIL: DXSDK_DIR not defined`
- **解决方案**:
  1. 在 `gradle.properties` 中添加 `WINDOWS_SDK_DIR=C:/Program Files (x86)/Windows Kits/10`
  2. 添加 `WINDOWS_DXSDK_DIR=C:/Program Files (x86)/Windows Kits/8.1`（包含 fxc.exe）
  3. 设置 `WINDOWS_VS_VER=140` 对应 VS2015
- **注意事项**:
  - JavaFX 17 官方推荐 VS2019 或 VS2022
  - VS2015 可能存在兼容性问题
  - Windows SDK 10 不包含 fxc.exe，需要 Windows SDK 8.1 或 DirectX SDK

**关键配置**:
```properties
# gradle.properties
WINDOWS_SDK_DIR=C:/Program Files (x86)/Windows Kits/10
WINDOWS_DXSDK_DIR=C:/Program Files (x86)/Windows Kits/8.1
WINDOWS_VS_VER=140
```

---

### 1.9 应用运行时问题（新增）

| 问题ID | 问题描述 | 发现时间 | 解决方案 | 状态 |
|--------|----------|----------|----------|------|
| P020 | 缺少 JavaFX 运行时组件 | 2026-04-25 20:00 | 使用 --module-path 和 --add-modules 参数 | ✅ 已解决 |
| P021 | 缺少 controlsfx 依赖 | 2026-04-25 20:05 | 编译并添加 controlsfx-11.2.3.jar | ✅ 已解决 |
| P022 | 模块冲突：javafx.controls 重复 | 2026-04-25 20:10 | 明确指定模块路径，排除冲突 JAR | ✅ 已解决 |

**详细说明**:

#### P020: 缺少 JavaFX 运行时组件
- **错误信息**: `错误: 缺少 JavaFX 运行时组件, 需要使用该组件来运行此应用程序`
- **原因**: JDK 11 不包含 JavaFX，需要单独配置
- **解决方案**:
  ```batch
  java --module-path "%JAVAFX_HOME%\lib" --add-modules javafx.controls,javafx.fxml,javafx.swing,javafx.media -cp "%CLASSPATH%" cn.cycle.echart.app.EChartApp
  ```

#### P021: 缺少 controlsfx 依赖
- **错误信息**: `Exception in Application start method`
- **原因**: echart-ui 依赖 ControlsFX，但 install/bin 目录缺少该 JAR
- **解决方案**: 编译 ControlsFX 11.2.3 并复制到 install/bin

#### P022: 模块冲突
- **错误信息**: `Two versions of module javafx.controls found in F:\cycle\trae\chart\install\bin (javafx.controls.jar and javafx-app-2.1.0.jar)`
- **原因**: `--module-path` 指向整个目录时，扫描到多个包含相同模块的 JAR
- **解决方案**: 明确指定模块路径，只包含需要的 JavaFX JAR：
  ```batch
  set MODULE_PATH=%SCRIPT_DIR%\javafx.base.jar
  set MODULE_PATH=%MODULE_PATH%;%SCRIPT_DIR%\javafx.controls.jar
  set MODULE_PATH=%MODULE_PATH%;%SCRIPT_DIR%\javafx.graphics.jar
  set MODULE_PATH=%MODULE_PATH%;%SCRIPT_DIR%\javafx.fxml.jar
  set MODULE_PATH=%MODULE_PATH%;%SCRIPT_DIR%\javafx.swing.jar
  set MODULE_PATH=%MODULE_PATH%;%SCRIPT_DIR%\javafx.media.jar
  
  java --module-path "%MODULE_PATH%" --add-modules javafx.controls,javafx.fxml,javafx.swing,javafx.media ...
  ```

---

### 1.10 ControlsFX 编译问题（新增）

| 问题ID | 问题描述 | 发现时间 | 解决方案 | 状态 |
|--------|----------|----------|----------|------|
| P023 | gradle.properties 路径转义问题 | 2026-04-25 20:15 | 使用正斜杠 `/` 替代反斜杠 `\` | ✅ 已解决 |

**详细说明**:
- **错误信息**: `Value 'E:javajdk-11.0.30.7' given for org.gradle.java.home Gradle property is invalid`
- **原因**: Windows 批处理中使用 `echo` 写入文件时，反斜杠被转义
- **错误写法**:
  ```batch
  echo org.gradle.java.home=%JAVA_HOME%>> gradle.properties
  ```
- **正确写法**:
  ```batch
  echo org.gradle.java.home=E:/java/jdk-11.0.30.7>> gradle.properties
  ```
- **经验**: Gradle/Java 都支持正斜杠路径，建议在配置文件中统一使用正斜杠

---

## 二、进行中问题

| 问题ID | 问题描述 | 影响模块 | 当前状态 |
|--------|----------|----------|----------|
| - | 无 | - | - |

---

## 三、待解决问题

| 问题ID | 问题描述 | 影响模块 | 优先级 |
|--------|----------|----------|--------|
| - | 无 | - | - |

---

## 四、迁移进度

### 4.1 已完成模块

| 模块 | 原Java版本 | 目标Java版本 | 编译状态 | 产物位置 |
|------|-----------|-------------|----------|----------|
| javawrapper | 1.8 | 11 | ✅ 成功 | install/bin/javawrapper-2.1.0.jar |
| javawrapper-adapter | - | 11 | ✅ 成功 | install/bin/javawrapper-adapter-1.2.1-alpha.jar |
| echart-core | 1.8 | 11 | ✅ 成功 | install/bin/echart-core-1.2.1-alpha.jar |
| echart-i18n | 1.8 | 11 | ✅ 成功 | install/bin/echart-i18n-1.2.1-alpha.jar |
| echart-render | 1.8 | 11 | ✅ 成功 | install/bin/echart-render-1.2.1-alpha.jar |
| echart-data | 1.8 | 11 | ✅ 成功 | install/bin/echart-data-1.2.1-alpha.jar |
| echart-alarm | 1.8 | 11 | ✅ 成功 | install/bin/echart-alarm-1.2.1-alpha.jar |
| echart-ais | 1.8 | 11 | ✅ 成功 | install/bin/echart-ais-1.2.1-alpha.jar |
| echart-route | 1.8 | 11 | ✅ 成功 | install/bin/echart-route-1.2.1-alpha.jar |
| echart-workspace | 1.8 | 11 | ✅ 成功 | install/bin/echart-workspace-1.2.1-alpha.jar |
| echart-theme | 1.8 | 11 | ✅ 成功 | install/bin/echart-theme-1.2.1-alpha.jar |
| echart-plugin | 1.8 | 11 | ✅ 成功 | install/bin/echart-plugin-1.2.1-alpha.jar |
| echart-facade | 1.8 | 11 | ✅ 成功 | install/bin/echart-facade-1.2.1-alpha.jar |
| fxribbon | 1.8 | 11 | ✅ 成功 | install/bin/fxribbon-2.1.0.jar |
| echart-ui | 1.8 | 11 | ✅ 成功 | install/bin/echart-ui-1.2.1-alpha.jar |
| echart-ui-render | 1.8 | 11 | ✅ 成功 | install/bin/echart-ui-render-1.2.1-alpha.jar |
| echart-app | 1.8 | 11 | ✅ 成功 | install/bin/echart-app-1.2.1-alpha.jar |
| controlsfx | - | 11 | ✅ 成功 | install/bin/controlsfx-11.2.3.jar |

### 4.2 待迁移模块

| 模块 | 原Java版本 | 目标Java版本 | 优先级 | 备注 |
|------|-----------|-------------|--------|------|
| - | - | - | - | 全部完成 |

---

## 五、配置变更记录

### 5.1 全局配置

| 文件 | 变更内容 | 变更时间 |
|------|----------|----------|
| ecn/config/common.gradle | 创建通用配置文件，提取公共配置和函数 | 2026-04-25 19:30 |
| cycle/gradle/wrapper/gradle-wrapper.properties | `gradle-4.5.1-all.zip` → `gradle-7.6.6-all.zip` | 2026-04-25 |

**common.gradle 提供的通用配置函数**:
- `configureJavaModule(project, moduleName)`: 配置 Java 模块（插件、版本、编码、构建目录、任务）
- `createSourcesJar(project)`: 创建源码 JAR 任务
- `createInstallJar(project)`: 创建安装 JAR 任务
- `configureTest(project)`: 配置测试任务
- `configurePublishing(project)`: 配置发布任务
- `addJavaFXDependencies(project, modules)`: 添加 JavaFX 依赖

### 5.2 运行脚本

| 文件 | 变更内容 | 变更时间 |
|------|----------|----------|
| install/bin/app_echart.bat | 添加 JavaFX 模块路径配置 | 2026-04-25 20:10 |
| install/bin/build_controlsfx.bat | 创建 ControlsFX 编译脚本 | 2026-04-25 20:15 |
| code/build_javafx.bat | 创建 JavaFX 源码编译脚本 | 2026-04-25 19:45 |

---

## 六、经验总结

### 6.1 架构问题

1. **模块职责边界不清**: javawrapper 作为"公共内核库"不应包含平台特定代码（JavaFX）
2. **文档与实际不符**: 迁移前需验证模块实际依赖
3. **隐式依赖**: 某些模块虽然声明不依赖 JavaFX，但实际代码中使用了 JavaFX 类

### 6.2 构建问题

1. **Gradle 版本必须升级**: Gradle 4.x 不支持 Java 11
2. **插件兼容性**: Gradle 7.x 移除了 `maven` 插件，需使用 `maven-publish`
3. **依赖声明语法**: Gradle 7.x 推荐使用 `implementation` 替代 `compile`
4. **属性访问方式**: `baseName` → `archiveBaseName.set()`，`classifier` → `archiveClassifier.set()`

### 6.3 JavaFX 依赖问题

1. **JavaFX 模块化**: Java 11 后 JavaFX 需要单独引入
2. **平台特定依赖**: JavaFX 依赖需要指定平台（win/linux/mac）
3. **子模块依赖**: JavaFX 有多个子模块（base, graphics, controls, fxml, swing, media），需按需引入
4. **ControlsFX 版本**: ControlsFX 8.x 不兼容 JavaFX 17，需升级到 11.x

### 6.4 JavaFX 运行时配置（新增）

1. **模块路径配置**: JDK 11 运行 JavaFX 应用需要 `--module-path` 和 `--add-modules` 参数
2. **模块冲突处理**: 当目录中存在多个包含相同模块的 JAR 时，需明确指定模块路径
3. **DLL 文件**: JavaFX 运行需要对应的原生库（.dll 文件），需放在 `java.library.path` 指向的目录
4. **推荐方案**: 
   - 方案A: 使用外部 JavaFX SDK（`E:\java\javafx-sdk-17.0.19`）
   - 方案B: 将 JavaFX JAR 和 DLL 复制到应用目录

### 6.5 JavaFX 源码编译（新增）

1. **编译环境要求**:
   - JDK 11+
   - Gradle 6.3+
   - Visual Studio 2019/2022（推荐）或 VS2015（有限支持）
   - Windows SDK 10
   - Windows SDK 8.1 或 DirectX SDK（用于 fxc.exe）

2. **环境变量配置**:
   - `WINDOWS_SDK_DIR`: Windows SDK 10 路径
   - `WINDOWS_DXSDK_DIR`: Windows SDK 8.1 路径（包含 fxc.exe）
   - `WINDOWS_VS_VER`: VS 版本号（140=VS2015, 150=VS2017, 160=VS2019）

3. **替代方案**: 如果编译困难，建议直接下载预编译的 JavaFX SDK

### 6.6 Windows 批处理路径问题（新增）

1. **反斜杠转义**: 批处理中使用 `echo` 写入文件时，反斜杠可能被转义
2. **解决方案**: 在配置文件中使用正斜杠 `/`，Gradle 和 Java 都支持
3. **示例**:
   ```batch
   REM 错误写法（反斜杠被转义）
   echo org.gradle.java.home=%JAVA_HOME%>> gradle.properties
   
   REM 正确写法（使用正斜杠）
   echo org.gradle.java.home=E:/java/jdk-11.0.30.7>> gradle.properties
   ```

### 6.7 迁移最佳实践

1. **按依赖层级迁移**: 从底层模块开始，逐步向上迁移
2. **验证依赖完整性**: 迁移前检查模块是否依赖未声明的库
3. **保持产物一致性**: 所有模块输出到统一目录（install/bin/）
4. **增量验证**: 每迁移一个模块立即编译验证

### 6.8 构建配置优化

1. **提取通用配置**: 将公共字段、路径、设置提取到 common.gradle
2. **使用配置函数**: 封装常用配置逻辑为可复用函数
3. **统一版本管理**: 在 common.gradle 中集中管理版本号
4. **简化模块配置**: 各模块 build.gradle 只需调用通用函数和声明特定依赖

---

## 七、相关脚本清单

| 脚本 | 路径 | 用途 |
|------|------|------|
| app_echart.bat | install/bin/ | 启动 E-Chart 应用 |
| build_controlsfx.bat | install/bin/ | 编译 ControlsFX |
| build_javafx.bat | code/ | 编译 JavaFX 源码 |
| compile_echart_jars.bat | ecn/doc/ | 编译所有 E-Chart 模块 |
| compile_cpp_modules.bat | code/ | 编译 C++ 模块 |

---

**维护者**: AI Migration Team  
**参考文档**: [java11_upgrade.md](./java11_upgrade.md)
