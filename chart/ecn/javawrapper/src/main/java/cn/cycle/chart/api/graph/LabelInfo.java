package cn.cycle.chart.api.graph;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class LabelInfo extends NativeObject {

    static {
        JniBridge.initialize();
    }

    LabelInfo(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(String text, double x, double y);
    private static native void nativeDestroy(long ptr);
    private native String nativeGetText(long ptr);
    private native double nativeGetX(long ptr);
    private native double nativeGetY(long ptr);
    private native void nativeSetText(long ptr, String text);
    private native void nativeSetPosition(long ptr, double x, double y);
}
