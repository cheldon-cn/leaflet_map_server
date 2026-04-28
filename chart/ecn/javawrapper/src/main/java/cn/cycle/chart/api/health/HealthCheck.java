package cn.cycle.chart.api.health;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class HealthCheck extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public HealthCheck(String name) {
        setNativePtr(nativeCreate(name));
    }

    HealthCheck(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(String name);
    private native void nativeDestroy(long ptr);
    private native long nativeExecute(long ptr);
    private static native void nativeDestroyResult(long resultPtr);
    private static native int nativeGetResultStatus(long resultPtr);
    private static native String nativeGetResultComponentName(long resultPtr);
    private static native String nativeGetResultMessage(long resultPtr);
    private static native double nativeGetResultDuration(long resultPtr);
}
