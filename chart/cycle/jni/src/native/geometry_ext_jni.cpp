#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== MultiPoint ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_MultiPoint_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_geometry_t* mp = ogc_multipoint_create();
        return JniConverter::ToJLongPtr(mp);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_MultiPoint_nativeGetNumGeometries
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_geometry_t* mp =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mp) {
            JniException::ThrowNullPointerException(env, "MultiPoint is null");
            return 0;
        }
        return static_cast<jlong>(ogc_multipoint_get_num_geometries(mp));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_MultiPoint_nativeGetGeometryN
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    try {
        ogc_geometry_t* mp =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mp) {
            JniException::ThrowNullPointerException(env, "MultiPoint is null");
            return 0;
        }
        ogc_geometry_t* geom = ogc_multipoint_get_geometry_n(mp, static_cast<size_t>(index));
        return JniConverter::ToJLongPtr(geom);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_MultiPoint_nativeAddGeometry
  (JNIEnv* env, jobject obj, jlong ptr, jlong pointPtr) {
    try {
        ogc_geometry_t* mp =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mp) {
            JniException::ThrowNullPointerException(env, "MultiPoint is null");
            return;
        }
        ogc_geometry_t* point =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(pointPtr));
        if (!point) {
            JniException::ThrowNullPointerException(env, "Point is null");
            return;
        }
        ogc_multipoint_add_geometry(mp, point);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== MultiLineString ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_MultiLineString_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_geometry_t* mls = ogc_multilinestring_create();
        return JniConverter::ToJLongPtr(mls);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_MultiLineString_nativeGetNumGeometries
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_geometry_t* mls =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mls) {
            JniException::ThrowNullPointerException(env, "MultiLineString is null");
            return 0;
        }
        return static_cast<jlong>(ogc_multilinestring_get_num_geometries(mls));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_MultiLineString_nativeGetGeometryN
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    try {
        ogc_geometry_t* mls =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mls) {
            JniException::ThrowNullPointerException(env, "MultiLineString is null");
            return 0;
        }
        ogc_geometry_t* geom = ogc_multilinestring_get_geometry_n(mls, static_cast<size_t>(index));
        return JniConverter::ToJLongPtr(geom);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_MultiLineString_nativeAddGeometry
  (JNIEnv* env, jobject obj, jlong ptr, jlong lineStringPtr) {
    try {
        ogc_geometry_t* mls =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mls) {
            JniException::ThrowNullPointerException(env, "MultiLineString is null");
            return;
        }
        ogc_geometry_t* lineString =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(lineStringPtr));
        if (!lineString) {
            JniException::ThrowNullPointerException(env, "LineString is null");
            return;
        }
        ogc_multilinestring_add_geometry(mls, lineString);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== MultiPolygon ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_MultiPolygon_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_geometry_t* mp = ogc_multipolygon_create();
        return JniConverter::ToJLongPtr(mp);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_MultiPolygon_nativeGetNumGeometries
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_geometry_t* mp =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mp) {
            JniException::ThrowNullPointerException(env, "MultiPolygon is null");
            return 0;
        }
        return static_cast<jlong>(ogc_multipolygon_get_num_geometries(mp));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_MultiPolygon_nativeGetGeometryN
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    try {
        ogc_geometry_t* mp =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mp) {
            JniException::ThrowNullPointerException(env, "MultiPolygon is null");
            return 0;
        }
        ogc_geometry_t* geom = ogc_multipolygon_get_geometry_n(mp, static_cast<size_t>(index));
        return JniConverter::ToJLongPtr(geom);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_MultiPolygon_nativeAddGeometry
  (JNIEnv* env, jobject obj, jlong ptr, jlong polygonPtr) {
    try {
        ogc_geometry_t* mp =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (!mp) {
            JniException::ThrowNullPointerException(env, "MultiPolygon is null");
            return;
        }
        ogc_geometry_t* polygon =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(polygonPtr));
        if (!polygon) {
            JniException::ThrowNullPointerException(env, "Polygon is null");
            return;
        }
        ogc_multipolygon_add_geometry(mp, polygon);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== GeometryCollection ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_GeometryCollection_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_geometry_t* gc = ogc_geometry_collection_create();
        return JniConverter::ToJLongPtr(gc);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_GeometryCollection_nativeGetNumGeometries
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_geometry_t* gc =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (!gc) {
            JniException::ThrowNullPointerException(env, "GeometryCollection is null");
            return 0;
        }
        return static_cast<jlong>(ogc_geometry_collection_get_num_geometries(gc));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_GeometryCollection_nativeGetGeometryN
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    try {
        ogc_geometry_t* gc =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (!gc) {
            JniException::ThrowNullPointerException(env, "GeometryCollection is null");
            return 0;
        }
        ogc_geometry_t* geom = ogc_geometry_collection_get_geometry_n(gc, static_cast<size_t>(index));
        return JniConverter::ToJLongPtr(geom);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_GeometryCollection_nativeAddGeometry
  (JNIEnv* env, jobject obj, jlong ptr, jlong geomPtr) {
    try {
        ogc_geometry_t* gc =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(ptr));
        if (!gc) {
            JniException::ThrowNullPointerException(env, "GeometryCollection is null");
            return;
        }
        ogc_geometry_t* geom =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(geomPtr));
        if (!geom) {
            JniException::ThrowNullPointerException(env, "Geometry is null");
            return;
        }
        ogc_geometry_collection_add_geometry(gc, geom);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== GeometryFactory ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_geometry_factory_t* factory = ogc_geometry_factory_create();
        return JniConverter::ToJLongPtr(factory);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_geometry_factory_t* factory =
            static_cast<ogc_geometry_factory_t*>(JniConverter::FromJLongPtr(ptr));
        if (!factory) {
            return;
        }
        ogc_geometry_factory_destroy(factory);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeCreatePoint
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y) {
    try {
        ogc_geometry_factory_t* factory =
            static_cast<ogc_geometry_factory_t*>(JniConverter::FromJLongPtr(ptr));
        if (!factory) {
            JniException::ThrowNullPointerException(env, "GeometryFactory is null");
            return 0;
        }
        ogc_geometry_t* point = ogc_geometry_factory_create_point(factory, x, y);
        return JniConverter::ToJLongPtr(point);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeCreateLineString
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_geometry_factory_t* factory =
            static_cast<ogc_geometry_factory_t*>(JniConverter::FromJLongPtr(ptr));
        if (!factory) {
            JniException::ThrowNullPointerException(env, "GeometryFactory is null");
            return 0;
        }
        ogc_geometry_t* ls = ogc_geometry_factory_create_linestring(factory);
        return JniConverter::ToJLongPtr(ls);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeCreatePolygon
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_geometry_factory_t* factory =
            static_cast<ogc_geometry_factory_t*>(JniConverter::FromJLongPtr(ptr));
        if (!factory) {
            JniException::ThrowNullPointerException(env, "GeometryFactory is null");
            return 0;
        }
        ogc_geometry_t* polygon = ogc_geometry_factory_create_polygon(factory);
        return JniConverter::ToJLongPtr(polygon);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeCreateFromWkt
  (JNIEnv* env, jobject obj, jlong ptr, jstring wkt) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }

    try {
        ogc_geometry_factory_t* factory =
            static_cast<ogc_geometry_factory_t*>(JniConverter::FromJLongPtr(ptr));
        if (!factory) {
            JniException::ThrowNullPointerException(env, "GeometryFactory is null");
            return 0;
        }
        const char* wktStr = env->GetStringUTFChars(wkt, nullptr);
        if (!wktStr) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get string");
            return 0;
        }
        ogc_geometry_t* geom = ogc_geometry_factory_create_from_wkt(factory, wktStr);
        env->ReleaseStringUTFChars(wkt, wktStr);
        return JniConverter::ToJLongPtr(geom);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeCreateFromWkb
  (JNIEnv* env, jobject obj, jlong ptr, jbyteArray wkb) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }

    try {
        ogc_geometry_factory_t* factory =
            static_cast<ogc_geometry_factory_t*>(JniConverter::FromJLongPtr(ptr));
        if (!factory) {
            JniException::ThrowNullPointerException(env, "GeometryFactory is null");
            return 0;
        }
        jsize size = env->GetArrayLength(wkb);
        jbyte* bytes = env->GetByteArrayElements(wkb, nullptr);
        if (!bytes) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get byte array");
            return 0;
        }
        ogc_geometry_t* geom = ogc_geometry_factory_create_from_wkb(
            factory, reinterpret_cast<const unsigned char*>(bytes), static_cast<size_t>(size));
        env->ReleaseByteArrayElements(wkb, bytes, JNI_ABORT);
        return JniConverter::ToJLongPtr(geom);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_geometry_GeometryFactory_nativeCreateFromGeoJson
  (JNIEnv* env, jobject obj, jlong ptr, jstring geojson) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }

    try {
        ogc_geometry_factory_t* factory =
            static_cast<ogc_geometry_factory_t*>(JniConverter::FromJLongPtr(ptr));
        if (!factory) {
            JniException::ThrowNullPointerException(env, "GeometryFactory is null");
            return 0;
        }
        const char* jsonStr = env->GetStringUTFChars(geojson, nullptr);
        if (!jsonStr) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get string");
            return 0;
        }
        ogc_geometry_t* geom = ogc_geometry_factory_create_from_geojson(factory, jsonStr);
        env->ReleaseStringUTFChars(geojson, jsonStr);
        return JniConverter::ToJLongPtr(geom);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

}
