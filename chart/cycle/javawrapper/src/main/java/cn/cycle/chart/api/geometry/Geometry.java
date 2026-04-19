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

    public static Geometry fromNativePtr(long ptr) {
        return GeometryFactory.fromNativePtr(ptr);
    }

    public abstract Type getType();

    public String getTypeName() {
        checkNotDisposed();
        return nativeGetTypeName(getNativePtr());
    }

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

    public boolean equals(Geometry other) {
        checkNotDisposed();
        return nativeEquals(getNativePtr(), other.getNativePtr());
    }

    public boolean intersects(Geometry other) {
        checkNotDisposed();
        return nativeIntersects(getNativePtr(), other.getNativePtr());
    }

    public boolean contains(Geometry other) {
        checkNotDisposed();
        return nativeContains(getNativePtr(), other.getNativePtr());
    }

    public boolean within(Geometry other) {
        checkNotDisposed();
        return nativeWithin(getNativePtr(), other.getNativePtr());
    }

    public boolean crosses(Geometry other) {
        checkNotDisposed();
        return nativeCrosses(getNativePtr(), other.getNativePtr());
    }

    public boolean touches(Geometry other) {
        checkNotDisposed();
        return nativeTouches(getNativePtr(), other.getNativePtr());
    }

    public boolean overlaps(Geometry other) {
        checkNotDisposed();
        return nativeOverlaps(getNativePtr(), other.getNativePtr());
    }

    public double distance(Geometry other) {
        checkNotDisposed();
        return nativeDistance(getNativePtr(), other.getNativePtr());
    }

    public Geometry intersection(Geometry other) {
        checkNotDisposed();
        long ptr = nativeIntersection(getNativePtr(), other.getNativePtr());
        return GeometryFactory.fromNativePtr(ptr);
    }

    public Geometry union(Geometry other) {
        checkNotDisposed();
        long ptr = nativeUnion(getNativePtr(), other.getNativePtr());
        return GeometryFactory.fromNativePtr(ptr);
    }

    public Geometry difference(Geometry other) {
        checkNotDisposed();
        long ptr = nativeDifference(getNativePtr(), other.getNativePtr());
        return GeometryFactory.fromNativePtr(ptr);
    }

    public Geometry buffer(double distance) {
        checkNotDisposed();
        long ptr = nativeBuffer(getNativePtr(), distance);
        return GeometryFactory.fromNativePtr(ptr);
    }

    public Geometry clone() {
        checkNotDisposed();
        long ptr = nativeClone(getNativePtr());
        return GeometryFactory.fromNativePtr(ptr);
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
    static native String nativeGetTypeName(long ptr);
    static native boolean nativeEquals(long ptr, long otherPtr);
    static native boolean nativeIntersects(long ptr, long otherPtr);
    static native boolean nativeContains(long ptr, long otherPtr);
    static native boolean nativeWithin(long ptr, long otherPtr);
    static native boolean nativeCrosses(long ptr, long otherPtr);
    static native boolean nativeTouches(long ptr, long otherPtr);
    static native boolean nativeOverlaps(long ptr, long otherPtr);
    static native double nativeDistance(long ptr, long otherPtr);
    static native long nativeIntersection(long ptr, long otherPtr);
    static native long nativeUnion(long ptr, long otherPtr);
    static native long nativeDifference(long ptr, long otherPtr);
    static native long nativeBuffer(long ptr, double distance);
    static native long nativeClone(long ptr);
}
