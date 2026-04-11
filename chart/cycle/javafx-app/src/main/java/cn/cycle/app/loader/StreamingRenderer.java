package cn.cycle.app.loader;

import cn.cycle.app.event.AppEvent;
import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import cn.cycle.app.render.DirtyRectRenderer;
import cn.cycle.app.render.TileCache;
import javafx.application.Platform;
import javafx.scene.canvas.GraphicsContext;

import java.nio.ByteBuffer;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

public class StreamingRenderer extends AbstractLifecycleComponent {
    
    private static final int DEFAULT_RENDER_INTERVAL_MS = 100;
    private static final int DEFAULT_TILE_SIZE = 256;
    
    private final ExecutorService executor;
    private final DirtyRectRenderer dirtyRenderer;
    private final TileCache tileCache;
    private final AtomicBoolean rendering = new AtomicBoolean(false);
    private final AtomicInteger renderedTiles = new AtomicInteger(0);
    
    private GraphicsContext graphicsContext;
    private int tileWidth = DEFAULT_TILE_SIZE;
    private int tileHeight = DEFAULT_TILE_SIZE;
    
    public StreamingRenderer() {
        this.executor = Executors.newSingleThreadExecutor();
        this.dirtyRenderer = new DirtyRectRenderer();
        this.tileCache = new TileCache();
    }
    
    public void setGraphicsContext(GraphicsContext gc) {
        this.graphicsContext = gc;
    }
    
    public void setTileSize(int width, int height) {
        this.tileWidth = width;
        this.tileHeight = height;
    }
    
    public CompletableFuture<Void> renderAsync(ByteBuffer data, int totalWidth, int totalHeight) {
        if (data == null || graphicsContext == null) {
            CompletableFuture<Void> failed = new CompletableFuture<>();
            failed.completeExceptionally(new IllegalStateException("Invalid state"));
            return failed;
        }
        
        rendering.set(true);
        renderedTiles.set(0);
        
        return CompletableFuture.runAsync(() -> {
            renderStreaming(data, totalWidth, totalHeight);
        }, executor);
    }
    
    private void renderStreaming(ByteBuffer data, int totalWidth, int totalHeight) {
        int tilesX = (int) Math.ceil((double) totalWidth / tileWidth);
        int tilesY = (int) Math.ceil((double) totalHeight / tileHeight);
        int totalTiles = tilesX * tilesY;
        
        AppEventBus.getInstance().publish(
            new AppEvent(AppEventType.RENDERING_STARTED, this)
                .withData("totalTiles", totalTiles)
        );
        
        for (int ty = 0; ty < tilesY && rendering.get(); ty++) {
            for (int tx = 0; tx < tilesX && rendering.get(); tx++) {
                int x = tx * tileWidth;
                int y = ty * tileHeight;
                int w = Math.min(tileWidth, totalWidth - x);
                int h = Math.min(tileHeight, totalHeight - y);
                
                renderTile(data, x, y, w, h, totalWidth);
                
                int rendered = renderedTiles.incrementAndGet();
                double progress = (double) rendered / totalTiles;
                
                Platform.runLater(() -> {
                    dirtyRenderer.markDirty(x, y, w, h);
                    if (graphicsContext != null) {
                        dirtyRenderer.render(graphicsContext, totalWidth, totalHeight);
                    }
                });
                
                AppEventBus.getInstance().publish(
                    new AppEvent(AppEventType.RENDERING_PROGRESS, this)
                        .withData("progress", progress)
                        .withData("renderedTiles", rendered)
                        .withData("totalTiles", totalTiles)
                );
                
                try {
                    Thread.sleep(DEFAULT_RENDER_INTERVAL_MS);
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                }
            }
        }
        
        rendering.set(false);
        
        AppEventBus.getInstance().publish(
            new AppEvent(AppEventType.RENDERING_COMPLETED, this)
                .withData("renderedTiles", renderedTiles.get())
        );
    }
    
    private void renderTile(ByteBuffer data, int x, int y, int width, int height, int totalWidth) {
        if (graphicsContext == null) {
            return;
        }
        
        Platform.runLater(() -> {
            graphicsContext.save();
            
            for (int py = y; py < y + height; py++) {
                for (int px = x; px < x + width; px++) {
                    int index = (py * totalWidth + px) * 4;
                    if (index + 3 < data.limit()) {
                        int r = data.get(index) & 0xFF;
                        int g = data.get(index + 1) & 0xFF;
                        int b = data.get(index + 2) & 0xFF;
                        int a = data.get(index + 3) & 0xFF;
                        
                        graphicsContext.setFill(javafx.scene.paint.Color.rgb(r, g, b, a / 255.0));
                        graphicsContext.fillRect(px, py, 1, 1);
                    }
                }
            }
            
            graphicsContext.restore();
        });
    }
    
    public void stopRendering() {
        rendering.set(false);
    }
    
    public boolean isRendering() {
        return rendering.get();
    }
    
    public double getProgress() {
        return tileCache.getCacheSize() > 0 ? 
            (double) renderedTiles.get() / tileCache.getCacheSize() : 0;
    }
    
    @Override
    protected void doInitialize() {
        dirtyRenderer.initialize();
        tileCache.initialize();
    }
    
    @Override
    protected void doDispose() {
        rendering.set(false);
        executor.shutdown();
        dirtyRenderer.dispose();
        tileCache.dispose();
    }
}
