package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class DataSource extends NativeObject {

    static {
        JniBridge.initialize();
    }

    DataSource(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native long nativeOpen(String path);
    private native void nativeClose(long ptr);
    private native boolean nativeIsOpen(long ptr);
    private native String nativeGetPath(long ptr);
    private native long nativeGetLayerCount(long ptr);
    private native long nativeGetLayer(long ptr, long index);
    private native long nativeGetLayerByName(long ptr, String name);
    private native long nativeCreateLayer(long ptr, String name, int geomType);
    private native int nativeDeleteLayer(long ptr, String name);
}
