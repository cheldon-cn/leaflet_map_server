package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class DrawStyle extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public DrawStyle() {
        setNativePtr(nativeCreate());
    }

    DrawStyle(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native long nativeGetPen(long ptr);
    private native long nativeGetBrush(long ptr);
    private native long nativeGetFont(long ptr);
    private native void nativeSetPen(long ptr, long penPtr);
    private native void nativeSetBrush(long ptr, long brushPtr);
    private native void nativeSetFont(long ptr, long fontPtr);
}
