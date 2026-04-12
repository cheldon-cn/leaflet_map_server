package cn.cycle.app.controller;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.core.Viewport;
import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.geometry.Envelope;
import cn.cycle.chart.api.layer.LayerManager;
import cn.cycle.app.view.AlertPanel;
import cn.cycle.app.view.MainView;
import cn.cycle.app.model.DataItem;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleDoubleProperty;
import javafx.beans.property.DoubleProperty;
import javafx.scene.control.Alert;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

public class MainController {

    private ChartViewer chartViewer;
    private LayerManager layerManager;
    private AlertPanel alertPanel;
    private MainView mainView;
    private Runnable renderCallback;
    private Runnable statusUpdateCallback;
    private java.util.function.Consumer<String> imageLoadCallback;
    
    private final BooleanProperty layerPanelVisible = new SimpleBooleanProperty(true);
    private final BooleanProperty alertPanelVisible = new SimpleBooleanProperty(false);
    private final DoubleProperty currentZoom = new SimpleDoubleProperty(1.0);
    
    private ToolType currentTool = ToolType.SELECT;
    private MeasureMode measureMode = MeasureMode.NONE;
    private double measureStartX;
    private double measureStartY;

    public enum ToolType {
        SELECT,
        PAN,
        ZOOM_IN,
        ZOOM_OUT,
        DISTANCE_MEASURE,
        AREA_MEASURE,
        ROUTE_PLANNING
    }

    public enum MeasureMode {
        NONE,
        DISTANCE,
        AREA
    }

    public MainController(ChartViewer chartViewer) {
        this.chartViewer = chartViewer;
    }

    public void setChartViewer(ChartViewer viewer) {
        this.chartViewer = viewer;
    }

    public void setLayerManager(LayerManager manager) {
        this.layerManager = manager;
    }

    public void setAlertPanel(AlertPanel panel) {
        this.alertPanel = panel;
    }
    
    public void setMainView(MainView view) {
        this.mainView = view;
    }
    
    public void setRenderCallback(Runnable callback) {
        this.renderCallback = callback;
    }
    
    public void setStatusUpdateCallback(Runnable callback) {
        this.statusUpdateCallback = callback;
    }
    
    public void setImageLoadCallback(java.util.function.Consumer<String> callback) {
        this.imageLoadCallback = callback;
    }
    
    private void requestRender() {
        if (renderCallback != null) {
            renderCallback.run();
        }
    }
    
    private void updateStatus() {
        if (statusUpdateCallback != null) {
            statusUpdateCallback.run();
        }
    }
    
    private void addToDataCatalog(java.io.File file) {
        if (mainView != null && mainView.getDataCatalogPanel() != null) {
            javafx.application.Platform.runLater(() -> {
                mainView.getDataCatalogPanel().addFile(file);
                
                if (mainView.isPropertiesPanelVisible()) {
                    DataItem dataItem = new DataItem(file);
                    mainView.updatePropertiesPanel(dataItem);
                }
            });
        }
    }

    public void openChart() {
        System.out.println("[DEBUG] openChart() called");

        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("打开文件");
        fileChooser.getExtensionFilters().addAll(
            new FileChooser.ExtensionFilter("支持的文件", "*.000", "*.enc", "*.png", "*.jpg", "*.jpeg", "*.bmp", "*.gif"),
            new FileChooser.ExtensionFilter("海图文件", "*.000", "*.enc"),
            new FileChooser.ExtensionFilter("图片文件", "*.png", "*.jpg", "*.jpeg", "*.bmp", "*.gif"),
            new FileChooser.ExtensionFilter("所有文件", "*.*")
        );

        Stage stage = new Stage();
        java.io.File file = fileChooser.showOpenDialog(stage);
        if (file != null) {
            String filePath = file.getAbsolutePath();
            String fileName = file.getName().toLowerCase();
            System.out.println("[DEBUG] Selected file: " + filePath);
            
            boolean isImage = fileName.endsWith(".png") || fileName.endsWith(".jpg") || 
                              fileName.endsWith(".jpeg") || fileName.endsWith(".bmp") || 
                              fileName.endsWith(".gif");
            
            if (isImage) {
                if (imageLoadCallback != null) {
                    imageLoadCallback.accept(filePath);
                    if (alertPanel != null) {
                        alertPanel.addInfo("已打开图片: " + file.getName());
                    }
                    addToDataCatalog(file);
                }
            } else {
                if (chartViewer == null) {
                    System.out.println("[ERROR] chartViewer is null");
                    return;
                }
                if (chartViewer.isDisposed()) {
                    System.out.println("[ERROR] chartViewer is disposed");
                    return;
                }
                
                try {
                    System.out.println("[DEBUG] Calling chartViewer.loadChart()...");
                    int result = chartViewer.loadChart(filePath);
                    System.out.println("[DEBUG] loadChart() returned: " + result);
                    
                    if (result == 0) {
                        System.out.println("[DEBUG] Chart loaded successfully");
                        if (alertPanel != null) {
                            alertPanel.addInfo("已打开海图: " + file.getName());
                        }
                        
                        addToDataCatalog(file);
                        
                        System.out.println("[DEBUG] Getting full extent...");
                        Envelope extent = chartViewer.getFullExtent();
                        if (extent != null) {
                            System.out.println("[DEBUG] Full extent: " + extent.getMinX() + ", " + extent.getMinY() + ", " + extent.getMaxX() + ", " + extent.getMaxY());
                        } else {
                            System.out.println("[WARNING] Full extent is null");
                        }
                        
                        System.out.println("[DEBUG] Calling fitToWindow()...");
                        fitToWindow();
                        
                        System.out.println("[DEBUG] Requesting render...");
                        requestRender();
                        updateStatus();
                    } else {
                        System.out.println("[ERROR] loadChart() failed with code: " + result);
                        if (alertPanel != null) {
                            alertPanel.addError("打开海图失败，错误码: " + result);
                        }
                    }
                } catch (Exception e) {
                    System.out.println("[ERROR] Exception in openChart: " + e.getMessage());
                    e.printStackTrace();
                    if (alertPanel != null) {
                        alertPanel.addError("打开海图失败: " + e.getMessage());
                    }
                }
            }
        } else {
            System.out.println("[DEBUG] No file selected");
        }
    }

    public void saveChart() {
        if (chartViewer == null || chartViewer.isDisposed()) {
            return;
        }
        System.out.println("保存海图功能暂未实现");
        if (alertPanel != null) {
            alertPanel.addInfo("保存海图功能暂未实现");
        }
    }

    public void closeChart() {
        if (chartViewer != null && !chartViewer.isDisposed()) {
            chartViewer.dispose();
            if (alertPanel != null) {
                alertPanel.addInfo("已关闭海图");
            }
        }
    }

    public void zoomIn() {
        if (chartViewer == null || chartViewer.isDisposed()) {
            return;
        }
        Viewport viewport = chartViewer.getViewportObject();
        if (viewport != null) {
            viewport.zoom(1.5);
            currentZoom.set(currentZoom.get() * 1.5);
        }
    }

    public void zoomOut() {
        if (chartViewer == null || chartViewer.isDisposed()) {
            return;
        }
        Viewport viewport = chartViewer.getViewportObject();
        if (viewport != null) {
            viewport.zoom(1.0 / 1.5);
            currentZoom.set(currentZoom.get() / 1.5);
        }
    }

    public void fitToWindow() {
        if (chartViewer == null || chartViewer.isDisposed()) {
            return;
        }
        Viewport viewport = chartViewer.getViewportObject();
        if (viewport != null) {
            Envelope extent = chartViewer.getFullExtent();
            if (extent != null) {
                viewport.setExtent(extent);
                currentZoom.set(1.0);
            }
        }
    }

    public void toggleLayerPanel() {
        layerPanelVisible.set(!layerPanelVisible.get());
    }

    public void copySelection() {
        System.out.println("复制选择...");
    }

    public void startDistanceMeasure() {
        currentTool = ToolType.DISTANCE_MEASURE;
        measureMode = MeasureMode.DISTANCE;
        if (alertPanel != null) {
            alertPanel.addInfo("距离测量模式已启用，点击地图开始测量");
        }
    }

    public void startAreaMeasure() {
        currentTool = ToolType.AREA_MEASURE;
        measureMode = MeasureMode.AREA;
        if (alertPanel != null) {
            alertPanel.addInfo("面积测量模式已启用，点击地图开始测量");
        }
    }

    public void openRoutePlanner() {
        currentTool = ToolType.ROUTE_PLANNING;
        if (alertPanel != null) {
            alertPanel.addInfo("航线规划模式已启用");
        }
    }

    public void openSettings() {
        System.out.println("打开设置...");
    }

    public void showAbout() {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle("关于");
        alert.setHeaderText("Cycle Chart Viewer");
        alert.setContentText("版本: 1.0.0\n\n基于OGC标准的海图显示系统");
        alert.showAndWait();
    }

    public void exitApplication() {
        if (chartViewer != null && !chartViewer.isDisposed()) {
            chartViewer.dispose();
        }
        System.exit(0);
    }

    public void onMouseClick(double screenX, double screenY) {
        switch (currentTool) {
            case DISTANCE_MEASURE:
                handleDistanceMeasure(screenX, screenY);
                break;
            case AREA_MEASURE:
                handleAreaMeasure(screenX, screenY);
                break;
            case ROUTE_PLANNING:
                handleRoutePlanning(screenX, screenY);
                break;
            default:
                break;
        }
    }

    private void handleDistanceMeasure(double x, double y) {
        if (measureStartX == 0 && measureStartY == 0) {
            measureStartX = x;
            measureStartY = y;
            if (alertPanel != null) {
                alertPanel.addInfo("起点已设置，点击终点完成测量");
            }
        } else {
            double distance = calculateDistance(measureStartX, measureStartY, x, y);
            if (alertPanel != null) {
                alertPanel.addInfo(String.format("测量距离: %.2f 米", distance));
            }
            measureStartX = 0;
            measureStartY = 0;
            measureMode = MeasureMode.NONE;
        }
    }

    private void handleAreaMeasure(double x, double y) {
        System.out.println("面积测量: " + x + ", " + y);
    }

    private void handleRoutePlanning(double x, double y) {
        System.out.println("航线规划: " + x + ", " + y);
    }

    private double calculateDistance(double x1, double y1, double x2, double y2) {
        if (chartViewer != null) {
            Coordinate coord1 = chartViewer.screenToWorld(x1, y1);
            Coordinate coord2 = chartViewer.screenToWorld(x2, y2);
            if (coord1 != null && coord2 != null) {
                return coord1.distance(coord2);
            }
        }
        return Math.sqrt(Math.pow(x2 - x1, 2) + Math.pow(y2 - y1, 2));
    }

    public boolean isLayerPanelVisible() {
        return layerPanelVisible.get();
    }

    public BooleanProperty layerPanelVisibleProperty() {
        return layerPanelVisible;
    }

    public boolean isAlertPanelVisible() {
        return alertPanelVisible.get();
    }

    public BooleanProperty alertPanelVisibleProperty() {
        return alertPanelVisible;
    }

    public double getCurrentZoom() {
        return currentZoom.get();
    }

    public DoubleProperty currentZoomProperty() {
        return currentZoom;
    }

    public ToolType getCurrentTool() {
        return currentTool;
    }

    public void setCurrentTool(ToolType tool) {
        this.currentTool = tool;
    }

    public void setZoomRatio(double ratio) {
        if (chartViewer == null || chartViewer.isDisposed()) {
            return;
        }
        Viewport viewport = chartViewer.getViewportObject();
        if (viewport != null) {
            viewport.setScale(ratio);
            currentZoom.set(ratio);
        }
    }

    public void setTheme(String theme) {
        System.out.println("设置主题: " + theme);
        if (alertPanel != null) {
            alertPanel.addInfo("已切换到" + theme + "主题");
        }
    }
    
    public void undo() {
        System.out.println("撤销操作");
        if (alertPanel != null) {
            alertPanel.addInfo("撤销操作");
        }
    }
    
    public void redo() {
        System.out.println("重做操作");
        if (alertPanel != null) {
            alertPanel.addInfo("重做操作");
        }
    }

    public void showHelp() {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle("帮助");
        alert.setHeaderText("Cycle Chart Viewer 帮助");
        alert.setContentText("快捷键:\n" +
            "Ctrl+O: 打开海图\n" +
            "Ctrl+S: 保存海图\n" +
            "+: 放大\n" +
            "-: 缩小\n" +
            "F: 适应窗口\n\n" +
            "详细帮助请访问: https://cycle.cn/help");
        alert.showAndWait();
    }
    
    public void loadChart(String filePath) {
        if (chartViewer == null || chartViewer.isDisposed()) {
            System.out.println("[ERROR] chartViewer is null or disposed");
            return;
        }
        
        try {
            int result = chartViewer.loadChart(filePath);
            if (result == 0) {
                fitToWindow();
                requestRender();
                updateStatus();
            } else {
                System.out.println("[ERROR] loadChart() failed with code: " + result);
            }
        } catch (Exception e) {
            System.out.println("[ERROR] Exception in loadChart: " + e.getMessage());
            e.printStackTrace();
        }
    }
}
