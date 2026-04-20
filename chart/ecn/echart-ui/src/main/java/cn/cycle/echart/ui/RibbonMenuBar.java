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
        
        try {
            ImageView icon = new ImageView(new Image(getClass().getResourceAsStream(iconPath)));
            icon.setFitWidth(16);
            icon.setFitHeight(16);
            button.setGraphic(icon);
        } catch (Exception e) {
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

    protected void onNewWorkspace() {}
    protected void onOpenWorkspace() {}
    protected void onSaveWorkspace() {}
    protected void onExport() {}
    protected void onPrint() {}
    
    protected void onZoomIn() {}
    protected void onZoomOut() {}
    protected void onFitToWindow() {}
    protected void onToggleSideBar() {}
    protected void onToggleRightTab() {}
    protected void onToggleStatusBar() {}
    
    protected void onLoadChart() {}
    protected void onUnloadChart() {}
    protected void onLayerManager() {}
    protected void onPropertyQuery() {}
    protected void onFeatureSearch() {}
    
    protected void onCreateRoute() {}
    protected void onEditRoute() {}
    protected void onDeleteRoute() {}
    protected void onImportRoute() {}
    protected void onExportRoute() {}
    protected void onCheckRoute() {}
    
    protected void onAlarmSettings() {}
    protected void onAlarmRules() {}
    protected void onAlarmHistory() {}
    protected void onAlarmStatistics() {}
    protected void onAlarmTest() {}
    
    protected void onMeasureDistance() {}
    protected void onMeasureArea() {}
    protected void onMeasureBearing() {}
    protected void onOptions() {}
    protected void onThemeSettings() {}
}
