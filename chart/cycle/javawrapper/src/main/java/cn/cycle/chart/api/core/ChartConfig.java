package cn.cycle.chart.api.core;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class ChartConfig extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public ChartConfig() {
        setNativePtr(nativeCreate());
    }

    ChartConfig(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public int getDisplayMode() {
        checkNotDisposed();
        return nativeGetDisplayMode(getNativePtr());
    }

    public void setDisplayMode(int mode) {
        checkNotDisposed();
        nativeSetDisplayMode(getNativePtr(), mode);
    }

    public boolean getShowGrid() {
        checkNotDisposed();
        return nativeGetShowGrid(getNativePtr());
    }

    public void setShowGrid(boolean show) {
        checkNotDisposed();
        nativeSetShowGrid(getNativePtr(), show);
    }

    public double getDpi() {
        checkNotDisposed();
        return nativeGetDpi(getNativePtr());
    }

    public void setDpi(double dpi) {
        checkNotDisposed();
        nativeSetDpi(getNativePtr(), dpi);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native int nativeGetDisplayMode(long ptr);
    private native void nativeSetDisplayMode(long ptr, int mode);
    private native boolean nativeGetShowGrid(long ptr);
    private native void nativeSetShowGrid(long ptr, boolean show);
    private native double nativeGetDpi(long ptr);
    private native void nativeSetDpi(long ptr, double dpi);
}
