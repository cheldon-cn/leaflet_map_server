package cn.cycle.chart.api.cache;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class TileCache extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public TileCache() {
        setNativePtr(nativeCreate());
    }

    TileCache(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public boolean hasTile(int x, int y, int z) {
        checkNotDisposed();
        return nativeHasTile(getNativePtr(), x, y, z);
    }

    public long getTilePtr(int x, int y, int z) {
        checkNotDisposed();
        return nativeGetTilePtr(getNativePtr(), x, y, z);
    }

    public void putTile(int x, int y, int z, long tilePtr) {
        checkNotDisposed();
        nativePutTile(getNativePtr(), x, y, z, tilePtr);
    }

    public void removeTile(int x, int y, int z) {
        checkNotDisposed();
        nativeRemoveTile(getNativePtr(), x, y, z);
    }

    public void clear() {
        checkNotDisposed();
        nativeClear(getNativePtr());
    }

    public long getSize() {
        checkNotDisposed();
        return nativeGetSize(getNativePtr());
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private static native void nativeDestroy(long ptr);
    private native boolean nativeHasTile(long ptr, int x, int y, int z);
    private native long nativeGetTilePtr(long ptr, int x, int y, int z);
    private native void nativePutTile(long ptr, int x, int y, int z, long tilePtr);
    private native void nativeRemoveTile(long ptr, int x, int y, int z);
    private native void nativeClear(long ptr);
    private native long nativeGetSize(long ptr);
}
