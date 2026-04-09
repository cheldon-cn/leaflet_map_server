package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class Envelope extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public Envelope(double minX, double minY, double maxX, double maxY) {
        setNativePtr(nativeCreate(minX, minY, maxX, maxY));
    }

    private Envelope(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public double getMinX() {
        checkNotDisposed();
        return nativeGetMinX(getNativePtr());
    }

    public double getMinY() {
        checkNotDisposed();
        return nativeGetMinY(getNativePtr());
    }

    public double getMaxX() {
        checkNotDisposed();
        return nativeGetMaxX(getNativePtr());
    }

    public double getMaxY() {
        checkNotDisposed();
        return nativeGetMaxY(getNativePtr());
    }

    public double getWidth() {
        checkNotDisposed();
        return nativeGetWidth(getNativePtr());
    }

    public double getHeight() {
        checkNotDisposed();
        return nativeGetHeight(getNativePtr());
    }

    public boolean contains(double x, double y) {
        checkNotDisposed();
        return nativeContains(getNativePtr(), x, y);
    }

    public boolean intersects(Envelope other) {
        checkNotDisposed();
        if (other == null) {
            throw new IllegalArgumentException("other envelope must not be null");
        }
        return nativeIntersects(getNativePtr(), other.getNativePtr());
    }

    public Coordinate getCenter() {
        checkNotDisposed();
        double cx = (getMinX() + getMaxX()) / 2.0;
        double cy = (getMinY() + getMaxY()) / 2.0;
        return new Coordinate(cx, cy);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private native long nativeCreate(double minX, double minY, double maxX, double maxY);
    private native void nativeDestroy(long ptr);
    private native double nativeGetMinX(long ptr);
    private native double nativeGetMinY(long ptr);
    private native double nativeGetMaxX(long ptr);
    private native double nativeGetMaxY(long ptr);
    private native double nativeGetWidth(long ptr);
    private native double nativeGetHeight(long ptr);
    private native boolean nativeContains(long ptr, double x, double y);
    private native boolean nativeIntersects(long ptr, long otherPtr);
}
