package cn.cycle.chart.api.alert;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class UkcCalculator extends NativeObject {

    static {
        JniBridge.initialize();
    }

    UkcCalculator(long nativePtr) {
        setNativePtr(nativePtr);
    }

    private static native Object nativeCalculate(double chartedDepth, double tideHeight, double draft, double speed, double squatCoeff);
}
