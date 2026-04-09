package cn.cycle.chart.api.feature;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Geometry;

public final class FeatureInfo extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public FeatureInfo(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public long getFid() {
        checkNotDisposed();
        return nativeGetFid(getNativePtr());
    }

    public void setFid(long fid) {
        checkNotDisposed();
        nativeSetFid(getNativePtr(), fid);
    }

    public long getFieldCount() {
        checkNotDisposed();
        return nativeGetFieldCount(getNativePtr());
    }

    public boolean isFieldSet(long index) {
        checkNotDisposed();
        return nativeIsFieldSet(getNativePtr(), index);
    }

    public boolean isFieldNull(long index) {
        checkNotDisposed();
        return nativeIsFieldNull(getNativePtr(), index);
    }

    public int getFieldAsInteger(long index) {
        checkNotDisposed();
        return nativeGetFieldAsInteger(getNativePtr(), index);
    }

    public double getFieldAsReal(long index) {
        checkNotDisposed();
        return nativeGetFieldAsReal(getNativePtr(), index);
    }

    public String getFieldAsString(long index) {
        checkNotDisposed();
        return nativeGetFieldAsString(getNativePtr(), index);
    }

    public void setFieldInteger(long index, int value) {
        checkNotDisposed();
        nativeSetFieldInteger(getNativePtr(), index, value);
    }

    public void setFieldReal(long index, double value) {
        checkNotDisposed();
        nativeSetFieldReal(getNativePtr(), index, value);
    }

    public void setFieldString(long index, String value) {
        checkNotDisposed();
        nativeSetFieldString(getNativePtr(), index, value);
    }

    public void setFieldNull(long index) {
        checkNotDisposed();
        nativeSetFieldNull(getNativePtr(), index);
    }

    public long getGeometryPtr() {
        checkNotDisposed();
        return nativeGetGeometry(getNativePtr());
    }

    public void setGeometryPtr(long geomPtr) {
        checkNotDisposed();
        nativeSetGeometry(getNativePtr(), geomPtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private native long nativeGetFid(long ptr);
    private native void nativeSetFid(long ptr, long fid);
    private native long nativeGetFieldCount(long ptr);
    private native boolean nativeIsFieldSet(long ptr, long index);
    private native boolean nativeIsFieldNull(long ptr, long index);
    private native int nativeGetFieldAsInteger(long ptr, long index);
    private native double nativeGetFieldAsReal(long ptr, long index);
    private native String nativeGetFieldAsString(long ptr, long index);
    private native void nativeSetFieldInteger(long ptr, long index, int value);
    private native void nativeSetFieldReal(long ptr, long index, double value);
    private native void nativeSetFieldString(long ptr, long index, String value);
    private native void nativeSetFieldNull(long ptr, long index);
    private native long nativeGetGeometry(long ptr);
    private native void nativeSetGeometry(long ptr, long geomPtr);
    private native void nativeDestroy(long ptr);
}
