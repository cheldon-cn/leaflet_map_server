package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class DrawDevice extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public DrawDevice(int type, int width, int height) {
        setNativePtr(nativeCreate(type, width, height));
    }

    DrawDevice(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(int type, int width, int height);
    private static native void nativeDestroy(long ptr);
    private native int nativeGetWidth(long ptr);
    private native int nativeGetHeight(long ptr);
    private native long nativeGetImage(long ptr);
}
