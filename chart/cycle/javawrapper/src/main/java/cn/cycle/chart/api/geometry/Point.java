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

    public static Point createFromCoord(Coordinate coord) {
        long ptr = nativeCreateFromCoord(coord.getX(), coord.getY());
        return ptr != 0 ? new Point(ptr) : null;
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

    public double getM() {
        checkNotDisposed();
        return nativeGetM(getNativePtr());
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

    public void setM(double m) {
        checkNotDisposed();
        nativeSetM(getNativePtr(), m);
    }

    public Coordinate getCoordinate() {
        checkNotDisposed();
        double[] coord = nativeGetCoordinate(getNativePtr());
        if (coord == null || coord.length < 2) {
            return new Coordinate(getX(), getY());
        }
        if (coord.length >= 3) {
            return new Coordinate(coord[0], coord[1], coord[2]);
        }
        return new Coordinate(coord[0], coord[1]);
    }

    @Override
    public Type getType() {
        return Type.POINT;
    }

    private native static long nativeCreate(double x, double y);
    private native static long nativeCreate3D(double x, double y, double z);
    private native static long nativeCreateFromCoord(double x, double y);
    private native double nativeGetX(long ptr);
    private native double nativeGetY(long ptr);
    private native double nativeGetZ(long ptr);
    private native double nativeGetM(long ptr);
    private native void nativeSetX(long ptr, double x);
    private native void nativeSetY(long ptr, double y);
    private native void nativeSetZ(long ptr, double z);
    private native void nativeSetM(long ptr, double m);
    private native double[] nativeGetCoordinate(long ptr);
}
