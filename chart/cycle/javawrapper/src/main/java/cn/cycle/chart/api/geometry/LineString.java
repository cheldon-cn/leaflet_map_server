package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.JniBridge;

public class LineString extends Geometry {

    static {
        JniBridge.initialize();
    }

    public LineString() {
        setNativePtr(nativeCreate());
    }

    LineString(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public Coordinate getCoordinateN(int index) {
        checkNotDisposed();
        double[] coord = nativeGetPointN(getNativePtr(), index);
        return new Coordinate(coord[0], coord[1]);
    }

    public double getLength() {
        checkNotDisposed();
        return nativeGetLength(getNativePtr());
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

    private native static long nativeCreate();
    private native double[] nativeGetPointN(long ptr, int index);
    private native double nativeGetLength(long ptr);
    private native void nativeAddPoint(long ptr, double x, double y);
}
