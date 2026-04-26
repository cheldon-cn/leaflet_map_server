package cn.cycle.chart.api.adapter;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.core.Viewport;
import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.geometry.Envelope;
import javafx.application.Platform;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritablePixelFormat;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;

public class CanvasAdapter {

    private final Canvas canvas;
    private final ChartViewer viewer;
    private final List<ChartEventHandler> handlers;
    private ImageDevice device;
    private boolean disposed;
    private javafx.scene.image.WritableImage bufferImage;
    private int lastWidth = 0;
    private int lastHeight = 0;

    private final AtomicBoolean isRendering = new AtomicBoolean(false);
    private final AtomicBoolean renderPending = new AtomicBoolean(false);
    private final AtomicLong lastRenderTime = new AtomicLong(0);
    private static final long MIN_RENDER_INTERVAL_MS = 16;
    private static final long THROTTLE_DELAY_MS = 8;
    private volatile byte[] pendingPixels = null;
    private volatile int pendingWidth = 0;
    private volatile int pendingHeight = 0;

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
        if (disposed) {
            return;
        }

        long now = System.currentTimeMillis();
        long timeSinceLastRender = now - lastRenderTime.get();
        
        if (timeSinceLastRender < MIN_RENDER_INTERVAL_MS) {
            if (renderPending.compareAndSet(false, true)) {
                scheduleThrottledRender();
            }
            return;
        }

        if (isRendering.get()) {
            renderPending.set(true);
            return;
        }

        startAsyncRender();
    }

    private void scheduleThrottledRender() {
        new Thread(() -> {
            try {
                Thread.sleep(THROTTLE_DELAY_MS);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
            Platform.runLater(() -> {
                renderPending.set(false);
                render();
            });
        }).start();
    }

    private void startAsyncRender() {
        if (!isRendering.compareAndSet(false, true)) {
            return;
        }

        final int width = (int) canvas.getWidth();
        final int height = (int) canvas.getHeight();

        if (width <= 0 || height <= 0) {
            isRendering.set(false);
            return;
        }

        new Thread(() -> {
            try {
                doRender(width, height);
            } finally {
                isRendering.set(false);
                lastRenderTime.set(System.currentTimeMillis());
                
                if (renderPending.getAndSet(false)) {
                    Platform.runLater(this::render);
                }
            }
        }).start();
    }

    private void doRender(int width, int height) {
        ImageDevice localDevice = device;
        if (localDevice == null) {
            localDevice = new ImageDevice(width, height);
            device = localDevice;
        } else {
            localDevice.resize(width, height);
        }

        long devicePtr = localDevice.getNativePtr();
        
        Viewport viewport = viewer.getViewportObject();
        if (viewport != null) {
            viewport.setSize(width, height);
        }
        
        final int renderResult = viewer.render(devicePtr, width, height);
        final byte[] pixels = localDevice.getPixels();
        
        if (pixels != null && pixels.length >= width * height * 4) {
            pendingPixels = pixels;
            pendingWidth = width;
            pendingHeight = height;
            
            Platform.runLater(() -> updateCanvas());
        }
    }

    private void updateCanvas() {
        if (disposed || pendingPixels == null) {
            return;
        }

        int width = pendingWidth;
        int height = pendingHeight;
        byte[] pixels = pendingPixels;

        if (bufferImage == null || lastWidth != width || lastHeight != height) {
            bufferImage = new javafx.scene.image.WritableImage(width, height);
            lastWidth = width;
            lastHeight = height;
        }
        
        PixelWriter writer = bufferImage.getPixelWriter();
        WritablePixelFormat<ByteBuffer> format = WritablePixelFormat.getByteBgraPreInstance();
        writer.setPixels(0, 0, width, height, format, ByteBuffer.wrap(pixels), width * 4);
        
        GraphicsContext gc = canvas.getGraphicsContext2D();
        gc.clearRect(0, 0, width, height);
        gc.drawImage(bufferImage, 0, 0);
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
            isRendering.set(false);
            renderPending.set(false);
            pendingPixels = null;
            for (ChartEventHandler handler : handlers) {
                handler.uninstall();
            }
            handlers.clear();
            if (device != null) {
                device.dispose();
                device = null;
            }
            bufferImage = null;
        }
    }

    public boolean isDisposed() {
        return disposed;
    }
}
