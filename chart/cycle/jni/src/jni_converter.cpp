#include "jni_converter.h"
#include "jni_exception.h"

namespace ogc {
namespace jni {

jlong JniConverter::ToJLongPtr(void* ptr) {
    return static_cast<jlong>(reinterpret_cast<intptr_t>(ptr));
}

void* JniConverter::FromJLongPtr(jlong ptr) {
    return reinterpret_cast<void*>(static_cast<intptr_t>(ptr));
}

std::string JniConverter::ToString(JNIEnv* env, jstring str) {
    if (!str) {
        return "";
    }

    const char* chars = env->GetStringUTFChars(str, nullptr);
    if (!chars) {
        return "";
    }

    std::string result(chars);
    env->ReleaseStringUTFChars(str, chars);
    return result;
}

jstring JniConverter::ToJString(JNIEnv* env, const std::string& str) {
    if (str.empty()) {
        return env->NewStringUTF("");
    }
    return env->NewStringUTF(str.c_str());
}

jobject JniConverter::CreateCoordinate(JNIEnv* env, double x, double y) {
    jclass coordClass = env->FindClass("cn/cycle/chart/api/geometry/Coordinate");
    if (!coordClass) {
        return nullptr;
    }

    jmethodID constructor = env->GetMethodID(coordClass, "<init>", "(DD)V");
    if (!constructor) {
        return nullptr;
    }

    return env->NewObject(coordClass, constructor, x, y);
}

jobject JniConverter::CreateCoordinate3D(JNIEnv* env, double x, double y, double z) {
    jclass coordClass = env->FindClass("cn/cycle/chart/api/geometry/Coordinate");
    if (!coordClass) {
        return nullptr;
    }

    jmethodID constructor = env->GetMethodID(coordClass, "<init>", "(DDD)V");
    if (!constructor) {
        return nullptr;
    }

    return env->NewObject(coordClass, constructor, x, y, z);
}

jobject JniConverter::CreateEnvelope(JNIEnv* env, double minX, double minY,
                                     double maxX, double maxY) {
    jclass envClass = env->FindClass("cn/cycle/chart/api/geometry/Envelope");
    if (!envClass) {
        return nullptr;
    }

    jmethodID constructor = env->GetMethodID(envClass, "<init>", "(DDDD)V");
    if (!constructor) {
        return nullptr;
    }

    return env->NewObject(envClass, constructor, minX, minY, maxX, maxY);
}

jobject JniConverter::CreateColor(JNIEnv* env, uint8_t r, uint8_t g,
                                  uint8_t b, uint8_t a) {
    jclass colorClass = env->FindClass("cn/cycle/chart/api/util/Color");
    if (!colorClass) {
        return nullptr;
    }

    jmethodID constructor = env->GetMethodID(colorClass, "<init>", "(IIII)V");
    if (!constructor) {
        return nullptr;
    }

    return env->NewObject(colorClass, constructor,
                          static_cast<jint>(r), static_cast<jint>(g),
                          static_cast<jint>(b), static_cast<jint>(a));
}

bool JniConverter::ToBool(JNIEnv* env, jboolean val) {
    return val == JNI_TRUE;
}

jboolean JniConverter::ToJBoolean(bool val) {
    return val ? JNI_TRUE : JNI_FALSE;
}

}
}
