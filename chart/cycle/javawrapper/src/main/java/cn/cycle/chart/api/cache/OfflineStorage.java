package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class OfflineStorage extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public OfflineStorage(String storagePath) {
        setNativePtr(nativeCreate(storagePath));
    }

    OfflineStorage(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private native long nativeGetRegionById(long ptr, String regionId);
    private native void nativeRemoveRegion(long ptr, long index);
    private native boolean nativeRemoveRegionById(long ptr, String regionId);
    private native boolean nativeStoreChart(long ptr, String chartId, byte[] data);
    private native String nativeGetChartPath(long ptr, String chartId);
    private native boolean nativeRemoveChart(long ptr, String chartId);
    private static native long nativeCreate(String storagePath);
    private native void nativeDestroy(long ptr);
    private native long nativeCreateRegion(long ptr, double minX, double minY, double maxX, double maxY, int minZoom, int maxZoom);
    private native long nativeGetRegionCount(long ptr);
    private native long nativeGetRegion(long ptr, long index);
    private native int nativeStartDownload(long ptr, String regionId);
    private native int nativePauseDownload(long ptr, String regionId);
    private native int nativeResumeDownload(long ptr, String regionId);
    private native int nativeCancelDownload(long ptr, String regionId);
    private native boolean nativeIsDownloading(long ptr, String regionId);
    private native double nativeGetDownloadProgress(long ptr, String regionId);
    private native long nativeGetStorageSize(long ptr);
    private native long nativeGetUsedSize(long ptr);
    private native long nativeGetChartCount(long ptr);
}
