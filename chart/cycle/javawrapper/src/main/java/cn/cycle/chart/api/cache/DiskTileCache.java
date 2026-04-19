package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class DiskTileCache extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public DiskTileCache() {
        setNativePtr(nativeCreate());
    }

    DiskTileCache(long nativePtr) {
        setNativePtr(nativePtr);
    }

    private static native long nativeCreate(String cacheDir, long maxSizeBytes);
}
