package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class RasterLayer extends NativeObject {

    static {
        JniBridge.initialize();
    }

    RasterLayer(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native long nativeCreate(String name, String filepath);
    private native int nativeGetWidth(long ptr);
    private native int nativeGetHeight(long ptr);
    private native int nativeGetBandCount(long ptr);
    private native double nativeGetNoDataValue(long ptr, int bandIndex);
    private native long nativeGetExtent(long ptr);
}
