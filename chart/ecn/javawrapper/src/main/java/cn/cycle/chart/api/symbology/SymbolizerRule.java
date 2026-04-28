package cn.cycle.chart.api.symbology;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class SymbolizerRule extends NativeObject {

    static {
        JniBridge.initialize();
    }

    SymbolizerRule(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native String nativeGetName(long ptr);
    private native void nativeSetName(long ptr, String name);
    private native long nativeGetFilter(long ptr);
    private native void nativeSetFilter(long ptr, long filterPtr);
    private native long nativeGetSymbolizer(long ptr, long index);
    private native void nativeSetSymbolizer(long ptr, long symPtr);
    private native double nativeGetMinScale(long ptr);
    private native double nativeGetMaxScale(long ptr);
    private native void nativeSetMinScale(long ptr, double scale);
    private native void nativeSetMaxScale(long ptr, double scale);
    private native void nativeAddSymbolizer(long ptr, long symPtr);
    private native boolean nativeIsApplicable(long ptr, long featurePtr, double scale);
    private native void nativeSetScaleRange(long ptr, double minScale, double maxScale);
}
