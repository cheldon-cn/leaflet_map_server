package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.JniBridge;

public final class Polygon extends Geometry {

    static {
        JniBridge.initialize();
    }

    public Polygon() {
        setNativePtr(nativeCreate());
    }

    Polygon(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public static Polygon createFromRing(LinearRing exteriorRing) {
        long ptr = nativeCreateFromRing(exteriorRing.getNativePtr());
        return ptr != 0 ? new Polygon(ptr) : null;
    }

    public static Polygon createFromCoords(double[] exteriorCoords) {
        long ptr = nativeCreateFromCoords(exteriorCoords);
        return ptr != 0 ? new Polygon(ptr) : null;
    }

    public long getNumInteriorRings() {
        checkNotDisposed();
        return nativeGetNumInteriorRings(getNativePtr());
    }

    public LinearRing getExteriorRing() {
        checkNotDisposed();
        long ptr = nativeGetExteriorRing(getNativePtr());
        return ptr != 0 ? new LinearRing(ptr) : null;
    }

    public LinearRing getInteriorRingN(int index) {
        checkNotDisposed();
        long ptr = nativeGetInteriorRingN(getNativePtr(), index);
        return ptr != 0 ? new LinearRing(ptr) : null;
    }

    public void addInteriorRing(LinearRing ring) {
        checkNotDisposed();
        if (ring == null) {
            throw new IllegalArgumentException("ring must not be null");
        }
        nativeAddInteriorRing(getNativePtr(), ring.getNativePtr());
    }

    public double getArea() {
        checkNotDisposed();
        return nativeGetArea(getNativePtr());
    }

    public Coordinate getCentroid() {
        checkNotDisposed();
        double[] coord = nativeGetCentroid(getNativePtr());
        return new Coordinate(coord[0], coord[1]);
    }

    @Override
    public Type getType() {
        return Type.POLYGON;
    }

    private native static long nativeCreate();
    private native static long nativeCreateFromRing(long ringPtr);
    private native static long nativeCreateFromCoords(double[] coords);
    private native long nativeGetNumInteriorRings(long ptr);
    private native long nativeGetExteriorRing(long ptr);
    private native long nativeGetInteriorRingN(long ptr, int index);
    private native void nativeAddInteriorRing(long ptr, long ringPtr);
    private native double nativeGetArea(long ptr);
    private native double[] nativeGetCentroid(long ptr);
}
