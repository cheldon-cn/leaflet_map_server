package cn.cycle.chart.api.graph;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class HitTest extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public HitTest() {
        setNativePtr(nativeCreate());
    }

    HitTest(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private static native void nativeDestroy(long ptr);
    private native long nativeQueryPoint(long ptr, long layerPtr, double x, double y, double tolerance);
    private native long[] nativeQueryRect(long ptr, long layerPtr, double minX, double minY, double maxX, double maxY);
    private static native void nativeFreeFeatures(long featuresPtr);
}
