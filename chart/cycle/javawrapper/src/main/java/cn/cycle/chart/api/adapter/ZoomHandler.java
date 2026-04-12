package cn.cycle.chart.api.adapter;

import cn.cycle.chart.api.core.Viewport;
import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.geometry.Envelope;

public class ZoomHandler implements ChartEventHandler.ScrollHandler, ChartEventHandler.MouseHandler {

    private CanvasAdapter adapter;
    private double zoomFactor;
    private double minZoom;
    private double maxZoom;
    private double lastY;

    public ZoomHandler() {
        this(1.2, 0.01, 100.0);
    }

    public ZoomHandler(double zoomFactor, double minZoom, double maxZoom) {
        this.zoomFactor = zoomFactor;
        this.minZoom = minZoom;
        this.maxZoom = maxZoom;
        this.lastY = 0;
    }

    @Override
    public void install(CanvasAdapter adapter) {
        this.adapter = adapter;
    }

    @Override
    public void uninstall() {
        this.adapter = null;
    }

    @Override
    public void onScroll(ChartScrollEvent event) {
        if (adapter == null) {
            return;
        }

        Viewport viewport = adapter.getViewer().getViewportObject();
        if (viewport == null) {
            return;
        }

        double delta = event.getDelta();
        double factor = delta > 0 ? zoomFactor : 1.0 / zoomFactor;

        Coordinate worldCenter = event.getWorldCoordinate();
        if (worldCenter != null) {
            viewport.zoom(factor, worldCenter.getX(), worldCenter.getY());
        } else {
            viewport.zoom(factor);
        }

        adapter.render();
    }

    @Override
    public void onMousePressed(ChartMouseEvent event) {
        lastY = event.getScreenY();
    }

    @Override
    public void onMouseReleased(ChartMouseEvent event) {
    }

    @Override
    public void onMouseMoved(ChartMouseEvent event) {
    }

    @Override
    public void onMouseDragged(ChartMouseEvent event) {
        if (event.isControlDown() && event.isPrimaryButtonDown()) {
            zoomToSelection(event);
        }
    }

    @Override
    public void onMouseClicked(ChartMouseEvent event) {
    }

    private void zoomToSelection(ChartMouseEvent event) {
        if (adapter == null) {
            return;
        }

        Viewport viewport = adapter.getViewer().getViewportObject();
        if (viewport == null) {
            return;
        }

        double delta = event.getScreenY() - lastY;
        double factor = delta > 0 ? zoomFactor : 1.0 / zoomFactor;
        lastY = event.getScreenY();

        Envelope extent = viewport.getExtent();
        if (extent == null) {
            return;
        }

        Coordinate worldCenter = event.getWorldCoordinate();
        if (worldCenter == null) {
            worldCenter = extent.getCenter();
        }

        double newWidth = extent.getWidth() * factor;
        double newHeight = extent.getHeight() * factor;

        double halfW = newWidth / 2;
        double halfH = newHeight / 2;
        Envelope newExtent = new Envelope(
            worldCenter.getX() - halfW,
            worldCenter.getY() - halfH,
            worldCenter.getX() + halfW,
            worldCenter.getY() + halfH
        );

        viewport.setExtent(newExtent);
        adapter.render();
    }

    public void zoomIn() {
        zoom(zoomFactor);
    }

    public void zoomOut() {
        zoom(1.0 / zoomFactor);
    }

    public void zoom(double factor) {
        if (adapter == null) {
            return;
        }

        Viewport viewport = adapter.getViewer().getViewportObject();
        if (viewport == null) {
            return;
        }

        Envelope extent = viewport.getExtent();
        if (extent == null) {
            return;
        }

        Coordinate center = extent.getCenter();
        double newWidth = extent.getWidth() * factor;
        double newHeight = extent.getHeight() * factor;

        double halfW = newWidth / 2;
        double halfH = newHeight / 2;
        Envelope newExtent = new Envelope(
            center.getX() - halfW,
            center.getY() - halfH,
            center.getX() + halfW,
            center.getY() + halfH
        );

        viewport.setExtent(newExtent);
        adapter.render();
    }

    public void zoomToExtent(Envelope extent) {
        if (adapter == null || extent == null) {
            return;
        }

        Viewport viewport = adapter.getViewer().getViewportObject();
        if (viewport == null) {
            return;
        }

        viewport.setExtent(extent);
        adapter.render();
    }

    public double getZoomFactor() {
        return zoomFactor;
    }

    public void setZoomFactor(double zoomFactor) {
        this.zoomFactor = zoomFactor;
    }

    public double getMinZoom() {
        return minZoom;
    }

    public void setMinZoom(double minZoom) {
        this.minZoom = minZoom;
    }

    public double getMaxZoom() {
        return maxZoom;
    }

    public void setMaxZoom(double maxZoom) {
        this.maxZoom = maxZoom;
    }
}
