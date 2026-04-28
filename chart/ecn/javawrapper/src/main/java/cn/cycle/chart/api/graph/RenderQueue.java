package cn.cycle.chart.api.graph;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class RenderQueue extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public RenderQueue() {
        setNativePtr(nativeCreate());
    }

    RenderQueue(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private static native void nativeDestroy(long ptr);
    private native void nativePush(long ptr, long taskPtr);
    private native long nativePop(long ptr);
    private native long nativeGetSize(long ptr);
    private native void nativeClear(long ptr);
}
