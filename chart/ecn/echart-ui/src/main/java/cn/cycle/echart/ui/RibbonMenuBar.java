package cn.cycle.echart.ui;

import com.cycle.control.Ribbon;
import com.cycle.control.ribbon.RibbonGroup;
import com.cycle.control.ribbon.RibbonTab;

import javafx.scene.control.Button;
import javafx.scene.control.Separator;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.Tooltip;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

/**
 * Ribbon菜单栏实现。
 * 
 * <p>使用fxribbon库实现的Ribbon样式菜单栏，包含标签页+功能组+按钮的布局。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class RibbonMenuBar extends Ribbon {

    private static final int ICON_SIZE = 28;

    private RibbonTab fileTab;
    private RibbonTab viewTab;
    private RibbonTab chartTab;
    private RibbonTab routeTab;
    private RibbonTab alarmTab;
    private RibbonTab toolsTab;
    
    private ToggleButton sideBarToggleButton;
    private ToggleButton rightTabToggleButton;
    private ToggleButton statusBarToggleButton;
    
    private boolean initialized = false;

    public RibbonMenuBar() {
        initializeRibbon();
    }
    
    public void initialize() {
        if (initialized) {
            return;
        }
        initialized = true;
    }
    
    public boolean isInitialized() {
        return initialized;
    }

    private void initializeRibbon() {
        fileTab = createFileTab();
        viewTab = createViewTab();
        chartTab = createChartTab();
        routeTab = createRouteTab();
        alarmTab = createAlarmTab();
        toolsTab = createToolsTab();

        getTabs().addAll(fileTab, viewTab, chartTab, routeTab, alarmTab, toolsTab);
        
        setSelectedRibbonTab(fileTab);
    }

    private RibbonTab createFileTab() {
        RibbonTab tab = new RibbonTab("文件");

        RibbonGroup workspaceGroup = new RibbonGroup();
        workspaceGroup.setTitle("工作区");
        workspaceGroup.getNodes().addAll(
                createButton("新建", "新建工作区", "new_32x32.png", this::onNewWorkspace),
                createButton("打开", "打开工作区", "open_32x32.png", this::onOpenWorkspace),
                createButton("保存", "保存工作区", "save_32x32.png", this::onSaveWorkspace)
        );

        RibbonGroup exportGroup = new RibbonGroup();
        exportGroup.setTitle("导出");
        exportGroup.getNodes().addAll(
                createButton("导出", "导出数据", "export_32x32.png", this::onExport),
                createButton("打印", "打印海图", "print_32x32.png", this::onPrint)
        );

        tab.getRibbonGroups().addAll(workspaceGroup, exportGroup);
        return tab;
    }

    private RibbonTab createViewTab() {
        RibbonTab tab = new RibbonTab("视图");

        RibbonGroup zoomGroup = new RibbonGroup();
        zoomGroup.setTitle("缩放");
        zoomGroup.getNodes().addAll(
                createButton("放大", "放大视图", "zoom-in_32x32.png", this::onZoomIn),
                createButton("缩小", "缩小视图", "zoom-out_32x32.png", this::onZoomOut),
                createButton("适应", "适应窗口", "fit_32x32.png", this::onFitToWindow)
        );

        RibbonGroup panelGroup = new RibbonGroup();
        panelGroup.setTitle("面板");
        
        sideBarToggleButton = createToggleButton("侧边栏", "显示/隐藏侧边栏", "sidebar_32x32.png", this::onToggleSideBar);
        rightTabToggleButton = createToggleButton("右侧面板", "显示/隐藏右侧面板", "right-tab_32x32.png", this::onToggleRightTab);
        statusBarToggleButton = createToggleButton("状态栏", "显示/隐藏状态栏", "status-bar_32x32.png", this::onToggleStatusBar);
        
        panelGroup.getNodes().addAll(
                sideBarToggleButton,
                rightTabToggleButton,
                statusBarToggleButton
        );

        tab.getRibbonGroups().addAll(zoomGroup, panelGroup);
        return tab;
    }

    private RibbonTab createChartTab() {
        RibbonTab tab = new RibbonTab("海图");

        RibbonGroup chartGroup = new RibbonGroup();
        chartGroup.setTitle("海图操作");
        chartGroup.getNodes().addAll(
                createButton("加载", "加载海图", "load-chart_32x32.png", this::onLoadChart),
                createButton("卸载", "卸载海图", "unload-chart_32x32.png", this::onUnloadChart),
                createButton("图层", "图层管理", "layers_32x32.png", this::onLayerManager)
        );

        RibbonGroup queryGroup = new RibbonGroup();
        queryGroup.setTitle("查询");
        queryGroup.getNodes().addAll(
                createButton("属性", "属性查询", "property_32x32.png", this::onPropertyQuery),
                createButton("搜索", "要素搜索", "search_32x32.png", this::onFeatureSearch)
        );

        tab.getRibbonGroups().addAll(chartGroup, queryGroup);
        return tab;
    }

    private RibbonTab createRouteTab() {
        RibbonTab tab = new RibbonTab("航线");

        RibbonGroup routeGroup = new RibbonGroup();
        routeGroup.setTitle("航线操作");
        routeGroup.getNodes().addAll(
                createButton("新建", "创建航线", "new-route_32x32.png", this::onCreateRoute),
                createButton("编辑", "编辑航线", "edit-route_32x32.png", this::onEditRoute),
                createButton("删除", "删除航线", "delete_32x32.png", this::onDeleteRoute)
        );

        RibbonGroup ioGroup = new RibbonGroup();
        ioGroup.setTitle("导入导出");
        ioGroup.getNodes().addAll(
                createButton("导入", "导入航线", "import_32x32.png", this::onImportRoute),
                createButton("导出", "导出航线", "export_32x32.png", this::onExportRoute)
        );

        RibbonGroup checkGroup = new RibbonGroup();
        checkGroup.setTitle("检查");
        checkGroup.getNodes().addAll(
                createButton("检查", "航线检查", "check_32x32.png", this::onCheckRoute)
        );

        tab.getRibbonGroups().addAll(routeGroup, ioGroup, checkGroup);
        return tab;
    }

    private RibbonTab createAlarmTab() {
        RibbonTab tab = new RibbonTab("预警");

        RibbonGroup configGroup = new RibbonGroup();
        configGroup.setTitle("配置");
        configGroup.getNodes().addAll(
                createButton("设置", "预警设置", "settings_32x32.png", this::onAlarmSettings),
                createButton("规则", "预警规则", "rules_32x32.png", this::onAlarmRules)
        );

        RibbonGroup historyGroup = new RibbonGroup();
        historyGroup.setTitle("历史");
        historyGroup.getNodes().addAll(
                createButton("历史", "预警历史", "history_32x32.png", this::onAlarmHistory),
                createButton("统计", "预警统计", "statistics_32x32.png", this::onAlarmStatistics)
        );

        RibbonGroup testGroup = new RibbonGroup();
        testGroup.setTitle("测试");
        testGroup.getNodes().addAll(
                createButton("测试", "预警测试", "test_32x32.png", this::onAlarmTest)
        );

        tab.getRibbonGroups().addAll(configGroup, historyGroup, testGroup);
        return tab;
    }

    private RibbonTab createToolsTab() {
        RibbonTab tab = new RibbonTab("工具");

        RibbonGroup measureGroup = new RibbonGroup();
        measureGroup.setTitle("测量");
        measureGroup.getNodes().addAll(
                createButton("距离", "距离测量", "distance_32x32.png", this::onMeasureDistance),
                createButton("面积", "面积测量", "area_32x32.png", this::onMeasureArea),
                createButton("方位", "方位测量", "bearing_32x32.png", this::onMeasureBearing)
        );

        RibbonGroup optionsGroup = new RibbonGroup();
        optionsGroup.setTitle("选项");
        optionsGroup.getNodes().addAll(
                createButton("设置", "系统设置", "options_32x32.png", this::onOptions),
                createButton("主题", "主题设置", "theme_32x32.png", this::onThemeSettings)
        );

        tab.getRibbonGroups().addAll(measureGroup, optionsGroup);
        return tab;
    }

    private Button createButton(String text, String tooltip, String iconFile, Runnable action) {
        Button button = new Button(text);
        button.setTooltip(new Tooltip(tooltip));
        
        ImageView icon = loadIcon(iconFile);
        if (icon != null) {
            button.setGraphic(icon);
        }
        
        button.setOnAction(e -> {
            if (action != null) {
                action.run();
            }
        });
        
        return button;
    }

    private ToggleButton createToggleButton(String text, String tooltip, String iconFile, Runnable action) {
        ToggleButton button = new ToggleButton(text);
        button.setTooltip(new Tooltip(tooltip));
        
        ImageView icon = loadIcon(iconFile);
        if (icon != null) {
            button.setGraphic(icon);
        }
        
        button.setOnAction(e -> {
            if (action != null) {
                action.run();
            }
        });
        
        return button;
    }
    
    public void updateToggleButtonStates(boolean sideBarVisible, boolean rightTabVisible, boolean statusBarVisible) {
        if (sideBarToggleButton != null) {
            sideBarToggleButton.setSelected(sideBarVisible);
        }
        if (rightTabToggleButton != null) {
            rightTabToggleButton.setSelected(rightTabVisible);
        }
        if (statusBarToggleButton != null) {
            statusBarToggleButton.setSelected(statusBarVisible);
        }
    }
    
    private ImageView loadIcon(String iconFile) {
        try {
            String iconPath = "/icons/" + iconFile;
            java.io.InputStream is = getClass().getResourceAsStream(iconPath);
            if (is != null) {
                Image image = new Image(is);
                ImageView icon = new ImageView(image);
                icon.setFitWidth(ICON_SIZE);
                icon.setFitHeight(ICON_SIZE);
                icon.setPreserveRatio(true);
                icon.setSmooth(true);
                return icon;
            }
        } catch (Exception e) {
            System.err.println("Failed to load icon: " + iconFile + " - " + e.getMessage());
        }
        return null;
    }
    
    private RibbonActionListener actionListener;
    
    public void setActionListener(RibbonActionListener listener) {
        this.actionListener = listener;
    }

    protected void onNewWorkspace() {
        if (actionListener != null) actionListener.onNewWorkspace();
    }
    protected void onOpenWorkspace() {
        if (actionListener != null) actionListener.onOpenWorkspace();
    }
    protected void onSaveWorkspace() {
        if (actionListener != null) actionListener.onSaveWorkspace();
    }
    protected void onExport() {
        if (actionListener != null) actionListener.onExport();
    }
    protected void onPrint() {
        if (actionListener != null) actionListener.onPrint();
    }
    
    protected void onZoomIn() {
        if (actionListener != null) actionListener.onZoomIn();
    }
    protected void onZoomOut() {
        if (actionListener != null) actionListener.onZoomOut();
    }
    protected void onFitToWindow() {
        if (actionListener != null) actionListener.onFitToWindow();
    }
    protected void onToggleSideBar() {
        if (actionListener != null) actionListener.onToggleSideBar();
    }
    protected void onToggleRightTab() {
        if (actionListener != null) actionListener.onToggleRightTab();
    }
    protected void onToggleStatusBar() {
        if (actionListener != null) actionListener.onToggleStatusBar();
    }
    
    protected void onLoadChart() {
        if (actionListener != null) actionListener.onLoadChart();
    }
    protected void onUnloadChart() {
        if (actionListener != null) actionListener.onUnloadChart();
    }
    protected void onLayerManager() {
        if (actionListener != null) actionListener.onLayerManager();
    }
    protected void onPropertyQuery() {
        if (actionListener != null) actionListener.onPropertyQuery();
    }
    protected void onFeatureSearch() {
        if (actionListener != null) actionListener.onFeatureSearch();
    }
    
    protected void onCreateRoute() {
        if (actionListener != null) actionListener.onCreateRoute();
    }
    protected void onEditRoute() {
        if (actionListener != null) actionListener.onEditRoute();
    }
    protected void onDeleteRoute() {
        if (actionListener != null) actionListener.onDeleteRoute();
    }
    protected void onImportRoute() {
        if (actionListener != null) actionListener.onImportRoute();
    }
    protected void onExportRoute() {
        if (actionListener != null) actionListener.onExportRoute();
    }
    protected void onCheckRoute() {
        if (actionListener != null) actionListener.onCheckRoute();
    }
    
    protected void onAlarmSettings() {
        if (actionListener != null) actionListener.onAlarmSettings();
    }
    protected void onAlarmRules() {
        if (actionListener != null) actionListener.onAlarmRules();
    }
    protected void onAlarmHistory() {
        if (actionListener != null) actionListener.onAlarmHistory();
    }
    protected void onAlarmStatistics() {
        if (actionListener != null) actionListener.onAlarmStatistics();
    }
    protected void onAlarmTest() {
        if (actionListener != null) actionListener.onAlarmTest();
    }
    
    protected void onMeasureDistance() {
        if (actionListener != null) actionListener.onMeasureDistance();
    }
    protected void onMeasureArea() {
        if (actionListener != null) actionListener.onMeasureArea();
    }
    protected void onMeasureBearing() {
        if (actionListener != null) actionListener.onMeasureBearing();
    }
    protected void onOptions() {
        if (actionListener != null) actionListener.onOptions();
    }
    protected void onThemeSettings() {
        if (actionListener != null) actionListener.onThemeSettings();
    }
    
    public interface RibbonActionListener {
        void onNewWorkspace();
        void onOpenWorkspace();
        void onSaveWorkspace();
        void onExport();
        void onPrint();
        void onZoomIn();
        void onZoomOut();
        void onFitToWindow();
        void onToggleSideBar();
        void onToggleRightTab();
        void onToggleStatusBar();
        void onLoadChart();
        void onUnloadChart();
        void onLayerManager();
        void onPropertyQuery();
        void onFeatureSearch();
        void onCreateRoute();
        void onEditRoute();
        void onDeleteRoute();
        void onImportRoute();
        void onExportRoute();
        void onCheckRoute();
        void onAlarmSettings();
        void onAlarmRules();
        void onAlarmHistory();
        void onAlarmStatistics();
        void onAlarmTest();
        void onMeasureDistance();
        void onMeasureArea();
        void onMeasureBearing();
        void onOptions();
        void onThemeSettings();
    }
}
