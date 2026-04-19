package cn.cycle.chart.api.perf;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class RenderStats extends NativeObject {

    static {
        JniBridge.initialize();
    }

    RenderStats(long nativePtr) {
        setNativePtr(nativePtr);
    }

    private native void nativeReset(long ptr);
}
