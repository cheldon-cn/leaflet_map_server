package cn.cycle.echart.render;

import java.awt.geom.Rectangle2D;
import java.util.Objects;

/**
 * 图层基类。
 * 
 * <p>定义图层的通用属性和行为。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public abstract class Layer {

    private final String name;
    private boolean visible;
    private double opacity;
    private int priority;
    private Rectangle2D bounds;

    protected Layer(String name) {
        this.name = Objects.requireNonNull(name, "name cannot be null");
        this.visible = true;
        this.opacity = 1.0;
        this.priority = 0;
        this.bounds = null;
    }

    /**
     * 获取图层名称。
     * 
     * @return 图层名称
     */
    public String getName() {
        return name;
    }

    /**
     * 检查图层是否可见。
     * 
     * @return 是否可见
     */
    public boolean isVisible() {
        return visible;
    }

    /**
     * 设置图层可见性。
     * 
     * @param visible 是否可见
     */
    public void setVisible(boolean visible) {
        this.visible = visible;
    }

    /**
     * 获取图层不透明度。
     * 
     * @return 不透明度（0.0-1.0）
     */
    public double getOpacity() {
        return opacity;
    }

    /**
     * 设置图层不透明度。
     * 
     * @param opacity 不透明度（0.0-1.0）
     */
    public void setOpacity(double opacity) {
        this.opacity = Math.max(0.0, Math.min(1.0, opacity));
    }

    /**
     * 获取图层优先级。
     * 
     * @return 优先级（数值越大越先渲染）
     */
    public int getPriority() {
        return priority;
    }

    /**
     * 设置图层优先级。
     * 
     * @param priority 优先级
     */
    public void setPriority(int priority) {
        this.priority = priority;
    }

    /**
     * 获取图层边界。
     * 
     * @return 图层边界（地理坐标），可能为null
     */
    public Rectangle2D getBounds() {
        return bounds;
    }

    /**
     * 设置图层边界。
     * 
     * @param bounds 图层边界
     */
    public void setBounds(Rectangle2D bounds) {
        this.bounds = bounds;
    }

    /**
     * 渲染图层。
     * 
     * @param context 渲染上下文
     */
    public abstract void render(RenderContext context);

    /**
     * 检查图层是否需要渲染。
     * 
     * @param context 渲染上下文
     * @return 是否需要渲染
     */
    public boolean needsRender(RenderContext context) {
        if (!visible || opacity <= 0) {
            return false;
        }
        
        if (bounds != null && context.getViewport() != null) {
            return bounds.intersects(context.getViewport());
        }
        
        return true;
    }

    @Override
    public String toString() {
        return "Layer{" +
               "name='" + name + '\'' +
               ", visible=" + visible +
               ", opacity=" + opacity +
               ", priority=" + priority +
               '}';
    }
}
