package cn.cycle.chart.api.exception;

import cn.cycle.chart.jni.JniBridge;

public final class ChartException extends RuntimeException {

    static {
        JniBridge.initialize();
    }

    private long nativePtr;

    public ChartException(int errorCode, String message) {
        super(message);
        this.nativePtr = nativeCreate(errorCode, message);
    }

    ChartException(long nativePtr) {
        super();
        this.nativePtr = nativePtr;
    }

    public int getErrorCode() {
        return nativeGetErrorCode(nativePtr);
    }

    public String getNativeMessage() {
        return nativeGetMessage(nativePtr);
    }

    public String getContext() {
        return nativeGetContext(nativePtr);
    }

    public void dispose() {
        if (nativePtr != 0) {
            nativeDestroy(nativePtr);
            nativePtr = 0;
        }
    }
    private static native long nativeCreate(int errorCode, String message);
    private static native void nativeDestroy(long ptr);
    private native int nativeGetErrorCode(long ptr);
    private native String nativeGetMessage(long ptr);
    private native String nativeGetContext(long ptr);
}
