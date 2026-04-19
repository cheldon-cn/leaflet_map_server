package cn.cycle.chart.api.layer;

import cn.cycle.chart.api.style.DrawStyle;
import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public class Layer extends NativeObject {

    public enum LayerType {
        VECTOR(0),
        RASTER(1),
        UNKNOWN(2);

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

    private LayerManager layerManager;
    private int layerIndex;
    private String name;
    private boolean visible = true;
    private boolean selectable = true;
    private double opacity = 1.0;
    private int zOrder = 0;
    private DrawStyle style;

    Layer() {
    }

    Layer(long nativePtr) {
        setNativePtr(nativePtr);
    }

    void setLayerManager(LayerManager manager, int index) {
        this.layerManager = manager;
        this.layerIndex = index;
    }

    public String getName() {
        checkNotDisposed();
        if (name != null) {
            return name;
        }
        name = nativeGetName(getNativePtr());
        return name;
    }

    public void setName(String name) {
        checkNotDisposed();
        this.name = name;
        nativeSetName(getNativePtr(), name);
    }

    public LayerType getType() {
        checkNotDisposed();
        return LayerType.fromValue(nativeGetType(getNativePtr()));
    }

    public boolean isVisible() {
        checkNotDisposed();
        return visible;
    }

    public void setVisible(boolean visible) {
        checkNotDisposed();
        this.visible = visible;
        nativeSetVisible(getNativePtr(), visible);
    }

    public boolean isSelectable() {
        checkNotDisposed();
        return selectable;
    }

    public void setSelectable(boolean selectable) {
        checkNotDisposed();
        this.selectable = selectable;
    }

    public double getOpacity() {
        checkNotDisposed();
        return opacity;
    }

    public void setOpacity(double opacity) {
        checkNotDisposed();
        this.opacity = Math.max(0.0, Math.min(1.0, opacity));
        nativeSetOpacity(getNativePtr(), this.opacity);
    }

    public int getZOrder() {
        checkNotDisposed();
        return zOrder;
    }

    public void setZOrder(int zOrder) {
        checkNotDisposed();
        this.zOrder = zOrder;
        if (layerManager != null) {
            layerManager.moveLayer(layerIndex, zOrder);
        }
    }

    public DrawStyle getStyle() {
        checkNotDisposed();
        return style;
    }

    public void setStyle(DrawStyle style) {
        checkNotDisposed();
        this.style = style;
    }

    public long getFeatureDefn() {
        checkNotDisposed();
        return nativeGetFeatureDefn(getNativePtr());
    }

    public int getGeomType() {
        checkNotDisposed();
        return nativeGetGeomType(getNativePtr());
    }

    public long getFeatureCount() {
        checkNotDisposed();
        return nativeGetFeatureCount(getNativePtr());
    }

    public void resetReading() {
        checkNotDisposed();
        nativeResetReading(getNativePtr());
    }

    public cn.cycle.chart.api.feature.FeatureInfo getNextFeature() {
        checkNotDisposed();
        long ptr = nativeGetNextFeature(getNativePtr());
        return ptr != 0 ? new cn.cycle.chart.api.feature.FeatureInfo(ptr) : null;
    }

    public cn.cycle.chart.api.feature.FeatureInfo getFeature(long fid) {
        checkNotDisposed();
        long ptr = nativeGetFeature(getNativePtr(), fid);
        return ptr != 0 ? new cn.cycle.chart.api.feature.FeatureInfo(ptr) : null;
    }

    public void setAttributeFilter(String filter) {
        checkNotDisposed();
        nativeSetAttributeFilter(getNativePtr(), filter);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private native String nativeGetName(long ptr);
    private native void nativeSetName(long ptr, String name);
    private native int nativeGetType(long ptr);
    private native void nativeSetVisible(long ptr, boolean visible);
    private native void nativeSetOpacity(long ptr, double opacity);
    private native long nativeGetFeatureDefn(long ptr);
    private native int nativeGetGeomType(long ptr);
    private native long nativeGetFeatureCount(long ptr);
    private native void nativeResetReading(long ptr);
    private native long nativeGetNextFeature(long ptr);
    private native long nativeGetFeature(long ptr, long fid);
    private native void nativeSetAttributeFilter(long ptr, String filter);
    private native void nativeDestroy(long ptr);
    static native void nativeArrayDestroy(long ptr);
}
