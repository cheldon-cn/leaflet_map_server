#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_proj_CoordTransformer_nativeCreate
  (JNIEnv* env, jclass clazz, jstring sourceCrs, jstring targetCrs) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return 0;
    }

    try {
        std::string src = JniConverter::ToString(env, sourceCrs);
        std::string tgt = JniConverter::ToString(env, targetCrs);
        ogc_coord_transformer_t* transformer =
            ogc_coord_transformer_create(src.c_str(), tgt.c_str());
        return JniConverter::ToJLongPtr(transformer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_proj_CoordTransformer_nativeDestroy
  (JNIEnv* env, jclass clazz, jlong ptr) {
    try {
        ogc_coord_transformer_t* transformer =
            static_cast<ogc_coord_transformer_t*>(JniConverter::FromJLongPtr(ptr));
        if (transformer) {
            ogc_coord_transformer_destroy(transformer);
        }
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_proj_CoordTransformer_nativeIsValid
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_coord_transformer_t* transformer =
            static_cast<ogc_coord_transformer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!transformer) {
            return JNI_FALSE;
        }
        int valid = ogc_coord_transformer_is_valid(transformer);
        return valid ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT jobject JNICALL
Java_cn_cycle_chart_api_proj_CoordTransformer_nativeTransform
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y) {
    try {
        ogc_coord_transformer_t* transformer =
            static_cast<ogc_coord_transformer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!transformer) {
            JniException::ThrowNullPointerException(env, "CoordTransformer is null");
            return nullptr;
        }

        ogc_coordinate_t coord;
        coord.x = x;
        coord.y = y;
        coord.z = 0.0;
        coord.m = 0.0;

        ogc_coordinate_t result = ogc_coord_transformer_transform(transformer, &coord);
        return JniConverter::CreateCoordinate(env, result.x, result.y);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jobject JNICALL
Java_cn_cycle_chart_api_proj_CoordTransformer_nativeTransformInverse
  (JNIEnv* env, jobject obj, jlong ptr, jdouble x, jdouble y) {
    try {
        ogc_coord_transformer_t* transformer =
            static_cast<ogc_coord_transformer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!transformer) {
            JniException::ThrowNullPointerException(env, "CoordTransformer is null");
            return nullptr;
        }

        ogc_coordinate_t coord;
        coord.x = x;
        coord.y = y;
        coord.z = 0.0;
        coord.m = 0.0;

        ogc_coordinate_t result = ogc_coord_transformer_transform_inverse(transformer, &coord);
        return JniConverter::CreateCoordinate(env, result.x, result.y);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_proj_CoordTransformer_nativeTransformArray
  (JNIEnv* env, jobject obj, jlong ptr, jdoubleArray xArr, jdoubleArray yArr) {
    try {
        ogc_coord_transformer_t* transformer =
            static_cast<ogc_coord_transformer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!transformer) {
            JniException::ThrowNullPointerException(env, "CoordTransformer is null");
            return;
        }

        jsize xLen = env->GetArrayLength(xArr);
        jsize yLen = env->GetArrayLength(yArr);
        if (xLen != yLen || xLen <= 0) {
            JniException::ThrowIllegalArgumentException(env, "Array length mismatch or zero");
            return;
        }

        jdouble* xElems = env->GetDoubleArrayElements(xArr, nullptr);
        jdouble* yElems = env->GetDoubleArrayElements(yArr, nullptr);
        if (!xElems || !yElems) {
            if (xElems) env->ReleaseDoubleArrayElements(xArr, xElems, JNI_ABORT);
            if (yElems) env->ReleaseDoubleArrayElements(yArr, yElems, JNI_ABORT);
            JniException::ThrowOutOfMemoryError(env, "Failed to get array elements");
            return;
        }

        ogc_coord_transformer_transform_array(transformer, xElems, yElems,
            static_cast<size_t>(xLen));

        env->ReleaseDoubleArrayElements(xArr, xElems, 0);
        env->ReleaseDoubleArrayElements(yArr, yElems, 0);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jobject JNICALL
Java_cn_cycle_chart_api_proj_CoordTransformer_nativeTransformEnvelope
  (JNIEnv* env, jobject obj, jlong ptr, jdouble minX, jdouble minY,
   jdouble maxX, jdouble maxY) {
    try {
        ogc_coord_transformer_t* transformer =
            static_cast<ogc_coord_transformer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!transformer) {
            JniException::ThrowNullPointerException(env, "CoordTransformer is null");
            return nullptr;
        }

        ogc_envelope_t* envIn = ogc_envelope_create_from_coords(minX, minY, maxX, maxY);
        if (!envIn) {
            JniException::ThrowRuntimeException(env, "Failed to create envelope");
            return nullptr;
        }

        ogc_envelope_t* envOut = ogc_coord_transformer_transform_envelope(transformer, envIn);
        ogc_envelope_destroy(envIn);

        if (!envOut) {
            return nullptr;
        }

        jobject result = JniConverter::CreateEnvelope(env,
            ogc_envelope_get_min_x(envOut), ogc_envelope_get_min_y(envOut),
            ogc_envelope_get_max_x(envOut), ogc_envelope_get_max_y(envOut));
        ogc_envelope_destroy(envOut);
        return result;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_proj_CoordTransformer_nativeTransformGeometry
  (JNIEnv* env, jobject obj, jlong ptr, jlong geomPtr) {
    try {
        ogc_coord_transformer_t* transformer =
            static_cast<ogc_coord_transformer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!transformer) {
            JniException::ThrowNullPointerException(env, "CoordTransformer is null");
            return 0;
        }

        ogc_geometry_t* geom =
            static_cast<ogc_geometry_t*>(JniConverter::FromJLongPtr(geomPtr));
        if (!geom) {
            JniException::ThrowNullPointerException(env, "Geometry is null");
            return 0;
        }

        ogc_geometry_t* result = ogc_coord_transformer_transform_geometry(transformer, geom);
        return JniConverter::ToJLongPtr(result);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_proj_CoordTransformer_nativeGetSourceCrs
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_coord_transformer_t* transformer =
            static_cast<ogc_coord_transformer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!transformer) {
            JniException::ThrowNullPointerException(env, "CoordTransformer is null");
            return nullptr;
        }

        const char* crs = ogc_coord_transformer_get_source_crs(transformer);
        return crs ? JniConverter::ToJString(env, std::string(crs)) : nullptr;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_proj_CoordTransformer_nativeGetTargetCrs
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_coord_transformer_t* transformer =
            static_cast<ogc_coord_transformer_t*>(JniConverter::FromJLongPtr(ptr));
        if (!transformer) {
            JniException::ThrowNullPointerException(env, "CoordTransformer is null");
            return nullptr;
        }

        const char* crs = ogc_coord_transformer_get_target_crs(transformer);
        return crs ? JniConverter::ToJString(env, std::string(crs)) : nullptr;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

}
