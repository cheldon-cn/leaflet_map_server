package cn.cycle.chart.api.plugin;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class ChartPlugin extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public ChartPlugin(String name, String version) {
        setNativePtr(nativeCreate(name, version));
    }

    ChartPlugin(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }

    public String getVersion() {
        checkNotDisposed();
        return nativeGetVersion(getNativePtr());
    }

    public int initialize() {
        checkNotDisposed();
        return nativeInitialize(getNativePtr());
    }

    public void shutdown() {
        checkNotDisposed();
        nativeShutdown(getNativePtr());
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(String name, String version);
    private native void nativeDestroy(long ptr);
    private native String nativeGetName(long ptr);
    private native String nativeGetVersion(long ptr);
    private native int nativeInitialize(long ptr);
    private native void nativeShutdown(long ptr);
}
