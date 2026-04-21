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

    private RibbonTab fileTab;
    private RibbonTab viewTab;
    private RibbonTab chartTab;
    private RibbonTab routeTab;
    private RibbonTab alarmTab;
    private RibbonTab toolsTab;
    
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
                createButton("新建", "新建工作区", "icons/new.png", this::onNewWorkspace),
                createButton("打开", "打开工作区", "icons/open.png", this::onOpenWorkspace),
                createButton("保存", "保存工作区", "icons/save.png", this::onSaveWorkspace)
        );

        RibbonGroup exportGroup = new RibbonGroup();
        exportGroup.setTitle("导出");
        exportGroup.getNodes().addAll(
                createButton("导出", "导出数据", "icons/export.png", this::onExport),
                createButton("打印", "打印海图", "icons/print.png", this::onPrint)
        );

        tab.getRibbonGroups().addAll(workspaceGroup, exportGroup);
        return tab;
    }

    private RibbonTab createViewTab() {
        RibbonTab tab = new RibbonTab("视图");

        RibbonGroup zoomGroup = new RibbonGroup();
        zoomGroup.setTitle("缩放");
        zoomGroup.getNodes().addAll(
                createButton("放大", "放大视图", "icons/zoom-in.png", this::onZoomIn),
                createButton("缩小", "缩小视图", "icons/zoom-out.png", this::onZoomOut),
                createButton("适应", "适应窗口", "icons/fit.png", this::onFitToWindow)
        );

        RibbonGroup panelGroup = new RibbonGroup();
        panelGroup.setTitle("面板");
        panelGroup.getNodes().addAll(
                createToggleButton("侧边栏", "显示/隐藏侧边栏", this::onToggleSideBar),
                createToggleButton("右侧面板", "显示/隐藏右侧面板", this::onToggleRightTab),
                createToggleButton("状态栏", "显示/隐藏状态栏", this::onToggleStatusBar)
        );

        tab.getRibbonGroups().addAll(zoomGroup, panelGroup);
        return tab;
    }

    private RibbonTab createChartTab() {
        RibbonTab tab = new RibbonTab("海图");

        RibbonGroup chartGroup = new RibbonGroup();
        chartGroup.setTitle("海图操作");
        chartGroup.getNodes().addAll(
                createButton("加载", "加载海图", "icons/load-chart.png", this::onLoadChart),
                createButton("卸载", "卸载海图", "icons/unload-chart.png", this::onUnloadChart),
                createButton("图层", "图层管理", "icons/layers.png", this::onLayerManager)
        );

        RibbonGroup queryGroup = new RibbonGroup();
        queryGroup.setTitle("查询");
        queryGroup.getNodes().addAll(
                createButton("属性", "属性查询", "icons/property.png", this::onPropertyQuery),
                createButton("搜索", "要素搜索", "icons/search.png", this::onFeatureSearch)
        );

        tab.getRibbonGroups().addAll(chartGroup, queryGroup);
        return tab;
    }

    private RibbonTab createRouteTab() {
        RibbonTab tab = new RibbonTab("航线");

        RibbonGroup routeGroup = new RibbonGroup();
        routeGroup.setTitle("航线操作");
        routeGroup.getNodes().addAll(
                createButton("新建", "创建航线", "icons/new-route.png", this::onCreateRoute),
                createButton("编辑", "编辑航线", "icons/edit-route.png", this::onEditRoute),
                createButton("删除", "删除航线", "icons/delete.png", this::onDeleteRoute)
        );

        RibbonGroup ioGroup = new RibbonGroup();
        ioGroup.setTitle("导入导出");
        ioGroup.getNodes().addAll(
                createButton("导入", "导入航线", "icons/import.png", this::onImportRoute),
                createButton("导出", "导出航线", "icons/export.png", this::onExportRoute)
        );

        RibbonGroup checkGroup = new RibbonGroup();
        checkGroup.setTitle("检查");
        checkGroup.getNodes().addAll(
                createButton("检查", "航线检查", "icons/check.png", this::onCheckRoute)
        );

        tab.getRibbonGroups().addAll(routeGroup, ioGroup, checkGroup);
        return tab;
    }

    private RibbonTab createAlarmTab() {
        RibbonTab tab = new RibbonTab("预警");

        RibbonGroup configGroup = new RibbonGroup();
        configGroup.setTitle("配置");
        configGroup.getNodes().addAll(
                createButton("设置", "预警设置", "icons/settings.png", this::onAlarmSettings),
                createButton("规则", "预警规则", "icons/rules.png", this::onAlarmRules)
        );

        RibbonGroup historyGroup = new RibbonGroup();
        historyGroup.setTitle("历史");
        historyGroup.getNodes().addAll(
                createButton("历史", "预警历史", "icons/history.png", this::onAlarmHistory),
                createButton("统计", "预警统计", "icons/statistics.png", this::onAlarmStatistics)
        );

        RibbonGroup testGroup = new RibbonGroup();
        testGroup.setTitle("测试");
        testGroup.getNodes().addAll(
                createButton("测试", "预警测试", "icons/test.png", this::onAlarmTest)
        );

        tab.getRibbonGroups().addAll(configGroup, historyGroup, testGroup);
        return tab;
    }

    private RibbonTab createToolsTab() {
        RibbonTab tab = new RibbonTab("工具");

        RibbonGroup measureGroup = new RibbonGroup();
        measureGroup.setTitle("测量");
        measureGroup.getNodes().addAll(
                createButton("距离", "距离测量", "icons/distance.png", this::onMeasureDistance),
                createButton("面积", "面积测量", "icons/area.png", this::onMeasureArea),
                createButton("方位", "方位测量", "icons/bearing.png", this::onMeasureBearing)
        );

        RibbonGroup optionsGroup = new RibbonGroup();
        optionsGroup.setTitle("选项");
        optionsGroup.getNodes().addAll(
                createButton("设置", "系统设置", "icons/options.png", this::onOptions),
                createButton("主题", "主题设置", "icons/theme.png", this::onThemeSettings)
        );

        tab.getRibbonGroups().addAll(measureGroup, optionsGroup);
        return tab;
    }

    private Button createButton(String text, String tooltip, String iconPath, Runnable action) {
        Button button = new Button(text);
        button.setTooltip(new Tooltip(tooltip));
        
        ImageView icon = createIcon(iconPath, text);
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

    private ToggleButton createToggleButton(String text, String tooltip, Runnable action) {
        ToggleButton button = new ToggleButton(text);
        button.setTooltip(new Tooltip(tooltip));
        button.setSelected(true);
        
        button.setOnAction(e -> {
            if (action != null) {
                action.run();
            }
        });
        
        return button;
    }
    
    private ImageView createIcon(String iconPath, String fallbackText) {
        try {
            ImageView icon = new ImageView(new Image(getClass().getResourceAsStream(iconPath)));
            icon.setFitWidth(16);
            icon.setFitHeight(16);
            return icon;
        } catch (Exception e) {
            return createTextIcon(iconPath);
        }
    }
    
    private ImageView createTextIcon(String iconPath) {
        if (iconPath == null) return null;
        
        javafx.scene.canvas.Canvas canvas = new javafx.scene.canvas.Canvas(16, 16);
        javafx.scene.canvas.GraphicsContext gc = canvas.getGraphicsContext2D();
        
        gc.setStroke(javafx.scene.paint.Color.web("#555555"));
        gc.setFill(javafx.scene.paint.Color.web("#555555"));
        gc.setLineWidth(1.5);
        
        drawIcon(gc, iconPath);
        
        javafx.scene.SnapshotParameters params = new javafx.scene.SnapshotParameters();
        params.setFill(javafx.scene.paint.Color.TRANSPARENT);
        javafx.scene.image.WritableImage wi = canvas.snapshot(params, null);
        ImageView icon = new ImageView(wi);
        icon.setFitWidth(16);
        icon.setFitHeight(16);
        return icon;
    }
    
    private void drawIcon(javafx.scene.canvas.GraphicsContext gc, String iconPath) {
        if (iconPath.contains("new")) {
            gc.strokeRect(2, 2, 12, 12);
            gc.strokeLine(8, 5, 8, 11);
            gc.strokeLine(5, 8, 11, 8);
        } else if (iconPath.contains("open")) {
            gc.strokeRect(2, 4, 12, 10);
            gc.strokeLine(2, 4, 6, 4);
            gc.strokeLine(6, 4, 8, 2);
            gc.strokeLine(8, 2, 14, 2);
            gc.strokeLine(14, 2, 14, 4);
        } else if (iconPath.contains("save")) {
            gc.strokeRect(2, 2, 12, 12);
            gc.strokeRect(4, 2, 8, 4);
            gc.strokeRect(4, 8, 8, 6);
        } else if (iconPath.contains("export")) {
            gc.strokeRect(2, 6, 8, 8);
            gc.strokeLine(10, 10, 14, 10);
            gc.strokeLine(12, 8, 14, 10);
            gc.strokeLine(12, 8, 14, 10);
            gc.strokeLine(12, 12, 14, 10);
        } else if (iconPath.contains("print")) {
            gc.strokeRect(4, 8, 8, 6);
            gc.strokeRect(2, 4, 12, 4);
            gc.strokeLine(12, 4, 12, 2);
            gc.strokeLine(12, 2, 4, 2);
            gc.strokeLine(4, 2, 4, 4);
        } else if (iconPath.contains("import")) {
            gc.strokeRect(6, 6, 8, 8);
            gc.strokeLine(2, 10, 10, 10);
            gc.strokeLine(4, 8, 2, 10);
            gc.strokeLine(4, 12, 2, 10);
        } else if (iconPath.contains("zoom-in")) {
            gc.strokeOval(2, 2, 10, 10);
            gc.strokeLine(11, 11, 14, 14);
            gc.strokeLine(7, 4, 7, 10);
            gc.strokeLine(4, 7, 10, 7);
        } else if (iconPath.contains("zoom-out")) {
            gc.strokeOval(2, 2, 10, 10);
            gc.strokeLine(11, 11, 14, 14);
            gc.strokeLine(4, 7, 10, 7);
        } else if (iconPath.contains("fit")) {
            gc.strokeRect(2, 2, 12, 12);
            gc.strokeLine(2, 2, 6, 6);
            gc.strokeLine(14, 2, 10, 6);
            gc.strokeLine(2, 14, 6, 10);
            gc.strokeLine(14, 14, 10, 10);
        } else if (iconPath.contains("load")) {
            gc.strokeLine(8, 2, 8, 10);
            gc.strokeLine(5, 7, 8, 10);
            gc.strokeLine(11, 7, 8, 10);
            gc.strokeRect(2, 10, 12, 4);
        } else if (iconPath.contains("unload")) {
            gc.strokeLine(8, 14, 8, 6);
            gc.strokeLine(5, 9, 8, 6);
            gc.strokeLine(11, 9, 8, 6);
            gc.strokeRect(2, 2, 12, 4);
        } else if (iconPath.contains("layers")) {
            gc.strokePolygon(new double[]{8, 2, 8, 14}, new double[]{2, 5, 8, 5}, 4);
            gc.strokePolygon(new double[]{8, 14, 8, 2}, new double[]{8, 11, 14, 11}, 4);
        } else if (iconPath.contains("property")) {
            gc.strokeRect(2, 2, 12, 12);
            gc.strokeLine(4, 5, 12, 5);
            gc.strokeLine(4, 8, 12, 8);
            gc.strokeLine(4, 11, 8, 11);
        } else if (iconPath.contains("search")) {
            gc.strokeOval(2, 2, 10, 10);
            gc.strokeLine(11, 11, 14, 14);
        } else if (iconPath.contains("route")) {
            gc.strokeLine(2, 8, 6, 4);
            gc.strokeLine(6, 4, 10, 10);
            gc.strokeLine(10, 10, 14, 6);
            gc.fillOval(1, 7, 2, 2);
            gc.fillOval(13, 5, 2, 2);
        } else if (iconPath.contains("edit")) {
            gc.strokeLine(12, 2, 14, 4);
            gc.strokeLine(4, 12, 2, 14);
            gc.strokeLine(4, 12, 12, 4);
        } else if (iconPath.contains("delete")) {
            gc.strokeLine(4, 4, 12, 12);
            gc.strokeLine(12, 4, 4, 12);
            gc.strokeOval(2, 2, 12, 12);
        } else if (iconPath.contains("check")) {
            gc.strokeLine(2, 8, 6, 12);
            gc.strokeLine(6, 12, 14, 4);
        } else if (iconPath.contains("alarm")) {
            gc.strokeLine(8, 2, 2, 12);
            gc.strokeLine(8, 2, 14, 12);
            gc.strokeLine(2, 12, 14, 12);
            gc.fillOval(7, 5, 2, 2);
        } else if (iconPath.contains("settings")) {
            gc.strokeOval(4, 4, 8, 8);
            gc.strokeLine(8, 2, 8, 4);
            gc.strokeLine(8, 12, 8, 14);
            gc.strokeLine(2, 8, 4, 8);
            gc.strokeLine(12, 8, 14, 8);
        } else if (iconPath.contains("rules")) {
            gc.strokeRect(2, 2, 12, 12);
            gc.strokeLine(4, 5, 12, 5);
            gc.strokeLine(4, 8, 12, 8);
            gc.strokeLine(4, 11, 12, 11);
        } else if (iconPath.contains("history")) {
            gc.strokeOval(2, 2, 12, 12);
            gc.strokeLine(8, 4, 8, 8);
            gc.strokeLine(8, 8, 11, 10);
            gc.strokeLine(2, 8, 4, 6);
            gc.strokeLine(2, 8, 4, 10);
        } else if (iconPath.contains("statistics")) {
            gc.strokeLine(2, 12, 14, 12);
            gc.fillRect(3, 8, 3, 4);
            gc.fillRect(7, 4, 3, 8);
            gc.fillRect(11, 6, 3, 6);
        } else if (iconPath.contains("test")) {
            gc.strokeOval(2, 2, 12, 12);
            gc.strokeLine(5, 8, 7, 10);
            gc.strokeLine(7, 10, 11, 6);
        } else if (iconPath.contains("distance")) {
            gc.strokeLine(2, 8, 14, 8);
            gc.strokeLine(2, 6, 2, 10);
            gc.strokeLine(14, 6, 14, 10);
            gc.strokeLine(6, 5, 8, 5);
            gc.strokeLine(6, 11, 8, 11);
        } else if (iconPath.contains("area")) {
            gc.strokeRect(2, 2, 12, 12);
            gc.strokeLine(2, 2, 14, 14);
        } else if (iconPath.contains("bearing")) {
            gc.strokeOval(2, 2, 12, 12);
            gc.strokeLine(8, 8, 8, 3);
            gc.strokeLine(8, 8, 13, 8);
            gc.fillPolygon(new double[]{8, 6, 10}, new double[]{3, 6, 6}, 3);
        } else if (iconPath.contains("options")) {
            gc.strokeLine(2, 4, 10, 4);
            gc.strokeLine(2, 8, 10, 8);
            gc.strokeLine(2, 12, 10, 12);
            gc.fillOval(10, 3, 4, 2);
            gc.fillOval(6, 7, 4, 2);
            gc.fillOval(10, 11, 4, 2);
        } else if (iconPath.contains("theme")) {
            gc.strokeOval(4, 2, 8, 8);
            gc.fillArc(4, 2, 8, 8, 90, 180, javafx.scene.shape.ArcType.CHORD);
        }
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
