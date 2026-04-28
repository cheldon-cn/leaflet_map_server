package cn.cycle.chart.api.draw;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class Color extends NativeObject {

    static {
        JniBridge.initialize();
    }


    Color(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native int nativeFromRgba(byte r, byte g, byte b, byte a);
    private static native int nativeFromRgb(byte r, byte g, byte b);
    private static native int nativeFromHex(int hex);
    private static native int nativeToHex(int packedColor);
    private static native int[] nativeToRgba(long ptr);
}
