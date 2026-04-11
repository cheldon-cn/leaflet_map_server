#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

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

JNIEXPORT jdoubleArray JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeGetEnvelope
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (!geom) {
        return nullptr;
    }
    ogc_envelope_t* env_ = ogc_geometry_get_envelope(geom);
    if (!env_) {
        return nullptr;
    }
    jdoubleArray result = env->NewDoubleArray(4);
    if (result) {
        jdouble vals[4] = { 
            ogc_envelope_get_min_x(env_),
            ogc_envelope_get_min_y(env_),
            ogc_envelope_get_max_x(env_),
            ogc_envelope_get_max_y(env_)
        };
        env->SetDoubleArrayRegion(result, 0, 4, vals);
    }
    ogc_envelope_destroy(env_);
    return result;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeGetNumPoints
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return geom ? static_cast<jlong>(ogc_geometry_get_num_points(geom)) : 0;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeIsSimple
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return geom ? (ogc_geometry_is_simple(geom) ? JNI_TRUE : JNI_FALSE) : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeIs3D
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return geom ? (ogc_geometry_is_3d(geom) ? JNI_TRUE : JNI_FALSE) : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeIsMeasured
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return geom ? (ogc_geometry_is_measured(geom) ? JNI_TRUE : JNI_FALSE) : JNI_FALSE;
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

JNIEXPORT jdoubleArray JNICALL
Java_cn_cycle_chart_api_geometry_LineString_nativeGetPointN
  (JNIEnv* env, jobject obj, jlong ptr, jint index) {
    ogc_geometry_t* line =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (!line) {
        return nullptr;
    }

    ogc_coordinate_t coord = ogc_linestring_get_point_n(line, static_cast<size_t>(index));
    jdoubleArray result = env->NewDoubleArray(2);
    if (result) {
        jdouble vals[2] = { coord.x, coord.y };
        env->SetDoubleArrayRegion(result, 0, 2, vals);
    }
    return result;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_LineString_nativeGetLength
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_geometry_t* line =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return line ? ogc_geometry_get_length(line) : 0.0;
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

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Polygon_nativeGetExteriorRing
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_geometry_t* polygon =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (!polygon) {
        return 0;
    }
    ogc_geometry_t* ring = ogc_polygon_get_exterior_ring(polygon);
    return JniConverter::ToJLongPtr(ring);
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Polygon_nativeGetInteriorRingN
  (JNIEnv* env, jobject obj, jlong ptr, jint index) {
    ogc_geometry_t* polygon =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (!polygon) {
        return 0;
    }
    ogc_geometry_t* ring = ogc_polygon_get_interior_ring_n(polygon, static_cast<size_t>(index));
    return JniConverter::ToJLongPtr(ring);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_Polygon_nativeAddInteriorRing
  (JNIEnv* env, jobject obj, jlong ptr, jlong ringPtr) {
    ogc_geometry_t* polygon =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_geometry_t* ring =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ringPtr));
    if (polygon && ring) {
        ogc_polygon_add_interior_ring(polygon, ring);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Polygon_nativeGetArea
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_geometry_t* polygon =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return polygon ? ogc_geometry_get_area(polygon) : 0.0;
}

JNIEXPORT jdoubleArray JNICALL
Java_cn_cycle_chart_api_geometry_Polygon_nativeGetCentroid
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_geometry_t* polygon =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (!polygon) {
        return nullptr;
    }

    ogc_coordinate_t coord = ogc_geometry_get_centroid(polygon);
    jdoubleArray result = env->NewDoubleArray(2);
    if (result) {
        jdouble vals[2] = { coord.x, coord.y };
        env->SetDoubleArrayRegion(result, 0, 2, vals);
    }
    return result;
}

/* ===== LinearRing ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_LinearRing_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_geometry_t* ring = ogc_linearring_create();
        return JniConverter::ToJLongPtr(ring);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_LinearRing_nativeIsClosed
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_geometry_t* ring =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return ring ? (ogc_linearring_is_closed(ring) ? JNI_TRUE : JNI_FALSE) : JNI_FALSE;
}

}
