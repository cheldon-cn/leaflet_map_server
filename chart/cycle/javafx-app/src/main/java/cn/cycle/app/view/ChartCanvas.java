package cn.cycle.app.view;

import cn.cycle.chart.api.adapter.CanvasAdapter;
import cn.cycle.chart.api.adapter.PanHandler;
import cn.cycle.chart.api.adapter.ZoomHandler;
import cn.cycle.chart.api.core.ChartViewer;
import javafx.scene.canvas.Canvas;

public class ChartCanvas extends Canvas {

    private ChartViewer chartViewer;
    private CanvasAdapter canvasAdapter;
    private PanHandler panHandler;
    private ZoomHandler zoomHandler;

    public ChartCanvas(ChartViewer chartViewer) {
        this.chartViewer = chartViewer;
        setWidth(800);
        setHeight(600);

        setupAdapter();
    }

    private void setupAdapter() {
        if (chartViewer != null) {
            canvasAdapter = new CanvasAdapter(this, chartViewer);
            
            panHandler = new PanHandler();
            zoomHandler = new ZoomHandler();
            
            canvasAdapter.addHandler(panHandler);
            canvasAdapter.addHandler(zoomHandler);

            widthProperty().addListener(obs -> render());
            heightProperty().addListener(obs -> render());
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
