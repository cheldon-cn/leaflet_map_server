package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class MemoryTileCache extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public static MemoryTileCache create(long maxSizeBytes) {
        MemoryTileCache cache = new MemoryTileCache();
        cache.setNativePtr(nativeCreate(maxSizeBytes));
        return cache;
    }

    MemoryTileCache(long nativePtr) {
        setNativePtr(nativePtr);
    }

    private MemoryTileCache() {
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native long nativeCreate(long maxSizeBytes);
    private native long nativeGetMemoryUsage(long ptr);
}
