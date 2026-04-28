package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class AisManager extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public AisManager() {
        setNativePtr(nativeCreate());
    }

    AisManager(long nativePtr) {
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

    public AisTarget getTarget(int mmsi) {
        checkNotDisposed();
        long ptr = nativeGetTarget(getNativePtr(), mmsi);
        return ptr != 0 ? new AisTarget(ptr) : null;
    }

    public int[] getAllTargetMmsi() {
        checkNotDisposed();
        return nativeGetAllTargetMmsi(getNativePtr());
    }

    public int getTargetCount() {
        checkNotDisposed();
        return nativeGetTargetCount(getNativePtr());
    }

    public int addTarget(long targetPtr) {
        checkNotDisposed();
        return nativeAddTarget(getNativePtr(), targetPtr);
    }

    public int removeTarget(int mmsi) {
        checkNotDisposed();
        return nativeRemoveTarget(getNativePtr(), mmsi);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private static native void nativeDestroy(long ptr);
    private native int nativeInitialize(long ptr);
    private native void nativeShutdown(long ptr);
    private native long nativeGetTarget(long ptr, int mmsi);
    private native int[] nativeGetAllTargetMmsi(long ptr);
    private native int nativeGetTargetCount(long ptr);
    private native int nativeAddTarget(long ptr, long targetPtr);
    private native int nativeRemoveTarget(long ptr, int mmsi);
    native void nativeSetCallback(long ptr, long callbackPtr);
    static native void nativeFreeTargets(long ptr);
}
