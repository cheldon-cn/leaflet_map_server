package cn.cycle.chart.api.geometry;

import cn.cycle.chart.jni.JniBridge;
import cn.cycle.chart.jni.NativeObject;

public final class GeometryFactory extends NativeObject {

    static {
        JniBridge.initialize();
    }

    public GeometryFactory() {
        setNativePtr(nativeCreate());
    }

    GeometryFactory(long nativePtr) {
        setNativePtr(nativePtr);
    }

    @Override
    protected void nativeDispose(long ptr) {
        nativeDestroy(ptr);
    }

    private static native long nativeCreate();
    private native void nativeDestroy(long ptr);
    private native long nativeCreatePoint(long ptr, double x, double y);
    private native long nativeCreateLineString(long ptr);
    private native long nativeCreatePolygon(long ptr);
    private native long nativeCreateFromWkt(long ptr, String wkt);
    private native long nativeCreateFromWkb(long ptr, byte[] wkb);
    private native long nativeCreateFromGeoJson(long ptr, String geojson);
}
