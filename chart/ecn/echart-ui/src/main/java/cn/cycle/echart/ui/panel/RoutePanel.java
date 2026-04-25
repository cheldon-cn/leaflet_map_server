package cn.cycle.echart.ui.panel;

import cn.cycle.echart.ui.FxRightTabPanel;
import cn.cycle.echart.facade.RouteFacade;
import cn.cycle.echart.route.Route;
import cn.cycle.echart.route.Waypoint;
import javafx.scene.control.*;
import javafx.scene.layout.BorderPane;
import javafx.stage.FileChooser;

import java.io.File;
import java.util.List;
import java.util.Objects;

/**
 * 航线面板。
 * 
 * <p>显示和管理航线信息。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class RoutePanel extends BorderPane implements FxRightTabPanel {

    private static final String TAB_ID = "route-panel";
    
    private final ListView<Route> routeListView;
    private final ListView<Waypoint> waypointListView;
    private final TabPane tabPane;
    private final Tab routesTab;
    private final Tab waypointsTab;
    
    private RouteFacade routeFacade;
    private final Tab tab;
    
    private Route selectedRoute;

    public RoutePanel() {
        this(null);
    }

    public RoutePanel(RouteFacade routeFacade) {
        this.routeFacade = routeFacade;
        this.routeListView = new ListView<>();
        this.waypointListView = new ListView<>();
        this.tabPane = new TabPane();
        this.routesTab = new Tab("航线列表");
        this.waypointsTab = new Tab("航路点");
        this.tab = new Tab("航线");
        this.tab.setContent(this);
        this.tab.setClosable(false);
        
        initializeLayout();
        if (routeFacade != null) {
            loadData();
        }
    }
    
    public void setRouteFacade(RouteFacade routeFacade) {
        this.routeFacade = routeFacade;
        if (routeFacade != null) {
            loadData();
        }
    }

    private void initializeLayout() {
        routesTab.setContent(routeListView);
        routesTab.setClosable(false);
        
        waypointsTab.setContent(waypointListView);
        waypointsTab.setClosable(false);
        
        tabPane.getTabs().addAll(routesTab, waypointsTab);
        
        routeListView.setCellFactory(param -> new ListCell<Route>() {
            @Override
            protected void updateItem(Route route, boolean empty) {
                super.updateItem(route, empty);
                setText(empty || route == null ? null : route.getName());
            }
        });
        
        waypointListView.setCellFactory(param -> new ListCell<Waypoint>() {
            @Override
            protected void updateItem(Waypoint point, boolean empty) {
                super.updateItem(point, empty);
                if (empty || point == null) {
                    setText(null);
                } else {
                    setText(String.format("%s: (%.4f, %.4f)", 
                            point.getName(), point.getLatitude(), point.getLongitude()));
                }
            }
        });
        
        routeListView.getSelectionModel().selectedItemProperty().addListener(
                (obs, oldVal, newVal) -> selectRoute(newVal));
        
        setTop(createToolBar());
        setCenter(tabPane);
        
        getStyleClass().add("route-panel");
    }

    private ToolBar createToolBar() {
        ToolBar toolBar = new ToolBar();
        
        Button addButton = new Button("添加");
        addButton.setOnAction(e -> addRoute());
        
        Button editButton = new Button("编辑");
        editButton.setOnAction(e -> editRoute());
        
        Button deleteButton = new Button("删除");
        deleteButton.setOnAction(e -> deleteRoute());
        
        Button importButton = new Button("导入");
        importButton.setOnAction(e -> importRoute());
        
        Button exportButton = new Button("导出");
        exportButton.setOnAction(e -> exportRoute());
        
        toolBar.getItems().addAll(addButton, editButton, deleteButton, 
                new Separator(), importButton, exportButton);
        return toolBar;
    }

    private void loadData() {
        routeListView.getItems().clear();
        
        List<Route> routes = routeFacade.getAllRoutes();
        routeListView.getItems().addAll(routes);
    }

    private void selectRoute(Route route) {
        selectedRoute = route;
        
        waypointListView.getItems().clear();
        
        if (route != null) {
            List<Waypoint> points = route.getWaypoints();
            waypointListView.getItems().addAll(points);
        }
    }

    private void addRoute() {
        TextInputDialog dialog = new TextInputDialog();
        dialog.setTitle("添加航线");
        dialog.setHeaderText("请输入航线名称");
        dialog.setContentText("名称:");
        
        dialog.showAndWait().ifPresent(name -> {
            Route route = routeFacade.createRoute(name);
            routeListView.getItems().add(route);
            routeListView.getSelectionModel().select(route);
        });
    }

    private void editRoute() {
        Route selected = routeListView.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showWarning("请先选择一条航线");
            return;
        }
        
        TextInputDialog dialog = new TextInputDialog(selected.getName());
        dialog.setTitle("编辑航线");
        dialog.setHeaderText("修改航线名称");
        dialog.setContentText("名称:");
        
        dialog.showAndWait().ifPresent(name -> {
            selected.setName(name);
            routeListView.refresh();
        });
    }

    private void deleteRoute() {
        Route selected = routeListView.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showWarning("请先选择一条航线");
            return;
        }
        
        Alert alert = new Alert(Alert.AlertType.CONFIRMATION);
        alert.setTitle("删除航线");
        alert.setHeaderText("确认删除航线: " + selected.getName());
        alert.setContentText("此操作不可撤销");
        
        alert.showAndWait().ifPresent(response -> {
            if (response == ButtonType.OK) {
                routeFacade.deleteRoute(selected.getId());
                routeListView.getItems().remove(selected);
                waypointListView.getItems().clear();
            }
        });
    }

    private void importRoute() {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("导入航线");
        fileChooser.getExtensionFilters().addAll(
                new FileChooser.ExtensionFilter("航线文件", "*.route", "*.gpx"),
                new FileChooser.ExtensionFilter("所有文件", "*.*"));
        
        File file = fileChooser.showOpenDialog(getScene().getWindow());
        if (file != null) {
            showInfo("航线导入功能暂未实现");
        }
    }

    private void exportRoute() {
        Route selected = routeListView.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showWarning("请先选择一条航线");
            return;
        }
        
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("导出航线");
        fileChooser.setInitialFileName(selected.getName() + ".route");
        fileChooser.getExtensionFilters().add(
                new FileChooser.ExtensionFilter("航线文件", "*.route"));
        
        File file = fileChooser.showSaveDialog(getScene().getWindow());
        if (file != null) {
            showInfo("航线导出功能暂未实现");
        }
    }

    private void showInfo(String message) {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle("信息");
        alert.setContentText(message);
        alert.showAndWait();
    }

    private void showWarning(String message) {
        Alert alert = new Alert(Alert.AlertType.WARNING);
        alert.setTitle("警告");
        alert.setContentText(message);
        alert.showAndWait();
    }

    private void showError(String title, String message) {
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle(title);
        alert.setContentText(message);
        alert.showAndWait();
    }

    @Override
    public String getTabId() {
        return TAB_ID;
    }

    @Override
    public String getIcon() {
        return "/icons/route.png";
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
        loadData();
    }

    @Override
    public void onDeselected() {
    }

    @Override
    public void refresh() {
        loadData();
    }
}
