package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class MultiLevelTileCache extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public MultiLevelTileCache(long memoryCachePtr, long diskCachePtr) {
        setNativePtr(nativeCreate(memoryCachePtr, diskCachePtr));
    }

    MultiLevelTileCache(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native long nativeCreate(long memoryCachePtr, long diskCachePtr);
}
