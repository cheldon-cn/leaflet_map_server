package cn.cycle.app.render;

import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import javafx.scene.canvas.GraphicsContext;
import javafx.geometry.Rectangle2D;

import java.util.ArrayList;
import java.util.List;
import java.util.function.Consumer;

public class DirtyRectRenderer extends AbstractLifecycleComponent {
    
    private Rectangle2D dirtyRect = null;
    private boolean fullRepaintNeeded = true;
    private final List<Consumer<GraphicsContext>> renderCallbacks = new ArrayList<>();
    
    public void addRenderCallback(Consumer<GraphicsContext> callback) {
        renderCallbacks.add(callback);
    }
    
    public void removeRenderCallback(Consumer<GraphicsContext> callback) {
        renderCallbacks.remove(callback);
    }
    
    public void markDirty(double x, double y, double width, double height) {
        if (dirtyRect == null) {
            dirtyRect = new Rectangle2D(x, y, width, height);
        } else {
            double minX = Math.min(dirtyRect.getMinX(), x);
            double minY = Math.min(dirtyRect.getMinY(), y);
            double maxX = Math.max(dirtyRect.getMaxX(), x + width);
            double maxY = Math.max(dirtyRect.getMaxY(), y + height);
            dirtyRect = new Rectangle2D(minX, minY, maxX - minX, maxY - minY);
        }
    }
    
    public void markDirty(Rectangle2D rect) {
        if (rect == null) {
            return;
        }
        markDirty(rect.getMinX(), rect.getMinY(), rect.getWidth(), rect.getHeight());
    }
    
    public void markFullRepaint() {
        fullRepaintNeeded = true;
        dirtyRect = null;
    }
    
    public void render(GraphicsContext gc, double canvasWidth, double canvasHeight) {
        if (fullRepaintNeeded) {
            renderFull(gc, canvasWidth, canvasHeight);
            fullRepaintNeeded = false;
        } else if (dirtyRect != null) {
            renderDirtyRect(gc, dirtyRect);
            dirtyRect = null;
        }
    }
    
    private void renderFull(GraphicsContext gc, double canvasWidth, double canvasHeight) {
        gc.save();
        gc.clearRect(0, 0, canvasWidth, canvasHeight);
        
        for (Consumer<GraphicsContext> callback : renderCallbacks) {
            callback.accept(gc);
        }
        
        gc.restore();
    }
    
    private void renderDirtyRect(GraphicsContext gc, Rectangle2D rect) {
        gc.save();
        gc.beginPath();
        gc.rect(rect.getMinX(), rect.getMinY(), rect.getWidth(), rect.getHeight());
        gc.clip();
        
        gc.clearRect(rect.getMinX(), rect.getMinY(), rect.getWidth(), rect.getHeight());
        
        for (Consumer<GraphicsContext> callback : renderCallbacks) {
            callback.accept(gc);
        }
        
        gc.restore();
    }
    
    public boolean needsRepaint() {
        return fullRepaintNeeded || dirtyRect != null;
    }
    
    public boolean isFullRepaintNeeded() {
        return fullRepaintNeeded;
    }
    
    public Rectangle2D getDirtyRect() {
        return dirtyRect;
    }
    
    public void clearDirtyRect() {
        dirtyRect = null;
    }
    
    @Override
    protected void doInitialize() {
    }
    
    @Override
    protected void doDispose() {
        renderCallbacks.clear();
        dirtyRect = null;
    }
}
