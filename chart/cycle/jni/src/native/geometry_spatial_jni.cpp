#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"
#include <vector>

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

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Coordinate_nativeCreateM
  (JNIEnv* env, jclass clazz, jdouble x, jdouble y, jdouble z, jdouble m) {
    try {
        ogc_coordinate_t* coord = ogc_coordinate_create_m(x, y, z, m);
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
Java_cn_cycle_chart_api_geometry_Coordinate_nativeGetM
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_coordinate_t* coord =
        static_cast<ogc_coordinate_t*>(JniConverter::FromJLongPtr(ptr));
    return coord ? coord->m : 0.0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Coordinate_nativeDistance
  (JNIEnv* env, jclass clazz, jlong ptrA, jlong ptrB) {
    ogc_coordinate_t* a =
        static_cast<ogc_coordinate_t*>(JniConverter::FromJLongPtr(ptrA));
    ogc_coordinate_t* b =
        static_cast<ogc_coordinate_t*>(JniConverter::FromJLongPtr(ptrB));
    if (!a || !b) {
        JniException::ThrowNullPointerException(env, "Coordinate is null");
        return 0.0;
    }
    return ogc_coordinate_distance(a, b);
}

/* ===== Envelope Extended ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeCreateFromCoords
  (JNIEnv* env, jclass clazz, jdouble minX, jdouble minY, jdouble maxX, jdouble maxY) {
    try {
        ogc_envelope_t* env_ = ogc_envelope_create_from_coords(minX, minY, maxX, maxY);
        return JniConverter::ToJLongPtr(env_);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
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

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeGetArea
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_envelope_t* env_ =
        static_cast<ogc_envelope_t*>(JniConverter::FromJLongPtr(ptr));
    return env_ ? ogc_envelope_get_area(env_) : 0.0;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeContains
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y) {
    ogc_envelope_t* env_ =
        static_cast<ogc_envelope_t*>(JniConverter::FromJLongPtr(ptr));
    return env_ ? (ogc_envelope_contains(env_, x, y) ? JNI_TRUE : JNI_FALSE) : JNI_FALSE;
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

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeExpand
  (JNIEnv* env, jobject obj, jlong ptr, jdouble dx, jdouble dy) {
    ogc_envelope_t* env_ =
        static_cast<ogc_envelope_t*>(JniConverter::FromJLongPtr(ptr));
    if (env_) {
        ogc_envelope_expand(env_, dx, dy);
    }
}

JNIEXPORT jdoubleArray JNICALL
Java_cn_cycle_chart_api_geometry_Envelope_nativeGetCenter
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_envelope_t* env_ =
        static_cast<ogc_envelope_t*>(JniConverter::FromJLongPtr(ptr));
    if (!env_) {
        return nullptr;
    }
    ogc_coordinate_t center = ogc_envelope_get_center(env_);
    jdoubleArray result = env->NewDoubleArray(2);
    if (result) {
        jdouble vals[2] = { center.x, center.y };
        env->SetDoubleArrayRegion(result, 0, 2, vals);
    }
    return result;
}

/* ===== Geometry Spatial Operations ===== */

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeEquals
  (JNIEnv* env, jclass clazz, jlong ptrA, jlong ptrB, jdouble tolerance) {
    ogc_geometry_t* a =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrA));
    ogc_geometry_t* b =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrB));
    if (!a || !b) {
        return JNI_FALSE;
    }
    return ogc_geometry_equals(a, b, tolerance) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeIntersects
  (JNIEnv* env, jclass clazz, jlong ptrA, jlong ptrB) {
    ogc_geometry_t* a =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrA));
    ogc_geometry_t* b =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrB));
    if (!a || !b) {
        return JNI_FALSE;
    }
    return ogc_geometry_intersects(a, b) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeContains
  (JNIEnv* env, jclass clazz, jlong ptrA, jlong ptrB) {
    ogc_geometry_t* a =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrA));
    ogc_geometry_t* b =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrB));
    if (!a || !b) {
        return JNI_FALSE;
    }
    return ogc_geometry_contains(a, b) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeWithin
  (JNIEnv* env, jclass clazz, jlong ptrA, jlong ptrB) {
    ogc_geometry_t* a =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrA));
    ogc_geometry_t* b =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrB));
    if (!a || !b) {
        return JNI_FALSE;
    }
    return ogc_geometry_within(a, b) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeCrosses
  (JNIEnv* env, jclass clazz, jlong ptrA, jlong ptrB) {
    ogc_geometry_t* a =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrA));
    ogc_geometry_t* b =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrB));
    if (!a || !b) {
        return JNI_FALSE;
    }
    return ogc_geometry_crosses(a, b) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeTouches
  (JNIEnv* env, jclass clazz, jlong ptrA, jlong ptrB) {
    ogc_geometry_t* a =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrA));
    ogc_geometry_t* b =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrB));
    if (!a || !b) {
        return JNI_FALSE;
    }
    return ogc_geometry_touches(a, b) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeOverlaps
  (JNIEnv* env, jclass clazz, jlong ptrA, jlong ptrB) {
    ogc_geometry_t* a =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrA));
    ogc_geometry_t* b =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrB));
    if (!a || !b) {
        return JNI_FALSE;
    }
    return ogc_geometry_overlaps(a, b) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeDistance
  (JNIEnv* env, jclass clazz, jlong ptrA, jlong ptrB) {
    ogc_geometry_t* a =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrA));
    ogc_geometry_t* b =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrB));
    if (!a || !b) {
        return -1.0;
    }
    return ogc_geometry_distance(a, b);
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeIntersection
  (JNIEnv* env, jclass clazz, jlong ptrA, jlong ptrB) {
    try {
        ogc_geometry_t* a =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrA));
        ogc_geometry_t* b =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrB));
        if (!a || !b) {
            JniException::ThrowNullPointerException(env, "Geometry is null");
            return 0;
        }
        ogc_geometry_t* result = ogc_geometry_intersection(a, b);
        return JniConverter::ToJLongPtr(result);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeUnion
  (JNIEnv* env, jclass clazz, jlong ptrA, jlong ptrB) {
    try {
        ogc_geometry_t* a =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrA));
        ogc_geometry_t* b =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrB));
        if (!a || !b) {
            JniException::ThrowNullPointerException(env, "Geometry is null");
            return 0;
        }
        ogc_geometry_t* result = ogc_geometry_union(a, b);
        return JniConverter::ToJLongPtr(result);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeDifference
  (JNIEnv* env, jclass clazz, jlong ptrA, jlong ptrB) {
    try {
        ogc_geometry_t* a =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrA));
        ogc_geometry_t* b =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptrB));
        if (!a || !b) {
            JniException::ThrowNullPointerException(env, "Geometry is null");
            return 0;
        }
        ogc_geometry_t* result = ogc_geometry_difference(a, b);
        return JniConverter::ToJLongPtr(result);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeBuffer
  (JNIEnv* env, jclass clazz, jlong ptr, jdouble distance, jint segments) {
    try {
        ogc_geometry_t* geom =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (!geom) {
            JniException::ThrowNullPointerException(env, "Geometry is null");
            return 0;
        }
        ogc_geometry_t* result = ogc_geometry_buffer(geom, distance, segments);
        return JniConverter::ToJLongPtr(result);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeClone
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_geometry_t* geom =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (!geom) {
            JniException::ThrowNullPointerException(env, "Geometry is null");
            return 0;
        }
        ogc_geometry_t* result = ogc_geometry_clone(geom);
        return JniConverter::ToJLongPtr(result);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_geometry_Geometry_nativeGetTypeName
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (!geom) {
        return nullptr;
    }
    const char* typeName = ogc_geometry_get_type_name(geom);
    if (!typeName) {
        return nullptr;
    }
    return env->NewStringUTF(typeName);
}

/* ===== Point Extended ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Point_nativeCreateFromCoord
  (JNIEnv* env, jclass clazz, jlong coordPtr) {
    try {
        ogc_coordinate_t* coord =
            static_cast<ogc_coordinate_t*>(JniConverter::FromJLongPtr(coordPtr));
        if (!coord) {
            JniException::ThrowNullPointerException(env, "Coordinate is null");
            return 0;
        }
        ogc_geometry_t* point = ogc_point_create_from_coord(coord);
        return JniConverter::ToJLongPtr(point);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_geometry_Point_nativeGetM
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_geometry_t* point =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return point ? ogc_point_get_m(point) : 0.0;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_Point_nativeSetM
  (JNIEnv* env, jobject obj, jlong ptr, jdouble m) {
    ogc_geometry_t* point =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (point) {
        ogc_point_set_m(point, m);
    }
}

JNIEXPORT jdoubleArray JNICALL
Java_cn_cycle_chart_api_geometry_Point_nativeGetCoordinate
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_geometry_t* point =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (!point) {
        return nullptr;
    }
    ogc_coordinate_t coord = ogc_point_get_coordinate(point);
    jdoubleArray result = env->NewDoubleArray(4);
    if (result) {
        jdouble vals[4] = { coord.x, coord.y, coord.z, coord.m };
        env->SetDoubleArrayRegion(result, 0, 4, vals);
    }
    return result;
}

/* ===== LineString Extended ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_LineString_nativeCreateFromCoords
  (JNIEnv* env, jclass clazz, jdoubleArray coords) {
    try {
        jsize len = env->GetArrayLength(coords);
        if (len < 2 || len % 2 != 0) {
            JniException::ThrowIllegalArgumentException(env, "Coords array must have even length >= 2");
            return 0;
        }
        jdouble* elems = env->GetDoubleArrayElements(coords, nullptr);
        if (!elems) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get double array");
            return 0;
        }
        size_t count = static_cast<size_t>(len / 2);
        std::vector<ogc_coordinate_t> coordVec(count);
        for (size_t i = 0; i < count; ++i) {
            coordVec[i].x = elems[i * 2];
            coordVec[i].y = elems[i * 2 + 1];
            coordVec[i].z = 0.0;
            coordVec[i].m = 0.0;
        }
        env->ReleaseDoubleArrayElements(coords, elems, JNI_ABORT);
        ogc_geometry_t* line = ogc_linestring_create_from_coords(coordVec.data(), count);
        return JniConverter::ToJLongPtr(line);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_LineString_nativeAddPoint3D
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y, jdouble z) {
    ogc_geometry_t* line =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (line) {
        ogc_linestring_add_point_3d(line, x, y, z);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_LineString_nativeSetPointN
  (JNIEnv* env, jobject obj, jlong ptr, jint index, jdouble x, jdouble y) {
    ogc_geometry_t* line =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (line) {
        ogc_coordinate_t coord;
        coord.x = x;
        coord.y = y;
        coord.z = 0.0;
        coord.m = 0.0;
        ogc_linestring_set_point_n(line, static_cast<size_t>(index), &coord);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_LineString_nativeGetPointGeometry
  (JNIEnv* env, jobject obj, jlong ptr, jint index) {
    ogc_geometry_t* line =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    if (!line) {
        return 0;
    }
    ogc_geometry_t* pt = ogc_linestring_get_point_geometry(line, static_cast<size_t>(index));
    return JniConverter::ToJLongPtr(pt);
}

/* ===== LinearRing Extended ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_LinearRing_nativeCreateFromCoords
  (JNIEnv* env, jclass clazz, jdoubleArray coords) {
    try {
        jsize len = env->GetArrayLength(coords);
        if (len < 4 || len % 2 != 0) {
            JniException::ThrowIllegalArgumentException(env, "Coords array must have even length >= 4");
            return 0;
        }
        jdouble* elems = env->GetDoubleArrayElements(coords, nullptr);
        if (!elems) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get double array");
            return 0;
        }
        size_t count = static_cast<size_t>(len / 2);
        std::vector<ogc_coordinate_t> coordVec(count);
        for (size_t i = 0; i < count; ++i) {
            coordVec[i].x = elems[i * 2];
            coordVec[i].y = elems[i * 2 + 1];
            coordVec[i].z = 0.0;
            coordVec[i].m = 0.0;
        }
        env->ReleaseDoubleArrayElements(coords, elems, JNI_ABORT);
        ogc_geometry_t* ring = ogc_linearring_create_from_coords(coordVec.data(), count);
        return JniConverter::ToJLongPtr(ring);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

/* ===== Polygon Extended ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Polygon_nativeCreateFromRing
  (JNIEnv* env, jclass clazz, jlong ringPtr) {
    try {
        ogc_geometry_t* ring =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ringPtr));
        if (!ring) {
            JniException::ThrowNullPointerException(env, "Ring is null");
            return 0;
        }
        ogc_geometry_t* polygon = ogc_polygon_create_from_ring(ring);
        return JniConverter::ToJLongPtr(polygon);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_Polygon_nativeCreateFromCoords
  (JNIEnv* env, jclass clazz, jdoubleArray coords) {
    try {
        jsize len = env->GetArrayLength(coords);
        if (len < 4 || len % 2 != 0) {
            JniException::ThrowIllegalArgumentException(env, "Coords array must have even length >= 4");
            return 0;
        }
        jdouble* elems = env->GetDoubleArrayElements(coords, nullptr);
        if (!elems) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get double array");
            return 0;
        }
        size_t count = static_cast<size_t>(len / 2);
        std::vector<ogc_coordinate_t> coordVec(count);
        for (size_t i = 0; i < count; ++i) {
            coordVec[i].x = elems[i * 2];
            coordVec[i].y = elems[i * 2 + 1];
            coordVec[i].z = 0.0;
            coordVec[i].m = 0.0;
        }
        env->ReleaseDoubleArrayElements(coords, elems, JNI_ABORT);
        ogc_geometry_t* polygon = ogc_polygon_create_from_coords(coordVec.data(), count);
        return JniConverter::ToJLongPtr(polygon);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_geometry_Polygon_nativeIsValid
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_geometry_t* polygon =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
    return polygon ? (ogc_polygon_is_valid(polygon) ? JNI_TRUE : JNI_FALSE) : JNI_FALSE;
}

}
