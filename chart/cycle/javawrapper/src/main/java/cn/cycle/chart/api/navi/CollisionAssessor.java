package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class CollisionAssessor extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public CollisionAssessor() {
        setNativePtr(nativeCreate());
    }

    CollisionAssessor(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native long nativeAssess(long ptr, double ownLat, double ownLon, double ownSpeed, double ownCourse, long targetPtr);
    private static native void nativeDestroyRisk(long riskPtr);
    private static native double nativeGetRiskCpa(long riskPtr);
    private static native double nativeGetRiskTcpa(long riskPtr);
    private static native int nativeGetRiskLevel(long riskPtr);
}
