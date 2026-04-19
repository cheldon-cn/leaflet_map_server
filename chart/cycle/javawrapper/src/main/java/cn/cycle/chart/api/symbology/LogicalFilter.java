package cn.cycle.chart.api.symbology;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class LogicalFilter extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public LogicalFilter() {
        setNativePtr(nativeCreate());
    }

    LogicalFilter(long nativePtr) {
        setNativePtr(nativePtr);
    }

    private static native long nativeCreate(int op);
    private native void nativeAddFilter(long ptr, long filterPtr);
}
