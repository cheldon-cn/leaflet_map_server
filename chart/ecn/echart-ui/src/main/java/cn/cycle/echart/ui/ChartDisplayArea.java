package cn.cycle.echart.ui;

import javafx.scene.canvas.Canvas;
import javafx.scene.layout.StackPane;

import java.util.ArrayList;
import java.util.List;

/**
 * 海图显示区。
 * 
 * <p>实现分层渲染架构（5层），支持海图渲染和交互。</p>
 * <p>渲染层次从底到上：</p>
 * <ol>
 *   <li>背景层 - 海底地形、水深等背景信息</li>
 *   <li>海图层 - 海图要素、航道、锚地等</li>
 *   <li>数据层 - AIS目标、航线、标注等动态数据</li>
 *   <li>交互层 - 测量工具、选择框、编辑器等交互元素</li>
 *   <li>覆盖层 - 提示信息、临时标注等</li>
 * </ol>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ChartDisplayArea extends StackPane implements LifecycleComponent {

    private static final int LAYER_COUNT = 5;

    private final List<Canvas> layers;
    private double zoomLevel;
    private double centerX;
    private double centerY;
    private double rotation;
    private int columnCount;

    private boolean initialized = false;
    private boolean active = false;
    private boolean disposed = false;

    public ChartDisplayArea() {
        this.layers = new ArrayList<>();
        this.zoomLevel = 1.0;
        this.centerX = 0;
        this.centerY = 0;
        this.rotation = 0;
        this.columnCount = 1;
        
        initializeLayers();
        initializeLayout();
    }

    private void initializeLayers() {
        for (int i = 0; i < LAYER_COUNT; i++) {
            Canvas layer = new Canvas();
            layer.setId("layer-" + i);
            layers.add(layer);
        }
    }

    private void initializeLayout() {
        getChildren().addAll(layers);
        
        for (Canvas layer : layers) {
            layer.widthProperty().bind(widthProperty());
            layer.heightProperty().bind(heightProperty());
        }
        
        widthProperty().addListener(obs -> redraw());
        heightProperty().addListener(obs -> redraw());
        
        getStyleClass().add("chart-display-area");
        
        setupMouseHandlers();
    }

    private void setupMouseHandlers() {
        Canvas interactionLayer = getLayer(LayerType.INTERACTION);
        
        interactionLayer.setOnMousePressed(event -> {
        });
        
        interactionLayer.setOnMouseDragged(event -> {
        });
        
        interactionLayer.setOnMouseReleased(event -> {
        });
        
        interactionLayer.setOnScroll(event -> {
            double delta = event.getDeltaY() > 0 ? 1.1 : 0.9;
            setZoomLevel(zoomLevel * delta);
        });
        
        interactionLayer.setOnMouseMoved(event -> {
        });
    }

    @Override
    public void initialize() {
        if (initialized) {
            return;
        }
        
        clearAllLayers();
        initialized = true;
    }

    @Override
    public void activate() {
        if (!initialized || active) {
            return;
        }
        active = true;
        redraw();
    }

    @Override
    public void deactivate() {
        if (!active) {
            return;
        }
        active = false;
    }

    @Override
    public void dispose() {
        if (disposed) {
            return;
        }
        
        deactivate();
        
        for (Canvas layer : layers) {
            layer.widthProperty().unbind();
            layer.heightProperty().unbind();
        }
        
        layers.clear();
        disposed = true;
    }

    @Override
    public boolean isInitialized() {
        return initialized;
    }

    @Override
    public boolean isActive() {
        return active;
    }

    @Override
    public boolean isDisposed() {
        return disposed;
    }

    public void redraw() {
        clearAllLayers();
        
        drawBackgroundLayer();
        drawChartLayer();
        drawDataLayer();
        drawInteractionLayer();
        drawOverlayLayer();
    }

    private void clearAllLayers() {
        for (Canvas layer : layers) {
            javafx.scene.canvas.GraphicsContext gc = layer.getGraphicsContext2D();
            gc.clearRect(0, 0, layer.getWidth(), layer.getHeight());
        }
    }

    private void drawBackgroundLayer() {
        Canvas layer = getLayer(LayerType.BACKGROUND);
        javafx.scene.canvas.GraphicsContext gc = layer.getGraphicsContext2D();
        
        gc.save();
        applyTransform(gc);
        
        gc.setFill(javafx.scene.paint.Color.LIGHTBLUE);
        gc.fillRect(-layer.getWidth(), -layer.getHeight(), 
                   layer.getWidth() * 2, layer.getHeight() * 2);
        
        gc.restore();
    }

    private void drawChartLayer() {
        Canvas layer = getLayer(LayerType.CHART);
        javafx.scene.canvas.GraphicsContext gc = layer.getGraphicsContext2D();
        
        gc.save();
        applyTransform(gc);
        
        gc.setStroke(javafx.scene.paint.Color.BLUE);
        gc.setLineWidth(1);
        gc.strokeRect(-100, -100, 200, 200);
        
        gc.restore();
    }

    private void drawDataLayer() {
        Canvas layer = getLayer(LayerType.DATA);
        javafx.scene.canvas.GraphicsContext gc = layer.getGraphicsContext2D();
        
        gc.save();
        applyTransform(gc);
        
        gc.restore();
    }

    private void drawInteractionLayer() {
        Canvas layer = getLayer(LayerType.INTERACTION);
        javafx.scene.canvas.GraphicsContext gc = layer.getGraphicsContext2D();
        
        gc.save();
        applyTransform(gc);
        
        gc.restore();
    }

    private void drawOverlayLayer() {
        Canvas layer = getLayer(LayerType.OVERLAY);
        javafx.scene.canvas.GraphicsContext gc = layer.getGraphicsContext2D();
        
        gc.save();
        
        gc.restore();
    }

    private void applyTransform(javafx.scene.canvas.GraphicsContext gc) {
        Canvas baseLayer = getLayer(LayerType.BACKGROUND);
        gc.translate(baseLayer.getWidth() / 2, baseLayer.getHeight() / 2);
        gc.scale(zoomLevel, zoomLevel);
        gc.translate(-centerX, -centerY);
        gc.rotate(rotation);
    }

    public Canvas getLayer(LayerType type) {
        return layers.get(type.getIndex());
    }

    public List<Canvas> getLayers() {
        return new ArrayList<>(layers);
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

    public int getColumnCount() {
        return columnCount;
    }

    public void setColumnCount(int columnCount) {
        this.columnCount = Math.max(1, columnCount);
    }

    public enum LayerType {
        BACKGROUND(0),
        CHART(1),
        DATA(2),
        INTERACTION(3),
        OVERLAY(4);

        private final int index;

        LayerType(int index) {
            this.index = index;
        }

        public int getIndex() {
            return index;
        }
    }
}
