package cn.cycle.chart.api.graph;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class RenderOptimizer extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public RenderOptimizer() {
        setNativePtr(nativeCreate());
    }

    RenderOptimizer(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native void nativeSetCacheEnabled(long ptr, boolean enable);
    private native boolean nativeIsCacheEnabled(long ptr);
    private native void nativeClearCache(long ptr);
}
