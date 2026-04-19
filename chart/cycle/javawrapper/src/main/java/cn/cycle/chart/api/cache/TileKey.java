package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class TileKey extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public TileKey(int x, int y, int z) {
        setNativePtr(nativeCreate(x, y, z));
    }

    TileKey(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public int getX() {
        checkNotDisposed();
        return nativeGetX(getNativePtr());
    }

    public int getY() {
        checkNotDisposed();
        return nativeGetY(getNativePtr());
    }

    public int getZ() {
        checkNotDisposed();
        return nativeGetZ(getNativePtr());
    }

    @Override
    public boolean equals(Object obj) {
        if (!(obj instanceof TileKey)) return false;
        TileKey other = (TileKey) obj;
        return nativeEquals(getNativePtr(), other.getNativePtr());
    }

    @Override
    public int hashCode() {
        return nativeHashCode(getNativePtr());
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate(int x, int y, int z);
    private static native void nativeDestroy(long ptr);
    private native int nativeGetX(long ptr);
    private native int nativeGetY(long ptr);
    private native int nativeGetZ(long ptr);
    private native boolean nativeEquals(long ptr, long otherPtr);
    private native int nativeHashCode(long ptr);
}
