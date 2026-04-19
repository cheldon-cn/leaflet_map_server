package cn.cycle.chart.api.alert;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class CpaCalculator extends NativeObject {

    static {
        JniBridge.initialize();
    }


    CpaCalculator(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native Object nativeCalculate(double ownLat, double ownLon, double ownSpeed, double ownCourse, double tgtLat, double tgtLon, double tgtSpeed, double tgtCourse);
}
