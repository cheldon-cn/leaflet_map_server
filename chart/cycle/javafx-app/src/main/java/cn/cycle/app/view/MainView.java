package cn.cycle.app.view;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.app.controller.MainController;
import com.pixelduke.control.Ribbon;
import com.pixelduke.control.ribbon.RibbonGroup;
import com.pixelduke.control.ribbon.RibbonTab;
import javafx.scene.control.Button;
import javafx.scene.layout.BorderPane;

public class MainView extends BorderPane {

    private final MainController controller;
    private final ChartCanvas chartCanvas;
    private final Ribbon ribbon;

    public MainView(ChartViewer chartViewer) {
        this.controller = new MainController(chartViewer);
        this.chartCanvas = new ChartCanvas(chartViewer);
        this.ribbon = createRibbon();

        setTop(ribbon);
        setCenter(chartCanvas);
    }

    private Ribbon createRibbon() {
        Ribbon ribbon = new Ribbon();

        RibbonTab homeTab = createHomeTab();
        RibbonTab viewTab = createViewTab();
        RibbonTab toolsTab = createToolsTab();

        ribbon.getTabs().addAll(homeTab, viewTab, toolsTab);

        return ribbon;
    }

    private RibbonTab createHomeTab() {
        RibbonTab tab = new RibbonTab("主页");

        RibbonGroup fileGroup = new RibbonGroup();
        fileGroup.setTitle("文件");
        Button openButton = new Button("打开海图");
        openButton.setOnAction(e -> controller.openChart());
        fileGroup.getNodes().add(openButton);

        RibbonGroup editGroup = new RibbonGroup();
        editGroup.setTitle("编辑");
        Button copyButton = new Button("复制");
        copyButton.setOnAction(e -> controller.copySelection());
        editGroup.getNodes().add(copyButton);

        tab.getRibbonGroups().addAll(fileGroup, editGroup);

        return tab;
    }

    private RibbonTab createViewTab() {
        RibbonTab tab = new RibbonTab("视图");

        RibbonGroup zoomGroup = new RibbonGroup();
        zoomGroup.setTitle("缩放");
        Button zoomInButton = new Button("放大");
        zoomInButton.setOnAction(e -> controller.zoomIn());

        Button zoomOutButton = new Button("缩小");
        zoomOutButton.setOnAction(e -> controller.zoomOut());

        Button fitButton = new Button("适应窗口");
        fitButton.setOnAction(e -> controller.fitToWindow());

        zoomGroup.getNodes().addAll(zoomInButton, zoomOutButton, fitButton);

        RibbonGroup layerGroup = new RibbonGroup();
        layerGroup.setTitle("图层");
        Button layerPanelButton = new Button("图层面板");
        layerPanelButton.setOnAction(e -> controller.toggleLayerPanel());
        layerGroup.getNodes().add(layerPanelButton);

        tab.getRibbonGroups().addAll(zoomGroup, layerGroup);

        return tab;
    }

    private RibbonTab createToolsTab() {
        RibbonTab tab = new RibbonTab("工具");

        RibbonGroup measureGroup = new RibbonGroup();
        measureGroup.setTitle("测量");
        Button distanceButton = new Button("距离测量");
        distanceButton.setOnAction(e -> controller.startDistanceMeasure());

        Button areaButton = new Button("面积测量");
        areaButton.setOnAction(e -> controller.startAreaMeasure());

        measureGroup.getNodes().addAll(distanceButton, areaButton);

        RibbonGroup navGroup = new RibbonGroup();
        navGroup.setTitle("导航");
        Button routeButton = new Button("航线规划");
        routeButton.setOnAction(e -> controller.openRoutePlanner());
        navGroup.getNodes().add(routeButton);

        tab.getRibbonGroups().addAll(measureGroup, navGroup);

        return tab;
    }

    public ChartCanvas getChartCanvas() {
        return chartCanvas;
    }

    public MainController getController() {
        return controller;
    }
}
