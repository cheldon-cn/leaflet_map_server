package cn.cycle.app.view;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.app.controller.MainController;
import javafx.scene.control.Button;
import javafx.scene.control.Menu;
import javafx.scene.control.MenuBar;
import javafx.scene.control.MenuItem;
import javafx.scene.control.SeparatorMenuItem;
import javafx.scene.control.ToolBar;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;

public class MainView extends BorderPane {

    private final MainController controller;
    private final ChartCanvas chartCanvas;
    private final MenuBar menuBar;
    private final ToolBar toolBar;

    public MainView(ChartViewer chartViewer) {
        this.controller = new MainController(chartViewer);
        this.chartCanvas = new ChartCanvas(chartViewer);
        this.menuBar = createMenuBar();
        this.toolBar = createToolBar();

        HBox topBox = new HBox();
        topBox.getChildren().addAll(menuBar, toolBar);
        
        setTop(topBox);
        setCenter(chartCanvas);
    }

    private MenuBar createMenuBar() {
        MenuBar menuBar = new MenuBar();

        Menu fileMenu = new Menu("文件");
        MenuItem openItem = new MenuItem("打开海图");
        openItem.setOnAction(e -> controller.openChart());
        MenuItem exitItem = new MenuItem("退出");
        fileMenu.getItems().addAll(openItem, new SeparatorMenuItem(), exitItem);

        Menu editMenu = new Menu("编辑");
        MenuItem copyItem = new MenuItem("复制");
        copyItem.setOnAction(e -> controller.copySelection());
        editMenu.getItems().add(copyItem);

        Menu viewMenu = new Menu("视图");
        MenuItem zoomInItem = new MenuItem("放大");
        zoomInItem.setOnAction(e -> controller.zoomIn());
        MenuItem zoomOutItem = new MenuItem("缩小");
        zoomOutItem.setOnAction(e -> controller.zoomOut());
        MenuItem fitItem = new MenuItem("适应窗口");
        fitItem.setOnAction(e -> controller.fitToWindow());
        viewMenu.getItems().addAll(zoomInItem, zoomOutItem, fitItem);

        Menu toolsMenu = new Menu("工具");
        MenuItem distanceItem = new MenuItem("距离测量");
        distanceItem.setOnAction(e -> controller.startDistanceMeasure());
        MenuItem areaItem = new MenuItem("面积测量");
        areaItem.setOnAction(e -> controller.startAreaMeasure());
        MenuItem routeItem = new MenuItem("航线规划");
        routeItem.setOnAction(e -> controller.openRoutePlanner());
        toolsMenu.getItems().addAll(distanceItem, areaItem, routeItem);

        menuBar.getMenus().addAll(fileMenu, editMenu, viewMenu, toolsMenu);

        return menuBar;
    }

    private ToolBar createToolBar() {
        ToolBar toolBar = new ToolBar();

        Button openButton = new Button("打开");
        openButton.setOnAction(e -> controller.openChart());

        Button zoomInButton = new Button("放大");
        zoomInButton.setOnAction(e -> controller.zoomIn());

        Button zoomOutButton = new Button("缩小");
        zoomOutButton.setOnAction(e -> controller.zoomOut());

        Button fitButton = new Button("适应");
        fitButton.setOnAction(e -> controller.fitToWindow());

        Button layerButton = new Button("图层");
        layerButton.setOnAction(e -> controller.toggleLayerPanel());

        toolBar.getItems().addAll(openButton, zoomInButton, zoomOutButton, fitButton, layerButton);

        return toolBar;
    }

    public ChartCanvas getChartCanvas() {
        return chartCanvas;
    }

    public MainController getController() {
        return controller;
    }
}
