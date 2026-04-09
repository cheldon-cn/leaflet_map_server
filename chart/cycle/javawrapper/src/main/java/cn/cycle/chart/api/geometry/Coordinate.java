package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class Coordinate extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public Coordinate(double x, double y) {
        setNativePtr(nativeCreate(x, y));
    }

    public Coordinate(double x, double y, double z) {
        setNativePtr(nativeCreate3D(x, y, z));
    }

    private Coordinate(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public double getX() {
        checkNotDisposed();
        return nativeGetX(getNativePtr());
    }

    public double getY() {
        checkNotDisposed();
        return nativeGetY(getNativePtr());
    }

    public double getZ() {
        checkNotDisposed();
        return nativeGetZ(getNativePtr());
    }

    public double distance(Coordinate other) {
        checkNotDisposed();
        if (other == null) {
            throw new IllegalArgumentException("other coordinate must not be null");
        }
        return nativeDistance(getNativePtr(), other.getNativePtr());
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private native long nativeCreate(double x, double y);
    private native long nativeCreate3D(double x, double y, double z);
    private native void nativeDestroy(long ptr);
    private native double nativeGetX(long ptr);
    private native double nativeGetY(long ptr);
    private native double nativeGetZ(long ptr);
    private native double nativeDistance(long ptrA, long ptrB);
}
