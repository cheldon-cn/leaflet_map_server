package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public class Layer extends NativeObject {

    public enum LayerType {
        UNKNOWN(0),
        VECTOR(1),
        RASTER(2);

        private final int value;

        LayerType(int value) {
            this.value = value;
        }

        public int getValue() {
            return value;
        }

        public static LayerType fromValue(int value) {
            for (LayerType type : values()) {
                if (type.value == value) {
                    return type;
                }
            }
            return UNKNOWN;
        }
    }

    static {
        JniBridge.initialize();
    }

    Layer() {
    }

    Layer(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public String getName() {
        checkNotDisposed();
        return nativeGetName(getNativePtr());
    }

    public LayerType getType() {
        checkNotDisposed();
        return LayerType.fromValue(nativeGetType(getNativePtr()));
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private native String nativeGetName(long ptr);
    private native int nativeGetType(long ptr);
    private native void nativeDestroy(long ptr);
}
