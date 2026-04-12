package cn.cycle.chart.api.adapter;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public class ImageDevice extends NativeObject {

    static {
        JniBridge.initialize();
    }

    private int width;
    private int height;

    public ImageDevice(int width, int height) {
        this.width = width;
        this.height = height;
        long ptr = nativeCreate(width, height);
        setNativePtr(ptr);
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public void resize(int width, int height) {
        if (this.width != width || this.height != height) {
            this.width = width;
            this.height = height;
            nativeResize(getNativePtr(), width, height);
        }
    }

    public void clear() {
        checkNotDisposed();
        nativeClear(getNativePtr());
    }

    public byte[] getPixels() {
        checkNotDisposed();
        byte[] pixels = nativeGetPixels(getNativePtr());
        return pixels;
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(int width, int height);
    private static native void nativeDestroy(long ptr);
    private static native void nativeResize(long ptr, int width, int height);
    private static native void nativeClear(long ptr);
    private static native byte[] nativeGetPixels(long ptr);
}
