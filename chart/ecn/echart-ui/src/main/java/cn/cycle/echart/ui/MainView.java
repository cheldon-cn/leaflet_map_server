package cn.cycle.echart.ui;

import cn.cycle.echart.ui.panel.*;
import javafx.geometry.Insets;
import javafx.geometry.Orientation;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.SplitPane;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;

import java.util.Objects;

/**
 * 主视图。
 * 
 * <p>应用主布局框架，实现LifecycleComponent接口，整合ResponsiveLayoutManager。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class MainView extends BorderPane implements LifecycleComponent {

    private final StackPane rootContainer;
    private final BorderPane contentPane;
    private final SplitPane centerSplitPane;
    private final RibbonMenuBar ribbonMenuBar;
    private final ActivityBar activityBar;
    private final SideBarManager sideBarManager;
    private final ChartDisplayArea chartDisplayArea;
    private final RightTabManager rightTabManager;
    private final StatusBar statusBar;
    private final ResponsiveLayoutManager responsiveLayoutManager;
    
    private TitleBar titleBar;
    private Stage stage;
    
    private boolean isDraggingDivider = false;

    private boolean initialized = false;
    private boolean active = false;
    private boolean disposed = false;

    public MainView() {
        this.rootContainer = new StackPane();
        this.contentPane = new BorderPane();
        this.centerSplitPane = new SplitPane();
        this.ribbonMenuBar = new RibbonMenuBar();
        this.activityBar = new ActivityBar();
        this.sideBarManager = new SideBarManager();
        this.chartDisplayArea = new ChartDisplayArea();
        this.rightTabManager = new RightTabManager();
        this.statusBar = new StatusBar();
        this.responsiveLayoutManager = new ResponsiveLayoutManager();
        
        initializeLayout();
        initializePanels();
        setupResponsiveLayout();
    }
    
    public void setStage(Stage stage) {
        this.stage = stage;
        if (stage != null) {
            titleBar = new TitleBar(stage);
            StackPane.setAlignment(titleBar, Pos.TOP_CENTER);
            rootContainer.getChildren().add(titleBar);
            
            titleBar.getWindowControls().setOnSettingsAction(() -> {
                showSettingsDialog();
            });
            
            stage.setMinWidth(800);
        }
    }
    
    public void enableWindowResize(Scene scene) {
        if (stage != null && scene != null) {
            new WindowResizer(stage, scene);
        }
    }
    
    private void showSettingsDialog() {
        javafx.scene.control.Alert alert = new javafx.scene.control.Alert(
                javafx.scene.control.Alert.AlertType.INFORMATION);
        alert.setTitle("设置");
        alert.setHeaderText("系统设置");
        alert.setContentText("设置功能开发中...");
        alert.showAndWait();
    }

    private void initializeLayout() {
        centerSplitPane.setOrientation(Orientation.HORIZONTAL);
        centerSplitPane.getItems().addAll(sideBarManager, chartDisplayArea, rightTabManager);
        centerSplitPane.setStyle("-fx-background-color: derive(-fx-base, -5%);");
        
        SplitPane.setResizableWithParent(sideBarManager, false);
        SplitPane.setResizableWithParent(rightTabManager, false);
        
        contentPane.setTop(ribbonMenuBar);
        contentPane.setCenter(centerSplitPane);
        contentPane.setBottom(statusBar);
        contentPane.setPadding(new Insets(32, 0, 0, 0));
        
        rootContainer.getChildren().add(contentPane);
        setCenter(rootContainer);
        
        javafx.application.Platform.runLater(() -> {
            double splitWidth = centerSplitPane.getWidth();
            if (splitWidth > 0) {
                double leftRatio = 40.0 / splitWidth;
                double rightRatio = 1 - rightTabManager.getPrefWidth() / splitWidth;
                centerSplitPane.setDividerPositions(leftRatio, Math.max(rightRatio, 0.5));
            }
        });
        
        setupDividerListeners();
        
        getStyleClass().add("main-view");
    }
    
    private void setupDividerListeners() {
        final double COLLAPSE_THRESHOLD = 80.0;
        
        centerSplitPane.getDividers().get(0).positionProperty().addListener((obs, oldVal, newVal) -> {
            isDraggingDivider = true;
            
            try {
                double splitWidth = centerSplitPane.getWidth();
                if (splitWidth <= 0) return;
                
                double newWidth = newVal.doubleValue() * splitWidth - 40;
                double maxPanelWidth = splitWidth * SideBarManager.MAX_EXPANDED_WIDTH_RATIO;
                
                if (!sideBarManager.isExpanded()) {
                    if (newWidth > 10) {
                        String firstPanelId = sideBarManager.getFirstPanelId();
                        if (firstPanelId != null) {
                            double actualWidth = Math.min(newWidth, maxPanelWidth);
                            sideBarManager.showPanel(firstPanelId, actualWidth, true);
                        }
                    }
                } else {
                    if (newWidth < COLLAPSE_THRESHOLD) {
                        sideBarManager.collapsePanel();
                    } else {
                        double actualWidth = Math.min(newWidth, maxPanelWidth);
                        if (Math.abs(actualWidth - sideBarManager.getExpandedWidth()) > 5) {
                            sideBarManager.setExpandedWidth(actualWidth, true);
                        }
                    }
                }
            } finally {
                isDraggingDivider = false;
            }
        });
        
        centerSplitPane.getDividers().get(1).positionProperty().addListener((obs, oldVal, newVal) -> {
            double rightWidth = (1 - newVal.doubleValue()) * centerSplitPane.getWidth();
            if (rightWidth > 0 && Math.abs(rightWidth - rightTabManager.getPrefWidth()) > 5) {
                rightTabManager.setPrefWidth(rightWidth);
            }
        });
        
        sideBarManager.currentWidthProperty().addListener((obs, oldVal, newVal) -> {
            if (!isDraggingDivider) {
                javafx.application.Platform.runLater(() -> {
                    updateDividerPositions();
                });
            }
        });
        
        rightTabManager.prefWidthProperty().addListener((obs, oldVal, newVal) -> {
            if (!isDraggingDivider) {
                javafx.application.Platform.runLater(() -> {
                    updateDividerPositions();
                });
            }
        });
        
        centerSplitPane.widthProperty().addListener((obs, oldVal, newVal) -> {
            if (!isDraggingDivider) {
                javafx.application.Platform.runLater(() -> {
                    updateDividerPositions();
                });
            }
        });
    }
    
    private void updateDividerPositions() {
        if (isDraggingDivider) {
            return;
        }
        
        double splitWidth = centerSplitPane.getWidth();
        if (splitWidth <= 0) return;
        
        double leftWidth = sideBarManager.getCurrentWidth();
        double rightWidth = rightTabManager.getPrefWidth();
        
        double leftRatio = leftWidth / splitWidth;
        double rightRatio = 1.0 - rightWidth / splitWidth;
        
        if (leftRatio >= rightRatio) {
            double totalSideWidth = leftWidth + rightWidth;
            if (totalSideWidth < splitWidth) {
                leftRatio = leftWidth / splitWidth;
                rightRatio = 1.0 - rightWidth / splitWidth;
            } else {
                leftRatio = leftWidth / (leftWidth + rightWidth + 100);
                rightRatio = 1.0 - rightWidth / (leftWidth + rightWidth + 100);
            }
        }
        
        leftRatio = Math.max(40.0 / splitWidth, Math.min(leftRatio, 0.4));
        rightRatio = Math.max(0.5, Math.min(rightRatio, 1.0 - 200.0 / splitWidth));
        
        centerSplitPane.setDividerPositions(leftRatio, rightRatio);
    }
    
    private void initializePanels() {
        SideBarManager.SideBarPanel layersPanel = new SideBarManager.SideBarPanel(
                "layers", "图层管理", "/icons/layers.png", "图层管理面板",
                new LayerManagerPanel());
        SideBarManager.SideBarPanel searchPanel = new SideBarManager.SideBarPanel(
                "search", "搜索", "/icons/search.png", "搜索面板",
                createPlaceholderPanel("搜索"));
        SideBarManager.SideBarPanel routePanel = new SideBarManager.SideBarPanel(
                "route", "航线", "/icons/route.png", "航线规划面板",
                new RoutePanel());
        SideBarManager.SideBarPanel alarmPanel = new SideBarManager.SideBarPanel(
                "alarm", "预警", "/icons/alarm.png", "预警管理面板",
                new AlarmPanel());
        SideBarManager.SideBarPanel aisPanel = new SideBarManager.SideBarPanel(
                "ais", "AIS", "/icons/ais.png", "AIS目标面板",
                createPlaceholderPanel("AIS目标"));
        SideBarManager.SideBarPanel measurePanel = new SideBarManager.SideBarPanel(
                "measure", "测量", "/icons/measure.png", "测量工具面板",
                createPlaceholderPanel("测量工具"));
        SideBarManager.SideBarPanel settingsPanel = new SideBarManager.SideBarPanel(
                "settings", "设置", "/icons/settings.png", "设置面板",
                createPlaceholderPanel("设置"));
        
        sideBarManager.registerPanel(layersPanel, 0);
        sideBarManager.registerPanel(searchPanel, 1);
        sideBarManager.registerPanel(routePanel, 2);
        sideBarManager.registerPanel(alarmPanel, 3);
        sideBarManager.registerPanel(aisPanel, 4);
        sideBarManager.registerPanel(measurePanel, 5);
        sideBarManager.registerPanel(settingsPanel, 6);
        
        rightTabManager.registerPanel(new AlarmPanel());
        rightTabManager.registerPanel(new LogPanel());
        rightTabManager.registerPanel(new PropertyPanel());
        rightTabManager.registerPanel(new TerminalPanel());
        
        setupRibbonActions();
    }
    
    private void setupRibbonActions() {
        ribbonMenuBar.setActionListener(new RibbonMenuBar.RibbonActionListener() {
            @Override
            public void onNewWorkspace() {
                showInfo("新建工作区", "新建工作区功能开发中...");
            }
            
            @Override
            public void onOpenWorkspace() {
                showInfo("打开工作区", "打开工作区功能开发中...");
            }
            
            @Override
            public void onSaveWorkspace() {
                showInfo("保存工作区", "保存工作区功能开发中...");
            }
            
            @Override
            public void onExport() {
                showInfo("导出", "导出功能开发中...");
            }
            
            @Override
            public void onPrint() {
                showInfo("打印", "打印功能开发中...");
            }
            
            @Override
            public void onZoomIn() {
                chartDisplayArea.zoomIn();
            }
            
            @Override
            public void onZoomOut() {
                chartDisplayArea.zoomOut();
            }
            
            @Override
            public void onFitToWindow() {
                chartDisplayArea.fitToWindow();
            }
            
            @Override
            public void onToggleSideBar() {
                toggleSideBar();
            }
            
            @Override
            public void onToggleRightTab() {
                toggleRightTab();
            }
            
            @Override
            public void onToggleStatusBar() {
                if (getBottom() == null) {
                    setBottom(statusBar);
                } else {
                    setBottom(null);
                }
            }
            
            @Override
            public void onLoadChart() {
                showInfo("加载海图", "加载海图功能开发中...");
            }
            
            @Override
            public void onUnloadChart() {
                showInfo("卸载海图", "卸载海图功能开发中...");
            }
            
            @Override
            public void onLayerManager() {
                sideBarManager.showPanel("layers");
            }
            
            @Override
            public void onPropertyQuery() {
                showInfo("属性查询", "属性查询功能开发中...");
            }
            
            @Override
            public void onFeatureSearch() {
                sideBarManager.showPanel("search");
            }
            
            @Override
            public void onCreateRoute() {
                showInfo("创建航线", "创建航线功能开发中...");
            }
            
            @Override
            public void onEditRoute() {
                showInfo("编辑航线", "编辑航线功能开发中...");
            }
            
            @Override
            public void onDeleteRoute() {
                showInfo("删除航线", "删除航线功能开发中...");
            }
            
            @Override
            public void onImportRoute() {
                showInfo("导入航线", "导入航线功能开发中...");
            }
            
            @Override
            public void onExportRoute() {
                showInfo("导出航线", "导出航线功能开发中...");
            }
            
            @Override
            public void onCheckRoute() {
                showInfo("航线检查", "航线检查功能开发中...");
            }
            
            @Override
            public void onAlarmSettings() {
                showInfo("预警设置", "预警设置功能开发中...");
            }
            
            @Override
            public void onAlarmRules() {
                showInfo("预警规则", "预警规则功能开发中...");
            }
            
            @Override
            public void onAlarmHistory() {
                showInfo("预警历史", "预警历史功能开发中...");
            }
            
            @Override
            public void onAlarmStatistics() {
                showInfo("预警统计", "预警统计功能开发中...");
            }
            
            @Override
            public void onAlarmTest() {
                showInfo("预警测试", "预警测试功能开发中...");
            }
            
            @Override
            public void onMeasureDistance() {
                showInfo("距离测量", "距离测量功能开发中...");
            }
            
            @Override
            public void onMeasureArea() {
                showInfo("面积测量", "面积测量功能开发中...");
            }
            
            @Override
            public void onMeasureBearing() {
                showInfo("方位测量", "方位测量功能开发中...");
            }
            
            @Override
            public void onOptions() {
                showSettingsDialog();
            }
            
            @Override
            public void onThemeSettings() {
                showInfo("主题设置", "主题设置功能开发中...");
            }
        });
    }
    
    private void showInfo(String title, String message) {
        javafx.scene.control.Alert alert = new javafx.scene.control.Alert(
                javafx.scene.control.Alert.AlertType.INFORMATION);
        alert.setTitle(title);
        alert.setHeaderText(null);
        alert.setContentText(message);
        alert.showAndWait();
    }
    
    private javafx.scene.Node createPlaceholderPanel(String title) {
        javafx.scene.control.Label label = new javafx.scene.control.Label(title + " 面板");
        label.setStyle("-fx-font-size: 16px; -fx-padding: 20px;");
        return new javafx.scene.layout.StackPane(label);
    }

    private void setupResponsiveLayout() {
        responsiveLayoutManager.addListener((oldMode, newMode) -> {
            applyLayoutConfig(ResponsiveLayoutManager.LayoutConfig.forMode(newMode));
        });
        
        responsiveLayoutManager.monitor(this);
    }

    private void applyLayoutConfig(ResponsiveLayoutManager.LayoutConfig config) {
        sideBarManager.setExpandedWidth(config.getSidebarWidth());
        
        if (config.isShowRightPanel()) {
            showRightTab();
            rightTabManager.setPrefWidth(config.getRightPanelWidth());
        } else {
            hideRightTab();
        }
        
        chartDisplayArea.setColumnCount(config.getChartColumns());
    }

    @Override
    public void initialize() {
        if (initialized) {
            return;
        }
        
        ribbonMenuBar.initialize();
        chartDisplayArea.initialize();
        sideBarManager.getPanels().values().forEach(panel -> {
            if (panel.getContent() instanceof LifecycleComponent) {
                ((LifecycleComponent) panel.getContent()).initialize();
            }
        });
        
        if (rightTabManager instanceof LifecycleComponent) {
            ((LifecycleComponent) rightTabManager).initialize();
        }
        
        initialized = true;
    }

    @Override
    public void activate() {
        if (!initialized || active) {
            return;
        }
        
        active = true;
        
        chartDisplayArea.activate();
        
        ResponsiveLayoutManager.LayoutConfig config = 
                ResponsiveLayoutManager.LayoutConfig.forMode(responsiveLayoutManager.getCurrentMode());
        applyLayoutConfig(config);
    }

    @Override
    public void deactivate() {
        if (!active) {
            return;
        }
        
        active = false;
        chartDisplayArea.deactivate();
    }

    @Override
    public void dispose() {
        if (disposed) {
            return;
        }
        
        deactivate();
        
        chartDisplayArea.dispose();
        
        sideBarManager.getPanels().values().forEach(panel -> {
            if (panel.getContent() instanceof LifecycleComponent) {
                ((LifecycleComponent) panel.getContent()).dispose();
            }
        });
        
        if (rightTabManager instanceof LifecycleComponent) {
            ((LifecycleComponent) rightTabManager).dispose();
        }
        
        responsiveLayoutManager.stopMonitoring();
        
        disposed = true;
    }

    @Override
    public boolean isInitialized() {
        return initialized;
    }

    @Override
    public boolean isActive() {
        return active;
    }

    @Override
    public boolean isDisposed() {
        return disposed;
    }

    public RibbonMenuBar getRibbonMenuBar() {
        return ribbonMenuBar;
    }

    public ActivityBar getActivityBar() {
        return activityBar;
    }

    public SideBarManager getSideBarManager() {
        return sideBarManager;
    }

    public ChartDisplayArea getChartDisplayArea() {
        return chartDisplayArea;
    }

    public RightTabManager getRightTabManager() {
        return rightTabManager;
    }

    public StatusBar getStatusBar() {
        return statusBar;
    }

    public ResponsiveLayoutManager getResponsiveLayoutManager() {
        return responsiveLayoutManager;
    }

    public void showSideBar() {
        sideBarManager.expandPanel();
    }

    public void hideSideBar() {
        sideBarManager.collapsePanel();
    }

    public void toggleSideBar() {
        if (sideBarManager.isExpanded()) {
            hideSideBar();
        } else {
            showSideBar();
        }
    }

    public void showRightTab() {
        if (!centerSplitPane.getItems().contains(rightTabManager)) {
            centerSplitPane.getItems().add(rightTabManager);
            javafx.application.Platform.runLater(() -> {
                updateDividerPositions();
            });
        }
    }

    public void hideRightTab() {
        centerSplitPane.getItems().remove(rightTabManager);
    }

    public void toggleRightTab() {
        if (centerSplitPane.getItems().contains(rightTabManager)) {
            hideRightTab();
        } else {
            showRightTab();
        }
    }

    public void setLayoutMode(ResponsiveLayoutManager.LayoutMode mode) {
        Objects.requireNonNull(mode, "mode cannot be null");
        applyLayoutConfig(ResponsiveLayoutManager.LayoutConfig.forMode(mode));
    }

    public ResponsiveLayoutManager.LayoutMode getLayoutMode() {
        return responsiveLayoutManager.getCurrentMode();
    }
}
