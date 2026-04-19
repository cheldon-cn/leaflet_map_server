package cn.cycle.chart.api.parser;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class S57Parser extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public S57Parser() {
        setNativePtr(nativeCreate());
    }

    S57Parser(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private static native void nativeDestroy(long ptr);
    private native boolean nativeOpen(long ptr, String path);
    private native long nativeParse(long ptr);
    private native void nativeSetFeatureFilter(long ptr, Object[] features);
    private native void nativeSetSpatialFilter(long ptr, double minX, double minY, double maxX, double maxY);
    private native long nativeParseFile(long ptr, String path, long configPtr);
}
