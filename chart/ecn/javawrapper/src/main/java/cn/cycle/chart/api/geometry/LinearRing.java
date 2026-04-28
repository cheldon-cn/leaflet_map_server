package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.JniBridge;

public final class LinearRing extends LineString {

    static {
        JniBridge.initialize();
    }

    public LinearRing() {
        setNativePtr(nativeCreate());
    }

    LinearRing(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public static LinearRing createFromCoords(double[] coords) {
        long ptr = nativeCreateFromCoords(coords);
        return ptr != 0 ? new LinearRing(ptr) : null;
    }

    public boolean isClosed() {
        checkNotDisposed();
        return nativeIsClosed(getNativePtr());
    }

    @Override
    public Type getType() {
        return Type.LINESTRING;
    }

    private native static long nativeCreate();
    private native static long nativeCreateFromCoords(double[] coords);
    private native boolean nativeIsClosed(long ptr);
}
