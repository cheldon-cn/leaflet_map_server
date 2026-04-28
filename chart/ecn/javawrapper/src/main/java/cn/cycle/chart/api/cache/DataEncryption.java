package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class DataEncryption extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public DataEncryption(String key) {
        setNativePtr(nativeCreate(key));
    }

    DataEncryption(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(String key);
    private native void nativeDestroy(long ptr);
    private native byte[] nativeEncrypt(long ptr, byte[] inputData);
    private native byte[] nativeDecrypt(long ptr, byte[] inputData);
}
