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

    public static LineString createFromCoords(double[] coords) {
        long ptr = nativeCreateFromCoords(coords);
        return ptr != 0 ? new LineString(ptr) : null;
    }

    public long getNumPoints() {
        checkNotDisposed();
        return Geometry.nativeGetNumPoints(getNativePtr());
    }

    public Coordinate getCoordinateN(int index) {
        checkNotDisposed();
        double[] coord = nativeGetPointN(getNativePtr(), index);
        return new Coordinate(coord[0], coord[1]);
    }

    public void setCoordinateN(int index, double x, double y) {
        checkNotDisposed();
        nativeSetPointN(getNativePtr(), index, x, y);
    }

    public Point getPointGeometry(int index) {
        checkNotDisposed();
        long ptr = nativeGetPointGeometry(getNativePtr(), index);
        return ptr != 0 ? new Point(ptr) : null;
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

    public void addPoint3D(double x, double y, double z) {
        checkNotDisposed();
        nativeAddPoint3D(getNativePtr(), x, y, z);
    }

    @Override
    public Type getType() {
        return Type.LINESTRING;
    }

    private native static long nativeCreate();
    private native static long nativeCreateFromCoords(double[] coords);
    private native double[] nativeGetPointN(long ptr, int index);
    private native void nativeSetPointN(long ptr, int index, double x, double y);
    private native long nativeGetPointGeometry(long ptr, int index);
    private native double nativeGetLength(long ptr);
    private native void nativeAddPoint(long ptr, double x, double y);
    private native void nativeAddPoint3D(long ptr, double x, double y, double z);
}
