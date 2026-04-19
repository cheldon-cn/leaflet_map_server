package cn.cycle.chart.api.parser;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class IncrementalParser extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public IncrementalParser() {
        setNativePtr(nativeCreate());
    }

    IncrementalParser(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private static native void nativeDestroy(long ptr);
    private native void nativeStart(long ptr, String path, int batchSize);
    private native void nativePause(long ptr);
    private native void nativeResume(long ptr);
    private native void nativeCancel(long ptr);
    private native long nativeParseNext(long ptr);
    private native boolean nativeHasMore(long ptr);
    private native float nativeGetProgress(long ptr);
    private native boolean nativeHasFileChanged(long ptr, String path);
    private native void nativeMarkProcessed(long ptr, String path);
    private native void nativeClearState(long ptr, String path);
}
