/**
 * @file sdk_c_api_geom.cpp
 * @brief OGC Chart SDK C API - Geometry Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/geom/coordinate.h>
#include <ogc/geom/envelope.h>
#include <ogc/geom/geometry.h>
#include <ogc/geom/point.h>
#include <ogc/geom/linestring.h>
#include <ogc/geom/linearring.h>
#include <ogc/geom/polygon.h>
#include <ogc/geom/multipoint.h>
#include <ogc/geom/multilinestring.h>
#include <ogc/geom/multipolygon.h>
#include <ogc/geom/geometrycollection.h>
#include <ogc/geom/factory.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace ogc;

#ifdef __cplusplus
extern "C" {
#endif

namespace { static std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

static char* AllocString(const std::string& str) {
    char* result = static_cast<char*>(std::malloc(str.size() + 1));
    if (result) {
        std::memcpy(result, str.c_str(), str.size() + 1);
    }
    return result;
}

static ogc_geom_type_e ToCType(GeomType type) {
    switch (type) {
        case GeomType::kPoint: return OGC_GEOM_TYPE_POINT;
        case GeomType::kLineString: return OGC_GEOM_TYPE_LINESTRING;
        case GeomType::kPolygon: return OGC_GEOM_TYPE_POLYGON;
        case GeomType::kMultiPoint: return OGC_GEOM_TYPE_MULTIPOINT;
        case GeomType::kMultiLineString: return OGC_GEOM_TYPE_MULTILINESTRING;
        case GeomType::kMultiPolygon: return OGC_GEOM_TYPE_MULTIPOLYGON;
        case GeomType::kGeometryCollection: return OGC_GEOM_TYPE_GEOMETRYCOLLECTION;
        default: return OGC_GEOM_TYPE_UNKNOWN;
    }
}

}  

ogc_coordinate_t* ogc_coordinate_create(double x, double y) {
    ogc_coordinate_t* coord = static_cast<ogc_coordinate_t*>(std::malloc(sizeof(ogc_coordinate_t)));
    if (coord) {
        coord->x = x;
        coord->y = y;
        coord->z = 0.0;
        coord->m = 0.0;
    }
    return coord;
}

ogc_coordinate_t* ogc_coordinate_create_3d(double x, double y, double z) {
    ogc_coordinate_t* coord = static_cast<ogc_coordinate_t*>(std::malloc(sizeof(ogc_coordinate_t)));
    if (coord) {
        coord->x = x;
        coord->y = y;
        coord->z = z;
        coord->m = 0.0;
    }
    return coord;
}

ogc_coordinate_t* ogc_coordinate_create_m(double x, double y, double z, double m) {
    ogc_coordinate_t* coord = static_cast<ogc_coordinate_t*>(std::malloc(sizeof(ogc_coordinate_t)));
    if (coord) {
        coord->x = x;
        coord->y = y;
        coord->z = z;
        coord->m = m;
    }
    return coord;
}

void ogc_coordinate_destroy(ogc_coordinate_t* coord) {
    std::free(coord);
}

double ogc_coordinate_distance(const ogc_coordinate_t* a, const ogc_coordinate_t* b) {
    if (a && b) {
        Coordinate ca(a->x, a->y, a->z, a->m);
        Coordinate cb(b->x, b->y, b->z, b->m);
        return ca.Distance(cb);
    }
    return 0.0;
}

ogc_envelope_t* ogc_envelope_create(void) {
    return reinterpret_cast<ogc_envelope_t*>(new Envelope());
}

ogc_envelope_t* ogc_envelope_create_from_coords(double min_x, double min_y, double max_x, double max_y) {
    return reinterpret_cast<ogc_envelope_t*>(new Envelope(min_x, min_y, max_x, max_y));
}

void ogc_envelope_destroy(ogc_envelope_t* env) {
    delete reinterpret_cast<Envelope*>(env);
}

double ogc_envelope_get_min_x(const ogc_envelope_t* env) {
    if (env) {
        return reinterpret_cast<const Envelope*>(env)->GetMinX();
    }
    return 0.0;
}

double ogc_envelope_get_min_y(const ogc_envelope_t* env) {
    if (env) {
        return reinterpret_cast<const Envelope*>(env)->GetMinY();
    }
    return 0.0;
}

double ogc_envelope_get_max_x(const ogc_envelope_t* env) {
    if (env) {
        return reinterpret_cast<const Envelope*>(env)->GetMaxX();
    }
    return 0.0;
}

double ogc_envelope_get_max_y(const ogc_envelope_t* env) {
    if (env) {
        return reinterpret_cast<const Envelope*>(env)->GetMaxY();
    }
    return 0.0;
}

double ogc_envelope_get_width(const ogc_envelope_t* env) {
    if (env) {
        return reinterpret_cast<const Envelope*>(env)->GetWidth();
    }
    return 0.0;
}

double ogc_envelope_get_height(const ogc_envelope_t* env) {
    if (env) {
        return reinterpret_cast<const Envelope*>(env)->GetHeight();
    }
    return 0.0;
}

double ogc_envelope_get_area(const ogc_envelope_t* env) {
    if (env) {
        return reinterpret_cast<const Envelope*>(env)->GetArea();
    }
    return 0.0;
}

int ogc_envelope_contains(const ogc_envelope_t* env, double x, double y) {
    if (env) {
        return reinterpret_cast<const Envelope*>(env)->Contains(Coordinate(x, y)) ? 1 : 0;
    }
    return 0;
}

int ogc_envelope_intersects(const ogc_envelope_t* env, const ogc_envelope_t* other) {
    if (env && other) {
        return reinterpret_cast<const Envelope*>(env)->Intersects(
            *reinterpret_cast<const Envelope*>(other)) ? 1 : 0;
    }
    return 0;
}

void ogc_envelope_expand(ogc_envelope_t* env, double dx, double dy) {
    if (env) {
        reinterpret_cast<Envelope*>(env)->ExpandBy(dx, dy);
    }
}

ogc_coordinate_t ogc_envelope_get_center(const ogc_envelope_t* env) {
    ogc_coordinate_t result = {0, 0, 0, 0};
    if (env) {
        Coordinate c = reinterpret_cast<const Envelope*>(env)->GetCentre();
        result.x = c.x;
        result.y = c.y;
        result.z = c.z;
        result.m = c.m;
    }
    return result;
}

void ogc_geometry_destroy(ogc_geometry_t* geom) {
    delete reinterpret_cast<Geometry*>(geom);
}

ogc_geom_type_e ogc_geometry_get_type(const ogc_geometry_t* geom) {
    if (geom) {
        return ToCType(reinterpret_cast<const Geometry*>(geom)->GetGeometryType());
    }
    return OGC_GEOM_TYPE_UNKNOWN;
}

const char* ogc_geometry_get_type_name(const ogc_geometry_t* geom) {
    if (geom) {
        return reinterpret_cast<const Geometry*>(geom)->GetGeometryTypeString();
    }
    return "UNKNOWN";
}

int ogc_geometry_get_dimension(const ogc_geometry_t* geom) {
    if (geom) {
        return static_cast<int>(reinterpret_cast<const Geometry*>(geom)->GetDimension());
    }
    return 0;
}

int ogc_geometry_is_empty(const ogc_geometry_t* geom) {
    if (geom) {
        return reinterpret_cast<const Geometry*>(geom)->IsEmpty() ? 1 : 0;
    }
    return 1;
}

int ogc_geometry_is_valid(const ogc_geometry_t* geom) {
    if (geom) {
        return reinterpret_cast<const Geometry*>(geom)->IsValid() ? 1 : 0;
    }
    return 0;
}

int ogc_geometry_is_simple(const ogc_geometry_t* geom) {
    if (geom) {
        return reinterpret_cast<const Geometry*>(geom)->IsSimple() ? 1 : 0;
    }
    return 0;
}

int ogc_geometry_is_3d(const ogc_geometry_t* geom) {
    if (geom) {
        return reinterpret_cast<const Geometry*>(geom)->Is3D() ? 1 : 0;
    }
    return 0;
}

int ogc_geometry_is_measured(const ogc_geometry_t* geom) {
    if (geom) {
        return reinterpret_cast<const Geometry*>(geom)->IsMeasured() ? 1 : 0;
    }
    return 0;
}

int ogc_geometry_get_srid(const ogc_geometry_t* geom) {
    if (geom) {
        return reinterpret_cast<const Geometry*>(geom)->GetSRID();
    }
    return 0;
}

void ogc_geometry_set_srid(ogc_geometry_t* geom, int srid) {
    if (geom) {
        reinterpret_cast<Geometry*>(geom)->SetSRID(srid);
    }
}

double ogc_geometry_get_area(const ogc_geometry_t* geom) {
    if (geom) {
        return reinterpret_cast<const Geometry*>(geom)->Area();
    }
    return 0.0;
}

double ogc_geometry_get_length(const ogc_geometry_t* geom) {
    if (geom) {
        return reinterpret_cast<const Geometry*>(geom)->Length();
    }
    return 0.0;
}

size_t ogc_geometry_get_num_points(const ogc_geometry_t* geom) {
    if (geom) {
        return reinterpret_cast<const Geometry*>(geom)->GetNumCoordinates();
    }
    return 0;
}

ogc_envelope_t* ogc_geometry_get_envelope(const ogc_geometry_t* geom) {
    if (geom) {
        const Envelope& env = reinterpret_cast<const Geometry*>(geom)->GetEnvelope();
        return reinterpret_cast<ogc_envelope_t*>(new Envelope(env));
    }
    return nullptr;
}

ogc_coordinate_t ogc_geometry_get_centroid(const ogc_geometry_t* geom) {
    ogc_coordinate_t result = {0, 0, 0, 0};
    if (geom) {
        Coordinate c = reinterpret_cast<const Geometry*>(geom)->GetCentroid();
        result.x = c.x;
        result.y = c.y;
        result.z = c.z;
        result.m = c.m;
    }
    return result;
}

char* ogc_geometry_as_text(const ogc_geometry_t* geom, int precision) {
    if (geom) {
        std::string wkt = reinterpret_cast<const Geometry*>(geom)->AsText(precision);
        return AllocString(wkt);
    }
    return nullptr;
}

char* ogc_geometry_as_geojson(const ogc_geometry_t* geom, int precision) {
    if (geom) {
        std::string json = reinterpret_cast<const Geometry*>(geom)->AsGeoJSON(precision);
        return AllocString(json);
    }
    return nullptr;
}

void ogc_geometry_free_string(char* str) {
    std::free(str);
}

int ogc_geometry_equals(const ogc_geometry_t* a, const ogc_geometry_t* b, double tolerance) {
    if (a && b) {
        return reinterpret_cast<const Geometry*>(a)->Equals(
            reinterpret_cast<const Geometry*>(b), tolerance) ? 1 : 0;
    }
    return 0;
}

int ogc_geometry_intersects(const ogc_geometry_t* a, const ogc_geometry_t* b) {
    if (a && b) {
        return reinterpret_cast<const Geometry*>(a)->Intersects(
            reinterpret_cast<const Geometry*>(b)) ? 1 : 0;
    }
    return 0;
}

int ogc_geometry_contains(const ogc_geometry_t* a, const ogc_geometry_t* b) {
    if (a && b) {
        return reinterpret_cast<const Geometry*>(a)->Contains(
            reinterpret_cast<const Geometry*>(b)) ? 1 : 0;
    }
    return 0;
}

int ogc_geometry_within(const ogc_geometry_t* a, const ogc_geometry_t* b) {
    if (a && b) {
        return reinterpret_cast<const Geometry*>(a)->Within(
            reinterpret_cast<const Geometry*>(b)) ? 1 : 0;
    }
    return 0;
}

int ogc_geometry_crosses(const ogc_geometry_t* a, const ogc_geometry_t* b) {
    if (a && b) {
        return reinterpret_cast<const Geometry*>(a)->Crosses(
            reinterpret_cast<const Geometry*>(b)) ? 1 : 0;
    }
    return 0;
}

int ogc_geometry_touches(const ogc_geometry_t* a, const ogc_geometry_t* b) {
    if (a && b) {
        return reinterpret_cast<const Geometry*>(a)->Touches(
            reinterpret_cast<const Geometry*>(b)) ? 1 : 0;
    }
    return 0;
}

int ogc_geometry_overlaps(const ogc_geometry_t* a, const ogc_geometry_t* b) {
    if (a && b) {
        return reinterpret_cast<const Geometry*>(a)->Overlaps(
            reinterpret_cast<const Geometry*>(b)) ? 1 : 0;
    }
    return 0;
}

double ogc_geometry_distance(const ogc_geometry_t* a, const ogc_geometry_t* b) {
    if (a && b) {
        return reinterpret_cast<const Geometry*>(a)->Distance(
            reinterpret_cast<const Geometry*>(b));
    }
    return 0.0;
}

ogc_geometry_t* ogc_geometry_intersection(const ogc_geometry_t* a, const ogc_geometry_t* b) {
    if (a && b) {
        GeometryPtr result;
        if (reinterpret_cast<const Geometry*>(a)->Intersection(
            reinterpret_cast<const Geometry*>(b), result) == GeomResult::kSuccess) {
            return reinterpret_cast<ogc_geometry_t*>(result.release());
        }
    }
    return nullptr;
}

ogc_geometry_t* ogc_geometry_union(const ogc_geometry_t* a, const ogc_geometry_t* b) {
    if (a && b) {
        GeometryPtr result;
        if (reinterpret_cast<const Geometry*>(a)->Union(
            reinterpret_cast<const Geometry*>(b), result) == GeomResult::kSuccess) {
            return reinterpret_cast<ogc_geometry_t*>(result.release());
        }
    }
    return nullptr;
}

ogc_geometry_t* ogc_geometry_difference(const ogc_geometry_t* a, const ogc_geometry_t* b) {
    if (a && b) {
        GeometryPtr result;
        if (reinterpret_cast<const Geometry*>(a)->Difference(
            reinterpret_cast<const Geometry*>(b), result) == GeomResult::kSuccess) {
            return reinterpret_cast<ogc_geometry_t*>(result.release());
        }
    }
    return nullptr;
}

ogc_geometry_t* ogc_geometry_buffer(const ogc_geometry_t* geom, double distance, int segments) {
    if (geom) {
        GeometryPtr result;
        if (reinterpret_cast<const Geometry*>(geom)->Buffer(distance, result, segments) == GeomResult::kSuccess) {
            return reinterpret_cast<ogc_geometry_t*>(result.release());
        }
    }
    return nullptr;
}

ogc_geometry_t* ogc_geometry_clone(const ogc_geometry_t* geom) {
    if (geom) {
        GeometryPtr cloned = reinterpret_cast<const Geometry*>(geom)->Clone();
        return reinterpret_cast<ogc_geometry_t*>(cloned.release());
    }
    return nullptr;
}

ogc_geometry_t* ogc_point_create(double x, double y) {
    return reinterpret_cast<ogc_geometry_t*>(Point::Create(x, y).release());
}

ogc_geometry_t* ogc_point_create_3d(double x, double y, double z) {
    return reinterpret_cast<ogc_geometry_t*>(Point::Create(x, y, z).release());
}

ogc_geometry_t* ogc_point_create_from_coord(const ogc_coordinate_t* coord) {
    if (coord) {
        Coordinate c(coord->x, coord->y, coord->z, coord->m);
        return reinterpret_cast<ogc_geometry_t*>(Point::Create(c).release());
    }
    return nullptr;
}

double ogc_point_get_x(const ogc_geometry_t* point) {
    if (point) {
        return reinterpret_cast<const Point*>(point)->GetX();
    }
    return 0.0;
}

double ogc_point_get_y(const ogc_geometry_t* point) {
    if (point) {
        return reinterpret_cast<const Point*>(point)->GetY();
    }
    return 0.0;
}

double ogc_point_get_z(const ogc_geometry_t* point) {
    if (point) {
        return reinterpret_cast<const Point*>(point)->GetZ();
    }
    return 0.0;
}

double ogc_point_get_m(const ogc_geometry_t* point) {
    if (point) {
        return reinterpret_cast<const Point*>(point)->GetM();
    }
    return 0.0;
}

void ogc_point_set_x(ogc_geometry_t* point, double x) {
    if (point) {
        reinterpret_cast<Point*>(point)->SetX(x);
    }
}

void ogc_point_set_y(ogc_geometry_t* point, double y) {
    if (point) {
        reinterpret_cast<Point*>(point)->SetY(y);
    }
}

void ogc_point_set_z(ogc_geometry_t* point, double z) {
    if (point) {
        reinterpret_cast<Point*>(point)->SetZ(z);
    }
}

void ogc_point_set_m(ogc_geometry_t* point, double m) {
    if (point) {
        reinterpret_cast<Point*>(point)->SetM(m);
    }
}

ogc_coordinate_t ogc_point_get_coordinate(const ogc_geometry_t* point) {
    ogc_coordinate_t result = {0, 0, 0, 0};
    if (point) {
        Coordinate c = reinterpret_cast<const Point*>(point)->GetCoordinate();
        result.x = c.x;
        result.y = c.y;
        result.z = c.z;
        result.m = c.m;
    }
    return result;
}

ogc_geometry_t* ogc_linestring_create(void) {
    return reinterpret_cast<ogc_geometry_t*>(LineString::Create().release());
}

ogc_geometry_t* ogc_linestring_create_from_coords(const ogc_coordinate_t* coords, size_t count) {
    if (coords && count > 0) {
        CoordinateList coordList;
        for (size_t i = 0; i < count; ++i) {
            coordList.push_back(Coordinate(coords[i].x, coords[i].y, coords[i].z, coords[i].m));
        }
        return reinterpret_cast<ogc_geometry_t*>(LineString::Create(coordList).release());
    }
    return nullptr;
}

size_t ogc_linestring_get_num_points(const ogc_geometry_t* line) {
    if (line) {
        return reinterpret_cast<const LineString*>(line)->GetNumPoints();
    }
    return 0;
}

ogc_coordinate_t ogc_linestring_get_point_n(const ogc_geometry_t* line, size_t index) {
    ogc_coordinate_t result = {0, 0, 0, 0};
    if (line) {
        Coordinate c = reinterpret_cast<const LineString*>(line)->GetCoordinateN(index);
        result.x = c.x;
        result.y = c.y;
        result.z = c.z;
        result.m = c.m;
    }
    return result;
}

void ogc_linestring_add_point(ogc_geometry_t* line, double x, double y) {
    if (line) {
        reinterpret_cast<LineString*>(line)->AddPoint(Coordinate(x, y));
    }
}

void ogc_linestring_add_point_3d(ogc_geometry_t* line, double x, double y, double z) {
    if (line) {
        reinterpret_cast<LineString*>(line)->AddPoint(Coordinate(x, y, z));
    }
}

void ogc_linestring_set_point_n(ogc_geometry_t* line, size_t index, const ogc_coordinate_t* coord) {
    if (line && coord) {
        LineString* ls = reinterpret_cast<LineString*>(line);
        Coordinate c(coord->x, coord->y, coord->z, coord->m);
        if (index < ls->GetNumPoints()) {
            ls->RemovePoint(index);
            ls->InsertPoint(index, c);
        }
    }
}

ogc_geometry_t* ogc_linestring_get_point_geometry(const ogc_geometry_t* line, size_t index) {
    if (line) {
        Coordinate c = reinterpret_cast<const LineString*>(line)->GetPointN(index);
        PointPtr pt = Point::Create(c);
        return reinterpret_cast<ogc_geometry_t*>(pt.release());
    }
    return nullptr;
}

ogc_geometry_t* ogc_linearring_create(void) {
    return reinterpret_cast<ogc_geometry_t*>(LinearRing::Create().release());
}

ogc_geometry_t* ogc_linearring_create_from_coords(const ogc_coordinate_t* coords, size_t count) {
    if (coords && count > 0) {
        CoordinateList coordList;
        for (size_t i = 0; i < count; ++i) {
            coordList.push_back(Coordinate(coords[i].x, coords[i].y, coords[i].z, coords[i].m));
        }
        return reinterpret_cast<ogc_geometry_t*>(LinearRing::Create(coordList).release());
    }
    return nullptr;
}

int ogc_linearring_is_closed(const ogc_geometry_t* ring) {
    if (ring) {
        return reinterpret_cast<const LinearRing*>(ring)->IsClosed() ? 1 : 0;
    }
    return 0;
}

ogc_geometry_t* ogc_polygon_create(void) {
    return reinterpret_cast<ogc_geometry_t*>(Polygon::Create().release());
}

ogc_geometry_t* ogc_polygon_create_from_ring(ogc_geometry_t* exterior_ring) {
    if (exterior_ring) {
        LinearRingPtr ring(reinterpret_cast<LinearRing*>(exterior_ring));
        return reinterpret_cast<ogc_geometry_t*>(Polygon::Create(std::move(ring)).release());
    }
    return nullptr;
}

ogc_geometry_t* ogc_polygon_create_from_coords(const ogc_coordinate_t* coords, size_t count) {
    if (coords && count > 0) {
        CoordinateList coordList;
        for (size_t i = 0; i < count; ++i) {
            coordList.push_back(Coordinate(coords[i].x, coords[i].y, coords[i].z, coords[i].m));
        }
        LinearRingPtr ring = LinearRing::Create(coordList);
        return reinterpret_cast<ogc_geometry_t*>(Polygon::Create(std::move(ring)).release());
    }
    return nullptr;
}

ogc_geometry_t* ogc_polygon_get_exterior_ring(const ogc_geometry_t* polygon) {
    if (polygon) {
        const LinearRing* ring = reinterpret_cast<const Polygon*>(polygon)->GetExteriorRing();
        if (ring) {
            GeometryPtr cloned = ring->Clone();
            return reinterpret_cast<ogc_geometry_t*>(cloned.release());
        }
    }
    return nullptr;
}

size_t ogc_polygon_get_num_interior_rings(const ogc_geometry_t* polygon) {
    if (polygon) {
        return reinterpret_cast<const Polygon*>(polygon)->GetNumInteriorRings();
    }
    return 0;
}

ogc_geometry_t* ogc_polygon_get_interior_ring_n(const ogc_geometry_t* polygon, size_t index) {
    if (polygon) {
        const LinearRing* ring = reinterpret_cast<const Polygon*>(polygon)->GetInteriorRingN(index);
        if (ring) {
            GeometryPtr cloned = ring->Clone();
            return reinterpret_cast<ogc_geometry_t*>(cloned.release());
        }
    }
    return nullptr;
}

void ogc_polygon_add_interior_ring(ogc_geometry_t* polygon, ogc_geometry_t* ring) {
    if (polygon && ring) {
        LinearRingPtr interiorRing(reinterpret_cast<LinearRing*>(ring));
        reinterpret_cast<Polygon*>(polygon)->AddInteriorRing(std::move(interiorRing));
    }
}

int ogc_polygon_is_valid(const ogc_geometry_t* polygon) {
    if (polygon) {
        return reinterpret_cast<const Polygon*>(polygon)->IsValid() ? 1 : 0;
    }
    return 0;
}

ogc_geometry_t* ogc_multipoint_create(void) {
    return reinterpret_cast<ogc_geometry_t*>(MultiPoint::Create().release());
}

size_t ogc_multipoint_get_num_geometries(const ogc_geometry_t* mp) {
    if (mp) {
        return reinterpret_cast<const MultiPoint*>(mp)->GetNumGeometries();
    }
    return 0;
}

ogc_geometry_t* ogc_multipoint_get_geometry_n(const ogc_geometry_t* mp, size_t index) {
    if (mp) {
        const Geometry* geom = reinterpret_cast<const MultiPoint*>(mp)->GetGeometryN(index);
        if (geom) {
            GeometryPtr cloned = geom->Clone();
            return reinterpret_cast<ogc_geometry_t*>(cloned.release());
        }
    }
    return nullptr;
}

void ogc_multipoint_add_geometry(ogc_geometry_t* mp, ogc_geometry_t* point) {
    if (mp && point) {
        PointPtr pt(reinterpret_cast<Point*>(point));
        reinterpret_cast<MultiPoint*>(mp)->AddPoint(std::move(pt));
    }
}

ogc_geometry_t* ogc_multilinestring_create(void) {
    return reinterpret_cast<ogc_geometry_t*>(MultiLineString::Create().release());
}

size_t ogc_multilinestring_get_num_geometries(const ogc_geometry_t* mls) {
    if (mls) {
        return reinterpret_cast<const MultiLineString*>(mls)->GetNumGeometries();
    }
    return 0;
}

ogc_geometry_t* ogc_multilinestring_get_geometry_n(const ogc_geometry_t* mls, size_t index) {
    if (mls) {
        const Geometry* geom = reinterpret_cast<const MultiLineString*>(mls)->GetGeometryN(index);
        if (geom) {
            GeometryPtr cloned = geom->Clone();
            return reinterpret_cast<ogc_geometry_t*>(cloned.release());
        }
    }
    return nullptr;
}

void ogc_multilinestring_add_geometry(ogc_geometry_t* mls, ogc_geometry_t* linestring) {
    if (mls && linestring) {
        LineStringPtr line(reinterpret_cast<LineString*>(linestring));
        reinterpret_cast<MultiLineString*>(mls)->AddLineString(std::move(line));
    }
}

ogc_geometry_t* ogc_multipolygon_create(void) {
    return reinterpret_cast<ogc_geometry_t*>(MultiPolygon::Create().release());
}

size_t ogc_multipolygon_get_num_geometries(const ogc_geometry_t* mp) {
    if (mp) {
        return reinterpret_cast<const MultiPolygon*>(mp)->GetNumGeometries();
    }
    return 0;
}

ogc_geometry_t* ogc_multipolygon_get_geometry_n(const ogc_geometry_t* mp, size_t index) {
    if (mp) {
        const Geometry* geom = reinterpret_cast<const MultiPolygon*>(mp)->GetGeometryN(index);
        if (geom) {
            GeometryPtr cloned = geom->Clone();
            return reinterpret_cast<ogc_geometry_t*>(cloned.release());
        }
    }
    return nullptr;
}

void ogc_multipolygon_add_geometry(ogc_geometry_t* mp, ogc_geometry_t* polygon) {
    if (mp && polygon) {
        PolygonPtr poly(reinterpret_cast<Polygon*>(polygon));
        reinterpret_cast<MultiPolygon*>(mp)->AddPolygon(std::move(poly));
    }
}

ogc_geometry_t* ogc_geometry_collection_create(void) {
    return reinterpret_cast<ogc_geometry_t*>(GeometryCollection::Create().release());
}

size_t ogc_geometry_collection_get_num_geometries(const ogc_geometry_t* gc) {
    if (gc) {
        return reinterpret_cast<const GeometryCollection*>(gc)->GetNumGeometries();
    }
    return 0;
}

ogc_geometry_t* ogc_geometry_collection_get_geometry_n(const ogc_geometry_t* gc, size_t index) {
    if (gc) {
        const Geometry* geom = reinterpret_cast<const GeometryCollection*>(gc)->GetGeometryN(index);
        if (geom) {
            GeometryPtr cloned = geom->Clone();
            return reinterpret_cast<ogc_geometry_t*>(cloned.release());
        }
    }
    return nullptr;
}

void ogc_geometry_collection_add_geometry(ogc_geometry_t* gc, ogc_geometry_t* geom) {
    if (gc && geom) {
        GeometryPtr g(reinterpret_cast<Geometry*>(geom));
        reinterpret_cast<GeometryCollection*>(gc)->AddGeometry(std::move(g));
    }
}

ogc_geometry_factory_t* ogc_geometry_factory_create(void) {
    return reinterpret_cast<ogc_geometry_factory_t*>(&GeometryFactory::GetInstance());
}

void ogc_geometry_factory_destroy(ogc_geometry_factory_t* factory) {
    // GeometryFactory is a singleton, no need to delete
}

ogc_geometry_t* ogc_geometry_factory_create_point(ogc_geometry_factory_t* factory, double x, double y) {
    if (factory) {
        GeometryPtr pt = reinterpret_cast<GeometryFactory*>(factory)->CreatePoint(x, y);
        return reinterpret_cast<ogc_geometry_t*>(pt.release());
    }
    return nullptr;
}

ogc_geometry_t* ogc_geometry_factory_create_linestring(ogc_geometry_factory_t* factory) {
    if (factory) {
        GeometryPtr line = LineString::Create();
        return reinterpret_cast<ogc_geometry_t*>(line.release());
    }
    return nullptr;
}

ogc_geometry_t* ogc_geometry_factory_create_polygon(ogc_geometry_factory_t* factory) {
    if (factory) {
        GeometryPtr poly = Polygon::Create();
        return reinterpret_cast<ogc_geometry_t*>(poly.release());
    }
    return nullptr;
}

ogc_geometry_t* ogc_geometry_factory_create_from_wkt(ogc_geometry_factory_t* factory, const char* wkt) {
    if (factory && wkt) {
        GeometryPtr geom;
        if (reinterpret_cast<GeometryFactory*>(factory)->FromWKT(std::string(wkt), geom) == GeomResult::kSuccess) {
            return reinterpret_cast<ogc_geometry_t*>(geom.release());
        }
    }
    return nullptr;
}

ogc_geometry_t* ogc_geometry_factory_create_from_wkb(ogc_geometry_factory_t* factory, const unsigned char* wkb, size_t size) {
    if (factory && wkb && size > 0) {
        std::vector<uint8_t> data(wkb, wkb + size);
        GeometryPtr geom;
        if (reinterpret_cast<GeometryFactory*>(factory)->FromWKB(data, geom) == GeomResult::kSuccess) {
            return reinterpret_cast<ogc_geometry_t*>(geom.release());
        }
    }
    return nullptr;
}

ogc_geometry_t* ogc_geometry_factory_create_from_geojson(ogc_geometry_factory_t* factory, const char* geojson) {
    if (factory && geojson) {
        GeometryPtr geom;
        if (reinterpret_cast<GeometryFactory*>(factory)->FromGeoJSON(std::string(geojson), geom) == GeomResult::kSuccess) {
            return reinterpret_cast<ogc_geometry_t*>(geom.release());
        }
    }
    return nullptr;
}

#ifdef __cplusplus
}
#endif
