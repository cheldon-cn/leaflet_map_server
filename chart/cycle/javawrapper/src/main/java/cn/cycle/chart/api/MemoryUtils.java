package cn.cycle.chart.api;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class MemoryUtils extends NativeObject {

    static {
        JniBridge.initialize();
    }


    MemoryUtils(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native void nativeFree(long ptr);
    private static native void nativeFreeMemory(long ptr);
    private static native void nativeFreeString(long ptr);
    private static native void nativeStringArrayDestroy(long arrayPtr, int count);
    private static native void nativeGeometryArrayDestroy(long arrayPtr, int count);
}
