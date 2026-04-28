package cn.cycle.chart.api.graph;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class LabelEngine extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public LabelEngine() {
        setNativePtr(nativeCreate());
    }

    LabelEngine(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private static native void nativeDestroy(long ptr);
    private native void nativeSetMaxLabels(long ptr, int maxLabels);
    private native int nativeGetMaxLabels(long ptr);
    private native void nativeSetCollisionDetection(long ptr, boolean enable);
    private native void nativeClear(long ptr);
}
