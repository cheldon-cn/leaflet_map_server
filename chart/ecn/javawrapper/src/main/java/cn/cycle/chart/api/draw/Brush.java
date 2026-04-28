package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class Brush extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public Brush() {
        setNativePtr(nativeCreate());
    }

    Brush(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native int nativeGetColor(long ptr);
    private native void nativeSetColor(long ptr, int packedColor);
    private native int nativeGetStyle(long ptr);
    private native void nativeSetStyle(long ptr, int style);
}
