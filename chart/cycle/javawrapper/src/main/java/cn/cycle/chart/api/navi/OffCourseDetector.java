package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class OffCourseDetector extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public OffCourseDetector() {
        setNativePtr(nativeCreate());
    }

    OffCourseDetector(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native void nativeSetThreshold(long ptr, double meters);
    private native boolean nativeCheck(long ptr, double currentLat, double currentLon, long fromWaypointPtr, long toWaypointPtr);
    private native double nativeGetCrossTrackError(long ptr, double currentLat, double currentLon, long fromWaypointPtr, long toWaypointPtr);
}
