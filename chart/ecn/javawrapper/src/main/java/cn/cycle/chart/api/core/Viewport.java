package cn.cycle.chart.api.core;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.geometry.Envelope;

public final class Viewport extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public Viewport() {
        setNativePtr(nativeCreate());
    }

    Viewport(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public double getCenterX() {
        checkNotDisposed();
        return nativeGetCenterX(getNativePtr());
    }

    public double getCenterY() {
        checkNotDisposed();
        return nativeGetCenterY(getNativePtr());
    }

    public double getScale() {
        checkNotDisposed();
        return nativeGetScale(getNativePtr());
    }

    public double getRotation() {
        checkNotDisposed();
        return nativeGetRotation(getNativePtr());
    }

    public void setCenter(double x, double y) {
        checkNotDisposed();
        nativeSetCenter(getNativePtr(), x, y);
    }

    public void setScale(double scale) {
        checkNotDisposed();
        nativeSetScale(getNativePtr(), scale);
    }

    public void setRotation(double rotation) {
        checkNotDisposed();
        nativeSetRotation(getNativePtr(), rotation);
    }

    public Coordinate getCenter() {
        checkNotDisposed();
        return new Coordinate(getCenterX(), getCenterY());
    }

    public Envelope getExtent() {
        checkNotDisposed();
        double[] extent = new double[4];
        nativeGetExtent(getNativePtr(), extent);
        return new Envelope(extent[0], extent[1], extent[2], extent[3]);
    }

    public void setExtent(Envelope extent) {
        if (extent == null) {
            throw new IllegalArgumentException("extent must not be null");
        }
        checkNotDisposed();
        nativeSetExtent(getNativePtr(), 
            extent.getMinX(), extent.getMinY(), 
            extent.getMaxX(), extent.getMaxY());
    }

    public void pan(double dx, double dy) {
        checkNotDisposed();
        nativePan(getNativePtr(), dx, dy);
    }

    public void zoom(double factor) {
        checkNotDisposed();
        nativeZoom(getNativePtr(), factor);
    }

    public void zoom(double factor, double centerX, double centerY) {
        checkNotDisposed();
        nativeZoomAt(getNativePtr(), factor, centerX, centerY);
    }

    public void setSize(int width, int height) {
        if (width <= 0 || height <= 0) {
            throw new IllegalArgumentException("Width and height must be positive");
        }
        checkNotDisposed();
        nativeSetSize(getNativePtr(), width, height);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private native static long nativeCreate();
    private native static void nativeDestroy(long ptr);
    private native double nativeGetCenterX(long ptr);
    private native double nativeGetCenterY(long ptr);
    private native double nativeGetScale(long ptr);
    private native double nativeGetRotation(long ptr);
    private native void nativeSetCenter(long ptr, double x, double y);
    private native void nativeSetScale(long ptr, double scale);
    private native void nativeSetRotation(long ptr, double rotation);
    private native void nativeGetExtent(long ptr, double[] outExtent);
    private native void nativeSetExtent(long ptr, double minX, double minY, double maxX, double maxY);
    private native void nativePan(long ptr, double dx, double dy);
    private native void nativeZoom(long ptr, double factor);
    private native void nativeZoomAt(long ptr, double factor, double centerX, double centerY);
    private native void nativeSetSize(long ptr, int width, int height);
}
