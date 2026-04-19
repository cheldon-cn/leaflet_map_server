package cn.cycle.chart.api.symbology;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class LogicalFilter extends NativeObject {

    static {
        JniBridge.initialize();
    }

    LogicalFilter(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native long nativeCreate(int op);
    private native void nativeAddFilter(long ptr, long filterPtr);
}
