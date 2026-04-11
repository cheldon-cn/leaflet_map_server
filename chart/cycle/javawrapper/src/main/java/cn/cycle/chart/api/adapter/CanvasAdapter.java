package cn.cycle.chart.api.adapter;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.core.Viewport;
import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.geometry.Envelope;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritablePixelFormat;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

public class CanvasAdapter {

    private final Canvas canvas;
    private final ChartViewer viewer;
    private final List<ChartEventHandler> handlers;
    private ImageDevice device;
    private boolean disposed;

    public CanvasAdapter(Canvas canvas, ChartViewer viewer) {
        this.canvas = canvas;
        this.viewer = viewer;
        this.handlers = new ArrayList<>();
        this.disposed = false;
        setupEventHandlers();
    }

    private void setupEventHandlers() {
        canvas.setOnMousePressed(e -> {
            ChartMouseEvent event = createMouseEvent(ChartMouseEvent.Type.PRESSED, e);
            fireMouseEvent(event);
        });

        canvas.setOnMouseReleased(e -> {
            ChartMouseEvent event = createMouseEvent(ChartMouseEvent.Type.RELEASED, e);
            fireMouseEvent(event);
        });

        canvas.setOnMouseMoved(e -> {
            ChartMouseEvent event = createMouseEvent(ChartMouseEvent.Type.MOVED, e);
            fireMouseEvent(event);
        });

        canvas.setOnMouseDragged(e -> {
            ChartMouseEvent event = createMouseEvent(ChartMouseEvent.Type.DRAGGED, e);
            fireMouseEvent(event);
        });

        canvas.setOnMouseClicked(e -> {
            ChartMouseEvent event = createMouseEvent(ChartMouseEvent.Type.CLICKED, e);
            fireMouseEvent(event);
        });

        canvas.setOnScroll(e -> {
            ChartScrollEvent event = createScrollEvent(e);
            fireScrollEvent(event);
        });

        canvas.setOnKeyPressed(e -> {
            ChartKeyEvent event = createKeyEvent(ChartKeyEvent.Type.PRESSED, e);
            fireKeyEvent(event);
        });

        canvas.setOnKeyReleased(e -> {
            ChartKeyEvent event = createKeyEvent(ChartKeyEvent.Type.RELEASED, e);
            fireKeyEvent(event);
        });
    }

    private ChartMouseEvent createMouseEvent(ChartMouseEvent.Type type, javafx.scene.input.MouseEvent e) {
        double screenX = e.getX();
        double screenY = e.getY();
        Coordinate worldCoord = screenToWorld(screenX, screenY);
        return new ChartMouseEvent(type, screenX, screenY, worldCoord, viewer, e);
    }

    private ChartScrollEvent createScrollEvent(javafx.scene.input.ScrollEvent e) {
        double screenX = e.getX();
        double screenY = e.getY();
        double delta = e.getDeltaY();
        Coordinate worldCoord = screenToWorld(screenX, screenY);
        return new ChartScrollEvent(ChartScrollEvent.Type.SCROLL, 
            screenX, screenY, e.getDeltaX(), e.getDeltaY(), delta, viewer, e, worldCoord);
    }

    private ChartKeyEvent createKeyEvent(ChartKeyEvent.Type type, javafx.scene.input.KeyEvent e) {
        return new ChartKeyEvent(type, 
            e.getCharacter(), e.getText(), 
            e.getCode() != null ? e.getCode().getName() : "", 
            viewer, e);
    }

    private Coordinate screenToWorld(double screenX, double screenY) {
        Viewport viewport = viewer.getViewportObject();
        if (viewport == null) {
            return new Coordinate(screenX, screenY);
        }
        Envelope extent = viewport.getExtent();
        if (extent == null) {
            return new Coordinate(screenX, screenY);
        }
        double width = canvas.getWidth();
        double height = canvas.getHeight();
        double worldX = extent.getMinX() + (screenX / width) * extent.getWidth();
        double worldY = extent.getMaxY() - (screenY / height) * extent.getHeight();
        return new Coordinate(worldX, worldY);
    }

    private void fireMouseEvent(ChartMouseEvent event) {
        for (ChartEventHandler handler : handlers) {
            if (handler instanceof ChartEventHandler.MouseHandler) {
                ChartEventHandler.MouseHandler mouseHandler = 
                    (ChartEventHandler.MouseHandler) handler;
                switch (event.getType()) {
                    case PRESSED:
                        mouseHandler.onMousePressed(event);
                        break;
                    case RELEASED:
                        mouseHandler.onMouseReleased(event);
                        break;
                    case MOVED:
                        mouseHandler.onMouseMoved(event);
                        break;
                    case DRAGGED:
                        mouseHandler.onMouseDragged(event);
                        break;
                    case CLICKED:
                        mouseHandler.onMouseClicked(event);
                        break;
                }
            }
        }
    }

    private void fireScrollEvent(ChartScrollEvent event) {
        for (ChartEventHandler handler : handlers) {
            if (handler instanceof ChartEventHandler.ScrollHandler) {
                ((ChartEventHandler.ScrollHandler) handler).onScroll(event);
            }
        }
    }

    private void fireKeyEvent(ChartKeyEvent event) {
        for (ChartEventHandler handler : handlers) {
            if (handler instanceof ChartEventHandler.KeyHandler) {
                ChartEventHandler.KeyHandler keyHandler = 
                    (ChartEventHandler.KeyHandler) handler;
                switch (event.getType()) {
                    case PRESSED:
                        keyHandler.onKeyPressed(event);
                        break;
                    case RELEASED:
                        keyHandler.onKeyReleased(event);
                        break;
                }
            }
        }
    }

    public void addHandler(ChartEventHandler handler) {
        handlers.add(handler);
        handler.install(this);
    }

    public void removeHandler(ChartEventHandler handler) {
        handlers.remove(handler);
        handler.uninstall();
    }

    public void render() {
        System.out.println("[DEBUG] CanvasAdapter.render() called");
        
        if (disposed) {
            System.out.println("[WARNING] CanvasAdapter is disposed, skipping render");
            return;
        }

        int width = (int) canvas.getWidth();
        int height = (int) canvas.getHeight();
        System.out.println("[DEBUG] Canvas size: " + width + " x " + height);

        if (width <= 0 || height <= 0) {
            System.out.println("[WARNING] Invalid canvas size, skipping render");
            return;
        }

        if (device == null) {
            System.out.println("[DEBUG] Creating new ImageDevice...");
            device = new ImageDevice(width, height);
        } else {
            System.out.println("[DEBUG] Resizing ImageDevice...");
            device.resize(width, height);
        }

        System.out.println("[DEBUG] Calling viewer.render()...");
        long devicePtr = device.getNativePtr();
        System.out.println("[DEBUG] Device pointer: " + devicePtr);
        
        int renderResult = viewer.render(devicePtr, width, height);
        System.out.println("[DEBUG] viewer.render() returned: " + renderResult);

        System.out.println("[DEBUG] Getting pixels from device...");
        byte[] pixels = device.getPixels();
        System.out.println("[DEBUG] Pixels array: " + (pixels != null ? "length=" + pixels.length : "null"));
        
        if (pixels != null && pixels.length >= width * height * 4) {
            System.out.println("[DEBUG] Drawing image to canvas...");
            
            GraphicsContext gc = canvas.getGraphicsContext2D();
            gc.clearRect(0, 0, width, height);
            
            javafx.scene.image.WritableImage image = 
                new javafx.scene.image.WritableImage(width, height);
            PixelWriter writer = image.getPixelWriter();
            WritablePixelFormat<ByteBuffer> format = WritablePixelFormat.getByteBgraPreInstance();
            writer.setPixels(0, 0, width, height, format, ByteBuffer.wrap(pixels), width * 4);
            
            boolean hasNonZeroPixels = false;
            int nonZeroCount = 0;
            for (int i = 0; i < pixels.length; i++) {
                if (pixels[i] != 0) {
                    hasNonZeroPixels = true;
                    nonZeroCount++;
                    if (nonZeroCount > 10) break;
                }
            }
            System.out.println("[DEBUG] Non-zero pixel count: " + nonZeroCount + " / " + pixels.length);
            System.out.println("[DEBUG] Has non-zero pixels: " + hasNonZeroPixels);
            
            gc.drawImage(image, 0, 0);
            System.out.println("[DEBUG] Image drawn successfully");
        } else {
            System.out.println("[WARNING] No valid pixel data to draw");
        }
    }

    public void clear() {
        if (device != null) {
            device.clear();
        }
        GraphicsContext gc = canvas.getGraphicsContext2D();
        gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());
    }

    public Canvas getCanvas() {
        return canvas;
    }

    public ChartViewer getViewer() {
        return viewer;
    }

    public void dispose() {
        if (!disposed) {
            disposed = true;
            for (ChartEventHandler handler : handlers) {
                handler.uninstall();
            }
            handlers.clear();
            if (device != null) {
                device.dispose();
                device = null;
            }
        }
    }

    public boolean isDisposed() {
        return disposed;
    }
}
