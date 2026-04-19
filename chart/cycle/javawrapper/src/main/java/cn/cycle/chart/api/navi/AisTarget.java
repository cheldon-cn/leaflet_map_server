package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class AisTarget extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public AisTarget() {
        setNativePtr(nativeCreate());
    }

    AisTarget(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(int mmsi);
    private static native void nativeDestroy(long ptr);
    private native int nativeGetMmsi(long ptr);
    private native String nativeGetName(long ptr);
    private native double nativeGetLatitude(long ptr);
    private native double nativeGetLongitude(long ptr);
    private native double nativeGetSpeed(long ptr);
    private native double nativeGetCourse(long ptr);
    private native double nativeGetHeading(long ptr);
    private native int nativeGetNavStatus(long ptr);
    private native void nativeUpdatePosition(long ptr, double lat, double lon, double speed, double course);
    private static native void nativeArrayDestroy(long arrayPtr, int count);
}
