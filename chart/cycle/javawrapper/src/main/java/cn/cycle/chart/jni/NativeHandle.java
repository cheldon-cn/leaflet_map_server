package cn.cycle.chart.jni;

public final class NativeHandle {

    private NativeHandle() {
    }

    public static long toNativePtr(Object obj) {
        if (obj instanceof NativeObject) {
            return ((NativeObject) obj).getNativePtr();
        }
        throw new IllegalArgumentException("Object is not a NativeObject");
    }

    public static void checkValid(long ptr) {
        if (ptr == 0) {
            throw new IllegalStateException("Native handle is null");
        }
    }
}
