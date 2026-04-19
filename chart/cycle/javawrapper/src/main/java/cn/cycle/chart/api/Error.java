package cn.cycle.chart.api;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class Error extends NativeObject {

    static {
        JniBridge.initialize();
    }


    Error(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native String nativeGetMessage(int code);
}
