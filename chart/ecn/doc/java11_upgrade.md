# JDK 11 / JavaFX 11/17 / Gradle 7.6+ 迁移计划

**版本**: v1.1  
**日期**: 2026-04-25  
**状态**: 规划中  
**目标**: 从JDK 1.8升级到JDK 11，从JavaFX 8升级到JavaFX 11/17，从Gradle 4.5.1升级到Gradle 7.6+

---

## 1. 概述

### 1.1 迁移背景

当前项目使用的技术栈版本较旧，存在以下问题：
- **JDK 1.8**: 已于2019年停止公共更新，缺乏新特性和安全补丁
- **JavaFX 8**: 内置于JDK 8，已不再维护
- **Gradle 4.5.1**: 发布于2017年，不支持新的Java特性和插件

### 1.2 迁移目标

| 组件 | 当前版本 | 目标版本 | 迁移优先级 |
|------|----------|----------|------------|
| JDK | 1.8 | 11 (LTS) | P0 |
| JavaFX | 8 | 17 (LTS) | P0 |
| Gradle | 4.5.1 | 7.6+ | P0 |
| JUnit | 4.12 | 5.9.3 | P1 |
| ControlsFX | 8.40.18 | 11.2.0 | P1 |

### 1.3 迁移范围

#### ECN模块 (15个)

| 序号 | 模块名 | Layer | 是否依赖JavaFX | 多端复用分类 | 迁移复杂度 |
|------|--------|-------|----------------|-------------|------------|
| 1 | fxribbon | Ribbon | ✅ 是 | JavaFX平台UI库 | 高 |
| 2 | echart-core | 0 | ❌ 否 | 公共内核库 | 低 |
| 3 | echart-i18n | 1 | ❌ 否 | 公共业务库 | 低 |
| 4 | echart-render | 2 | ❌ 否 | 公共业务库 | 低 |
| 5 | echart-data | 2 | ❌ 否 | 公共业务库 | 低 |
| 6 | echart-alarm | 3 | ❌ 否 | 公共业务库 | 低 |
| 7 | echart-ais | 3 | ❌ 否 | 公共业务库 | 低 |
| 8 | echart-route | 3 | ❌ 否 | 公共业务库 | 低 |
| 9 | echart-workspace | 3 | ❌ 否 | 公共业务库 | 低 |
| 10 | echart-ui | 4 | ✅ 是 | JavaFX平台UI库 | 高 |
| 11 | echart-ui-render | 5 | ✅ 是 | JavaFX平台UI库 | 高 |
| 12 | echart-theme | 5 | ❌ 否 | 公共业务库 | 低 |
| 13 | echart-plugin | 5 | ❌ 否 | 公共业务库 | 低 |
| 14 | echart-facade | 6 | ❌ 否 | 公共业务库 | 低 |
| 15 | echart-app | 7 | ✅ 是 | JavaFX平台UI库 | 高 |

> **多端复用分类说明**:
> - **公共内核库**: 无外部依赖的核心框架，所有平台共享
> - **公共业务库**: 不依赖JavaFX的业务逻辑模块，可在Android/Web等平台复用
> - **JavaFX平台UI库**: 依赖JavaFX的UI模块，需为不同平台单独适配

#### Cycle模块 (3个)

| 序号 | 模块名 | 是否依赖JavaFX | 迁移复杂度 |
|------|--------|----------------|------------|
| 1 | javawrapper | ❌ 否 | 中 |
| 2 | javafx-app | ✅ 是 | 高 |
| 3 | fxribbon | ✅ 是 | 高 |

---

## 2. 当前技术栈分析

### 2.1 JDK 1.8

**配置文件**: `ecn/config/common.gradle`

```gradle
ext {
    javaVersion = 1.8
    // ...
}
```

**构建文件示例**: `ecn/echart-core/build.gradle`

```gradle
sourceCompatibility = javaVersion
targetCompatibility = javaVersion
```

**依赖的JDK 8特性**:
- `javax.xml.bind` (JAXB) - JDK 9+移除
- `javax.annotation` - JDK 9+移除
- `sun.misc.Unsafe` - JDK 9+受限访问
- `java.util.logging` - 仍在使用

### 2.2 JavaFX 8

**当前状态**: JavaFX 8内置于JDK 8，无需额外依赖

**依赖JavaFX的模块**:
- `fxribbon`: JavaFX Ribbon控件库
- `echart-ui`: JavaFX UI控件
- `echart-ui-render`: JavaFX Canvas渲染
- `echart-app`: 应用入口

**使用的JavaFX 8特性**:
- `javafx.application.Application`
- `javafx.scene.control.*`
- `javafx.scene.layout.*`
- `javafx.concurrent.Task`
- `javafx.beans.property.*`
- `javafx.fxml.FXMLLoader`

### 2.3 Gradle 4.5.1

**配置文件**: `gradle/gradle-4.5.1/`

**当前Gradle配置**:
- 使用旧版插件DSL: `apply plugin: 'java'`
- 使用旧版依赖配置: `compile`, `testCompile`
- 使用旧版Maven插件: `apply plugin: 'maven'`

**示例**: `ecn/echart-ui/build.gradle`

```gradle
apply plugin: 'java'
apply plugin: 'maven'

dependencies {
    compile project(':echart-core')
    compile 'org.controlsfx:controlsfx:8.40.18'
    testCompile group: 'junit', name: 'junit', version: junitVersion
}
```

### 2.4 第三方依赖

| 依赖 | 当前版本 | 目标版本 | 说明 |
|------|----------|----------|------|
| ControlsFX | 8.40.18 | 11.1.2 | JavaFX控件库 |
| JUnit | 4.12 | 5.9.3 | 测试框架 |

---

## 3. 迁移影响分析

### 3.1 JDK 8 → 11 变更

#### 3.1.1 移除的模块

| 模块 | JDK 8 | JDK 11 | 替代方案 |
|------|-------|--------|----------|
| JavaFX | 内置 | 移除 | 独立JavaFX SDK |
| Java EE模块 | 内置 | 移除 | 添加Maven依赖 |
| JAXB | 内置 | 移除 | `jakarta.xml.bind:jakarta.xml.bind-api` |
| JAX-WS | 内置 | 移除 | `jakarta.xml.ws:jakarta.xml.ws-api` |
| Commons Annotations | 内置 | 移除 | `jakarta.annotation:jakarta.annotation-api` |
| CORBA | 内置 | 移除 | 无替代，需重构 |

#### 3.1.2 API变更

| API | JDK 8 | JDK 11 | 影响 |
|-----|-------|--------|------|
| `sun.misc.Unsafe` | 可用 | 受限 | 需使用替代API |
| `com.sun.*` | 可用 | 受限 | 需使用公共API |
| `java.lang.Module` | 无 | 新增 | 支持模块化 |
| `var` 关键字 | 不支持 | 支持 | 可选使用 |
| `HttpClient` | 无 | 新增 | 可选使用 |

#### 3.1.3 JVM参数变更

| 参数 | JDK 8 | JDK 11 | 说明 |
|------|-------|--------|------|
| `-XX:MaxPermSize` | 支持 | 移除 | 使用`-XX:MaxMetaspaceSize` |
| `-XX:PermSize` | 支持 | 移除 | 使用`-XX:MetaspaceSize` |

### 3.2 JavaFX 8 → 17 变更

#### 3.2.1 依赖变更

**JDK 8 (内置)**:
```gradle
// 无需额外依赖，JavaFX内置于JDK
```

**JDK 11+ (独立依赖)**:
```gradle
dependencies {
    implementation 'org.openjfx:javafx-controls:17:win'
    implementation 'org.openjfx:javafx-fxml:17:win'
    implementation 'org.openjfx:javafx-graphics:17:win'
    implementation 'org.openjfx:javafx-base:17:win'
}
```

#### 3.2.2 模块化要求

**JDK 11+ 需要模块化配置**:

**方式1: module-info.java**
```java
module cn.cycle.echart.ui {
    requires javafx.controls;
    requires javafx.fxml;
    requires javafx.graphics;
    
    exports cn.cycle.echart.ui;
    opens cn.cycle.echart.ui to javafx.fxml;
}
```

**方式2: 非模块化JAR (推荐初期使用)**
```gradle
// 不使用module-info.java，将JAR放在classpath上
```

#### 3.2.3 API变更

| API | JavaFX 8 | JavaFX 17 | 影响 |
|-----|----------|-----------|------|
| `Platform.exit()` | 可用 | 可用 | 无变化 |
| `Application.launch()` | 可用 | 可用 | 无变化 |
| `FXMLLoader` | 可用 | 可用 | 无变化 |
| `WebView` | 可用 | 可用 | 需要额外模块 |

### 3.3 Gradle 4.5.1 → 7.6+ 变更

#### 3.3.1 插件DSL变更

**旧版 (4.5.1)**:
```gradle
apply plugin: 'java'
apply plugin: 'maven'
```

**新版 (7.6+)**:
```gradle
plugins {
    id 'java-library'
    id 'maven-publish'
}
```

#### 3.3.2 依赖配置变更

**旧版 (4.5.1)**:
```gradle
dependencies {
    compile 'group:artifact:version'
    testCompile 'group:artifact:version'
}
```

**新版 (7.6+)**:
```gradle
dependencies {
    implementation 'group:artifact:version'
    api 'group:artifact:version'  // 对外暴露的API
    testImplementation 'group:artifact:version'
}
```

#### 3.3.3 任务配置变更

**旧版 (4.5.1)**:
```gradle
task sourcesJar(type: Jar, dependsOn: classes) {
    classifier = 'sources'
    from sourceSets.main.allSource
}
```

**新版 (7.6+)**:
```gradle
tasks.register('sourcesJar', Jar) {
    dependsOn tasks.classes
    archiveClassifier.set('sources')
    from sourceSets.main.allSource
}
```

#### 3.3.4 发布配置变更

**旧版 (4.5.1)**:
```gradle
apply plugin: 'maven'

artifacts {
    archives sourcesJar
}
```

**新版 (7.6+)**:
```gradle
plugins {
    id 'maven-publish'
}

publishing {
    publications {
        mavenJava(MavenPublication) {
            from components.java
            artifact tasks.sourcesJar
        }
    }
}
```

---

## 4. 详细迁移步骤

### 4.1 阶段一: 环境准备 (1天)

#### 4.1.1 安装JDK 11

**Windows**:
```powershell
# 使用Chocolatey
choco install openjdk11

# 或手动下载安装
# https://adoptium.net/temurin/releases/?version=11
```

**验证安装**:
```powershell
java -version
# 应显示: openjdk version "11.x.x"
```

#### 4.1.2 安装Gradle 7.6+

**方式1: 使用Gradle Wrapper (推荐)**

在项目根目录创建`gradle/wrapper/gradle-wrapper.properties`:
```properties
distributionBase=GRADLE_USER_HOME
distributionPath=wrapper/dists
distributionUrl=https\://services.gradle.org/distributions/gradle-7.6.4-bin.zip
zipStoreBase=GRADLE_USER_HOME
zipStorePath=wrapper/dists
```

**方式2: 手动安装**
```powershell
# 使用Chocolatey
choco install gradle

# 或手动下载
# https://gradle.org/releases/
```

#### 4.1.3 配置环境变量

```powershell
# 设置JAVA_HOME
$env:JAVA_HOME='C:\Program Files\Eclipse Adoptium\jdk-11.x.x'

# 验证
echo $env:JAVA_HOME
```

---

### 4.2 阶段二: Gradle升级 (2天)

#### 4.2.1 升级common.gradle

**文件**: `ecn/config/common.gradle`

**修改前**:
```gradle
ext {
    javaVersion = 1.8
    encoding = 'UTF-8'
    junitVersion = '4.12'
    projectVersion = '1.2.1-alpha'
    projectGroup = 'cn.cycle.echart'
    
    buildDir = new File(rootProject.projectDir, '../../build/ecn')
    installDir = new File(rootProject.projectDir, '../../install/bin')
    policyFile = new File(rootProject.projectDir, '../config/java.policy')
}

repositories {
    mavenCentral()
    mavenLocal()
}
```

**修改后**:
```gradle
ext {
    javaVersion = 11
    encoding = 'UTF-8'
    junitVersion = '5.9.3'
    projectVersion = '1.3.0-alpha'
    projectGroup = 'cn.cycle.echart'
    
    javafxVersion = '17'
    javafxPlatform = System.getProperty('javafx.platform', 'win')
    controlsfxVersion = '11.2.0'
    
    buildDir = new File(rootProject.projectDir, '../../build/ecn')
    installDir = new File(rootProject.projectDir, '../../install/bin')
    policyFile = new File(rootProject.projectDir, '../config/java.policy')
}

repositories {
    mavenCentral()
    mavenLocal()
}
```

#### 4.2.2 升级非JavaFX模块构建脚本

**示例**: `ecn/echart-core/build.gradle`

**修改前**:
```gradle
apply from: '../config/common.gradle'

apply plugin: 'java'
apply plugin: 'maven'

group projectGroup
version projectVersion

sourceCompatibility = javaVersion
targetCompatibility = javaVersion

[compileJava, compileTestJava, javadoc]*.options*.encoding = encoding

buildDir = new File(projectDir, '../../build/ecn/echart-core')

dependencies {
    testCompile group: 'junit', name: 'junit', version: junitVersion
}

jar {
    baseName = 'echart-core'
    
    manifest {
        attributes 'Implementation-Title': 'E-Chart Core Library',
                   'Implementation-Version': version
    }
}

test {
    testLogging {
        events "passed", "skipped", "failed"
        exceptionFormat "full"
    }
    
    forkEvery = 1
    maxParallelForks = 1
    enableAssertions = false
    jvmArgs = ['-Djava.security.policy=' + policyFile.absolutePath]
}

task sourcesJar(type: Jar, dependsOn: classes) {
    classifier = 'sources'
    from sourceSets.main.allSource
}

task installJar(type: Copy, dependsOn: jar) {
    from jar.archivePath
    into installDir
}

build.finalizedBy installJar

artifacts {
    archives sourcesJar
}
```

**修改后**:
```gradle
apply from: '../config/common.gradle'

plugins {
    id 'java-library'
    id 'maven-publish'
}

group projectGroup
version projectVersion

java {
    sourceCompatibility = JavaVersion.VERSION_11
    targetCompatibility = JavaVersion.VERSION_11
}

tasks.withType(JavaCompile).configureEach {
    options.encoding = encoding
}

buildDir = new File(projectDir, '../../build/ecn/echart-core')

dependencies {
    testImplementation "org.junit.jupiter:junit-jupiter:${junitVersion}"
    testRuntimeOnly "org.junit.jupiter:junit-jupiter-engine:${junitVersion}"
}

jar {
    archiveBaseName.set('echart-core')
    
    manifest {
        attributes 'Implementation-Title': 'E-Chart Core Library',
                   'Implementation-Version': archiveVersion.get()
    }
}

test {
    useJUnitPlatform()
    
    testLogging {
        events "passed", "skipped", "failed"
        exceptionFormat "full"
    }
    
    forkEvery = 1
    maxParallelForks = 1
    enableAssertions = false
    jvmArgs = ['-Djava.security.policy=' + policyFile.absolutePath]
}

tasks.register('sourcesJar', Jar) {
    dependsOn tasks.classes
    archiveClassifier.set('sources')
    from sourceSets.main.allSource
}

tasks.register('installJar', Copy) {
    dependsOn tasks.jar
    from tasks.jar.archiveFile
    into installDir
}

tasks.build.finalizedBy tasks.installJar

publishing {
    publications {
        mavenJava(MavenPublication) {
            from components.java
            artifact tasks.sourcesJar
        }
    }
}
```

#### 4.2.3 升级JavaFX模块构建脚本

**示例**: `ecn/echart-ui/build.gradle`

**修改前**:
```gradle
apply from: '../config/common.gradle'

apply plugin: 'java'
apply plugin: 'maven'

group projectGroup
version projectVersion

sourceCompatibility = javaVersion
targetCompatibility = javaVersion

[compileJava, compileTestJava, javadoc]*.options*.encoding = encoding

buildDir = new File(projectDir, '../../build/ecn/echart-ui')

dependencies {
    compile project(':echart-core')
    compile project(':echart-i18n')
    compile project(':echart-data')
    compile project(':echart-alarm')
    compile project(':echart-ais')
    compile project(':echart-route')
    compile project(':echart-workspace')
    compile project(':echart-theme')
    compile project(':echart-render')
    compile files('../../install/bin/fxribbon-2.1.0.jar')
    compile 'org.controlsfx:controlsfx:8.40.18'
    testCompile group: 'junit', name: 'junit', version: junitVersion
}

// ... 其余配置
```

**修改后**:
```gradle
apply from: '../config/common.gradle'

plugins {
    id 'java-library'
    id 'maven-publish'
    id 'org.openjfx.javafxplugin' version '0.1.0'
}

group projectGroup
version projectVersion

java {
    sourceCompatibility = JavaVersion.VERSION_11
    targetCompatibility = JavaVersion.VERSION_11
}

tasks.withType(JavaCompile).configureEach {
    options.encoding = encoding
}

buildDir = new File(projectDir, '../../build/ecn/echart-ui')

javafx {
    version = javafxVersion
    modules = ['javafx.controls', 'javafx.fxml', 'javafx.graphics']
}

dependencies {
    api project(':echart-core')
    api project(':echart-i18n')
    api project(':echart-data')
    api project(':echart-alarm')
    api project(':echart-ais')
    api project(':echart-route')
    api project(':echart-workspace')
    api project(':echart-theme')
    api project(':echart-render')
    implementation files('../../install/bin/fxribbon-2.1.0.jar')
    implementation "org.controlsfx:controlsfx:${controlsfxVersion}"
    
    testImplementation "org.junit.jupiter:junit-jupiter:${junitVersion}"
    testRuntimeOnly "org.junit.jupiter:junit-jupiter-engine:${junitVersion}"
}

jar {
    archiveBaseName.set('echart-ui')
    
    manifest {
        attributes 'Implementation-Title': 'E-Chart UI Library',
                   'Implementation-Version': archiveVersion.get()
    }
}

test {
    useJUnitPlatform()
    
    testLogging {
        events "passed", "skipped", "failed"
        exceptionFormat "full"
    }
    
    forkEvery = 1
    maxParallelForks = 1
    enableAssertions = false
    jvmArgs = ['-Djava.security.policy=' + policyFile.absolutePath]
}

tasks.register('sourcesJar', Jar) {
    dependsOn tasks.classes
    archiveClassifier.set('sources')
    from sourceSets.main.allSource
}

tasks.register('installJar', Copy) {
    dependsOn tasks.jar
    from tasks.jar.archiveFile
    into installDir
}

tasks.build.finalizedBy tasks.installJar

publishing {
    publications {
        mavenJava(MavenPublication) {
            from components.java
            artifact tasks.sourcesJar
        }
    }
}
```

---

### 4.3 阶段三: 代码迁移 (3天)

#### 4.3.1 迁移javawrapper模块

**文件**: `cycle/javawrapper/build.gradle`

**修改前**:
```gradle
plugins {
    id 'java-library'
}

group 'cn.cycle.chart'

sourceCompatibility = JavaVersion.VERSION_1_8
targetCompatibility = JavaVersion.VERSION_1_8

repositories {
    mavenCentral()
}

dependencies {
    testImplementation 'junit:junit:4.12'
}

tasks.withType(JavaCompile) {
    options.encoding = 'UTF-8'
}

jar {
    destinationDir = new File(rootDir, '../build/test')
    manifest {
        attributes(
            'Implementation-Title': 'Cycle Chart Java Wrapper',
            'Implementation-Version': version
        )
    }
}

test {
    workingDir = new File(rootDir, '../build/test')
    jvmArgs = ['-Djava.library.path=' + new File(rootDir, '../build/test').absolutePath]
}
```

**修改后**:
```gradle
plugins {
    id 'java-library'
}

group 'cn.cycle.chart'

java {
    sourceCompatibility = JavaVersion.VERSION_11
    targetCompatibility = JavaVersion.VERSION_11
}

repositories {
    mavenCentral()
}

dependencies {
    testImplementation "org.junit.jupiter:junit-jupiter:${junitVersion}"
    testRuntimeOnly "org.junit.jupiter:junit-jupiter-engine:${junitVersion}"
}

tasks.withType(JavaCompile).configureEach {
    options.encoding = 'UTF-8'
}

jar {
    destinationDirectory.set(new File(rootDir, '../build/test'))
    manifest {
        attributes(
            'Implementation-Title': 'Cycle Chart Java Wrapper',
            'Implementation-Version': archiveVersion.get()
        )
    }
}

test {
    useJUnitPlatform()
    workingDir = new File(rootDir, '../build/test')
    jvmArgs = ['-Djava.library.path=' + new File(rootDir, '../build/test').absolutePath]
}
```

#### 4.3.2 迁移fxribbon模块

**文件**: `ecn/fxribbon/build.gradle`

**修改前**:
```gradle
apply from: '../config/common.gradle'

apply plugin: 'java'
apply plugin: 'maven'

group projectGroup
version '2.1.0'

sourceCompatibility = javaVersion
targetCompatibility = javaVersion

[compileJava, compileTestJava, javadoc]*.options*.encoding = encoding

buildDir = new File(projectDir, '../../build/ecn/fxribbon')

dependencies {
    implementation 'org.openjfx:javafx-controls:11:win'
    implementation 'org.openjfx:javafx-graphics:11:win'
    testCompile group: 'junit', name: 'junit', version: junitVersion
}

// ... 其余配置
```

**修改后**:
```gradle
apply from: '../config/common.gradle'

plugins {
    id 'java-library'
    id 'maven-publish'
    id 'org.openjfx.javafxplugin' version '0.1.0'
}

group projectGroup
version '2.1.0'

java {
    sourceCompatibility = JavaVersion.VERSION_11
    targetCompatibility = JavaVersion.VERSION_11
}

tasks.withType(JavaCompile).configureEach {
    options.encoding = encoding
}

buildDir = new File(projectDir, '../../build/ecn/fxribbon')

javafx {
    version = javafxVersion
    modules = ['javafx.controls', 'javafx.graphics']
}

dependencies {
    testImplementation "org.junit.jupiter:junit-jupiter:${junitVersion}"
    testRuntimeOnly "org.junit.jupiter:junit-jupiter-engine:${junitVersion}"
}

jar {
    archiveBaseName.set('fxribbon')
    
    manifest {
        attributes 'Implementation-Title': 'Cycle FXRibbon Library',
                   'Implementation-Version': archiveVersion.get()
    }
}

test {
    useJUnitPlatform()
    
    testLogging {
        events "passed", "skipped", "failed"
        exceptionFormat "full"
    }
    
    forkEvery = 1
    maxParallelForks = 1
    enableAssertions = false
    jvmArgs = ['-Djava.security.policy=' + policyFile.absolutePath]
}

tasks.register('sourcesJar', Jar) {
    dependsOn tasks.classes
    archiveClassifier.set('sources')
    from sourceSets.main.allSource
}

tasks.register('installJar', Copy) {
    dependsOn tasks.jar
    from tasks.jar.archiveFile
    into installDir
}

tasks.build.finalizedBy tasks.installJar

publishing {
    publications {
        mavenJava(MavenPublication) {
            from components.java
            artifact tasks.sourcesJar
        }
    }
}
```

#### 4.3.3 代码兼容性修改

**问题1: JAXB移除**

**修改前**:
```java
import javax.xml.bind.JAXBContext;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;

// 使用JAXB
JAXBContext context = JAXBContext.newInstance(MyClass.class);
Marshaller marshaller = context.createMarshaller();
```

**修改后**:
```java
// 方案1: 添加JAXB依赖
// build.gradle:
// implementation 'jakarta.xml.bind:jakarta.xml.bind-api:4.0.0'
// runtimeOnly 'org.glassfish.jaxb:jaxb-runtime:4.0.2'

import jakarta.xml.bind.JAXBContext;
import jakarta.xml.bind.Marshaller;
import jakarta.xml.bind.Unmarshaller;

// 使用JAXB
JAXBContext context = JAXBContext.newInstance(MyClass.class);
Marshaller marshaller = context.createMarshaller();
```

**问题2: JavaFX应用启动**

**修改前**:
```java
public class MainApp extends Application {
    public static void main(String[] args) {
        launch(args);
    }
}
```

**修改后**:
```java
public class MainApp extends Application {
    public static void main(String[] args) {
        // JDK 11+ 需要显式指定模块
        // 方式1: 使用module-info.java
        // 方式2: 使用--add-modules参数
        launch(args);
    }
}
```

**问题3: ControlsFX兼容性**

**修改前**:
```gradle
dependencies {
    compile 'org.controlsfx:controlsfx:8.40.18'
}
```

**修改后**:
```gradle
dependencies {
    implementation "org.controlsfx:controlsfx:${controlsfxVersion}"
}
```

---

### 4.4 阶段四: 测试与验证 (2天)

#### 4.4.1 编译测试

**测试命令**:
```powershell
# 设置JAVA_HOME
$env:JAVA_HOME='C:\Program Files\Eclipse Adoptium\jdk-11.0.20.101-hotspot'

# 编译单个模块
.\gradlew.bat -p ecn/echart-core build -x test

# 编译所有模块
.\gradlew.bat build -x test
```

**预期结果**:
- 所有模块编译成功
- 无编译错误
- 无弃用警告（或仅有少量）

#### 4.4.2 单元测试

**测试命令**:
```powershell
# 运行单个模块测试
.\gradlew.bat -p ecn/echart-core test

# 运行所有测试
.\gradlew.bat test
```

**预期结果**:
- 所有测试通过
- 测试覆盖率 > 80%

#### 4.4.3 集成测试

**测试场景**:
1. 启动应用程序
2. 打开海图文件
3. 显示图层管理面板
4. 切换图层可见性
5. 查看属性面板

**预期结果**:
- 应用启动正常
- UI显示正常
- 功能运行正常

#### 4.4.4 性能测试

**测试指标**:
| 指标 | JDK 8 | JDK 11 | 目标 |
|------|-------|--------|------|
| 启动时间 | X秒 | Y秒 | ≤ X秒 |
| 内存占用 | XMB | YMB | ≤ XMB |
| 响应时间 | Xms | Yms | ≤ Xms |

---

### 4.5 阶段五: 部署与发布 (1天)

#### 4.5.1 更新文档

**需要更新的文档**:
1. `ecn/index.md` - 更新环境要求
2. `ecn/config/common.gradle` - 更新配置
3. `README.md` - 更新构建说明

#### 4.5.2 更新CI/CD

**Jenkins Pipeline示例**:
```groovy
pipeline {
    agent any
    
    tools {
        jdk 'JDK 11'
        gradle 'Gradle 7.6'
    }
    
    stages {
        stage('Build') {
            steps {
                sh './gradlew build'
            }
        }
        
        stage('Test') {
            steps {
                sh './gradlew test'
            }
        }
        
        stage('Deploy') {
            steps {
                sh './gradlew publish'
            }
        }
    }
}
```

#### 4.5.3 发布版本

**版本号变更**:
- 旧版本: `1.2.1-alpha`
- 新版本: `1.3.0-alpha`

---

## 5. 迁移任务清单

### 5.1 ECN模块迁移清单

| 序号 | 模块 | 优先级 | 复杂度 | 预估工时 | 状态 |
|------|------|--------|--------|----------|------|
| 1 | echart-core | P0 | 低 | 2h | 📋 Todo |
| 2 | echart-i18n | P0 | 低 | 1h | 📋 Todo |
| 3 | echart-data | P0 | 低 | 1h | 📋 Todo |
| 4 | echart-render | P0 | 低 | 1h | 📋 Todo |
| 5 | echart-alarm | P0 | 低 | 1h | 📋 Todo |
| 6 | echart-ais | P0 | 低 | 1h | 📋 Todo |
| 7 | echart-route | P0 | 低 | 1h | 📋 Todo |
| 8 | echart-workspace | P0 | 低 | 1h | 📋 Todo |
| 9 | echart-theme | P0 | 低 | 1h | 📋 Todo |
| 10 | echart-plugin | P0 | 低 | 1h | 📋 Todo |
| 11 | echart-facade | P0 | 低 | 1h | 📋 Todo |
| 12 | fxribbon | P0 | 高 | 8h | 📋 Todo |
| 13 | echart-ui | P0 | 高 | 12h | 📋 Todo |
| 14 | echart-ui-render | P0 | 高 | 8h | 📋 Todo |
| 15 | echart-app | P0 | 高 | 8h | 📋 Todo |

**总预估工时**: 50小时

### 5.2 Cycle模块迁移清单

| 序号 | 模块 | 优先级 | 复杂度 | 预估工时 | 状态 |
|------|------|--------|--------|----------|------|
| 1 | javawrapper | P0 | 中 | 3h | 📋 Todo |
| 2 | fxribbon | P0 | 高 | 4h | 📋 Todo |
| 3 | javafx-app | P0 | 高 | 4h | 📋 Todo |

**总预估工时**: 11小时

### 5.3 总体迁移计划

| 阶段 | 任务 | 预估工时 | 负责人 |
|------|------|----------|--------|
| 阶段一 | 环境准备 | 8h | 运维工程师 |
| 阶段二 | Gradle升级 | 16h | 开发工程师 |
| 阶段三 | 代码迁移 | 44h | 开发工程师 |
| 阶段四 | 测试验证 | 16h | 测试工程师 |
| 阶段五 | 部署发布 | 8h | 运维工程师 |

**总预估工时**: 92小时 (12人天)

---

## 6. 风险评估

### 6.1 技术风险

| 风险ID | 风险描述 | 概率 | 影响 | 缓解措施 | 责任人 |
|--------|----------|------|------|----------|--------|
| TR-001 | JavaFX模块化兼容性问题 | 高 | 高 | 1. 使用非模块化JAR<br>2. 添加--add-modules参数<br>3. 创建module-info.java | 开发工程师 |
| TR-002 | 第三方库不兼容JDK 11 | 中 | 高 | 1. 升级到兼容版本<br>2. 寻找替代库<br>3. 自行修复 | 开发工程师 |
| TR-003 | JNI本地库不兼容 | 中 | 高 | 1. 重新编译本地库<br>2. 使用JDK 11编译 | 开发工程师 |
| TR-004 | 性能下降 | 低 | 中 | 1. JVM参数调优<br>2. 代码优化 | 开发工程师 |
| TR-005 | 内存泄漏 | 中 | 高 | 1. 内存分析<br>2. 修复泄漏点 | 开发工程师 |

### 6.2 进度风险

| 风险ID | 风险描述 | 概率 | 影响 | 缓解措施 | 责任人 |
|--------|----------|------|------|----------|--------|
| SR-001 | 迁移时间超出预期 | 中 | 中 | 1. 预留缓冲时间<br>2. 分阶段迁移 | 项目经理 |
| SR-002 | 测试发现大量问题 | 中 | 高 | 1. 增加测试资源<br>2. 并行修复 | 测试工程师 |
| SR-003 | 团队成员不熟悉新技术 | 中 | 中 | 1. 技术培训<br>2. 文档支持 | 技术负责人 |

### 6.3 业务风险

| 风险ID | 风险描述 | 概率 | 影响 | 缓解措施 | 责任人 |
|--------|----------|------|------|----------|--------|
| BR-001 | 功能回归 | 中 | 高 | 1. 完整回归测试<br>2. 自动化测试 | 测试工程师 |
| BR-002 | 用户体验变化 | 低 | 低 | 1. 用户培训<br>2. 更新文档 | 产品经理 |

---

## 7. 回滚计划

### 7.1 回滚触发条件

- 迁移后性能下降超过20%
- 关键功能无法正常工作
- 无法在规定时间内解决的重大问题

### 7.2 回滚步骤

1. **停止新版本部署**
   ```powershell
   # 停止应用服务
   Stop-Service EChartApp
   ```

2. **恢复JDK 8环境**
   ```powershell
   # 切换JAVA_HOME
   $env:JAVA_HOME='F:\enc\java'
   
   # 验证版本
   java -version
   ```

3. **恢复Gradle 4.5.1**
   ```powershell
   # 使用旧版Gradle
   gradle/gradle-4.5.1/bin/gradle.bat clean build
   ```

4. **恢复代码版本**
   ```powershell
   # Git回滚
   git checkout <previous-version-tag>
   ```

5. **重新部署**
   ```powershell
   # 部署旧版本
   .\install\bin\app_echart.bat
   ```

### 7.3 回滚验证

- [ ] 应用启动正常
- [ ] 功能测试通过
- [ ] 性能恢复正常

---

## 8. 附录

### 8.1 参考资料

- [JDK 11 Migration Guide](https://docs.oracle.com/en/java/javase/11/migrate/)
- [JavaFX 17 Documentation](https://openjfx.io/javadoc/17/)
- [Gradle 7.6 Release Notes](https://docs.gradle.org/7.6/release-notes.html)
- [ControlsFX 11 Documentation](https://controlsfx.github.io/)

### 8.2 常见问题

**Q1: 为什么选择JDK 11而不是JDK 17？**
A: JDK 11是LTS版本，稳定性更好，且迁移路径更清晰。完成JDK 11迁移后，可以再考虑升级到JDK 17。

**Q2: JavaFX 11和JavaFX 17有什么区别？**
A: JavaFX 17是更新的LTS版本，修复了一些bug，增加了新特性。建议直接使用JavaFX 17。

**Q3: 是否需要创建module-info.java？**
A: 初期建议不使用模块化，以减少迁移复杂度。待稳定后再考虑模块化。

**Q4: 如何处理JNI本地库？**
A: 需要使用JDK 11重新编译本地库，确保兼容性。

### 8.3 变更历史

| 版本 | 日期 | 变更内容 | 变更人 |
|------|------|----------|--------|
| v1.0 | 2026-04-24 | 初始版本 | AI Migration Planner |
| v1.1 | 2026-04-25 | 根据检查报告修正: P0-修正ControlsFX版本号格式(${controlsfxVersion}); P1-统一JUnit版本引用(${junitVersion}); P1-JavaFX平台自动检测(System.getProperty); P1-fxribbon版本保持2.1.0; P2-调整JavaFX模块工时估算; 新增多端复用分类列 | AI Code Reviewer |

---

**文档维护**: Cycle Team  
**技术支持**: 参见项目 README.md
