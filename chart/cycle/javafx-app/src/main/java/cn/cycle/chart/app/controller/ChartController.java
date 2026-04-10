package cn.cycle.chart.app.controller;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.core.Viewport;
import cn.cycle.chart.api.geometry.Envelope;
import cn.cycle.chart.api.layer.LayerManager;
import cn.cycle.chart.app.model.ChartModel;
import cn.cycle.app.view.AlertPanel;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.DoubleProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleDoubleProperty;
import javafx.application.Platform;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

import java.io.File;
import java.util.concurrent.CompletableFuture;

public class ChartController {

    private ChartViewer chartViewer;
    private LayerManager layerManager;
    private ChartModel chartModel;
    private AlertPanel alertPanel;
    
    private final BooleanProperty loading = new SimpleBooleanProperty(false);
    private final DoubleProperty loadProgress = new SimpleDoubleProperty(0);
    
    public ChartController(ChartModel model) {
        this.chartModel = model;
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
    
    public ChartModel getChartModel() {
        return chartModel;
    }
    
    public BooleanProperty loadingProperty() {
        return loading;
    }
    
    public DoubleProperty loadProgressProperty() {
        return loadProgress;
    }

    public void openChart() {
        if (chartViewer == null || chartViewer.isDisposed()) {
            return;
        }

        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("打开海图文件");
        fileChooser.getExtensionFilters().addAll(
            new FileChooser.ExtensionFilter("海图文件", "*.000", "*.enc"),
            new FileChooser.ExtensionFilter("所有文件", "*.*")
        );

        Stage stage = new Stage();
        File file = fileChooser.showOpenDialog(stage);
        if (file != null) {
            loadChart(file.getAbsolutePath());
        }
    }
    
    public void loadChart(String filePath) {
        if (chartViewer == null || chartViewer.isDisposed()) {
            return;
        }
        
        loading.set(true);
        loadProgress.set(0);
        chartModel.setRendering(true);
        chartModel.setChartPath(filePath);
        
        CompletableFuture.runAsync(() -> {
            try {
                int result = chartViewer.loadChart(filePath);
                
                if (result == 0) {
                    File file = new File(filePath);
                    String chartName = file.getName();
                    
                    Platform.runLater(() -> {
                        chartModel.setChartName(chartName);
                        chartModel.setLoaded(true);
                        
                        Viewport viewport = chartViewer.getViewportObject();
                        if (viewport != null) {
                            Envelope extent = chartViewer.getFullExtent();
                            if (extent != null) {
                                viewport.setExtent(extent);
                            }
                        }
                        
                        loadProgress.set(1.0);
                        if (alertPanel != null) {
                            alertPanel.addInfo("已加载海图: " + chartName);
                        }
                    });
                } else {
                    Platform.runLater(() -> {
                        if (alertPanel != null) {
                            alertPanel.addError("加载海图失败，错误码: " + result);
                        }
                    });
                }
            } catch (Exception e) {
                Platform.runLater(() -> {
                    if (alertPanel != null) {
                        alertPanel.addError("加载海图异常: " + e.getMessage());
                    }
                });
            } finally {
                Platform.runLater(() -> {
                    loading.set(false);
                    chartModel.setRendering(false);
                });
            }
        });
    }
    
    public CompletableFuture<Void> loadChartAsync(String filePath) {
        return CompletableFuture.runAsync(() -> loadChart(filePath));
    }

    public void closeChart() {
        if (chartViewer != null && !chartViewer.isDisposed()) {
            chartViewer.shutdown();
            chartModel.reset();
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
            chartModel.setScale(chartModel.getScale() * 1.5);
        }
    }

    public void zoomOut() {
        if (chartViewer == null || chartViewer.isDisposed()) {
            return;
        }
        Viewport viewport = chartViewer.getViewportObject();
        if (viewport != null) {
            viewport.zoom(1.0 / 1.5);
            chartModel.setScale(chartModel.getScale() / 1.5);
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
                chartModel.setScale(1.0);
            }
        }
    }
    
    public void pan(double dx, double dy) {
        if (chartViewer == null || chartViewer.isDisposed()) {
            return;
        }
        Viewport viewport = chartViewer.getViewportObject();
        if (viewport != null) {
            viewport.pan(dx, dy);
        }
    }
    
    public void setCenter(double x, double y) {
        chartModel.setCenterX(x);
        chartModel.setCenterY(y);
    }
    
    public void refresh() {
        if (chartViewer != null && !chartViewer.isDisposed()) {
            chartModel.setRendering(true);
        }
    }
}
