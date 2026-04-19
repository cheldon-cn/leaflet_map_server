package cn.cycle.chart.api.feature;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class FeatureDefn extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public FeatureDefn() {
        setNativePtr(nativeCreate());
    }

    FeatureDefn(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(String name);
    private static native void nativeDestroy(long ptr);
    private native String nativeGetName(long ptr);
    private native long nativeGetFieldCount(long ptr);
    private native long nativeGetFieldDefn(long ptr, long index);
    private native int nativeGetFieldIndex(long ptr, String name);
    private native void nativeAddFieldDefn(long ptr, long fieldPtr);
    private native long nativeGetGeomFieldCount(long ptr);
}
