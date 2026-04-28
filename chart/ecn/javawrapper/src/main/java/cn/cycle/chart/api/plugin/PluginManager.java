package cn.cycle.chart.api.plugin;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class PluginManager extends NativeObject {

    static {
        JniBridge.initialize();
    }

    private PluginManager() {
        setNativePtr(nativeGetInstance());
    }

    PluginManager(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public static PluginManager getInstance() {
        long ptr = nativeGetInstance();
        return ptr != 0 ? new PluginManager(ptr) : null;
    }

    public int loadPlugin(String path) {
        checkNotDisposed();
        return nativeLoadPlugin(getNativePtr(), path);
    }

    public int unloadPlugin(String name) {
        checkNotDisposed();
        return nativeUnloadPlugin(getNativePtr(), name);
    }

    public int getPluginCount() {
        checkNotDisposed();
        return nativeGetPluginCount(getNativePtr());
    }

    public String getPluginName(int index) {
        checkNotDisposed();
        return nativeGetPluginName(getNativePtr(), index);
    }

    public ChartPlugin getPlugin(String name) {
        checkNotDisposed();
        long ptr = nativeGetPlugin(getNativePtr(), name);
        return ptr != 0 ? new ChartPlugin(ptr) : null;
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private static native long nativeGetInstance();
    private native int nativeLoadPlugin(long ptr, String path);
    private native int nativeUnloadPlugin(long ptr, String name);
    private native int nativeGetPluginCount(long ptr);
    private native String nativeGetPluginName(long ptr, int index);
    private native long nativeGetPlugin(long ptr, String name);
}
