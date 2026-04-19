package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class Route extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public Route() {
        setNativePtr(nativeCreate());
    }

    Route(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public String getId() {
        checkNotDisposed();
        return nativeGetId(getNativePtr());
    }

    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }

    public void setName(String name) {
        checkNotDisposed();
        nativeSetName(getNativePtr(), name);
    }

    public int getStatus() {
        checkNotDisposed();
        return nativeGetStatus(getNativePtr());
    }

    public double getTotalDistance() {
        checkNotDisposed();
        return nativeGetTotalDistance(getNativePtr());
    }

    public int getWaypointCount() {
        checkNotDisposed();
        return nativeGetWaypointCount(getNativePtr());
    }

    public Waypoint getWaypoint(int index) {
        checkNotDisposed();
        long ptr = nativeGetWaypoint(getNativePtr(), index);
        return ptr != 0 ? new Waypoint(ptr) : null;
    }

    public void addWaypoint(long wpPtr) {
        checkNotDisposed();
        nativeAddWaypoint(getNativePtr(), wpPtr);
    }

    public void removeWaypoint(int index) {
        checkNotDisposed();
        nativeRemoveWaypoint(getNativePtr(), index);
    }

    public Waypoint getCurrentWaypoint() {
        checkNotDisposed();
        long ptr = nativeGetCurrentWaypoint(getNativePtr());
        return ptr != 0 ? new Waypoint(ptr) : null;
    }

    public int advanceToNextWaypoint() {
        checkNotDisposed();
        return nativeAdvanceToNextWaypoint(getNativePtr());
    }

    public void insertWaypoint(int index, long wpPtr) {
        checkNotDisposed();
        nativeInsertWaypoint(getNativePtr(), index, wpPtr);
    }

    public void clear() {
        checkNotDisposed();
        nativeClear(getNativePtr());
    }

    public void reverse() {
        checkNotDisposed();
        nativeReverse(getNativePtr());
    }

    public long getEta() {
        checkNotDisposed();
        return nativeGetEta(getNativePtr());
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private static native void nativeDestroy(long ptr);
    private native String nativeGetId(long ptr);
    private native String nativeGetName(long ptr);
    private native void nativeSetName(long ptr, String name);
    private native int nativeGetStatus(long ptr);
    private native double nativeGetTotalDistance(long ptr);
    private native int nativeGetWaypointCount(long ptr);
    private native long nativeGetWaypoint(long ptr, int index);
    private native void nativeAddWaypoint(long ptr, long wpPtr);
    private native void nativeRemoveWaypoint(long ptr, int index);
    private native long nativeGetCurrentWaypoint(long ptr);
    private native int nativeAdvanceToNextWaypoint(long ptr);
    private native void nativeInsertWaypoint(long ptr, int index, long wpPtr);
    private native void nativeClear(long ptr);
    private native void nativeReverse(long ptr);
    private native long nativeGetEta(long ptr);
}
