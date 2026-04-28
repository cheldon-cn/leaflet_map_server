package cn.cycle.chart.api.feature;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class FieldDefn extends NativeObject {

    public static final int TYPE_INTEGER = 0;
    public static final int TYPE_REAL = 1;
    public static final int TYPE_STRING = 2;

    static {
        JniBridge.initialize();
    }

    public FieldDefn(String name, int type) {
        if (name == null) {
            throw new IllegalArgumentException("name must not be null");
        }
        setNativePtr(nativeCreate(name, type));
    }

    public FieldDefn(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }

    public int getType() {
        checkNotDisposed();
        return nativeGetType(getNativePtr());
    }

    public int getWidth() {
        checkNotDisposed();
        return nativeGetWidth(getNativePtr());
    }

    public void setWidth(int width) {
        checkNotDisposed();
        nativeSetWidth(getNativePtr(), width);
    }

    public int getPrecision() {
        checkNotDisposed();
        return nativeGetPrecision(getNativePtr());
    }

    public void setPrecision(int precision) {
        checkNotDisposed();
        nativeSetPrecision(getNativePtr(), precision);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private native long nativeCreate(String name, int type);
    private native void nativeDestroy(long ptr);
    private native String nativeGetName(long ptr);
    private native int nativeGetType(long ptr);
    private native int nativeGetWidth(long ptr);
    private native void nativeSetWidth(long ptr, int width);
    private native int nativeGetPrecision(long ptr);
    private native void nativeSetPrecision(long ptr, int precision);
}
