package cn.cycle.app.controller;

import cn.cycle.chart.api.core.ChartViewer;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

public class MainController {

    private ChartViewer chartViewer;

    public MainController(ChartViewer chartViewer) {
        this.chartViewer = chartViewer;
    }

    public void setChartViewer(ChartViewer viewer) {
        this.chartViewer = viewer;
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
        java.io.File selectedFile = fileChooser.showOpenDialog(stage);
        if (selectedFile != null) {
            int result = chartViewer.loadChart(selectedFile.getAbsolutePath());
            if (result != 0) {
                System.err.println("Failed to load chart: " + selectedFile.getAbsolutePath());
            }
        }
    }

    public void copySelection() {
    }

    public void zoomIn() {
        if (chartViewer == null || chartViewer.isDisposed()) {
            return;
        }
        chartViewer.zoom(1.5, 0, 0);
    }

    public void zoomOut() {
        if (chartViewer == null || chartViewer.isDisposed()) {
            return;
        }
        chartViewer.zoom(0.67, 0, 0);
    }

    public void fitToWindow() {
        if (chartViewer == null || chartViewer.isDisposed()) {
            return;
        }
        double[] viewport = chartViewer.getViewport();
        chartViewer.setViewport(viewport[0], viewport[1], 1.0);
    }

    public void toggleLayerPanel() {
    }

    public void startDistanceMeasure() {
    }

    public void startAreaMeasure() {
    }

    public void openRoutePlanner() {
    }
}
