package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class DrawEngine extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public DrawEngine() {
        setNativePtr(nativeCreate());
    }

    DrawEngine(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(int type);
    private static native void nativeDestroy(long ptr);
    private native boolean nativeInitialize(long ptr, long devicePtr);
    private native void nativeFinalize(long ptr);
}
