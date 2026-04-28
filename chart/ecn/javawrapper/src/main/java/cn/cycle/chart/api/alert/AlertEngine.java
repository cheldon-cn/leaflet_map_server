package cn.cycle.chart.api.alert;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class AlertEngine extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public AlertEngine() {
        setNativePtr(nativeCreate());
    }

    AlertEngine(long nativePtr) {
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

    public void checkAll() {
        checkNotDisposed();
        nativeCheckAll(getNativePtr());
    }

    public long[] getActiveAlertIds() {
        checkNotDisposed();
        return nativeGetActiveAlertIds(getNativePtr());
    }

    public void acknowledgeAlert(long alertId) {
        checkNotDisposed();
        nativeAcknowledgeAlert(getNativePtr(), alertId);
    }

    public int addAlert(long alertPtr) {
        checkNotDisposed();
        return nativeAddAlert(getNativePtr(), alertPtr);
    }

    public int removeAlert(long alertId) {
        checkNotDisposed();
        return nativeRemoveAlert(getNativePtr(), alertId);
    }

    public Alert getAlert(long alertId) {
        checkNotDisposed();
        long ptr = nativeGetAlert(getNativePtr(), alertId);
        return ptr != 0 ? new Alert(ptr) : null;
    }

    public int getAlertCount() {
        checkNotDisposed();
        return nativeGetAlertCount(getNativePtr());
    }

    public void clearAlerts() {
        checkNotDisposed();
        nativeClearAlerts(getNativePtr());
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private static native void nativeDestroy(long ptr);
    private native int nativeInitialize(long ptr);
    private native void nativeShutdown(long ptr);
    private native void nativeCheckAll(long ptr);
    private native long[] nativeGetActiveAlertIds(long ptr);
    private native void nativeAcknowledgeAlert(long ptr, long alertId);
    private native int nativeAddAlert(long ptr, long alertPtr);
    private native int nativeRemoveAlert(long ptr, long alertId);
    private native long nativeGetAlert(long ptr, long alertId);
    private native int nativeGetAlertCount(long ptr);
    private native void nativeClearAlerts(long ptr);
    native void nativeSetCallback(long ptr, long callbackPtr);
    static native void nativeFreeAlerts(long ptr);
}
