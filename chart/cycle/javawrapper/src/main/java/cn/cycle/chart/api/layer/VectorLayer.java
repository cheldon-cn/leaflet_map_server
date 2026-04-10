package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.api.feature.FeatureInfo;
import cn.cycle.chart.api.feature.FieldDefn;
import cn.cycle.chart.api.geometry.Envelope;

public final class VectorLayer extends Layer {

    static {
        JniBridge.initialize();
    }

    VectorLayer(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public long getFeatureCount() {
        checkNotDisposed();
        return nativeGetFeatureCount(getNativePtr());
    }

    public FeatureInfo getFeatureByFid(long fid) {
        checkNotDisposed();
        long featurePtr = nativeGetFeature(getNativePtr(), fid);
        if (featurePtr == 0) {
            return null;
        }
        return new FeatureInfo(featurePtr);
    }

    public FeatureInfo getNextFeature() {
        checkNotDisposed();
        long featurePtr = nativeGetNextFeature(getNativePtr());
        if (featurePtr == 0) {
            return null;
        }
        return new FeatureInfo(featurePtr);
    }

    public void setSpatialFilter(double minX, double minY, double maxX, double maxY) {
        checkNotDisposed();
        nativeSetSpatialFilterRect(getNativePtr(), minX, minY, maxX, maxY);
    }

    public Envelope getExtent() {
        checkNotDisposed();
        double[] extent = new double[4];
        boolean success = nativeGetExtent(getNativePtr(), extent);
        if (!success) {
            return null;
        }
        return new Envelope(extent[0], extent[1], extent[2], extent[3]);
    }

    public void resetReading() {
        checkNotDisposed();
        nativeResetReading(getNativePtr());
    }

    public int addFeature(FeatureInfo feature) {
        checkNotDisposed();
        if (feature == null) {
            throw new IllegalArgumentException("feature must not be null");
        }
        return nativeAddFeature(getNativePtr(), feature.getNativePtr());
    }

    public long getFieldCount() {
        checkNotDisposed();
        return nativeGetFieldCount(getNativePtr());
    }

    public FieldDefn getFieldDefn(int index) {
        checkNotDisposed();
        long defnPtr = nativeGetFieldDefn(getNativePtr(), index);
        if (defnPtr == 0) {
            return null;
        }
        return new FieldDefn(defnPtr);
    }

    public FieldDefn getFieldDefn(long index) {
        return getFieldDefn((int) index);
    }

    private native long nativeGetFeatureCount(long ptr);
    private native long nativeGetFeature(long ptr, long fid);
    private native long nativeGetNextFeature(long ptr);
    private native void nativeSetSpatialFilterRect(long ptr, double minX, double minY, double maxX, double maxY);
    private native boolean nativeGetExtent(long ptr, double[] extent);
    private native void nativeResetReading(long ptr);
    private native int nativeAddFeature(long ptr, long featurePtr);
    private native long nativeGetFieldCount(long ptr);
    private native long nativeGetFieldDefn(long ptr, int index);
}
