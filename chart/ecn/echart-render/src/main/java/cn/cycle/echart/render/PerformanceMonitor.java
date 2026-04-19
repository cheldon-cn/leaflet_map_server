package cn.cycle.echart.render;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicLong;

/**
 * 性能监控器。
 * 
 * <p>监控渲染性能指标。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class PerformanceMonitor {

    private final Map<String, LayerStats> layerStats;
    private final AtomicLong totalFrameTime;
    private final AtomicLong frameCount;
    private long lastFrameTime;
    private double averageFPS;

    public PerformanceMonitor() {
        this.layerStats = new HashMap<>();
        this.totalFrameTime = new AtomicLong(0);
        this.frameCount = new AtomicLong(0);
        this.lastFrameTime = 0;
        this.averageFPS = 0;
    }

    /**
     * 开始图层渲染计时。
     * 
     * @param layerName 图层名称
     */
    public void beginLayerRender(String layerName) {
        LayerStats stats = layerStats.computeIfAbsent(layerName, k -> new LayerStats());
        stats.beginRender();
    }

    /**
     * 结束图层渲染计时。
     * 
     * @param layerName 图层名称
     */
    public void endLayerRender(String layerName) {
        LayerStats stats = layerStats.get(layerName);
        if (stats != null) {
            stats.endRender();
        }
    }

    /**
     * 记录帧时间。
     * 
     * @param frameTime 帧时间（毫秒）
     */
    public void recordFrameTime(long frameTime) {
        lastFrameTime = frameTime;
        totalFrameTime.addAndGet(frameTime);
        long count = frameCount.incrementAndGet();
        
        if (count > 0) {
            averageFPS = 1000.0 / (totalFrameTime.get() / (double) count);
        }
    }

    /**
     * 获取图层渲染时间。
     * 
     * @param layerName 图层名称
     * @return 平均渲染时间（毫秒）
     */
    public double getLayerRenderTime(String layerName) {
        LayerStats stats = layerStats.get(layerName);
        return stats != null ? stats.getAverageRenderTime() : 0;
    }

    /**
     * 获取最后帧时间。
     * 
     * @return 最后帧时间（毫秒）
     */
    public long getLastFrameTime() {
        return lastFrameTime;
    }

    /**
     * 获取平均帧时间。
     * 
     * @return 平均帧时间（毫秒）
     */
    public double getAverageFrameTime() {
        long count = frameCount.get();
        return count > 0 ? totalFrameTime.get() / (double) count : 0;
    }

    /**
     * 获取平均FPS。
     * 
     * @return 平均FPS
     */
    public double getAverageFPS() {
        return averageFPS;
    }

    /**
     * 获取帧计数。
     * 
     * @return 帧计数
     */
    public long getFrameCount() {
        return frameCount.get();
    }

    /**
     * 获取所有图层统计。
     * 
     * @return 图层统计映射
     */
    public Map<String, LayerStats> getAllLayerStats() {
        return new HashMap<>(layerStats);
    }

    /**
     * 重置统计。
     */
    public void reset() {
        layerStats.clear();
        totalFrameTime.set(0);
        frameCount.set(0);
        lastFrameTime = 0;
        averageFPS = 0;
    }

    /**
     * 图层统计类。
     */
    public static class LayerStats {
        private long totalRenderTime;
        private long renderCount;
        private long lastRenderStart;

        public void beginRender() {
            lastRenderStart = System.currentTimeMillis();
        }

        public void endRender() {
            if (lastRenderStart > 0) {
                totalRenderTime += System.currentTimeMillis() - lastRenderStart;
                renderCount++;
                lastRenderStart = 0;
            }
        }

        public double getAverageRenderTime() {
            return renderCount > 0 ? totalRenderTime / (double) renderCount : 0;
        }

        public long getRenderCount() {
            return renderCount;
        }

        public long getTotalRenderTime() {
            return totalRenderTime;
        }
    }
}
