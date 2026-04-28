package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class NavigationEngine extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public NavigationEngine() {
        setNativePtr(nativeCreate());
    }

    NavigationEngine(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public int initialize() {
        checkNotDisposed();
        return nativeInitialize(getNativePtr());
    }

    public void shutdown() {
        checkNotDisposed();
        nativeShutdown(getNativePtr());
    }

    public void setRoute(long routePtr) {
        checkNotDisposed();
        nativeSetRoute(getNativePtr(), routePtr);
    }

    public long getRoute() {
        checkNotDisposed();
        return nativeGetRoute(getNativePtr());
    }

    public void start() {
        checkNotDisposed();
        nativeStart(getNativePtr());
    }

    public void stop() {
        checkNotDisposed();
        nativeStop(getNativePtr());
    }

    public void pause() {
        checkNotDisposed();
        nativePause(getNativePtr());
    }

    public void resume() {
        checkNotDisposed();
        nativeResume(getNativePtr());
    }

    public int getStatus() {
        checkNotDisposed();
        return nativeGetStatus(getNativePtr());
    }

    public Waypoint getCurrentWaypoint() {
        checkNotDisposed();
        long ptr = nativeGetCurrentWaypoint(getNativePtr());
        return ptr != 0 ? new Waypoint(ptr) : null;
    }

    public double getDistanceToWaypoint() {
        checkNotDisposed();
        return nativeGetDistanceToWaypoint(getNativePtr());
    }

    public double getBearingToWaypoint() {
        checkNotDisposed();
        return nativeGetBearingToWaypoint(getNativePtr());
    }

    public double getCrossTrackError() {
        checkNotDisposed();
        return nativeGetCrossTrackError(getNativePtr());
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native int nativeInitialize(long ptr);
    private native void nativeShutdown(long ptr);
    private native void nativeSetRoute(long ptr, long routePtr);
    private native long nativeGetRoute(long ptr);
    private native void nativeStart(long ptr);
    private native void nativeStop(long ptr);
    private native void nativePause(long ptr);
    private native void nativeResume(long ptr);
    private native int nativeGetStatus(long ptr);
    private native long nativeGetCurrentWaypoint(long ptr);
    private native double nativeGetDistanceToWaypoint(long ptr);
    private native double nativeGetBearingToWaypoint(long ptr);
    private native double nativeGetCrossTrackError(long ptr);
}
