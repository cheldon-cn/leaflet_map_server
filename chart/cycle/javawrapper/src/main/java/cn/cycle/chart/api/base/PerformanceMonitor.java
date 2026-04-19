package cn.cycle.chart.api.base;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class PerformanceMonitor extends NativeObject {

    static {
        JniBridge.initialize();
    }

    private PerformanceMonitor() {
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native long nativeGetInstance();
    private native void nativeStartFrame(long ptr);
    private native void nativeEndFrame(long ptr);
    private native double nativeGetFps(long ptr);
    private native long nativeGetMemoryUsed(long ptr);
    private native void nativeReset(long ptr);
}
