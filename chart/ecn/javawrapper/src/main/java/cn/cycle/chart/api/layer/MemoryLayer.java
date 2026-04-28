package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class MemoryLayer extends NativeObject {

    static {
        JniBridge.initialize();
    }

    MemoryLayer(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native long nativeCreate(String name);
    private native boolean nativeAddFeature(long ptr, long featurePtr);
    private native boolean nativeRemoveFeature(long ptr, long fid);
    private native void nativeClear(long ptr);
    private native long nativeGetFeatureCount(long ptr);
    private static native long nativeCreateFromFeatures(String name, long[] featurePtrs);
}
