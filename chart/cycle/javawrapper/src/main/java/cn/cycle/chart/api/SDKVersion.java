package cn.cycle.chart.api;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class SDKVersion extends NativeObject {

    static {
        JniBridge.initialize();
    }

    SDKVersion(long nativePtr) {
        setNativePtr(nativePtr);
    }

    private static native String nativeGetVersion();
    private static native int nativeGetVersionMajor();
    private static native int nativeGetVersionMinor();
    private static native int nativeGetVersionPatch();
    private static native String nativeGetBuildDate();
}
