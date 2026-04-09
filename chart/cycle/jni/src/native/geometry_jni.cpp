#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== Coordinate ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Coordinate_nativeCreate
  (JNIEnv* env, jclass clazz, jdouble x, jdouble y) {
    try {
        ogc_coordinate_t* coord = ogc_coordinate_create(x, y);
        return JniConverter::ToJLongPtr(coord);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Coordinate_nativeCreate3D
  (JNIEnv* env, jclass clazz, jdouble x, jdouble y, jdouble z) {
    try {
        ogc_coordinate_t* coord = ogc_coordinate_create_3d(x, y, z);
        return JniConverter::ToJLongPtr(coord);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_Coordinate_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_coordinate_t* coord =
            static_cast<ogc_coordinate_t*>(JniConverter::FromJLongPtr(ptr));
        if (coord) {
            ogc_coordinate_destroy(coord);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Coordinate_nativeGetX
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_coordinate_t* coord =
        static_cast<ogc_coordinate_t*>(JniConverter::FromJLongPtr(ptr));
    return coord ? coord->x : 0.0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Coordinate_nativeGetY
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_coordinate_t* coord =
        static_cast<ogc_coordinate_t*>(JniConverter::FromJLongPtr(ptr));
    return coord ? coord->y : 0.0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Coordinate_nativeGetZ
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_coordinate_t* coord =
        static_cast<ogc_coordinate_t*>(JniConverter::FromJLongPtr(ptr));
    return coord ? coord->z : 0.0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Coordinate_nativeDistance
  (JNIEnv* env, jclass clazz, jlong ptrA, jlong ptrB) {
    ogc_coordinate_t* a =
        static_cast<ogc_coordinate_t*>(JniConverter::FromJLongPtr(ptrA));
    ogc_coordinate_t* b =
        static_cast<ogc_coordinate_t*>(JniConverter::FromJLongPtr(ptrB));
    if (!a || !b) {
        return 0.0;
    }
    return ogc_coordinate_distance(a, b);
}

/* ===== Envelope ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeCreate
  (JNIEnv* env, jclass clazz, jdouble minX, jdouble minY, jdouble maxX, jdouble maxY) {
    try {
        ogc_envelope_t* env_ = ogc_envelope_create_from_coords(minX, minY, maxX, maxY);
        return JniConverter::ToJLongPtr(env_);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_envelope_t* env_ =
            static_cast<ogc_envelope_t*>(JniConverter::FromJLongPtr(ptr));
        if (env_) {
            ogc_envelope_destroy(env_);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeGetMinX
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_envelope_t* env_ =
        static_cast<ogc_envelope_t*>(JniConverter::FromJLongPtr(ptr));
    return env_ ? ogc_envelope_get_min_x(env_) : 0.0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeGetMinY
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_envelope_t* env_ =
        static_cast<ogc_envelope_t*>(JniConverter::FromJLongPtr(ptr));
    return env_ ? ogc_envelope_get_min_y(env_) : 0.0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeGetMaxX
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_envelope_t* env_ =
        static_cast<ogc_envelope_t*>(JniConverter::FromJLongPtr(ptr));
    return env_ ? ogc_envelope_get_max_x(env_) : 0.0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeGetMaxY
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_envelope_t* env_ =
        static_cast<ogc_envelope_t*>(JniConverter::FromJLongPtr(ptr));
    return env_ ? ogc_envelope_get_max_y(env_) : 0.0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeGetWidth
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_envelope_t* env_ =
        static_cast<ogc_envelope_t*>(JniConverter::FromJLongPtr(ptr));
    return env_ ? ogc_envelope_get_width(env_) : 0.0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeGetHeight
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_envelope_t* env_ =
        static_cast<ogc_envelope_t*>(JniConverter::FromJLongPtr(ptr));
    return env_ ? ogc_envelope_get_height(env_) : 0.0;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeContains
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y) {
    ogc_envelope_t* env_ =
        static_cast<ogc_envelope_t*>(JniConverter::FromJLongPtr(ptr));
    if (!env_) {
        return JNI_FALSE;
    }
    return ogc_envelope_contains(env_, x, y) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeIntersects
  (JNIEnv* env, jobject obj, jlong ptr, jlong otherPtr) {
    ogc_envelope_t* env_ =
        static_cast<ogc_envelope_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_envelope_t* other =
        static_cast<ogc_envelope_t*>(JniConverter::FromJLongPtr(otherPtr));
    if (!env_ || !other) {
        return JNI_FALSE;
    }
    return ogc_envelope_intersects(env_, other) ? JNI_TRUE : JNI_FALSE;
}

/* ===== Geometry (Point) ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Point_nativeCreate
  (JNIEnv* env, jclass clazz, jdouble x, jdouble y) {
    try {
        ogc_geometry_t* point = ogc_point_create(x, y);
        return JniConverter::ToJLongPtr(point);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Point_nativeCreate3D
  (JNIEnv* env, jclass clazz, jdouble x, jdouble y, jdouble z) {
    try {
        ogc_geometry_t* point = ogc_point_create_3d(x, y, z);
        return JniConverter::ToJLongPtr(point);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Point_nativeGetX
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_geometry_t* point =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return point ? ogc_point_get_x(point) : 0.0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Point_nativeGetY
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_geometry_t* point =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return point ? ogc_point_get_y(point) : 0.0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Point_nativeGetZ
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_geometry_t* point =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return point ? ogc_point_get_z(point) : 0.0;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_Point_nativeSetX
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x) {
    ogc_geometry_t* point =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (point) {
        ogc_point_set_x(point, x);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_Point_nativeSetY
  (JNIEnv* env, jobject obj, jlong ptr, jdouble y) {
    ogc_geometry_t* point =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (point) {
        ogc_point_set_y(point, y);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_Point_nativeSetZ
  (JNIEnv* env, jobject obj, jlong ptr, jdouble z) {
    ogc_geometry_t* point =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (point) {
        ogc_point_set_z(point, z);
    }
}

/* ===== Geometry (common) ===== */

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_geometry_t* geom =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (geom) {
            ogc_geometry_destroy(geom);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeGetType
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (!geom) {
        return 0;
    }
    return static_cast<jint>(ogc_geometry_get_type(geom));
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeGetDimension
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return geom ? ogc_geometry_get_dimension(geom) : 0;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeIsEmpty
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return geom ? (ogc_geometry_is_empty(geom) ? JNI_TRUE : JNI_FALSE) : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeIsValid
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return geom ? (ogc_geometry_is_valid(geom) ? JNI_TRUE : JNI_FALSE) : JNI_FALSE;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeGetSRID
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return geom ? ogc_geometry_get_srid(geom) : 0;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeSetSRID
  (JNIEnv* env, jclass clazz, jlong ptr, jint srid) {
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (geom) {
        ogc_geometry_set_srid(geom, srid);
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeAsText
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (!geom) {
        return nullptr;
    }

    char* wkt = ogc_geometry_as_text(geom, 8);
    if (!wkt) {
        return nullptr;
    }

    jstring result = env->NewStringUTF(wkt);
    ogc_geometry_free_string(wkt);
    return result;
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeAsGeoJSON
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (!geom) {
        return nullptr;
    }

    char* geojson = ogc_geometry_as_geojson(geom, 8);
    if (!geojson) {
        return nullptr;
    }

    jstring result = env->NewStringUTF(geojson);
    ogc_geometry_free_string(geojson);
    return result;
}

/* ===== LineString ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_LineString_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_geometry_t* line = ogc_linestring_create();
        return JniConverter::ToJLongPtr(line);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_LineString_nativeGetNumPoints
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_geometry_t* line =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return line ? static_cast<jlong>(ogc_linestring_get_num_points(line)) : 0;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_LineString_nativeAddPoint
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y) {
    ogc_geometry_t* line =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (line) {
        ogc_linestring_add_point(line, x, y);
    }
}

/* ===== Polygon ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Polygon_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_geometry_t* polygon = ogc_polygon_create();
        return JniConverter::ToJLongPtr(polygon);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Polygon_nativeGetNumInteriorRings
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_geometry_t* polygon =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return polygon ? static_cast<jlong>(ogc_polygon_get_num_interior_rings(polygon)) : 0;
}

}
