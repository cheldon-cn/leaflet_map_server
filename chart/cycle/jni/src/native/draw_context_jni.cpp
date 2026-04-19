#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== Image ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_Image_nativeCreate
  (JNIEnv* env, jclass clazz, jint width, jint height, jint channels) {
    try {
        ogc_image_t* image = ogc_image_create(width, height, channels);
        return JniConverter::ToJLongPtr(image);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_Image_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_image_t* image =
            static_cast<ogc_image_t*>(JniConverter::FromJLongPtr(ptr));
        if (image) {
            ogc_image_destroy(image);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_Image_nativeGetWidth
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_image_t* image =
        static_cast<ogc_image_t*>(JniConverter::FromJLongPtr(ptr));
    return image ? ogc_image_get_width(image) : 0;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_Image_nativeGetHeight
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_image_t* image =
        static_cast<ogc_image_t*>(JniConverter::FromJLongPtr(ptr));
    return image ? ogc_image_get_height(image) : 0;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_Image_nativeGetChannels
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_image_t* image =
        static_cast<ogc_image_t*>(JniConverter::FromJLongPtr(ptr));
    return image ? ogc_image_get_channels(image) : 0;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_Image_nativeLoadFromFile
  (JNIEnv* env, jclass clazz, jstring path) {
    try {
        const char* cpath = env->GetStringUTFChars(path, nullptr);
        if (!cpath) {
            JniException::ThrowNullPointerException(env, "Path is null");
            return 0;
        }
        ogc_image_t* image = ogc_image_load_from_file(cpath);
        env->ReleaseStringUTFChars(path, cpath);
        return JniConverter::ToJLongPtr(image);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_draw_Image_nativeSaveToFile
  (JNIEnv* env, jobject obj, jlong ptr, jstring path) {
    ogc_image_t* image =
        static_cast<ogc_image_t*>(JniConverter::FromJLongPtr(ptr));
    if (!image) {
        return JNI_FALSE;
    }
    const char* cpath = env->GetStringUTFChars(path, nullptr);
    if (!cpath) {
        return JNI_FALSE;
    }
    int result = ogc_image_save_to_file(image, cpath);
    env->ReleaseStringUTFChars(path, cpath);
    return result == 0 ? JNI_TRUE : JNI_FALSE;
}

/* ===== DrawDevice ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_DrawDevice_nativeCreate
  (JNIEnv* env, jclass clazz, jint type, jint width, jint height) {
    try {
        ogc_draw_device_t* device =
            ogc_draw_device_create(static_cast<ogc_device_type_e>(type), width, height);
        return JniConverter::ToJLongPtr(device);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_DrawDevice_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_draw_device_t* device =
            static_cast<ogc_draw_device_t*>(JniConverter::FromJLongPtr(ptr));
        if (device) {
            ogc_draw_device_destroy(device);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_DrawDevice_nativeGetWidth
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_draw_device_t* device =
        static_cast<ogc_draw_device_t*>(JniConverter::FromJLongPtr(ptr));
    return device ? ogc_draw_device_get_width(device) : 0;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_DrawDevice_nativeGetHeight
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_draw_device_t* device =
        static_cast<ogc_draw_device_t*>(JniConverter::FromJLongPtr(ptr));
    return device ? ogc_draw_device_get_height(device) : 0;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_DrawDevice_nativeGetImage
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_draw_device_t* device =
        static_cast<ogc_draw_device_t*>(JniConverter::FromJLongPtr(ptr));
    if (!device) {
        return 0;
    }
    ogc_image_t* image = ogc_draw_device_get_image(device);
    return JniConverter::ToJLongPtr(image);
}

/* ===== ImageDevice ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_ImageDevice_nativeCreate
  (JNIEnv* env, jclass clazz, jlong width, jlong height) {
    try {
        ogc_image_device_t* device =
            ogc_image_device_create(static_cast<size_t>(width), static_cast<size_t>(height));
        return JniConverter::ToJLongPtr(device);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_ImageDevice_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_image_device_t* device =
            static_cast<ogc_image_device_t*>(JniConverter::FromJLongPtr(ptr));
        if (device) {
            ogc_image_device_destroy(device);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_draw_ImageDevice_nativeResize
  (JNIEnv* env, jobject obj, jlong ptr, jlong width, jlong height) {
    ogc_image_device_t* device =
        static_cast<ogc_image_device_t*>(JniConverter::FromJLongPtr(ptr));
    if (!device) {
        return JNI_FALSE;
    }
    int result = ogc_image_device_resize(device, static_cast<size_t>(width), static_cast<size_t>(height));
    return result == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_ImageDevice_nativeClear
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_image_device_t* device =
        static_cast<ogc_image_device_t*>(JniConverter::FromJLongPtr(ptr));
    if (device) {
        ogc_image_device_clear(device);
    }
}

JNIEXPORT jbyteArray JNICALL
Java_cn_cycle_chart_api_draw_ImageDevice_nativeGetPixels
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_image_device_t* device =
        static_cast<ogc_image_device_t*>(JniConverter::FromJLongPtr(ptr));
    if (!device) {
        return nullptr;
    }
    size_t size = 0;
    const unsigned char* pixels = ogc_image_device_get_pixels(device, &size);
    if (!pixels || size == 0) {
        return nullptr;
    }
    jbyteArray result = env->NewByteArray(static_cast<jsize>(size));
    if (result) {
        env->SetByteArrayRegion(result, 0, static_cast<jsize>(size), reinterpret_cast<const jbyte*>(pixels));
    }
    return result;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_ImageDevice_nativeGetWidth
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_image_device_t* device =
        static_cast<ogc_image_device_t*>(JniConverter::FromJLongPtr(ptr));
    return device ? ogc_image_device_get_width(device) : 0;
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_ImageDevice_nativeGetHeight
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_image_device_t* device =
        static_cast<ogc_image_device_t*>(JniConverter::FromJLongPtr(ptr));
    return device ? ogc_image_device_get_height(device) : 0;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_ImageDevice_nativeGetNativePtr
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_image_device_t* device =
        static_cast<ogc_image_device_t*>(JniConverter::FromJLongPtr(ptr));
    return device ? ogc_image_device_get_native_ptr(device) : 0;
}

/* ===== DrawEngine ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_DrawEngine_nativeCreate
  (JNIEnv* env, jclass clazz, jint type) {
    try {
        ogc_draw_engine_t* engine =
            ogc_draw_engine_create(static_cast<ogc_engine_type_e>(type));
        return JniConverter::ToJLongPtr(engine);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_DrawEngine_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_draw_engine_t* engine =
            static_cast<ogc_draw_engine_t*>(JniConverter::FromJLongPtr(ptr));
        if (engine) {
            ogc_draw_engine_destroy(engine);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_draw_DrawEngine_nativeInitialize
  (JNIEnv* env, jobject obj, jlong ptr, jlong devicePtr) {
    ogc_draw_engine_t* engine =
        static_cast<ogc_draw_engine_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_draw_device_t* device =
        static_cast<ogc_draw_device_t*>(JniConverter::FromJLongPtr(devicePtr));
    if (!engine || !device) {
        return JNI_FALSE;
    }
    int result = ogc_draw_engine_initialize(engine, device);
    return result == 0 ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_DrawEngine_nativeFinalize
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_draw_engine_t* engine =
        static_cast<ogc_draw_engine_t*>(JniConverter::FromJLongPtr(ptr));
    if (engine) {
        ogc_draw_engine_finalize(engine);
    }
}

/* ===== DrawContext ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeCreate
  (JNIEnv* env, jclass clazz, jlong devicePtr, jlong enginePtr) {
    try {
        ogc_draw_device_t* device =
            static_cast<ogc_draw_device_t*>(JniConverter::FromJLongPtr(devicePtr));
        ogc_draw_engine_t* engine =
            static_cast<ogc_draw_engine_t*>(JniConverter::FromJLongPtr(enginePtr));
        if (!device || !engine) {
            JniException::ThrowNullPointerException(env, "Device or Engine is null");
            return 0;
        }
        ogc_draw_context_t* ctx = ogc_draw_context_create(device, engine);
        return JniConverter::ToJLongPtr(ctx);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_draw_context_t* ctx =
            static_cast<ogc_draw_context_t*>(JniConverter::FromJLongPtr(ptr));
        if (ctx) {
            ogc_draw_context_destroy(ctx);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeBeginDraw
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_draw_context_t* ctx =
        static_cast<ogc_draw_context_t*>(JniConverter::FromJLongPtr(ptr));
    if (ctx) {
        ogc_draw_context_begin_draw(ctx);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeEndDraw
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_draw_context_t* ctx =
        static_cast<ogc_draw_context_t*>(JniConverter::FromJLongPtr(ptr));
    if (ctx) {
        ogc_draw_context_end_draw(ctx);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeClear
  (JNIEnv* env, jobject obj, jlong ptr, jint r, jint g, jint b, jint a) {
    ogc_draw_context_t* ctx =
        static_cast<ogc_draw_context_t*>(JniConverter::FromJLongPtr(ptr));
    if (ctx) {
        ogc_color_t color;
        color.r = static_cast<unsigned char>(r);
        color.g = static_cast<unsigned char>(g);
        color.b = static_cast<unsigned char>(b);
        color.a = static_cast<unsigned char>(a);
        ogc_draw_context_clear(ctx, color);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeDrawGeometry
  (JNIEnv* env, jobject obj, jlong ptr, jlong geomPtr, jlong stylePtr) {
    ogc_draw_context_t* ctx =
        static_cast<ogc_draw_context_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(geomPtr));
    ogc_draw_style_t* style =
        static_cast<ogc_draw_style_t*>(JniConverter::FromJLongPtr(stylePtr));
    if (ctx && geom && style) {
        ogc_draw_context_draw_geometry(ctx, geom, style);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeDrawPoint
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y, jlong stylePtr) {
    ogc_draw_context_t* ctx =
        static_cast<ogc_draw_context_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_draw_style_t* style =
        static_cast<ogc_draw_style_t*>(JniConverter::FromJLongPtr(stylePtr));
    if (ctx && style) {
        ogc_draw_context_draw_point(ctx, x, y, style);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeDrawLine
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x1, jdouble y1, jdouble x2, jdouble y2, jlong stylePtr) {
    ogc_draw_context_t* ctx =
        static_cast<ogc_draw_context_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_draw_style_t* style =
        static_cast<ogc_draw_style_t*>(JniConverter::FromJLongPtr(stylePtr));
    if (ctx && style) {
        ogc_draw_context_draw_line(ctx, x1, y1, x2, y2, style);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeDrawRect
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y, jdouble w, jdouble h, jlong stylePtr) {
    ogc_draw_context_t* ctx =
        static_cast<ogc_draw_context_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_draw_style_t* style =
        static_cast<ogc_draw_style_t*>(JniConverter::FromJLongPtr(stylePtr));
    if (ctx && style) {
        ogc_draw_context_draw_rect(ctx, x, y, w, h, style);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeFillRect
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y, jdouble w, jdouble h, jlong stylePtr) {
    ogc_draw_context_t* ctx =
        static_cast<ogc_draw_context_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_draw_style_t* style =
        static_cast<ogc_draw_style_t*>(JniConverter::FromJLongPtr(stylePtr));
    if (ctx && style) {
        ogc_draw_context_fill_rect(ctx, x, y, w, h, style);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeDrawText
  (JNIEnv* env, jobject obj, jlong ptr, jstring text, jdouble x, jdouble y, jlong stylePtr) {
    ogc_draw_context_t* ctx =
        static_cast<ogc_draw_context_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_draw_style_t* style =
        static_cast<ogc_draw_style_t*>(JniConverter::FromJLongPtr(stylePtr));
    if (!ctx || !style) {
        return;
    }
    const char* ctext = env->GetStringUTFChars(text, nullptr);
    if (ctext) {
        ogc_draw_context_draw_text(ctx, ctext, x, y, style);
        env->ReleaseStringUTFChars(text, ctext);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeSetTransform
  (JNIEnv* env, jobject obj, jlong ptr, jdoubleArray matrix) {
    ogc_draw_context_t* ctx =
        static_cast<ogc_draw_context_t*>(JniConverter::FromJLongPtr(ptr));
    if (!ctx) {
        return;
    }
    jsize len = env->GetArrayLength(matrix);
    if (len != 9) {
        JniException::ThrowIllegalArgumentException(env, "Transform matrix must have 9 elements");
        return;
    }
    jdouble* elems = env->GetDoubleArrayElements(matrix, nullptr);
    if (elems) {
        ogc_draw_context_set_transform(ctx, elems);
        env->ReleaseDoubleArrayElements(matrix, elems, JNI_ABORT);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeResetTransform
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_draw_context_t* ctx =
        static_cast<ogc_draw_context_t*>(JniConverter::FromJLongPtr(ptr));
    if (ctx) {
        ogc_draw_context_reset_transform(ctx);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeClip
  (JNIEnv* env, jobject obj, jlong ptr, jlong geomPtr) {
    ogc_draw_context_t* ctx =
        static_cast<ogc_draw_context_t*>(JniConverter::FromJLongPtr(ptr));
    ogc_geometry_t* geom =
        static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(geomPtr));
    if (ctx && geom) {
        ogc_draw_context_clip(ctx, geom);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_core_RenderContext_nativeResetClip
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_draw_context_t* ctx =
        static_cast<ogc_draw_context_t*>(JniConverter::FromJLongPtr(ptr));
    if (ctx) {
        ogc_draw_context_reset_clip(ctx);
    }
}

}
