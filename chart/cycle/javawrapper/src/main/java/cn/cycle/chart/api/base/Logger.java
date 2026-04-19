package cn.cycle.chart.api.base;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class Logger extends NativeObject {

    static {
        JniBridge.initialize();
    }


    Logger(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native long nativeGetInstance();
    private native void nativeSetLevel(long ptr, int level);
    private native void nativeTrace(long ptr, String message);
    private native void nativeDebug(long ptr, String message);
    private native void nativeInfo(long ptr, String message);
    private native void nativeWarning(long ptr, String message);
    private native void nativeError(long ptr, String message);
    private native void nativeFatal(long ptr, String message);
    private native void nativeSetLogFile(long ptr, String filepath);
    private native void nativeSetConsoleOutput(long ptr, int enable);
}
