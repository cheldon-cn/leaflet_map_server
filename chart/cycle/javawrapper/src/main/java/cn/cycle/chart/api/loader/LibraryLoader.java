package cn.cycle.chart.api.loader;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class LibraryLoader extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public LibraryLoader() {
        setNativePtr(nativeCreate());
    }

    LibraryLoader(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native long nativeLoad(long ptr, String libraryPath);
    private native void nativeUnload(long ptr, long handlePtr);
    private native long nativeGetSymbol(long ptr, long handlePtr, String symbolName);
    private native String nativeGetError(long ptr);
}
