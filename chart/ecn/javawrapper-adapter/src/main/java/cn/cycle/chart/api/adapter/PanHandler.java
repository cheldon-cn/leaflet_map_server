package cn.cycle.chart.api.adapter;

import cn.cycle.chart.api.core.Viewport;
import cn.cycle.chart.api.geometry.Envelope;

public class PanHandler implements ChartEventHandler.MouseHandler {

    private CanvasAdapter adapter;
    private double lastX;
    private double lastY;
    private boolean panning;

    @Override
    public void install(CanvasAdapter adapter) {
        this.adapter = adapter;
        this.panning = false;
    }

    @Override
    public void uninstall() {
        this.adapter = null;
        this.panning = false;
    }

    @Override
    public void onMousePressed(ChartMouseEvent event) {
        if (event.isPrimaryButtonDown() && !event.isControlDown()) {
            lastX = event.getScreenX();
            lastY = event.getScreenY();
            panning = true;
        }
    }

    @Override
    public void onMouseReleased(ChartMouseEvent event) {
        panning = false;
    }

    @Override
    public void onMouseMoved(ChartMouseEvent event) {
    }

    @Override
    public void onMouseDragged(ChartMouseEvent event) {
        if (panning && adapter != null) {
            double dx = event.getScreenX() - lastX;
            double dy = event.getScreenY() - lastY;

            Viewport viewport = adapter.getViewer().getViewportObject();
            if (viewport != null) {
                Envelope extent = viewport.getExtent();
                if (extent != null) {
                    double width = adapter.getCanvas().getWidth();
                    double height = adapter.getCanvas().getHeight();
                    double worldDx = -dx * extent.getWidth() / width;
                    double worldDy = dy * extent.getHeight() / height;
                    viewport.pan(worldDx, worldDy);
                    adapter.render();
                }
            }

            lastX = event.getScreenX();
            lastY = event.getScreenY();
        }
    }

    @Override
    public void onMouseClicked(ChartMouseEvent event) {
    }

    public boolean isPanning() {
        return panning;
    }
}
