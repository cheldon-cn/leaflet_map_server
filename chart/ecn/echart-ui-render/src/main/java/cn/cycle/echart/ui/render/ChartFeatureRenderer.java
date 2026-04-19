package cn.cycle.echart.ui.render;

import cn.cycle.echart.render.Layer;
import cn.cycle.echart.render.RenderContext;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;

/**
 * 海图要素渲染器。
 * 
 * <p>负责渲染海图要素，如海岸线、水深、航标等。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ChartFeatureRenderer extends Layer {

    private Color waterColor;
    private Color landColor;
    private Color depthColor;

    public ChartFeatureRenderer() {
        super("chart-features");
        this.waterColor = Color.rgb(173, 216, 230);
        this.landColor = Color.rgb(210, 180, 140);
        this.depthColor = Color.rgb(0, 0, 139);
    }

    @Override
    public void render(RenderContext context) {
        if (!(context instanceof FxRenderContext)) {
            return;
        }
        
        FxRenderContext fxContext = (FxRenderContext) context;
        GraphicsContext gc = fxContext.getGraphicsContext();
        
        gc.save();
        
        drawWater(gc, context);
        drawLand(gc, context);
        drawDepthContours(gc, context);
        drawNavigationAids(gc, context);
        
        gc.restore();
    }

    private void drawWater(GraphicsContext gc, RenderContext context) {
        gc.setFill(waterColor);
        gc.fillRect(-context.getWidth() / 2, -context.getHeight() / 2,
                context.getWidth(), context.getHeight());
    }

    private void drawLand(GraphicsContext gc, RenderContext context) {
        gc.setFill(landColor);
        gc.beginPath();
        gc.moveTo(-100, -50);
        gc.lineTo(-80, -30);
        gc.lineTo(-60, -40);
        gc.lineTo(-40, -20);
        gc.lineTo(-20, -35);
        gc.lineTo(0, -25);
        gc.lineTo(20, -40);
        gc.lineTo(40, -30);
        gc.lineTo(60, -45);
        gc.lineTo(80, -35);
        gc.lineTo(100, -50);
        gc.lineTo(100, 50);
        gc.lineTo(-100, 50);
        gc.closePath();
        gc.fill();
    }

    private void drawDepthContours(GraphicsContext gc, RenderContext context) {
        gc.setStroke(depthColor);
        gc.setLineWidth(0.5);
        
        for (int depth = 10; depth <= 100; depth += 10) {
            double radius = depth * 2;
            gc.strokeOval(-radius, -radius, radius * 2, radius * 2);
        }
    }

    private void drawNavigationAids(GraphicsContext gc, RenderContext context) {
        gc.setFill(Color.RED);
        gc.fillOval(-50, -20, 4, 4);
        gc.fillOval(30, -10, 4, 4);
        gc.fillOval(-20, 20, 4, 4);
        
        gc.setFill(Color.GREEN);
        gc.fillOval(50, 10, 4, 4);
        gc.fillOval(-40, 30, 4, 4);
    }

    @Override
    public boolean needsRender(RenderContext context) {
        return true;
    }

    public void setWaterColor(Color color) {
        this.waterColor = color;
    }

    public void setLandColor(Color color) {
        this.landColor = color;
    }

    public void setDepthColor(Color color) {
        this.depthColor = color;
    }
}
