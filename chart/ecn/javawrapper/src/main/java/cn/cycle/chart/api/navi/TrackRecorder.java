package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class TrackRecorder extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public TrackRecorder() {
        setNativePtr(nativeCreate());
    }

    TrackRecorder(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native void nativeStart(long ptr, String trackName);
    private native void nativeStop(long ptr);
    private native void nativePause(long ptr);
    private native void nativeResume(long ptr);
    private native boolean nativeIsRecording(long ptr);
    private native long nativeGetCurrentTrack(long ptr);
    private native void nativeSetInterval(long ptr, int seconds);
    private native void nativeSetMinDistance(long ptr, double meters);
}
