package cn.cycle.chart.api.core;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class RenderContext extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public RenderContext(long devicePtr, long enginePtr) {
        setNativePtr(nativeCreate(devicePtr, enginePtr));
    }

    RenderContext(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(long devicePtr, long enginePtr);
    private static native void nativeDestroy(long ptr);
    private native void nativeBeginDraw(long ptr);
    private native void nativeEndDraw(long ptr);
    private native void nativeClear(long ptr, int r, int g, int b, int a);
    private native void nativeDrawGeometry(long ptr, long geomPtr, long stylePtr);
    private native void nativeDrawPoint(long ptr, double x, double y, long stylePtr);
    private native void nativeDrawLine(long ptr, double x1, double y1, double x2, double y2, long stylePtr);
    private native void nativeDrawRect(long ptr, double x, double y, double w, double h, long stylePtr);
    private native void nativeFillRect(long ptr, double x, double y, double w, double h, long stylePtr);
    private native void nativeDrawText(long ptr, String text, double x, double y, long stylePtr);
    private native void nativeSetTransform(long ptr, double[] matrix);
    private native void nativeResetTransform(long ptr);
    private native void nativeClip(long ptr, long geomPtr);
    private native void nativeResetClip(long ptr);
}
