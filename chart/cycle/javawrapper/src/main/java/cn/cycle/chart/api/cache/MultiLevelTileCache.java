package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class MultiLevelTileCache extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public MultiLevelTileCache() {
        setNativePtr(nativeCreate());
    }

    MultiLevelTileCache(long nativePtr) {
        setNativePtr(nativePtr);
    }

    private static native long nativeCreate(long memoryCachePtr, long diskCachePtr);
}
