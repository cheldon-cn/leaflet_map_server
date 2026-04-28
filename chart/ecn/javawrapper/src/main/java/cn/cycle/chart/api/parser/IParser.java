package cn.cycle.chart.api.parser;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public abstract class IParser extends NativeObject {

    static {
        JniBridge.initialize();
    }

    IParser() {
    }

    IParser(long nativePtr) {
        setNativePtr(nativePtr);
    }

    private static native long nativeCreate(int format);
    private static native void nativeDestroy(long ptr);
    private native int nativeOpen(long ptr, String path);
    private native void nativeClose(long ptr);
    private native long nativeParse(long ptr);
    private native long nativeParseIncremental(long ptr, int batchSize);
    private native int nativeGetFormat(long ptr);
}
