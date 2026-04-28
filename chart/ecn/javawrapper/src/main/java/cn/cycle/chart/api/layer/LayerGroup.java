package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class LayerGroup extends NativeObject {

    static {
        JniBridge.initialize();
    }

    LayerGroup(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(String name);
    private static native void nativeDestroy(long ptr);
    private native String nativeGetName(long ptr);
    private native long nativeGetLayerCount(long ptr);
    private native long nativeGetLayer(long ptr, long index);
    private native void nativeAddLayer(long ptr, long layerPtr);
    private native void nativeRemoveLayer(long ptr, long index);
    private native void nativeMoveLayer(long ptr, long from, long to);
    private native boolean nativeIsVisible(long ptr);
    private native void nativeSetVisible(long ptr, boolean visible);
    private native double nativeGetOpacity(long ptr);
    private native void nativeSetOpacity(long ptr, double opacity);
    private native int nativeGetZOrder(long ptr);
    private native void nativeSetZOrder(long ptr, int zOrder);
}
