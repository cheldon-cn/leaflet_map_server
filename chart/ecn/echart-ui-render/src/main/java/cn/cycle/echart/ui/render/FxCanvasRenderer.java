package cn.cycle.echart.ui.render;

import cn.cycle.echart.render.RenderEngine;
import cn.cycle.echart.render.Layer;
import cn.cycle.echart.render.LayerManager;
import javafx.animation.AnimationTimer;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;

import java.util.Objects;

/**
 * JavaFX Canvas渲染器。
 * 
 * <p>基于JavaFX Canvas实现的渲染器，使用AnimationTimer实现动画循环。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class FxCanvasRenderer {

    private final Canvas canvas;
    private final FxRenderContext context;
    private final RenderEngine engine;
    private final AnimationTimer animationTimer;
    
    private boolean running;
    private Color backgroundColor;

    public FxCanvasRenderer(Canvas canvas) {
        this.canvas = Objects.requireNonNull(canvas, "canvas cannot be null");
        this.context = new FxRenderContext(canvas);
        this.engine = new RenderEngine();
        this.engine.setContext(context);
        this.animationTimer = createAnimationTimer();
        this.running = false;
        this.backgroundColor = Color.WHITE;
        
        setupCanvasListeners();
    }

    private AnimationTimer createAnimationTimer() {
        return new AnimationTimer() {
            @Override
            public void handle(long now) {
                if (context.needsRender()) {
                    renderFrame();
                }
            }
        };
    }

    private void setupCanvasListeners() {
        canvas.widthProperty().addListener((obs, oldVal, newVal) -> {
            context.resize(newVal.doubleValue(), canvas.getHeight());
            requestRepaint();
        });
        
        canvas.heightProperty().addListener((obs, oldVal, newVal) -> {
            context.resize(canvas.getWidth(), newVal.doubleValue());
            requestRepaint();
        });
    }

    public void start() {
        if (!running) {
            running = true;
            animationTimer.start();
        }
    }

    public void stop() {
        if (running) {
            running = false;
            animationTimer.stop();
        }
    }

    public void requestRepaint() {
        context.markDirty();
    }

    private void renderFrame() {
        GraphicsContext gc = context.getGraphicsContext();
        
        gc.save();
        
        gc.setFill(backgroundColor);
        gc.fillRect(0, 0, canvas.getWidth(), canvas.getHeight());
        
        gc.translate(canvas.getWidth() / 2, canvas.getHeight() / 2);
        gc.scale(context.getZoom(), context.getZoom());
        
        engine.render();
        
        gc.restore();
    }

    public void setZoom(double zoom) {
        context.setZoom(zoom);
        requestRepaint();
    }

    public double getZoom() {
        return context.getZoom();
    }

    public void zoomIn() {
        setZoom(getZoom() * 1.2);
    }

    public void zoomOut() {
        setZoom(getZoom() / 1.2);
    }

    public void setBackgroundColor(Color color) {
        this.backgroundColor = Objects.requireNonNull(color, "color cannot be null");
        requestRepaint();
    }

    public Color getBackgroundColor() {
        return backgroundColor;
    }

    public LayerManager getLayerManager() {
        return engine.getLayerManager();
    }

    public RenderEngine getEngine() {
        return engine;
    }

    public FxRenderContext getContext() {
        return context;
    }

    public Canvas getCanvas() {
        return canvas;
    }

    public boolean isRunning() {
        return running;
    }

    public void dispose() {
        stop();
        engine.dispose();
    }
}
