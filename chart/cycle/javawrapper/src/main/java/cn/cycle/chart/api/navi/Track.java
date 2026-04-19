package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class Track extends NativeObject {

    static {
        JniBridge.initialize();
    }

    Track(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native String nativeGetId(long ptr);
    private native String nativeGetName(long ptr);
    private native void nativeSetName(long ptr, String name);
    private native long nativeGetPointCount(long ptr);
    private native long nativeGetPointPtr(long ptr, long index);
    private native void nativeAddPoint(long ptr, double lat, double lon, double speed, double course, double timestamp);
    private native void nativeClear(long ptr);
    private native long nativeToLineString(long ptr);
}
