package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class ImageDevice extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public ImageDevice(long width, long height) {
        setNativePtr(nativeCreate(width, height));
    }

    ImageDevice(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(long width, long height);
    private static native void nativeDestroy(long ptr);
    private native boolean nativeResize(long ptr, long width, long height);
    private native void nativeClear(long ptr);
    private native byte[] nativeGetPixels(long ptr);
    private native int nativeGetWidth(long ptr);
    private native int nativeGetHeight(long ptr);
    private native long nativeGetNativePtr(long ptr);
}
