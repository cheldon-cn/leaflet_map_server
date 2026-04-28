package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class CacheManager extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public CacheManager() {
        setNativePtr(nativeCreate());
    }

    CacheManager(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native long nativeGetCacheCount(long ptr);
    private native long nativeGetCache(long ptr, String name);
    private native void nativeAddCache(long ptr, String name, long cachePtr);
    private native void nativeRemoveCache(long ptr, String name);
    private native void nativeClearAll(long ptr);
    private native long nativeGetTotalSize(long ptr);
    private native long nativeGetTotalTileCount(long ptr);
    private native void nativeFlushAll(long ptr);
}
