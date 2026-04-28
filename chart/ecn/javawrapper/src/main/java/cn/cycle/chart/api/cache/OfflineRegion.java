package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class OfflineRegion extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public OfflineRegion(long storagePtr, double minX, double minY, double maxX, double maxY, int minZoom, int maxZoom) {
        setNativePtr(nativeCreate(storagePtr, minX, minY, maxX, maxY, minZoom, maxZoom));
    }

    OfflineRegion(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(long storagePtr, double minX, double minY, double maxX, double maxY, int minZoom, int maxZoom);
    private static native void nativeDestroy(long ptr);
    private native boolean nativeDownload(long ptr);
    private native float nativeGetProgress(long ptr);
    private native String nativeGetId(long ptr);
    private native String nativeGetName(long ptr);
}
