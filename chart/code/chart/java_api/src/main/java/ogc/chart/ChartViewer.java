package ogc.chart;

/**
 * 海图显示系统主入口类
 * 
 * <p>ChartViewer是海图显示系统的核心类，提供海图加载、显示、交互等功能。</p>
 * 
 * <h3>使用示例：</h3>
 * <pre>{@code
 * // 创建ChartViewer实例
 * ChartViewer viewer = new ChartViewer(context);
 * 
 * // 设置显示区域
 * viewer.setViewBounds(116.0, 39.0, 117.0, 40.0);
 * 
 * // 加载海图
 * viewer.loadChart("/sdcard/charts/chart.enc");
 * 
 * // 设置渲染回调
 * viewer.setRenderCallback(new ChartViewer.RenderCallback() {
 *     @Override
 *     public void onRenderComplete() {
 *         // 渲染完成
 *     }
 * });
 * }</pre>
 * 
 * @version 1.0.0
 * @since 1.0.0
 */
public class ChartViewer {
    
    static {
        System.loadLibrary("ogc_chart_jni");
    }
    
    /**
     * 渲染完成回调接口
     */
    public interface RenderCallback {
        /**
         * 渲染完成时调用
         */
        void onRenderComplete();
    }
    
    /**
     * 海图加载回调接口
     */
    public interface LoadCallback {
        /**
         * 加载成功时调用
         * @param chartId 海图ID
         */
        void onLoadSuccess(String chartId);
        
        /**
         * 加载失败时调用
         * @param error 错误信息
         */
        void onLoadError(String error);
    }
    
    /**
     * 触摸事件回调接口
     */
    public interface TouchCallback {
        /**
         * 单击事件
         * @param x X坐标
         * @param y Y坐标
         */
        void onSingleTap(float x, float y);
        
        /**
         * 双击事件
         * @param x X坐标
         * @param y Y坐标
         */
        void onDoubleTap(float x, float y);
        
        /**
         * 长按事件
         * @param x X坐标
         * @param y Y坐标
         */
        void onLongPress(float x, float y);
        
        /**
         * 缩放事件
         * @param focusX 焦点X坐标
         * @param focusY 焦点Y坐标
         * @param scale 缩放比例
         */
        void onScale(float focusX, float focusY, float scale);
        
        /**
         * 平移事件
         * @param dx X方向偏移
         * @param dy Y方向偏移
         */
        void onPan(float dx, float dy);
    }
    
    private long nativePointer;
    private RenderCallback renderCallback;
    private LoadCallback loadCallback;
    private TouchCallback touchCallback;
    
    /**
     * 构造函数
     * 
     * @param context Android上下文
     */
    public ChartViewer(Object context) {
        nativePointer = nativeCreate(context);
    }
    
    /**
     * 析构函数
     */
    @Override
    protected void finalize() throws Throwable {
        try {
            if (nativePointer != 0) {
                nativeDestroy(nativePointer);
                nativePointer = 0;
            }
        } finally {
            super.finalize();
        }
    }
    
    /**
     * 释放资源
     */
    public void release() {
        if (nativePointer != 0) {
            nativeDestroy(nativePointer);
            nativePointer = 0;
        }
    }
    
    /**
     * 设置渲染回调
     * 
     * @param callback 渲染回调接口
     */
    public void setRenderCallback(RenderCallback callback) {
        this.renderCallback = callback;
        nativeSetRenderCallback(nativePointer, callback);
    }
    
    /**
     * 设置加载回调
     * 
     * @param callback 加载回调接口
     */
    public void setLoadCallback(LoadCallback callback) {
        this.loadCallback = callback;
        nativeSetLoadCallback(nativePointer, callback);
    }
    
    /**
     * 设置触摸回调
     * 
     * @param callback 触摸回调接口
     */
    public void setTouchCallback(TouchCallback callback) {
        this.touchCallback = callback;
        nativeSetTouchCallback(nativePointer, callback);
    }
    
    /**
     * 加载海图文件
     * 
     * @param filePath 海图文件路径
     * @return 是否成功启动加载
     */
    public boolean loadChart(String filePath) {
        return nativeLoadChart(nativePointer, filePath);
    }
    
    /**
     * 卸载海图
     * 
     * @param chartId 海图ID
     */
    public void unloadChart(String chartId) {
        nativeUnloadChart(nativePointer, chartId);
    }
    
    /**
     * 设置显示范围
     * 
     * @param minLon 最小经度
     * @param minLat 最小纬度
     * @param maxLon 最大经度
     * @param maxLat 最大纬度
     */
    public void setViewBounds(double minLon, double minLat, double maxLon, double maxLat) {
        nativeSetViewBounds(nativePointer, minLon, minLat, maxLon, maxLat);
    }
    
    /**
     * 设置中心点
     * 
     * @param lon 经度
     * @param lat 纬度
     */
    public void setCenter(double lon, double lat) {
        nativeSetCenter(nativePointer, lon, lat);
    }
    
    /**
     * 获取中心点经度
     * 
     * @return 经度
     */
    public double getCenterLon() {
        return nativeGetCenterLon(nativePointer);
    }
    
    /**
     * 获取中心点纬度
     * 
     * @return 纬度
     */
    public double getCenterLat() {
        return nativeGetCenterLat(nativePointer);
    }
    
    /**
     * 设置缩放级别
     * 
     * @param level 缩放级别 (1-20)
     */
    public void setZoomLevel(int level) {
        nativeSetZoomLevel(nativePointer, level);
    }
    
    /**
     * 获取缩放级别
     * 
     * @return 缩放级别
     */
    public int getZoomLevel() {
        return nativeGetZoomLevel(nativePointer);
    }
    
    /**
     * 设置旋转角度
     * 
     * @param angle 角度（度）
     */
    public void setRotation(float angle) {
        nativeSetRotation(nativePointer, angle);
    }
    
    /**
     * 获取旋转角度
     * 
     * @return 角度（度）
     */
    public float getRotation() {
        return nativeGetRotation(nativePointer);
    }
    
    /**
     * 渲染一帧
     */
    public void render() {
        nativeRender(nativePointer);
    }
    
    /**
     * 设置渲染表面
     * 
     * @param surface Android Surface对象
     */
    public void setSurface(Object surface) {
        nativeSetSurface(nativePointer, surface);
    }
    
    /**
     * 设置视口大小
     * 
     * @param width 宽度
     * @param height 高度
     */
    public void setViewport(int width, int height) {
        nativeSetViewport(nativePointer, width, height);
    }
    
    /**
     * 屏幕坐标转地理坐标
     * 
     * @param screenX 屏幕X坐标
     * @param screenY 屏幕Y坐标
     * @return 地理坐标 [经度, 纬度]
     */
    public double[] screenToGeo(float screenX, float screenY) {
        return nativeScreenToGeo(nativePointer, screenX, screenY);
    }
    
    /**
     * 地理坐标转屏幕坐标
     * 
     * @param lon 经度
     * @param lat 纬度
     * @return 屏幕坐标 [X, Y]
     */
    public float[] geoToScreen(double lon, double lat) {
        return nativeGeoToScreen(nativePointer, lon, lat);
    }
    
    /**
     * 查询指定位置的特征
     * 
     * @param screenX 屏幕X坐标
     * @param screenY 屏幕Y坐标
     * @return 特征信息列表
     */
    public FeatureInfo[] queryFeatures(float screenX, float screenY) {
        return nativeQueryFeatures(nativePointer, screenX, screenY);
    }
    
    /**
     * 设置显示样式
     * 
     * @param styleName 样式名称
     */
    public void setDisplayStyle(String styleName) {
        nativeSetDisplayStyle(nativePointer, styleName);
    }
    
    /**
     * 设置图层可见性
     * 
     * @param layerName 图层名称
     * @param visible 是否可见
     */
    public void setLayerVisible(String layerName, boolean visible) {
        nativeSetLayerVisible(nativePointer, layerName, visible);
    }
    
    /**
     * 获取版本信息
     * 
     * @return 版本字符串
     */
    public static String getVersion() {
        return nativeGetVersion();
    }
    
    // Native methods
    private native long nativeCreate(Object context);
    private native void nativeDestroy(long ptr);
    private native void nativeSetRenderCallback(long ptr, RenderCallback callback);
    private native void nativeSetLoadCallback(long ptr, LoadCallback callback);
    private native void nativeSetTouchCallback(long ptr, TouchCallback callback);
    private native boolean nativeLoadChart(long ptr, String filePath);
    private native void nativeUnloadChart(long ptr, String chartId);
    private native void nativeSetViewBounds(long ptr, double minLon, double minLat, double maxLon, double maxLat);
    private native void nativeSetCenter(long ptr, double lon, double lat);
    private native double nativeGetCenterLon(long ptr);
    private native double nativeGetCenterLat(long ptr);
    private native void nativeSetZoomLevel(long ptr, int level);
    private native int nativeGetZoomLevel(long ptr);
    private native void nativeSetRotation(long ptr, float angle);
    private native float nativeGetRotation(long ptr);
    private native void nativeRender(long ptr);
    private native void nativeSetSurface(long ptr, Object surface);
    private native void nativeSetViewport(long ptr, int width, int height);
    private native double[] nativeScreenToGeo(long ptr, float screenX, float screenY);
    private native float[] nativeGeoToScreen(long ptr, double lon, double lat);
    private native FeatureInfo[] nativeQueryFeatures(long ptr, float screenX, float screenY);
    private native void nativeSetDisplayStyle(long ptr, String styleName);
    private native void nativeSetLayerVisible(long ptr, String layerName, boolean visible);
    private static native String nativeGetVersion();
}
