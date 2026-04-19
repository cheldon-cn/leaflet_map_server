package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class LayerManager extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public LayerManager() {
        setNativePtr(nativeCreate());
    }

    LayerManager(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public long getLayerCount() {
        checkNotDisposed();
        return nativeGetLayerCount(getNativePtr());
    }

    public Layer getLayer(long index) {
        checkNotDisposed();
        long layerPtr = nativeGetLayer(getNativePtr(), index);
        if (layerPtr == 0) {
            return null;
        }
        int type = nativeGetLayerType(getNativePtr(), index);
        if (type == Layer.LayerType.VECTOR.getValue()) {
            return new VectorLayer(layerPtr);
        }
        return new Layer(layerPtr);
    }

    public Layer getLayerByName(String name) {
        checkNotDisposed();
        if (name == null) {
            throw new IllegalArgumentException("name must not be null");
        }
        long count = getLayerCount();
        for (long i = 0; i < count; i++) {
            Layer layer = getLayer(i);
            if (layer != null && name.equals(layer.getName())) {
                return layer;
            }
        }
        return null;
    }

    public void addLayer(Layer layer) {
        checkNotDisposed();
        if (layer == null) {
            throw new IllegalArgumentException("layer must not be null");
        }
        nativeAddLayer(getNativePtr(), layer.getNativePtr());
    }

    public void removeLayer(long index) {
        checkNotDisposed();
        nativeRemoveLayer(getNativePtr(), index);
    }

    public void moveLayer(long fromIndex, long toIndex) {
        checkNotDisposed();
        nativeMoveLayer(getNativePtr(), fromIndex, toIndex);
    }

    public void clearLayers() {
        checkNotDisposed();
        long count = getLayerCount();
        for (long i = count - 1; i >= 0; i--) {
            removeLayer(i);
        }
    }

    public boolean getLayerVisible(long index) {
        checkNotDisposed();
        return nativeGetLayerVisible(getNativePtr(), index);
    }

    public void setLayerVisible(long index, boolean visible) {
        checkNotDisposed();
        nativeSetLayerVisible(getNativePtr(), index, visible);
    }

    public double getLayerOpacity(long index) {
        checkNotDisposed();
        return nativeGetLayerOpacity(getNativePtr(), index);
    }

    public void setLayerOpacity(long index, double opacity) {
        checkNotDisposed();
        nativeSetLayerOpacity(getNativePtr(), index, opacity);
    }

    public int getLayerZOrder(long index) {
        checkNotDisposed();
        return nativeGetLayerZOrder(getNativePtr(), index);
    }

    public void setLayerZOrder(long index, int zOrder) {
        checkNotDisposed();
        nativeSetLayerZOrder(getNativePtr(), index, zOrder);
    }

    public void sortByZOrder() {
        checkNotDisposed();
        nativeSortByZOrder(getNativePtr());
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native long nativeGetLayerCount(long ptr);
    private native long nativeGetLayer(long ptr, long index);
    private native int nativeGetLayerType(long ptr, long index);
    private native void nativeAddLayer(long ptr, long layerPtr);
    private native void nativeRemoveLayer(long ptr, long index);
    private native void nativeMoveLayer(long ptr, long fromIndex, long toIndex);
    private native boolean nativeGetLayerVisible(long ptr, long index);
    private native void nativeSetLayerVisible(long ptr, long index, boolean visible);
    private native double nativeGetLayerOpacity(long ptr, long index);
    private native void nativeSetLayerOpacity(long ptr, long index, double opacity);
    private native long nativeGetLayerByName(long ptr, String name);
    private native int nativeGetLayerZOrder(long ptr, long index);
    private native void nativeSetLayerZOrder(long ptr, long index, int zOrder);
    private native void nativeSortByZOrder(long ptr);
}
