package cn.cycle.chart.api.symbology;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public abstract class Filter extends NativeObject {

    static {
        JniBridge.initialize();
    }

    Filter(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native int nativeGetType(long ptr);
    private native boolean nativeEvaluate(long ptr, long featurePtr);
    private native String nativeToString(long ptr);
}
