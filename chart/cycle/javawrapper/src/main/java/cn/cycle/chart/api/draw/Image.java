package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class Image extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public Image() {
        setNativePtr(nativeCreate());
    }

    Image(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(int width, int height, int channels);
    private static native void nativeDestroy(long ptr);
    private native int nativeGetWidth(long ptr);
    private native int nativeGetHeight(long ptr);
    private native int nativeGetChannels(long ptr);
    private static native long nativeLoadFromFile(String path);
    private native boolean nativeSaveToFile(long ptr, String path);
    private native long nativeGetDataPtr(long ptr);
    private native long nativeGetDataConstPtr(long ptr);
}
