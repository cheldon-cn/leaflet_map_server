#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"
#include <vector>
#include <string>

using namespace ogc::jni;

extern "C" {

/* ===== S57Parser ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_parser_S57Parser_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_s57_parser_t* parser = ogc_s57_parser_create();
        return JniConverter::ToJLongPtr(parser);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_parser_S57Parser_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_s57_parser_t* parser =
            static_cast<ogc_s57_parser_t*>(JniConverter::FromJLongPtr(ptr));
        if (parser) {
            ogc_s57_parser_destroy(parser);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_parser_S57Parser_nativeOpen
  (JNIEnv* env, jobject obj, jlong ptr, jstring path) {
    ogc_s57_parser_t* parser =
        static_cast<ogc_s57_parser_t*>(JniConverter::FromJLongPtr(ptr));
    if (!parser || !path) {
        return JNI_FALSE;
    }
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    if (!cpath) {
        return JNI_FALSE;
    }
    int result = ogc_s57_parser_open(parser, cpath);
    env->ReleaseStringUTFChars(path, cpath);
    return result == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_parser_S57Parser_nativeParse
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_s57_parser_t* parser =
        static_cast<ogc_s57_parser_t*>(JniConverter::FromJLongPtr(ptr));
    if (!parser) {
        return 0;
    }
    ogc_parse_result_t* result = ogc_s57_parser_parse(parser);
    return JniConverter::ToJLongPtr(result);
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_parser_S57Parser_nativeSetFeatureFilter
  (JNIEnv* env, jobject obj, jlong ptr, jobjectArray features) {
    ogc_s57_parser_t* parser =
        static_cast<ogc_s57_parser_t*>(JniConverter::FromJLongPtr(ptr));
    if (!parser || !features) {
        return;
    }
    jsize count = env->GetArrayLength(features);
    std::vector<const char*> cfeatures(count);
    std::vector<std::string> storage(count);
    for (jsize i = 0; i < count; ++i) {
        jstring jstr = static_cast<jstring>(env->GetObjectArrayElement(features, i));
        if (jstr) {
            const char* cstr = env->GetStringUTFChars(jstr, nullptr);
            storage[i] = cstr;
            cfeatures[i] = storage[i].c_str();
            env->ReleaseStringUTFChars(jstr, cstr);
            env->DeleteLocalRef(jstr);
        } else {
            cfeatures[i] = "";
        }
    }
    ogc_s57_parser_set_feature_filter(parser, cfeatures.data(), static_cast<int>(count));
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_parser_S57Parser_nativeSetSpatialFilter
  (JNIEnv* env, jobject obj, jlong ptr, jdouble minX, jdouble minY, jdouble maxX, jdouble maxY) {
    ogc_s57_parser_t* parser =
        static_cast<ogc_s57_parser_t*>(JniConverter::FromJLongPtr(ptr));
    if (!parser) {
        return;
    }
    ogc_envelope_t* bounds = ogc_envelope_create_from_coords(minX, minY, maxX, maxY);
    if (bounds) {
        ogc_s57_parser_set_spatial_filter(parser, bounds);
        ogc_envelope_destroy(bounds);
    }
}

/* ===== IncrementalParser ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_parser_IncrementalParser_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_incremental_parser_t* parser = ogc_incremental_parser_create();
        return JniConverter::ToJLongPtr(parser);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_parser_IncrementalParser_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_incremental_parser_t* parser =
            static_cast<ogc_incremental_parser_t*>(JniConverter::FromJLongPtr(ptr));
        if (parser) {
            ogc_incremental_parser_destroy(parser);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_parser_IncrementalParser_nativeStart
  (JNIEnv* env, jobject obj, jlong ptr, jstring path, jint batchSize) {
    ogc_incremental_parser_t* parser =
        static_cast<ogc_incremental_parser_t*>(JniConverter::FromJLongPtr(ptr));
    if (!parser || !path) {
        return;
    }
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    if (cpath) {
        ogc_incremental_parser_start(parser, cpath, batchSize);
        env->ReleaseStringUTFChars(path, cpath);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_parser_IncrementalParser_nativePause
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_incremental_parser_t* parser =
        static_cast<ogc_incremental_parser_t*>(JniConverter::FromJLongPtr(ptr));
    if (parser) {
        ogc_incremental_parser_pause(parser);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_parser_IncrementalParser_nativeResume
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_incremental_parser_t* parser =
        static_cast<ogc_incremental_parser_t*>(JniConverter::FromJLongPtr(ptr));
    if (parser) {
        ogc_incremental_parser_resume(parser);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_parser_IncrementalParser_nativeCancel
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_incremental_parser_t* parser =
        static_cast<ogc_incremental_parser_t*>(JniConverter::FromJLongPtr(ptr));
    if (parser) {
        ogc_incremental_parser_cancel(parser);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_parser_IncrementalParser_nativeParseNext
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_incremental_parser_t* parser =
        static_cast<ogc_incremental_parser_t*>(JniConverter::FromJLongPtr(ptr));
    if (!parser) {
        return 0;
    }
    ogc_parse_result_t* result = ogc_incremental_parser_parse_next(parser);
    return JniConverter::ToJLongPtr(result);
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_parser_IncrementalParser_nativeHasMore
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_incremental_parser_t* parser =
        static_cast<ogc_incremental_parser_t*>(JniConverter::FromJLongPtr(ptr));
    return parser ? (ogc_incremental_parser_has_more(parser) ? JNI_TRUE : JNI_FALSE) : JNI_FALSE;
}

JNIEXPORT jfloat JNICALL
Java_cn_cycle_chart_api_parser_IncrementalParser_nativeGetProgress
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_incremental_parser_t* parser =
        static_cast<ogc_incremental_parser_t*>(JniConverter::FromJLongPtr(ptr));
    return parser ? ogc_incremental_parser_get_progress(parser) : 0.0f;
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_parser_IncrementalParser_nativeHasFileChanged
  (JNIEnv* env, jobject obj, jlong ptr, jstring path) {
    ogc_incremental_parser_t* parser =
        static_cast<ogc_incremental_parser_t*>(JniConverter::FromJLongPtr(ptr));
    if (!parser || !path) {
        return JNI_FALSE;
    }
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    if (!cpath) {
        return JNI_FALSE;
    }
    int result = ogc_incremental_parser_has_file_changed(parser, cpath);
    env->ReleaseStringUTFChars(path, cpath);
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_parser_IncrementalParser_nativeMarkProcessed
  (JNIEnv* env, jobject obj, jlong ptr, jstring path) {
    ogc_incremental_parser_t* parser =
        static_cast<ogc_incremental_parser_t*>(JniConverter::FromJLongPtr(ptr));
    if (!parser || !path) {
        return;
    }
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    if (cpath) {
        ogc_incremental_parser_mark_processed(parser, cpath);
        env->ReleaseStringUTFChars(path, cpath);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_parser_IncrementalParser_nativeClearState
  (JNIEnv* env, jobject obj, jlong ptr, jstring path) {
    ogc_incremental_parser_t* parser =
        static_cast<ogc_incremental_parser_t*>(JniConverter::FromJLongPtr(ptr));
    if (!parser || !path) {
        return;
    }
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    if (cpath) {
        ogc_incremental_parser_clear_state(parser, cpath);
        env->ReleaseStringUTFChars(path, cpath);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_parser_S57Parser_nativeParseFile
  (JNIEnv* env, jobject obj, jlong ptr, jstring path, jlong configPtr) {
    ogc_s57_parser_t* parser =
        static_cast<ogc_s57_parser_t*>(JniConverter::FromJLongPtr(ptr));
    if (!parser || !path) { return 0; }
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    if (!cpath) { return 0; }
    ogc_parse_config_t* config =
        static_cast<ogc_parse_config_t*>(JniConverter::FromJLongPtr(configPtr));
    ogc_parse_result_t* result = ogc_s57_parser_parse_file(parser, cpath, config);
    env->ReleaseStringUTFChars(path, cpath);
    return JniConverter::ToJLongPtr(result);
}

}
