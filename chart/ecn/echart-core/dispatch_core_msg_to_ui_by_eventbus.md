# 核心模块消息分发到UI面板方案

> 版本: v1.0  
> 日期: 2026-04-23

---

## 一、需求背景

`LogUtil` 位于 `echart-core` 模块，需要将日志消息发送到 `LogPanel`（位于 `echart-ui` 模块）显示。

**约束条件**：
- `echart-core` 不能依赖 `echart-ui`（会导致循环依赖）
- 日志可能在任意线程产生，需要线程安全
- 需要支持多个日志消费者

---

## 二、方案概述

利用现有的 `AppEventBus` 事件总线机制实现解耦通信：

```
┌──────────────────────────────────────────────────────────────────────────┐
│                           消息流向                                        │
├──────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│   ┌─────────────┐         ┌─────────────┐         ┌─────────────┐       │
│   │   LogUtil   │ publish │ AppEventBus │ dispatch│  LogPanel   │       │
│   │ (echart-core)│ ──────→ │ (echart-core)│ ──────→ │ (echart-ui) │       │
│   └─────────────┘         └─────────────┘         └─────────────┘       │
│         │                       │                       │               │
│         │                       │                       │               │
│         │                       ↓                       │               │
│         │               ┌─────────────┐                 │               │
│         │               │ BlockingQueue│                 │               │
│         │               └─────────────┘                 │               │
│         │                       │                       │               │
│         │                       ↓                       │               │
│         │               ┌─────────────┐                 │               │
│         │               │ 后台消费者线程│                 │               │
│         │               └─────────────┘                 │               │
│         │                       │                       │               │
│         │                       ↓                       │               │
│         │               ┌─────────────┐                 │               │
│         │               │PlatformAdapter│                │               │
│         │               │.runOnUiThread│                 │               │
│         │               └─────────────┘                 │               │
│         │                       │                       │               │
│         └───────────────────────┴───────────────────────┘               │
│                              UI线程执行                                   │
│                                                                          │
└──────────────────────────────────────────────────────────────────────────┘
```

---

## 三、模块依赖关系

```
┌─────────────────────────────────────────────────────────────────┐
│                        模块依赖图                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   echart-ui ──────依赖────→ echart-core                         │
│       │                           │                             │
│       │                           │                             │
│       ↓                           ↓                             │
│   ┌─────────┐               ┌─────────────┐                     │
│   │LogPanel │               │   LogUtil   │                     │
│   │         │               │ AppEventBus │                     │
│   │ 订阅事件 │ ←── 事件 ────│  发布事件   │                     │
│   └─────────┘               └─────────────┘                     │
│                                                                 │
│   ✅ 无循环依赖：echart-core 不依赖 echart-ui                    │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 四、实现步骤

### 4.1 步骤一：添加日志事件类型

**文件**: `ecn/echart-core/src/main/java/cn/cycle/echart/core/AppEventType.java`

**修改内容**: 在枚举中添加日志事件类型

```java
// 日志事件
LOG_DEBUG("log.debug", "调试日志"),
LOG_INFO("log.info", "信息日志"),
LOG_WARN("log.warn", "警告日志"),
LOG_ERROR("log.error", "错误日志");
```

---

### 4.2 步骤二：修改 LogUtil 发布事件

**文件**: `ecn/echart-core/src/main/java/cn/cycle/echart/core/LogUtil.java`

**修改内容**:

```java
public final class LogUtil {

    private static boolean debugEnabled = true;
    private static boolean infoEnabled = true;
    private static boolean warnEnabled = true;
    private static boolean errorEnabled = true;
    private static boolean eventPublishEnabled = true;

    private LogUtil() {
    }

    /**
     * 设置是否发布事件到UI。
     * 
     * @param enabled true启用，false禁用
     */
    public static void setEventPublishEnabled(boolean enabled) {
        eventPublishEnabled = enabled;
    }

    public static void debug(String tag, String message) {
        if (debugEnabled) {
            String formattedMsg = "[" + tag + "] " + message;
            System.out.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_DEBUG, formattedMsg);
        }
    }

    public static void debug(String tag, String format, Object... args) {
        if (debugEnabled) {
            String formattedMsg = "[" + tag + "] " + String.format(format, args);
            System.out.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_DEBUG, formattedMsg);
        }
    }

    public static void info(String tag, String message) {
        if (infoEnabled) {
            String formattedMsg = "[" + tag + "] INFO: " + message;
            System.out.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_INFO, formattedMsg);
        }
    }

    public static void info(String tag, String format, Object... args) {
        if (infoEnabled) {
            String formattedMsg = "[" + tag + "] INFO: " + String.format(format, args);
            System.out.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_INFO, formattedMsg);
        }
    }

    public static void warn(String tag, String message) {
        if (warnEnabled) {
            String formattedMsg = "[" + tag + "] WARN: " + message;
            System.out.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_WARN, formattedMsg);
        }
    }

    public static void warn(String tag, String format, Object... args) {
        if (warnEnabled) {
            String formattedMsg = "[" + tag + "] WARN: " + String.format(format, args);
            System.out.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_WARN, formattedMsg);
        }
    }

    public static void error(String tag, String message) {
        if (errorEnabled) {
            String formattedMsg = "[" + tag + "] ERROR: " + message;
            System.err.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_ERROR, formattedMsg);
        }
    }

    public static void error(String tag, String message, Throwable t) {
        if (errorEnabled) {
            String formattedMsg = "[" + tag + "] ERROR: " + message;
            System.err.println(formattedMsg);
            if (t != null) {
                t.printStackTrace(System.err);
            }
            publishLogEvent(AppEventType.LOG_ERROR, formattedMsg);
        }
    }

    public static void error(String tag, String format, Object... args) {
        if (errorEnabled) {
            String formattedMsg = "[" + tag + "] ERROR: " + String.format(format, args);
            System.err.println(formattedMsg);
            publishLogEvent(AppEventType.LOG_ERROR, formattedMsg);
        }
    }

    /**
     * 发布日志事件到事件总线。
     * 
     * @param type 事件类型
     * @param message 格式化后的消息
     */
    private static void publishLogEvent(AppEventType type, String message) {
        if (!eventPublishEnabled) {
            return;
        }
        try {
            AppEventBus bus = AppEventBus.getInstance();
            if (bus != null && bus.isRunning()) {
                bus.publish(new AppEvent(LogUtil.class, type, message));
            }
        } catch (Exception e) {
            // 避免日志发布失败导致无限循环
            System.err.println("[LogUtil] Failed to publish log event: " + e.getMessage());
        }
    }
}
```

---

### 4.3 步骤三：LogPanel 订阅日志事件

**文件**: `ecn/echart-ui/src/main/java/cn/cycle/echart/ui/panel/LogPanel.java`

**修改内容**:

```java
package cn.cycle.echart.ui.panel;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventBus;
import cn.cycle.echart.core.AppEventType;
import cn.cycle.echart.core.EventHandler;
import cn.cycle.echart.ui.FxRightTabPanel;
import cn.cycle.echart.ui.dialog.ErrorDialog;
import javafx.scene.control.*;
import javafx.scene.layout.BorderPane;
import javafx.stage.FileChooser;

import java.io.File;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

public class LogPanel extends BorderPane implements FxRightTabPanel {

    private static final String TAB_ID = "log-panel";
    private static final DateTimeFormatter TIME_FORMATTER = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");
    
    private final TextArea logArea;
    private final TextField filterField;
    private final Button exportButton;
    private final Button clearButton;
    private final Tab tab;

    public LogPanel() {
        this.logArea = new TextArea();
        this.filterField = new TextField();
        this.exportButton = new Button("导出");
        this.clearButton = new Button("清除");
        this.tab = new Tab("日志");
        this.tab.setContent(this);
        this.tab.setClosable(false);
        
        initializeLayout();
        subscribeLogEvents();
    }

    private void initializeLayout() {
        logArea.setEditable(false);
        logArea.setWrapText(true);
        logArea.setStyle("-fx-font-family: monospace;");
        
        filterField.setPromptText("过滤日志...");
        filterField.textProperty().addListener((obs, oldVal, newVal) -> filterLogs(newVal));
        
        exportButton.setOnAction(e -> exportLogs());
        clearButton.setOnAction(e -> logArea.clear());
        
        setTop(createToolBar());
        setCenter(logArea);
        
        getStyleClass().add("log-panel");
        
        log("日志面板已初始化");
    }

    private ToolBar createToolBar() {
        ToolBar toolBar = new ToolBar();
        toolBar.getItems().addAll(filterField, exportButton, clearButton);
        return toolBar;
    }

    /**
     * 订阅日志事件。
     */
    private void subscribeLogEvents() {
        AppEventBus bus = AppEventBus.getInstance();
        
        bus.subscribe(AppEventType.LOG_DEBUG, new EventHandler<String>() {
            @Override
            public void onEvent(AppEvent event, String data) {
                logDebug(data);
            }
        });
        
        bus.subscribe(AppEventType.LOG_INFO, new EventHandler<String>() {
            @Override
            public void onEvent(AppEvent event, String data) {
                logInfo(data);
            }
        });
        
        bus.subscribe(AppEventType.LOG_WARN, new EventHandler<String>() {
            @Override
            public void onEvent(AppEvent event, String data) {
                logWarning(data);
            }
        });
        
        bus.subscribe(AppEventType.LOG_ERROR, new EventHandler<String>() {
            @Override
            public void onEvent(AppEvent event, String data) {
                logError(data);
            }
        });
    }

    public void log(String message) {
        String timestamp = LocalDateTime.now().format(TIME_FORMATTER);
        logArea.appendText(String.format("[%s] %s%n", timestamp, message));
    }

    public void logError(String message) {
        String timestamp = LocalDateTime.now().format(TIME_FORMATTER);
        logArea.appendText(String.format("[%s] ERROR: %s%n", timestamp, message));
    }

    public void logWarning(String message) {
        String timestamp = LocalDateTime.now().format(TIME_FORMATTER);
        logArea.appendText(String.format("[%s] WARN: %s%n", timestamp, message));
    }

    public void logInfo(String message) {
        log(message);
    }

    public void logDebug(String message) {
        String timestamp = LocalDateTime.now().format(TIME_FORMATTER);
        logArea.appendText(String.format("[%s] DEBUG: %s%n", timestamp, message));
    }

    private void filterLogs(String keyword) {
    }

    private void exportLogs() {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("导出日志");
        fileChooser.getExtensionFilters().add(
                new FileChooser.ExtensionFilter("文本文件", "*.txt"));
        
        File file = fileChooser.showSaveDialog(getScene().getWindow());
        if (file != null) {
            try {
                java.nio.file.Files.write(file.toPath(), logArea.getText().getBytes());
                log("日志已导出到: " + file.getAbsolutePath());
            } catch (Exception e) {
                ErrorDialog.showError("导出失败", "无法导出日志文件", e.getMessage());
            }
        }
    }

    @Override
    public String getTabId() {
        return TAB_ID;
    }

    @Override
    public String getIcon() {
        return "/icons/log.png";
    }

    @Override
    public Tab getTab() {
        return tab;
    }

    @Override
    public javafx.scene.Node getContent() {
        return this;
    }

    @Override
    public void onSelected() {
    }

    @Override
    public void onDeselected() {
    }

    @Override
    public void refresh() {
    }
}
```

---

## 五、可行性检查清单

| 检查项 | 状态 | 说明 |
|--------|------|------|
| 模块依赖方向 | ✅ | echart-ui 依赖 echart-core，无循环依赖 |
| AppEventBus 可用性 | ✅ | 已存在于 echart-core |
| AppEventType 可扩展 | ✅ | 枚举类型，可直接添加 |
| 线程安全 | ✅ | AppEventBus 已处理 UI 线程调度 |
| 编译顺序 | ✅ | echart-core 先于 echart-ui 编译 |

---

## 六、方案优点

| 优点 | 说明 |
|------|------|
| **解耦** | LogUtil 不依赖 LogPanel，避免循环依赖 |
| **线程安全** | AppEventBus 已处理 UI 线程调度，通过 PlatformAdapter.runOnUiThread |
| **可扩展** | 支持多个日志消费者（如文件日志、远程日志） |
| **复用现有机制** | 无需引入新的依赖或框架 |
| **可控性** | 提供 setEventPublishEnabled 开关，可禁用事件发布 |

---

## 七、注意事项

1. **避免无限循环**：LogUtil 的 publishLogEvent 方法中的异常捕获非常重要，避免日志发布失败导致无限循环

2. **性能考虑**：高频日志可能影响 UI 性能，建议：
   - 生产环境禁用 debug 级别日志
   - 考虑添加日志缓冲机制

3. **初始化顺序**：确保 AppEventBus 在 LogPanel 创建前已初始化

4. **JavaFX 渲染异常问题**：
   - **问题描述**：拖动窗口时，高频日志（WindowResizer 每帧约 10+ 条）直接更新 TextArea 会触发 JavaFX 内部 `PrismTextLayout.layout` 的 `NullPointerException`
   - **错误堆栈**：
     ```
     Exception in thread "JavaFX Application Thread" java.lang.NullPointerException
        at com.sun.javafx.text.PrismTextLayout.layout(PrismTextLayout.java:1112)
     ```
   - **解决方案**：在 LogPanel 中实现防抖机制：
     - 使用 `ConcurrentLinkedQueue` 缓冲日志消息
     - 使用 `AnimationTimer` 每 100ms 批量刷新到 TextArea
     - 添加 `MAX_LOG_LENGTH = 100000` 限制日志长度，自动裁剪旧日志
     - 添加异常保护 `try-catch` 防止异常传播

---

## 八、测试验证

编译验证命令：

```bash
# 编译 echart-core
gradle -p ecn/echart-core build -x test

# 编译 echart-ui
gradle -p ecn/echart-ui build -x test

# 编译 echart-app
gradle -p ecn/echart-app build -x test
```

---

**版本**: v1.0
