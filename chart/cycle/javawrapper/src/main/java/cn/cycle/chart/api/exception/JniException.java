package cn.cycle.chart.api.exception;

import cn.cycle.chart.jni.JniBridge;

public final class JniException extends RuntimeException {

    static {
        JniBridge.initialize();
    }

    private long nativePtr;

    public JniException(int errorCode, String message) {
        super(message);
        this.nativePtr = nativeCreate(errorCode, message);
    }

    JniException(long nativePtr) {
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
    private static native long nativeCreate(String message, String javaClass);
    private static native void nativeDestroy(long ptr);
}
