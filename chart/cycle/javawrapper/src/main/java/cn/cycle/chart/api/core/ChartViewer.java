package cn.cycle.chart.api.core;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;

public final class ChartViewer extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public ChartViewer() {
        setNativePtr(nativeCreate());
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
        return nativeLoadChart(getNativePtr(), filePath);
    }

    public int render(long devicePtr, int width, int height) {
        checkNotDisposed();
        return nativeRender(getNativePtr(), devicePtr, width, height);
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

    @Override
    protected void nativeDispose(long ptr) {
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
    private native void nativePan(long ptr, double dx, double dy);
    private native void nativeZoom(long ptr, double factor, double centerX, double centerY);
    private native long nativeQueryFeature(long ptr, double x, double y);
    private native void nativeScreenToWorld(long ptr, double screenX, double screenY, double[] outCoord);
    private native void nativeWorldToScreen(long ptr, double worldX, double worldY, double[] outCoord);
}
