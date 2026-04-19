package cn.cycle.echart.ui.render;

import cn.cycle.echart.render.RenderContext;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;

import java.awt.geom.Rectangle2D;
import java.util.Objects;

/**
 * JavaFX Canvas渲染上下文实现。
 * 
 * <p>基于JavaFX Canvas实现的渲染上下文。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class FxRenderContext implements RenderContext {

    private final Canvas canvas;
    private double zoom;
    private Rectangle2D viewport;
    private boolean dirty;
    private double devicePixelRatio;

    public FxRenderContext(Canvas canvas) {
        this.canvas = Objects.requireNonNull(canvas, "canvas cannot be null");
        this.zoom = 1.0;
        this.viewport = new Rectangle2D.Double(-180, -90, 360, 180);
        this.dirty = true;
        this.devicePixelRatio = 1.0;
    }

    @Override
    public int getWidth() {
        return (int) canvas.getWidth();
    }

    @Override
    public int getHeight() {
        return (int) canvas.getHeight();
    }

    @Override
    public double getZoom() {
        return zoom;
    }

    @Override
    public void setZoom(double zoom) {
        if (this.zoom != zoom) {
            this.zoom = Math.max(0.1, Math.min(100.0, zoom));
            markDirty();
        }
    }

    @Override
    public Rectangle2D getViewport() {
        return new Rectangle2D.Double(
                viewport.getX(), viewport.getY(),
                viewport.getWidth(), viewport.getHeight());
    }

    @Override
    public void setViewport(Rectangle2D viewport) {
        if (!this.viewport.equals(viewport)) {
            this.viewport = new Rectangle2D.Double(
                    viewport.getX(), viewport.getY(),
                    viewport.getWidth(), viewport.getHeight());
            markDirty();
        }
    }

    @Override
    public boolean needsRender() {
        return dirty;
    }

    @Override
    public void markDirty() {
        this.dirty = true;
    }

    @Override
    public void clearDirty() {
        this.dirty = false;
    }

    @Override
    public double getDevicePixelRatio() {
        return devicePixelRatio;
    }

    public void setDevicePixelRatio(double ratio) {
        this.devicePixelRatio = ratio;
    }

    @Override
    public void dispose() {
        clearDirty();
    }

    public GraphicsContext getGraphicsContext() {
        return canvas.getGraphicsContext2D();
    }

    public Canvas getCanvas() {
        return canvas;
    }

    public void resize(double width, double height) {
        canvas.setWidth(width);
        canvas.setHeight(height);
        markDirty();
    }

    public void clear() {
        GraphicsContext gc = getGraphicsContext();
        gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());
    }
}
