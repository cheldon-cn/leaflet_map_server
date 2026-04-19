package cn.cycle.chart.api.symbology;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class ComparisonFilter extends NativeObject {

    static {
        JniBridge.initialize();
    }

    ComparisonFilter(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native void nativeDestroy(long ptr);
    private native boolean nativeEvaluate(long ptr, long featurePtr);
    private native String nativeGetPropertyName(long ptr);
    private native int nativeGetOperator(long ptr);
    private native String nativeGetValue(long ptr);
    private static native long nativeCreate(String property, int op, String value);
}
