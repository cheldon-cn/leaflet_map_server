package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class PositionManager extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public PositionManager() {
        setNativePtr(nativeCreate());
    }

    PositionManager(long nativePtr) {
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
    private native double nativeGetLatitude(long ptr);
    private native double nativeGetLongitude(long ptr);
    private native double nativeGetSpeed(long ptr);
    private native double nativeGetCourse(long ptr);
    private native double nativeGetHeading(long ptr);
    private native int nativeGetFixQuality(long ptr);
    private native int nativeGetSatelliteCount(long ptr);
    private native int nativeSetPosition(long ptr, double latitude, double longitude);
    private native double[] nativeGetPosition(long ptr);
    private native void nativeSetCallback(long ptr, long callbackPtr);
}
