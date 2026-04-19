package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class Font extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public Font(String family, double size) {
        setNativePtr(nativeCreate(family, size));
    }

    Font(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(String family, double size);
    private native void nativeDestroy(long ptr);
    private native String nativeGetFamily(long ptr);
    private native double nativeGetSize(long ptr);
    private native boolean nativeIsBold(long ptr);
    private native boolean nativeIsItalic(long ptr);
    private native void nativeSetBold(long ptr, boolean bold);
    private native void nativeSetItalic(long ptr, boolean italic);
}
