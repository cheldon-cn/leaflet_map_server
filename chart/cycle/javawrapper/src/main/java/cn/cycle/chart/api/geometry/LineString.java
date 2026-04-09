package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.JniBridge;

public final class LineString extends Geometry {

    static {
        JniBridge.initialize();
    }

    public LineString() {
        setNativePtr(nativeCreate());
    }

    LineString(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public long getNumPoints() {
        checkNotDisposed();
        return nativeGetNumPoints(getNativePtr());
    }

    public void addPoint(double x, double y) {
        checkNotDisposed();
        nativeAddPoint(getNativePtr(), x, y);
    }

    public void addPoint(Coordinate coord) {
        checkNotDisposed();
        if (coord == null) {
            throw new IllegalArgumentException("coordinate must not be null");
        }
        nativeAddPoint(getNativePtr(), coord.getX(), coord.getY());
    }

    @Override
    public Type getType() {
        return Type.LINESTRING;
    }

    private native long nativeCreate();
    private native long nativeGetNumPoints(long ptr);
    private native void nativeAddPoint(long ptr, double x, double y);
}
