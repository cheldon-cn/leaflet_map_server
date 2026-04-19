package cn.cycle.chart.api.perf;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class PerformanceMonitor extends NativeObject {

    static {
        JniBridge.initialize();
    }

    PerformanceMonitor(long nativePtr) {
        setNativePtr(nativePtr);
    }

    private static native long nativeGetInstance();
    private native void nativeStartFrame(long ptr);
    private native void nativeEndFrame(long ptr);
    private native double nativeGetFPS(long ptr);
    private native long nativeGetMemoryUsed(long ptr);
    private native void nativeReset(long ptr);
    private native void nativeGetStats(long statsPtr);
}
