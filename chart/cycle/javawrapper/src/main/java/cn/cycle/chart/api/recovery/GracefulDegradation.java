package cn.cycle.chart.api.recovery;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class GracefulDegradation extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public GracefulDegradation() {
        setNativePtr(nativeCreate());
    }

    GracefulDegradation(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native int nativeGetLevel(long ptr);
    private native void nativeSetLevel(long ptr, int level);
    private native boolean nativeIsFeatureEnabled(long ptr, String feature);
    private native void nativeDisableFeature(long ptr, String feature);
    private native void nativeEnableFeature(long ptr, String feature);
}
