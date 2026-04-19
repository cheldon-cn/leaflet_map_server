#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== FieldValue ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_FieldValue_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_field_value_t* val = ogc_field_value_create();
        return JniConverter::ToJLongPtr(val);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FieldValue_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_field_value_t* val =
            static_cast<ogc_field_value_t*>(JniConverter::FromJLongPtr(ptr));
        if (val) {
            ogc_field_value_destroy(val);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_feature_FieldValue_nativeIsNull
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_field_value_t* val =
        static_cast<ogc_field_value_t*>(JniConverter::FromJLongPtr(ptr));
    return val ? (ogc_field_value_is_null(val) ? JNI_TRUE : JNI_FALSE) : JNI_TRUE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_feature_FieldValue_nativeIsSet
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_field_value_t* val =
        static_cast<ogc_field_value_t*>(JniConverter::FromJLongPtr(ptr));
    return val ? (ogc_field_value_is_set(val) ? JNI_TRUE : JNI_FALSE) : JNI_FALSE;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_feature_FieldValue_nativeGetAsInteger
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_field_value_t* val =
        static_cast<ogc_field_value_t*>(JniConverter::FromJLongPtr(ptr));
    return val ? ogc_field_value_get_as_integer(val) : 0;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_FieldValue_nativeGetAsInteger64
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_field_value_t* val =
        static_cast<ogc_field_value_t*>(JniConverter::FromJLongPtr(ptr));
    return val ? ogc_field_value_get_as_integer64(val) : 0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_feature_FieldValue_nativeGetAsReal
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_field_value_t* val =
        static_cast<ogc_field_value_t*>(JniConverter::FromJLongPtr(ptr));
    return val ? ogc_field_value_get_as_real(val) : 0.0;
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_feature_FieldValue_nativeGetAsString
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_field_value_t* val =
        static_cast<ogc_field_value_t*>(JniConverter::FromJLongPtr(ptr));
    if (!val) {
        return nullptr;
    }
    const char* str = ogc_field_value_get_as_string(val);
    return str ? env->NewStringUTF(str) : nullptr;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FieldValue_nativeSetInteger
  (JNIEnv* env, jobject obj, jlong ptr, jint v) {
    ogc_field_value_t* val =
        static_cast<ogc_field_value_t*>(JniConverter::FromJLongPtr(ptr));
    if (val) {
        ogc_field_value_set_integer(val, v);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FieldValue_nativeSetInteger64
  (JNIEnv* env, jobject obj, jlong ptr, jlong v) {
    ogc_field_value_t* val =
        static_cast<ogc_field_value_t*>(JniConverter::FromJLongPtr(ptr));
    if (val) {
        ogc_field_value_set_integer64(val, v);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FieldValue_nativeSetReal
  (JNIEnv* env, jobject obj, jlong ptr, jdouble v) {
    ogc_field_value_t* val =
        static_cast<ogc_field_value_t*>(JniConverter::FromJLongPtr(ptr));
    if (val) {
        ogc_field_value_set_real(val, v);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FieldValue_nativeSetString
  (JNIEnv* env, jobject obj, jlong ptr, jstring v) {
    ogc_field_value_t* val =
        static_cast<ogc_field_value_t*>(JniConverter::FromJLongPtr(ptr));
    if (!val || !v) {
        return;
    }
    const char* cv = env->GetStringUTFChars(v, nullptr);
    if (cv) {
        ogc_field_value_set_string(val, cv);
        env->ReleaseStringUTFChars(v, cv);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FieldValue_nativeSetNull
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_field_value_t* val =
        static_cast<ogc_field_value_t*>(JniConverter::FromJLongPtr(ptr));
    if (val) {
        ogc_field_value_set_null(val);
    }
}

/* ===== FeatureDefn ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_FeatureDefn_nativeCreate
  (JNIEnv* env, jclass clazz, jstring name) {
    try {
        const char* cname = env->GetStringUTFChars(name, nullptr);
        if (!cname) {
            JniException::ThrowNullPointerException(env, "Name is null");
            return 0;
        }
        ogc_feature_defn_t* defn = ogc_feature_defn_create(cname);
        env->ReleaseStringUTFChars(name, cname);
        return JniConverter::ToJLongPtr(defn);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FeatureDefn_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_feature_defn_t* defn =
            static_cast<ogc_feature_defn_t*>(JniConverter::FromJLongPtr(ptr));
        if (defn) {
            ogc_feature_defn_destroy(defn);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_feature_FeatureDefn_nativeGetName
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_feature_defn_t* defn =
        static_cast<ogc_feature_defn_t*>(JniConverter::FromJLongPtr(ptr));
    if (!defn) {
        return nullptr;
    }
    const char* name = ogc_feature_defn_get_name(defn);
    return name ? env->NewStringUTF(name) : nullptr;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_FeatureDefn_nativeGetFieldCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_feature_defn_t* defn =
        static_cast<ogc_feature_defn_t*>(JniConverter::FromJLongPtr(ptr));
    return defn ? static_cast<jlong>(ogc_feature_defn_get_field_count(defn)) : 0;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_FeatureDefn_nativeGetFieldDefn
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_feature_defn_t* defn =
        static_cast<ogc_feature_defn_t*>(JniConverter::FromJLongPtr(ptr));
    if (!defn) {
        return 0;
    }
    ogc_field_defn_t* field = ogc_feature_defn_get_field_defn(defn, static_cast<size_t>(index));
    return JniConverter::ToJLongPtr(field);
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_feature_FeatureDefn_nativeGetFieldIndex
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    ogc_feature_defn_t* defn =
        static_cast<ogc_feature_defn_t*>(JniConverter::FromJLongPtr(ptr));
    if (!defn || !name) {
        return -1;
    }
    const char* cname = env->GetStringUTFChars(name, nullptr);
    if (!cname) {
        return -1;
    }
    int idx = ogc_feature_defn_get_field_index(defn, cname);
    env->ReleaseStringUTFChars(name, cname);
    return idx;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FeatureDefn_nativeAddFieldDefn
  (JNIEnv* env, jobject obj, jlong ptr, jlong fieldPtr) {
    ogc_feature_defn_t* defn =
        static_cast<ogc_feature_defn_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_field_defn_t* field =
        static_cast<ogc_field_defn_t*>(JniConverter::FromJLongPtr(fieldPtr));
    if (defn && field) {
        ogc_feature_defn_add_field_defn(defn, field);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_FeatureDefn_nativeGetGeomFieldCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_feature_defn_t* defn =
        static_cast<ogc_feature_defn_t*>(JniConverter::FromJLongPtr(ptr));
    return defn ? static_cast<jlong>(ogc_feature_defn_get_geom_field_count(defn)) : 0;
}

/* ===== Feature Extended ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_Feature_nativeCreateWithDefn
  (JNIEnv* env, jclass clazz, jlong defnPtr) {
    try {
        ogc_feature_defn_t* defn =
            static_cast<ogc_feature_defn_t*>(JniConverter::FromJLongPtr(defnPtr));
        if (!defn) {
            JniException::ThrowNullPointerException(env, "FeatureDefn is null");
            return 0;
        }
        ogc_feature_t* feature = ogc_feature_create(defn);
        return JniConverter::ToJLongPtr(feature);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_Feature_nativeGetDefn
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature) {
        return 0;
    }
    ogc_feature_defn_t* defn = ogc_feature_get_defn(feature);
    return JniConverter::ToJLongPtr(defn);
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_feature_Feature_nativeIsFieldSet
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    return feature ? (ogc_feature_is_field_set(feature, static_cast<size_t>(index)) ? JNI_TRUE : JNI_FALSE) : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_feature_Feature_nativeIsFieldNull
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    return feature ? (ogc_feature_is_field_null(feature, static_cast<size_t>(index)) ? JNI_TRUE : JNI_FALSE) : JNI_TRUE;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_feature_Feature_nativeGetFieldAsIntegerByName
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature || !name) {
        return 0;
    }
    const char* cname = env->GetStringUTFChars(name, nullptr);
    if (!cname) {
        return 0;
    }
    int val = ogc_feature_get_field_as_integer_by_name(feature, cname);
    env->ReleaseStringUTFChars(name, cname);
    return val;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_feature_Feature_nativeGetFieldAsRealByName
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature || !name) {
        return 0.0;
    }
    const char* cname = env->GetStringUTFChars(name, nullptr);
    if (!cname) {
        return 0.0;
    }
    double val = ogc_feature_get_field_as_real_by_name(feature, cname);
    env->ReleaseStringUTFChars(name, cname);
    return val;
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_feature_Feature_nativeGetFieldAsStringByName
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature || !name) {
        return nullptr;
    }
    const char* cname = env->GetStringUTFChars(name, nullptr);
    if (!cname) {
        return nullptr;
    }
    const char* val = ogc_feature_get_field_as_string_by_name(feature, cname);
    env->ReleaseStringUTFChars(name, cname);
    return val ? env->NewStringUTF(val) : nullptr;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_feature_Feature_nativeSetFieldIntegerByName
  (JNIEnv* env, jobject obj, jlong ptr, jstring name, jint value) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature || !name) {
        return JNI_FALSE;
    }
    const char* cname = env->GetStringUTFChars(name, nullptr);
    if (!cname) {
        return JNI_FALSE;
    }
    int result = ogc_feature_set_field_integer_by_name(feature, cname, value);
    env->ReleaseStringUTFChars(name, cname);
    return result == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_feature_Feature_nativeSetFieldRealByName
  (JNIEnv* env, jobject obj, jlong ptr, jstring name, jdouble value) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature || !name) {
        return JNI_FALSE;
    }
    const char* cname = env->GetStringUTFChars(name, nullptr);
    if (!cname) {
        return JNI_FALSE;
    }
    int result = ogc_feature_set_field_real_by_name(feature, cname, value);
    env->ReleaseStringUTFChars(name, cname);
    return result == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_feature_Feature_nativeSetFieldStringByName
  (JNIEnv* env, jobject obj, jlong ptr, jstring name, jstring value) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature || !name || !value) {
        return JNI_FALSE;
    }
    const char* cname = env->GetStringUTFChars(name, nullptr);
    const char* cvalue = env->GetStringUTFChars(value, nullptr);
    if (!cname || !cvalue) {
        if (cname) env->ReleaseStringUTFChars(name, cname);
        if (cvalue) env->ReleaseStringUTFChars(value, cvalue);
        return JNI_FALSE;
    }
    int result = ogc_feature_set_field_string_by_name(feature, cname, cvalue);
    env->ReleaseStringUTFChars(name, cname);
    env->ReleaseStringUTFChars(value, cvalue);
    return result == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_Feature_nativeStealGeometry
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature) {
        return 0;
    }
    ogc_geometry_t* geom = ogc_feature_steal_geometry(feature);
    return JniConverter::ToJLongPtr(geom);
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_Feature_nativeGetEnvelope
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature) {
        return 0;
    }
    ogc_envelope_t* env_ = ogc_feature_get_envelope(feature);
    return JniConverter::ToJLongPtr(env_);
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_Feature_nativeClone
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature) {
        return 0;
    }
    ogc_feature_t* cloned = ogc_feature_clone(feature);
    return JniConverter::ToJLongPtr(cloned);
}

/* ===== VectorLayer Extended ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreate
  (JNIEnv* env, jclass clazz, jstring name) {
    try {
        const char* cname = env->GetStringUTFChars(name, nullptr);
        if (!cname) {
            JniException::ThrowNullPointerException(env, "Name is null");
            return 0;
        }
        ogc_layer_t* layer = ogc_vector_layer_create(cname);
        env->ReleaseStringUTFChars(name, cname);
        return JniConverter::ToJLongPtr(layer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreateFromDatasource
  (JNIEnv* env, jclass clazz, jstring path, jstring layerName) {
    try {
        const char* cpath = env->GetStringUTFChars(path, nullptr);
        const char* clayer = env->GetStringUTFChars(layerName, nullptr);
        if (!cpath || !clayer) {
            if (cpath) env->ReleaseStringUTFChars(path, cpath);
            if (clayer) env->ReleaseStringUTFChars(layerName, clayer);
            JniException::ThrowNullPointerException(env, "Path or layerName is null");
            return 0;
        }
        ogc_layer_t* layer = ogc_vector_layer_create_from_datasource(cpath, clayer);
        env->ReleaseStringUTFChars(path, cpath);
        env->ReleaseStringUTFChars(layerName, clayer);
        return JniConverter::ToJLongPtr(layer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetFeatureById
  (JNIEnv* env, jobject obj, jlong ptr, jlong fid) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return 0;
    }
    ogc_feature_t* feature = ogc_vector_layer_get_feature_by_id(layer, static_cast<int64_t>(fid));
    return JniConverter::ToJLongPtr(feature);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeSetSpatialFilter
  (JNIEnv* env, jobject obj, jlong ptr, jdouble minX, jdouble minY, jdouble maxX, jdouble maxY) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (layer) {
        ogc_vector_layer_set_spatial_filter(layer, minX, minY, maxX, maxY);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetFeatureDefn
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return 0;
    }
    ogc_feature_defn_t* defn = ogc_vector_layer_get_feature_defn(layer);
    return JniConverter::ToJLongPtr(defn);
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetSpatialFilter
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return 0;
    }
    ogc_geometry_t* geom = ogc_vector_layer_get_spatial_filter(layer);
    return JniConverter::ToJLongPtr(geom);
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeGetAttributeFilter
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return nullptr;
    }
    const char* filter = ogc_vector_layer_get_attribute_filter(layer);
    return filter ? env->NewStringUTF(filter) : nullptr;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeUpdateFeature
  (JNIEnv* env, jobject obj, jlong ptr, jlong featurePtr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(featurePtr));
    if (!layer || !feature) {
        return JNI_FALSE;
    }
    return ogc_vector_layer_update_feature(layer, feature) == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeDeleteFeature
  (JNIEnv* env, jobject obj, jlong ptr, jlong fid) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return JNI_FALSE;
    }
    return ogc_vector_layer_delete_feature(layer, static_cast<int64_t>(fid)) == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeCreateFeature
  (JNIEnv* env, jobject obj, jlong ptr, jlong featurePtr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(featurePtr));
    if (!layer || !feature) {
        return JNI_FALSE;
    }
    return ogc_vector_layer_create_feature(layer, feature) == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_layer_VectorLayer_nativeSetStyle
  (JNIEnv* env, jobject obj, jlong ptr, jstring styleName) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer || !styleName) {
        return JNI_FALSE;
    }
    const char* cstyle = env->GetStringUTFChars(styleName, nullptr);
    if (!cstyle) {
        return JNI_FALSE;
    }
    int result = ogc_vector_layer_set_style(layer, cstyle);
    env->ReleaseStringUTFChars(styleName, cstyle);
    return result == 0 ? JNI_TRUE : JNI_FALSE;
}

/* ===== RasterLayer ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_RasterLayer_nativeCreate
  (JNIEnv* env, jclass clazz, jstring name, jstring filepath) {
    try {
        const char* cname = env->GetStringUTFChars(name, nullptr);
        const char* cpath = env->GetStringUTFChars(filepath, nullptr);
        if (!cname || !cpath) {
            if (cname) env->ReleaseStringUTFChars(name, cname);
            if (cpath) env->ReleaseStringUTFChars(filepath, cpath);
            JniException::ThrowNullPointerException(env, "Name or filepath is null");
            return 0;
        }
        ogc_layer_t* layer = ogc_raster_layer_create(cname, cpath);
        env->ReleaseStringUTFChars(name, cname);
        env->ReleaseStringUTFChars(filepath, cpath);
        return JniConverter::ToJLongPtr(layer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_layer_RasterLayer_nativeGetWidth
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    return layer ? ogc_raster_layer_get_width(layer) : 0;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_layer_RasterLayer_nativeGetHeight
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    return layer ? ogc_raster_layer_get_height(layer) : 0;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_layer_RasterLayer_nativeGetBandCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    return layer ? ogc_raster_layer_get_band_count(layer) : 0;
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_layer_RasterLayer_nativeGetNoDataValue
  (JNIEnv* env, jobject obj, jlong ptr, jint bandIndex) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    return layer ? ogc_raster_layer_get_no_data_value(layer, bandIndex) : 0.0;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_RasterLayer_nativeGetExtent
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return 0;
    }
    ogc_envelope_t* env_ = ogc_raster_layer_get_extent(layer);
    return JniConverter::ToJLongPtr(env_);
}

/* ===== MemoryLayer ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_MemoryLayer_nativeCreate
  (JNIEnv* env, jclass clazz, jstring name) {
    try {
        const char* cname = env->GetStringUTFChars(name, nullptr);
        if (!cname) {
            JniException::ThrowNullPointerException(env, "Name is null");
            return 0;
        }
        ogc_layer_t* layer = ogc_memory_layer_create(cname);
        env->ReleaseStringUTFChars(name, cname);
        return JniConverter::ToJLongPtr(layer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_layer_MemoryLayer_nativeAddFeature
  (JNIEnv* env, jobject obj, jlong ptr, jlong featurePtr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(featurePtr));
    if (!layer || !feature) {
        return JNI_FALSE;
    }
    return ogc_memory_layer_add_feature(layer, feature) == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_layer_MemoryLayer_nativeRemoveFeature
  (JNIEnv* env, jobject obj, jlong ptr, jlong fid) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return JNI_FALSE;
    }
    return ogc_memory_layer_remove_feature(layer, static_cast<int64_t>(fid)) == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_MemoryLayer_nativeClear
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (layer) {
        ogc_memory_layer_clear(layer);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_MemoryLayer_nativeGetFeatureCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    return layer ? static_cast<jlong>(ogc_memory_layer_get_feature_count(layer)) : 0;
}

/* ===== LayerGroup ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_LayerGroup_nativeCreate
  (JNIEnv* env, jclass clazz, jstring name) {
    try {
        const char* cname = env->GetStringUTFChars(name, nullptr);
        if (!cname) {
            JniException::ThrowNullPointerException(env, "Name is null");
            return 0;
        }
        ogc_layer_group_t* group = ogc_layer_group_create(cname);
        env->ReleaseStringUTFChars(name, cname);
        return JniConverter::ToJLongPtr(group);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_LayerGroup_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_layer_group_t* group =
            static_cast<ogc_layer_group_t*>(JniConverter::FromJLongPtr(ptr));
        if (group) {
            ogc_layer_group_destroy(group);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_layer_LayerGroup_nativeGetName
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_group_t* group =
        static_cast<ogc_layer_group_t*>(JniConverter::FromJLongPtr(ptr));
    if (!group) {
        return nullptr;
    }
    const char* name = ogc_layer_group_get_name(group);
    return name ? env->NewStringUTF(name) : nullptr;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_LayerGroup_nativeGetLayerCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_group_t* group =
        static_cast<ogc_layer_group_t*>(JniConverter::FromJLongPtr(ptr));
    return group ? static_cast<jlong>(ogc_layer_group_get_layer_count(group)) : 0;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_LayerGroup_nativeGetLayer
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_layer_group_t* group =
        static_cast<ogc_layer_group_t*>(JniConverter::FromJLongPtr(ptr));
    if (!group) {
        return 0;
    }
    ogc_layer_t* layer = ogc_layer_group_get_layer(group, static_cast<size_t>(index));
    return JniConverter::ToJLongPtr(layer);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_LayerGroup_nativeAddLayer
  (JNIEnv* env, jobject obj, jlong ptr, jlong layerPtr) {
    ogc_layer_group_t* group =
        static_cast<ogc_layer_group_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(layerPtr));
    if (group && layer) {
        ogc_layer_group_add_layer(group, layer);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_LayerGroup_nativeRemoveLayer
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_layer_group_t* group =
        static_cast<ogc_layer_group_t*>(JniConverter::FromJLongPtr(ptr));
    if (group) {
        ogc_layer_group_remove_layer(group, static_cast<size_t>(index));
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_LayerGroup_nativeMoveLayer
  (JNIEnv* env, jobject obj, jlong ptr, jlong from, jlong to) {
    ogc_layer_group_t* group =
        static_cast<ogc_layer_group_t*>(JniConverter::FromJLongPtr(ptr));
    if (group) {
        ogc_layer_group_move_layer(group, static_cast<size_t>(from), static_cast<size_t>(to));
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_layer_LayerGroup_nativeIsVisible
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_group_t* group =
        static_cast<ogc_layer_group_t*>(JniConverter::FromJLongPtr(ptr));
    return group ? (ogc_layer_group_is_visible(group) ? JNI_TRUE : JNI_FALSE) : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_LayerGroup_nativeSetVisible
  (JNIEnv* env, jobject obj, jlong ptr, jboolean visible) {
    ogc_layer_group_t* group =
        static_cast<ogc_layer_group_t*>(JniConverter::FromJLongPtr(ptr));
    if (group) {
        ogc_layer_group_set_visible(group, visible ? 1 : 0);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_layer_LayerGroup_nativeGetOpacity
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_group_t* group =
        static_cast<ogc_layer_group_t*>(JniConverter::FromJLongPtr(ptr));
    return group ? ogc_layer_group_get_opacity(group) : 1.0;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_LayerGroup_nativeSetOpacity
  (JNIEnv* env, jobject obj, jlong ptr, jdouble opacity) {
    ogc_layer_group_t* group =
        static_cast<ogc_layer_group_t*>(JniConverter::FromJLongPtr(ptr));
    if (group) {
        ogc_layer_group_set_opacity(group, opacity);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_layer_LayerGroup_nativeGetZOrder
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_group_t* group =
        static_cast<ogc_layer_group_t*>(JniConverter::FromJLongPtr(ptr));
    return group ? ogc_layer_group_get_z_order(group) : 0;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_LayerGroup_nativeSetZOrder
  (JNIEnv* env, jobject obj, jlong ptr, jint zOrder) {
    ogc_layer_group_t* group =
        static_cast<ogc_layer_group_t*>(JniConverter::FromJLongPtr(ptr));
    if (group) {
        ogc_layer_group_set_z_order(group, zOrder);
    }
}

/* ===== Layer Extended (common) ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_Layer_nativeGetFeatureDefn
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return 0;
    }
    ogc_feature_defn_t* defn = ogc_layer_get_feature_defn(layer);
    return JniConverter::ToJLongPtr(defn);
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_layer_Layer_nativeGetGeomType
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    return layer ? static_cast<jint>(ogc_layer_get_geom_type(layer)) : 0;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_Layer_nativeGetFeatureCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    return layer ? static_cast<jlong>(ogc_layer_get_feature_count(layer)) : 0;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_Layer_nativeResetReading
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (layer) {
        ogc_layer_reset_reading(layer);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_Layer_nativeGetNextFeature
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return 0;
    }
    ogc_feature_t* feature = ogc_layer_get_next_feature(layer);
    return JniConverter::ToJLongPtr(feature);
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_layer_Layer_nativeGetFeature
  (JNIEnv* env, jobject obj, jlong ptr, jlong fid) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return 0;
    }
    ogc_feature_t* feature = ogc_layer_get_feature(layer, static_cast<long long>(fid));
    return JniConverter::ToJLongPtr(feature);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_layer_Layer_nativeSetAttributeFilter
  (JNIEnv* env, jobject obj, jlong ptr, jstring filter) {
    ogc_layer_t* layer =
        static_cast<ogc_layer_t*>(JniConverter::FromJLongPtr(ptr));
    if (!layer) {
        return;
    }
    if (!filter) {
        ogc_layer_set_attribute_filter(layer, nullptr);
        return;
    }
    const char* cfilter = env->GetStringUTFChars(filter, nullptr);
    if (cfilter) {
        ogc_layer_set_attribute_filter(layer, cfilter);
        env->ReleaseStringUTFChars(filter, cfilter);
    }
}

}
