package cn.cycle.chart.api.graph;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class RenderTask extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public RenderTask() {
        setNativePtr(nativeCreate());
    }

    RenderTask(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private static native void nativeDestroy(long ptr);
    private native boolean nativeExecute(long ptr);
    private native boolean nativeIsComplete(long ptr);
    private native void nativeCancel(long ptr);
}
