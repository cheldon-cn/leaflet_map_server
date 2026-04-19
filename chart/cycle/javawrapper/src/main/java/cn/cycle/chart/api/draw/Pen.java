package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class Pen extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public Pen() {
        setNativePtr(nativeCreate());
    }

    Pen(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native int nativeGetColor(long ptr);
    private native double nativeGetWidth(long ptr);
    private native void nativeSetColor(long ptr, int packedColor);
    private native void nativeSetWidth(long ptr, double width);
    private native int nativeGetStyle(long ptr);
    private native void nativeSetStyle(long ptr, int style);
}
