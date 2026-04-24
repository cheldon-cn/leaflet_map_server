package cn.cycle.echart.ui;

import cn.cycle.echart.theme.Theme;
import cn.cycle.echart.theme.ThemeManager;
import cn.cycle.echart.ui.handler.*;
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
    
    private ThemeManager themeManager;
    private UIConfig uiConfig;
    
    private boolean isDraggingDivider = false;

    private boolean initialized = false;
    private boolean active = false;
    private boolean disposed = false;
    
    private FileHandler fileHandler;
    private ViewHandler viewHandler;
    private ChartHandler chartHandler;
    private RouteHandler routeHandler;
    private AlarmHandler alarmHandler;
    private MeasureHandler measureHandler;
    private ToolsHandler toolsHandler;

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
            
            titleBar.setOnSettingsAction(() -> {
                toolsHandler.onOptions();
            });
            
            titleBar.setOnToggleLeftBar(() -> {
                viewHandler.toggleSideBar();
            });
            
            titleBar.setOnToggleRightBar(() -> {
                viewHandler.toggleRightTab();
            });
            
            titleBar.setOnLoginAction(() -> {
                showInfo("用户登录", "用户登录功能开发中...");
            });
            
            titleBar.setOnSearch(() -> {
                String searchText = titleBar.getSearchText();
                if (searchText != null && !searchText.trim().isEmpty()) {
                    sideBarManager.showPanel("search");
                }
            });
            
            chartHandler.setStage(stage);
            
            stage.setMinWidth(800);
        }
    }
    
    public void setThemeManager(ThemeManager themeManager) {
        this.themeManager = themeManager;
        if (toolsHandler != null) {
            toolsHandler.setThemeManager(themeManager);
        }
        if (themeManager != null) {
            themeManager.addThemeChangeListener((oldTheme, newTheme) -> {
                applyTheme(newTheme);
            });
        }
    }
    
    public void setAppConfig(UIConfig uiConfig) {
        this.uiConfig = uiConfig;
        if (toolsHandler != null) {
            toolsHandler.setUiConfig(uiConfig);
        }
    }
    
    private void applyTheme(Theme theme) {
        if (theme == null || stage == null) {
            return;
        }
        
        stage.getScene().getRoot().getStyleClass().removeAll("light-theme", "dark-theme", "high-contrast-theme");
        
        String themeClass = theme.isDark() ? "dark-theme" : "light-theme";
        if (theme.getName().contains("high-contrast")) {
            themeClass = "high-contrast-theme";
        }
        stage.getScene().getRoot().getStyleClass().add(themeClass);
        
        stage.getScene().getStylesheets().clear();
        
        String mainCss = getClass().getResource("/styles/main.css") != null ?
                getClass().getResource("/styles/main.css").toExternalForm() : null;
        if (mainCss != null) {
            stage.getScene().getStylesheets().add(mainCss);
        }
        
        String themeCss = theme.getStyleSheet() != null ?
                getClass().getResource(theme.getStyleSheet()) != null ?
                getClass().getResource(theme.getStyleSheet()).toExternalForm() : null : null;
        if (themeCss != null) {
            stage.getScene().getStylesheets().add(themeCss);
        }
    }
    
    public void enableWindowResize(Scene scene) {
        if (stage != null && scene != null) {
            new WindowResizer(stage, scene);
        }
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
        setupSideBarListeners();
        
        getStyleClass().add("main-view");
    }
    
    private void setupSideBarListeners() {
        sideBarManager.expandedProperty().addListener((obs, oldVal, newVal) -> {
            javafx.application.Platform.runLater(() -> {
                updateDividerPositions();
            });
        });
    }
    
    private void setupDividerListeners() {
        final double COLLAPSE_THRESHOLD = 80.0;
        
        centerSplitPane.getDividers().get(0).positionProperty().addListener((obs, oldVal, newVal) -> {
            double splitWidth = centerSplitPane.getWidth();
            if (splitWidth <= 0) return;
            
            double actualWidth = newVal.doubleValue() * splitWidth;
            double contentWidth = actualWidth - 40;
            
            if (contentWidth < COLLAPSE_THRESHOLD) {
                if (sideBarManager.isExpanded()) {
                    isDraggingDivider = true;
                    try {
                        sideBarManager.collapsePanel();
                    } finally {
                        isDraggingDivider = false;
                    }
                }
            } else {
                if (!sideBarManager.isExpanded()) {
                    isDraggingDivider = true;
                    try {
                        String firstPanelId = sideBarManager.getFirstPanelId();
                        if (firstPanelId != null) {
                            sideBarManager.showPanel(firstPanelId, contentWidth, true);
                        }
                    } finally {
                        isDraggingDivider = false;
                    }
                }
            }
        });
        
        centerSplitPane.getDividers().get(1).positionProperty().addListener((obs, oldVal, newVal) -> {
            double rightWidth = (1 - newVal.doubleValue()) * centerSplitPane.getWidth();
            if (rightWidth > 0 && Math.abs(rightWidth - rightTabManager.getPrefWidth()) > 5) {
                rightTabManager.setPrefWidth(rightWidth);
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
        LayerManagerPanel layerManagerPanel = new LayerManagerPanel();
        SideBarManager.SideBarPanel layersPanel = new SideBarManager.SideBarPanel(
                "layers", "图层管理", "/icons/layers.png", "图层管理面板",
                layerManagerPanel.getContent());
        
        SearchPanel searchPanel = new SearchPanel();
        SideBarManager.SideBarPanel searchSideBarPanel = new SideBarManager.SideBarPanel(
                "search", "搜索", "/icons/search.png", "搜索面板",
                searchPanel.getContent());
        
        SideBarManager.SideBarPanel routePanel = new SideBarManager.SideBarPanel(
                "route", "航线", "/icons/route.png", "航线规划面板",
                new RoutePanel());
        SideBarManager.SideBarPanel alarmPanel = new SideBarManager.SideBarPanel(
                "alarm", "预警", "/icons/alarm.png", "预警管理面板",
                new AlarmPanel());
        SideBarManager.SideBarPanel aisPanel = new SideBarManager.SideBarPanel(
                "ais", "AIS", "/icons/ais.png", "AIS目标面板",
                createPlaceholderPanel("AIS目标"));
        
        MeasurePanel measurePanel = new MeasurePanel();
        SideBarManager.SideBarPanel measureSideBarPanel = new SideBarManager.SideBarPanel(
                "measure", "测量", "/icons/measure.png", "测量工具面板",
                measurePanel.getContent());
        
        SettingsPanel settingsPanel = new SettingsPanel();
        SideBarManager.SideBarPanel settingsSideBarPanel = new SideBarManager.SideBarPanel(
                "settings", "设置", "/icons/settings.png", "设置面板",
                settingsPanel.getContent());
        
        sideBarManager.registerPanel(layersPanel, 0);
        sideBarManager.registerPanel(searchSideBarPanel, 1);
        sideBarManager.registerPanel(routePanel, 2);
        sideBarManager.registerPanel(alarmPanel, 3);
        sideBarManager.registerPanel(aisPanel, 4);
        sideBarManager.registerPanel(measureSideBarPanel, 5);
        sideBarManager.registerPanel(settingsSideBarPanel, 6);
        
        PropertyPanel propertyPanel = new PropertyPanel();
        rightTabManager.registerPanel(new AlarmPanel());
        rightTabManager.registerPanel(new LogPanel());
        rightTabManager.registerPanel(propertyPanel);
        rightTabManager.registerPanel(new TerminalPanel());
        
        initializeHandlers();
        setupLayerPanelCallbacks(layerManagerPanel, propertyPanel);
        setupSearchPanelCallbacks(searchPanel);
        setupMeasurePanelCallbacks(measurePanel);
        setupSettingsPanelCallbacks(settingsPanel);
        setupRibbonActions();
    }
    
    private void setupLayerPanelCallbacks(LayerManagerPanel layerManagerPanel, PropertyPanel propertyPanel) {
        layerManagerPanel.setOnHidePanel(() -> {
            sideBarManager.collapsePanel();
            updateDividerPositions();
        });
        
        layerManagerPanel.setOnConfigPanel(() -> {
        });
        
        layerManagerPanel.setOnViewProperties(layerNode -> {
            propertyPanel.clear();
            propertyPanel.addCommonProperty("名称", layerNode.getName());
            propertyPanel.addCommonProperty("ID", layerNode.getId());
            propertyPanel.addCommonProperty("类型", layerNode.getType().toString());
            propertyPanel.addCommonProperty("可见性", layerNode.isVisible() ? "可见" : "隐藏");
            
            java.util.Map<String, String> dataProps = layerNode.getProperties();
            for (java.util.Map.Entry<String, String> entry : dataProps.entrySet()) {
                propertyPanel.addDataProperty(entry.getKey(), entry.getValue());
            }
            
            rightTabManager.selectPanel("property-panel");
            if (!rightTabManager.isVisible()) {
                viewHandler.showRightTab();
            }
        });
        
        chartHandler.setLayerManagerPanel(layerManagerPanel);
    }
    
    private void setupSearchPanelCallbacks(SearchPanel searchPanel) {
        searchPanel.setOnHidePanel(() -> {
            sideBarManager.collapsePanel();
            updateDividerPositions();
        });
        
        searchPanel.setOnSearchResultSelected(result -> {
            showInfo("搜索结果", "选中: " + result.getName() + "\n类型: " + result.getType() + "\n位置: " + result.getLocation());
        });
    }
    
    private void setupMeasurePanelCallbacks(MeasurePanel measurePanel) {
        measurePanel.setOnHidePanel(() -> {
            sideBarManager.collapsePanel();
            updateDividerPositions();
        });
        
        measurePanel.setOnMeasurementSelected(measurement -> {
            showInfo("测量结果", "选中: " + measurement.getValue() + " " + measurement.getUnit() + 
                    "\n类型: " + measurement.getMode() + "\n描述: " + measurement.getDescription());
        });
        
        measurePanel.setOnMeasureModeChanged(mode -> {
            statusBar.showMessage("测量模式: " + mode.toString());
        });
    }
    
    private void setupSettingsPanelCallbacks(SettingsPanel settingsPanel) {
        settingsPanel.setOnHidePanel(() -> {
            sideBarManager.collapsePanel();
            updateDividerPositions();
        });
        
        settingsPanel.setOnSettingsChanged(() -> {
            statusBar.showMessage("设置已更改");
        });
    }
    
    private void initializeHandlers() {
        MessageCallback messageCallback = this::showInfo;
        
        fileHandler = new FileHandler(stage, chartDisplayArea, statusBar, messageCallback);
        
        viewHandler = new ViewHandler(chartDisplayArea, sideBarManager, rightTabManager, 
                centerSplitPane, statusBar, this::updateDividerPositions);
        
        chartHandler = new ChartHandler(sideBarManager, messageCallback);
        
        routeHandler = new RouteHandler(messageCallback);
        
        alarmHandler = new AlarmHandler(messageCallback);
        
        measureHandler = new MeasureHandler(messageCallback);
        
        toolsHandler = new ToolsHandler(stage, statusBar, messageCallback);
        if (themeManager != null) {
            toolsHandler.setThemeManager(themeManager);
        }
        if (uiConfig != null) {
            toolsHandler.setUiConfig(uiConfig);
        }
    }
    
    private void setupRibbonActions() {
        ribbonMenuBar.setActionListener(new RibbonMenuBar.RibbonActionListener() {
            @Override
            public void onNewWorkspace() {
                fileHandler.onNewWorkspace();
            }
            
            @Override
            public void onOpenWorkspace() {
                fileHandler.onOpenWorkspace();
            }
            
            @Override
            public void onSaveWorkspace() {
                fileHandler.onSaveWorkspace();
            }
            
            @Override
            public void onExport() {
                fileHandler.onExport();
            }
            
            @Override
            public void onPrint() {
                fileHandler.onPrint();
            }
            
            @Override
            public void onZoomIn() {
                viewHandler.onZoomIn();
            }
            
            @Override
            public void onZoomOut() {
                viewHandler.onZoomOut();
            }
            
            @Override
            public void onFitToWindow() {
                viewHandler.onFitToWindow();
            }
            
            @Override
            public void onToggleSideBar() {
                viewHandler.onToggleSideBar();
            }
            
            @Override
            public void onToggleRightTab() {
                viewHandler.onToggleRightTab();
            }
            
            @Override
            public void onToggleStatusBar() {
                viewHandler.onToggleStatusBar(MainView.this);
            }
            
            @Override
            public void onLoadChart() {
                chartHandler.onLoadChart();
            }
            
            @Override
            public void onUnloadChart() {
                chartHandler.onUnloadChart();
            }
            
            @Override
            public void onLayerManager() {
                chartHandler.onLayerManager();
            }
            
            @Override
            public void onPropertyQuery() {
                chartHandler.onPropertyQuery();
            }
            
            @Override
            public void onFeatureSearch() {
                chartHandler.onFeatureSearch();
            }
            
            @Override
            public void onCreateRoute() {
                routeHandler.onCreateRoute();
            }
            
            @Override
            public void onEditRoute() {
                routeHandler.onEditRoute();
            }
            
            @Override
            public void onDeleteRoute() {
                routeHandler.onDeleteRoute();
            }
            
            @Override
            public void onImportRoute() {
                routeHandler.onImportRoute();
            }
            
            @Override
            public void onExportRoute() {
                routeHandler.onExportRoute();
            }
            
            @Override
            public void onCheckRoute() {
                routeHandler.onCheckRoute();
            }
            
            @Override
            public void onAlarmSettings() {
                alarmHandler.onAlarmSettings();
            }
            
            @Override
            public void onAlarmRules() {
                alarmHandler.onAlarmRules();
            }
            
            @Override
            public void onAlarmHistory() {
                alarmHandler.onAlarmHistory();
            }
            
            @Override
            public void onAlarmStatistics() {
                alarmHandler.onAlarmStatistics();
            }
            
            @Override
            public void onAlarmTest() {
                alarmHandler.onAlarmTest();
            }
            
            @Override
            public void onMeasureDistance() {
                measureHandler.onMeasureDistance();
            }
            
            @Override
            public void onMeasureArea() {
                measureHandler.onMeasureArea();
            }
            
            @Override
            public void onMeasureBearing() {
                measureHandler.onMeasureBearing();
            }
            
            @Override
            public void onOptions() {
                toolsHandler.onOptions();
            }
            
            @Override
            public void onThemeSettings() {
                toolsHandler.onThemeSettings();
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
            viewHandler.showRightTab();
            rightTabManager.setPrefWidth(config.getRightPanelWidth());
        } else {
            viewHandler.hideRightTab();
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
        viewHandler.showSideBar();
    }

    public void hideSideBar() {
        viewHandler.hideSideBar();
    }

    public void toggleSideBar() {
        viewHandler.toggleSideBar();
    }

    public void showRightTab() {
        viewHandler.showRightTab();
    }

    public void hideRightTab() {
        viewHandler.hideRightTab();
    }

    public void toggleRightTab() {
        viewHandler.toggleRightTab();
    }

    public void setLayoutMode(ResponsiveLayoutManager.LayoutMode mode) {
        Objects.requireNonNull(mode, "mode cannot be null");
        applyLayoutConfig(ResponsiveLayoutManager.LayoutConfig.forMode(mode));
    }

    public ResponsiveLayoutManager.LayoutMode getLayoutMode() {
        return responsiveLayoutManager.getCurrentMode();
    }
}
