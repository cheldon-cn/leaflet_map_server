# 海图图层管理与属性查看功能 设计文档

**版本**: v1.0  
**日期**: 2026-04-24  
**状态**: 设计中  
**需求来源**: 海图图层管理与属性查看功能 需求规格说明书 v1.0

---

## 1. 概述

### 1.1 文档目的

本文档描述海图图层管理与属性查看功能的技术架构和详细设计，重点阐述：
- 事件驱动架构：使用事件总线实现海图加载事件与图层管理面板的解耦
- 数据绑定：使用JavaFX属性绑定实现图层可见性与地图显示的同步
- 异步加载：对于大型海图文件，使用异步任务加载属性信息

### 1.2 范围

本设计涵盖以下功能：
- F001: 打开海图数据后自动添加到图层管理面板
- F002: 图层节点右键菜单功能
- F003: 右键菜单"查看属性"功能
- F004: 属性面板显示海图数据属性
- F005: 图层节点可见性控制
- F006: 图层节点重命名功能

### 1.3 参考文档

- 海图图层管理与属性查看功能 需求规格说明书 v1.0
- [javafx_chart_application_design.md](../../code/cycle/doc/javafx_chart_application_design.md)
- [view_eventbus.md](./view_eventbus.md)

### 1.4 需求追溯

| 需求ID | 需求描述 | 设计章节 |
|--------|----------|----------|
| F001 | 打开海图添加节点 | 4.1, 4.2 |
| F002 | 右键菜单功能 | 4.3 |
| F003 | 查看属性功能 | 4.4, 4.5 |
| F004 | 属性显示功能 | 4.5 |
| F005 | 可见性控制 | 4.2, 5.2 |
| F006 | 重命名功能 | 4.3 |

---

## 2. 设计目标

### 2.1 核心目标

1. **解耦性**: 海图加载模块与图层管理面板通过事件总线解耦，降低模块间依赖
2. **响应性**: 使用异步加载确保UI不阻塞，提升用户体验
3. **一致性**: 使用数据绑定确保UI状态与数据状态同步
4. **可扩展性**: 设计支持未来扩展更多图层类型和属性字段

### 2.2 设计原则

- **单一职责原则**: 每个类只负责一个功能领域
- **开闭原则**: 对扩展开放，对修改关闭
- **依赖倒置原则**: 依赖抽象而非具体实现
- **接口隔离原则**: 接口应该小而专一

### 2.3 约束条件

- 使用JavaFX作为UI框架
- 使用现有的事件总线实现（`AppEventBus`）
- 兼容现有图层管理面板和属性面板
- 支持S57、S63等主流海图格式

---

## 3. 系统架构

### 3.1 整体架构

采用**事件驱动架构**，结合**MVC模式**：

```
┌─────────────────────────────────────────────────────────────┐
│                      表现层 (View)                           │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ 图层管理面板  │  │  属性面板    │  │  海图显示区  │      │
│  │LayerManager  │  │PropertyPanel │  │ChartDisplay  │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
├─────────────────────────────────────────────────────────────┤
│                      控制层 (Controller)                     │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │LayerManager  │  │PropertyPanel │  │ChartRender   │      │
│  │  Controller  │  │  Controller  │  │  Service     │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
├─────────────────────────────────────────────────────────────┤
│                      业务层 (Service)                        │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ ChartService │  │LayerService  │  │PropertyService│     │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
├─────────────────────────────────────────────────────────────┤
│                      事件总线 (EventBus)                     │
│  ┌──────────────────────────────────────────────────────┐  │
│  │              AppEventBus (单例)                       │  │
│  │  - publish(event)                                     │  │
│  │  - subscribe(type, handler)                           │  │
│  └──────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────┤
│                      数据层 (Data)                           │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ ChartViewer  │  │ LayerNode    │  │ChartData     │      │
│  │ (Native)     │  │ (JavaFX)     │  │ (POJO)       │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 架构决策

#### ADR-001: 选择事件驱动架构

**背景**  
海图加载模块与图层管理面板需要解耦，避免直接依赖。同时需要支持多个UI组件响应同一事件。

**决策**  
采用事件驱动架构，使用现有的`AppEventBus`作为事件总线。

**理由**  
1. 项目已有`AppEventBus`实现，无需引入新依赖
2. 事件驱动架构天然支持模块解耦
3. 支持一对多的事件分发，多个订阅者可响应同一事件
4. 线程安全：`AppEventBus`自动在JavaFX应用线程执行回调

**影响**  
- 海图加载成功后发布`CHART_LOADED`事件
- 图层管理面板订阅`CHART_LOADED`事件
- 属性面板订阅`LAYER_SELECTED`事件
- 所有事件处理在JavaFX应用线程执行

#### ADR-002: 使用JavaFX属性绑定

**背景**  
图层可见性状态需要在多个地方同步：图层管理面板的复选框、地图显示区域、数据模型。

**决策**  
使用JavaFX属性绑定机制，将`LayerNode.visible`属性绑定到UI控件和渲染逻辑。

**理由**  
1. JavaFX属性绑定自动同步，无需手动更新
2. 支持双向绑定，UI变化自动反映到数据模型
3. 支持监听器模式，属性变化时触发回调

**影响**  
- `LayerNode`类使用`BooleanProperty`存储可见性
- UI控件使用`selectedProperty().bindBidirectional()`
- 属性变化监听器触发地图重绘

#### ADR-003: 使用异步任务加载属性

**背景**  
大型海图文件的属性信息可能较多，同步加载会阻塞UI线程，导致界面卡顿。

**决策**  
使用`javafx.concurrent.Task`在后台线程加载属性信息，加载完成后在JavaFX应用线程更新UI。

**理由**  
1. JavaFX Task提供线程安全的异步任务支持
2. 支持进度更新和取消操作
3. 自动在JavaFX应用线程执行成功/失败回调
4. 避免UI阻塞，提升用户体验

**影响**  
- 属性加载使用`PropertyLoadTask`类
- 加载过程中显示进度提示
- 加载失败时显示错误信息

### 3.3 技术选型

| 层次 | 技术选型 | 版本 | 说明 |
|------|----------|------|------|
| UI框架 | JavaFX | 8+ | 已使用 |
| 事件总线 | AppEventBus | 现有 | 已实现 |
| 异步任务 | javafx.concurrent.Task | 8+ | JavaFX内置 |
| 数据绑定 | JavaFX Properties | 8+ | JavaFX内置 |
| 日志 | SLF4J + Logback | 现有 | 已使用 |

---

## 4. 核心模块设计

### 4.1 事件定义模块 (event)

**功能描述**: 定义海图图层管理相关的事件类型和事件数据结构

**需求覆盖**: F001

**类设计**:

#### 4.1.1 ChartLayerEvent

```java
package cn.cycle.echart.event;

import cn.cycle.app.event.AppEvent;
import cn.cycle.app.event.AppEventType;

/**
 * 海图图层事件。
 * 
 * <p>封装海图图层相关的事件数据。</p>
 */
public class ChartLayerEvent extends AppEvent {
    
    public static final AppEventType CHART_LAYER_ADDED = 
        AppEventType.valueOf("CHART_LAYER_ADDED");
    public static final AppEventType CHART_LAYER_REMOVED = 
        AppEventType.valueOf("CHART_LAYER_REMOVED");
    public static final AppEventType CHART_LAYER_SELECTED = 
        AppEventType.valueOf("CHART_LAYER_SELECTED");
    public static final AppEventType CHART_LAYER_VISIBILITY_CHANGED = 
        AppEventType.valueOf("CHART_LAYER_VISIBILITY_CHANGED");
    public static final AppEventType CHART_PROPERTIES_LOADED = 
        AppEventType.valueOf("CHART_PROPERTIES_LOADED");
    
    public ChartLayerEvent(Object source, AppEventType type) {
        super(source, type);
    }
    
    public ChartLayerEvent(Object source, AppEventType type, Object data) {
        super(source, type);
        setData("layerData", data);
    }
    
    public void setLayerId(String layerId) {
        setData("layerId", layerId);
    }
    
    public String getLayerId() {
        return getData("layerId", String.class);
    }
    
    public void setLayerName(String name) {
        setData("layerName", name);
    }
    
    public String getLayerName() {
        return getData("layerName", String.class);
    }
    
    public void setChartPath(String path) {
        setData("chartPath", path);
    }
    
    public String getChartPath() {
        return getData("chartPath", String.class);
    }
    
    public void setVisible(boolean visible) {
        setData("visible", visible);
    }
    
    public boolean isVisible() {
        return getData("visible", Boolean.class);
    }
}
```

#### 4.1.2 扩展AppEventType

在现有`AppEventType`枚举中添加新事件类型：

```java
public enum AppEventType {
    // 现有事件类型...
    
    // 海图图层事件
    CHART_LAYER_ADDED,
    CHART_LAYER_REMOVED,
    CHART_LAYER_SELECTED,
    CHART_LAYER_VISIBILITY_CHANGED,
    CHART_PROPERTIES_LOADED,
    
    // 属性面板事件
    PROPERTY_PANEL_SWITCH_REQUEST,
    PROPERTY_LOAD_STARTED,
    PROPERTY_LOAD_COMPLETED,
    PROPERTY_LOAD_FAILED
}
```

**依赖关系**: 无

---

### 4.2 图层数据模型模块 (model)

**功能描述**: 定义图层节点的数据模型，支持JavaFX属性绑定

**需求覆盖**: F001, F005

**类设计**:

#### 4.2.1 LayerNode (增强版)

```java
package cn.cycle.echart.model;

import javafx.beans.property.*;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;

/**
 * 图层节点数据模型。
 * 
 * <p>使用JavaFX属性支持数据绑定。</p>
 */
public class LayerNode {
    
    private final StringProperty id = new SimpleStringProperty();
    private final StringProperty name = new SimpleStringProperty();
    private final ObjectProperty<LayerNodeType> type = new SimpleObjectProperty<>();
    private final BooleanProperty visible = new SimpleBooleanProperty(true);
    private final BooleanProperty selected = new SimpleBooleanProperty(false);
    private final DoubleProperty opacity = new SimpleDoubleProperty(1.0);
    private final StringProperty chartPath = new SimpleStringProperty();
    private final ObjectProperty<ChartData> chartData = new SimpleObjectProperty<>();
    
    private ObservableList<LayerNode> children = FXCollections.observableArrayList();
    private LayerNode parent;
    
    public LayerNode() {
        this("", "", LayerNodeType.CATEGORY_CHART);
    }
    
    public LayerNode(String id, String name, LayerNodeType type) {
        setId(id);
        setName(name);
        setType(type);
        
        visible.addListener((obs, oldVal, newVal) -> {
            onVisibilityChanged(newVal);
        });
        
        opacity.addListener((obs, oldVal, newVal) -> {
            onOpacityChanged(newVal.doubleValue());
        });
    }
    
    // ==================== 属性访问器 ====================
    
    public StringProperty idProperty() {
        return id;
    }
    
    public String getId() {
        return id.get();
    }
    
    public void setId(String value) {
        id.set(value);
    }
    
    public StringProperty nameProperty() {
        return name;
    }
    
    public String getName() {
        return name.get();
    }
    
    public void setName(String value) {
        name.set(value);
    }
    
    public ObjectProperty<LayerNodeType> typeProperty() {
        return type;
    }
    
    public LayerNodeType getType() {
        return type.get();
    }
    
    public void setType(LayerNodeType value) {
        type.set(value);
    }
    
    public BooleanProperty visibleProperty() {
        return visible;
    }
    
    public boolean isVisible() {
        return visible.get();
    }
    
    public void setVisible(boolean value) {
        visible.set(value);
    }
    
    public BooleanProperty selectedProperty() {
        return selected;
    }
    
    public boolean isSelected() {
        return selected.get();
    }
    
    public void setSelected(boolean value) {
        selected.set(value);
    }
    
    public DoubleProperty opacityProperty() {
        return opacity;
    }
    
    public double getOpacity() {
        return opacity.get();
    }
    
    public void setOpacity(double value) {
        opacity.set(value);
    }
    
    public StringProperty chartPathProperty() {
        return chartPath;
    }
    
    public String getChartPath() {
        return chartPath.get();
    }
    
    public void setChartPath(String value) {
        chartPath.set(value);
    }
    
    public ObjectProperty<ChartData> chartDataProperty() {
        return chartData;
    }
    
    public ChartData getChartData() {
        return chartData.get();
    }
    
    public void setChartData(ChartData value) {
        chartData.set(value);
    }
    
    // ==================== 子节点管理 ====================
    
    public ObservableList<LayerNode> getChildren() {
        return children;
    }
    
    public void addChild(LayerNode child) {
        child.parent = this;
        children.add(child);
    }
    
    public void removeChild(LayerNode child) {
        children.remove(child);
        child.parent = null;
    }
    
    public LayerNode getParent() {
        return parent;
    }
    
    // ==================== 事件处理 ====================
    
    protected void onVisibilityChanged(boolean newVisibility) {
        // 发布可见性变化事件
        ChartLayerEvent event = new ChartLayerEvent(this, 
            AppEventType.CHART_LAYER_VISIBILITY_CHANGED);
        event.setLayerId(getId());
        event.setVisible(newVisibility);
        AppEventBus.getInstance().publish(event);
    }
    
    protected void onOpacityChanged(double newOpacity) {
        // 触发重绘
        // 由监听器处理
    }
    
    // ==================== 工具方法 ====================
    
    public boolean isCategory() {
        return getType().isCategory();
    }
    
    public boolean isLeaf() {
        return children.isEmpty();
    }
    
    @Override
    public String toString() {
        return getName();
    }
}
```

#### 4.2.2 ChartData

```java
package cn.cycle.echart.model;

import java.io.File;
import java.util.LinkedHashMap;
import java.util.Map;

/**
 * 海图数据模型。
 * 
 * <p>存储海图的元数据和属性信息。</p>
 */
public class ChartData {
    
    private String filePath;
    private String fileName;
    private long fileSize;
    private String format;
    private String coordinateSystem;
    private double minX, minY, maxX, maxY;
    private int featureCount;
    private int layerCount;
    private Map<String, String> properties;
    
    public ChartData() {
        properties = new LinkedHashMap<>();
    }
    
    public ChartData(String filePath) {
        this();
        this.filePath = filePath;
        File file = new File(filePath);
        if (file.exists()) {
            this.fileName = file.getName();
            this.fileSize = file.length();
        }
    }
    
    // ==================== 通用属性 ====================
    
    public Map<String, String> getCommonProperties() {
        Map<String, String> commonProps = new LinkedHashMap<>();
        commonProps.put("文件名", fileName);
        commonProps.put("文件路径", filePath);
        commonProps.put("文件大小", formatFileSize(fileSize));
        commonProps.put("文件格式", format != null ? format : "未知");
        commonProps.put("坐标系", coordinateSystem != null ? coordinateSystem : "未知");
        return commonProps;
    }
    
    // ==================== 数据关联属性 ====================
    
    public Map<String, String> getDataProperties() {
        Map<String, String> dataProps = new LinkedHashMap<>();
        dataProps.put("要素数量", String.valueOf(featureCount));
        dataProps.put("图层数量", String.valueOf(layerCount));
        dataProps.put("范围X", String.format("%.6f ~ %.6f", minX, maxX));
        dataProps.put("范围Y", String.format("%.6f ~ %.6f", minY, maxY));
        properties.forEach(dataProps::put);
        return dataProps;
    }
    
    // ==================== Getter/Setter ====================
    
    public String getFilePath() {
        return filePath;
    }
    
    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }
    
    public String getFileName() {
        return fileName;
    }
    
    public void setFileName(String fileName) {
        this.fileName = fileName;
    }
    
    public long getFileSize() {
        return fileSize;
    }
    
    public void setFileSize(long fileSize) {
        this.fileSize = fileSize;
    }
    
    public String getFormat() {
        return format;
    }
    
    public void setFormat(String format) {
        this.format = format;
    }
    
    public String getCoordinateSystem() {
        return coordinateSystem;
    }
    
    public void setCoordinateSystem(String coordinateSystem) {
        this.coordinateSystem = coordinateSystem;
    }
    
    public double getMinX() {
        return minX;
    }
    
    public void setMinX(double minX) {
        this.minX = minX;
    }
    
    public double getMinY() {
        return minY;
    }
    
    public void setMinY(double minY) {
        this.minY = minY;
    }
    
    public double getMaxX() {
        return maxX;
    }
    
    public void setMaxX(double maxX) {
        this.maxX = maxX;
    }
    
    public double getMaxY() {
        return maxY;
    }
    
    public void setMaxY(double maxY) {
        this.maxY = maxY;
    }
    
    public int getFeatureCount() {
        return featureCount;
    }
    
    public void setFeatureCount(int featureCount) {
        this.featureCount = featureCount;
    }
    
    public int getLayerCount() {
        return layerCount;
    }
    
    public void setLayerCount(int layerCount) {
        this.layerCount = layerCount;
    }
    
    public Map<String, String> getProperties() {
        return properties;
    }
    
    public void setProperties(Map<String, String> properties) {
        this.properties = properties;
    }
    
    public void addProperty(String key, String value) {
        properties.put(key, value);
    }
    
    // ==================== 工具方法 ====================
    
    private String formatFileSize(long size) {
        if (size < 1024) {
            return size + " B";
        } else if (size < 1024 * 1024) {
            return String.format("%.2f KB", size / 1024.0);
        } else if (size < 1024 * 1024 * 1024) {
            return String.format("%.2f MB", size / (1024.0 * 1024));
        } else {
            return String.format("%.2f GB", size / (1024.0 * 1024 * 1024));
        }
    }
}
```

**依赖关系**: 
- 依赖 `AppEventBus`
- 依赖 `ChartLayerEvent`

---

### 4.3 图层管理面板控制器模块 (controller)

**功能描述**: 增强图层管理面板控制器，支持事件订阅和右键菜单

**需求覆盖**: F001, F002, F003, F006

**类设计**:

#### 4.3.1 LayerManagerPanelController (增强版)

```java
package cn.cycle.echart.ui.panel;

import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.echart.event.ChartLayerEvent;
import cn.cycle.echart.model.ChartData;
import cn.cycle.echart.model.LayerNode;
import cn.cycle.echart.model.LayerNodeType;
import cn.cycle.echart.service.PropertyLoadService;
import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.input.ContextMenuEvent;

import java.io.File;
import java.net.URL;
import java.util.ResourceBundle;
import java.util.function.Consumer;

public class LayerManagerPanelController implements Initializable {

    @FXML private TreeView<LayerNode> layerTreeView;
    @FXML private ContextMenu treeContextMenu;
    
    private TreeItem<LayerNode> rootNode;
    private Map<String, TreeItem<LayerNode>> categoryNodes;
    
    private Consumer<LayerNode> onViewProperties;
    private Consumer<LayerNode> onLayerAction;
    private PropertyLoadService propertyLoadService;
    
    @Override
    public void initialize(URL location, ResourceBundle resources) {
        categoryNodes = new LinkedHashMap<>();
        initializeTree();
        initializeListeners();
        subscribeToEvents();
    }
    
    private void initializeTree() {
        LayerNode rootData = new LayerNode("root", "图层", LayerNodeType.ROOT);
        rootNode = new TreeItem<>(rootData);
        rootNode.setExpanded(true);
        
        addCategory("海图数据", LayerNodeType.CATEGORY_CHART);
        addCategory("航线数据", LayerNodeType.CATEGORY_ROUTE);
        addCategory("AIS数据", LayerNodeType.CATEGORY_AIS);
        addCategory("预警区域", LayerNodeType.CATEGORY_ALARM);
        addCategory("叠加图层", LayerNodeType.CATEGORY_OVERLAY);
        
        layerTreeView.setRoot(rootNode);
        layerTreeView.setShowRoot(false);
        
        layerTreeView.setCellFactory(tree -> new LayerTreeCell());
    }
    
    private void addCategory(String name, LayerNodeType type) {
        LayerNode categoryData = new LayerNode(
            name.toLowerCase().replace(" ", "-"), name, type);
        TreeItem<LayerNode> categoryItem = new TreeItem<>(categoryData);
        categoryItem.setExpanded(true);
        rootNode.getChildren().add(categoryItem);
        categoryNodes.put(name, categoryItem);
    }
    
    // ==================== 事件订阅 ====================
    
    private void subscribeToEvents() {
        AppEventBus.getInstance().subscribe(
            AppEventType.CHART_LOADED, 
            this::handleChartLoaded
        );
        
        AppEventBus.getInstance().subscribe(
            AppEventType.CHART_LAYER_VISIBILITY_CHANGED,
            this::handleLayerVisibilityChanged
        );
    }
    
    private void handleChartLoaded(AppEvent event) {
        String chartPath = event.getData("chartPath", String.class);
        if (chartPath != null) {
            Platform.runLater(() -> {
                addChartLayer(chartPath);
            });
        }
    }
    
    private void handleLayerVisibilityChanged(AppEvent event) {
        String layerId = event.getData("layerId", String.class);
        Boolean visible = event.getData("visible", Boolean.class);
        
        if (layerId != null && visible != null) {
            updateLayerVisibility(layerId, visible);
        }
    }
    
    // ==================== 图层管理 ====================
    
    public void addChartLayer(String chartPath) {
        File file = new File(chartPath);
        String fileName = file.getName();
        String layerId = "chart-" + System.currentTimeMillis();
        
        LayerNode layerNode = new LayerNode(layerId, fileName, LayerNodeType.CHART);
        layerNode.setChartPath(chartPath);
        layerNode.setVisible(true);
        
        ChartData chartData = new ChartData(chartPath);
        layerNode.setChartData(chartData);
        
        TreeItem<LayerNode> chartCategory = categoryNodes.get("海图数据");
        if (chartCategory != null) {
            TreeItem<LayerNode> layerItem = new TreeItem<>(layerNode);
            chartCategory.getChildren().add(layerItem);
            
            chartCategory.setExpanded(true);
            
            layerTreeView.getSelectionModel().select(layerItem);
            
            publishLayerAddedEvent(layerNode);
        }
    }
    
    private void publishLayerAddedEvent(LayerNode layerNode) {
        ChartLayerEvent event = new ChartLayerEvent(this, 
            AppEventType.CHART_LAYER_ADDED);
        event.setLayerId(layerNode.getId());
        event.setLayerName(layerNode.getName());
        event.setChartPath(layerNode.getChartPath());
        AppEventBus.getInstance().publish(event);
    }
    
    private void updateLayerVisibility(String layerId, boolean visible) {
        findLayerNode(layerId).ifPresent(node -> {
            if (node.isVisible() != visible) {
                node.setVisible(visible);
            }
        });
    }
    
    private Optional<LayerNode> findLayerNode(String layerId) {
        for (TreeItem<LayerNode> category : rootNode.getChildren()) {
            for (TreeItem<LayerNode> layer : category.getChildren()) {
                if (layer.getValue().getId().equals(layerId)) {
                    return Optional.of(layer.getValue());
                }
            }
        }
        return Optional.empty();
    }
    
    // ==================== 右键菜单处理 ====================
    
    @FXML
    private void onViewProperties() {
        TreeItem<LayerNode> selectedItem = layerTreeView.getSelectionModel().getSelectedItem();
        if (selectedItem != null) {
            LayerNode node = selectedItem.getValue();
            if (!node.isCategory()) {
                if (onViewProperties != null) {
                    onViewProperties.accept(node);
                }
                
                publishLayerSelectedEvent(node);
            }
        }
    }
    
    private void publishLayerSelectedEvent(LayerNode node) {
        ChartLayerEvent event = new ChartLayerEvent(this, 
            AppEventType.CHART_LAYER_SELECTED);
        event.setLayerId(node.getId());
        event.setLayerName(node.getName());
        event.setChartPath(node.getChartPath());
        AppEventBus.getInstance().publish(event);
    }
    
    @FXML
    private void onToggleVisibility() {
        TreeItem<LayerNode> selectedItem = layerTreeView.getSelectionModel().getSelectedItem();
        if (selectedItem != null) {
            LayerNode node = selectedItem.getValue();
            node.setVisible(!node.isVisible());
        }
    }
    
    @FXML
    private void onRename() {
        TreeItem<LayerNode> selectedItem = layerTreeView.getSelectionModel().getSelectedItem();
        if (selectedItem == null) {
            return;
        }
        
        LayerNode node = selectedItem.getValue();
        if (node.isCategory()) {
            return;
        }
        
        TextInputDialog dialog = new TextInputDialog(node.getName());
        dialog.setTitle("重命名");
        dialog.setHeaderText("请输入新名称");
        dialog.setContentText("名称:");
        
        dialog.showAndWait().ifPresent(name -> {
            if (!name.trim().isEmpty()) {
                node.setName(name.trim());
            }
        });
    }
    
    // ==================== 设置回调 ====================
    
    public void setOnViewProperties(Consumer<LayerNode> callback) {
        this.onViewProperties = callback;
    }
    
    public void setOnLayerAction(Consumer<LayerNode> callback) {
        this.onLayerAction = callback;
    }
    
    public void setPropertyLoadService(PropertyLoadService service) {
        this.propertyLoadService = service;
    }
    
    // ==================== 清理 ====================
    
    public void dispose() {
        AppEventBus.getInstance().unsubscribe(
            AppEventType.CHART_LOADED, 
            this::handleChartLoaded
        );
        AppEventBus.getInstance().unsubscribe(
            AppEventType.CHART_LAYER_VISIBILITY_CHANGED,
            this::handleLayerVisibilityChanged
        );
    }
}
```

**依赖关系**: 
- 依赖 `AppEventBus`
- 依赖 `ChartLayerEvent`
- 依赖 `LayerNode`
- 依赖 `ChartData`
- 依赖 `PropertyLoadService`

---

### 4.4 右侧面板管理模块 (manager)

**功能描述**: 管理右侧面板的切换，支持从图层管理面板切换到属性面板

**需求覆盖**: F003

**类设计**:

#### 4.4.1 RightTabManager (增强版)

```java
package cn.cycle.echart.ui.manager;

import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.echart.ui.panel.PropertyPanel;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;

import java.util.HashMap;
import java.util.Map;

/**
 * 右侧标签页管理器。
 * 
 * <p>管理右侧面板的标签页切换。</p>
 */
public class RightTabManager {
    
    private TabPane tabPane;
    private Map<String, Tab> tabs;
    private PropertyPanel propertyPanel;
    
    public RightTabManager(TabPane tabPane) {
        this.tabPane = tabPane;
        this.tabs = new HashMap<>();
        initializeTabs();
        subscribeToEvents();
    }
    
    private void initializeTabs() {
        for (Tab tab : tabPane.getTabs()) {
            tabs.put(tab.getId(), tab);
            
            if ("property-panel".equals(tab.getId())) {
                if (tab.getContent() instanceof PropertyPanel) {
                    propertyPanel = (PropertyPanel) tab.getContent();
                }
            }
        }
    }
    
    private void subscribeToEvents() {
        AppEventBus.getInstance().subscribe(
            AppEventType.PROPERTY_PANEL_SWITCH_REQUEST,
            this::handlePropertyPanelSwitchRequest
        );
        
        AppEventBus.getInstance().subscribe(
            AppEventType.CHART_LAYER_SELECTED,
            this::handleLayerSelected
        );
    }
    
    private void handlePropertyPanelSwitchRequest(AppEvent event) {
        switchToTab("property-panel");
    }
    
    private void handleLayerSelected(AppEvent event) {
        switchToTab("property-panel");
    }
    
    public void switchToTab(String tabId) {
        Tab targetTab = tabs.get(tabId);
        if (targetTab != null) {
            tabPane.getSelectionModel().select(targetTab);
        }
    }
    
    public void showPropertyPanel() {
        switchToTab("property-panel");
    }
    
    public PropertyPanel getPropertyPanel() {
        return propertyPanel;
    }
    
    public void dispose() {
        AppEventBus.getInstance().unsubscribe(
            AppEventType.PROPERTY_PANEL_SWITCH_REQUEST,
            this::handlePropertyPanelSwitchRequest
        );
        AppEventBus.getInstance().unsubscribe(
            AppEventType.CHART_LAYER_SELECTED,
            this::handleLayerSelected
        );
    }
}
```

**依赖关系**: 
- 依赖 `AppEventBus`
- 依赖 `PropertyPanel`

---

### 4.5 属性加载服务模块 (service)

**功能描述**: 异步加载海图属性信息

**需求覆盖**: F004

**类设计**:

#### 4.5.1 PropertyLoadService

```java
package cn.cycle.echart.service;

import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.echart.event.ChartLayerEvent;
import cn.cycle.echart.model.ChartData;
import cn.cycle.echart.model.LayerNode;
import javafx.concurrent.Task;

import java.io.File;
import java.util.Map;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * 属性加载服务。
 * 
 * <p>异步加载海图属性信息。</p>
 */
public class PropertyLoadService {
    
    private final ExecutorService executor;
    private ChartViewer chartViewer;
    
    public PropertyLoadService() {
        this.executor = Executors.newFixedThreadPool(2);
    }
    
    public void setChartViewer(ChartViewer viewer) {
        this.chartViewer = viewer;
    }
    
    public void loadPropertiesAsync(LayerNode layerNode, 
                                     PropertyLoadCallback callback) {
        PropertyLoadTask task = new PropertyLoadTask(layerNode, chartViewer);
        
        task.setOnSucceeded(e -> {
            ChartData chartData = task.getValue();
            layerNode.setChartData(chartData);
            
            publishPropertiesLoadedEvent(layerNode, chartData);
            
            if (callback != null) {
                callback.onSuccess(chartData);
            }
        });
        
        task.setOnFailed(e -> {
            Throwable exception = task.getException();
            if (callback != null) {
                callback.onFailure(exception);
            }
        });
        
        executor.submit(task);
    }
    
    private void publishPropertiesLoadedEvent(LayerNode node, ChartData data) {
        ChartLayerEvent event = new ChartLayerEvent(this, 
            AppEventType.CHART_PROPERTIES_LOADED);
        event.setLayerId(node.getId());
        event.setLayerName(node.getName());
        event.setData("chartData", data);
        AppEventBus.getInstance().publish(event);
    }
    
    public void shutdown() {
        executor.shutdown();
    }
    
    public interface PropertyLoadCallback {
        void onSuccess(ChartData chartData);
        void onFailure(Throwable exception);
    }
}
```

#### 4.5.2 PropertyLoadTask

```java
package cn.cycle.echart.service;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.geometry.Envelope;
import cn.cycle.echart.model.ChartData;
import cn.cycle.echart.model.LayerNode;
import javafx.concurrent.Task;

import java.io.File;

/**
 * 属性加载任务。
 * 
 * <p>在后台线程加载海图属性信息。</p>
 */
public class PropertyLoadTask extends Task<ChartData> {
    
    private final LayerNode layerNode;
    private final ChartViewer chartViewer;
    
    public PropertyLoadTask(LayerNode layerNode, ChartViewer chartViewer) {
        this.layerNode = layerNode;
        this.chartViewer = chartViewer;
    }
    
    @Override
    protected ChartData call() throws Exception {
        updateMessage("正在加载属性信息...");
        updateProgress(0, 100);
        
        String chartPath = layerNode.getChartPath();
        ChartData chartData = new ChartData(chartPath);
        
        updateProgress(20, 100);
        updateMessage("读取文件信息...");
        
        File file = new File(chartPath);
        if (file.exists()) {
            chartData.setFileName(file.getName());
            chartData.setFileSize(file.length());
        }
        
        updateProgress(40, 100);
        updateMessage("读取海图元数据...");
        
        if (chartViewer != null) {
            Envelope extent = chartViewer.getFullExtent();
            if (extent != null) {
                chartData.setMinX(extent.getMinX());
                chartData.setMinY(extent.getMinY());
                chartData.setMaxX(extent.getMaxX());
                chartData.setMaxY(extent.getMaxY());
            }
            
            chartData.setCoordinateSystem("WGS84");
            chartData.setFormat("S57");
            
            chartData.setFeatureCount(estimateFeatureCount(chartPath));
            chartData.setLayerCount(estimateLayerCount(chartPath));
        }
        
        updateProgress(80, 100);
        updateMessage("加载完成...");
        
        Thread.sleep(100);
        
        updateProgress(100, 100);
        
        return chartData;
    }
    
    private int estimateFeatureCount(String chartPath) {
        return 1000;
    }
    
    private int estimateLayerCount(String chartPath) {
        return 10;
    }
}
```

**依赖关系**: 
- 依赖 `ChartViewer`
- 依赖 `LayerNode`
- 依赖 `ChartData`
- 依赖 `AppEventBus`

---

## 5. 数据绑定设计

### 5.1 图层可见性绑定

**目标**: 实现图层可见性状态在UI控件、数据模型和地图显示之间的自动同步

**实现方案**:

#### 5.1.1 LayerTreeCell (自定义树单元格)

```java
package cn.cycle.echart.ui.cell;

import cn.cycle.echart.model.LayerNode;
import javafx.scene.control.CheckBox;
import javafx.scene.control.CheckBoxTreeItem;
import javafx.scene.control.TreeCell;
import javafx.scene.layout.HBox;
import javafx.scene.text.Text;

/**
 * 图层树单元格。
 * 
 * <p>支持可见性复选框和图标显示。</p>
 */
public class LayerTreeCell extends TreeCell<LayerNode> {
    
    private final HBox container;
    private final CheckBox visibilityCheckBox;
    private final Text label;
    
    public LayerTreeCell() {
        container = new HBox(5);
        visibilityCheckBox = new CheckBox();
        label = new Text();
        
        container.getChildren().addAll(visibilityCheckBox, label);
        
        visibilityCheckBox.selectedProperty().addListener((obs, oldVal, newVal) -> {
            if (getItem() != null && !getItem().isCategory()) {
                getItem().setVisible(newVal);
            }
        });
    }
    
    @Override
    protected void updateItem(LayerNode item, boolean empty) {
        super.updateItem(item, empty);
        
        if (empty || item == null) {
            setGraphic(null);
            setText(null);
        } else {
            label.setText(item.getName());
            
            if (!item.isCategory()) {
                visibilityCheckBox.selectedProperty().unbind();
                visibilityCheckBox.selectedProperty().bindBidirectional(
                    item.visibleProperty());
                visibilityCheckBox.setDisable(false);
            } else {
                visibilityCheckBox.setSelected(true);
                visibilityCheckBox.setDisable(true);
            }
            
            setGraphic(container);
            setText(null);
        }
    }
}
```

#### 5.1.2 图层可见性监听器

在`ChartDisplayArea`中监听图层可见性变化：

```java
private void setupLayerVisibilityBinding() {
    AppEventBus.getInstance().subscribe(
        AppEventType.CHART_LAYER_VISIBILITY_CHANGED,
        event -> {
            String layerId = event.getData("layerId", String.class);
            Boolean visible = event.getData("visible", Boolean.class);
            
            if (layerId != null && visible != null) {
                updateLayerVisibility(layerId, visible);
            }
        }
    );
}

private void updateLayerVisibility(String layerId, boolean visible) {
    // 更新地图显示
    redraw();
}
```

### 5.2 属性面板数据绑定

**目标**: 属性面板自动显示选中的图层属性

**实现方案**:

#### 5.2.1 PropertyPanelController (增强版)

```java
public class PropertyPanelController implements Initializable {
    
    @FXML private GridPane commonPropertiesGrid;
    @FXML private GridPane dataPropertiesGrid;
    
    private final Map<String, String> commonProperties = new LinkedHashMap<>();
    private final Map<String, String> dataProperties = new LinkedHashMap<>();
    
    private String currentObjectName;
    private LayerNode currentLayerNode;
    
    @Override
    public void initialize(URL location, ResourceBundle resources) {
        subscribeToEvents();
    }
    
    private void subscribeToEvents() {
        AppEventBus.getInstance().subscribe(
            AppEventType.CHART_LAYER_SELECTED,
            this::handleLayerSelected
        );
        
        AppEventBus.getInstance().subscribe(
            AppEventType.CHART_PROPERTIES_LOADED,
            this::handlePropertiesLoaded
        );
    }
    
    private void handleLayerSelected(AppEvent event) {
        String layerId = event.getData("layerId", String.class);
        String layerName = event.getData("layerName", String.class);
        String chartPath = event.getData("chartPath", String.class);
        
        currentObjectName = layerName;
        
        commonProperties.clear();
        commonProperties.put("名称", layerName);
        commonProperties.put("路径", chartPath);
        
        dataProperties.clear();
        dataProperties.put("状态", "加载中...");
        
        refreshDisplay();
    }
    
    private void handlePropertiesLoaded(AppEvent event) {
        ChartData chartData = event.getData("chartData", ChartData.class);
        
        if (chartData != null) {
            commonProperties.putAll(chartData.getCommonProperties());
            dataProperties.clear();
            dataProperties.putAll(chartData.getDataProperties());
            
            refreshDisplay();
        }
    }
    
    public void setObject(String objectName, 
                          Map<String, String> commonProps, 
                          Map<String, String> dataProps) {
        this.currentObjectName = objectName;
        this.commonProperties.clear();
        this.commonProperties.putAll(commonProps);
        this.dataProperties.clear();
        this.dataProperties.putAll(dataProps);
        
        refreshDisplay();
    }
    
    public void refreshDisplay() {
        populateGrid(commonPropertiesGrid, commonProperties);
        populateGrid(dataPropertiesGrid, dataProperties);
    }
    
    private void populateGrid(GridPane grid, Map<String, String> properties) {
        grid.getChildren().clear();
        
        int row = 0;
        for (Map.Entry<String, String> entry : properties.entrySet()) {
            Label nameLabel = new Label(entry.getKey());
            nameLabel.setStyle("-fx-font-weight: bold;");
            
            Label valueLabel = new Label(entry.getValue());
            valueLabel.setWrapText(true);
            
            grid.add(nameLabel, 0, row);
            grid.add(valueLabel, 1, row);
            
            row++;
        }
    }
}
```

---

## 6. 异步加载设计

### 6.1 异步加载流程

```
用户点击"查看属性"
        ↓
LayerManagerPanelController.onViewProperties()
        ↓
发布 CHART_LAYER_SELECTED 事件
        ↓
RightTabManager 接收事件
        ↓
切换到属性面板
        ↓
PropertyPanelController 接收事件
        ↓
显示"加载中..."
        ↓
PropertyLoadService.loadPropertiesAsync()
        ↓
创建 PropertyLoadTask
        ↓
在后台线程执行
        ↓
加载完成后发布 CHART_PROPERTIES_LOADED 事件
        ↓
PropertyPanelController 接收事件
        ↓
更新属性显示
```

### 6.2 进度反馈

在属性面板中显示加载进度：

```java
private void showLoadingProgress() {
    commonPropertiesGrid.getChildren().clear();
    dataPropertiesGrid.getChildren().clear();
    
    ProgressIndicator progress = new ProgressIndicator();
    progress.setMaxSize(50, 50);
    
    Label loadingLabel = new Label("正在加载属性信息...");
    
    VBox loadingBox = new VBox(10, progress, loadingLabel);
    loadingBox.setAlignment(Pos.CENTER);
    
    dataPropertiesGrid.add(loadingBox, 0, 0, 2, 1);
}
```

### 6.3 错误处理

加载失败时显示错误信息：

```java
private void showError(String errorMessage) {
    dataPropertiesGrid.getChildren().clear();
    
    Label errorLabel = new Label("加载失败: " + errorMessage);
    errorLabel.setStyle("-fx-text-fill: red;");
    
    Button retryButton = new Button("重试");
    retryButton.setOnAction(e -> {
        if (currentLayerNode != null) {
            propertyLoadService.loadPropertiesAsync(
                currentLayerNode, 
                new PropertyLoadCallback() {
                    @Override
                    public void onSuccess(ChartData chartData) {
                        updateProperties(chartData);
                    }
                    
                    @Override
                    public void onFailure(Throwable exception) {
                        showError(exception.getMessage());
                    }
                }
            );
        }
    });
    
    HBox errorBox = new HBox(10, errorLabel, retryButton);
    dataPropertiesGrid.add(errorBox, 0, 0, 2, 1);
}
```

---

## 7. 接口设计

### 7.1 事件接口

#### 7.1.1 ChartLayerEvent

| 方法 | 参数 | 返回值 | 说明 |
|------|------|--------|------|
| setLayerId | String | void | 设置图层ID |
| getLayerId | - | String | 获取图层ID |
| setLayerName | String | void | 设置图层名称 |
| getLayerName | - | String | 获取图层名称 |
| setChartPath | String | void | 设置海图路径 |
| getChartPath | - | String | 获取海图路径 |
| setVisible | boolean | void | 设置可见性 |
| isVisible | - | boolean | 获取可见性 |

### 7.2 服务接口

#### 7.2.1 PropertyLoadService

| 方法 | 参数 | 返回值 | 说明 |
|------|------|--------|------|
| loadPropertiesAsync | LayerNode, PropertyLoadCallback | void | 异步加载属性 |
| setChartViewer | ChartViewer | void | 设置海图查看器 |
| shutdown | - | void | 关闭服务 |

#### 7.2.2 PropertyLoadCallback

| 方法 | 参数 | 返回值 | 说明 |
|------|------|--------|------|
| onSuccess | ChartData | void | 加载成功回调 |
| onFailure | Throwable | void | 加载失败回调 |

### 7.3 控制器接口

#### 7.3.1 LayerManagerPanelController

| 方法 | 参数 | 返回值 | 说明 |
|------|------|--------|------|
| addChartLayer | String | void | 添加海图图层 |
| setOnViewProperties | Consumer<LayerNode> | void | 设置查看属性回调 |
| dispose | - | void | 清理资源 |

#### 7.3.2 RightTabManager

| 方法 | 参数 | 返回值 | 说明 |
|------|------|--------|------|
| switchToTab | String | void | 切换到指定标签页 |
| showPropertyPanel | - | void | 显示属性面板 |
| getPropertyPanel | - | PropertyPanel | 获取属性面板实例 |

---

## 8. 非功能性设计

### 8.1 性能设计

| 性能指标 | 目标值 | 实现方法 |
|----------|--------|----------|
| 属性面板切换响应时间 | < 200ms | 事件总线异步分发 |
| 属性加载时间 | < 500ms (小文件) | 后台线程加载 |
| 图层可见性切换响应 | < 100ms | JavaFX属性绑定 |
| 内存占用 | < 50MB (100个图层) | 懒加载属性信息 |

### 8.2 可用性设计

- 右键菜单项清晰明了，用户无需培训即可使用
- 属性加载过程中显示进度提示
- 加载失败时提供重试按钮
- 支持键盘快捷键（如Delete删除图层）

### 8.3 可扩展性设计

- 支持新增图层类型（通过扩展`LayerNodeType`枚举）
- 支持新增属性字段（通过`ChartData.addProperty()`方法）
- 支持新增事件类型（通过扩展`AppEventType`枚举）

### 8.4 可维护性设计

- 模块职责单一，易于理解和修改
- 使用事件总线解耦，降低模块间依赖
- 代码注释完整，遵循JavaDoc规范
- 单元测试覆盖核心逻辑

---

## 9. 测试策略

### 9.1 单元测试

| 测试类 | 测试内容 | 覆盖率目标 |
|--------|----------|------------|
| ChartLayerEventTest | 事件数据读写 | > 90% |
| LayerNodeTest | 属性绑定和事件发布 | > 85% |
| ChartDataTest | 属性获取和格式化 | > 90% |
| PropertyLoadTaskTest | 异步加载逻辑 | > 80% |

### 9.2 集成测试

| 测试场景 | 测试内容 |
|----------|----------|
| 海图加载流程 | 打开海图 → 图层添加 → 属性显示 |
| 可见性切换流程 | 切换可见性 → 事件发布 → 地图重绘 |
| 属性加载流程 | 点击查看属性 → 面板切换 → 异步加载 → 显示属性 |

### 9.3 性能测试

| 测试场景 | 性能指标 |
|----------|----------|
| 连续打开10个海图 | 每个海图加载时间 < 500ms |
| 属性面板切换 | 切换响应时间 < 200ms |
| 100个图层可见性切换 | 切换响应时间 < 100ms |

---

## 10. 风险识别与应对

### 10.1 技术风险

| 风险ID | 风险描述 | 风险等级 | 影响范围 | 应对措施 | 责任人 |
|--------|----------|----------|----------|----------|--------|
| TR-001 | 事件总线性能瓶颈 | 中 | 系统响应速度 | 1. 限制单个事件订阅者数量（<50）<br>2. 使用弱引用避免内存泄漏<br>3. 异步事件分发 | 架构师 |
| TR-002 | 异步任务线程池耗尽 | 中 | 属性加载 | 1. 限制线程池大小（2个线程）<br>2. 任务队列管理<br>3. 任务超时控制（30秒） | 开发工程师 |
| TR-003 | JavaFX属性绑定内存泄漏 | 高 | 内存占用 | 1. 及时解绑不再使用的属性<br>2. 使用WeakChangeListener<br>3. 定期内存分析 | 开发工程师 |
| TR-004 | 大型海图文件加载慢 | 中 | 用户体验 | 1. 异步加载属性信息<br>2. 显示加载进度<br>3. 支持取消加载操作 | 开发工程师 |
| TR-005 | 事件订阅未及时清理 | 高 | 内存泄漏 | 1. 在dispose()中清理订阅<br>2. 使用try-finally确保清理<br>3. 单元测试验证清理逻辑 | 开发工程师 |

### 10.2 资源风险

| 风险ID | 风险描述 | 风险等级 | 影响范围 | 应对措施 | 责任人 |
|--------|----------|----------|----------|----------|--------|
| RR-001 | 内存占用过高 | 中 | 系统稳定性 | 1. 懒加载属性信息<br>2. 限制缓存大小<br>3. 及时释放不用的资源 | 开发工程师 |
| RR-002 | CPU使用率过高 | 低 | 系统性能 | 1. 异步任务避免阻塞UI线程<br>2. 限制并发任务数量<br>3. 使用性能分析工具监控 | 开发工程师 |
| RR-003 | 磁盘空间不足 | 低 | 数据存储 | 1. 监控磁盘空间使用<br>2. 提供清理临时文件功能<br>3. 用户提示机制 | 开发工程师 |

### 10.3 时间风险

| 风险ID | 风险描述 | 风险等级 | 影响范围 | 应对措施 | 责任人 |
|--------|----------|----------|----------|----------|--------|
| SR-001 | 开发周期延误 | 中 | 项目进度 | 1. 分阶段交付<br>2. 优先实现核心功能<br>3. 每日进度跟踪 | 项目经理 |
| SR-002 | 依赖组件交付延迟 | 低 | 集成测试 | 1. 提前识别依赖项<br>2. 使用Mock对象测试<br>3. 并行开发 | 项目经理 |
| SR-003 | 需求变更频繁 | 中 | 开发效率 | 1. 需求评审机制<br>2. 变更控制流程<br>3. 影响评估 | 产品经理 |

### 10.4 风险监控机制

- **每日检查**: 开发进度、技术难点
- **每周评审**: 风险状态更新、应对措施执行情况
- **里程碑检查**: 关键功能完成度、质量指标达成情况

---

## 11. 部署架构

### 11.1 开发环境配置

| 配置项 | 要求 | 说明 |
|--------|------|------|
| JDK版本 | JDK 8 或 JDK 11 | 推荐使用JDK 11 |
| IDE | IntelliJ IDEA 2020+ | 或 Eclipse 2020+ |
| 构建工具 | Maven 3.6+ | 或 Gradle 6.0+ |
| JavaFX SDK | JavaFX 11+ | 如使用JDK 11需单独安装 |
| 内存要求 | 最低4GB，推荐8GB | 开发环境 |
| 操作系统 | Windows 10/11, macOS 10.14+, Linux | 跨平台支持 |

**开发环境搭建步骤**:

1. 安装JDK 11
   ```bash
   # Windows
   choco install openjdk11
   
   # macOS
   brew install openjdk@11
   
   # Linux
   sudo apt-get install openjdk-11-jdk
   ```

2. 安装JavaFX SDK
   ```bash
   # Maven依赖
   <dependency>
       <groupId>org.openjfx</groupId>
       <artifactId>javafx-controls</artifactId>
       <version>11</version>
   </dependency>
   ```

3. 配置IDE
   - 设置JDK路径
   - 配置JavaFX SDK路径
   - 安装Lombok插件（可选）

### 11.2 测试环境配置

| 配置项 | 要求 | 说明 |
|--------|------|------|
| 测试框架 | JUnit 5 | 单元测试 |
| Mock框架 | Mockito 3+ | 模拟依赖 |
| UI测试 | TestFX 4+ | JavaFX UI测试 |
| 测试数据 | 标准S57/S63海图文件 | 测试资源 |
| CI/CD | Jenkins/GitHub Actions | 持续集成 |

**测试环境搭建步骤**:

1. 配置Maven测试依赖
   ```xml
   <dependency>
       <groupId>org.junit.jupiter</groupId>
       <artifactId>junit-jupiter</artifactId>
       <version>5.7.0</version>
       <scope>test</scope>
   </dependency>
   <dependency>
       <groupId>org.mockito</groupId>
       <artifactId>mockito-core</artifactId>
       <version>3.7.0</version>
       <scope>test</scope>
   </dependency>
   <dependency>
       <groupId>org.testfx</groupId>
       <artifactId>testfx-core</artifactId>
       <version>4.0.16-alpha</version>
       <scope>test</scope>
   </dependency>
   ```

2. 配置测试数据
   - 准备标准测试海图文件
   - 配置测试数据路径
   - 设置测试环境变量

### 11.3 生产环境配置

| 配置项 | 要求 | 说明 |
|--------|------|------|
| JRE版本 | JRE 8 或 JRE 11 | 运行时环境 |
| JVM参数 | -Xms256m -Xmx1024m | 内存配置 |
| 部署方式 | 可执行JAR或安装包 | Windows: exe, macOS: dmg |
| 日志配置 | Logback配置文件 | 日志级别、格式、存储 |
| 配置文件 | application.properties | 外部配置 |

**生产环境部署步骤**:

1. 打包应用
   ```bash
   # Maven打包
   mvn clean package
   
   # 生成可执行JAR
   java -jar target/echart-app-1.0.0.jar
   ```

2. 配置JVM参数
   ```bash
   # Windows
   java -Xms256m -Xmx1024m -jar echart-app.jar
   
   # macOS/Linux
   java -Xms256m -Xmx1024m -jar echart-app.jar
   ```

3. 生成安装包
   ```bash
   # 使用jpackage生成安装包
   jpackage --name EChartApp --input target --main-jar echart-app-1.0.0.jar --main-class cn.cycle.echart.App
   ```

### 11.4 环境差异说明

| 环境类型 | 配置差异 | 说明 |
|----------|----------|------|
| 开发环境 | 日志级别DEBUG | 详细日志输出 |
| 测试环境 | 日志级别INFO | 关键信息日志 |
| 生产环境 | 日志级别WARN | 仅警告和错误日志 |

---

## 12. 监控与运维

### 12.1 日志策略

#### 12.1.1 日志级别定义

| 日志级别 | 使用场景 | 示例 |
|----------|----------|------|
| ERROR | 系统错误、异常 | 文件加载失败、空指针异常 |
| WARN | 潜在问题、性能警告 | 内存使用率>80%、加载时间>1s |
| INFO | 关键业务操作 | 海图加载成功、图层添加、属性查看 |
| DEBUG | 调试信息 | 事件发布、属性绑定、异步任务状态 |

#### 12.1.2 日志格式

```
[%d{yyyy-MM-dd HH:mm:ss.SSS}] [%thread] %-5level %logger{36} - %msg%n
```

#### 12.1.3 日志配置（logback.xml）

```xml
<configuration>
    <appender name="FILE" class="ch.qos.logback.core.rolling.RollingFileAppender">
        <file>logs/echart-app.log</file>
        <rollingPolicy class="ch.qos.logback.core.rolling.TimeBasedRollingPolicy">
            <fileNamePattern>logs/echart-app.%d{yyyy-MM-dd}.log</fileNamePattern>
            <maxHistory>30</maxHistory>
            <totalSizeCap>1GB</totalSizeCap>
        </rollingPolicy>
        <encoder>
            <pattern>[%d{yyyy-MM-dd HH:mm:ss.SSS}] [%thread] %-5level %logger{36} - %msg%n</pattern>
        </encoder>
    </appender>
    
    <appender name="CONSOLE" class="ch.qos.logback.core.ConsoleAppender">
        <encoder>
            <pattern>%d{HH:mm:ss.SSS} %-5level %logger{36} - %msg%n</pattern>
        </encoder>
    </appender>
    
    <root level="INFO">
        <appender-ref ref="FILE" />
        <appender-ref ref="CONSOLE" />
    </root>
    
    <logger name="cn.cycle.echart" level="DEBUG" />
</configuration>
```

#### 12.1.4 关键日志点

| 操作 | 日志级别 | 日志内容 |
|------|----------|----------|
| 海图加载 | INFO | "海图加载成功: {chartPath}" |
| 图层添加 | INFO | "图层添加: {layerId}, {layerName}" |
| 属性查看 | INFO | "查看属性: {layerId}" |
| 事件发布 | DEBUG | "发布事件: {eventType}, {data}" |
| 异步任务开始 | DEBUG | "开始异步任务: {taskId}" |
| 异步任务完成 | DEBUG | "异步任务完成: {taskId}, 耗时: {time}ms" |
| 异常捕获 | ERROR | "异常: {exception}", exception |

### 12.2 告警规则

#### 12.2.1 系统告警

| 告警类型 | 触发条件 | 告警级别 | 处理措施 |
|----------|----------|----------|----------|
| 内存告警 | 内存使用率 > 80% | 警告 | 检查内存泄漏，清理缓存 |
| 内存告警 | 内存使用率 > 90% | 严重 | 重启应用，分析内存dump |
| CPU告警 | CPU使用率 > 80%持续5分钟 | 警告 | 检查异步任务，优化算法 |
| 磁盘告警 | 磁盘空间 < 1GB | 警告 | 清理临时文件，日志文件 |

#### 12.2.2 业务告警

| 告警类型 | 触发条件 | 告警级别 | 处理措施 |
|----------|----------|----------|----------|
| 海图加载失败 | 连续3次加载失败 | 警告 | 检查文件路径、文件格式 |
| 属性加载超时 | 加载时间 > 10秒 | 警告 | 检查文件大小，优化加载逻辑 |
| 事件总线积压 | 事件队列 > 100 | 警告 | 检查订阅者处理速度 |
| 线程池耗尽 | 所有线程忙碌 | 严重 | 增加线程池大小，优化任务 |

#### 12.2.3 告警通知方式

- **警告级别**: 日志记录 + 状态栏提示
- **严重级别**: 日志记录 + 弹窗提示 + 声音提醒

### 12.3 性能监控指标

#### 12.3.1 关键性能指标（KPI）

| 指标名称 | 目标值 | 监控方式 | 采集频率 |
|----------|--------|----------|----------|
| 应用启动时间 | < 5秒 | 日志时间戳 | 每次启动 |
| 海图加载时间 | < 500ms | 日志时间戳 | 每次加载 |
| 属性面板切换时间 | < 200ms | 日志时间戳 | 每次切换 |
| 内存占用 | < 100MB | JVM监控 | 每分钟 |
| CPU使用率 | < 30% | JVM监控 | 每分钟 |
| 事件处理延迟 | < 50ms | 日志时间戳 | 每次事件 |

#### 12.3.2 性能监控实现

```java
public class PerformanceMonitor {
    private static final Logger logger = LoggerFactory.getLogger(PerformanceMonitor.class);
    
    public static void logPerformance(String operation, long startTime) {
        long duration = System.currentTimeMillis() - startTime;
        
        if (duration > 1000) {
            logger.warn("性能警告: {} 耗时 {}ms", operation, duration);
        } else {
            logger.debug("性能统计: {} 耗时 {}ms", operation, duration);
        }
    }
    
    public static void monitorMemory() {
        Runtime runtime = Runtime.getRuntime();
        long usedMemory = runtime.totalMemory() - runtime.freeMemory();
        long maxMemory = runtime.maxMemory();
        double usagePercent = (double) usedMemory / maxMemory * 100;
        
        if (usagePercent > 80) {
            logger.warn("内存使用率: {:.2f}%, 已用: {}MB, 最大: {}MB", 
                usagePercent, usedMemory / 1024 / 1024, maxMemory / 1024 / 1024);
        }
    }
}
```

#### 12.3.3 性能数据采集

- **启动时**: 记录启动时间
- **运行时**: 定期采集内存、CPU使用率
- **操作时**: 记录关键操作耗时
- **异常时**: 记录异常堆栈和性能数据

### 12.4 运维支持

#### 12.4.1 问题诊断工具

| 工具 | 用途 | 使用场景 |
|------|------|----------|
| 日志分析 | 问题定位 | 查看ERROR日志，分析异常堆栈 |
| JVM监控 | 性能分析 | 查看内存、CPU、线程状态 |
| 内存分析 | 内存泄漏 | 分析heap dump，查找大对象 |
| 线程分析 | 死锁排查 | 查看线程堆栈，识别阻塞线程 |

#### 12.4.2 常见问题处理

| 问题现象 | 可能原因 | 处理步骤 |
|----------|----------|----------|
| 应用启动慢 | 类加载多、初始化复杂 | 1. 检查日志<br>2. 优化初始化逻辑<br>3. 延迟加载非必要组件 |
| 内存占用高 | 内存泄漏、缓存过大 | 1. 分析heap dump<br>2. 检查事件订阅清理<br>3. 清理缓存 |
| UI卡顿 | 长时间操作阻塞UI线程 | 1. 检查日志中的耗时操作<br>2. 改为异步执行<br>3. 优化算法 |
| 海图加载失败 | 文件损坏、格式不支持 | 1. 检查文件路径<br>2. 验证文件格式<br>3. 查看错误日志 |

---

## 13. 需求追溯矩阵

| 需求ID | 需求描述 | 设计章节 | 实现模块 | 接口 | 测试用例 | 状态 |
|--------|----------|----------|----------|------|----------|------|
| F001 | 打开海图添加节点 | 4.1, 4.2, 4.3 | LayerManagerPanelController | addChartLayer | TC001 | 📋 待实现 |
| F002 | 右键菜单功能 | 4.3 | LayerManagerPanelController | onViewProperties, onToggleVisibility, onRename | TC002 | 📋 待实现 |
| F003 | 查看属性功能 | 4.3, 4.4 | LayerManagerPanelController, RightTabManager | onViewProperties, switchToTab | TC003 | 📋 待实现 |
| F004 | 属性显示功能 | 4.5, 5.2 | PropertyLoadService, PropertyPanelController | loadPropertiesAsync, refreshDisplay | TC004 | 📋 待实现 |
| F005 | 可见性控制 | 4.2, 5.1 | LayerNode, LayerTreeCell | visibleProperty, updateLayerVisibility | TC005 | 📋 待实现 |
| F006 | 重命名功能 | 4.3 | LayerManagerPanelController | onRename | TC006 | 📋 待实现 |

---

## 14. 实施计划

### 14.1 第一阶段：核心功能（P0）

**预计工时**: 16小时

| 任务 | 工时 | 依赖 |
|------|------|------|
| 实现ChartLayerEvent和扩展AppEventType | 2h | 无 |
| 增强LayerNode支持JavaFX属性 | 3h | 无 |
| 实现ChartData数据模型 | 2h | 无 |
| 增强LayerManagerPanelController支持事件订阅 | 4h | T1, T2, T3 |
| 实现PropertyLoadService和PropertyLoadTask | 3h | T3 |
| 集成测试 | 2h | T4, T5 |

### 14.2 第二阶段：增强功能（P1）

**预计工时**: 8小时

| 任务 | 工时 | 依赖 |
|------|------|------|
| 实现LayerTreeCell自定义单元格 | 3h | 第一阶段 |
| 实现图层可见性绑定 | 2h | T7 |
| 增强RightTabManager支持事件切换 | 2h | 第一阶段 |
| 集成测试 | 1h | T8, T9 |

### 14.3 第三阶段：优化功能（P2）

**预计工时**: 4小时

| 任务 | 工时 | 依赖 |
|------|------|------|
| 实现进度反馈和错误处理 | 2h | 第二阶段 |
| 性能优化和测试 | 2h | T11 |

---

## 15. 附录

### 15.1 术语表

| 术语 | 说明 |
|------|------|
| EventBus | 事件总线，实现发布/订阅模式的事件分发机制 |
| JavaFX Property | JavaFX属性，支持数据绑定和监听器 |
| Task | JavaFX异步任务，在后台线程执行 |
| LayerNode | 图层节点数据模型 |
| ChartData | 海图数据模型 |

### 15.2 参考资源

- [JavaFX Property Binding](https://openjfx.io/javadoc/11/javafx.base/javafx/beans/property/package-summary.html)
- [JavaFX Concurrency](https://openjfx.io/javadoc/11/javafx.graphics/javafx/concurrent/package-summary.html)
- [Event Bus Pattern](https://martinfowler.com/articles/201701-event-driven.html)

### 15.3 变更历史

| 版本 | 日期 | 变更内容 | 作者 |
|------|------|----------|------|
| v1.0 | 2026-04-24 | 初始版本 | Cycle Team |

---

**文档维护**: Cycle Team  
**技术支持**: 参见项目 README.md
