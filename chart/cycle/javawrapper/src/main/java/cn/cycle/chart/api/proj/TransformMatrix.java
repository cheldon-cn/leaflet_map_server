package cn.cycle.chart.api.proj;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class TransformMatrix extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public TransformMatrix() {
        setNativePtr(nativeCreate());
    }

    TransformMatrix(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private static native long nativeCreateIdentity();
    private static native long nativeCreateTranslation(double tx, double ty);
    private static native long nativeCreateScale(double sx, double sy);
    private static native long nativeCreateRotation(double angle);
    private native void nativeDestroy(long ptr);
    private native long nativeMultiply(long aPtr, long bPtr);
    private native long nativeInverse(long ptr);
    private native void nativeTransformPoint(long ptr, double[] xyArr);
    private native double[] nativeGetElements(long ptr);
    private native void nativeSetElements(long ptr, double[] elementsArr);
    private native void nativeTranslate(long ptr, double dx, double dy);
    private native void nativeScale(long ptr, double sx, double sy);
    private native void nativeRotate(long ptr, double angleDegrees);
    private native void nativeMultiplyInPlace(long resultPtr, long aPtr, long bPtr);
    private native double[] nativeTransformCoord(long ptr, double x, double y);
    private native double[] nativeGetValues(long ptr);
}
