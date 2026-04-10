#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== Feature ===== */

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_feature_t* feature =
            static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
        if (feature) {
            ogc_feature_destroy(feature);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetFid
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature) {
        return 0;
    }
    return static_cast<jlong>(ogc_feature_get_fid(feature));
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetFid
  (JNIEnv* env, jobject obj, jlong ptr, jlong fid) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (feature) {
        ogc_feature_set_fid(feature, static_cast<long long>(fid));
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetFieldCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature) {
        return 0;
    }
    return static_cast<jlong>(ogc_feature_get_field_count(feature));
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeIsFieldSet
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature) {
        return JNI_FALSE;
    }
    return ogc_feature_is_field_set(feature, static_cast<size_t>(index)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeIsFieldNull
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature) {
        return JNI_TRUE;
    }
    return ogc_feature_is_field_null(feature, static_cast<size_t>(index)) ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetFieldAsInteger
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature) {
        return 0;
    }
    return ogc_feature_get_field_as_integer(feature, static_cast<size_t>(index));
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetFieldAsReal
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature) {
        return 0.0;
    }
    return ogc_feature_get_field_as_real(feature, static_cast<size_t>(index));
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetFieldAsString
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature) {
        return nullptr;
    }

    const char* str = ogc_feature_get_field_as_string(feature, static_cast<size_t>(index));
    if (!str) {
        return nullptr;
    }

    return env->NewStringUTF(str);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetFieldInteger
  (JNIEnv* env, jobject obj, jlong ptr, jlong index, jint value) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (feature) {
        ogc_feature_set_field_integer(feature, static_cast<size_t>(index), value);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetFieldReal
  (JNIEnv* env, jobject obj, jlong ptr, jlong index, jdouble value) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (feature) {
        ogc_feature_set_field_real(feature, static_cast<size_t>(index), value);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetFieldString
  (JNIEnv* env, jobject obj, jlong ptr, jlong index, jstring value) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (feature) {
        std::string str = JniConverter::ToString(env, value);
        ogc_feature_set_field_string(feature, static_cast<size_t>(index), str.c_str());
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetFieldNull
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (feature) {
        ogc_feature_set_field_null(feature, static_cast<size_t>(index));
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeGetGeometry
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    if (!feature) {
        return 0;
    }
    ogc_geometry_t* geom = ogc_feature_get_geometry(feature);
    return JniConverter::ToJLongPtr(geom);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FeatureInfo_nativeSetGeometry
  (JNIEnv* env, jobject obj, jlong ptr, jlong geomPtr) {
    ogc_feature_t* feature =
        static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(geomPtr));
    if (feature && geom) {
        ogc_feature_set_geometry(feature, geom);
    }
}

/* ===== FieldDefn ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_feature_FieldDefn_nativeCreate
  (JNIEnv* env, jclass clazz, jstring name, jint type) {
    try {
        std::string fieldName = JniConverter::ToString(env, name);
        ogc_field_defn_t* defn = ogc_field_defn_create(fieldName.c_str(),
                                                        static_cast<ogc_field_type_e>(type));
        return JniConverter::ToJLongPtr(defn);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FieldDefn_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_field_defn_t* defn =
            static_cast<ogc_field_defn_t*>(JniConverter::FromJLongPtr(ptr));
        if (defn) {
            ogc_field_defn_destroy(defn);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_feature_FieldDefn_nativeGetName
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_field_defn_t* defn =
        static_cast<ogc_field_defn_t*>(JniConverter::FromJLongPtr(ptr));
    if (!defn) {
        return nullptr;
    }
    const char* name = ogc_field_defn_get_name(defn);
    return name ? env->NewStringUTF(name) : nullptr;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_feature_FieldDefn_nativeGetType
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_field_defn_t* defn =
        static_cast<ogc_field_defn_t*>(JniConverter::FromJLongPtr(ptr));
    if (!defn) {
        return 0;
    }
    return static_cast<jint>(ogc_field_defn_get_type(defn));
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_feature_FieldDefn_nativeGetWidth
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_field_defn_t* defn =
        static_cast<ogc_field_defn_t*>(JniConverter::FromJLongPtr(ptr));
    return defn ? ogc_field_defn_get_width(defn) : 0;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FieldDefn_nativeSetWidth
  (JNIEnv* env, jobject obj, jlong ptr, jint width) {
    ogc_field_defn_t* defn =
        static_cast<ogc_field_defn_t*>(JniConverter::FromJLongPtr(ptr));
    if (defn) {
        ogc_field_defn_set_width(defn, width);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_feature_FieldDefn_nativeGetPrecision
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_field_defn_t* defn =
        static_cast<ogc_field_defn_t*>(JniConverter::FromJLongPtr(ptr));
    return defn ? ogc_field_defn_get_precision(defn) : 0;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_feature_FieldDefn_nativeSetPrecision
  (JNIEnv* env, jobject obj, jlong ptr, jint precision) {
    ogc_field_defn_t* defn =
        static_cast<ogc_field_defn_t*>(JniConverter::FromJLongPtr(ptr));
    if (defn) {
        ogc_field_defn_set_precision(defn, precision);
    }
}

}
