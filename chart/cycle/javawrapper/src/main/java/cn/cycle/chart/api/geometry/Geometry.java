package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public abstract class Geometry extends NativeObject {

    public enum Type {
        UNKNOWN(0),
        POINT(1),
        LINESTRING(2),
        POLYGON(3),
        MULTIPOINT(4),
        MULTILINESTRING(5),
        MULTIPOLYGON(6),
        GEOMETRYCOLLECTION(7);

        private final int value;

        Type(int value) {
            this.value = value;
        }

        public int getValue() {
            return value;
        }

        public static Type fromValue(int value) {
            for (Type type : values()) {
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

    Geometry() {
    }

    Geometry(long nativePtr) {
        setNativePtr(nativePtr);
    }

    public abstract Type getType();

    public int getDimension() {
        checkNotDisposed();
        return nativeGetDimension(getNativePtr());
    }

    public boolean isEmpty() {
        checkNotDisposed();
        return nativeIsEmpty(getNativePtr());
    }

    public boolean isValid() {
        checkNotDisposed();
        return nativeIsValid(getNativePtr());
    }

    public int getSRID() {
        checkNotDisposed();
        return nativeGetSRID(getNativePtr());
    }

    public void setSRID(int srid) {
        checkNotDisposed();
        nativeSetSRID(getNativePtr(), srid);
    }

    public String asText() {
        checkNotDisposed();
        return nativeAsText(getNativePtr());
    }

    public String asGeoJSON() {
        checkNotDisposed();
        return nativeAsGeoJSON(getNativePtr());
    }

    public Envelope getEnvelope() {
        checkNotDisposed();
        double[] coords = nativeGetEnvelope(getNativePtr());
        if (coords == null || coords.length < 4) {
            return null;
        }
        return new Envelope(coords[0], coords[1], coords[2], coords[3]);
    }

    public long getNumPoints() {
        checkNotDisposed();
        return nativeGetNumPoints(getNativePtr());
    }

    public boolean isSimple() {
        checkNotDisposed();
        return nativeIsSimple(getNativePtr());
    }

    public boolean is3D() {
        checkNotDisposed();
        return nativeIs3D(getNativePtr());
    }

    public boolean isMeasured() {
        checkNotDisposed();
        return nativeIsMeasured(getNativePtr());
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    static native void nativeDestroy(long ptr);
    static native int nativeGetType(long ptr);
    static native int nativeGetDimension(long ptr);
    static native boolean nativeIsEmpty(long ptr);
    static native boolean nativeIsValid(long ptr);
    static native int nativeGetSRID(long ptr);
    static native void nativeSetSRID(long ptr, int srid);
    static native String nativeAsText(long ptr);
    static native String nativeAsGeoJSON(long ptr);
    static native double[] nativeGetEnvelope(long ptr);
    static native long nativeGetNumPoints(long ptr);
    static native boolean nativeIsSimple(long ptr);
    static native boolean nativeIs3D(long ptr);
    static native boolean nativeIsMeasured(long ptr);
}
