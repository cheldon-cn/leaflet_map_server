package cn.cycle.chart.api.geometry;

class GeometryFactory {

    static Geometry fromNativePtr(long ptr) {
        if (ptr == 0) {
            return null;
        }
        int type = Geometry.nativeGetType(ptr);
        switch (Geometry.Type.fromValue(type)) {
            case POINT:
                return new Point(ptr);
            case LINESTRING:
                return new LineString(ptr);
            case POLYGON:
                return new Polygon(ptr);
            case MULTIPOINT:
                return new MultiPoint(ptr);
            case MULTILINESTRING:
                return new MultiLineString(ptr);
            case MULTIPOLYGON:
                return new MultiPolygon(ptr);
            case GEOMETRYCOLLECTION:
                return new GeometryCollection(ptr);
            default:
                return null;
        }
    }
}
