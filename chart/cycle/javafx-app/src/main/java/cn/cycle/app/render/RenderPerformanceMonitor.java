package cn.cycle.app.render;

import cn.cycle.app.lifecycle.AbstractLifecycleComponent;

import java.util.LinkedList;
import java.util.Queue;

public class RenderPerformanceMonitor extends AbstractLifecycleComponent {
    
    private static final int MAX_SAMPLES = 60;
    
    private final Queue<Long> renderTimes = new LinkedList<>();
    private final Queue<Long> frameTimes = new LinkedList<>();
    
    private long renderStartTime = 0;
    private long lastFrameTime = 0;
    private int frameCount = 0;
    private double averageFPS = 0;
    private double averageRenderTime = 0;
    
    public RenderPerformanceMonitor() {
    }
    
    public void recordRenderStart() {
        renderStartTime = System.nanoTime();
    }
    
    public void recordRenderEnd() {
        long renderTime = System.nanoTime() - renderStartTime;
        renderTimes.offer(renderTime);
        if (renderTimes.size() > MAX_SAMPLES) {
            renderTimes.poll();
        }
        
        long currentTime = System.nanoTime();
        if (lastFrameTime > 0) {
            long frameTime = currentTime - lastFrameTime;
            frameTimes.offer(frameTime);
            if (frameTimes.size() > MAX_SAMPLES) {
                frameTimes.poll();
            }
        }
        lastFrameTime = currentTime;
        
        frameCount++;
        updateAverages();
    }
    
    private void updateAverages() {
        if (renderTimes.isEmpty()) {
            return;
        }
        
        long totalRenderTime = 0;
        for (Long time : renderTimes) {
            totalRenderTime += time;
        }
        averageRenderTime = totalRenderTime / (double) renderTimes.size() / 1_000_000.0;
        
        if (!frameTimes.isEmpty()) {
            long totalFrameTime = 0;
            for (Long time : frameTimes) {
                totalFrameTime += time;
            }
            double avgFrameTimeMs = totalFrameTime / (double) frameTimes.size() / 1_000_000.0;
            averageFPS = avgFrameTimeMs > 0 ? 1000.0 / avgFrameTimeMs : 0;
        }
    }
    
    public double getAverageFPS() {
        return averageFPS;
    }
    
    public double getAverageRenderTime() {
        return averageRenderTime;
    }
    
    public int getFrameCount() {
        return frameCount;
    }
    
    public long getLastRenderTime() {
        return lastFrameTime;
    }
    
    public void reset() {
        renderTimes.clear();
        frameTimes.clear();
        frameCount = 0;
        averageFPS = 0;
        averageRenderTime = 0;
        lastFrameTime = 0;
    }
    
    public boolean isPerformanceGood() {
        return averageFPS >= 30;
    }
    
    public boolean isPerformanceAcceptable() {
        return averageFPS >= 15;
    }
    
    public String getPerformanceStatus() {
        if (averageFPS >= 30) {
            return "优秀";
        } else if (averageFPS >= 15) {
            return "良好";
        } else {
            return "需要优化";
        }
    }
    
    public int getSampleCount() {
        return renderTimes.size();
    }
    
    @Override
    protected void doInitialize() {
        reset();
    }
    
    @Override
    protected void doDispose() {
        renderTimes.clear();
        frameTimes.clear();
    }
}
