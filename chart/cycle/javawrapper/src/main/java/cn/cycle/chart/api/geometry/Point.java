package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.JniBridge;

public final class Point extends Geometry {

    static {
        JniBridge.initialize();
    }

    public Point(double x, double y) {
        setNativePtr(nativeCreate(x, y));
    }

    public Point(double x, double y, double z) {
        setNativePtr(nativeCreate3D(x, y, z));
    }

    Point(long nativePtr) {
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

    public void setX(double x) {
        checkNotDisposed();
        nativeSetX(getNativePtr(), x);
    }

    public void setY(double y) {
        checkNotDisposed();
        nativeSetY(getNativePtr(), y);
    }

    public void setZ(double z) {
        checkNotDisposed();
        nativeSetZ(getNativePtr(), z);
    }

    public Coordinate getCoordinate() {
        checkNotDisposed();
        return new Coordinate(getX(), getY(), getZ());
    }

    @Override
    public Type getType() {
        return Type.POINT;
    }

    private native long nativeCreate(double x, double y);
    private native long nativeCreate3D(double x, double y, double z);
    private native double nativeGetX(long ptr);
    private native double nativeGetY(long ptr);
    private native double nativeGetZ(long ptr);
    private native void nativeSetX(long ptr, double x);
    private native void nativeSetY(long ptr, double y);
    private native void nativeSetZ(long ptr, double z);
}
