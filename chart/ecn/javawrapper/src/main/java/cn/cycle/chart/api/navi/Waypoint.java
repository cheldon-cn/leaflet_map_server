package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class Waypoint extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public Waypoint(double lat, double lon) {
        setNativePtr(nativeCreate(lat, lon));
    }

    Waypoint(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public double getLatitude() {
        checkNotDisposed();
        return nativeGetLatitude(getNativePtr());
    }

    public double getLongitude() {
        checkNotDisposed();
        return nativeGetLongitude(getNativePtr());
    }

    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }

    public void setName(String name) {
        checkNotDisposed();
        nativeSetName(getNativePtr(), name);
    }

    public boolean isArrival() {
        checkNotDisposed();
        return nativeIsArrival(getNativePtr());
    }

    public void setArrivalRadius(double radius) {
        checkNotDisposed();
        nativeSetArrivalRadius(getNativePtr(), radius);
    }

    public double getArrivalRadius() {
        checkNotDisposed();
        return nativeGetArrivalRadius(getNativePtr());
    }

    public void setTurnRadius(double radius) {
        checkNotDisposed();
        nativeSetTurnRadius(getNativePtr(), radius);
    }

    public double getTurnRadius() {
        checkNotDisposed();
        return nativeGetTurnRadius(getNativePtr());
    }

    public void setType(int type) {
        checkNotDisposed();
        nativeSetType(getNativePtr(), type);
    }

    public int getType() {
        checkNotDisposed();
        return nativeGetType(getNativePtr());
    }

    public void setDescription(String description) {
        checkNotDisposed();
        nativeSetDescription(getNativePtr(), description);
    }

    public String getDescription() {
        checkNotDisposed();
        return nativeGetDescription(getNativePtr());
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(double lat, double lon);
    private static native void nativeDestroy(long ptr);
    private native double nativeGetLatitude(long ptr);
    private native double nativeGetLongitude(long ptr);
    private native String nativeGetName(long ptr);
    private native void nativeSetName(long ptr, String name);
    private native boolean nativeIsArrival(long ptr);
    private native void nativeSetArrivalRadius(long ptr, double radius);
    private native double nativeGetArrivalRadius(long ptr);
    private native void nativeSetTurnRadius(long ptr, double radius);
    private native double nativeGetTurnRadius(long ptr);
    private native void nativeSetType(long ptr, int type);
    private native int nativeGetType(long ptr);
    private native void nativeSetDescription(long ptr, String description);
    private native String nativeGetDescription(long ptr);
}
