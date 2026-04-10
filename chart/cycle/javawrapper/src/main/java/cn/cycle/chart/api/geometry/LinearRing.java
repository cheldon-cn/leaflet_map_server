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

    public boolean isClosed() {
        checkNotDisposed();
        return nativeIsClosed(getNativePtr());
    }

    @Override
    public Type getType() {
        return Type.LINESTRING;
    }

    private native static long nativeCreate();
    private native boolean nativeIsClosed(long ptr);
}
