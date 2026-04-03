#include <jni.h>
#include <string>
#include <vector>
#include <android/log.h>

#define LOG_TAG "ChartJNI"

class ChartViewerNative {
public:
    ChartViewerNative() 
        : m_centerLon(0.0)
        , m_centerLat(0.0)
        , m_zoomLevel(1)
        , m_rotation(0.0f) {
    }
    
    ~ChartViewerNative() {
    }
    
    void SetRenderCallback(JNIEnv* env, jobject callback) {
        m_renderCallback = env->NewGlobalRef(callback);
        m_renderCallbackMethod = env->GetMethodID(
            env->GetObjectClass(callback), "onRenderComplete", "()V");
    }
    
    void SetLoadCallback(JNIEnv* env, jobject callback) {
        m_loadCallback = env->NewGlobalRef(callback);
        m_loadSuccessMethod = env->GetMethodID(
            env->GetObjectClass(callback), "onLoadSuccess", "(Ljava/lang/String;)V");
        m_loadErrorMethod = env->GetMethodID(
            env->GetObjectClass(callback), "onLoadError", "(Ljava/lang/String;)V");
    }
    
    void SetTouchCallback(JNIEnv* env, jobject callback) {
        m_touchCallback = env->NewGlobalRef(callback);
        m_singleTapMethod = env->GetMethodID(
            env->GetObjectClass(callback), "onSingleTap", "(FF)V");
        m_doubleTapMethod = env->GetMethodID(
            env->GetObjectClass(callback), "onDoubleTap", "(FF)V");
        m_longPressMethod = env->GetMethodID(
            env->GetObjectClass(callback), "onLongPress", "(FF)V");
        m_scaleMethod = env->GetMethodID(
            env->GetObjectClass(callback), "onScale", "(FFF)V");
        m_panMethod = env->GetMethodID(
            env->GetObjectClass(callback), "onPan", "(FF)V");
    }
    
    bool LoadChart(const std::string& path) {
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Loading chart: %s", path.c_str());
        return true;
    }
    
    void UnloadChart(const std::string& chartId) {
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Unloading chart: %s", chartId.c_str());
    }
    
    void SetViewBounds(double minLon, double minLat, double maxLon, double maxLat) {
        m_centerLon = (minLon + maxLon) / 2.0;
        m_centerLat = (minLat + maxLat) / 2.0;
    }
    
    void SetCenter(double lon, double lat) {
        m_centerLon = lon;
        m_centerLat = lat;
    }
    
    double GetCenterLon() const { return m_centerLon; }
    double GetCenterLat() const { return m_centerLat; }
    
    void SetZoomLevel(int level) { m_zoomLevel = level; }
    int GetZoomLevel() const { return m_zoomLevel; }
    
    void SetRotation(float angle) { m_rotation = angle; }
    float GetRotation() const { return m_rotation; }
    
    void Render() {
    }
    
    void SetSurface(JNIEnv* env, jobject surface) {
        m_surface = env->NewGlobalRef(surface);
    }
    
    void SetViewport(int width, int height) {
        m_viewportWidth = width;
        m_viewportHeight = height;
    }
    
    void ScreenToGeo(float screenX, float screenY, double& lon, double& lat) {
        lon = m_centerLon + (screenX - m_viewportWidth / 2) * 0.0001;
        lat = m_centerLat - (screenY - m_viewportHeight / 2) * 0.0001;
    }
    
    void GeoToScreen(double lon, double lat, float& screenX, float& screenY) {
        screenX = static_cast<float>((lon - m_centerLon) / 0.0001 + m_viewportWidth / 2);
        screenY = static_cast<float>((m_centerLat - lat) / 0.0001 + m_viewportHeight / 2);
    }
    
    jobjectArray QueryFeatures(JNIEnv* env, float screenX, float screenY) {
        jclass featureClass = env->FindClass("ogc/chart/FeatureInfo");
        return env->NewObjectArray(0, featureClass, nullptr);
    }
    
    void SetDisplayStyle(const std::string& styleName) {
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Setting style: %s", styleName.c_str());
    }
    
    void SetLayerVisible(const std::string& layerName, bool visible) {
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, 
            "Setting layer %s visible: %d", layerName.c_str(), visible);
    }

private:
    double m_centerLon;
    double m_centerLat;
    int m_zoomLevel;
    float m_rotation;
    int m_viewportWidth;
    int m_viewportHeight;
    
    jobject m_renderCallback;
    jmethodID m_renderCallbackMethod;
    
    jobject m_loadCallback;
    jmethodID m_loadSuccessMethod;
    jmethodID m_loadErrorMethod;
    
    jobject m_touchCallback;
    jmethodID m_singleTapMethod;
    jmethodID m_doubleTapMethod;
    jmethodID m_longPressMethod;
    jmethodID m_scaleMethod;
    jmethodID m_panMethod;
    
    jobject m_surface;
};
