package ogc.chart;

/**
 * 海图配置类
 * 
 * <p>用于配置海图显示系统的各种参数。</p>
 * 
 * <h3>使用示例：</h3>
 * <pre>{@code
 * ChartConfig config = new ChartConfig.Builder()
 *     .setCachePath("/sdcard/chart/cache")
 *     .setMaxCacheSize(100 * 1024 * 1024)
 *     .setThreadCount(4)
 *     .setEnableAntiAliasing(true)
 *     .build();
 * 
 * ChartViewer viewer = new ChartViewer(context);
 * viewer.setConfig(config);
 * }</pre>
 * 
 * @version 1.0.0
 * @since 1.0.0
 */
public class ChartConfig {
    
    private final String cachePath;
    private final long maxCacheSize;
    private final int threadCount;
    private final boolean enableAntiAliasing;
    private final boolean enableHighQuality;
    private final String defaultStyle;
    private final float pixelRatio;
    
    private ChartConfig(Builder builder) {
        this.cachePath = builder.cachePath;
        this.maxCacheSize = builder.maxCacheSize;
        this.threadCount = builder.threadCount;
        this.enableAntiAliasing = builder.enableAntiAliasing;
        this.enableHighQuality = builder.enableHighQuality;
        this.defaultStyle = builder.defaultStyle;
        this.pixelRatio = builder.pixelRatio;
    }
    
    /**
     * 获取缓存路径
     * 
     * @return 缓存路径
     */
    public String getCachePath() {
        return cachePath;
    }
    
    /**
     * 获取最大缓存大小（字节）
     * 
     * @return 最大缓存大小
     */
    public long getMaxCacheSize() {
        return maxCacheSize;
    }
    
    /**
     * 获取渲染线程数
     * 
     * @return 线程数
     */
    public int getThreadCount() {
        return threadCount;
    }
    
    /**
     * 是否启用抗锯齿
     * 
     * @return 是否启用
     */
    public boolean isEnableAntiAliasing() {
        return enableAntiAliasing;
    }
    
    /**
     * 是否启用高质量渲染
     * 
     * @return 是否启用
     */
    public boolean isEnableHighQuality() {
        return enableHighQuality;
    }
    
    /**
     * 获取默认样式名称
     * 
     * @return 样式名称
     */
    public String getDefaultStyle() {
        return defaultStyle;
    }
    
    /**
     * 获取像素比例
     * 
     * @return 像素比例
     */
    public float getPixelRatio() {
        return pixelRatio;
    }
    
    /**
     * 配置构建器
     */
    public static class Builder {
        private String cachePath = "";
        private long maxCacheSize = 50 * 1024 * 1024; // 50MB
        private int threadCount = 2;
        private boolean enableAntiAliasing = true;
        private boolean enableHighQuality = false;
        private String defaultStyle = "default";
        private float pixelRatio = 1.0f;
        
        /**
         * 设置缓存路径
         * 
         * @param cachePath 缓存路径
         * @return Builder对象
         */
        public Builder setCachePath(String cachePath) {
            this.cachePath = cachePath;
            return this;
        }
        
        /**
         * 设置最大缓存大小
         * 
         * @param maxCacheSize 最大缓存大小（字节）
         * @return Builder对象
         */
        public Builder setMaxCacheSize(long maxCacheSize) {
            this.maxCacheSize = maxCacheSize;
            return this;
        }
        
        /**
         * 设置渲染线程数
         * 
         * @param threadCount 线程数
         * @return Builder对象
         */
        public Builder setThreadCount(int threadCount) {
            this.threadCount = threadCount;
            return this;
        }
        
        /**
         * 设置是否启用抗锯齿
         * 
         * @param enable 是否启用
         * @return Builder对象
         */
        public Builder setEnableAntiAliasing(boolean enable) {
            this.enableAntiAliasing = enable;
            return this;
        }
        
        /**
         * 设置是否启用高质量渲染
         * 
         * @param enable 是否启用
         * @return Builder对象
         */
        public Builder setEnableHighQuality(boolean enable) {
            this.enableHighQuality = enable;
            return this;
        }
        
        /**
         * 设置默认样式
         * 
         * @param styleName 样式名称
         * @return Builder对象
         */
        public Builder setDefaultStyle(String styleName) {
            this.defaultStyle = styleName;
            return this;
        }
        
        /**
         * 设置像素比例
         * 
         * @param pixelRatio 像素比例
         * @return Builder对象
         */
        public Builder setPixelRatio(float pixelRatio) {
            this.pixelRatio = pixelRatio;
            return this;
        }
        
        /**
         * 构建配置对象
         * 
         * @return ChartConfig对象
         */
        public ChartConfig build() {
            return new ChartConfig(this);
        }
    }
}
