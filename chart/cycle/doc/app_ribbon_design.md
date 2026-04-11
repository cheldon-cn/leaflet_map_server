# FXRibbon集成设计文档

**文档版本**: v1.2  
**创建日期**: 2026-04-11  
**更新日期**: 2026-04-11  
**目标**: 将FXRibbon库集成到javafx-app中，替换传统MenuBar/ToolBar，提升用户体验

---

## 一、项目分析

### 1.1 FXRibbon原始项目结构

| 目录 | 内容 | 文件数 |
|------|------|--------|
| `com/pixelduke/control/` | Ribbon主控件 | 1个 (Ribbon.java) |
| `com/pixelduke/control/ribbon/` | 子控件 | 5个 |
| `impl/com/pixelduke/skin/` | 皮肤实现 | 1个 (RibbonSkin.java) |
| `impl/com/pixelduke/skin/ribbon/` | 子控件皮肤 | 3个 |
| `resources/com/pixelduke/control/` | CSS样式 | 1个 (fxribbon.css) |

**核心类说明**:

| 类名 | 功能 | 原始位置 | 新位置 |
|------|------|----------|--------|
| `Ribbon` | Ribbon主控件 | com.pixelduke.control | com.cycle.control |
| `RibbonTab` | Ribbon标签页 | com.pixelduke.control.ribbon | com.cycle.control.ribbon |
| `RibbonGroup` | Ribbon分组 | com.pixelduke.control.ribbon | com.cycle.control.ribbon |
| `RibbonItem` | Ribbon项 | com.pixelduke.control.ribbon | com.cycle.control.ribbon |
| `Column` | 列布局容器 | com.pixelduke.control.ribbon | com.cycle.control.ribbon |
| `QuickAccessBar` | 快速访问工具栏 | com.pixelduke.control.ribbon | com.cycle.control.ribbon |

### 1.2 javafx-app当前结构

| 组件 | 当前实现 | 功能 |
|------|----------|------|
| MainView | BorderPane | 主视图布局 |
| MenuBar | javafx.scene.control.MenuBar | 传统菜单栏 |
| ToolBar | javafx.scene.control.ToolBar | 传统工具栏 |
| ChartCanvas | Canvas | 海图显示区域 |

### 1.3 依赖关系

```
fxribbon (新模块)
├── JavaFX 8+ (必需)
└── 无其他外部依赖
```

---

## 二、集成方案

### 2.1 方案选择：独立JAR包 + 包名重构

**实施方式**: 
1. 创建新模块 `cycle/fxribbon`（JAR包根目录）
2. 拷贝FXRibbon源码
3. **包名重构**: `com.pixelduke` → `com.cycle`
4. 批量修改所有import语句和引用路径
5. 编译为独立jar包，供javafx-app依赖

**优点**:
- 模块解耦，便于维护
- 包名符合项目规范 (`com.cycle`)
- 可复用于其他项目
- 版本管理清晰

**包名映射规则**:

| 原包名 | 新包名 |
|--------|--------|
| `com.pixelduke.control` | `com.cycle.control` |
| `com.pixelduke.control.ribbon` | `com.cycle.control.ribbon` |
| `impl.com.pixelduke.skin` | `com.cycle.skin` |
| `impl.com.pixelduke.skin.ribbon` | `com.cycle.skin.ribbon` |

---

## 三、模块结构设计

### 3.1 新模块目录结构

```
cycle/
├── fxribbon/                              # 新建模块（JAR包根目录）
│   ├── build.gradle
│   └── src/main/
│       ├── java/
│       │   └── com/cycle/                 # 包名根目录
│       │       ├── control/               # 控件包
│       │       │   ├── Ribbon.java
│       │       │   └── ribbon/
│       │       │       ├── Column.java
│       │       │       ├── QuickAccessBar.java
│       │       │       ├── RibbonGroup.java
│       │       │       ├── RibbonItem.java
│       │       │       └── RibbonTab.java
│       │       └── skin/                  # 皮肤包（impl目录提升到com/cycle下）
│       │           ├── RibbonSkin.java
│       │           └── ribbon/
│       │               ├── QuickAccessBarSkin.java
│       │               ├── RibbonGroupSkin.java
│       │               └── RibbonItemSkin.java
│       └── resources/
│           └── com/cycle/control/         # 资源路径同步修改
│               └── fxribbon.css
├── javafx-app/
│   ├── build.gradle                       # 添加fxribbon依赖
│   └── src/main/java/cn/cycle/app/
│       └── view/
│           └── MainView.java              # 使用Ribbon替换MenuBar/ToolBar
└── settings.gradle                        # 添加fxribbon模块
```

### 3.2 目录结构变更对照

| 原路径 | 新路径 |
|--------|--------|
| `com/pixelduke/control/Ribbon.java` | `com/cycle/control/Ribbon.java` |
| `com/pixelduke/control/ribbon/*.java` | `com/cycle/control/ribbon/*.java` |
| `impl/com/pixelduke/skin/*.java` | `com/cycle/skin/*.java` |
| `impl/com/pixelduke/skin/ribbon/*.java` | `com/cycle/skin/ribbon/*.java` |
| `com/pixelduke/control/fxribbon.css` | `com/cycle/control/fxribbon.css` |

---

## 四、文件修改清单

### 4.1 Java文件包名修改

每个Java文件需要修改以下内容：

**1. package声明**:
```java
// 修改前
package com.pixelduke.control;
package com.pixelduke.control.ribbon;
package impl.com.pixelduke.skin;
package impl.com.pixelduke.skin.ribbon;

// 修改后
package com.cycle.control;
package com.cycle.control.ribbon;
package com.cycle.skin;
package com.cycle.skin.ribbon;
```

**2. import语句**:
```java
// 修改前
import com.pixelduke.control.Ribbon;
import com.pixelduke.control.ribbon.*;
import impl.com.pixelduke.skin.RibbonSkin;

// 修改后
import com.cycle.control.Ribbon;
import com.cycle.control.ribbon.*;
import com.cycle.skin.RibbonSkin;
```

**3. CSS资源路径**:
```java
// 修改前 (Ribbon.java)
return Ribbon.class.getResource("fxribbon.css").toExternalForm();

// 修改后 (保持不变，因为CSS在同目录下)
return Ribbon.class.getResource("fxribbon.css").toExternalForm();
```

### 4.2 CSS文件修改

fxribbon.css中的样式类名保持不变，但注释中的版权声明需保留：

```css
/*
 * Copyright (c) 2016, 2018 Pixel Duke (Pedro Duque Vieira - www.pixelduke.com)
 * Modified by Cycle Team for integration
 * Original: BSD 3-Clause License
 */
```

### 4.3 文件修改详情

| 文件 | 修改内容 |
|------|----------|
| `Ribbon.java` | package声明、import语句、CSS路径 |
| `RibbonTab.java` | package声明、import语句 |
| `RibbonGroup.java` | package声明、import语句 |
| `RibbonItem.java` | package声明、import语句 |
| `Column.java` | package声明 |
| `QuickAccessBar.java` | package声明、import语句 |
| `RibbonSkin.java` | package声明、import语句 |
| `QuickAccessBarSkin.java` | package声明、import语句 |
| `RibbonGroupSkin.java` | package声明、import语句 |
| `RibbonItemSkin.java` | package声明、import语句 |

---

## 五、构建配置

### 5.1 fxribbon/build.gradle

```gradle
plugins {
    id 'java-library'
}

group 'com.cycle'
version '1.2.2'

sourceCompatibility = JavaVersion.VERSION_1_8
targetCompatibility = JavaVersion.VERSION_1_8

[compileJava, compileTestJava]*.options*.encoding = 'UTF-8'

jar {
    destinationDir = new File(rootDir, '../code/build/test')
    manifest {
        attributes(
            'Implementation-Title': 'Cycle FXRibbon',
            'Implementation-Version': version,
            'Original-Author': 'Pixel Duke (Pedro Duque Vieira)',
            'License': 'BSD 3-Clause'
        )
    }
}
```

### 5.2 settings.gradle修改

```gradle
include 'jni', 'javawrapper', 'javafx-app', 'fxribbon'
```

### 5.3 javafx-app/build.gradle修改

```gradle
dependencies {
    implementation project(':javawrapper')
    implementation project(':fxribbon')  // 添加fxribbon依赖
    testImplementation 'junit:junit:4.12'
}
```

---

## 六、MainView改造设计

### 6.1 改造后代码

```java
package cn.cycle.app.view;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.app.controller.MainController;

// 使用新包名导入FXRibbon
import com.cycle.control.Ribbon;
import com.cycle.control.ribbon.RibbonTab;
import com.cycle.control.ribbon.RibbonGroup;
import com.cycle.control.ribbon.RibbonItem;
import com.cycle.control.ribbon.Column;
import com.cycle.control.ribbon.QuickAccessBar;

import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.layout.BorderPane;

public class MainView extends BorderPane {

    private final MainController controller;
    private final ChartCanvas chartCanvas;
    private final Ribbon ribbon;

    public MainView(ChartViewer chartViewer) {
        this.controller = new MainController(chartViewer);
        this.chartCanvas = new ChartCanvas(chartViewer);
        this.ribbon = createRibbon();

        setTop(ribbon);
        setCenter(chartCanvas);
    }

    private Ribbon createRibbon() {
        Ribbon ribbon = new Ribbon();

        // 文件标签页
        RibbonTab fileTab = createFileTab();
        
        // 视图标签页
        RibbonTab viewTab = createViewTab();
        
        // 工具标签页
        RibbonTab toolsTab = createToolsTab();

        ribbon.getTabs().addAll(fileTab, viewTab, toolsTab);
        return ribbon;
    }

    private RibbonTab createFileTab() {
        RibbonTab tab = new RibbonTab("文件");
        
        RibbonGroup group = new RibbonGroup("文件操作");
        
        Button openBtn = new Button("打开");
        openBtn.setOnAction(e -> controller.openChart());
        
        Button exportBtn = new Button("导出");
        exportBtn.setOnAction(e -> controller.exportChart());
        
        group.getNodes().addAll(openBtn, exportBtn);
        tab.getRibbonGroups().add(group);
        
        return tab;
    }

    private RibbonTab createViewTab() {
        RibbonTab tab = new RibbonTab("视图");
        
        RibbonGroup zoomGroup = new RibbonGroup("缩放");
        
        Button zoomInBtn = new Button("放大");
        zoomInBtn.setOnAction(e -> controller.zoomIn());
        
        Button zoomOutBtn = new Button("缩小");
        zoomOutBtn.setOnAction(e -> controller.zoomOut());
        
        Button fitBtn = new Button("适应窗口");
        fitBtn.setOnAction(e -> controller.fitToWindow());
        
        zoomGroup.getNodes().addAll(zoomInBtn, zoomOutBtn, fitBtn);
        tab.getRibbonGroups().add(zoomGroup);
        
        return tab;
    }

    private RibbonTab createToolsTab() {
        RibbonTab tab = new RibbonTab("工具");
        
        RibbonGroup measureGroup = new RibbonGroup("测量");
        
        Button distanceBtn = new Button("距离测量");
        distanceBtn.setOnAction(e -> controller.startDistanceMeasure());
        
        Button areaBtn = new Button("面积测量");
        areaBtn.setOnAction(e -> controller.startAreaMeasure());
        
        Button routeBtn = new Button("航线规划");
        routeBtn.setOnAction(e -> controller.openRoutePlanner());
        
        measureGroup.getNodes().addAll(distanceBtn, areaBtn, routeBtn);
        tab.getRibbonGroups().add(measureGroup);
        
        return tab;
    }

    public ChartCanvas getChartCanvas() {
        return chartCanvas;
    }

    public MainController getController() {
        return controller;
    }
}
```

---

## 七、Ribbon标签页设计

### 7.1 标签页规划

| 标签页 | 分组 | 功能 |
|--------|------|------|
| **文件** | 文件操作 | 打开、保存、导出 |
| | 打印 | 打印预览、打印设置 |
| **视图** | 缩放 | 放大、缩小、适应窗口、指定比例 |
| | 显示 | 显示网格、显示要素标签、显示比例尺 |
| | 窗口 | 全屏、分屏 |
| **工具** | 测量 | 距离测量、面积测量、方位角测量 |
| | 航行 | 航线规划、航点管理 |
| | 查询 | 要素查询、属性查询 |
| **图层** | 图层管理 | 图层列表、可见性控制、顺序调整 |
| | 图层操作 | 添加图层、移除图层、图层属性 |
| **设置** | 显示设置 | 显示模式、颜色方案、符号库 |
| | 系统设置 | 语言、单位、路径配置 |
| **帮助** | 帮助 | 用户手册、快捷键、在线帮助 |
| | 关于 | 版本信息、许可证 |

### 7.2 快速访问工具栏设计

| 按钮 | 功能 | 图标 |
|------|------|------|
| 打开 | 打开海图文件 | 文件夹图标 |
| 保存 | 保存当前状态 | 保存图标 |
| 撤销 | 撤销上一步操作 | 撤销图标 |
| 重做 | 重做操作 | 重做图标 |
| 放大 | 放大视图 | 放大镜+图标 |
| 缩小 | 缩小视图 | 放大镜-图标 |

---

## 八、实施计划

### 8.1 阶段一：模块创建与文件拷贝（预计0.5天）

| 任务 | 说明 |
|------|------|
| 1. 创建目录结构 | cycle/fxribbon/src/main/java/com/cycle/... |
| 2. 拷贝Java文件 | 从FXRibbon-master拷贝到新目录 |
| 3. 拷贝CSS文件 | 拷贝fxribbon.css到resources目录 |
| 4. 创建build.gradle | 配置构建脚本 |

### 8.2 阶段二：包名重构（预计0.5天）

| 任务 | 说明 |
|------|------|
| 1. 修改package声明 | 所有Java文件的package语句 |
| 2. 修改import语句 | 所有Java文件的import语句 |
| 3. 验证编译 | 确保无编译错误 |

### 8.3 阶段三：集成与测试（预计1天）

| 任务 | 说明 |
|------|------|
| 1. 添加模块依赖 | javafx-app依赖fxribbon |
| 2. 改造MainView | 使用Ribbon替换MenuBar/ToolBar |
| 3. 功能测试 | 验证所有功能正常 |

### 8.4 阶段四：功能扩展（预计2天）

| 任务 | 说明 |
|------|------|
| 1. 图层标签页 | 实现图层管理功能 |
| 2. 设置标签页 | 实现设置功能 |
| 3. 帮助标签页 | 实现帮助功能 |
| 4. 样式定制 | 调整CSS以匹配应用风格 |

---

## 九、样式定制

### 9.1 CSS文件位置

```
fxribbon/src/main/resources/com/cycle/control/fxribbon.css
```

### 9.2 海图应用定制建议

```css
/*
 * Copyright (c) 2016, 2018 Pixel Duke
 * Modified for Cycle Chart Application
 */

/* 海图应用主题色 */
.ribbon {
    -fx-background-color: derive(#2B579A, 80%);
}

.ribbon-tab:selected {
    -fx-background-color: #2B579A;
    -fx-text-fill: white;
}

.ribbon-group-title {
    -fx-text-fill: #2B579A;
    -fx-font-weight: bold;
}

.quick-access-bar {
    -fx-background-color: #2B579A;
}
```

---

## 十、注意事项

### 10.1 许可证兼容性

FXRibbon使用**BSD 3-Clause**许可证：
- ✅ 允许商业使用
- ✅ 允许修改和分发
- ⚠️ 需保留原始版权声明

### 10.2 版权声明保留

在每个Java文件头部保留原始版权声明：

```java
/*
 * Copyright (c) 2016, 2018 Pixel Duke (Pedro Duque Vieira - www.pixelduke.com)
 * All rights reserved.
 * 
 * Modified by Cycle Team for integration into Cycle Chart Application
 * Original license: BSD 3-Clause
 * ...
 */
```

### 10.3 Java版本兼容

FXRibbon要求Java 8+，与项目要求一致。

### 10.4 JavaFX版本兼容

FXRibbon兼容JavaFX 8+，当前项目使用JavaFX 8，无兼容问题。

---

## 十一、测试验证

### 11.1 功能测试清单

| 测试项 | 预期结果 |
|--------|----------|
| Ribbon显示 | 正确显示所有标签页和分组 |
| 标签页切换 | 点击标签页可切换内容 |
| 按钮点击 | 触发对应功能 |
| 快速访问栏 | 按钮功能正常 |
| 样式加载 | CSS正确应用 |
| 窗口缩放 | Ribbon自适应宽度 |

### 11.2 集成测试清单

| 测试项 | 预期结果 |
|--------|----------|
| 打开海图 | 文件选择对话框弹出 |
| 缩放操作 | 海图正确缩放 |
| 测量工具 | 测量功能正常 |
| 图层管理 | 图层可见性切换正常 |

---

## 十二、附录

### 12.1 文件拷贝与重命名清单

| 源文件 | 目标文件 |
|--------|----------|
| `FXRibbon/src/main/java/com/pixelduke/control/Ribbon.java` | `fxribbon/src/main/java/com/cycle/control/Ribbon.java` |
| `FXRibbon/src/main/java/com/pixelduke/control/ribbon/Column.java` | `fxribbon/src/main/java/com/cycle/control/ribbon/Column.java` |
| `FXRibbon/src/main/java/com/pixelduke/control/ribbon/QuickAccessBar.java` | `fxribbon/src/main/java/com/cycle/control/ribbon/QuickAccessBar.java` |
| `FXRibbon/src/main/java/com/pixelduke/control/ribbon/RibbonGroup.java` | `fxribbon/src/main/java/com/cycle/control/ribbon/RibbonGroup.java` |
| `FXRibbon/src/main/java/com/pixelduke/control/ribbon/RibbonItem.java` | `fxribbon/src/main/java/com/cycle/control/ribbon/RibbonItem.java` |
| `FXRibbon/src/main/java/com/pixelduke/control/ribbon/RibbonTab.java` | `fxribbon/src/main/java/com/cycle/control/ribbon/RibbonTab.java` |
| `FXRibbon/src/main/java/impl/com/pixelduke/skin/RibbonSkin.java` | `fxribbon/src/main/java/com/cycle/skin/RibbonSkin.java` |
| `FXRibbon/src/main/java/impl/com/pixelduke/skin/ribbon/QuickAccessBarSkin.java` | `fxribbon/src/main/java/com/cycle/skin/ribbon/QuickAccessBarSkin.java` |
| `FXRibbon/src/main/java/impl/com/pixelduke/skin/ribbon/RibbonGroupSkin.java` | `fxribbon/src/main/java/com/cycle/skin/ribbon/RibbonGroupSkin.java` |
| `FXRibbon/src/main/java/impl/com/pixelduke/skin/ribbon/RibbonItemSkin.java` | `fxribbon/src/main/java/com/cycle/skin/ribbon/RibbonItemSkin.java` |
| `FXRibbon/src/main/resources/com/pixelduke/control/fxribbon.css` | `fxribbon/src/main/resources/com/cycle/control/fxribbon.css` |

### 12.2 参考资源

- FXRibbon官方文档: http://www.pixelduke.com/fxribbon
- FXRibbon GitHub: https://github.com/dukke/FXRibbon
- Microsoft Ribbon设计指南: https://docs.microsoft.com/en-us/windows/win32/uxguide/ribbons

---

**文档维护**: 随集成进度更新  
**负责人**: 开发团队
