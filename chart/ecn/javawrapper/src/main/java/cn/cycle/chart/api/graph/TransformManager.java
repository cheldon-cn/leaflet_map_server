package cn.cycle.chart.api.graph;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class TransformManager extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public TransformManager() {
        setNativePtr(nativeCreate());
    }

    TransformManager(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private static native void nativeDestroy(long ptr);
    private native void nativeSetViewport(long ptr, double cx, double cy, double scale, double rotation);
    private native double[] nativeGetMatrix(long ptr);
}
