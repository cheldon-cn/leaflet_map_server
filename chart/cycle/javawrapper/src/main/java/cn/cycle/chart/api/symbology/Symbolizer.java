package cn.cycle.chart.api.symbology;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class Symbolizer extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public Symbolizer(int type) {
        setNativePtr(nativeCreate(type));
    }

    Symbolizer(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public boolean symbolize(long featurePtr, long devicePtr) {
        checkNotDisposed();
        return nativeSymbolize(getNativePtr(), featurePtr, devicePtr);
    }

    public int getType() {
        checkNotDisposed();
        return nativeGetType(getNativePtr());
    }

    public long getStylePtr() {
        checkNotDisposed();
        return nativeGetStyle(getNativePtr());
    }

    public void setStylePtr(long stylePtr) {
        checkNotDisposed();
        nativeSetStyle(getNativePtr(), stylePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private native boolean nativeSymbolize(long ptr, long featurePtr, long devicePtr);
    private static native long nativeCreate(int type);
    private native void nativeDestroy(long ptr);
    private native int nativeGetType(long ptr);
    private native long nativeGetStyle(long ptr);
    private native void nativeSetStyle(long ptr, long stylePtr);
}
