#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_transform_matrix_t* matrix = ogc_transform_matrix_create();
        return JniConverter::ToJLongPtr(matrix);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeCreateIdentity
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_transform_matrix_t* matrix = ogc_transform_matrix_create_identity();
        return JniConverter::ToJLongPtr(matrix);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeCreateTranslation
  (JNIEnv* env, jclass clazz, jdouble tx, jdouble ty) {
    try {
        ogc_transform_matrix_t* matrix = ogc_transform_matrix_create_translation(tx, ty);
        return JniConverter::ToJLongPtr(matrix);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeCreateScale
  (JNIEnv* env, jclass clazz, jdouble sx, jdouble sy) {
    try {
        ogc_transform_matrix_t* matrix = ogc_transform_matrix_create_scale(sx, sy);
        return JniConverter::ToJLongPtr(matrix);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeCreateRotation
  (JNIEnv* env, jclass clazz, jdouble angle) {
    try {
        ogc_transform_matrix_t* matrix = ogc_transform_matrix_create_rotation(angle);
        return JniConverter::ToJLongPtr(matrix);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_transform_matrix_t* matrix =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(ptr));
        if (!matrix) { return; }
        ogc_transform_matrix_destroy(matrix);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeMultiply
  (JNIEnv* env, jobject obj, jlong aPtr, jlong bPtr) {
    try {
        ogc_transform_matrix_t* a =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(aPtr));
        ogc_transform_matrix_t* b =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(bPtr));
        if (!a || !b) {
            JniException::ThrowNullPointerException(env, "TransformMatrix is null");
            return 0;
        }
        ogc_transform_matrix_t* result = ogc_transform_matrix_multiply_new(a, b);
        return JniConverter::ToJLongPtr(result);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeInverse
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_transform_matrix_t* matrix =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(ptr));
        if (!matrix) {
            JniException::ThrowNullPointerException(env, "TransformMatrix is null");
            return 0;
        }
        ogc_transform_matrix_t* result = ogc_transform_matrix_inverse(matrix);
        return JniConverter::ToJLongPtr(result);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeTransformPoint
  (JNIEnv* env, jobject obj, jlong ptr, jdoubleArray xyArr) {
    try {
        ogc_transform_matrix_t* matrix =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(ptr));
        if (!matrix) {
            JniException::ThrowNullPointerException(env, "TransformMatrix is null");
            return;
        }
        jdouble* xy = env->GetDoubleArrayElements(xyArr, nullptr);
        if (!xy) { return; }
        ogc_transform_matrix_transform_point(matrix, &xy[0], &xy[1]);
        env->ReleaseDoubleArrayElements(xyArr, xy, 0);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jdoubleArray JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeGetElements
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_transform_matrix_t* matrix =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(ptr));
        if (!matrix) { return nullptr; }
        double elements[9] = {0};
        ogc_transform_matrix_get_elements(matrix, elements);
        jdoubleArray result = env->NewDoubleArray(9);
        if (!result) { return nullptr; }
        env->SetDoubleArrayRegion(result, 0, 9, elements);
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeSetElements
  (JNIEnv* env, jobject obj, jlong ptr, jdoubleArray elementsArr) {
    try {
        ogc_transform_matrix_t* matrix =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(ptr));
        if (!matrix) {
            JniException::ThrowNullPointerException(env, "TransformMatrix is null");
            return;
        }
        jdouble* elements = env->GetDoubleArrayElements(elementsArr, nullptr);
        if (!elements) { return; }
        ogc_transform_matrix_set_elements(matrix, elements);
        env->ReleaseDoubleArrayElements(elementsArr, elements, JNI_ABORT);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeTranslate
  (JNIEnv* env, jobject obj, jlong ptr, jdouble dx, jdouble dy) {
    try {
        ogc_transform_matrix_t* matrix =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(ptr));
        if (!matrix) {
            JniException::ThrowNullPointerException(env, "TransformMatrix is null");
            return;
        }
        ogc_transform_matrix_translate(matrix, dx, dy);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeScale
  (JNIEnv* env, jobject obj, jlong ptr, jdouble sx, jdouble sy) {
    try {
        ogc_transform_matrix_t* matrix =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(ptr));
        if (!matrix) {
            JniException::ThrowNullPointerException(env, "TransformMatrix is null");
            return;
        }
        ogc_transform_matrix_scale(matrix, sx, sy);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeRotate
  (JNIEnv* env, jobject obj, jlong ptr, jdouble angleDegrees) {
    try {
        ogc_transform_matrix_t* matrix =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(ptr));
        if (!matrix) {
            JniException::ThrowNullPointerException(env, "TransformMatrix is null");
            return;
        }
        ogc_transform_matrix_rotate(matrix, angleDegrees);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeMultiplyInPlace
  (JNIEnv* env, jobject obj, jlong resultPtr, jlong aPtr, jlong bPtr) {
    try {
        ogc_transform_matrix_t* result =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(resultPtr));
        ogc_transform_matrix_t* a =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(aPtr));
        ogc_transform_matrix_t* b =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(bPtr));
        if (!result || !a || !b) {
            JniException::ThrowNullPointerException(env, "TransformMatrix is null");
            return;
        }
        ogc_transform_matrix_multiply(result, a, b);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jdoubleArray JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeTransformCoord
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y) {
    try {
        ogc_transform_matrix_t* matrix =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(ptr));
        if (!matrix) { return nullptr; }
        ogc_coordinate_t coord;
        coord.x = x;
        coord.y = y;
        ogc_coordinate_t result = ogc_transform_matrix_transform(matrix, &coord);
        jdoubleArray arr = env->NewDoubleArray(2);
        if (!arr) { return nullptr; }
        jdouble vals[2] = {result.x, result.y};
        env->SetDoubleArrayRegion(arr, 0, 2, vals);
        return arr;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jdoubleArray JNICALL
Java_cn_cycle_chart_api_proj_TransformMatrix_nativeGetValues
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_transform_matrix_t* matrix =
            static_cast<ogc_transform_matrix_t*>(JniConverter::FromJLongPtr(ptr));
        if (!matrix) { return nullptr; }
        double values[9] = {0};
        ogc_transform_matrix_get_values(matrix, values);
        jdoubleArray result = env->NewDoubleArray(9);
        if (!result) { return nullptr; }
        env->SetDoubleArrayRegion(result, 0, 9, values);
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

}
