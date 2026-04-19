#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== Filter ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_symbology_Filter_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_filter_t* filter = ogc_filter_create();
        return JniConverter::ToJLongPtr(filter);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_symbology_Filter_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_filter_t* filter =
            static_cast<ogc_filter_t*>(JniConverter::FromJLongPtr(ptr));
        if (!filter) {
            return;
        }
        ogc_filter_destroy(filter);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_symbology_Filter_nativeGetType
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_filter_t* filter =
            static_cast<ogc_filter_t*>(JniConverter::FromJLongPtr(ptr));
        if (!filter) {
            JniException::ThrowNullPointerException(env, "Filter is null");
            return 0;
        }
        return static_cast<jint>(ogc_filter_get_type(filter));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_symbology_Filter_nativeEvaluate
  (JNIEnv* env, jobject obj, jlong ptr, jlong featurePtr) {
    try {
        ogc_filter_t* filter =
            static_cast<ogc_filter_t*>(JniConverter::FromJLongPtr(ptr));
        if (!filter) {
            JniException::ThrowNullPointerException(env, "Filter is null");
            return JNI_FALSE;
        }
        ogc_feature_t* feature =
            static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(featurePtr));
        if (!feature) {
            JniException::ThrowNullPointerException(env, "Feature is null");
            return JNI_FALSE;
        }
        return ogc_filter_evaluate(filter, feature) ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_symbology_Filter_nativeToString
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_filter_t* filter =
            static_cast<ogc_filter_t*>(JniConverter::FromJLongPtr(ptr));
        if (!filter) {
            JniException::ThrowNullPointerException(env, "Filter is null");
            return nullptr;
        }
        char* str = ogc_filter_to_string(filter);
        if (!str) {
            return nullptr;
        }
        jstring result = env->NewStringUTF(str);
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_symbology_ComparisonFilter_nativeCreate
  (JNIEnv* env, jclass clazz, jstring property, jint op, jstring value) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }

    try {
        const char* propStr = env->GetStringUTFChars(property, nullptr);
        if (!propStr) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get property string");
            return 0;
        }
        const char* valStr = env->GetStringUTFChars(value, nullptr);
        if (!valStr) {
            env->ReleaseStringUTFChars(property, propStr);
            JniException::ThrowOutOfMemoryError(env, "Failed to get value string");
            return 0;
        }
        ogc_filter_t* filter = ogc_comparison_filter_create(
            propStr, static_cast<ogc_comparison_operator_e>(op), valStr);
        env->ReleaseStringUTFChars(value, valStr);
        env->ReleaseStringUTFChars(property, propStr);
        return JniConverter::ToJLongPtr(filter);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_symbology_LogicalFilter_nativeCreate
  (JNIEnv* env, jclass clazz, jint op) {
    try {
        ogc_filter_t* filter = ogc_logical_filter_create(
            static_cast<ogc_logical_operator_e>(op));
        return JniConverter::ToJLongPtr(filter);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_symbology_LogicalFilter_nativeAddFilter
  (JNIEnv* env, jobject obj, jlong ptr, jlong filterPtr) {
    try {
        ogc_filter_t* logical =
            static_cast<ogc_filter_t*>(JniConverter::FromJLongPtr(ptr));
        if (!logical) {
            JniException::ThrowNullPointerException(env, "LogicalFilter is null");
            return;
        }
        ogc_filter_t* filter =
            static_cast<ogc_filter_t*>(JniConverter::FromJLongPtr(filterPtr));
        if (!filter) {
            JniException::ThrowNullPointerException(env, "Filter is null");
            return;
        }
        ogc_logical_filter_add_filter(logical, filter);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== Symbolizer ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_symbology_Symbolizer_nativeCreate
  (JNIEnv* env, jclass clazz, jint type) {
    try {
        ogc_symbolizer_t* symbolizer = ogc_symbolizer_create(
            static_cast<ogc_symbolizer_type_e>(type));
        return JniConverter::ToJLongPtr(symbolizer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_symbology_Symbolizer_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_symbolizer_t* symbolizer =
            static_cast<ogc_symbolizer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!symbolizer) {
            return;
        }
        ogc_symbolizer_destroy(symbolizer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_symbology_Symbolizer_nativeGetType
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_symbolizer_t* symbolizer =
            static_cast<ogc_symbolizer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!symbolizer) {
            JniException::ThrowNullPointerException(env, "Symbolizer is null");
            return 0;
        }
        return static_cast<jint>(ogc_symbolizer_get_type(symbolizer));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_symbology_Symbolizer_nativeGetStyle
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_symbolizer_t* symbolizer =
            static_cast<ogc_symbolizer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!symbolizer) {
            JniException::ThrowNullPointerException(env, "Symbolizer is null");
            return 0;
        }
        ogc_draw_style_t* style = ogc_symbolizer_get_style(symbolizer);
        return JniConverter::ToJLongPtr(style);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_symbology_Symbolizer_nativeSetStyle
  (JNIEnv* env, jobject obj, jlong ptr, jlong stylePtr) {
    try {
        ogc_symbolizer_t* symbolizer =
            static_cast<ogc_symbolizer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!symbolizer) {
            JniException::ThrowNullPointerException(env, "Symbolizer is null");
            return;
        }
        ogc_draw_style_t* style =
            static_cast<ogc_draw_style_t*>(JniConverter::FromJLongPtr(stylePtr));
        ogc_symbolizer_set_style(symbolizer, style);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== SymbolizerRule ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_symbolizer_rule_t* rule = ogc_symbolizer_rule_create();
        return JniConverter::ToJLongPtr(rule);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_symbolizer_rule_t* rule =
            static_cast<ogc_symbolizer_rule_t*>(JniConverter::FromJLongPtr(ptr));
        if (!rule) {
            return;
        }
        ogc_symbolizer_rule_destroy(rule);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeGetName
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_symbolizer_rule_t* rule =
            static_cast<ogc_symbolizer_rule_t*>(JniConverter::FromJLongPtr(ptr));
        if (!rule) {
            JniException::ThrowNullPointerException(env, "SymbolizerRule is null");
            return nullptr;
        }
        const char* name = ogc_symbolizer_rule_get_name(rule);
        if (!name) {
            return nullptr;
        }
        return env->NewStringUTF(name);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeSetName
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_symbolizer_rule_t* rule =
            static_cast<ogc_symbolizer_rule_t*>(JniConverter::FromJLongPtr(ptr));
        if (!rule) {
            JniException::ThrowNullPointerException(env, "SymbolizerRule is null");
            return;
        }
        const char* nameStr = env->GetStringUTFChars(name, nullptr);
        if (!nameStr) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get string");
            return;
        }
        ogc_symbolizer_rule_set_name(rule, nameStr);
        env->ReleaseStringUTFChars(name, nameStr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeGetFilter
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_symbolizer_rule_t* rule =
            static_cast<ogc_symbolizer_rule_t*>(JniConverter::FromJLongPtr(ptr));
        if (!rule) {
            JniException::ThrowNullPointerException(env, "SymbolizerRule is null");
            return 0;
        }
        ogc_filter_t* filter = ogc_symbolizer_rule_get_filter(rule);
        return JniConverter::ToJLongPtr(filter);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeSetFilter
  (JNIEnv* env, jobject obj, jlong ptr, jlong filterPtr) {
    try {
        ogc_symbolizer_rule_t* rule =
            static_cast<ogc_symbolizer_rule_t*>(JniConverter::FromJLongPtr(ptr));
        if (!rule) {
            JniException::ThrowNullPointerException(env, "SymbolizerRule is null");
            return;
        }
        ogc_filter_t* filter =
            static_cast<ogc_filter_t*>(JniConverter::FromJLongPtr(filterPtr));
        ogc_symbolizer_rule_set_filter(rule, filter);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeGetSymbolizer
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    try {
        ogc_symbolizer_rule_t* rule =
            static_cast<ogc_symbolizer_rule_t*>(JniConverter::FromJLongPtr(ptr));
        if (!rule) {
            JniException::ThrowNullPointerException(env, "SymbolizerRule is null");
            return 0;
        }
        ogc_symbolizer_t* sym = ogc_symbolizer_rule_get_symbolizer(
            rule, static_cast<size_t>(index));
        return JniConverter::ToJLongPtr(sym);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeSetSymbolizer
  (JNIEnv* env, jobject obj, jlong ptr, jlong symPtr) {
    try {
        ogc_symbolizer_rule_t* rule =
            static_cast<ogc_symbolizer_rule_t*>(JniConverter::FromJLongPtr(ptr));
        if (!rule) {
            JniException::ThrowNullPointerException(env, "SymbolizerRule is null");
            return;
        }
        ogc_symbolizer_t* sym =
            static_cast<ogc_symbolizer_t*>(JniConverter::FromJLongPtr(symPtr));
        ogc_symbolizer_rule_set_symbolizer(rule, sym);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeGetMinScale
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_symbolizer_rule_t* rule =
            static_cast<ogc_symbolizer_rule_t*>(JniConverter::FromJLongPtr(ptr));
        if (!rule) {
            JniException::ThrowNullPointerException(env, "SymbolizerRule is null");
            return 0.0;
        }
        return ogc_symbolizer_rule_get_min_scale(rule);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeGetMaxScale
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_symbolizer_rule_t* rule =
            static_cast<ogc_symbolizer_rule_t*>(JniConverter::FromJLongPtr(ptr));
        if (!rule) {
            JniException::ThrowNullPointerException(env, "SymbolizerRule is null");
            return 0.0;
        }
        return ogc_symbolizer_rule_get_max_scale(rule);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeSetMinScale
  (JNIEnv* env, jobject obj, jlong ptr, jdouble scale) {
    try {
        ogc_symbolizer_rule_t* rule =
            static_cast<ogc_symbolizer_rule_t*>(JniConverter::FromJLongPtr(ptr));
        if (!rule) {
            JniException::ThrowNullPointerException(env, "SymbolizerRule is null");
            return;
        }
        ogc_symbolizer_rule_set_min_scale(rule, scale);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeSetMaxScale
  (JNIEnv* env, jobject obj, jlong ptr, jdouble scale) {
    try {
        ogc_symbolizer_rule_t* rule =
            static_cast<ogc_symbolizer_rule_t*>(JniConverter::FromJLongPtr(ptr));
        if (!rule) {
            JniException::ThrowNullPointerException(env, "SymbolizerRule is null");
            return;
        }
        ogc_symbolizer_rule_set_max_scale(rule, scale);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeAddSymbolizer
  (JNIEnv* env, jobject obj, jlong ptr, jlong symPtr) {
    try {
        ogc_symbolizer_rule_t* rule =
            static_cast<ogc_symbolizer_rule_t*>(JniConverter::FromJLongPtr(ptr));
        if (!rule) {
            JniException::ThrowNullPointerException(env, "SymbolizerRule is null");
            return;
        }
        ogc_symbolizer_t* sym =
            static_cast<ogc_symbolizer_t*>(JniConverter::FromJLongPtr(symPtr));
        ogc_symbolizer_rule_add_symbolizer(rule, sym);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeIsApplicable
  (JNIEnv* env, jobject obj, jlong ptr, jlong featurePtr, jdouble scale) {
    try {
        ogc_symbolizer_rule_t* rule =
            static_cast<ogc_symbolizer_rule_t*>(JniConverter::FromJLongPtr(ptr));
        if (!rule) {
            JniException::ThrowNullPointerException(env, "SymbolizerRule is null");
            return JNI_FALSE;
        }
        ogc_feature_t* feature =
            static_cast<ogc_feature_t*>(JniConverter::FromJLongPtr(featurePtr));
        if (!feature) {
            JniException::ThrowNullPointerException(env, "Feature is null");
            return JNI_FALSE;
        }
        return ogc_symbolizer_rule_is_applicable(rule, feature, scale) ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_symbology_SymbolizerRule_nativeSetScaleRange
  (JNIEnv* env, jobject obj, jlong ptr, jdouble minScale, jdouble maxScale) {
    try {
        ogc_symbolizer_rule_t* rule =
            static_cast<ogc_symbolizer_rule_t*>(JniConverter::FromJLongPtr(ptr));
        if (!rule) {
            JniException::ThrowNullPointerException(env, "SymbolizerRule is null");
            return;
        }
        ogc_symbolizer_rule_set_scale_range(rule, minScale, maxScale);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

}
