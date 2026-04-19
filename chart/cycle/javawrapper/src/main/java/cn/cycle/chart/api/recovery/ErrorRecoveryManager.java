package cn.cycle.chart.api.recovery;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class ErrorRecoveryManager extends NativeObject {

    static {
        JniBridge.initialize();
    }

    ErrorRecoveryManager(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native int nativeInitialize(long ptr);
    private native void nativeShutdown(long ptr);
    private native int nativeHandleError(long ptr, int errorCode, String context);
    private native void nativeSetDegradationLevel(long ptr, int level);
    private native void nativeRegisterStrategy(long ptr, int errorCode, long strategyPtr);
}
