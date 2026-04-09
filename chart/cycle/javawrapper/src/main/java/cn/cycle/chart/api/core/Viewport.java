package cn.cycle.chart.api.core;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Coordinate;

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

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native double nativeGetCenterX(long ptr);
    private native double nativeGetCenterY(long ptr);
    private native double nativeGetScale(long ptr);
    private native double nativeGetRotation(long ptr);
    private native void nativeSetCenter(long ptr, double x, double y);
    private native void nativeSetScale(long ptr, double scale);
    private native void nativeSetRotation(long ptr, double rotation);
}
