#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

/* ===== Track ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_Track_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_track_t* track = ogc_track_create();
        return JniConverter::ToJLongPtr(track);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Track_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_track_t* track =
            static_cast<ogc_track_t*>(JniConverter::FromJLongPtr(ptr));
        if (!track) {
            return;
        }
        ogc_track_destroy(track);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_navi_Track_nativeGetId
  (JNIEnv* env, jobject obj, jlong ptr) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return nullptr;
    }

    try {
        ogc_track_t* track =
            static_cast<ogc_track_t*>(JniConverter::FromJLongPtr(ptr));
        if (!track) {
            JniException::ThrowNullPointerException(env, "Track is null");
            return nullptr;
        }
        char buffer[256] = {0};
        ogc_track_get_id(track, buffer, sizeof(buffer));
        return env->NewStringUTF(buffer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT jstring JNICALL
Java_cn_cycle_chart_api_navi_Track_nativeGetName
  (JNIEnv* env, jobject obj, jlong ptr) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return nullptr;
    }

    try {
        ogc_track_t* track =
            static_cast<ogc_track_t*>(JniConverter::FromJLongPtr(ptr));
        if (!track) {
            JniException::ThrowNullPointerException(env, "Track is null");
            return nullptr;
        }
        char buffer[256] = {0};
        ogc_track_get_name(track, buffer, sizeof(buffer));
        return env->NewStringUTF(buffer);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return nullptr;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Track_nativeSetName
  (JNIEnv* env, jobject obj, jlong ptr, jstring name) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_track_t* track =
            static_cast<ogc_track_t*>(JniConverter::FromJLongPtr(ptr));
        if (!track) {
            JniException::ThrowNullPointerException(env, "Track is null");
            return;
        }
        const char* nameStr = env->GetStringUTFChars(name, nullptr);
        if (!nameStr) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get string");
            return;
        }
        ogc_track_set_name(track, nameStr);
        env->ReleaseStringUTFChars(name, nameStr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_Track_nativeGetPointCount
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_track_t* track =
            static_cast<ogc_track_t*>(JniConverter::FromJLongPtr(ptr));
        if (!track) {
            JniException::ThrowNullPointerException(env, "Track is null");
            return 0;
        }
        return static_cast<jlong>(ogc_track_get_point_count(track));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_Track_nativeGetPointPtr
  (JNIEnv* env, jobject obj, jlong ptr, jlong index) {
    try {
        ogc_track_t* track =
            static_cast<ogc_track_t*>(JniConverter::FromJLongPtr(ptr));
        if (!track) {
            JniException::ThrowNullPointerException(env, "Track is null");
            return 0;
        }
        ogc_track_point_t* point = ogc_track_get_point(track, static_cast<size_t>(index));
        return JniConverter::ToJLongPtr(point);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Track_nativeAddPoint
  (JNIEnv* env, jobject obj, jlong ptr,
   jdouble lat, jdouble lon, jdouble speed, jdouble course, jdouble timestamp) {
    try {
        ogc_track_t* track =
            static_cast<ogc_track_t*>(JniConverter::FromJLongPtr(ptr));
        if (!track) {
            JniException::ThrowNullPointerException(env, "Track is null");
            return;
        }
        ogc_track_add_point(track, lat, lon, speed, course, timestamp);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_Track_nativeClear
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_track_t* track =
            static_cast<ogc_track_t*>(JniConverter::FromJLongPtr(ptr));
        if (!track) {
            JniException::ThrowNullPointerException(env, "Track is null");
            return;
        }
        ogc_track_clear(track);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_Track_nativeToLineString
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_track_t* track =
            static_cast<ogc_track_t*>(JniConverter::FromJLongPtr(ptr));
        if (!track) {
            JniException::ThrowNullPointerException(env, "Track is null");
            return 0;
        }
        ogc_geometry_t* geom = ogc_track_to_linestring(track);
        return JniConverter::ToJLongPtr(geom);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

/* ===== TrackRecorder ===== */

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_TrackRecorder_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_track_recorder_t* recorder = ogc_track_recorder_create();
        return JniConverter::ToJLongPtr(recorder);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_TrackRecorder_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_track_recorder_t* recorder =
            static_cast<ogc_track_recorder_t*>(JniConverter::FromJLongPtr(ptr));
        if (!recorder) {
            return;
        }
        ogc_track_recorder_destroy(recorder);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_TrackRecorder_nativeStart
  (JNIEnv* env, jobject obj, jlong ptr, jstring trackName) {
    JniLocalRefScope scope(env);
    if (!scope.Success()) {
        JniException::ThrowOutOfMemoryError(env, "Failed to create local frame");
        return;
    }

    try {
        ogc_track_recorder_t* recorder =
            static_cast<ogc_track_recorder_t*>(JniConverter::FromJLongPtr(ptr));
        if (!recorder) {
            JniException::ThrowNullPointerException(env, "TrackRecorder is null");
            return;
        }
        const char* nameStr = env->GetStringUTFChars(trackName, nullptr);
        if (!nameStr) {
            JniException::ThrowOutOfMemoryError(env, "Failed to get string");
            return;
        }
        ogc_track_recorder_start(recorder, nameStr);
        env->ReleaseStringUTFChars(trackName, nameStr);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_TrackRecorder_nativeStop
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_track_recorder_t* recorder =
            static_cast<ogc_track_recorder_t*>(JniConverter::FromJLongPtr(ptr));
        if (!recorder) {
            JniException::ThrowNullPointerException(env, "TrackRecorder is null");
            return;
        }
        ogc_track_recorder_stop(recorder);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_TrackRecorder_nativePause
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_track_recorder_t* recorder =
            static_cast<ogc_track_recorder_t*>(JniConverter::FromJLongPtr(ptr));
        if (!recorder) {
            JniException::ThrowNullPointerException(env, "TrackRecorder is null");
            return;
        }
        ogc_track_recorder_pause(recorder);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_TrackRecorder_nativeResume
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_track_recorder_t* recorder =
            static_cast<ogc_track_recorder_t*>(JniConverter::FromJLongPtr(ptr));
        if (!recorder) {
            JniException::ThrowNullPointerException(env, "TrackRecorder is null");
            return;
        }
        ogc_track_recorder_resume(recorder);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_navi_TrackRecorder_nativeIsRecording
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_track_recorder_t* recorder =
            static_cast<ogc_track_recorder_t*>(JniConverter::FromJLongPtr(ptr));
        if (!recorder) {
            JniException::ThrowNullPointerException(env, "TrackRecorder is null");
            return JNI_FALSE;
        }
        return ogc_track_recorder_is_recording(recorder) ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_TrackRecorder_nativeGetCurrentTrack
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_track_recorder_t* recorder =
            static_cast<ogc_track_recorder_t*>(JniConverter::FromJLongPtr(ptr));
        if (!recorder) {
            JniException::ThrowNullPointerException(env, "TrackRecorder is null");
            return 0;
        }
        ogc_track_t* track = ogc_track_recorder_get_current_track(recorder);
        return JniConverter::ToJLongPtr(track);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_TrackRecorder_nativeSetInterval
  (JNIEnv* env, jobject obj, jlong ptr, jint seconds) {
    try {
        ogc_track_recorder_t* recorder =
            static_cast<ogc_track_recorder_t*>(JniConverter::FromJLongPtr(ptr));
        if (!recorder) {
            JniException::ThrowNullPointerException(env, "TrackRecorder is null");
            return;
        }
        ogc_track_recorder_set_interval(recorder, static_cast<int>(seconds));
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_TrackRecorder_nativeSetMinDistance
  (JNIEnv* env, jobject obj, jlong ptr, jdouble meters) {
    try {
        ogc_track_recorder_t* recorder =
            static_cast<ogc_track_recorder_t*>(JniConverter::FromJLongPtr(ptr));
        if (!recorder) {
            JniException::ThrowNullPointerException(env, "TrackRecorder is null");
            return;
        }
        ogc_track_recorder_set_min_distance(recorder, meters);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

}
