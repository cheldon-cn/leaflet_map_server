package cn.cycle.chart.api.parser;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class ChartParser extends NativeObject {

    static {
        JniBridge.initialize();
    }

    private ChartParser() {
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native long nativeGetInstance();
    private native int nativeInitialize(long ptr);
    private native void nativeShutdown(long ptr);
    private native int[] nativeGetSupportedFormats(long ptr);
}
