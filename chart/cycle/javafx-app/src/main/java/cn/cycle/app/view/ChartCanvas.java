package cn.cycle.app.view;

import cn.cycle.chart.api.adapter.CanvasAdapter;
import cn.cycle.chart.api.adapter.PanHandler;
import cn.cycle.chart.api.adapter.ZoomHandler;
import cn.cycle.chart.api.core.ChartViewer;
import javafx.scene.canvas.Canvas;
import javafx.scene.layout.Pane;

public class ChartCanvas extends Pane {

    private Canvas canvas;
    private ChartViewer chartViewer;
    private CanvasAdapter canvasAdapter;
    private PanHandler panHandler;
    private ZoomHandler zoomHandler;

    public ChartCanvas(ChartViewer chartViewer) {
        this.chartViewer = chartViewer;
        this.canvas = new Canvas();
        getChildren().add(canvas);
        
        canvas.widthProperty().bind(widthProperty());
        canvas.heightProperty().bind(heightProperty());
        
        widthProperty().addListener(obs -> render());
        heightProperty().addListener(obs -> render());

        setupAdapter();
        setStyle("-fx-background-color: white;");
    }

    private void setupAdapter() {
        if (chartViewer != null && canvas != null) {
            canvasAdapter = new CanvasAdapter(canvas, chartViewer);
            
            panHandler = new PanHandler();
            zoomHandler = new ZoomHandler();
            
            canvasAdapter.addHandler(panHandler);
            canvasAdapter.addHandler(zoomHandler);
        }
    }

    public void setChartViewer(ChartViewer viewer) {
        if (canvasAdapter != null) {
            canvasAdapter.dispose();
        }
        this.chartViewer = viewer;
        setupAdapter();
        render();
    }

    public void render() {
        if (canvasAdapter != null && !canvasAdapter.isDisposed()) {
            canvasAdapter.render();
        }
    }

    public void clear() {
        if (canvasAdapter != null) {
            canvasAdapter.clear();
        }
    }

    public CanvasAdapter getCanvasAdapter() {
        return canvasAdapter;
    }

    public void dispose() {
        if (canvasAdapter != null) {
            canvasAdapter.dispose();
            canvasAdapter = null;
        }
    }

    public boolean isDisposed() {
        return canvasAdapter == null || canvasAdapter.isDisposed();
    }
}
