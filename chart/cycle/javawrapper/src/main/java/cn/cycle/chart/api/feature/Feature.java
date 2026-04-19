package cn.cycle.chart.api.feature;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class Feature extends NativeObject {

    static {
        JniBridge.initialize();
    }

    Feature(long nativePtr) {
        setNativePtr(nativePtr);
    }

    private static native long nativeCreateWithDefn(long defnPtr);
    private native long nativeGetDefn(long ptr);
    private native boolean nativeIsFieldSet(long ptr, long index);
    private native boolean nativeIsFieldNull(long ptr, long index);
    private native int nativeGetFieldAsIntegerByName(long ptr, String name);
    private native double nativeGetFieldAsRealByName(long ptr, String name);
    private native String nativeGetFieldAsStringByName(long ptr, String name);
    private native boolean nativeSetFieldIntegerByName(long ptr, String name, int value);
    private native boolean nativeSetFieldRealByName(long ptr, String name, double value);
    private native boolean nativeSetFieldStringByName(long ptr, String name, String value);
    private native long nativeStealGeometry(long ptr);
    private native long nativeGetEnvelope(long ptr);
    private native long nativeClone(long ptr);
}
