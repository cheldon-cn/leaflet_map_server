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

    public long getNumInteriorRings() {
        checkNotDisposed();
        return nativeGetNumInteriorRings(getNativePtr());
    }

    @Override
    public Type getType() {
        return Type.POLYGON;
    }

    private native long nativeCreate();
    private native long nativeGetNumInteriorRings(long ptr);
}
