package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class DriverManager extends NativeObject {

    static {
        JniBridge.initialize();
    }

    private DriverManager() {
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native long nativeGetInstance();
    private native int nativeRegisterDriver(long ptr, long driverPtr);
    private native int nativeUnregisterDriver(long ptr, String name);
    private native long nativeGetDriver(long ptr, String name);
    private native int nativeGetDriverCount(long ptr);
    private native String nativeGetDriverName(long ptr, int index);
}
