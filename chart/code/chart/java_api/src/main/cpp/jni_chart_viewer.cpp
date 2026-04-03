#include <jni.h>
#include <android/log.h>
#include <string>

#define LOG_TAG "ChartJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" {

JNIEXPORT jlong JNICALL
Java_ogc_chart_ChartViewer_nativeCreate(JNIEnv* env, jobject thiz, jobject context) {
    LOGI("Creating ChartViewer native instance");
    return reinterpret_cast<jlong>(new ChartViewerNative());
}

JNIEXPORT void JNICALL
Java_ogc_chart_ChartViewer_nativeDestroy(JNIEnv* env, jobject thiz, jlong ptr) {
    LOGI("Destroying ChartViewer native instance");
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (viewer) {
        delete viewer;
    }
}

JNIEXPORT void JNICALL
Java_ogc_chart_ChartViewer_nativeSetRenderCallback(JNIEnv* env, jobject thiz, jlong ptr, jobject callback) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (viewer && callback) {
        viewer->SetRenderCallback(env, callback);
    }
}

JNIEXPORT void JNICALL
Java_ogc_chart_ChartViewer_nativeSetLoadCallback(JNIEnv* env, jobject thiz, jlong ptr, jobject callback) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (viewer && callback) {
        viewer->SetLoadCallback(env, callback);
    }
}

JNIEXPORT void JNICALL
Java_ogc_chart_ChartViewer_nativeSetTouchCallback(JNIEnv* env, jobject thiz, jlong ptr, jobject callback) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (viewer && callback) {
        viewer->SetTouchCallback(env, callback);
    }
}

JNIEXPORT jboolean JNICALL
Java_ogc_chart_ChartViewer_nativeLoadChart(JNIEnv* env, jobject thiz, jlong ptr, jstring file_path) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (!viewer || !file_path) {
        return JNI_FALSE;
    }
    
    const char* path = env->GetStringUTFChars(file_path, nullptr);
    bool result = viewer->LoadChart(path);
    env->ReleaseStringUTFChars(file_path, path);
    
    return result ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_ogc_chart_ChartViewer_nativeUnloadChart(JNIEnv* env, jobject thiz, jlong ptr, jstring chart_id) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (viewer && chart_id) {
        const char* id = env->GetStringUTFChars(chart_id, nullptr);
        viewer->UnloadChart(id);
        env->ReleaseStringUTFChars(chart_id, id);
    }
}

JNIEXPORT void JNICALL
Java_ogc_chart_ChartViewer_nativeSetViewBounds(JNIEnv* env, jobject thiz, jlong ptr,
                                                jdouble min_lon, jdouble min_lat,
                                                jdouble max_lon, jdouble max_lat) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (viewer) {
        viewer->SetViewBounds(min_lon, min_lat, max_lon, max_lat);
    }
}

JNIEXPORT void JNICALL
Java_ogc_chart_ChartViewer_nativeSetCenter(JNIEnv* env, jobject thiz, jlong ptr,
                                           jdouble lon, jdouble lat) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (viewer) {
        viewer->SetCenter(lon, lat);
    }
}

JNIEXPORT jdouble JNICALL
Java_ogc_chart_ChartViewer_nativeGetCenterLon(JNIEnv* env, jobject thiz, jlong ptr) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    return viewer ? viewer->GetCenterLon() : 0.0;
}

JNIEXPORT jdouble JNICALL
Java_ogc_chart_ChartViewer_nativeGetCenterLat(JNIEnv* env, jobject thiz, jlong ptr) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    return viewer ? viewer->GetCenterLat() : 0.0;
}

JNIEXPORT void JNICALL
Java_ogc_chart_ChartViewer_nativeSetZoomLevel(JNIEnv* env, jobject thiz, jlong ptr, jint level) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (viewer) {
        viewer->SetZoomLevel(level);
    }
}

JNIEXPORT jint JNICALL
Java_ogc_chart_ChartViewer_nativeGetZoomLevel(JNIEnv* env, jobject thiz, jlong ptr) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    return viewer ? viewer->GetZoomLevel() : 1;
}

JNIEXPORT void JNICALL
Java_ogc_chart_ChartViewer_nativeSetRotation(JNIEnv* env, jobject thiz, jlong ptr, jfloat angle) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (viewer) {
        viewer->SetRotation(angle);
    }
}

JNIEXPORT jfloat JNICALL
Java_ogc_chart_ChartViewer_nativeGetRotation(JNIEnv* env, jobject thiz, jlong ptr) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    return viewer ? viewer->GetRotation() : 0.0f;
}

JNIEXPORT void JNICALL
Java_ogc_chart_ChartViewer_nativeRender(JNIEnv* env, jobject thiz, jlong ptr) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (viewer) {
        viewer->Render();
    }
}

JNIEXPORT void JNICALL
Java_ogc_chart_ChartViewer_nativeSetSurface(JNIEnv* env, jobject thiz, jlong ptr, jobject surface) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (viewer) {
        viewer->SetSurface(env, surface);
    }
}

JNIEXPORT void JNICALL
Java_ogc_chart_ChartViewer_nativeSetViewport(JNIEnv* env, jobject thiz, jlong ptr,
                                             jint width, jint height) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (viewer) {
        viewer->SetViewport(width, height);
    }
}

JNIEXPORT jdoubleArray JNICALL
Java_ogc_chart_ChartViewer_nativeScreenToGeo(JNIEnv* env, jobject thiz, jlong ptr,
                                             jfloat screen_x, jfloat screen_y) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (!viewer) {
        return nullptr;
    }
    
    double lon, lat;
    viewer->ScreenToGeo(screen_x, screen_y, lon, lat);
    
    jdoubleArray result = env->NewDoubleArray(2);
    if (result) {
        jdouble coords[] = {lon, lat};
        env->SetDoubleArrayRegion(result, 0, 2, coords);
    }
    return result;
}

JNIEXPORT jfloatArray JNICALL
Java_ogc_chart_ChartViewer_nativeGeoToScreen(JNIEnv* env, jobject thiz, jlong ptr,
                                             jdouble lon, jdouble lat) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (!viewer) {
        return nullptr;
    }
    
    float x, y;
    viewer->GeoToScreen(lon, lat, x, y);
    
    jfloatArray result = env->NewFloatArray(2);
    if (result) {
        jfloat coords[] = {x, y};
        env->SetFloatArrayRegion(result, 0, 2, coords);
    }
    return result;
}

JNIEXPORT jobjectArray JNICALL
Java_ogc_chart_ChartViewer_nativeQueryFeatures(JNIEnv* env, jobject thiz, jlong ptr,
                                               jfloat screen_x, jfloat screen_y) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (!viewer) {
        return nullptr;
    }
    
    return viewer->QueryFeatures(env, screen_x, screen_y);
}

JNIEXPORT void JNICALL
Java_ogc_chart_ChartViewer_nativeSetDisplayStyle(JNIEnv* env, jobject thiz, jlong ptr,
                                                  jstring style_name) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (viewer && style_name) {
        const char* style = env->GetStringUTFChars(style_name, nullptr);
        viewer->SetDisplayStyle(style);
        env->ReleaseStringUTFChars(style_name, style);
    }
}

JNIEXPORT void JNICALL
Java_ogc_chart_ChartViewer_nativeSetLayerVisible(JNIEnv* env, jobject thiz, jlong ptr,
                                                  jstring layer_name, jboolean visible) {
    ChartViewerNative* viewer = reinterpret_cast<ChartViewerNative*>(ptr);
    if (viewer && layer_name) {
        const char* layer = env->GetStringUTFChars(layer_name, nullptr);
        viewer->SetLayerVisible(layer, visible == JNI_TRUE);
        env->ReleaseStringUTFChars(layer_name, layer);
    }
}

JNIEXPORT jstring JNICALL
Java_ogc_chart_ChartViewer_nativeGetVersion(JNIEnv* env, jclass clazz) {
    return env->NewStringUTF("1.0.0");
}

}
