#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

#include <vector>

using namespace ogc::jni;

extern "C" {

/* ===== ChartParser (singleton) ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_parser_ChartParser_nativeGetInstance
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_chart_parser_t* parser = ogc_chart_parser_get_instance();
        return JniConverter::ToJLongPtr(parser);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_parser_ChartParser_nativeInitialize
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_chart_parser_t* parser =
            static_cast<ogc_chart_parser_t*>(JniConverter::FromJLongPtr(ptr));
        if (!parser) {
            JniException::ThrowNullPointerException(env, "ChartParser is null");
            return -1;
        }
        return ogc_chart_parser_initialize(parser);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_parser_ChartParser_nativeShutdown
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_chart_parser_t* parser =
            static_cast<ogc_chart_parser_t*>(JniConverter::FromJLongPtr(ptr));
        if (parser) {
            ogc_chart_parser_shutdown(parser);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jintArray JNICALL
Java_cn_cycle_chart_api_parser_ChartParser_nativeGetSupportedFormats
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_chart_parser_t* parser =
            static_cast<ogc_chart_parser_t*>(JniConverter::FromJLongPtr(ptr));
        if (!parser) {
            JniException::ThrowNullPointerException(env, "ChartParser is null");
            return nullptr;
        }

        ogc_chart_format_e formats[16] = {OGC_CHART_FORMAT_UNKNOWN};
        int count = ogc_chart_parser_get_supported_formats(parser, formats, 16);
        if (count <= 0) {
            return nullptr;
        }

        jintArray result = env->NewIntArray(static_cast<jsize>(count));
        if (!result) {
            return nullptr;
        }

        std::vector<jint> formatList(static_cast<size_t>(count));
        for (int i = 0; i < count; ++i) {
            formatList[static_cast<size_t>(i)] = static_cast<jint>(formats[i]);
        }
        env->SetIntArrayRegion(result, 0, static_cast<jsize>(count), formatList.data());
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

/* ===== IParser ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_parser_IParser_nativeCreate
  (JNIEnv* env, jclass clazz, jint format) {
    try {
        ogc_iparser_t* parser = ogc_iparser_create(
            static_cast<ogc_chart_format_e>(format));
        return JniConverter::ToJLongPtr(parser);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_parser_IParser_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_iparser_t* parser =
            static_cast<ogc_iparser_t*>(JniConverter::FromJLongPtr(ptr));
        if (parser) {
            ogc_iparser_destroy(parser);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_parser_IParser_nativeOpen
  (JNIEnv* env, jobject obj, jlong ptr, jstring path) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return -1;
    }

    try {
        ogc_iparser_t* parser =
            static_cast<ogc_iparser_t*>(JniConverter::FromJLongPtr(ptr));
        if (!parser) {
            JniException::ThrowNullPointerException(env, "IParser is null");
            return -1;
        }
        std::string filePath = JniConverter::ToString(env, path);
        return ogc_iparser_open(parser, filePath.c_str());
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return -1;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_parser_IParser_nativeClose
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_iparser_t* parser =
            static_cast<ogc_iparser_t*>(JniConverter::FromJLongPtr(ptr));
        if (parser) {
            ogc_iparser_close(parser);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_parser_IParser_nativeParse
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_iparser_t* parser =
            static_cast<ogc_iparser_t*>(JniConverter::FromJLongPtr(ptr));
        if (!parser) {
            JniException::ThrowNullPointerException(env, "IParser is null");
            return 0;
        }
        ogc_parse_result_t* result = ogc_iparser_parse(parser);
        return JniConverter::ToJLongPtr(result);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_parser_IParser_nativeParseIncremental
  (JNIEnv* env, jobject obj, jlong ptr, jint batchSize) {
    try {
        ogc_iparser_t* parser =
            static_cast<ogc_iparser_t*>(JniConverter::FromJLongPtr(ptr));
        if (!parser) {
            JniException::ThrowNullPointerException(env, "IParser is null");
            return 0;
        }
        ogc_parse_result_t* result = ogc_iparser_parse_incremental(parser,
            static_cast<int>(batchSize));
        return JniConverter::ToJLongPtr(result);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_parser_IParser_nativeGetFormat
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_iparser_t* parser =
            static_cast<ogc_iparser_t*>(JniConverter::FromJLongPtr(ptr));
        if (!parser) {
            return static_cast<jint>(OGC_CHART_FORMAT_UNKNOWN);
        }
        return static_cast<jint>(ogc_iparser_get_format(parser));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return static_cast<jint>(OGC_CHART_FORMAT_UNKNOWN);
    }
}

}
