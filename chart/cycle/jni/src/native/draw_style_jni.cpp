#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== Color ===== */

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_Color_nativeFromRgba
  (JNIEnv* env, jclass clazz, jbyte r, jbyte g, jbyte b, jbyte a) {
    try {
        ogc_color_t color = ogc_color_from_rgba(
            static_cast<unsigned char>(r & 0xFF),
            static_cast<unsigned char>(g & 0xFF),
            static_cast<unsigned char>(b & 0xFF),
            static_cast<unsigned char>(a & 0xFF));
        jint result = (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_Color_nativeFromRgb
  (JNIEnv* env, jclass clazz, jbyte r, jbyte g, jbyte b) {
    try {
        ogc_color_t color = ogc_color_from_rgb(
            static_cast<unsigned char>(r & 0xFF),
            static_cast<unsigned char>(g & 0xFF),
            static_cast<unsigned char>(b & 0xFF));
        jint result = (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_Color_nativeFromHex
  (JNIEnv* env, jclass clazz, jint hex) {
    try {
        ogc_color_t color = ogc_color_from_hex(static_cast<unsigned int>(hex));
        jint result = (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_Color_nativeToHex
  (JNIEnv* env, jclass clazz, jint packedColor) {
    try {
        ogc_color_t color;
        color.r = static_cast<unsigned char>((packedColor >> 24) & 0xFF);
        color.g = static_cast<unsigned char>((packedColor >> 16) & 0xFF);
        color.b = static_cast<unsigned char>((packedColor >> 8) & 0xFF);
        color.a = static_cast<unsigned char>(packedColor & 0xFF);
        return static_cast<jint>(ogc_color_to_hex(&color));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

/* ===== Font ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_Font_nativeCreate
  (JNIEnv* env, jclass clazz, jstring family, jdouble size) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }

    try {
        const char* familyStr = env->GetStringUTFChars(family, nullptr);
        if (!familyStr) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get string");
            return 0;
        }
        ogc_font_t* font = ogc_font_create(familyStr, size);
        env->ReleaseStringUTFChars(family, familyStr);
        return JniConverter::ToJLongPtr(font);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_Font_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_font_t* font =
            static_cast<ogc_font_t*>(JniConverter::FromJLongPtr(ptr));
        if (!font) {
            return;
        }
        ogc_font_destroy(font);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_draw_Font_nativeGetFamily
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_font_t* font =
            static_cast<ogc_font_t*>(JniConverter::FromJLongPtr(ptr));
        if (!font) {
            JniException::ThrowNullPointerException(env, "Font is null");
            return nullptr;
        }
        const char* family = ogc_font_get_family(font);
        if (!family) {
            return nullptr;
        }
        return env->NewStringUTF(family);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_draw_Font_nativeGetSize
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_font_t* font =
            static_cast<ogc_font_t*>(JniConverter::FromJLongPtr(ptr));
        if (!font) {
            JniException::ThrowNullPointerException(env, "Font is null");
            return 0.0;
        }
        return ogc_font_get_size(font);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_draw_Font_nativeIsBold
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_font_t* font =
            static_cast<ogc_font_t*>(JniConverter::FromJLongPtr(ptr));
        if (!font) {
            JniException::ThrowNullPointerException(env, "Font is null");
            return JNI_FALSE;
        }
        return ogc_font_is_bold(font) ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_draw_Font_nativeIsItalic
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_font_t* font =
            static_cast<ogc_font_t*>(JniConverter::FromJLongPtr(ptr));
        if (!font) {
            JniException::ThrowNullPointerException(env, "Font is null");
            return JNI_FALSE;
        }
        return ogc_font_is_italic(font) ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_Font_nativeSetBold
  (JNIEnv* env, jobject obj, jlong ptr, jboolean bold) {
    try {
        ogc_font_t* font =
            static_cast<ogc_font_t*>(JniConverter::FromJLongPtr(ptr));
        if (!font) {
            JniException::ThrowNullPointerException(env, "Font is null");
            return;
        }
        ogc_font_set_bold(font, bold ? 1 : 0);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_Font_nativeSetItalic
  (JNIEnv* env, jobject obj, jlong ptr, jboolean italic) {
    try {
        ogc_font_t* font =
            static_cast<ogc_font_t*>(JniConverter::FromJLongPtr(ptr));
        if (!font) {
            JniException::ThrowNullPointerException(env, "Font is null");
            return;
        }
        ogc_font_set_italic(font, italic ? 1 : 0);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== Pen ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_Pen_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_pen_t* pen = ogc_pen_create();
        return JniConverter::ToJLongPtr(pen);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_Pen_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_pen_t* pen =
            static_cast<ogc_pen_t*>(JniConverter::FromJLongPtr(ptr));
        if (!pen) {
            return;
        }
        ogc_pen_destroy(pen);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_Pen_nativeGetColor
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_pen_t* pen =
            static_cast<ogc_pen_t*>(JniConverter::FromJLongPtr(ptr));
        if (!pen) {
            JniException::ThrowNullPointerException(env, "Pen is null");
            return 0;
        }
        ogc_color_t color = ogc_pen_get_color(pen);
        return (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_draw_Pen_nativeGetWidth
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_pen_t* pen =
            static_cast<ogc_pen_t*>(JniConverter::FromJLongPtr(ptr));
        if (!pen) {
            JniException::ThrowNullPointerException(env, "Pen is null");
            return 0.0;
        }
        return ogc_pen_get_width(pen);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_Pen_nativeSetColor
  (JNIEnv* env, jobject obj, jlong ptr, jint packedColor) {
    try {
        ogc_pen_t* pen =
            static_cast<ogc_pen_t*>(JniConverter::FromJLongPtr(ptr));
        if (!pen) {
            JniException::ThrowNullPointerException(env, "Pen is null");
            return;
        }
        ogc_color_t color;
        color.r = static_cast<unsigned char>((packedColor >> 24) & 0xFF);
        color.g = static_cast<unsigned char>((packedColor >> 16) & 0xFF);
        color.b = static_cast<unsigned char>((packedColor >> 8) & 0xFF);
        color.a = static_cast<unsigned char>(packedColor & 0xFF);
        ogc_pen_set_color(pen, color);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_Pen_nativeSetWidth
  (JNIEnv* env, jobject obj, jlong ptr, jdouble width) {
    try {
        ogc_pen_t* pen =
            static_cast<ogc_pen_t*>(JniConverter::FromJLongPtr(ptr));
        if (!pen) {
            JniException::ThrowNullPointerException(env, "Pen is null");
            return;
        }
        ogc_pen_set_width(pen, width);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_Pen_nativeGetStyle
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_pen_t* pen =
            static_cast<ogc_pen_t*>(JniConverter::FromJLongPtr(ptr));
        if (!pen) {
            JniException::ThrowNullPointerException(env, "Pen is null");
            return 0;
        }
        return static_cast<jint>(ogc_pen_get_style(pen));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_Pen_nativeSetStyle
  (JNIEnv* env, jobject obj, jlong ptr, jint style) {
    try {
        ogc_pen_t* pen =
            static_cast<ogc_pen_t*>(JniConverter::FromJLongPtr(ptr));
        if (!pen) {
            JniException::ThrowNullPointerException(env, "Pen is null");
            return;
        }
        ogc_pen_set_style(pen, static_cast<ogc_pen_style_e>(style));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== Brush ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_Brush_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_brush_t* brush = ogc_brush_create();
        return JniConverter::ToJLongPtr(brush);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_Brush_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_brush_t* brush =
            static_cast<ogc_brush_t*>(JniConverter::FromJLongPtr(ptr));
        if (!brush) {
            return;
        }
        ogc_brush_destroy(brush);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_Brush_nativeGetColor
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_brush_t* brush =
            static_cast<ogc_brush_t*>(JniConverter::FromJLongPtr(ptr));
        if (!brush) {
            JniException::ThrowNullPointerException(env, "Brush is null");
            return 0;
        }
        ogc_color_t color = ogc_brush_get_color(brush);
        return (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_Brush_nativeSetColor
  (JNIEnv* env, jobject obj, jlong ptr, jint packedColor) {
    try {
        ogc_brush_t* brush =
            static_cast<ogc_brush_t*>(JniConverter::FromJLongPtr(ptr));
        if (!brush) {
            JniException::ThrowNullPointerException(env, "Brush is null");
            return;
        }
        ogc_color_t color;
        color.r = static_cast<unsigned char>((packedColor >> 24) & 0xFF);
        color.g = static_cast<unsigned char>((packedColor >> 16) & 0xFF);
        color.b = static_cast<unsigned char>((packedColor >> 8) & 0xFF);
        color.a = static_cast<unsigned char>(packedColor & 0xFF);
        ogc_brush_set_color(brush, color);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_draw_Brush_nativeGetStyle
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_brush_t* brush =
            static_cast<ogc_brush_t*>(JniConverter::FromJLongPtr(ptr));
        if (!brush) {
            JniException::ThrowNullPointerException(env, "Brush is null");
            return 0;
        }
        return static_cast<jint>(ogc_brush_get_style(brush));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_Brush_nativeSetStyle
  (JNIEnv* env, jobject obj, jlong ptr, jint style) {
    try {
        ogc_brush_t* brush =
            static_cast<ogc_brush_t*>(JniConverter::FromJLongPtr(ptr));
        if (!brush) {
            JniException::ThrowNullPointerException(env, "Brush is null");
            return;
        }
        ogc_brush_set_style(brush, static_cast<ogc_brush_style_e>(style));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

/* ===== DrawStyle ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_DrawStyle_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_draw_style_t* style = ogc_draw_style_create();
        return JniConverter::ToJLongPtr(style);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_DrawStyle_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_draw_style_t* style =
            static_cast<ogc_draw_style_t*>(JniConverter::FromJLongPtr(ptr));
        if (!style) {
            return;
        }
        ogc_draw_style_destroy(style);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_DrawStyle_nativeGetPen
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_draw_style_t* style =
            static_cast<ogc_draw_style_t*>(JniConverter::FromJLongPtr(ptr));
        if (!style) {
            JniException::ThrowNullPointerException(env, "DrawStyle is null");
            return 0;
        }
        ogc_pen_t* pen = ogc_draw_style_get_pen(style);
        return JniConverter::ToJLongPtr(pen);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_DrawStyle_nativeGetBrush
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_draw_style_t* style =
            static_cast<ogc_draw_style_t*>(JniConverter::FromJLongPtr(ptr));
        if (!style) {
            JniException::ThrowNullPointerException(env, "DrawStyle is null");
            return 0;
        }
        ogc_brush_t* brush = ogc_draw_style_get_brush(style);
        return JniConverter::ToJLongPtr(brush);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_DrawStyle_nativeGetFont
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_draw_style_t* style =
            static_cast<ogc_draw_style_t*>(JniConverter::FromJLongPtr(ptr));
        if (!style) {
            JniException::ThrowNullPointerException(env, "DrawStyle is null");
            return 0;
        }
        ogc_font_t* font = ogc_draw_style_get_font(style);
        return JniConverter::ToJLongPtr(font);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_DrawStyle_nativeSetPen
  (JNIEnv* env, jobject obj, jlong ptr, jlong penPtr) {
    try {
        ogc_draw_style_t* style =
            static_cast<ogc_draw_style_t*>(JniConverter::FromJLongPtr(ptr));
        if (!style) {
            JniException::ThrowNullPointerException(env, "DrawStyle is null");
            return;
        }
        ogc_pen_t* pen =
            static_cast<ogc_pen_t*>(JniConverter::FromJLongPtr(penPtr));
        ogc_draw_style_set_pen(style, pen);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_DrawStyle_nativeSetBrush
  (JNIEnv* env, jobject obj, jlong ptr, jlong brushPtr) {
    try {
        ogc_draw_style_t* style =
            static_cast<ogc_draw_style_t*>(JniConverter::FromJLongPtr(ptr));
        if (!style) {
            JniException::ThrowNullPointerException(env, "DrawStyle is null");
            return;
        }
        ogc_brush_t* brush =
            static_cast<ogc_brush_t*>(JniConverter::FromJLongPtr(brushPtr));
        ogc_draw_style_set_brush(style, brush);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_draw_DrawStyle_nativeSetFont
  (JNIEnv* env, jobject obj, jlong ptr, jlong fontPtr) {
    try {
        ogc_draw_style_t* style =
            static_cast<ogc_draw_style_t*>(JniConverter::FromJLongPtr(ptr));
        if (!style) {
            JniException::ThrowNullPointerException(env, "DrawStyle is null");
            return;
        }
        ogc_font_t* font =
            static_cast<ogc_font_t*>(JniConverter::FromJLongPtr(fontPtr));
        ogc_draw_style_set_font(style, font);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jintArray JNICALL
Java_cn_cycle_chart_api_draw_Color_nativeToRgba
  (JNIEnv* env, jclass clazz, jlong ptr) {
    ogc_color_t* color =
        static_cast<ogc_color_t*>(JniConverter::FromJLongPtr(ptr));
    if (!color) { return nullptr; }
    unsigned char r = 0, g = 0, b = 0, a = 0;
    ogc_color_to_rgba(color, &r, &g, &b, &a);
    jintArray result = env->NewIntArray(4);
    if (!result) { return nullptr; }
    jint vals[4] = {r, g, b, a};
    env->SetIntArrayRegion(result, 0, 4, vals);
    return result;
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_Image_nativeGetDataPtr
  (JNIEnv* env, jobject obj, jlong ptr) {
    ogc_image_t* image =
        static_cast<ogc_image_t*>(JniConverter::FromJLongPtr(ptr));
    if (!image) { return 0; }
    unsigned char* data = ogc_image_get_data(image);
    return JniConverter::ToJLongPtr(data);
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_draw_Image_nativeGetDataConstPtr
  (JNIEnv* env, jobject obj, jlong ptr) {
    const ogc_image_t* image =
        static_cast<const ogc_image_t*>(JniConverter::FromJLongPtr(ptr));
    if (!image) { return 0; }
    const unsigned char* data = ogc_image_get_data_const(image);
    return JniConverter::ToJLongPtr(const_cast<unsigned char*>(data));
}

}
