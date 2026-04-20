package cn.cycle.echart.render;

import java.awt.geom.Rectangle2D;
import java.util.Map;

/**
 * 渲染上下文接口。
 * 
 * <p>定义平台无关的渲染上下文，可被不同平台（JavaFX、Swing等）实现。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface RenderContext {

    /**
     * 获取渲染区域宽度。
     * 
     * @return 宽度（像素）
     */
    int getWidth();

    /**
     * 获取渲染区域高度。
     * 
     * @return 高度（像素）
     */
    int getHeight();

    /**
     * 获取当前缩放级别。
     * 
     * @return 缩放级别
     */
    double getZoom();

    /**
     * 设置缩放级别。
     * 
     * @param zoom 缩放级别
     */
    void setZoom(double zoom);

    /**
     * 获取当前视图范围。
     * 
     * @return 视图范围（地理坐标）
     */
    Rectangle2D getViewport();

    /**
     * 设置视图范围。
     * 
     * @param viewport 视图范围（地理坐标）
     */
    void setViewport(Rectangle2D viewport);

    /**
     * 检查是否需要渲染。
     * 
     * @return 是否需要渲染
     */
    boolean needsRender();

    /**
     * 标记需要渲染。
     */
    void markDirty();

    /**
     * 清除渲染标记。
     */
    void clearDirty();

    /**
     * 获取设备像素比。
     * 
     * @return 设备像素比
     */
    double getDevicePixelRatio();

    /**
     * 设置符号样式。
     * 
     * @param symbolCode 符号代码
     * @param style 样式属性
     */
    void setSymbolStyle(String symbolCode, Map<String, Object> style);

    /**
     * 获取符号样式。
     * 
     * @param symbolCode 符号代码
     * @return 样式属性
     */
    Map<String, Object> getSymbolStyle(String symbolCode);

    /**
     * 销毁上下文。
     */
    void dispose();
}
