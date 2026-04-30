# 应用界面面板布局说明

**版本**: v1.2  
**更新日期**: 2026-04-30

---

## 术语定义

| 术语 | 定义 |
|------|------|
| contentPane | 侧边栏内容面板，包含图层面板、搜索面板等具体内容 |
| Button Bar | 侧边栏按钮栏，包含图层、搜索、航线等图标按钮 |

---

## 一、整体布局的组件层次

采用 JavaFX 的 BorderPane 作为根容器（Root）。该容器将窗口划分为五个区域：顶部（Top）、底部（Bottom）、左侧（Left）、右侧（Right）和中央（Center）。

### 1.1 布局常量定义

| 常量名 | 值 | 说明 |
|--------|-----|------|
| `SIDEBAR_BUTTON_WIDTH` | 40px | 侧边栏按钮栏宽度 |
| `SIDEBAR_CONTENT_WIDTH` | 280px | 侧边栏内容面板宽度（可调整） |
| `RIGHT_PANEL_WIDTH` | 300px | 右侧面板宽度（可调整） |
| `TitleBar高度` | 32px | 标题栏高度 |
| `StatusBar高度` | 28px | 状态栏高度 |
| `MIN_WINDOW_WIDTH` | 800px | 窗口最小宽度 |
| `MIN_WINDOW_HEIGHT` | 600px | 窗口最小高度 |

### 1.2 各区域组件

**顶部区域**：一个垂直排列的 VBox，内部从上到下包含：
- **TitleBar**：高度 32px，用于显示应用图标、标题和窗口控制按钮
- **RibbonMenuBar**：展示选项卡式菜单（如"开始"、"插入"、"数据"等）

**底部区域**：StatusBar（HBox），高度固定为 28px，用于显示状态信息、进度或用户提示

**左侧区域**：SideBarManager，包含：
- **按钮栏（Button Bar）**：固定宽度 SIDEBAR_BUTTON_WIDTH（40px）
- **可展开/收起的内容面板（contentPane）**：宽度 SIDEBAR_CONTENT_WIDTH（280px）

**右侧区域**：RightTabManager，宽度 RIGHT_PANEL_WIDTH（300px）

**中央区域**：ChartDisplayArea，占据除上述四边区域外的所有剩余空间

### 1.3 初始布局示意

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

---

## 二、各区域的交互行为与动态控制

### 2.1 面板的显示与隐藏

在运维场景中，用户可能需要临时隐藏左侧图层面板或右侧选项卡面板以扩大地图显示范围。推荐方案通过 `setVisible()` 和 `setManaged()` 两个方法联合控制：

1. 调用 `layerPanel.setVisible(false)` 使其不可见
2. 同时调用 `layerPanel.setManaged(false)` 使其不再参与父容器的布局计算

此时，左侧区域只剩下按钮栏（宽度 SIDEBAR_BUTTON_WIDTH），BorderPane 的中央区域会自动向右扩展，占据原本图层面板占用的 SIDEBAR_CONTENT_WIDTH 宽度，实现全幅地图显示。

右侧面板的隐藏同理。恢复显示时，只需将两个标志设回 `true`，布局会自动还原。

### 2.2 面板宽度的手动调整

#### 左侧区域（按钮栏 + 图层面板）

左侧区域使用水平 SplitPane 包裹按钮栏（Button Bar）和图层面板（Layer Panel）。

- 按钮栏宽度固定为 SIDEBAR_BUTTON_WIDTH（40px），不可通过分割条调整
- 图层面板的宽度初始为 0（即完全隐藏），此时界面中只显示按钮栏，地图区域占据剩余全部空间

**拖动分割条的交互逻辑**：

- **初始状态**：分割条紧贴在按钮栏右侧。当用户向右侧（即地图区域方向）拖动分割条时，图层面板会自动弹出，并赋予一个默认宽度（SIDEBAR_CONTENT_WIDTH = 280px），此时面板完整显示
- **继续向右拖动**：图层面板宽度增大，地图显示区域被压缩（变窄）
- **向左拖动**：图层面板宽度减小，地图显示区域扩大。当向左拖过面板的最小宽度（如 80px）后，面板可被完全关闭（宽度归零），此时分割条再次回到按钮栏右侧

**技术实现要点**：

- 设置 `SplitPane.setResizableWithParent(buttonBar, false)` 固定按钮栏宽度
- 监听分割条位置变化，当位置值小于某阈值（如 0.02）时，程序化设置图层面板的 `visible` 和 `managed` 为 `false`；当用户再次拖动使位置值大于阈值时，恢复面板显示并设置其宽度为默认值（SIDEBAR_CONTENT_WIDTH）
- 图层面板的最小宽度可通过 `setMinWidth()` 限制，防止被过度压缩

#### 右侧区域（右侧选项卡面板）

右侧区域同样使用水平 SplitPane 包裹右侧选项卡面板（RightTabManager）。

- 面板初始宽度为 RIGHT_PANEL_WIDTH（300px），显示在窗口右侧
- 分割条位于面板左侧，**向左拖动分割条时**，右侧面板宽度增大，地图显示区域被压缩；**向右拖动分割条时**，右侧面板宽度减小，地图显示区域扩大
- 当面板宽度减至最小值后，继续向右拖动不会隐藏面板，面板保持最小宽度

**技术实现要点**：

- 将右侧面板单独放入 SplitPane（只有一个子节点），分割条位于面板左侧
- 通过监听分割条位置，动态开关面板的 `visible/managed`，并记录上一次的宽度值以便恢复
- 支持记忆用户调整后的宽度，下次启动时读取 Preferences 中的设置

---

## 三、布局变化详解

### 3.1 侧边栏展开时布局变化

| 组件 | 折叠状态 | 展开状态 | 变化说明 |
|------|----------|----------|----------|
| **SideBarManager** | 宽度=SIDEBAR_BUTTON_WIDTH | 宽度=SIDEBAR_BUTTON_WIDTH+SIDEBAR_CONTENT_WIDTH | contentPane变为可见可管理 |
| **ChartDisplayArea** | 宽度=窗口-SIDEBAR_BUTTON_WIDTH-RIGHT_PANEL_WIDTH | 宽度=窗口-SIDEBAR_BUTTON_WIDTH-SIDEBAR_CONTENT_WIDTH-RIGHT_PANEL_WIDTH | 宽度减小SIDEBAR_CONTENT_WIDTH |
| **RightTabManager** | 位置靠右 | 位置靠右 | 宽度不变，左侧边界右移 |
| **TitleBar** | 无变化 | 无变化 | 不受侧边栏影响 |
| **StatusBar** | 无变化 | 无变化 | 不受侧边栏影响 |

**展开后布局示意**：

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

### 3.2 侧边栏折叠时布局变化

| 组件 | 展开状态 | 折叠状态 | 变化说明 |
|------|----------|----------|----------|
| **SideBarManager** | 宽度=SIDEBAR_BUTTON_WIDTH+SIDEBAR_CONTENT_WIDTH | 宽度=SIDEBAR_BUTTON_WIDTH | contentPane变为不可见不可管理 |
| **ChartDisplayArea** | 宽度=窗口-SIDEBAR_BUTTON_WIDTH-SIDEBAR_CONTENT_WIDTH-RIGHT_PANEL_WIDTH | 宽度=窗口-SIDEBAR_BUTTON_WIDTH-RIGHT_PANEL_WIDTH | 宽度增加SIDEBAR_CONTENT_WIDTH |
| **RightTabManager** | 位置靠右 | 位置靠右 | 宽度不变，左侧边界左移 |
| **TitleBar** | 无变化 | 无变化 | 不受侧边栏影响 |
| **StatusBar** | 无变化 | 无变化 | 不受侧边栏影响 |

### 3.3 右侧面板显示/隐藏时布局变化

| 组件 | 显示右侧面板 | 隐藏右侧面板 |
|------|--------------|--------------|
| **SideBarManager** | 无变化 | 无变化 |
| **ChartDisplayArea** | 宽度减小 | 宽度增加 |
| **RightTabManager** | 添加到SplitPane | 从SplitPane移除 |

**显示右侧面板布局示意**：

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

**隐藏右侧面板布局示意**：

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

---

## 四、布局的自适应机制

### 4.1 基本自适应规则

1. 当左侧面板宽度发生变化（无论是由用户拖动分割条，还是通过代码动态隐藏），BorderPane 会自动重新计算 center 区域的可用宽度，并立即填充

2. 当右侧面板宽度变化时，中央区域同样会自动伸缩

3. 状态栏固定在窗口底部，即使窗口高度变化，中央区域的高度也会自动调整（减去顶部和底部区域的高度）

4. 整个布局不需要任何手动绑定或事件监听来刷新大小，完全依赖 BorderPane 和 SplitPane 的内置布局算法

### 4.2 响应式布局策略

| 屏幕宽度 | 布局策略 | 说明 |
|----------|----------|------|
| ≥1280px | 完整布局 | 所有面板正常显示 |
| 1024px~1280px | 紧凑布局 | 右侧面板默认隐藏，侧边栏默认折叠 |
| <1024px | 最小布局 | 仅显示ChartDisplayArea和StatusBar |

### 4.3 窗口尺寸约束

```java
stage.setMinWidth(MIN_WINDOW_WIDTH);   // 800px
stage.setMinHeight(MIN_WINDOW_HEIGHT); // 600px
```

窗口resize时，各面板宽度按比例调整，但不会小于各自的最小宽度限制。

---

## 五、边界情况处理

### 5.1 面板宽度极限值处理

| 场景 | 处理方式 |
|------|----------|
| 侧边栏拖动到最小宽度以下 | 自动折叠，设置visible=false, managed=false |
| 右侧面板拖动到最小宽度 | 保持最小宽度，不再缩小 |
| 面板展开时空间不足 | 优先保证ChartDisplayArea最小宽度(400px) |

### 5.2 窗口resize响应

| 场景 | 处理方式 |
|------|----------|
| 窗口宽度减小 | 各面板按比例压缩，触发最小宽度约束后优先压缩ChartDisplayArea |
| 窗口宽度增大 | 各面板按比例扩展，保持用户设置的相对比例 |
| 窗口高度变化 | ChartDisplayArea高度自动调整，面板高度不变 |

### 5.3 多显示器切换

| 场景 | 处理方式 |
|------|----------|
| 从高DPI切换到低DPI | 自动缩放，保持相对比例 |
| 从大屏切换到小屏 | 触发响应式布局策略 |
| DPI变化 | JavaFX自动处理，无需额外代码 |

---

## 六、核心代码示例

### 6.1 基础布局结构

```java
public class MainView extends BorderPane {
    private static final double SIDEBAR_BUTTON_WIDTH = 40;
    private static final double SIDEBAR_CONTENT_WIDTH = 280;
    private static final double RIGHT_PANEL_WIDTH = 300;
    
    private final SplitPane centerSplitPane;
    private final SideBarManager sideBarManager;
    private final ChartDisplayArea chartDisplayArea;
    private final RightTabManager rightTabManager;
    
    public MainView() {
        centerSplitPane = new SplitPane();
        centerSplitPane.getItems().addAll(
            sideBarManager, 
            chartDisplayArea, 
            rightTabManager
        );
        setCenter(centerSplitPane);
        setTop(createTopContainer());
        setBottom(createStatusBar());
    }
}
```

### 6.2 分割条位置监听

```java
private void setupDividerListener() {
    centerSplitPane.getDividers().get(0).positionProperty().addListener(
        (obs, oldVal, newVal) -> {
            double threshold = SIDEBAR_BUTTON_WIDTH / getWidth();
            if (newVal.doubleValue() < threshold) {
                sideBarManager.collapsePanel();
            }
        }
    );
}
```

### 6.3 面板显示/隐藏控制

```java
public void toggleSideBar() {
    if (sideBarManager.isExpanded()) {
        contentPane.setVisible(false);
        contentPane.setManaged(false);
        sideBarManager.setPrefWidth(SIDEBAR_BUTTON_WIDTH);
    } else {
        contentPane.setVisible(true);
        contentPane.setManaged(true);
        sideBarManager.setPrefWidth(SIDEBAR_BUTTON_WIDTH + SIDEBAR_CONTENT_WIDTH);
    }
}
```

---

## 七、用户偏好存储与恢复

为了让用户体验保持连续性（例如下次启动应用时恢复上次关闭时各面板的宽度和隐藏状态），推荐结合 `java.util.prefs.Preferences` API 进行持久化：

### 7.1 保存的内容

- 左侧 SplitPane 的分隔条位置（即图层面板宽度）
- 右侧 SplitPane 的分隔条位置（即右侧面板宽度）
- 左侧图层面板的显示/隐藏状态
- 右侧面板的显示/隐藏状态

### 7.2 保存时机

在窗口关闭时触发保存，或者监听分隔条位置变化事件实时保存：

```java
splitPane.getDividers().get(0).positionProperty().addListener(...)
```

### 7.3 恢复时机

在应用启动、布局初始化完成后，读取存储的值并应用：

- 调用 `splitPane.setDividerPositions(storedPosition)` 恢复分割条位置
- 调用 `setVisible(storedFlag)` 和 `setManaged(storedFlag)` 恢复面板显示状态

通过这种方式，用户的个性化布局设置可以跨会话保留。

---

## 八、与"左侧按钮栏独立于 SplitPane 外"方案的区别

有人可能会问：为什么不直接让按钮栏作为 `BorderPane.setLeft` 的内容，而图层面板单独用 SplitPane 或放在其他位置？原因在于：

- 将按钮栏和图层面板组合在同一个 SplitPane 内，可以保持两者在视觉和交互上的紧密联系：用户拖拽分割条时，直观地调整"按钮栏与图层面板"的比例
- 如果将按钮栏独立出去，再单独给图层面板添加一个右边框调整条，就需要额外编写事件处理逻辑，增加复杂度和出错可能

因此，推荐方案将按钮栏也纳入 SplitPane，但通过 `setResizableWithParent(false)` 禁用了其在拖拽时的宽度变化，从而实现了"按钮栏宽度不变、图层面板宽度可变"的效果，同时保留了 SplitPane 统一管理分割条的优势。

---

## 九、总结

该推荐方案的核心思想是 **"层次分明、职责单一"**：

| 层次 | 职责 |
|------|------|
| BorderPane | 负责宏观区域的划分（上、下、左、右、中） |
| SplitPane | 负责侧边栏内部的宽度动态调整 |
| HBox / VBox | 负责组内基本排列 |
| setManaged | 实现动态显隐，避免额外的高度/宽度计算代码 |
| Preferences API | 实现用户布局持久化 |

这种设计使得代码结构清晰，易于维护和扩展，且充分复用了 JavaFX 布局容器已有的特性，避免了手动布局计算带来的潜在错误和性能损耗。

---
