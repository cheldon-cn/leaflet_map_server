# E-Chart应用界面布局与交互逻辑文档

**版本**: v1.1  
**日期**: 2026-04-27  
**目的**: 辅助大模型后期重新完美复刻当前已有成果

---

## 0. 技术栈版本说明

### 0.1 当前技术栈

| 组件 | 当前版本 | 说明 |
|------|----------|------|
| JDK | 1.8 | Oracle JDK 8 |
| JavaFX | 8.x | JDK内置版本 |
| Gradle | 4.5.1 | 构建工具 |
| ControlsFX | 8.40.x | UI组件库 |

### 0.2 迁移计划

| 组件 | 目标版本 | 迁移影响 |
|------|----------|----------|
| JDK | 11+ | 需移除JavaFX模块，使用独立JavaFX SDK |
| JavaFX | 11/17 | 需更新模块系统配置，修改启动方式 |
| Gradle | 7.6+ | 需更新构建脚本语法，使用新的插件API |

### 0.3 迁移注意事项

**JDK 8 → JDK 11 迁移要点**:
- JavaFX从JDK 11开始不再内置，需单独引入依赖
- 需要使用模块系统(jigsaw)或配置自动模块
- 部分内部API可能被移除或限制访问

**JavaFX 8 → JavaFX 11/17 迁移要点**:
- 应用启动方式变化：需使用`Application.launch()`或模块化启动
- CSS样式可能需要调整以适应新版本渲染引擎
- 部分控件API有变化，需检查兼容性

**Gradle 4.5.1 → 7.6+ 迁移要点**:
- `compile`配置已废弃，需改用`implementation`
- `build.gradle`语法变化，部分API已移除
- 插件版本需要更新

> **详细迁移计划**: 参见 `ecn/doc/java11_upgrade.md`

---

## 1. 概述

### 1.1 应用架构

E-Chart应用采用JavaFX技术栈，基于BorderPane布局框架，实现了现代化的Ribbon风格界面。应用采用分层架构设计：

```
┌─────────────────────────────────────────────────────────────┐
│                        TitleBar                              │
│  [Logo] [登录] [设置] [搜索框] [左侧栏切换] [右侧栏切换] [窗口控制] │
├─────────────────────────────────────────────────────────────┤
│                     RibbonMenuBar                            │
│  [文件] [视图] [海图] [航线] [预警] [工具]                       │
├──────────┬──────────────────────────────────┬───────────────┤
│          │                                  │               │
│SideBar   │      ChartDisplayArea            │  RightTab     │
│Manager   │      (海图显示区)                  │  Manager      │
│          │                                  │               │
│ [图层]   │    ┌─────────────────┐           │  [预警]       │
│ [搜索]   │    │                 │           │  [日志]       │
│ [航线]   │    │   Canvas分层    │           │  [属性]       │
│ [预警]   │    │   渲染架构      │           │  [终端]       │
│ [AIS]    │    │                 │           │               │
│ [测量]   │    └─────────────────┘           │               │
│ [设置]   │                                  │               │
│          │                                  │               │
├──────────┴──────────────────────────────────┴───────────────┤
│                       StatusBar                              │
│  [坐标] [缩放] [比例尺] [预警数量] [图层数量] [时间]             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 核心组件

| 组件 | 类名 | 位置 | 功能 |
|------|------|------|------|
| 主视图 | MainView | BorderPane | 应用主布局框架 |
| 标题栏 | TitleBar | Top (StackPane层) | 应用标题、用户操作、窗口控制 |
| Ribbon菜单栏 | RibbonMenuBar | Top | 功能菜单和工具按钮 |
| 侧边栏管理器 | SideBarManager | Left | 左侧面板容器和切换 |
| 海图显示区 | ChartDisplayArea | Center | 海图渲染和交互 |
| 右侧标签页管理器 | RightTabManager | Right | 右侧面板容器和切换 |
| 状态栏 | StatusBar | Bottom | 状态信息和提示 |
| 活动栏 | ActivityBar | 预留（未使用） | 预留的快捷活动切换组件 |

> **注意**: ActivityBar是预留组件，在MainView中已创建但未添加到布局中，可根据需要启用。

---

## 2. 主视图布局 (MainView)

### 2.1 布局结构

**文件**: `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/MainView.java`

```java
public class MainView extends BorderPane implements LifecycleComponent {
    private final StackPane rootContainer;      // 根容器（用于叠加TitleBar）
    private final BorderPane contentPane;       // 内容面板
    private final SplitPane centerSplitPane;    // 中央分割面板
    private final RibbonMenuBar ribbonMenuBar;  // Ribbon菜单栏
    private final SideBarManager sideBarManager; // 侧边栏管理器
    private final ChartDisplayArea chartDisplayArea; // 海图显示区
    private final RightTabManager rightTabManager;   // 右侧标签页管理器
    private final StatusBar statusBar;          // 状态栏
    private final ActivityBar activityBar;      // 活动栏（预留，未添加到布局）
}
```

### 2.2 布局层次

```
MainView (BorderPane)
└── rootContainer (StackPane)
    ├── titleBar (TitleBar) - StackPane.setAlignment(TOP_CENTER)
    └── contentPane (BorderPane)
        ├── top: ribbonMenuBar (RibbonMenuBar)
        ├── center: centerSplitPane (SplitPane, HORIZONTAL)
        │   ├── [0] sideBarManager (SideBarManager)
        │   ├── [1] chartDisplayArea (ChartDisplayArea)
        │   └── [2] rightTabManager (RightTabManager)
        └── bottom: statusBar (StatusBar)
```

### 2.3 初始化流程

```java
// 1. 创建组件
public MainView() {
    this.rootContainer = new StackPane();
    this.contentPane = new BorderPane();
    this.centerSplitPane = new SplitPane();
    this.ribbonMenuBar = new RibbonMenuBar();
    this.sideBarManager = new SideBarManager();
    this.chartDisplayArea = new ChartDisplayArea();
    this.rightTabManager = new RightTabManager();
    this.statusBar = new StatusBar();
    
    initializeLayout();      // 初始化布局
    initializePanels();      // 初始化面板
    setupResponsiveLayout(); // 设置响应式布局
}

// 2. 设置Stage后添加TitleBar
public void setStage(Stage stage) {
    this.stage = stage;
    titleBar = new TitleBar(stage);
    rootContainer.getChildren().add(titleBar);
    // ... 设置TitleBar回调
}

// 3. 初始化布局
private void initializeLayout() {
    centerSplitPane.setOrientation(Orientation.HORIZONTAL);
    centerSplitPane.getItems().addAll(sideBarManager, chartDisplayArea, rightTabManager);
    
    contentPane.setTop(ribbonMenuBar);
    contentPane.setCenter(centerSplitPane);
    contentPane.setBottom(statusBar);
    contentPane.setPadding(new Insets(32, 0, 0, 0)); // 为TitleBar留空间
    
    rootContainer.getChildren().add(contentPane);
    setCenter(rootContainer);
}
```

### 2.4 分割器位置管理

**初始位置**:
```java
// 延迟设置分割器位置
javafx.application.Platform.runLater(() -> {
    double splitWidth = centerSplitPane.getWidth();
    double leftRatio = 40.0 / splitWidth;        // 左侧栏按钮宽度
    double rightRatio = 1 - rightTabManager.getPrefWidth() / splitWidth;
    centerSplitPane.setDividerPositions(leftRatio, Math.max(rightRatio, 0.5));
});
```

**动态调整**:
```java
// 监听分割器位置变化
centerSplitPane.getDividers().get(0).positionProperty().addListener((obs, oldVal, newVal) -> {
    double splitWidth = centerSplitPane.getWidth();
    double actualWidth = newVal.doubleValue() * splitWidth;
    double contentWidth = actualWidth - 40; // 减去按钮栏宽度
    
    // 当内容宽度小于阈值时自动折叠
    if (contentWidth < COLLAPSE_THRESHOLD) {
        sideBarManager.collapsePanel();
    } else {
        // 否则展开并显示面板
        sideBarManager.showPanel(firstPanelId, contentWidth, true);
    }
});
```

---

## 3. 标题栏 (TitleBar)

### 3.1 布局结构

**文件**: `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/TitleBar.java`

```
TitleBar (HBox, 高度32px)
├── leftSection (HBox)
│   ├── logoView (ImageView, 20x20)
│   ├── loginButton (Button, "👤")
│   └── settingsButton (Button, "⚙")
├── spacerLeft (HBox, HGrow=ALWAYS)
├── centerSection (HBox)
│   └── titleLabel (Label, "E-Chart Display and Alarm Application")
├── spacerRight (HBox, HGrow=ALWAYS)
└── rightSection (HBox)
    ├── searchField (TextField, 宽度150px)
    ├── toggleLeftBarButton (Button, "☰")
    ├── toggleRightBarButton (Button, "⊳")
    └── windowControls (WindowControls)
        ├── minimizeButton
        ├── maximizeButton
        └── closeButton
```

### 3.2 功能按钮

| 按钮 | 图标 | 功能 | 回调 |
|------|------|------|------|
| 登录 | 👤 | 用户登录 | onLoginAction |
| 设置 | ⚙ | 打开设置对话框 | onSettingsAction |
| 搜索框 | - | 输入搜索关键词 | onSearch |
| 左侧栏切换 | ☰ | 显示/隐藏左侧栏 | onToggleLeftBar |
| 右侧栏切换 | ⊳ | 显示/隐藏右侧栏 | onToggleRightBar |

### 3.3 窗口拖拽

```java
private void setupDragHandler() {
    setOnMousePressed(event -> {
        xOffset = event.getSceneX();
        yOffset = event.getSceneY();
        dragging = true;
    });
    
    setOnMouseDragged(event -> {
        if (dragging && !stage.isMaximized()) {
            stage.setX(event.getScreenX() - xOffset);
            stage.setY(event.getScreenY() - yOffset);
        }
    });
    
    setOnMouseReleased(event -> {
        dragging = false;
    });
}
```

### 3.4 窗口控制按钮组 (WindowControls)

**文件**: `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/WindowControls.java`

WindowControls是窗口控制按钮组，提供最小化、最大化/还原、关闭按钮。

```
WindowControls (HBox)
├── minimizeButton (Button, "─")  // 最小化
├── maximizeButton (Button, "□")  // 最大化/还原
└── closeButton (Button, "✕")     // 关闭
```

#### 3.4.1 最小化功能

```java
private void onMinimize() {
    if (stage != null) {
        stage.setIconified(true);  // 设置窗口为最小化状态
    }
}
```

#### 3.4.2 最大化/还原功能

```java
private void onMaximize() {
    if (stage == null) return;
    
    if (isMaximized) {
        // 还原窗口
        stage.setX(prevX);
        stage.setY(prevY);
        stage.setWidth(prevWidth);
        stage.setHeight(prevHeight);
        isMaximized = false;
    } else {
        // 保存当前窗口状态
        prevX = stage.getX();
        prevY = stage.getY();
        prevWidth = stage.getWidth();
        prevHeight = stage.getHeight();
        
        // 最大化到屏幕可视区域
        Rectangle2D screenBounds = Screen.getPrimary().getVisualBounds();
        stage.setX(screenBounds.getMinX());
        stage.setY(screenBounds.getMinY());
        stage.setWidth(screenBounds.getWidth());
        stage.setHeight(screenBounds.getHeight());
        isMaximized = true;
    }
    
    updateMaximizeButton();  // 更新按钮图标和提示
}
```

#### 3.4.3 关闭功能

```java
private void onClose() {
    if (stage != null) {
        stage.close();  // 触发窗口关闭事件
    }
}
```

> **注意**: `stage.close()` 会触发 `setOnCloseRequest` 事件，在EChartApp中该事件被消费并调用 `shutdown()` 方法执行完整的关闭流程。

### 3.5 窗口边缘调整大小 (WindowResizer)

**文件**: `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/WindowResizer.java`

由于应用使用 `UNDECORATED` 样式（无边框窗口），需要自定义窗口边缘拖动调整大小的功能。

#### 3.5.1 调整方向

```java
private enum ResizeDirection {
    NONE,           // 无调整
    N, S, E, W,     // 四个边
    NE, NW, SE, SW  // 四个角
}
```

#### 3.5.2 边缘检测

```java
private static final int RESIZE_MARGIN = 8;  // 边缘检测区域宽度

private ResizeDirection getResizeDirection(MouseEvent event) {
    double relX = event.getScreenX() - stage.getX();
    double relY = event.getScreenY() - stage.getY();
    
    boolean onNorth = relY >= 0 && relY < RESIZE_MARGIN;
    boolean onSouth = relY > stage.getHeight() - RESIZE_MARGIN;
    boolean onWest = relX >= 0 && relX < RESIZE_MARGIN;
    boolean onEast = relX > stage.getWidth() - RESIZE_MARGIN;
    
    // 返回对应的调整方向
    if (onNorth && onEast) return ResizeDirection.NE;
    // ... 其他方向判断
}
```

#### 3.5.3 调整大小逻辑

```java
EventHandler<MouseEvent> dragHandler = event -> {
    if (!resizing) return;
    
    double deltaX = event.getScreenX() - startX;
    double deltaY = event.getScreenY() - startY;
    
    // 根据方向计算新的窗口位置和大小
    switch (direction) {
        case E:
            newWidth = Math.max(minWidth, startWidth + deltaX);
            break;
        case W:
            newWidth = Math.max(minWidth, startWidth - deltaX);
            newX = startWindowX + startWidth - newWidth;
            break;
        // ... 其他方向处理
    }
    
    stage.setX(newX);
    stage.setY(newY);
    stage.setWidth(newWidth);
    stage.setHeight(newHeight);
};
```

#### 3.5.4 光标变化

```java
private void updateCursor(MouseEvent event) {
    ResizeDirection dir = getResizeDirection(event);
    
    Cursor cursor;
    switch (dir) {
        case N: case S: cursor = Cursor.V_RESIZE; break;
        case E: case W: cursor = Cursor.H_RESIZE; break;
        case NE: case SW: cursor = Cursor.NE_RESIZE; break;
        case NW: case SE: cursor = Cursor.NW_RESIZE; break;
        default: cursor = Cursor.DEFAULT;
    }
    
    scene.setCursor(cursor);
}
```

---

## 4. Ribbon菜单栏 (RibbonMenuBar)

### 4.1 布局结构

**文件**: `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/RibbonMenuBar.java`

```
RibbonMenuBar (extends Ribbon)
├── fileTab (RibbonTab, "文件")
│   ├── workspaceGroup (RibbonGroup, "工作区")
│   │   ├── 新建按钮
│   │   ├── 打开按钮
│   │   └── 保存按钮
│   └── exportGroup (RibbonGroup, "导出")
│       ├── 导出按钮
│       └── 打印按钮
├── viewTab (RibbonTab, "视图")
│   ├── zoomGroup (RibbonGroup, "缩放")
│   │   ├── 放大按钮
│   │   ├── 缩小按钮
│   │   └── 适应按钮
│   └── panelGroup (RibbonGroup, "面板")
│       ├── 侧边栏切换按钮 (ToggleButton)
│       ├── 右侧面板切换按钮 (ToggleButton)
│       └── 状态栏切换按钮 (ToggleButton)
├── chartTab (RibbonTab, "海图")
│   ├── chartGroup (RibbonGroup, "海图操作")
│   │   ├── 加载按钮
│   │   ├── 卸载按钮
│   │   └── 图层按钮
│   └── queryGroup (RibbonGroup, "查询")
│       ├── 属性按钮
│       └── 搜索按钮
├── routeTab (RibbonTab, "航线")
│   ├── routeGroup (RibbonGroup, "航线操作")
│   ├── ioGroup (RibbonGroup, "导入导出")
│   └── checkGroup (RibbonGroup, "检查")
├── alarmTab (RibbonTab, "预警")
│   ├── configGroup (RibbonGroup, "配置")
│   ├── historyGroup (RibbonGroup, "历史")
│   └── testGroup (RibbonGroup, "测试")
└── toolsTab (RibbonTab, "工具")
    ├── measureGroup (RibbonGroup, "测量")
    └── optionsGroup (RibbonGroup, "选项")
```

### 4.2 按钮创建

```java
private Button createButton(String text, String tooltip, String iconFile, Runnable action) {
    Button button = new Button(text);
    button.setTooltip(new Tooltip(tooltip));
    
    ImageView icon = loadIcon(iconFile);
    if (icon != null) {
        button.setGraphic(icon);  // 图标尺寸28x28
    }
    
    button.setOnAction(e -> {
        if (action != null) {
            action.run();
        }
    });
    
    return button;
}
```

### 4.3 动作监听器

```java
public interface RibbonActionListener {
    // 文件操作
    void onNewWorkspace();
    void onOpenWorkspace();
    void onSaveWorkspace();
    void onExport();
    void onPrint();
    
    // 视图操作
    void onZoomIn();
    void onZoomOut();
    void onFitToWindow();
    void onToggleSideBar();
    void onToggleRightTab();
    void onToggleStatusBar();
    
    // 海图操作
    void onLoadChart();
    void onUnloadChart();
    void onLayerManager();
    void onPropertyQuery();
    void onFeatureSearch();
    
    // 航线操作
    void onCreateRoute();
    void onEditRoute();
    void onDeleteRoute();
    void onImportRoute();
    void onExportRoute();
    void onCheckRoute();
    
    // 预警操作
    void onAlarmSettings();
    void onAlarmRules();
    void onAlarmHistory();
    void onAlarmStatistics();
    void onAlarmTest();
    
    // 工具操作
    void onMeasureDistance();
    void onMeasureArea();
    void onMeasureBearing();
    void onOptions();
    void onThemeSettings();
}
```

---

## 5. 侧边栏管理器 (SideBarManager)

### 5.1 布局结构

**文件**: `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/SideBarManager.java`

```
SideBarManager (HBox)
├── buttonBar (VBox, 宽度40px)
│   ├── ToggleButton (图层, 36x36)
│   ├── ToggleButton (搜索, 36x36)
│   ├── ToggleButton (航线, 36x36)
│   ├── ToggleButton (预警, 36x36)
│   ├── ToggleButton (AIS, 36x36)
│   ├── ToggleButton (测量, 36x36)
│   └── ToggleButton (设置, 36x36)
└── contentPane (StackPane, 宽度动态)
    ├── LayerManagerPanel (图层管理面板)
    ├── SearchPanel (搜索面板)
    ├── RoutePanel (航线面板)
    ├── AlarmPanel (预警面板)
    ├── AISPanel (AIS面板)
    ├── MeasurePanel (测量面板)
    └── SettingsPanel (设置面板)
```

### 5.2 面板注册

```java
// 初始化面板
private void initializePanels() {
    // 图层管理面板
    LayerManagerPanel layerManagerPanel = new LayerManagerPanel();
    SideBarPanel layersPanel = new SideBarPanel(
        "layers", "图层管理", "/icons/layers.png", "图层管理面板",
        layerManagerPanel.getContent());
    
    // 搜索面板
    SearchPanel searchPanel = new SearchPanel();
    SideBarPanel searchSideBarPanel = new SideBarPanel(
        "search", "搜索", "/icons/search.png", "搜索面板",
        searchPanel.getContent());
    
    // ... 其他面板
    
    // 注册面板（按顺序）
    sideBarManager.registerPanel(layersPanel, 0);
    sideBarManager.registerPanel(searchSideBarPanel, 1);
    sideBarManager.registerPanel(routePanel, 2);
    sideBarManager.registerPanel(alarmPanel, 3);
    sideBarManager.registerPanel(aisPanel, 4);
    sideBarManager.registerPanel(measureSideBarPanel, 5);
    sideBarManager.registerPanel(settingsSideBarPanel, 6);
}
```

### 5.3 面板切换逻辑

```java
// 显示面板
public void showPanel(String panelId) {
    SideBarPanel panel = panels.get(panelId);
    
    // 1. 隐藏当前活动面板
    if (activePanel != null && activePanel != panel) {
        activePanel.getContent().setVisible(false);
        activePanel.getContent().setManaged(false);
        activePanel.onDeactivate();
    }
    
    // 2. 激活新面板
    activePanel = panel;
    panel.getContent().setVisible(true);
    panel.getContent().setManaged(true);
    panel.getContent().toFront();
    panel.onActivate();
    
    // 3. 展开侧边栏
    if (!isExpanded) {
        expandPanel();
    }
    
    // 4. 选中对应按钮
    ToggleButton button = buttons.get(panelId);
    if (button != null) {
        button.setSelected(true);
    }
}

// 折叠面板
public void collapsePanel() {
    isExpanded = false;
    contentPane.setVisible(false);
    contentPane.setManaged(false);
    contentPane.setPrefWidth(0);
    setPrefWidth(40);  // 只保留按钮栏
    
    if (activePanel != null) {
        activePanel.onDeactivate();
        activePanel = null;
    }
    
    toggleGroup.selectToggle(null);
}

// 展开面板
public void expandPanel() {
    isExpanded = true;
    contentPane.setVisible(true);
    contentPane.setManaged(true);
    contentPane.setPrefWidth(expandedWidth);  // 默认280px
    setPrefWidth(40 + expandedWidth);
}
```

### 5.4 按钮点击行为

```java
button.setOnAction(e -> {
    // 如果点击的是当前活动面板，则折叠
    if (isExpanded && activePanel != null && activePanel.getId().equals(panel.getId())) {
        collapsePanel();
    } else {
        // 否则显示该面板
        showPanel(panel.getId());
        button.setSelected(true);
    }
});
```

---

## 6. 图层管理面板 (LayerManagerPanel)

### 6.1 布局结构

**文件**: `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/panel/LayerManagerPanel.java`

**FXML**: `ecn/echart-ui/src/main/resources/fxml/LayerManagerPanel.fxml`

```
LayerManagerPanel (VBox)
├── panelHeader (StackPane, 高度36px)
│   ├── HBox (CENTER_LEFT)
│   │   └── Label ("图层管理")
│   └── HBox (CENTER_RIGHT)
│       ├── configButton (配置按钮)
│       └── hideButton (隐藏按钮)
├── toolbar-flat (HBox, 高度28px)
│   ├── moveUpButton (上移)
│   ├── moveDownButton (下移)
│   ├── expandAllButton (展开所有)
│   └── collapseAllButton (折叠所有)
└── layerTreeView (TreeView, VBox.vgrow=ALWAYS)
    └── rootNode (TreeItem)
        ├── 海图数据 (Category)
        │   └── [海图图层节点...]
        ├── 航线数据 (Category)
        │   └── [航线图层节点...]
        ├── AIS数据 (Category)
        │   └── [AIS图层节点...]
        ├── 预警区域 (Category)
        │   └── [预警图层节点...]
        └── 叠加图层 (Category)
            └── [叠加图层节点...]
```

### 6.2 树节点结构

```java
public enum LayerNodeType {
    ROOT(false),              // 根节点
    CATEGORY_CHART(true),     // 海图数据分类
    CATEGORY_ROUTE(true),     // 航线数据分类
    CATEGORY_AIS(true),       // AIS数据分类
    CATEGORY_ALARM(true),     // 预警区域分类
    CATEGORY_OVERLAY(true),   // 叠加图层分类
    DATA(false),              // 普通数据节点
    DATA_CHART(false),        // 海图数据节点
    DATA_ROUTE(false),        // 航线数据节点
    DATA_AIS(false),          // AIS数据节点
    DATA_ALARM(false);        // 预警数据节点
}
```

### 6.3 树单元格渲染

```java
private static class LayerTreeCell extends TreeCell<LayerNode> {
    private final HBox content;
    private final CheckBox checkBox;    // 可见性复选框
    private final Label label;          // 名称标签
    private final ImageView iconView;   // 图标
    
    @Override
    protected void updateItem(LayerNode item, boolean empty) {
        super.updateItem(item, empty);
        
        if (empty || item == null) {
            setGraphic(null);
        } else {
            label.setText(item.getName());
            checkBox.setSelected(item.isVisible());
            
            // 可见性切换
            checkBox.selectedProperty().addListener((obs, oldVal, newVal) -> {
                item.setVisible(newVal);
            });
            
            // 更新图标
            updateIcon(item.getType());
            
            setGraphic(content);
        }
    }
}
```

### 6.4 右键菜单

```xml
<ContextMenu fx:id="treeContextMenu">
    <items>
        <MenuItem text="显示/隐藏" onAction="#onToggleVisibility"/>
        <MenuItem text="重命名" onAction="#onRename"/>
        <SeparatorMenuItem/>
        <MenuItem text="定位到图层" onAction="#onLocateLayer"/>
        <MenuItem text="查看属性" onAction="#onViewProperties"/>
        <SeparatorMenuItem/>
        <MenuItem text="删除" onAction="#onDeleteLayer"/>
    </items>
</ContextMenu>
```

### 6.5 回调设置

```java
// 设置回调
public void setOnViewProperties(Consumer<LayerNode> callback) {
    this.onViewProperties = callback;
}

// 在MainView中设置回调
layerManagerPanel.setOnViewProperties(layerNode -> {
    // 1. 清空属性面板
    propertyPanel.clear();
    
    // 2. 添加通用属性
    propertyPanel.addCommonProperty("名称", layerNode.getName());
    propertyPanel.addCommonProperty("ID", layerNode.getId());
    propertyPanel.addCommonProperty("类型", layerNode.getType().toString());
    propertyPanel.addCommonProperty("可见性", layerNode.isVisible() ? "可见" : "隐藏");
    
    // 3. 添加数据属性
    Map<String, String> dataProps = layerNode.getProperties();
    for (Map.Entry<String, String> entry : dataProps.entrySet()) {
        propertyPanel.addDataProperty(entry.getKey(), entry.getValue());
    }
    
    // 4. 切换到属性面板
    rightTabManager.selectPanel("property-panel");
    
    // 5. 如果右侧面板不在分割面板中，则显示
    if (!centerSplitPane.getItems().contains(rightTabManager)) {
        viewHandler.showRightTab();
    }
});
```

---

## 7. 海图显示区 (ChartDisplayArea)

### 7.1 布局结构

**文件**: `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/ChartDisplayArea.java`

```
ChartDisplayArea (StackPane)
├── layer-0 (Canvas, 背景层)
├── layer-1 (Canvas, 海图层)
├── layer-2 (Canvas, 数据层)
├── layer-3 (Canvas, 交互层)
├── layer-4 (Canvas, 覆盖层)
└── zoomControls (VBox, BOTTOM_RIGHT)
    ├── zoomInButton (Button, "+")
    └── zoomOutButton (Button, "-")
```

### 7.2 分层渲染架构

```java
public enum LayerType {
    BACKGROUND(0),   // 背景层 - 海底地形、水深等背景信息
    CHART(1),        // 海图层 - 海图要素、航道、锚地等
    DATA(2),         // 数据层 - AIS目标、航线、标注等动态数据
    INTERACTION(3),  // 交互层 - 测量工具、选择框、编辑器等交互元素
    OVERLAY(4);      // 覆盖层 - 提示信息、临时标注等
    
    private final int index;
}
```

### 7.3 视口管理

```java
public class ChartDisplayArea {
    private double zoomLevel;    // 缩放级别 (0.1 - 10.0)
    private double centerX;      // 中心点X坐标
    private double centerY;      // 中心点Y坐标
    private double rotation;     // 旋转角度
    
    // 世界坐标边界
    private double worldMinX;
    private double worldMaxX;
    private double worldMinY;
    private double worldMaxY;
}
```

### 7.4 坐标转换

```java
// 屏幕坐标转世界坐标
public double screenToWorldX(double screenX) {
    return (screenX - getWidth() / 2.0) / zoomLevel + centerX;
}

public double screenToWorldY(double screenY) {
    return (screenY - getHeight() / 2.0) / zoomLevel + centerY;
}

// 世界坐标转屏幕坐标
public double worldToScreenX(double worldX) {
    return (worldX - centerX) * zoomLevel + getWidth() / 2.0;
}

public double worldToScreenY(double worldY) {
    return (worldY - centerY) * zoomLevel + getHeight() / 2.0;
}
```

### 7.5 交互操作

**平移**:
```java
setOnMousePressed(event -> {
    if (event.getButton() == MouseButton.PRIMARY) {
        lastMouseX = event.getX();
        lastMouseY = event.getY();
        isPanning = true;
    }
});

setOnMouseDragged(event -> {
    if (isPanning && event.getButton() == MouseButton.PRIMARY) {
        double deltaX = event.getX() - lastMouseX;
        double deltaY = event.getY() - lastMouseY;
        
        pan(deltaX, deltaY);  // 平移
        
        lastMouseX = event.getX();
        lastMouseY = event.getY();
    }
});
```

**缩放**:
```java
setOnScroll(event -> {
    double delta = event.getDeltaY() > 0 ? 1.1 : 1.0 / 1.1;
    double newZoom = Math.max(0.1, Math.min(10.0, zoomLevel * delta));
    
    if (newZoom != zoomLevel) {
        // 以鼠标位置为中心缩放
        double mouseX = event.getX();
        double mouseY = event.getY();
        
        double worldXBefore = screenToWorldX(mouseX);
        double worldYBefore = screenToWorldY(mouseY);
        
        zoomLevel = newZoom;
        
        double worldXAfter = screenToWorldX(mouseX);
        double worldYAfter = screenToWorldY(mouseY);
        
        centerX += worldXBefore - worldXAfter;
        centerY += worldYBefore - worldYAfter;
        
        redraw();
    }
});
```

### 7.6 右键菜单

```java
contextMenu.getItems().addAll(
    zoomInItem,           // 放大
    zoomOutItem,          // 缩小
    fitWindowItem,        // 适应窗口
    resetViewItem,        // 重置视图
    new SeparatorMenuItem(),
    measureDistanceItem,  // 测量距离
    measureAreaItem,      // 测量面积
    measureBearingItem,   // 测量方位
    new SeparatorMenuItem(),
    propertiesItem        // 属性
);
```

---

## 8. 右侧标签页管理器 (RightTabManager)

### 8.1 布局结构

**文件**: `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/RightTabManager.java`

```
RightTabManager (TabPane, 宽度300px)
├── 预警标签页 (Tab, "预警")
│   └── AlarmPanel
├── 日志标签页 (Tab, "日志")
│   └── LogPanel
├── 属性标签页 (Tab, "属性", 不可关闭)
│   └── PropertyPanel
└── 终端标签页 (Tab, "终端")
    └── TerminalPanel
```

### 8.2 面板注册

```java
// 初始化右侧面板
PropertyPanel propertyPanel = new PropertyPanel();
rightTabManager.registerPanel(new AlarmPanel());
rightTabManager.registerPanel(new LogPanel());
rightTabManager.registerPanel(propertyPanel);
rightTabManager.registerPanel(new TerminalPanel());
```

### 8.3 标签页切换

```java
getSelectionModel().selectedItemProperty().addListener((obs, oldTab, newTab) -> {
    // 旧标签页取消选中
    if (oldTab != null) {
        FxRightTabPanel oldPanel = findPanelByTab(oldTab);
        if (oldPanel != null) {
            oldPanel.onDeselected();
        }
    }
    
    // 新标签页选中
    if (newTab != null) {
        FxRightTabPanel newPanel = findPanelByTab(newTab);
        if (newPanel != null) {
            newPanel.onSelected();
            selectedPanel = newPanel;
        }
    }
});
```

---

## 9. 属性面板 (PropertyPanel)

### 9.1 布局结构

**文件**: `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/panel/PropertyPanel.java`

```
PropertyPanel (Tab内容)
├── panelHeader (StackPane)
│   ├── Label ("属性")
│   └── pinButton (固定按钮)
├── objectNameLabel (Label, 对象名称)
├── commonPropertiesSection (TitledPane, "通用属性")
│   └── TableView
│       ├── 名称列
│       └── 值列
└── dataPropertiesSection (TitledPane, "数据属性")
    └── TableView
        ├── 名称列
        └── 值列
```

### 9.2 属性添加

```java
// 添加通用属性
public void addCommonProperty(String name, String value) {
    // 添加到通用属性表格
}

// 添加数据属性
public void addDataProperty(String name, String value) {
    // 添加到数据属性表格
}

// 清空所有属性
public void clear() {
    // 清空对象名称
    // 清空通用属性表格
    // 清空数据属性表格
}
```

---

## 10. 状态栏 (StatusBar)

### 10.1 布局结构

**文件**: `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/StatusBar.java`

```
StatusBar (extends ControlsFX StatusBar)
├── leftItems
│   ├── coordinatesLabel ("经度: -- 纬度: --")
│   ├── Separator
│   ├── zoomLabel ("缩放: 100%")
│   ├── Separator
│   └── scaleLabel ("比例尺: 1:10000")
└── rightItems
    ├── alarmCountLabel ("预警: 0")
    ├── Separator
    ├── layerCountLabel ("图层: 0")
    ├── Separator
    └── timeLabel ("2026-04-24 12:00:00")
```

### 10.2 状态更新

```java
// 设置坐标
public void setCoordinates(double longitude, double latitude) {
    coordinatesLabel.setText(String.format("经度: %.6f 纬度: %.6f", longitude, latitude));
}

// 设置缩放
public void setZoom(double zoom) {
    zoomLabel.setText(String.format("缩放: %.0f%%", zoom * 100));
}

// 设置比例尺
public void setScale(double scale) {
    scaleLabel.setText(String.format("比例尺: 1:%.0f", scale));
}

// 更新预警数量
public void setAlarmCount(int count) {
    this.alarmCount = count;
    // 更新显示，数量>0时显示红色
}

// 更新图层数量
public void setLayerCount(int count) {
    this.layerCount = count;
    // 更新显示
}
```

### 10.3 时钟

```java
private void startClock() {
    updateTime();
    Timeline timeline = new Timeline(
        new KeyFrame(Duration.seconds(1), event -> updateTime())
    );
    timeline.setCycleCount(Animation.INDEFINITE);
    timeline.play();
}
```

---

## 11. 响应式布局

### 11.1 布局模式

**文件**: `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/ResponsiveLayoutManager.java`

| 模式 | 宽度范围 | 侧边栏宽度 | 右侧面板宽度 | 海图列数 |
|------|----------|------------|--------------|----------|
| 紧凑模式 | < 1024px | 200px | 0 (隐藏) | 1 |
| 标准模式 | 1024-1440px | 280px | 250px | 1 |
| 宽屏模式 | > 1440px | 320px | 300px | 2 |

### 11.2 模式切换

```java
public void monitor(Region region) {
    region.widthProperty().addListener((obs, oldVal, newVal) -> {
        if (enabled) {
            updateLayoutMode(newVal.doubleValue());
        }
    });
}

private void updateLayoutMode(double width) {
    LayoutMode newMode = determineLayoutMode(width);
    LayoutMode currentMode = currentModeProperty.get();
    
    if (newMode != currentMode) {
        currentModeProperty.set(newMode);
        notifyListeners(currentMode, newMode);
    }
}
```

---

## 12. 事件处理流程

### 12.1 Handler架构

```
MainView
├── fileHandler (FileHandler)
│   ├── onNewWorkspace()
│   ├── onOpenWorkspace()
│   ├── onSaveWorkspace()
│   ├── onExport()
│   └── onPrint()
├── viewHandler (ViewHandler)
│   ├── onZoomIn()
│   ├── onZoomOut()
│   ├── onFitToWindow()
│   ├── onToggleSideBar()
│   ├── onToggleRightTab()
│   └── onToggleStatusBar()
├── chartHandler (ChartHandler)
│   ├── onLoadChart()
│   ├── onUnloadChart()
│   ├── onLayerManager()
│   ├── onPropertyQuery()
│   └── onFeatureSearch()
├── routeHandler (RouteHandler)
│   ├── onCreateRoute()
│   ├── onEditRoute()
│   ├── onDeleteRoute()
│   ├── onImportRoute()
│   ├── onExportRoute()
│   └── onCheckRoute()
├── alarmHandler (AlarmHandler)
│   ├── onAlarmSettings()
│   ├── onAlarmRules()
│   ├── onAlarmHistory()
│   ├── onAlarmStatistics()
│   └── onAlarmTest()
├── measureHandler (MeasureHandler)
│   ├── onMeasureDistance()
│   ├── onMeasureArea()
│   └── onMeasureBearing()
└── toolsHandler (ToolsHandler)
    ├── onOptions()
    └── onThemeSettings()
```

### 12.2 Ribbon动作绑定

```java
ribbonMenuBar.setActionListener(new RibbonMenuBar.RibbonActionListener() {
    @Override
    public void onZoomIn() {
        viewHandler.onZoomIn();
    }
    
    @Override
    public void onToggleSideBar() {
        viewHandler.onToggleSideBar();
    }
    
    @Override
    public void onLoadChart() {
        chartHandler.onLoadChart();
    }
    
    // ... 其他动作
});
```

---

## 13. 初始化时界面状态

### 13.1 应用启动流程

```java
// 1. EChartApp.init()
initializePlatformAdapter();  // 注册平台适配器
initializeFacade();           // 初始化应用门面
initializeThemeManager();     // 初始化主题管理器
initializeI18nManager();      // 初始化国际化管理器

// 2. EChartApp.start()
facade.start();               // 启动应用门面
mainView = new MainView();    // 创建主视图
mainView.setStage(primaryStage);
mainView.setThemeManager(themeManager);

Scene scene = createScene();  // 创建场景
applyTheme(scene);            // 应用主题
mainView.enableWindowResize(scene);
configureStage(primaryStage, scene);

mainView.initialize();        // 初始化主视图
mainView.activate();          // 激活主视图

primaryStage.show();          // 显示窗口
```

### 13.2 初始界面状态

**常量说明**:
- `SIDEBAR_BUTTON_WIDTH = 40px` (侧边栏按钮栏宽度)
- `SIDEBAR_CONTENT_WIDTH = 280px` (侧边栏内容面板宽度，可调整)
- `RIGHT_PANEL_WIDTH = RightTabManager.getPrefWidth()` (右侧面板宽度，默认300px，可调整)

| 组件 | 初始状态 |
|------|----------|
| TitleBar | 显示，高度32px |
| RibbonMenuBar | 显示，默认选中"文件"标签页 |
| SideBarManager | 折叠状态，只显示按钮栏(SIDEBAR_BUTTON_WIDTH) |
| ChartDisplayArea | 显示，空白状态，宽度自适应 |
| RightTabManager | 显示，宽度=RIGHT_PANEL_WIDTH，默认选中"属性"标签页 |
| StatusBar | 显示 |
| 分割器0位置 | ≈SIDEBAR_BUTTON_WIDTH/窗口宽度 |
| 分割器1位置 | ≈1-RIGHT_PANEL_WIDTH/窗口宽度 |

**初始布局示意**:
```
┌─────────────────────────────────────────────────────────────┐
│ TitleBar (32px)                                             │
├─────────────────────────────────────────────────────────────┤
│ RibbonMenuBar                                               │
├────┬─────────────────────────────────────┬─────────────────┤
│    │                                     │                 │
│按钮│                                     │  RightTabManager│
│栏  │      ChartDisplayArea               │  (RIGHT_PANEL_WIDTH)     │
│40px│        (自适应宽度)                  │                 │
│    │                                     │                 │
├────┴─────────────────────────────────────┴─────────────────┤
│ StatusBar                                                   │
└─────────────────────────────────────────────────────────────┘
```

### 13.3 侧边栏初始状态

```
初始状态：折叠
┌────┐
│ 📊 │  图层按钮 (未选中)
│ 🔍 │  搜索按钮 (未选中)
│ 🛳 │  航线按钮 (未选中)
│ ⚠️ │  预警按钮 (未选中)
│ 📡 │  AIS按钮 (未选中)
│ 📏 │  测量按钮 (未选中)
│ ⚙️ │  设置按钮 (未选中)
└────┘
```

### 13.4 窗口状态管理

#### 13.4.1 窗口样式配置

```java
// EChartApp.configureStage()
stage.initStyle(javafx.stage.StageStyle.UNDECORATED);  // 无边框窗口
stage.setMinWidth(800);   // 最小宽度
stage.setMinHeight(600);  // 最小高度
```

#### 13.4.2 窗口最小化处理

```java
// WindowControls.onMinimize()
stage.setIconified(true);  // 设置窗口为最小化状态

// 窗口最小化时JavaFX会自动处理：
// - 隐藏窗口内容
// - 在任务栏显示窗口图标
// - 恢复时恢复窗口状态
```

#### 13.4.3 窗口最大化/还原处理

```java
// WindowControls.onMaximize()
if (isMaximized) {
    // 还原：恢复之前保存的窗口位置和大小
    stage.setX(prevX);
    stage.setY(prevY);
    stage.setWidth(prevWidth);
    stage.setHeight(prevHeight);
    isMaximized = false;
} else {
    // 最大化：保存当前状态，然后最大化到屏幕可视区域
    prevX = stage.getX();
    prevY = stage.getY();
    prevWidth = stage.getWidth();
    prevHeight = stage.getHeight();
    
    Rectangle2D screenBounds = Screen.getPrimary().getVisualBounds();
    stage.setX(screenBounds.getMinX());
    stage.setY(screenBounds.getMinY());
    stage.setWidth(screenBounds.getWidth());
    stage.setHeight(screenBounds.getHeight());
    isMaximized = true;
}
```

#### 13.4.4 窗口关闭事件处理

```java
// EChartApp.configureStage()
stage.setOnCloseRequest(event -> {
    event.consume();  // 消费事件，阻止默认关闭行为
    shutdown();       // 执行完整的关闭流程
});
```

**关闭流程说明**:
1. 用户点击关闭按钮 → WindowControls.onClose() → stage.close()
2. stage.close() 触发 setOnCloseRequest 事件
3. 事件被消费，阻止窗口直接关闭
4. 调用 shutdown() 执行完整的资源释放流程
5. 最后调用 Platform.exit() 和 System.exit(0) 退出应用

### 13.5 界面销毁流程

#### 13.5.1 应用关闭流程

```
用户点击窗口关闭按钮
    ↓
EChartApp.stop() 被调用
    ↓
shutdown() 执行:
├── facade.stop()      // 停止应用门面
├── facade.destroy()   // 销毁应用门面
├── ServiceLocator.clear()  // 清除服务定位器
├── primaryStage.close()    // 关闭主窗口
├── Platform.exit()   // 退出JavaFX平台
└── System.exit(0)    // 退出JVM
```

#### 13.5.2 MainView销毁流程

```java
@Override
public void dispose() {
    if (disposed) {
        return;
    }
    
    // 1. 先停用组件
    deactivate();
    
    // 2. 销毁海图显示区
    chartDisplayArea.dispose();
    
    // 3. 销毁侧边栏所有面板
    sideBarManager.getPanels().values().forEach(panel -> {
        if (panel.getContent() instanceof LifecycleComponent) {
            ((LifecycleComponent) panel.getContent()).dispose();
        }
    });
    
    // 4. 销毁右侧标签页管理器
    if (rightTabManager instanceof LifecycleComponent) {
        ((LifecycleComponent) rightTabManager).dispose();
    }
    
    // 5. 停止响应式布局监听
    responsiveLayoutManager.stopMonitoring();
    
    disposed = true;
}
```

#### 13.5.3 资源释放顺序

| 步骤 | 操作 | 说明 |
|------|------|------|
| 1 | deactivate() | 停用所有组件，暂停渲染 |
| 2 | chartDisplayArea.dispose() | 释放Canvas资源、停止动画 |
| 3 | 侧边栏面板dispose() | 释放各面板资源、取消事件订阅 |
| 4 | rightTabManager.dispose() | 释放右侧面板资源 |
| 5 | responsiveLayoutManager.stopMonitoring() | 停止窗口大小监听 |

---

## 14. 交互流程示例

### 14.1 加载海图流程

```
用户点击Ribbon "海图" -> "加载"
    ↓
ChartHandler.onLoadChart()
    ↓
打开文件选择对话框 (FileChooser)
    ↓
用户选择海图文件 (*.000, *.s57)
    ↓
ChartHandler.loadChartFile(file)
    ↓
生成海图ID和名称
    ↓
LayerManagerPanel.addChartLayer(id, name, visible=true)
    ↓
在"海图数据"分类下添加图层节点
    ↓
SideBarManager.showPanel("layers")
    ↓
展开侧边栏，显示图层管理面板
    ↓
状态栏显示 "已加载海图: xxx"
```

### 14.2 查看图层属性流程

```
用户在图层管理面板中右键点击图层节点
    ↓
显示右键菜单
    ↓
用户点击 "查看属性"
    ↓
LayerManagerPanelController.onViewProperties()
    ↓
调用 onViewProperties 回调
    ↓
MainView.setupLayerPanelCallbacks() 中的回调执行:
    ├── PropertyPanel.clear()
    ├── PropertyPanel.addCommonProperty("名称", ...)
    ├── PropertyPanel.addCommonProperty("ID", ...)
    ├── PropertyPanel.addCommonProperty("类型", ...)
    ├── PropertyPanel.addCommonProperty("可见性", ...)
    ├── PropertyPanel.addDataProperty(...)
    ├── RightTabManager.selectPanel("property-panel")
    └── ViewHandler.showRightTab() (如果右侧面板不可见,!rightTabManager.isVisible())
```

### 14.3 切换侧边栏面板流程

```
用户点击侧边栏按钮 (如 "搜索")
    ↓
ToggleButton.setOnAction() 触发
    ↓
判断当前状态:
├── 如果当前是活动面板且已展开
│   └── SideBarManager.collapsePanel()
│       ├── contentPane.setVisible(false)
│       ├── contentPane.setManaged(false)
│       └── activePanel.onDeactivate()
└── 否则
    └── SideBarManager.showPanel("search")
        ├── 隐藏当前活动面板
        ├── 激活搜索面板
        ├── 展开侧边栏
        └── 选中搜索按钮
```

### 14.4 侧边栏展开/折叠时布局变动

#### 14.4.1 侧边栏展开时布局变化

当用户点击侧边栏按钮展开面板时，各组件布局发生以下变化：

| 组件 | 变化前 | 变化后 | 变化说明 |
|------|--------|--------|----------|
| **SideBarManager** | 宽度=SIDEBAR_BUTTON_WIDTH | 宽度=SIDEBAR_BUTTON_WIDTH+SIDEBAR_CONTENT_WIDTH | contentPane变为可见可管理 |
| **ChartDisplayArea** | 宽度=窗口-SIDEBAR_BUTTON_WIDTH-RIGHT_PANEL_WIDTH | 宽度=窗口-SIDEBAR_BUTTON_WIDTH-SIDEBAR_CONTENT_WIDTH-RIGHT_PANEL_WIDTH | 宽度减小SIDEBAR_CONTENT_WIDTH，内容自动缩放 |
| **RightTabManager** | 位置靠右 | 位置靠右 | 宽度不变，但左侧边界右移 |
| **TitleBar** | 无变化 | 无变化 | 不受侧边栏影响 |
| **RibbonMenuBar** | 无变化 | 无变化 | 不受侧边栏影响 |
| **StatusBar** | 无变化 | 无变化 | 不受侧边栏影响 |
| **缩放控件** | 右下角 | 右下角 | 位置跟随ChartDisplayArea |
| **分割器0** | ≈SIDEBAR_BUTTON_WIDTH/总宽度 | ≈(SIDEBAR_BUTTON_WIDTH+SIDEBAR_CONTENT_WIDTH)/总宽度 | 左侧分割器右移 |
| **分割器1** | ≈1-RIGHT_PANEL_WIDTH/总宽度 | ≈1-RIGHT_PANEL_WIDTH/总宽度 | 位置不变 |

**常量说明**:
- `SIDEBAR_BUTTON_WIDTH = 40px` (侧边栏按钮栏宽度)
- `SIDEBAR_CONTENT_WIDTH = 280px` (侧边栏内容面板宽度，可调整)
- `RIGHT_PANEL_WIDTH = RightTabManager.getPrefWidth()` (右侧面板宽度，默认300px，可调整)

**展开后布局示意**:
```
┌─────────────────────────────────────────────────────────────┐
│ TitleBar (32px)                                             │
├─────────────────────────────────────────────────────────────┤
│ RibbonMenuBar                                               │
├────┬──────────────────────┬─────────────────┬───────────────┤
│    │  contentPane         │                 │               │
│按钮│  (SIDEBAR_CONTENT_WIDTH)   │                 │ RightTabManager│
│栏  │  [图层面板内容]       │ ChartDisplayArea│  (RIGHT_PANEL_WIDTH)    │
│40px│                      │   (自适应宽度)   │               │
│    │                      │                 │               │
├────┴──────────────────────┴─────────────────┴───────────────┤
│ StatusBar                                                   │
└─────────────────────────────────────────────────────────────┘
```

**布局变化流程**:
```
用户点击侧边栏按钮
    ↓
SideBarManager.showPanel(panelId)
    ↓
├── contentPane.setVisible(true)
├── contentPane.setManaged(true)
├── contentPane.setPrefWidth(SIDEBAR_CONTENT_WIDTH)
├── setPrefWidth(SIDEBAR_BUTTON_WIDTH + SIDEBAR_CONTENT_WIDTH)
└── expandedProperty.set(true)
    ↓
expandedProperty变化触发MainView.setupSideBarListeners()
    ↓
updateDividerPositions()
    ↓
├── 计算新的分割器位置
├── centerSplitPane.setDividerPositions(leftRatio, rightRatio)
└── ChartDisplayArea宽度自动调整
    ↓
ChartDisplayArea.widthProperty变化
    ↓
├── Canvas层宽高自动绑定更新
├── preserveWorldBoundsOnResize() 保持世界坐标
└── redraw() 重绘所有层
```

#### 14.4.2 侧边栏折叠时布局变化

当用户再次点击活动面板按钮或拖动分割器折叠时：

| 组件 | 变化前 | 变化后 | 变化说明 |
|------|--------|--------|----------|
| **SideBarManager** | 宽度=SIDEBAR_BUTTON_WIDTH+SIDEBAR_CONTENT_WIDTH | 宽度=SIDEBAR_BUTTON_WIDTH | contentPane变为不可见不可管理 |
| **ChartDisplayArea** | 宽度=窗口-SIDEBAR_BUTTON_WIDTH-SIDEBAR_CONTENT_WIDTH-RIGHT_PANEL_WIDTH | 宽度=窗口-SIDEBAR_BUTTON_WIDTH-RIGHT_PANEL_WIDTH | 宽度增加SIDEBAR_CONTENT_WIDTH，内容自动缩放 |
| **RightTabManager** | 位置靠右 | 位置靠右 | 宽度不变，但左侧边界左移 |
| **TitleBar** | 无变化 | 无变化 | 不受侧边栏影响 |
| **RibbonMenuBar** | 无变化 | 无变化 | 不受侧边栏影响 |
| **StatusBar** | 无变化 | 无变化 | 不受侧边栏影响 |
| **缩放控件** | 右下角 | 右下角 | 位置跟随ChartDisplayArea |
| **分割器0** | ≈(SIDEBAR_BUTTON_WIDTH+SIDEBAR_CONTENT_WIDTH)/总宽度 | ≈SIDEBAR_BUTTON_WIDTH/总宽度 | 左侧分割器左移 |
| **分割器1** | ≈1-RIGHT_PANEL_WIDTH/总宽度 | ≈1-RIGHT_PANEL_WIDTH/总宽度 | 位置不变 |

**布局变化流程**:
```
用户点击活动面板按钮 或 拖动分割器到阈值以下
    ↓
SideBarManager.collapsePanel()
    ↓
├── contentPane.setVisible(false)
├── contentPane.setManaged(false)
├── contentPane.setPrefWidth(0)
├── setPrefWidth(SIDEBAR_BUTTON_WIDTH)
├── expandedProperty.set(false)
├── activePanel.onDeactivate()
└── activePanel = null
    ↓
expandedProperty变化触发MainView.setupSideBarListeners()
    ↓
updateDividerPositions()
    ↓
├── 计算新的分割器位置
├── centerSplitPane.setDividerPositions(leftRatio, rightRatio)
└── ChartDisplayArea宽度自动调整
    ↓
ChartDisplayArea.widthProperty变化
    ↓
├── Canvas层宽高自动绑定更新
├── preserveWorldBoundsOnResize() 保持世界坐标
└── redraw() 重绘所有层
```

#### 14.4.3 右侧面板显示/隐藏时布局变化

当用户点击标题栏右侧切换按钮或通过Ribbon菜单切换右侧面板时：

| 组件 | 显示右侧面板 | 隐藏右侧面板 |
|------|--------------|--------------|
| **SideBarManager** | 无变化 | 无变化 |
| **ChartDisplayArea** | 宽度减小 | 宽度增加 |
| **RightTabManager** | 添加到SplitPane | 从SplitPane移除 |
| **分割器1** | 显示 | 隐藏（只有一个分割器） |

**显示右侧面板布局示意**:
```
┌─────────────────────────────────────────────────────────────┐
│ TitleBar (32px)                                             │
├─────────────────────────────────────────────────────────────┤
│ RibbonMenuBar                                               │
├────┬─────────────────────────────────────┬─────────────────┤
│    │                                     │                 │
│按钮│      ChartDisplayArea               │ RightTabManager │
│栏  │        (自适应宽度)                  │  (RIGHT_PANEL_WIDTH)     │
│40px│                                     │                 │
│    │                                     │                 │
├────┴─────────────────────────────────────┴─────────────────┤
│ StatusBar                                                   │
└─────────────────────────────────────────────────────────────┘
```

**隐藏右侧面板布局示意**:
```
┌─────────────────────────────────────────────────────────────┐
│ TitleBar (32px)                                             │
├─────────────────────────────────────────────────────────────┤
│ RibbonMenuBar                                               │
├────┬────────────────────────────────────────────────────────┤
│    │                                                        │
│按钮│              ChartDisplayArea                          │
│栏  │               (全宽自适应)                              │
│40px│                                                        │
│    │                                                        │
├────┴────────────────────────────────────────────────────────┤
│ StatusBar                                                   │
└─────────────────────────────────────────────────────────────┘
```

**显示右侧面板流程**:
```
ViewHandler.showRightTab()
    ↓
centerSplitPane.getItems().add(rightTabManager)
    ↓
Platform.runLater(() -> updateDividerPositions())
    ↓
ChartDisplayArea宽度自动调整
```

**隐藏右侧面板流程**:
```
ViewHandler.hideRightTab()
    ↓
centerSplitPane.getItems().remove(rightTabManager)
    ↓
ChartDisplayArea宽度自动增加
```

#### 14.4.4 缩放控件位置响应

缩放控件（zoomControls）位于ChartDisplayArea右下角，使用StackPane布局：

```java
StackPane.setAlignment(zoomControls, Pos.BOTTOM_RIGHT);
StackPane.setMargin(zoomControls, new Insets(0, 10, 10, 0));
```

**位置特性**:
- 始终固定在ChartDisplayArea的右下角
- 距离右边缘10px，距离下边缘10px
- 不受ChartDisplayArea宽度变化影响
- 不受侧边栏展开/折叠影响
- 始终保持可见（除非ChartDisplayArea被隐藏）

### 14.5 视图缩放流程

```
用户在ChartDisplayArea上滚动鼠标滚轮
    ↓
ScrollEvent 触发
    ↓
ChartDisplayArea.handleScroll(event)
    ↓
计算缩放因子 (delta = 1.1 或 1/1.1)
    ↓
计算新的缩放级别 (限制在 0.1 - 10.0)
    ↓
以鼠标位置为中心进行缩放:
├── 计算缩放前的世界坐标
├── 更新 zoomLevel
├── 计算缩放后的世界坐标
└── 调整 centerX, centerY 保持鼠标位置不变
    ↓
更新世界坐标边界
    ↓
通知视口监听器
    ↓
redraw() 重绘所有层
```

---

## 15. 关键代码片段

### 15.1 MainView初始化面板

```java
private void initializePanels() {
    // 创建图层管理面板
    LayerManagerPanel layerManagerPanel = new LayerManagerPanel();
    SideBarManager.SideBarPanel layersPanel = new SideBarManager.SideBarPanel(
            "layers", "图层管理", "/icons/layers.png", "图层管理面板",
            layerManagerPanel.getContent());
    
    // 创建搜索面板
    SearchPanel searchPanel = new SearchPanel();
    SideBarManager.SideBarPanel searchSideBarPanel = new SideBarManager.SideBarPanel(
            "search", "搜索", "/icons/search.png", "搜索面板",
            searchPanel.getContent());
    
    // 注册侧边栏面板
    sideBarManager.registerPanel(layersPanel, 0);
    sideBarManager.registerPanel(searchSideBarPanel, 1);
    // ... 其他面板
    
    // 注册右侧标签页
    PropertyPanel propertyPanel = new PropertyPanel();
    rightTabManager.registerPanel(new AlarmPanel());
    rightTabManager.registerPanel(new LogPanel());
    rightTabManager.registerPanel(propertyPanel);
    rightTabManager.registerPanel(new TerminalPanel());
    
    // 初始化Handler
    initializeHandlers();
    
    // 设置回调
    setupLayerPanelCallbacks(layerManagerPanel, propertyPanel);
    setupSearchPanelCallbacks(searchPanel);
    setupRibbonActions();
}
```

### 15.2 设置图层面板回调

```java
private void setupLayerPanelCallbacks(LayerManagerPanel layerManagerPanel, PropertyPanel propertyPanel) {
    // 隐藏面板回调
    layerManagerPanel.setOnHidePanel(() -> {
        sideBarManager.collapsePanel();
        updateDividerPositions();
    });
    
    // 查看属性回调
    layerManagerPanel.setOnViewProperties(layerNode -> {
        // 清空属性面板
        propertyPanel.clear();
        
        // 添加通用属性
        propertyPanel.addCommonProperty("名称", layerNode.getName());
        propertyPanel.addCommonProperty("ID", layerNode.getId());
        propertyPanel.addCommonProperty("类型", layerNode.getType().toString());
        propertyPanel.addCommonProperty("可见性", layerNode.isVisible() ? "可见" : "隐藏");
        
        // 添加数据属性
        Map<String, String> dataProps = layerNode.getProperties();
        for (Map.Entry<String, String> entry : dataProps.entrySet()) {
            propertyPanel.addDataProperty(entry.getKey(), entry.getValue());
        }
        
        // 切换到属性面板
        rightTabManager.selectPanel("property-panel");
        
        // 如果右侧面板不在分割面板中，则显示
        if (!centerSplitPane.getItems().contains(rightTabManager)) {
            viewHandler.showRightTab();
        }
    });
    
    // 设置图层管理面板引用
    chartHandler.setLayerManagerPanel(layerManagerPanel);
}
```

### 15.3 ViewHandler面板切换

```java
public class ViewHandler {
    // 切换侧边栏
    public void toggleSideBar() {
        if (sideBarManager.isExpanded()) {
            hideSideBar();
        } else {
            showSideBar();
        }
    }
    
    // 切换右侧面板
    public void toggleRightTab() {
        if (centerSplitPane.getItems().contains(rightTabManager)) {
            hideRightTab();
        } else {
            showRightTab();
        }
    }
    
    // 显示右侧面板
    public void showRightTab() {
        if (!centerSplitPane.getItems().contains(rightTabManager)) {
            centerSplitPane.getItems().add(rightTabManager);
            javafx.application.Platform.runLater(() -> {
                if (updateDividerCallback != null) {
                    updateDividerCallback.run();
                }
            });
        }
    }
    
    // 隐藏右侧面板
    public void hideRightTab() {
        centerSplitPane.getItems().remove(rightTabManager);
    }
}
```

---

## 16. 样式类参考

### 16.1 主要样式类

| 组件 | 样式类 |
|------|--------|
| MainView | `.main-view` |
| TitleBar | `.title-bar`, `.title-bar-title`, `.title-bar-button`, `.title-bar-search` |
| RibbonMenuBar | `.ribbon-menu-bar` |
| SideBarManager | `.side-bar-manager`, `.sidebar-button-bar`, `.sidebar-content-pane`, `.sidebar-toggle-button` |
| ChartDisplayArea | `.chart-display-area`, `.zoom-controls`, `.zoom-button` |
| RightTabManager | `.right-tab-manager` |
| StatusBar | `.status-coordinates`, `.status-zoom`, `.status-scale`, `.status-alarm-count`, `.status-layer-count`, `.status-time` |
| WindowControls | `.window-controls`, `.window-control-settings`, `.window-control-minimize`, `.window-control-maximize`, `.window-control-close` |
| LayerManagerPanel | `.layer-manager-panel`, `.panel-header`, `.panel-title`, `.layer-tree`, `.category-node`, `.data-node` |

---

## 17. 文件路径索引

### 17.1 Java文件

| 文件 | 路径 |
|------|------|
| EChartApp | `ecn/echart-app/src/main/java/cn/cycle/echart/app/EChartApp.java` |
| MainView | `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/MainView.java` |
| TitleBar | `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/TitleBar.java` |
| RibbonMenuBar | `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/RibbonMenuBar.java` |
| SideBarManager | `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/SideBarManager.java` |
| ChartDisplayArea | `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/ChartDisplayArea.java` |
| RightTabManager | `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/RightTabManager.java` |
| StatusBar | `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/StatusBar.java` |
| WindowControls | `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/WindowControls.java` |
| WindowResizer | `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/WindowResizer.java` |
| LayerManagerPanel | `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/panel/LayerManagerPanel.java` |
| PropertyPanel | `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/panel/PropertyPanel.java` |
| ViewHandler | `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/handler/ViewHandler.java` |
| ChartHandler | `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/handler/ChartHandler.java` |

### 17.2 FXML文件

| 文件 | 路径 |
|------|------|
| LayerManagerPanel | `ecn/echart-ui/src/main/resources/fxml/LayerManagerPanel.fxml` |
| PropertyPanel | `ecn/echart-ui/src/main/resources/fxml/PropertyPanel.fxml` |
| SearchPanel | `ecn/echart-ui/src/main/resources/fxml/SearchPanel.fxml` |
| MeasurePanel | `ecn/echart-ui/src/main/resources/fxml/MeasurePanel.fxml` |
| SettingsPanel | `ecn/echart-ui/src/main/resources/fxml/SettingsPanel.fxml` |

---

## 18. 复刻要点详解

本章节详细说明复刻界面时需要特别注意的关键点，帮助开发者准确还原界面布局和交互行为。

### 18.1 布局架构关键点

#### 18.1.1 TitleBar叠加技术

**核心原理**: 使用StackPane的层叠特性，将TitleBar悬浮在ContentPane上方。

**实现要点**:
- 根容器使用StackPane，允许子组件层叠显示
- TitleBar添加到StackPane时设置TOP_CENTER对齐
- ContentPane设置顶部内边距32px，为TitleBar预留空间
- TitleBar高度固定为32px，宽度随窗口自适应

**注意事项**:
- TitleBar需要设置鼠标事件穿透，避免阻挡下层组件交互
- 窗口拖拽功能绑定在TitleBar上，需要正确处理鼠标事件
- 窗口最大化时需要禁用拖拽功能

#### 18.1.2 SplitPane三分割布局

**核心原理**: 使用水平方向的SplitPane将界面分为左、中、右三个区域。

**实现要点**:
- SplitPane方向设置为HORIZONTAL（水平）
- 三个子组件依次为：SideBarManager、ChartDisplayArea、RightTabManager
- 左右两侧设置不可随父容器缩放（setResizableWithParent为false）
- 分割器位置通过比例值控制，范围0.0-1.0

**初始分割比例**:
- 左侧分割器位置：约40/总宽度（仅按钮栏宽度）
- 右侧分割器位置：约1 - 300/总宽度（右侧面板宽度）

**动态调整逻辑**:
- 监听第一个分割器位置变化，当左侧内容宽度小于阈值时自动折叠
- 监听第二个分割器位置变化，同步更新右侧面板宽度
- 窗口大小变化时重新计算分割器位置

#### 18.1.3 侧边栏折叠机制

**核心原理**: 使用ToggleButton+StackPane模式，通过控制内容面板的可见性实现折叠。

**组件结构**:
- 外层HBox作为容器
- 左侧VBox作为按钮栏，固定宽度40px
- 右侧StackPane作为内容面板，宽度动态变化

**折叠状态**:
- 内容面板：setVisible(false)、setManaged(false)、setPrefWidth(0)
- 整体宽度：仅保留按钮栏40px
- 活动面板引用：置为null
- ToggleGroup选择：清除选择状态

**展开状态**:
- 内容面板：setVisible(true)、setManaged(true)、setPrefWidth(展开宽度)
- 整体宽度：40px + 内容宽度
- 默认展开宽度：280px（由SIDEBAR_CONTENT_WIDTH常量定义）

**按钮点击行为**:
- 点击已展开的活动面板按钮：触发折叠
- 点击其他面板按钮：切换到该面板并展开（如已折叠）
- ToggleButton使用ToggleGroup确保单选

#### 18.1.4 Canvas分层渲染架构

**核心原理**: 使用5层Canvas叠加，每层负责不同类型的渲染内容。

**层次结构**（从底到上）:
1. **背景层(layer-0)**: 海底地形、水深、背景色等静态背景信息
2. **海图层(layer-1)**: 海图要素、航道、锚地、助航标志等核心海图内容
3. **数据层(layer-2)**: AIS目标、航线、标注等动态数据
4. **交互层(layer-3)**: 测量工具、选择框、编辑器等交互元素
5. **覆盖层(layer-4)**: 提示信息、临时标注、弹出框等

**渲染优化**:
- 使用脏标记机制，只重绘变化的层
- 每层Canvas绑定到父容器的宽高属性
- 变换矩阵统一应用于所有层的GraphicsContext

**坐标系统**:
- 屏幕坐标：以Canvas左上角为原点
- 世界坐标：以海图中心为原点，单位为地理坐标
- 缩放和平移通过变换矩阵实现

### 18.2 交互行为关键点

#### 18.2.1 面板切换交互

**切换流程**:
1. 用户点击侧边栏按钮
2. 判断当前状态：
   - 如果点击的是已展开的活动面板：执行折叠
   - 如果点击的是其他面板：执行切换
3. 切换时：
   - 隐藏当前活动面板（setVisible/Managed=false）
   - 显示新面板（setVisible/Managed=true）
   - 调用面板的onDeactivate/onActivate生命周期方法
4. 更新按钮选中状态

**面板生命周期**:
- onActivate(): 面板被激活时调用，用于初始化数据或刷新显示
- onDeactivate(): 面板被停用时调用，用于保存状态或释放资源

#### 18.2.2 分割器联动交互

**联动机制**:
- 分割器位置变化时，计算实际内容宽度
- 当内容宽度小于阈值（如80px）时，自动触发折叠
- 当内容宽度大于阈值时，自动展开并显示面板

**防抖处理**:
- 使用标志位isDraggingDivider防止递归更新
- 在折叠/展开操作期间暂停分割器监听
- 使用Platform.runLater确保UI更新在主线程执行

**宽度同步**:
- 侧边栏宽度变化时，更新expandedWidth和lastExpandedWidth
- 右侧面板宽度变化时，更新prefWidth属性
- 窗口大小变化时，重新计算分割器位置

#### 18.2.3 属性查看联动

**触发路径**:
1. 用户在图层管理面板中右键点击图层节点
2. 显示上下文菜单
3. 用户点击"查看属性"菜单项
4. 触发onViewProperties回调

**回调处理流程**:
1. 清空属性面板的现有内容
2. 添加通用属性：名称、ID、类型、可见性
3. 添加数据属性：从LayerNode的properties映射中获取
4. 切换右侧标签页到属性面板
5. 如果右侧面板不可见，显示右侧面板

**面板间通信**:
- 使用回调函数（Consumer<LayerNode>）实现面板间通信
- MainView作为协调者，设置各面板的回调函数
- 避免面板之间的直接依赖

#### 18.2.4 视口缩放交互

**缩放触发方式**:
- 鼠标滚轮：向上滚动放大，向下滚动缩小
- Ribbon菜单按钮：放大、缩小、适应窗口
- 缩放控件按钮：界面右下角的+/-按钮

**以鼠标为中心缩放**:
1. 记录缩放前的鼠标位置对应的世界坐标
2. 更新缩放级别（限制在0.1-10.0范围内）
3. 计算缩放后的鼠标位置对应的世界坐标
4. 调整centerX和centerY，使鼠标指向的地理位置保持不变

**视口状态同步**:
- 缩放后更新世界坐标边界（worldMinX/Y, worldMaxX/Y）
- 通知视口监听器（viewportChangeListener）
- 触发重绘（redraw）

### 18.3 状态管理关键点

#### 18.3.1 面板状态管理

**SideBarManager状态**:
- isExpanded: 布尔值，表示侧边栏是否展开
- activePanel: SideBarPanel引用，当前活动的面板
- expandedWidth: 双精度值，当前展开宽度
- lastExpandedWidth: 双精度值，上次展开宽度（用于恢复）

**状态属性绑定**:
- expandedProperty: ReadOnlyBooleanProperty，供外部监听展开状态变化
- currentWidthProperty: ReadOnlyDoubleProperty，供外部监听宽度变化

**状态持久化**:
- 可通过序列化保存面板展开状态和宽度
- 应用启动时恢复上次的面板状态

#### 18.3.2 图层状态管理

**LayerNode状态**:
- id: 唯一标识符
- name: 显示名称
- type: 节点类型（分类/数据）
- visible: 可见性（使用SimpleBooleanProperty实现绑定）
- properties: 属性映射（Map<String, String>）

**可见性绑定**:
- LayerNode的visible属性使用JavaFX Property
- TreeCell中的CheckBox绑定到LayerNode的visible属性
- 可见性变化自动触发海图重绘

**树结构状态**:
- TreeItem的expanded状态控制节点展开/折叠
- 根节点默认展开，分类节点默认展开
- 数据节点按需添加和移除

#### 18.3.3 视口状态管理

**ChartDisplayArea状态**:
- zoomLevel: 缩放级别（0.1-10.0）
- centerX, centerY: 视口中心的世界坐标
- rotation: 旋转角度
- worldMinX/Y, worldMaxX/Y: 世界坐标边界

**状态约束**:
- 缩放级别限制在MIN_ZOOM和MAX_ZOOM之间
- 世界坐标边界根据视口和缩放级别计算
- 平移时更新中心坐标，缩放时更新边界

**状态监听**:
- ViewportChangeListener接口定义视口变化回调
- onZoom: 缩放变化
- onPan: 平移变化
- onViewportChanged: 视口综合变化

#### 18.3.4 响应式布局状态

**布局模式**:
- COMPACT: 紧凑模式（宽度<1024px），隐藏右侧面板
- STANDARD: 标准模式（1024-1440px），显示所有面板
- WIDE: 宽屏模式（>1440px），增加面板宽度

**模式切换**:
- 监听窗口宽度变化
- 根据断点值确定布局模式
- 通知监听器执行布局调整

**布局配置**:
- 每种模式对应不同的面板宽度配置
- 紧凑模式：侧边栏200px，右侧面板隐藏
- 标准模式：侧边栏250px，右侧面板250px
- 宽屏模式：侧边栏300px，右侧面板300px

### 18.4 事件处理关键点

#### 18.4.1 Handler架构

**设计原则**:
- 每个功能领域一个Handler类
- Handler持有相关组件的引用
- MainView作为协调者创建和连接Handler

**Handler列表**:
- FileHandler: 文件操作（新建、打开、保存、导出、打印）
- ViewHandler: 视图操作（缩放、面板切换、状态栏）
- ChartHandler: 海图操作（加载、卸载、图层管理、属性查询）
- RouteHandler: 航线操作（创建、编辑、删除、导入导出）
- AlarmHandler: 预警操作（设置、规则、历史）
- MeasureHandler: 测量操作（距离、面积、方位）
- ToolsHandler: 工具操作（设置、主题）

#### 18.4.2 Ribbon动作绑定

**绑定机制**:
- RibbonMenuBar定义RibbonActionListener接口
- MainView实现接口方法，调用对应Handler
- Handler执行具体业务逻辑

**动作流向**:
用户点击按钮 → RibbonMenuBar触发事件 → MainView接口方法 → Handler执行 → 更新UI组件

#### 18.4.3 事件总线机制

**AppEventBus**:
- 单例模式的事件总线
- 支持订阅/发布模式
- 用于跨模块通信

**事件类型**:
- CHART_LOADED: 海图加载完成
- CHART_UPDATED: 海图更新
- 其他业务事件...

**订阅机制**:
- 组件在initialize()中订阅感兴趣的事件
- 组件在dispose()中取消订阅
- 事件处理在主线程执行

### 18.5 生命周期管理

#### 18.5.1 LifecycleComponent接口

**接口方法**:
- initialize(): 初始化资源、订阅事件
- activate(): 激活组件、开始渲染
- deactivate(): 停用组件、暂停渲染
- dispose(): 释放资源、取消订阅

**状态标志**:
- initialized: 是否已初始化
- active: 是否处于活动状态
- disposed: 是否已销毁

#### 18.5.2 组件生命周期

**MainView生命周期**:
1. 构造函数创建所有子组件
2. setStage()添加TitleBar
3. initialize()初始化面板和Handler
4. activate()激活所有子组件
5. deactivate()停用（窗口最小化时）
6. dispose()销毁（窗口关闭时）

**子组件生命周期**:
- 子组件在父组件initialize时被初始化
- 子组件在父组件activate时被激活
- 子组件在父组件dispose时被销毁

---

## 19. 常见问题与解决方案

### 19.1 布局问题

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| TitleBar遮挡内容 | 内边距未设置 | ContentPane设置setPadding(new Insets(32, 0, 0, 0)) |
| 分割器位置不正确 | 延迟设置未执行 | 使用Platform.runLater延迟设置 |
| 侧边栏无法折叠 | 状态不同步 | 检查isExpanded标志和分割器监听 |
| Canvas不显示 | 宽高未绑定 | 绑定widthProperty和heightProperty |

### 19.2 交互问题

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| 面板切换闪烁 | 未正确隐藏旧面板 | 先隐藏旧面板，再显示新面板 |
| 右键菜单不显示 | ContextMenu未设置 | 检查setOnContextMenuRequested |
| 缩放中心偏移 | 坐标转换错误 | 检查screenToWorld和worldToScreen |
| 拖拽不流畅 | 事件处理阻塞 | 使用事件消费避免重复处理 |

### 19.3 状态问题

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| 状态丢失 | 未持久化 | 实现状态保存和恢复机制 |
| 属性绑定失效 | 监听器被移除 | 检查Property监听器生命周期 |
| 内存泄漏 | 未取消订阅 | 在dispose中取消所有订阅 |

### 19.4 窗口控制问题

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| 窗口无法拖动 | 事件被消费 | 检查TitleBar的鼠标事件处理 |
| 窗口无法调整大小 | 边缘检测失效 | 检查WindowResizer的RESIZE_MARGIN设置 |
| 最大化后无法还原 | 状态未保存 | 检查WindowControls的prevX/Y/Width/Height |
| 关闭时资源未释放 | 未调用dispose | 确保shutdown()中调用mainView.dispose() |
| 双击标题栏无响应 | 未实现双击事件 | 添加setOnMouseClicked处理双击最大化 |

---

**文档维护**: Cycle Team  
**最后更新**: 2026-04-27
