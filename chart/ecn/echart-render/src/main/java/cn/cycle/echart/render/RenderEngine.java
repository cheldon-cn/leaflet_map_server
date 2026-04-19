package cn.cycle.echart.render;

import java.util.List;
import java.util.Objects;

/**
 * 渲染引擎。
 * 
 * <p>负责协调图层渲染和性能优化。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class RenderEngine {

    private final LayerManager layerManager;
    private final DirtyRegionManager dirtyRegionManager;
    private final PerformanceMonitor performanceMonitor;
    private final LODStrategy lodStrategy;
    
    private RenderContext context;
    private boolean rendering;

    public RenderEngine() {
        this.layerManager = new LayerManager();
        this.dirtyRegionManager = new DirtyRegionManager();
        this.performanceMonitor = new PerformanceMonitor();
        this.lodStrategy = new LODStrategy();
        this.rendering = false;
    }

    /**
     * 获取图层管理器。
     * 
     * @return 图层管理器
     */
    public LayerManager getLayerManager() {
        return layerManager;
    }

    /**
     * 获取脏区域管理器。
     * 
     * @return 脏区域管理器
     */
    public DirtyRegionManager getDirtyRegionManager() {
        return dirtyRegionManager;
    }

    /**
     * 获取性能监控器。
     * 
     * @return 性能监控器
     */
    public PerformanceMonitor getPerformanceMonitor() {
        return performanceMonitor;
    }

    /**
     * 获取LOD策略。
     * 
     * @return LOD策略
     */
    public LODStrategy getLODStrategy() {
        return lodStrategy;
    }

    /**
     * 设置渲染上下文。
     * 
     * @param context 渲染上下文
     */
    public void setContext(RenderContext context) {
        this.context = context;
    }

    /**
     * 获取渲染上下文。
     * 
     * @return 渲染上下文
     */
    public RenderContext getContext() {
        return context;
    }

    /**
     * 执行渲染。
     */
    public void render() {
        if (context == null || rendering) {
            return;
        }
        
        rendering = true;
        long startTime = System.currentTimeMillis();
        
        try {
            List<Layer> visibleLayers = layerManager.getVisibleLayers();
            
            for (Layer layer : visibleLayers) {
                if (layer.needsRender(context)) {
                    performanceMonitor.beginLayerRender(layer.getName());
                    
                    int lod = lodStrategy.calculateLOD(context.getZoom(), layer);
                    layer.render(context);
                    
                    performanceMonitor.endLayerRender(layer.getName());
                }
            }
            
            dirtyRegionManager.clear();
            context.clearDirty();
            
        } finally {
            rendering = false;
            long elapsed = System.currentTimeMillis() - startTime;
            performanceMonitor.recordFrameTime(elapsed);
        }
    }

    /**
     * 请求重绘。
     */
    public void requestRepaint() {
        if (context != null) {
            context.markDirty();
        }
    }

    /**
     * 标记区域为脏。
     * 
     * @param x X坐标
     * @param y Y坐标
     * @param width 宽度
     * @param height 高度
     */
    public void markDirty(int x, int y, int width, int height) {
        dirtyRegionManager.addRegion(x, y, width, height);
        requestRepaint();
    }

    /**
     * 检查是否正在渲染。
     * 
     * @return 是否正在渲染
     */
    public boolean isRendering() {
        return rendering;
    }

    /**
     * 销毁渲染引擎。
     */
    public void dispose() {
        if (context != null) {
            context.dispose();
            context = null;
        }
        layerManager.clear();
        dirtyRegionManager.clear();
    }
}
