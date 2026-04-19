package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class DiskTileCache extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public DiskTileCache(String cacheDir, long maxSizeBytes) {
        setNativePtr(nativeCreate(cacheDir, maxSizeBytes));
    }

    DiskTileCache(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native long nativeCreate(String cacheDir, long maxSizeBytes);
}
