#include "jni_env_manager.h"
#include "jni_converter.h"
#include "jni_exception.h"
#include "jni_memory.h"
#include "ogc/capi/sdk_c_api.h"

using namespace ogc::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_CollisionAssessor_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_collision_assessor_t* assessor = ogc_collision_assessor_create();
        return JniConverter::ToJLongPtr(assessor);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_CollisionAssessor_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_collision_assessor_t* assessor =
            static_cast<ogc_collision_assessor_t*>(JniConverter::FromJLongPtr(ptr));
        if (!assessor) { return; }
        ogc_collision_assessor_destroy(assessor);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_CollisionAssessor_nativeAssess
  (JNIEnv* env, jobject obj, jlong ptr,
   jdouble ownLat, jdouble ownLon, jdouble ownSpeed, jdouble ownCourse,
   jlong targetPtr) {
    try {
        ogc_collision_assessor_t* assessor =
            static_cast<ogc_collision_assessor_t*>(JniConverter::FromJLongPtr(ptr));
        if (!assessor) {
            JniException::ThrowNullPointerException(env, "CollisionAssessor is null");
            return 0;
        }
        ogc_ais_target_t* target =
            static_cast<ogc_ais_target_t*>(JniConverter::FromJLongPtr(targetPtr));
        if (!target) {
            JniException::ThrowNullPointerException(env, "AIS target is null");
            return 0;
        }
        ogc_collision_risk_t* risk = ogc_collision_assessor_assess(
            assessor, ownLat, ownLon, ownSpeed, ownCourse, target);
        return JniConverter::ToJLongPtr(risk);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_CollisionAssessor_nativeDestroyRisk
  (JNIEnv* env, jclass clazz, jlong riskPtr) {
    try {
        ogc_collision_risk_t* risk =
            static_cast<ogc_collision_risk_t*>(JniConverter::FromJLongPtr(riskPtr));
        if (!risk) { return; }
        ogc_collision_risk_destroy(risk);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_CollisionAssessor_nativeGetRiskCpa
  (JNIEnv* env, jclass clazz, jlong riskPtr) {
    try {
        ogc_collision_risk_t* risk =
            static_cast<ogc_collision_risk_t*>(JniConverter::FromJLongPtr(riskPtr));
        if (!risk) { return 0.0; }
        return risk->cpa;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_CollisionAssessor_nativeGetRiskTcpa
  (JNIEnv* env, jclass clazz, jlong riskPtr) {
    try {
        ogc_collision_risk_t* risk =
            static_cast<ogc_collision_risk_t*>(JniConverter::FromJLongPtr(riskPtr));
        if (!risk) { return 0.0; }
        return risk->tcpa;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

JNIEXPORT jint JNICALL
Java_cn_cycle_chart_api_navi_CollisionAssessor_nativeGetRiskLevel
  (JNIEnv* env, jclass clazz, jlong riskPtr) {
    try {
        ogc_collision_risk_t* risk =
            static_cast<ogc_collision_risk_t*>(JniConverter::FromJLongPtr(riskPtr));
        if (!risk) { return 0; }
        return risk->risk_level;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT jlong JNICALL
Java_cn_cycle_chart_api_navi_OffCourseDetector_nativeCreate
  (JNIEnv* env, jclass clazz) {
    try {
        ogc_off_course_detector_t* detector = ogc_off_course_detector_create();
        return JniConverter::ToJLongPtr(detector);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0;
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_OffCourseDetector_nativeDestroy
  (JNIEnv* env, jobject obj, jlong ptr) {
    try {
        ogc_off_course_detector_t* detector =
            static_cast<ogc_off_course_detector_t*>(JniConverter::FromJLongPtr(ptr));
        if (!detector) { return; }
        ogc_off_course_detector_destroy(detector);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT void JNICALL
Java_cn_cycle_chart_api_navi_OffCourseDetector_nativeSetThreshold
  (JNIEnv* env, jobject obj, jlong ptr, jdouble meters) {
    try {
        ogc_off_course_detector_t* detector =
            static_cast<ogc_off_course_detector_t*>(JniConverter::FromJLongPtr(ptr));
        if (!detector) {
            JniException::ThrowNullPointerException(env, "OffCourseDetector is null");
            return;
        }
        ogc_off_course_detector_set_threshold(detector, meters);
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
    }
}

JNIEXPORT jboolean JNICALL
Java_cn_cycle_chart_api_navi_OffCourseDetector_nativeCheck
  (JNIEnv* env, jobject obj, jlong ptr,
   jdouble currentLat, jdouble currentLon,
   jlong fromWaypointPtr, jlong toWaypointPtr) {
    try {
        ogc_off_course_detector_t* detector =
            static_cast<ogc_off_course_detector_t*>(JniConverter::FromJLongPtr(ptr));
        if (!detector) {
            JniException::ThrowNullPointerException(env, "OffCourseDetector is null");
            return JNI_FALSE;
        }
        ogc_waypoint_t* from =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(fromWaypointPtr));
        ogc_waypoint_t* to =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(toWaypointPtr));
        if (!from || !to) {
            JniException::ThrowNullPointerException(env, "Waypoint is null");
            return JNI_FALSE;
        }
        ogc_deviation_result_t result = ogc_off_course_detector_check(
            detector, currentLat, currentLon, from, to);
        return result.is_off_course ? JNI_TRUE : JNI_FALSE;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return JNI_FALSE;
    }
}

JNIEXPORT jdouble JNICALL
Java_cn_cycle_chart_api_navi_OffCourseDetector_nativeGetCrossTrackError
  (JNIEnv* env, jobject obj, jlong ptr,
   jdouble currentLat, jdouble currentLon,
   jlong fromWaypointPtr, jlong toWaypointPtr) {
    try {
        ogc_off_course_detector_t* detector =
            static_cast<ogc_off_course_detector_t*>(JniConverter::FromJLongPtr(ptr));
        if (!detector) { return 0.0; }
        ogc_waypoint_t* from =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(fromWaypointPtr));
        ogc_waypoint_t* to =
            static_cast<ogc_waypoint_t*>(JniConverter::FromJLongPtr(toWaypointPtr));
        if (!from || !to) { return 0.0; }
        ogc_deviation_result_t result = ogc_off_course_detector_check(
            detector, currentLat, currentLon, from, to);
        return result.cross_track_error;
    } catch (const std::exception& e) {
        JniException::TranslateAndThrow(env, e);
        return 0.0;
    }
}

}
