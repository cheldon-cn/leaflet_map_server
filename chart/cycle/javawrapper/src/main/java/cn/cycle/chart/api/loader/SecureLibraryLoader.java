package cn.cycle.chart.api.loader;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class SecureLibraryLoader extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public SecureLibraryLoader() {
        setNativePtr(nativeCreate());
    }

    SecureLibraryLoader(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(String allowedPaths);
    private static native void nativeDestroy(long ptr);
    private native long nativeLoad(long ptr, String libraryPath);
    private native int nativeVerifySignature(long ptr, long handlePtr, String publicKey);
}
