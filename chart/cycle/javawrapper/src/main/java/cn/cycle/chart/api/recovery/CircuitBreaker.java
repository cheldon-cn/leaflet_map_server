package cn.cycle.chart.api.recovery;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class CircuitBreaker extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public CircuitBreaker() {
        setNativePtr(nativeCreate());
    }

    CircuitBreaker(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(int failureThreshold, int recoveryTimeoutMs);
    private native void nativeDestroy(long ptr);
    private native int nativeGetState(long ptr);
    private native boolean nativeAllowRequest(long ptr);
    private native void nativeRecordSuccess(long ptr);
    private native void nativeRecordFailure(long ptr);
    private native void nativeReset(long ptr);
}
