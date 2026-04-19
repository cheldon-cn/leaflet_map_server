package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class MemoryTileCache extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public MemoryTileCache() {
        setNativePtr(nativeCreate());
    }

    MemoryTileCache(long nativePtr) {
        setNativePtr(nativePtr);
    }

    private static native long nativeCreate(long maxSizeBytes);
    private native long nativeGetMemoryUsage(long ptr);
}
