package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.api.geometry.Geometry;

public final class MultiLineString extends Geometry {

    static {
        JniBridge.initialize();
    }

    public MultiLineString() {
        setNativePtr(nativeCreate());
    }

    MultiLineString(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    public Type getType() {
        return Type.MULTILINESTRING;
    }

    private static native long nativeCreate();
    private native long nativeGetNumGeometries(long ptr);
    private native long nativeGetGeometryN(long ptr, long index);
    private native void nativeAddGeometry(long ptr, long lineStringPtr);
}
