package cn.cycle.chart.api.navi;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class RouteManager extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public RouteManager() {
        setNativePtr(nativeCreate());
    }

    RouteManager(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private static native void nativeDestroy(long ptr);
    private native int nativeGetRouteCount(long ptr);
    private native long nativeGetRoute(long ptr, int index);
    private native long nativeGetRouteById(long ptr, String id);
    private native void nativeAddRoute(long ptr, long routePtr);
    private native void nativeRemoveRoute(long ptr, String id);
    private native long nativeGetActiveRoute(long ptr);
    private native void nativeSetActiveRoute(long ptr, String id);
}
