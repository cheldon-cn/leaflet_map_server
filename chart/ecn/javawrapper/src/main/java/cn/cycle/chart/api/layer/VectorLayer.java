package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.api.feature.FeatureInfo;
import cn.cycle.chart.api.feature.FieldDefn;
import cn.cycle.chart.api.geometry.Envelope;
import cn.cycle.chart.api.geometry.Geometry;

public final class VectorLayer extends Layer {

    static {
        JniBridge.initialize();
    }

    VectorLayer(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public static VectorLayer create(String name, String geomType) {
        long ptr = nativeCreate(name, geomType);
        return ptr != 0 ? new VectorLayer(ptr) : null;
    }

    public static VectorLayer createFromDatasource(long dsPtr, int layerIndex) {
        long ptr = nativeCreateFromDatasource(dsPtr, layerIndex);
        return ptr != 0 ? new VectorLayer(ptr) : null;
    }

    public static VectorLayer createFromFeatures(String name, String geomType, long[] featurePtrs) {
        long ptr = nativeCreateFromFeatures(name, geomType, featurePtrs);
        return ptr != 0 ? new VectorLayer(ptr) : null;
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

    public FeatureInfo getFeatureById(long id) {
        checkNotDisposed();
        long featurePtr = nativeGetFeatureById(getNativePtr(), id);
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

    public void setSpatialFilter(Geometry filter) {
        checkNotDisposed();
        nativeSetSpatialFilter(getNativePtr(), filter.getNativePtr());
    }

    public Geometry getSpatialFilter() {
        checkNotDisposed();
        long ptr = nativeGetSpatialFilter(getNativePtr());
        return ptr != 0 ? Geometry.fromNativePtr(ptr) : null;
    }

    public String getAttributeFilter() {
        checkNotDisposed();
        return nativeGetAttributeFilter(getNativePtr());
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

    public long getExtentPtr() {
        checkNotDisposed();
        return nativeGetExtentPtr(getNativePtr());
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

    public int updateFeature(FeatureInfo feature) {
        checkNotDisposed();
        if (feature == null) {
            throw new IllegalArgumentException("feature must not be null");
        }
        return nativeUpdateFeature(getNativePtr(), feature.getNativePtr());
    }

    public int deleteFeature(long fid) {
        checkNotDisposed();
        return nativeDeleteFeature(getNativePtr(), fid);
    }

    public int createFeature(FeatureInfo feature) {
        checkNotDisposed();
        if (feature == null) {
            throw new IllegalArgumentException("feature must not be null");
        }
        return nativeCreateFeature(getNativePtr(), feature.getNativePtr());
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

    public long getFeatureDefn() {
        checkNotDisposed();
        return nativeGetFeatureDefn(getNativePtr());
    }

    public void setStyle(String style) {
        checkNotDisposed();
        nativeSetStyle(getNativePtr(), style);
    }

    private static native long nativeCreate(String name, String geomType);
    private static native long nativeCreateFromDatasource(long dsPtr, int layerIndex);
    private static native long nativeCreateFromFeatures(String name, String geomType, long[] featurePtrs);
    private native long nativeGetFeatureCount(long ptr);
    private native long nativeGetFeature(long ptr, long fid);
    private native long nativeGetFeatureById(long ptr, long id);
    private native long nativeGetNextFeature(long ptr);
    private native void nativeSetSpatialFilterRect(long ptr, double minX, double minY, double maxX, double maxY);
    private native void nativeSetSpatialFilter(long ptr, long geomPtr);
    private native long nativeGetSpatialFilter(long ptr);
    private native String nativeGetAttributeFilter(long ptr);
    private native boolean nativeGetExtent(long ptr, double[] extent);
    private native long nativeGetExtentPtr(long ptr);
    private native void nativeResetReading(long ptr);
    private native int nativeAddFeature(long ptr, long featurePtr);
    private native int nativeUpdateFeature(long ptr, long featurePtr);
    private native int nativeDeleteFeature(long ptr, long fid);
    private native int nativeCreateFeature(long ptr, long featurePtr);
    private native long nativeGetFieldCount(long ptr);
    private native long nativeGetFieldDefn(long ptr, int index);
    private native long nativeGetFeatureDefn(long ptr);
    private native void nativeSetStyle(long ptr, String style);
}
