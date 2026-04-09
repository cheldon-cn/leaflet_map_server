package cn.cycle.chart.api.layer;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.api.feature.FeatureInfo;

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

    public FeatureInfo getNextFeature() {
        checkNotDisposed();
        long featurePtr = nativeGetNextFeature(getNativePtr());
        if (featurePtr == 0) {
            return null;
        }
        return new FeatureInfo(featurePtr);
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

    private native long nativeGetFeatureCount(long ptr);
    private native long nativeGetNextFeature(long ptr);
    private native void nativeResetReading(long ptr);
    private native int nativeAddFeature(long ptr, long featurePtr);
}
