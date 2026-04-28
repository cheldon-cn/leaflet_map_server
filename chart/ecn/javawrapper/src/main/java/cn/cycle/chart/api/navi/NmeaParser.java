package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class NmeaParser extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public NmeaParser() {
        setNativePtr(nativeCreate());
    }

    NmeaParser(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native int nativeParse(long ptr, String sentence);
    private native String nativeGetSentenceType(long ptr);
}
