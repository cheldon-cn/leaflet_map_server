package cn.cycle.chart.api.core;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.geometry.Envelope;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.function.Consumer;

public final class ChartViewer extends NativeObject {

    static {
        JniBridge.initialize();
    }

    private Viewport viewport;
    private ExecutorService asyncExecutor;
    private volatile boolean loading = false;

    public ChartViewer() {
        setNativePtr(nativeCreate());
        int result = initialize();
        if (result != 0) {
            throw new RuntimeException("Failed to initialize ChartViewer, error code: " + result);
        }
        this.asyncExecutor = Executors.newSingleThreadExecutor();
    }

    public int initialize() {
        checkNotDisposed();
        return nativeInitialize(getNativePtr());
    }

    public void shutdown() {
        checkNotDisposed();
        nativeShutdown(getNativePtr());
    }

    public int loadChart(String filePath) {
        checkNotDisposed();
        if (filePath == null) {
            throw new IllegalArgumentException("filePath must not be null");
        }
        System.out.println("[DEBUG] ChartViewer.loadChart() - nativePtr: " + getNativePtr());
        System.out.println("[DEBUG] Loading chart from: " + filePath);
        int result = nativeLoadChart(getNativePtr(), filePath);
        System.out.println("[DEBUG] nativeLoadChart() returned: " + result);
        return result;
    }

    public CompletableFuture<Integer> loadChartAsync(String filePath) {
        checkNotDisposed();
        if (filePath == null) {
            CompletableFuture<Integer> future = new CompletableFuture<>();
            future.completeExceptionally(new IllegalArgumentException("filePath must not be null"));
            return future;
        }
        if (loading) {
            CompletableFuture<Integer> future = new CompletableFuture<>();
            future.completeExceptionally(new IllegalStateException("Another load operation is in progress"));
            return future;
        }
        
        loading = true;
        return CompletableFuture.supplyAsync(() -> {
            try {
                return nativeLoadChart(getNativePtr(), filePath);
            } finally {
                loading = false;
            }
        }, asyncExecutor);
    }

    public void loadChartAsync(String filePath, Consumer<Integer> callback) {
        loadChartAsync(filePath).thenAccept(callback);
    }

    public void loadChartAsync(String filePath, Consumer<Integer> onSuccess, Consumer<Throwable> onError) {
        loadChartAsync(filePath)
            .thenAccept(onSuccess)
            .exceptionally(e -> {
                onError.accept(e);
                return null;
            });
    }

    public boolean isLoading() {
        return loading;
    }

    public CompletableFuture<Envelope> getFullExtentAsync() {
        checkNotDisposed();
        return CompletableFuture.supplyAsync(() -> getFullExtent(), asyncExecutor);
    }

    public CompletableFuture<Coordinate> screenToWorldAsync(double screenX, double screenY) {
        checkNotDisposed();
        return CompletableFuture.supplyAsync(() -> screenToWorld(screenX, screenY), asyncExecutor);
    }

    public CompletableFuture<Coordinate> worldToScreenAsync(double worldX, double worldY) {
        checkNotDisposed();
        return CompletableFuture.supplyAsync(() -> worldToScreen(worldX, worldY), asyncExecutor);
    }

    public int render(long devicePtr, int width, int height) {
        checkNotDisposed();
        System.out.println("[DEBUG] ChartViewer.render() - nativePtr: " + getNativePtr() + ", devicePtr: " + devicePtr + ", size: " + width + "x" + height);
        int result = nativeRender(getNativePtr(), devicePtr, width, height);
        System.out.println("[DEBUG] nativeRender() returned: " + result);
        return result;
    }

    public void setViewport(double centerX, double centerY, double scale) {
        checkNotDisposed();
        nativeSetViewport(getNativePtr(), centerX, centerY, scale);
    }

    public double[] getViewport() {
        checkNotDisposed();
        double[] viewport = new double[3];
        nativeGetViewport(getNativePtr(), viewport);
        return viewport;
    }

    public Viewport getViewportObject() {
        checkNotDisposed();
        if (viewport == null || viewport.isDisposed()) {
            long viewportPtr = nativeGetViewportPtr(getNativePtr());
            if (viewportPtr != 0) {
                viewport = new Viewport(viewportPtr);
            }
        }
        return viewport;
    }

    public void pan(double dx, double dy) {
        checkNotDisposed();
        nativePan(getNativePtr(), dx, dy);
    }

    public void zoom(double factor, double centerX, double centerY) {
        checkNotDisposed();
        nativeZoom(getNativePtr(), factor, centerX, centerY);
    }

    public long queryFeaturePtr(double x, double y) {
        checkNotDisposed();
        return nativeQueryFeature(getNativePtr(), x, y);
    }

    public Coordinate screenToWorld(double screenX, double screenY) {
        checkNotDisposed();
        double[] coord = new double[2];
        nativeScreenToWorld(getNativePtr(), screenX, screenY, coord);
        return new Coordinate(coord[0], coord[1]);
    }

    public Coordinate worldToScreen(double worldX, double worldY) {
        checkNotDisposed();
        double[] coord = new double[2];
        nativeWorldToScreen(getNativePtr(), worldX, worldY, coord);
        return new Coordinate(coord[0], coord[1]);
    }

    public Envelope getFullExtent() {
        checkNotDisposed();
        double[] extent = new double[4];
        nativeGetFullExtent(getNativePtr(), extent);
        return new Envelope(extent[0], extent[1], extent[2], extent[3]);
    }

    public void zoomIn() {
        checkNotDisposed();
        zoom(1.25, 0, 0);
    }

    public void zoomOut() {
        checkNotDisposed();
        zoom(0.8, 0, 0);
    }

    public void setZoom(double scale) {
        checkNotDisposed();
        double[] viewport = getViewport();
        setViewport(viewport[0], viewport[1], scale);
    }

    public void fitToWindow() {
        checkNotDisposed();
        Envelope extent = getFullExtent();
        Viewport vp = getViewportObject();
        if (vp != null && extent != null) {
            vp.setExtent(extent);
        }
    }

    public void resetView() {
        checkNotDisposed();
        Envelope extent = getFullExtent();
        if (extent != null) {
            double centerX = (extent.getMinX() + extent.getMaxX()) / 2;
            double centerY = (extent.getMinY() + extent.getMaxY()) / 2;
            setViewport(centerX, centerY, 1.0);
        }
    }

    public void centerView() {
        checkNotDisposed();
        Envelope extent = getFullExtent();
        if (extent != null) {
            double centerX = (extent.getMinX() + extent.getMaxX()) / 2;
            double centerY = (extent.getMinY() + extent.getMaxY()) / 2;
            double[] viewport = getViewport();
            setViewport(centerX, centerY, viewport[2]);
        }
    }

    @Override
    protected void nativeDispose(long ptr) {
        if (viewport != null) {
            viewport.dispose();
            viewport = null;
        }
        if (asyncExecutor != null) {
            asyncExecutor.shutdown();
            asyncExecutor = null;
        }
        nativeDisposeImpl(ptr);
    }

    private native long nativeCreate();
    private native int nativeInitialize(long ptr);
    private native void nativeShutdown(long ptr);
    private native void nativeDisposeImpl(long ptr);
    private native int nativeLoadChart(long ptr, String filePath);
    private native int nativeRender(long ptr, long devicePtr, int width, int height);
    private native void nativeSetViewport(long ptr, double centerX, double centerY, double scale);
    private native void nativeGetViewport(long ptr, double[] outViewport);
    private native long nativeGetViewportPtr(long ptr);
    private native void nativePan(long ptr, double dx, double dy);
    private native void nativeZoom(long ptr, double factor, double centerX, double centerY);
    private native long nativeQueryFeature(long ptr, double x, double y);
    private native void nativeScreenToWorld(long ptr, double screenX, double screenY, double[] outCoord);
    private native void nativeWorldToScreen(long ptr, double worldX, double worldY, double[] outCoord);
    private native void nativeGetFullExtent(long ptr, double[] outExtent);
}
