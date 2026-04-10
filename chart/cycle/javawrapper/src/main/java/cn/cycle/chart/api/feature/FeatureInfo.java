package cn.cycle.chart.api.feature;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;
import cn.cycle.chart.api.geometry.Geometry;
import cn.cycle.chart.api.layer.VectorLayer;

import java.util.HashMap;
import java.util.Map;

public final class FeatureInfo extends NativeObject {

    static {
        JniBridge.initialize();
    }

    private VectorLayer layer;
    private Map<String, Integer> fieldNameIndexCache;

    public FeatureInfo(long nativePtr) {
        setNativePtr(nativePtr);
    }

    void setLayer(VectorLayer layer) {
        this.layer = layer;
    }

    public long getFid() {
        checkNotDisposed();
        return nativeGetFid(getNativePtr());
    }

    public void setFid(long fid) {
        checkNotDisposed();
        nativeSetFid(getNativePtr(), fid);
    }

    public long getFieldCount() {
        checkNotDisposed();
        return nativeGetFieldCount(getNativePtr());
    }

    public boolean isFieldSet(long index) {
        checkNotDisposed();
        return nativeIsFieldSet(getNativePtr(), index);
    }

    public boolean isFieldNull(long index) {
        checkNotDisposed();
        return nativeIsFieldNull(getNativePtr(), index);
    }

    public int getFieldAsInteger(long index) {
        checkNotDisposed();
        return nativeGetFieldAsInteger(getNativePtr(), index);
    }

    public double getFieldAsReal(long index) {
        checkNotDisposed();
        return nativeGetFieldAsReal(getNativePtr(), index);
    }

    public String getFieldAsString(long index) {
        checkNotDisposed();
        return nativeGetFieldAsString(getNativePtr(), index);
    }

    public void setFieldInteger(long index, int value) {
        checkNotDisposed();
        nativeSetFieldInteger(getNativePtr(), index, value);
    }

    public void setFieldReal(long index, double value) {
        checkNotDisposed();
        nativeSetFieldReal(getNativePtr(), index, value);
    }

    public void setFieldString(long index, String value) {
        checkNotDisposed();
        nativeSetFieldString(getNativePtr(), index, value);
    }

    public void setFieldNull(long index) {
        checkNotDisposed();
        nativeSetFieldNull(getNativePtr(), index);
    }

    public long getGeometryPtr() {
        checkNotDisposed();
        return nativeGetGeometry(getNativePtr());
    }

    public void setGeometryPtr(long geomPtr) {
        checkNotDisposed();
        nativeSetGeometry(getNativePtr(), geomPtr);
    }

    public String getFeatureName() {
        checkNotDisposed();
        String name = nativeGetFeatureName(getNativePtr());
        return name != null ? name : "Feature_" + getFid();
    }

    public String getFeatureClass() {
        checkNotDisposed();
        return nativeGetFeatureClass(getNativePtr());
    }

    public String getFieldName(long index) {
        checkNotDisposed();
        if (fieldNameIndexCache != null) {
            for (Map.Entry<String, Integer> entry : fieldNameIndexCache.entrySet()) {
                if (entry.getValue() == index) {
                    return entry.getKey();
                }
            }
        }
        String name = nativeGetFieldName(getNativePtr(), index);
        if (name == null && layer != null) {
            FieldDefn defn = layer.getFieldDefn(index);
            if (defn != null) {
                name = defn.getName();
            }
        }
        return name;
    }

    public Object getFieldValue(String name) {
        checkNotDisposed();
        int index = getFieldIndex(name);
        if (index < 0) {
            return null;
        }
        return getFieldValue(index);
    }

    public Object getFieldValue(int index) {
        checkNotDisposed();
        if (isFieldNull(index)) {
            return null;
        }
        if (layer != null) {
            FieldDefn defn = layer.getFieldDefn(index);
            if (defn != null) {
                switch (defn.getType()) {
                    case FieldDefn.TYPE_INTEGER:
                        return getFieldAsInteger(index);
                    case FieldDefn.TYPE_REAL:
                        return getFieldAsReal(index);
                    case FieldDefn.TYPE_STRING:
                        return getFieldAsString(index);
                    default:
                        return getFieldAsString(index);
                }
            }
        }
        return getFieldAsString(index);
    }

    public int getFieldIndex(String name) {
        checkNotDisposed();
        if (name == null) {
            return -1;
        }
        if (fieldNameIndexCache == null) {
            buildFieldNameIndexCache();
        }
        Integer index = fieldNameIndexCache.get(name);
        return index != null ? index : -1;
    }

    private void buildFieldNameIndexCache() {
        fieldNameIndexCache = new HashMap<>();
        long count = getFieldCount();
        for (int i = 0; i < count; i++) {
            String name = nativeGetFieldName(getNativePtr(), i);
            if (name != null && !name.isEmpty()) {
                fieldNameIndexCache.put(name, i);
            }
        }
    }

    public Map<String, Object> getAllFields() {
        checkNotDisposed();
        Map<String, Object> fields = new HashMap<>();
        long count = getFieldCount();
        for (int i = 0; i < count; i++) {
            String name = getFieldName(i);
            if (name != null) {
                fields.put(name, getFieldValue(i));
            }
        }
        return fields;
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private native long nativeGetFid(long ptr);
    private native void nativeSetFid(long ptr, long fid);
    private native long nativeGetFieldCount(long ptr);
    private native boolean nativeIsFieldSet(long ptr, long index);
    private native boolean nativeIsFieldNull(long ptr, long index);
    private native int nativeGetFieldAsInteger(long ptr, long index);
    private native double nativeGetFieldAsReal(long ptr, long index);
    private native String nativeGetFieldAsString(long ptr, long index);
    private native void nativeSetFieldInteger(long ptr, long index, int value);
    private native void nativeSetFieldReal(long ptr, long index, double value);
    private native void nativeSetFieldString(long ptr, long index, String value);
    private native void nativeSetFieldNull(long ptr, long index);
    private native long nativeGetGeometry(long ptr);
    private native void nativeSetGeometry(long ptr, long geomPtr);
    private native String nativeGetFeatureName(long ptr);
    private native String nativeGetFeatureClass(long ptr);
    private native String nativeGetFieldName(long ptr, long index);
    private native void nativeDestroy(long ptr);
}
