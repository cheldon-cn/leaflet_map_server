package cn.cycle.chart.api.graph;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class LODManager extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public LODManager() {
        setNativePtr(nativeCreate());
    }

    LODManager(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private static native void nativeDestroy(long ptr);
    private native int nativeGetCurrentLevel(long ptr);
    private native void nativeSetCurrentLevel(long ptr, int level);
    private native int nativeGetLevelCount(long ptr);
    private native double nativeGetScaleForLevel(long ptr, int level);
}
