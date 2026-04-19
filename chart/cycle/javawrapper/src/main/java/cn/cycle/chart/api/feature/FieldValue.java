package cn.cycle.chart.api.feature;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class FieldValue extends NativeObject {

    static {
        JniBridge.initialize();
    }

    FieldValue(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public int getType() {
        checkNotDisposed();
        return nativeGetType(getNativePtr());
    }

    public boolean isNull() {
        checkNotDisposed();
        return nativeIsNull(getNativePtr());
    }

    public int getAsInteger() {
        checkNotDisposed();
        return nativeGetAsInteger(getNativePtr());
    }

    public long getAsInteger64() {
        checkNotDisposed();
        return nativeGetAsInteger64(getNativePtr());
    }

    public double getAsDouble() {
        checkNotDisposed();
        return nativeGetAsDouble(getNativePtr());
    }

    public String getAsString() {
        checkNotDisposed();
        return nativeGetAsString(getNativePtr());
    }

    public void setAsInteger(int value) {
        checkNotDisposed();
        nativeSetAsInteger(getNativePtr(), value);
    }

    public void setAsInteger64(long value) {
        checkNotDisposed();
        nativeSetAsInteger64(getNativePtr(), value);
    }

    public void setAsDouble(double value) {
        checkNotDisposed();
        nativeSetAsDouble(getNativePtr(), value);
    }

    public void setAsString(String value) {
        checkNotDisposed();
        nativeSetAsString(getNativePtr(), value);
    }

    public void setNull() {
        checkNotDisposed();
        nativeSetNull(getNativePtr());
    }

    @Override
    protected void nativeDispose(long ptr) {
    }

    private native int nativeGetType(long ptr);
    private native boolean nativeIsNull(long ptr);
    private native int nativeGetAsInteger(long ptr);
    private native long nativeGetAsInteger64(long ptr);
    private native double nativeGetAsDouble(long ptr);
    private native String nativeGetAsString(long ptr);
    private native void nativeSetAsInteger(long ptr, int value);
    private native void nativeSetAsInteger64(long ptr, long value);
    private native void nativeSetAsDouble(long ptr, double value);
    private native void nativeSetAsString(long ptr, String value);
    private native void nativeSetNull(long ptr);
}
