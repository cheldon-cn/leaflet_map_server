package cn.cycle.echart.ui;

import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.canvas.Canvas;
import javafx.scene.control.Button;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.MenuItem;
import javafx.scene.control.SeparatorMenuItem;
import javafx.scene.image.Image;
import javafx.scene.input.MouseButton;
import javafx.scene.input.ScrollEvent;
import javafx.scene.layout.VBox;
import javafx.scene.layout.StackPane;

import java.io.File;
import java.io.FileInputStream;
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
    private static final double MIN_ZOOM = 0.1;
    private static final double MAX_ZOOM = 10.0;
    private static final double ZOOM_FACTOR = 1.1;
    private static final double ZOOM_BUTTON_SIZE = 32.0;

    private final List<Canvas> layers;
    private double zoomLevel;
    private double centerX;
    private double centerY;
    private double rotation;
    private int columnCount;
    
    private double worldMinX;
    private double worldMaxX;
    private double worldMinY;
    private double worldMaxY;
    private boolean preserveWorldBounds = true;
    private double previousWidth;

    private boolean initialized = false;
    private boolean active = false;
    private boolean disposed = false;

    private double lastMouseX;
    private double lastMouseY;
    private boolean isPanning;

    private ContextMenu contextMenu;
    private ChartContextMenuListener menuListener;
    
    private VBox zoomControls;
    private Button zoomInButton;
    private Button zoomOutButton;
    
    private Image displayImage;
    private String imageFilePath;
    private double imageWidth;
    private double imageHeight;

    public ChartDisplayArea() {
        this.layers = new ArrayList<>();
        this.zoomLevel = 1.0;
        this.centerX = 0;
        this.centerY = 0;
        this.rotation = 0;
        this.columnCount = 1;
        this.worldMinX = -100;
        this.worldMaxX = 100;
        this.worldMinY = -100;
        this.worldMaxY = 100;
        this.previousWidth = 0;
        
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
        
        widthProperty().addListener((obs, oldVal, newVal) -> {
            if (preserveWorldBounds && previousWidth > 0 && oldVal.doubleValue() > 0) {
                preserveWorldBoundsOnResize(oldVal.doubleValue(), newVal.doubleValue());
            }
            previousWidth = newVal.doubleValue();
            redraw();
        });
        heightProperty().addListener(obs -> redraw());
        
        getStyleClass().add("chart-display-area");
        
        setupMouseHandlers();
        setupContextMenu();
        setupZoomControls();
    }

    private void setupZoomControls() {
        zoomInButton = new Button("+");
        zoomInButton.setPrefSize(ZOOM_BUTTON_SIZE, ZOOM_BUTTON_SIZE);
        zoomInButton.setMinSize(ZOOM_BUTTON_SIZE, ZOOM_BUTTON_SIZE);
        zoomInButton.setMaxSize(ZOOM_BUTTON_SIZE, ZOOM_BUTTON_SIZE);
        zoomInButton.getStyleClass().add("zoom-button");
        zoomInButton.setOnAction(e -> zoomIn());
        
        zoomOutButton = new Button("-");
        zoomOutButton.setPrefSize(ZOOM_BUTTON_SIZE, ZOOM_BUTTON_SIZE);
        zoomOutButton.setMinSize(ZOOM_BUTTON_SIZE, ZOOM_BUTTON_SIZE);
        zoomOutButton.setMaxSize(ZOOM_BUTTON_SIZE, ZOOM_BUTTON_SIZE);
        zoomOutButton.getStyleClass().add("zoom-button");
        zoomOutButton.setOnAction(e -> zoomOut());
        
        zoomControls = new VBox(4);
        zoomControls.getChildren().addAll(zoomInButton, zoomOutButton);
        zoomControls.setAlignment(Pos.CENTER);
        zoomControls.setPadding(new Insets(6));
        zoomControls.getStyleClass().add("zoom-controls");
        zoomControls.setMouseTransparent(false);
        zoomControls.setMaxSize(javafx.scene.layout.Region.USE_PREF_SIZE, javafx.scene.layout.Region.USE_PREF_SIZE);
        
        StackPane.setAlignment(zoomControls, Pos.BOTTOM_RIGHT);
        StackPane.setMargin(zoomControls, new Insets(0, 10, 10, 0));
        getChildren().add(zoomControls);
    }
    
    private void preserveWorldBoundsOnResize(double oldWidth, double newWidth) {
        updateWorldBounds();
        
        double worldWidth = worldMaxX - worldMinX;
        double worldHeight = worldMaxY - worldMinY;
        
        zoomLevel = newWidth / worldWidth;
        
        centerX = (worldMinX + worldMaxX) / 2.0;
        centerY = (worldMinY + worldMaxY) / 2.0;
    }
    
    private void updateWorldBounds() {
        double halfWidth = getWidth() / 2.0 / zoomLevel;
        double halfHeight = getHeight() / 2.0 / zoomLevel;
        
        worldMinX = centerX - halfWidth;
        worldMaxX = centerX + halfWidth;
        worldMinY = centerY - halfHeight;
        worldMaxY = centerY + halfHeight;
    }

    private void setupMouseHandlers() {
        Canvas interactionLayer = getLayer(LayerType.INTERACTION);
        
        setOnMousePressed(event -> {
            if (event.getButton() == MouseButton.PRIMARY) {
                lastMouseX = event.getX();
                lastMouseY = event.getY();
                isPanning = true;
            }
        });
        
        setOnMouseDragged(event -> {
            if (isPanning && event.getButton() == MouseButton.PRIMARY) {
                double deltaX = event.getX() - lastMouseX;
                double deltaY = event.getY() - lastMouseY;
                
                pan(deltaX, deltaY);
                
                lastMouseX = event.getX();
                lastMouseY = event.getY();
            }
        });
        
        setOnMouseReleased(event -> {
            isPanning = false;
        });
        
        setOnScroll(this::handleScroll);
        
        setOnMouseMoved(event -> {
        });
    }

    private void handleScroll(ScrollEvent event) {
        if (event.getDeltaY() == 0) {
            return;
        }
        
        double mouseX = event.getX();
        double mouseY = event.getY();
        
        double worldXBefore = screenToWorldX(mouseX);
        double worldYBefore = screenToWorldY(mouseY);
        
        double delta = event.getDeltaY() > 0 ? ZOOM_FACTOR : 1.0 / ZOOM_FACTOR;
        double newZoom = Math.max(MIN_ZOOM, Math.min(MAX_ZOOM, zoomLevel * delta));
        
        if (newZoom != zoomLevel) {
            zoomLevel = newZoom;
            
            double worldXAfter = screenToWorldX(mouseX);
            double worldYAfter = screenToWorldY(mouseY);
            
            centerX += worldXBefore - worldXAfter;
            centerY += worldYBefore - worldYAfter;
            
            redraw();
        }
        
        event.consume();
    }

    private void setupContextMenu() {
        contextMenu = new ContextMenu();
        
        MenuItem zoomInItem = new MenuItem("放大");
        zoomInItem.setOnAction(e -> {
            zoomIn();
            if (menuListener != null) {
                menuListener.onZoomIn();
            }
        });
        
        MenuItem zoomOutItem = new MenuItem("缩小");
        zoomOutItem.setOnAction(e -> {
            zoomOut();
            if (menuListener != null) {
                menuListener.onZoomOut();
            }
        });
        
        MenuItem fitWindowItem = new MenuItem("适应窗口");
        fitWindowItem.setOnAction(e -> {
            fitToWindow();
            if (menuListener != null) {
                menuListener.onFitToWindow();
            }
        });
        
        MenuItem resetViewItem = new MenuItem("重置视图");
        resetViewItem.setOnAction(e -> {
            resetView();
            if (menuListener != null) {
                menuListener.onResetView();
            }
        });
        
        MenuItem measureDistanceItem = new MenuItem("测量距离");
        measureDistanceItem.setOnAction(e -> {
            if (menuListener != null) {
                menuListener.onMeasureDistance();
            }
        });
        
        MenuItem measureAreaItem = new MenuItem("测量面积");
        measureAreaItem.setOnAction(e -> {
            if (menuListener != null) {
                menuListener.onMeasureArea();
            }
        });
        
        MenuItem measureBearingItem = new MenuItem("测量方位");
        measureBearingItem.setOnAction(e -> {
            if (menuListener != null) {
                menuListener.onMeasureBearing();
            }
        });
        
        MenuItem propertiesItem = new MenuItem("属性");
        propertiesItem.setOnAction(e -> {
            if (menuListener != null) {
                menuListener.onShowProperties();
            }
        });
        
        contextMenu.getItems().addAll(
            zoomInItem,
            zoomOutItem,
            fitWindowItem,
            resetViewItem,
            new SeparatorMenuItem(),
            measureDistanceItem,
            measureAreaItem,
            measureBearingItem,
            new SeparatorMenuItem(),
            propertiesItem
        );
        
        setOnContextMenuRequested(event -> {
            contextMenu.show(this, event.getScreenX(), event.getScreenY());
            event.consume();
        });
    }

    public void setContextMenuListener(ChartContextMenuListener listener) {
        this.menuListener = listener;
    }

    public double screenToWorldX(double screenX) {
        return (screenX - getWidth() / 2.0) / zoomLevel + centerX;
    }

    public double screenToWorldY(double screenY) {
        return (screenY - getHeight() / 2.0) / zoomLevel + centerY;
    }

    public double worldToScreenX(double worldX) {
        return (worldX - centerX) * zoomLevel + getWidth() / 2.0;
    }

    public double worldToScreenY(double worldY) {
        return (worldY - centerY) * zoomLevel + getHeight() / 2.0;
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
        
        if (displayImage != null) {
            double halfWidth = imageWidth / 2.0;
            double halfHeight = imageHeight / 2.0;
            gc.drawImage(displayImage, -halfWidth, -halfHeight, imageWidth, imageHeight);
        } else {
            gc.setStroke(javafx.scene.paint.Color.BLUE);
            gc.setLineWidth(1);
            gc.strokeRect(-100, -100, 200, 200);
        }
        
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
        updateWorldBounds();
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
        updateWorldBounds();
        redraw();
    }

    public void resetView() {
        zoomLevel = 1.0;
        centerX = 0;
        centerY = 0;
        rotation = 0;
        updateWorldBounds();
        redraw();
    }

    public double getCenterX() {
        return centerX;
    }

    public void setCenterX(double centerX) {
        this.centerX = centerX;
        updateWorldBounds();
        redraw();
    }

    public double getCenterY() {
        return centerY;
    }

    public void setCenterY(double centerY) {
        this.centerY = centerY;
        updateWorldBounds();
        redraw();
    }

    public void pan(double dx, double dy) {
        centerX -= dx / zoomLevel;
        centerY -= dy / zoomLevel;
        updateWorldBounds();
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
    
    public boolean isPreserveWorldBounds() {
        return preserveWorldBounds;
    }
    
    public void setPreserveWorldBounds(boolean preserve) {
        this.preserveWorldBounds = preserve;
    }
    
    public double getWorldMinX() {
        return worldMinX;
    }
    
    public double getWorldMaxX() {
        return worldMaxX;
    }
    
    public double getWorldMinY() {
        return worldMinY;
    }
    
    public double getWorldMaxY() {
        return worldMaxY;
    }
    
    public void setWorldBounds(double minX, double maxX, double minY, double maxY) {
        this.worldMinX = minX;
        this.worldMaxX = maxX;
        this.worldMinY = minY;
        this.worldMaxY = maxY;
        
        double worldWidth = maxX - minX;
        double worldHeight = maxY - minY;
        
        zoomLevel = Math.min(getWidth() / worldWidth, getHeight() / worldHeight);
        centerX = (minX + maxX) / 2.0;
        centerY = (minY + maxY) / 2.0;
        
        redraw();
    }
    
    public boolean loadImage(File imageFile) {
        if (imageFile == null || !imageFile.exists()) {
            return false;
        }
        
        try {
            FileInputStream fis = new FileInputStream(imageFile);
            displayImage = new Image(fis);
            fis.close();
            
            imageFilePath = imageFile.getAbsolutePath();
            imageWidth = displayImage.getWidth();
            imageHeight = displayImage.getHeight();
            
            fitImageToWindow();
            
            return true;
        } catch (Exception e) {
            System.err.println("Failed to load image: " + imageFile.getPath() + " - " + e.getMessage());
            displayImage = null;
            imageFilePath = null;
            return false;
        }
    }
    
    public boolean loadImage(String filePath) {
        if (filePath == null || filePath.isEmpty()) {
            return false;
        }
        return loadImage(new File(filePath));
    }
    
    public void clearImage() {
        displayImage = null;
        imageFilePath = null;
        imageWidth = 0;
        imageHeight = 0;
        redraw();
    }
    
    public boolean hasImage() {
        return displayImage != null;
    }
    
    public String getImageFilePath() {
        return imageFilePath;
    }
    
    public double getImageWidth() {
        return imageWidth;
    }
    
    public double getImageHeight() {
        return imageHeight;
    }
    
    public void fitImageToWindow() {
        if (displayImage == null || getWidth() <= 0 || getHeight() <= 0) {
            return;
        }
        
        double scaleX = getWidth() / imageWidth;
        double scaleY = getHeight() / imageHeight;
        zoomLevel = Math.min(scaleX, scaleY) * 0.9;
        
        centerX = 0;
        centerY = 0;
        
        worldMinX = -imageWidth / 2.0;
        worldMaxX = imageWidth / 2.0;
        worldMinY = -imageHeight / 2.0;
        worldMaxY = imageHeight / 2.0;
        
        redraw();
    }
    
    public boolean loadImageFromPixels(byte[] pixels, int width, int height) {
        if (pixels == null || width <= 0 || height <= 0) {
            return false;
        }
        
        try {
            int expectedSize = width * height * 4;
            if (pixels.length < expectedSize) {
                System.err.println("Pixel data size mismatch. Expected: " + expectedSize + ", Actual: " + pixels.length);
                return false;
            }
            
            javafx.scene.image.WritablePixelFormat<java.nio.ByteBuffer> format = 
                javafx.scene.image.PixelFormat.getByteBgraPreInstance();
            
            javafx.scene.image.WritableImage writableImage = new javafx.scene.image.WritableImage(width, height);
            writableImage.getPixelWriter().setPixels(0, 0, width, height, format, 
                java.nio.ByteBuffer.wrap(pixels), width * 4);
            
            this.displayImage = writableImage;
            this.imageWidth = width;
            this.imageHeight = height;
            
            fitImageToWindow();
            
            return true;
        } catch (Exception e) {
            System.err.println("Failed to load image from pixels: " + e.getMessage());
            return false;
        }
    }
    
    public boolean loadImageFromBgrPixels(byte[] pixels, int width, int height) {
        if (pixels == null || width <= 0 || height <= 0) {
            return false;
        }
        
        try {
            int expectedSize = width * height * 3;
            if (pixels.length < expectedSize) {
                System.err.println("Pixel data size mismatch. Expected: " + expectedSize + ", Actual: " + pixels.length);
                return false;
            }
            
            byte[] bgraPixels = new byte[width * height * 4];
            for (int i = 0; i < width * height; i++) {
                int srcIdx = i * 3;
                int dstIdx = i * 4;
                bgraPixels[dstIdx] = pixels[srcIdx + 2];     // B -> R
                bgraPixels[dstIdx + 1] = pixels[srcIdx + 1]; // G -> G
                bgraPixels[dstIdx + 2] = pixels[srcIdx];     // R -> B
                bgraPixels[dstIdx + 3] = (byte) 255;         // A
            }
            
            return loadImageFromPixels(bgraPixels, width, height);
        } catch (Exception e) {
            System.err.println("Failed to load image from BGR pixels: " + e.getMessage());
            return false;
        }
    }
    
    public boolean loadImageFromRgbaPixels(byte[] pixels, int width, int height) {
        if (pixels == null || width <= 0 || height <= 0) {
            return false;
        }
        
        try {
            int expectedSize = width * height * 4;
            if (pixels.length < expectedSize) {
                System.err.println("Pixel data size mismatch. Expected: " + expectedSize + ", Actual: " + pixels.length);
                return false;
            }
            
            byte[] bgraPixels = new byte[width * height * 4];
            for (int i = 0; i < width * height; i++) {
                int idx = i * 4;
                bgraPixels[idx] = pixels[idx + 2];     // R -> B
                bgraPixels[idx + 1] = pixels[idx + 1]; // G -> G
                bgraPixels[idx + 2] = pixels[idx];     // B -> R
                bgraPixels[idx + 3] = pixels[idx + 3]; // A -> A
            }
            
            return loadImageFromPixels(bgraPixels, width, height);
        } catch (Exception e) {
            System.err.println("Failed to load image from RGBA pixels: " + e.getMessage());
            return false;
        }
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

    public interface ChartContextMenuListener {
        void onZoomIn();
        void onZoomOut();
        void onFitToWindow();
        void onResetView();
        void onMeasureDistance();
        void onMeasureArea();
        void onMeasureBearing();
        void onShowProperties();
    }
}
