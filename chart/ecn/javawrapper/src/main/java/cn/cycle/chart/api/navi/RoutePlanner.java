package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class RoutePlanner extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public RoutePlanner() {
        setNativePtr(nativeCreate());
    }

    RoutePlanner(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native long nativePlanRoute(long ptr, long startPtr, long endPtr);
}
