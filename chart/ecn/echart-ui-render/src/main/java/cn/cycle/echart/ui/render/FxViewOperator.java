package cn.cycle.echart.ui.render;

import javafx.scene.canvas.Canvas;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import javafx.scene.input.ZoomEvent;
import javafx.scene.input.RotateEvent;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * JavaFX视图操作器。
 * 
 * <p>实现视图的平移、缩放、旋转等交互操作。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class FxViewOperator {

    private static final double MIN_ZOOM = 0.1;
    private static final double MAX_ZOOM = 100.0;
    private static final double ZOOM_FACTOR = 1.2;
    private static final double ROTATION_STEP = 15.0;

    private final Canvas canvas;
    
    private double centerX;
    private double centerY;
    private double zoom;
    private double rotation;
    
    private double lastMouseX;
    private double lastMouseY;
    private boolean isPanning;
    private boolean isRotating;
    
    private final List<ViewChangeListener> listeners;
    private ViewOperationMode mode;

    public FxViewOperator(Canvas canvas) {
        this.canvas = Objects.requireNonNull(canvas, "canvas cannot be null");
        this.centerX = 0.0;
        this.centerY = 0.0;
        this.zoom = 1.0;
        this.rotation = 0.0;
        this.lastMouseX = 0.0;
        this.lastMouseY = 0.0;
        this.isPanning = false;
        this.isRotating = false;
        this.listeners = new ArrayList<>();
        this.mode = ViewOperationMode.PAN;
        
        setupEventHandlers();
    }

    private void setupEventHandlers() {
        canvas.setOnMousePressed(this::onMousePressed);
        canvas.setOnMouseDragged(this::onMouseDragged);
        canvas.setOnMouseReleased(this::onMouseReleased);
        canvas.setOnScroll(this::onScroll);
        canvas.setOnZoom(this::onZoom);
        canvas.setOnRotate(this::onRotate);
    }

    private void onMousePressed(MouseEvent event) {
        lastMouseX = event.getX();
        lastMouseY = event.getY();
        
        if (mode == ViewOperationMode.PAN) {
            isPanning = true;
        } else if (mode == ViewOperationMode.ROTATE) {
            isRotating = true;
        }
    }

    private void onMouseDragged(MouseEvent event) {
        double deltaX = event.getX() - lastMouseX;
        double deltaY = event.getY() - lastMouseY;
        
        if (isPanning) {
            pan(deltaX, deltaY);
        } else if (isRotating) {
            double angle = Math.atan2(deltaY, deltaX) * 180.0 / Math.PI;
            setRotation(rotation + angle * 0.1);
        }
        
        lastMouseX = event.getX();
        lastMouseY = event.getY();
    }

    private void onMouseReleased(MouseEvent event) {
        isPanning = false;
        isRotating = false;
    }

    private void onScroll(ScrollEvent event) {
        double delta = event.getDeltaY() > 0 ? ZOOM_FACTOR : 1.0 / ZOOM_FACTOR;
        double newZoom = zoom * delta;
        
        if (event.isControlDown()) {
            double zoomX = event.getX();
            double zoomY = event.getY();
            zoomAtPoint(newZoom, zoomX, zoomY);
        } else {
            setZoom(newZoom);
        }
    }

    private void onZoom(ZoomEvent event) {
        double newZoom = zoom * event.getZoomFactor();
        setZoom(newZoom);
    }

    private void onRotate(RotateEvent event) {
        setRotation(rotation + event.getAngle());
    }

    public void pan(double deltaX, double deltaY) {
        this.centerX -= deltaX / zoom;
        this.centerY -= deltaY / zoom;
        notifyViewChanged();
    }

    public void panTo(double x, double y) {
        this.centerX = x;
        this.centerY = y;
        notifyViewChanged();
    }

    public void setZoom(double zoom) {
        this.zoom = Math.max(MIN_ZOOM, Math.min(MAX_ZOOM, zoom));
        notifyViewChanged();
    }

    public void zoomAtPoint(double newZoom, double screenX, double screenY) {
        double worldX = screenToWorldX(screenX);
        double worldY = screenToWorldY(screenY);
        
        double clampedZoom = Math.max(MIN_ZOOM, Math.min(MAX_ZOOM, newZoom));
        
        double newWorldX = screenX / clampedZoom + centerX - canvas.getWidth() / (2 * clampedZoom);
        double newWorldY = screenY / clampedZoom + centerY - canvas.getHeight() / (2 * clampedZoom);
        
        this.zoom = clampedZoom;
        this.centerX = newWorldX;
        this.centerY = newWorldY;
        
        notifyViewChanged();
    }

    public void zoomIn() {
        setZoom(zoom * ZOOM_FACTOR);
    }

    public void zoomOut() {
        setZoom(zoom / ZOOM_FACTOR);
    }

    public void zoomToExtent(double minX, double minY, double maxX, double maxY) {
        double width = maxX - minX;
        double height = maxY - minY;
        
        double zoomX = canvas.getWidth() / width;
        double zoomY = canvas.getHeight() / height;
        
        this.zoom = Math.min(zoomX, zoomY) * 0.9;
        this.centerX = (minX + maxX) / 2.0;
        this.centerY = (minY + maxY) / 2.0;
        
        notifyViewChanged();
    }

    public void fitToWindow() {
        this.zoom = 1.0;
        this.centerX = canvas.getWidth() / 2.0;
        this.centerY = canvas.getHeight() / 2.0;
        this.rotation = 0.0;
        
        notifyViewChanged();
    }

    public void setRotation(double rotation) {
        this.rotation = rotation;
        while (this.rotation < 0) {
            this.rotation += 360;
        }
        while (this.rotation >= 360) {
            this.rotation -= 360;
        }
        notifyViewChanged();
    }

    public void rotateLeft() {
        setRotation(rotation - ROTATION_STEP);
    }

    public void rotateRight() {
        setRotation(rotation + ROTATION_STEP);
    }

    public void resetRotation() {
        setRotation(0.0);
    }

    public double screenToWorldX(double screenX) {
        return (screenX - canvas.getWidth() / 2.0) / zoom + centerX;
    }

    public double screenToWorldY(double screenY) {
        return (screenY - canvas.getHeight() / 2.0) / zoom + centerY;
    }

    public double worldToScreenX(double worldX) {
        return (worldX - centerX) * zoom + canvas.getWidth() / 2.0;
    }

    public double worldToScreenY(double worldY) {
        return (worldY - centerY) * zoom + canvas.getHeight() / 2.0;
    }

    public double getCenterX() {
        return centerX;
    }

    public void setCenterX(double centerX) {
        this.centerX = centerX;
        notifyViewChanged();
    }

    public double getCenterY() {
        return centerY;
    }

    public void setCenterY(double centerY) {
        this.centerY = centerY;
        notifyViewChanged();
    }

    public double getZoom() {
        return zoom;
    }

    public double getRotation() {
        return rotation;
    }

    public void setMode(ViewOperationMode mode) {
        Objects.requireNonNull(mode, "mode cannot be null");
        this.mode = mode;
    }

    public ViewOperationMode getMode() {
        return mode;
    }

    public void addViewChangeListener(ViewChangeListener listener) {
        Objects.requireNonNull(listener, "listener cannot be null");
        listeners.add(listener);
    }

    public void removeViewChangeListener(ViewChangeListener listener) {
        listeners.remove(listener);
    }

    private void notifyViewChanged() {
        ViewChangeEvent event = new ViewChangeEvent(centerX, centerY, zoom, rotation);
        for (ViewChangeListener listener : listeners) {
            listener.onViewChanged(event);
        }
    }

    public void applyTransform(javafx.scene.canvas.GraphicsContext gc) {
        gc.translate(canvas.getWidth() / 2.0, canvas.getHeight() / 2.0);
        gc.scale(zoom, zoom);
        gc.translate(-centerX, -centerY);
        gc.rotate(rotation);
    }

    public enum ViewOperationMode {
        PAN,
        ROTATE,
        SELECT,
        MEASURE
    }

    public interface ViewChangeListener {
        void onViewChanged(ViewChangeEvent event);
    }

    public static class ViewChangeEvent {
        private final double centerX;
        private final double centerY;
        private final double zoom;
        private final double rotation;

        public ViewChangeEvent(double centerX, double centerY, double zoom, double rotation) {
            this.centerX = centerX;
            this.centerY = centerY;
            this.zoom = zoom;
            this.rotation = rotation;
        }

        public double getCenterX() {
            return centerX;
        }

        public double getCenterY() {
            return centerY;
        }

        public double getZoom() {
            return zoom;
        }

        public double getRotation() {
            return rotation;
        }
    }
}
