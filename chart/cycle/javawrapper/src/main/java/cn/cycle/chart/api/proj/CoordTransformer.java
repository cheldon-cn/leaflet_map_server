package cn.cycle.chart.api.proj;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class CoordTransformer extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public CoordTransformer() {
        setNativePtr(nativeCreate());
    }

    CoordTransformer(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(String sourceCrs, String targetCrs);
    private static native void nativeDestroy(long ptr);
    private native boolean nativeIsValid(long ptr);
    private native Object nativeTransform(long ptr, double x, double y);
    private native Object nativeTransformInverse(long ptr, double x, double y);
    private native void nativeTransformArray(long ptr, double[] xArr, double[] yArr);
    private native Object nativeTransformEnvelope(long ptr, double minX, double minY, double maxX, double maxY);
    private native long nativeTransformGeometry(long ptr, long geomPtr);
    private native String nativeGetSourceCrs(long ptr);
    private native String nativeGetTargetCrs(long ptr);
}
