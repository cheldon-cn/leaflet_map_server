package cn.cycle.echart.ui;

import javafx.scene.canvas.Canvas;
import javafx.scene.layout.StackPane;

/**
 * 海图显示区。
 * 
 * <p>海图渲染和交互的主区域。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ChartDisplayArea extends StackPane {

    private final Canvas canvas;
    private double zoomLevel;
    private double centerX;
    private double centerY;
    private double rotation;

    public ChartDisplayArea() {
        this.canvas = new Canvas();
        this.zoomLevel = 1.0;
        this.centerX = 0;
        this.centerY = 0;
        this.rotation = 0;
        
        initializeLayout();
    }

    private void initializeLayout() {
        getChildren().add(canvas);
        
        canvas.widthProperty().bind(widthProperty());
        canvas.heightProperty().bind(heightProperty());
        
        canvas.widthProperty().addListener(obs -> redraw());
        canvas.heightProperty().addListener(obs -> redraw());
        
        getStyleClass().add("chart-display-area");
        
        setupMouseHandlers();
    }

    private void setupMouseHandlers() {
        canvas.setOnMousePressed(event -> {
        });
        
        canvas.setOnMouseDragged(event -> {
        });
        
        canvas.setOnMouseReleased(event -> {
        });
        
        canvas.setOnScroll(event -> {
            double delta = event.getDeltaY() > 0 ? 1.1 : 0.9;
            setZoomLevel(zoomLevel * delta);
        });
    }

    public void redraw() {
        javafx.scene.canvas.GraphicsContext gc = canvas.getGraphicsContext2D();
        gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());
        
        gc.save();
        gc.translate(canvas.getWidth() / 2, canvas.getHeight() / 2);
        gc.scale(zoomLevel, zoomLevel);
        gc.translate(-centerX, -centerY);
        gc.rotate(rotation);
        
        drawChart(gc);
        
        gc.restore();
    }

    protected void drawChart(javafx.scene.canvas.GraphicsContext gc) {
        gc.setStroke(javafx.scene.paint.Color.BLUE);
        gc.setLineWidth(1);
        gc.strokeRect(-100, -100, 200, 200);
    }

    public double getZoomLevel() {
        return zoomLevel;
    }

    public void setZoomLevel(double zoomLevel) {
        this.zoomLevel = Math.max(0.1, Math.min(10.0, zoomLevel));
        redraw();
    }

    public void zoomIn() {
        setZoomLevel(zoomLevel * 1.2);
    }

    public void zoomOut() {
        setZoomLevel(zoomLevel / 1.2);
    }

    public void fitToWindow() {
        zoomLevel = 1.0;
        centerX = 0;
        centerY = 0;
        redraw();
    }

    public double getCenterX() {
        return centerX;
    }

    public void setCenterX(double centerX) {
        this.centerX = centerX;
        redraw();
    }

    public double getCenterY() {
        return centerY;
    }

    public void setCenterY(double centerY) {
        this.centerY = centerY;
        redraw();
    }

    public void pan(double dx, double dy) {
        centerX += dx / zoomLevel;
        centerY += dy / zoomLevel;
        redraw();
    }

    public double getRotation() {
        return rotation;
    }

    public void setRotation(double rotation) {
        this.rotation = rotation;
        redraw();
    }

    public void rotate(double angle) {
        this.rotation += angle;
        redraw();
    }

    public Canvas getCanvas() {
        return canvas;
    }
}
