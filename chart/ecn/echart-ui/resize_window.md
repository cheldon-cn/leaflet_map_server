# 窗口拖动调整大小问题总结

> 本文档记录了在实现 UNDECORATED 窗口边缘拖动调整大小功能时遇到的问题及解决方案。

---

## 一、背景

JavaFX 的 `StageStyle.UNDECORATED` 样式会移除窗口的系统边框和标题栏，导致无法通过拖动边缘来调整窗口大小。需要自行实现此功能。

---

## 二、问题列表

### 问题1：窗口边界不能拖动调整大小

**现象**：
- 鼠标移动到窗口边缘，无法拖动调整窗口大小
- 没有边缘检测和调整大小的功能

**原因**：
- UNDECORATED 窗口没有系统边框，需要自己实现调整大小功能

**解决方案**：
创建 `WindowResizer` 类，使用 Scene 级别的事件过滤器捕获鼠标事件。

```java
public class WindowResizer {
    private static final int RESIZE_MARGIN = 8;
    
    public WindowResizer(Stage stage, Scene scene) {
        this.stage = stage;
        this.scene = scene;
        setupResizeHandler();
    }
    
    private void setupResizeHandler() {
        scene.addEventFilter(MouseEvent.MOUSE_PRESSED, pressHandler);
        scene.addEventFilter(MouseEvent.MOUSE_DRAGGED, dragHandler);
        scene.addEventFilter(MouseEvent.MOUSE_MOVED, moveHandler);
        scene.addEventFilter(MouseEvent.MOUSE_RELEASED, releaseHandler);
    }
}
```

---

### 问题2：边缘检测失效，事件被子组件拦截

**现象**：
- 鼠标移动到左、右、下边缘，光标不会变成箭头
- 上边缘会变箭头，但拖动时窗口变动不跟鼠标

**原因**：
- 初始实现使用 `Region` 作为边缘检测区域
- 子组件（如按钮、标题栏）会拦截鼠标事件，导致边缘检测失效

**解决方案**：
改用 Scene 级别的事件过滤器（`addEventFilter`），在事件分发到子组件之前捕获。

```java
// 错误方式：使用 Region 作为边缘检测区域
edgeRegion.setOnMousePressed(event -> { ... });  // 事件被子组件拦截

// 正确方式：使用 Scene 级别的事件过滤器
scene.addEventFilter(MouseEvent.MOUSE_PRESSED, event -> {
    // 在事件分发之前处理
});
```

---

### 问题3：竖直方向拖动敏感度过高

**现象**：
- 竖直方向上，变成箭头后，只要按下鼠标稍微移动
- 窗口高度就立马变动很大，超出桌面显示范围

**原因**：
- 位置计算方式错误
- 直接使用鼠标移动距离 `deltaY` 来计算新位置
- 没有考虑窗口大小变化的约束

**错误代码**：
```java
case N:
    newHeight = Math.max(minHeight, startHeight - deltaY);
    if (newHeight > minHeight) {
        newY = startWindowY + deltaY;  // ❌ 错误：直接使用鼠标移动距离
    }
    break;
```

**解决方案**：
使用正确的几何关系计算新位置。当从上边缘调整大小时，窗口底部位置应保持不变：

```
newY + newHeight = startWindowY + startHeight
newY = startWindowY + startHeight - newHeight
```

**正确代码**：
```java
case N:
    newHeight = Math.max(minHeight, startHeight - deltaY);
    newY = startWindowY + startHeight - newHeight;  // ✓ 正确
    break;
```

---

### 问题4：水平方向拖动导致窗口向下跳动

**现象**：
- 水平方向拖动窗口会向下跳动
- 然后水平方向拖动正常

**原因**：
- `TitleBar` 和 `WindowResizer` 同时处理鼠标事件
- 两个处理器都修改窗口位置，导致冲突

**解决方案**：
在 `TitleBar` 中检查事件是否已被消费：

```java
// TitleBar.java
setOnMousePressed(event -> {
    if (event.isConsumed()) {
        return;  // 事件已被 WindowResizer 处理，跳过
    }
    // ... 处理拖动逻辑
});

setOnMouseDragged(event -> {
    if (event.isConsumed() || !dragging) {
        return;  // 事件已被消费，跳过
    }
    // ... 处理拖动逻辑
});
```

---

### 问题5：窗口变动量远大于鼠标拖动量

**现象**：
- 向上拖动：窗口向上跳，竖直方向窗口变动量远大于鼠标拖动量
- 向下拖动：窗口向下跳，竖直方向窗口变动量远大于鼠标拖动量
- 窗口超出桌面显示范围

**调试日志分析**：
```
startHeight=612.0, deltaY=-4.0
calc: startHeight - deltaY = 616.0      # 计算正确
newHeight=768.0, newY=-102.0            # 结果错误！
```

**原因**：
- `stage.setMinHeight(768)` 设置的最小高度大于当前窗口高度 612
- `Math.max(768, 612)` 总是返回 768
- 导致窗口高度被强制设为最小高度

**错误配置**：
```java
// EChartApp.java
stage.setMinWidth(1024);
stage.setMinHeight(768);  // ❌ 最小高度大于实际窗口高度
```

**解决方案**：
调整最小高度设置，使其小于或等于实际窗口高度：

```java
// EChartApp.java
stage.setMinWidth(800);
stage.setMinHeight(600);  // ✓ 合理的最小高度
```

---

## 三、关键技术点

### 3.1 事件过滤器 vs 事件处理器

| 类型 | 执行顺序 | 适用场景 |
|------|----------|----------|
| `addEventFilter` | 事件分发之前 | 需要优先捕获事件，防止被子组件拦截 |
| `setOnXxx` | 事件分发到目标后 | 处理目标组件自身的事件 |

### 3.2 窗口调整大小的几何关系

```
从上边缘调整（N方向）：
- 底部位置不变：newY + newHeight = startWindowY + startHeight
- 新位置：newY = startWindowY + startHeight - newHeight

从左边缘调整（W方向）：
- 右侧位置不变：newX + newWidth = startWindowX + startWidth
- 新位置：newX = startWindowX + startWidth - newWidth
```

### 3.3 事件消费机制

```java
event.consume();  // 标记事件已处理

// 其他处理器检查
if (event.isConsumed()) {
    return;  // 跳过处理
}
```

---

## 四、相关文件

| 文件 | 说明 |
|------|------|
| `WindowResizer.java` | 窗口边缘调整大小处理器 |
| `TitleBar.java` | 标题栏，处理窗口拖动移动 |
| `MainView.java` | 主视图，初始化 WindowResizer |
| `EChartApp.java` | 应用入口，配置窗口最小尺寸 |

---

## 五、最佳实践

1. **使用 Scene 级别事件过滤器**：确保在事件分发之前捕获，避免被子组件拦截

2. **正确计算位置**：使用几何关系而非直接使用鼠标移动距离

3. **检查事件消费状态**：避免多个处理器同时修改窗口状态

4. **合理设置最小尺寸**：最小尺寸应小于或等于初始窗口尺寸

5. **添加调试日志**：在开发阶段添加详细日志，便于定位问题

---

**版本**: v1.0  
**更新日期**: 2026-04-21
