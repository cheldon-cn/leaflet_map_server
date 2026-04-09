#ifndef JNI_CONVERTER_H
#define JNI_CONVERTER_H

#include <jni.h>
#include <string>
#include <stdint.h>

namespace ogc {
namespace jni {

class JniConverter {
public:
    static jlong ToJLongPtr(void* ptr);
    static void* FromJLongPtr(jlong ptr);

    static std::string ToString(JNIEnv* env, jstring str);
    static jstring ToJString(JNIEnv* env, const std::string& str);

    static jobject CreateCoordinate(JNIEnv* env, double x, double y);
    static jobject CreateCoordinate3D(JNIEnv* env, double x, double y, double z);
    static jobject CreateEnvelope(JNIEnv* env, double minX, double minY,
                                  double maxX, double maxY);
    static jobject CreateColor(JNIEnv* env, uint8_t r, uint8_t g,
                               uint8_t b, uint8_t a);

    static bool ToBool(JNIEnv* env, jboolean val);
    static jboolean ToJBoolean(bool val);
};

}
}

#endif
