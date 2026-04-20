package cn.cycle.chart.api.exception;

import cn.cycle.chart.jni.JniBridge;

public final class RenderException extends RuntimeException {

    static {
        JniBridge.initialize();
    }

    private long nativePtr;

    public RenderException(String message, String layerName) {
        super(message);
        this.nativePtr = nativeCreate(message, layerName);
    }

    RenderException(long nativePtr) {
        super();
        this.nativePtr = nativePtr;
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

    private static native long nativeCreate(String message, String layerName);
    private static native void nativeDestroy(long ptr);
    private static native String nativeGetMessage(long ptr);
    private static native String nativeGetContext(long ptr);
}
