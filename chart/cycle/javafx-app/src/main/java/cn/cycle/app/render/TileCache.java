package cn.cycle.app.render;

import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import javafx.scene.image.Image;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class TileCache extends AbstractLifecycleComponent {
    
    private static final int DEFAULT_MAX_CACHE_SIZE = 100;
    private static final int DEFAULT_THREAD_POOL_SIZE = 2;
    
    private final int maxCacheSize;
    private final LRUCache<TileKey, Image> cache;
    private final ExecutorService executor;
    private final Map<TileKey, CompletableFuture<Image>> pendingLoads = new ConcurrentHashMap<>();
    
    public TileCache() {
        this(DEFAULT_MAX_CACHE_SIZE, DEFAULT_THREAD_POOL_SIZE);
    }
    
    public TileCache(int maxCacheSize, int threadPoolSize) {
        this.maxCacheSize = maxCacheSize;
        this.cache = new LRUCache<>(maxCacheSize);
        this.executor = Executors.newFixedThreadPool(threadPoolSize);
    }
    
    public Image getTile(TileKey key) {
        return cache.get(key);
    }
    
    public CompletableFuture<Image> getTileAsync(TileKey key) {
        Image cached = cache.get(key);
        if (cached != null) {
            return CompletableFuture.completedFuture(cached);
        }
        
        return pendingLoads.computeIfAbsent(key, k -> 
            CompletableFuture.supplyAsync(() -> {
                Image tile = loadTileFromSource(k);
                if (tile != null) {
                    synchronized (cache) {
                        cache.put(k, tile);
                    }
                }
                pendingLoads.remove(k);
                return tile;
            }, executor)
        );
    }
    
    public void putTile(TileKey key, Image image) {
        if (key != null && image != null) {
            synchronized (cache) {
                cache.put(key, image);
            }
        }
    }
    
    public boolean containsTile(TileKey key) {
        return cache.containsKey(key);
    }
    
    public void removeTile(TileKey key) {
        synchronized (cache) {
            cache.remove(key);
        }
    }
    
    public void preloadTiles(TileKey[] keys) {
        for (TileKey key : keys) {
            if (!containsTile(key)) {
                getTileAsync(key);
            }
        }
    }
    
    public void clearCache() {
        synchronized (cache) {
            cache.clear();
        }
        pendingLoads.clear();
    }
    
    public int getCacheSize() {
        return cache.size();
    }
    
    public int getMaxCacheSize() {
        return maxCacheSize;
    }
    
    protected Image loadTileFromSource(TileKey key) {
        return null;
    }
    
    @Override
    protected void doInitialize() {
    }
    
    @Override
    protected void doDispose() {
        executor.shutdown();
        pendingLoads.clear();
        cache.clear();
    }
    
    public static class TileKey {
        private final int x;
        private final int y;
        private final int z;
        private final String source;
        
        public TileKey(int x, int y, int z, String source) {
            this.x = x;
            this.y = y;
            this.z = z;
            this.source = source != null ? source : "";
        }
        
        public int getX() {
            return x;
        }
        
        public int getY() {
            return y;
        }
        
        public int getZ() {
            return z;
        }
        
        public String getSource() {
            return source;
        }
        
        @Override
        public boolean equals(Object obj) {
            if (this == obj) return true;
            if (obj == null || getClass() != obj.getClass()) return false;
            TileKey tileKey = (TileKey) obj;
            return x == tileKey.x && y == tileKey.y && z == tileKey.z && source.equals(tileKey.source);
        }
        
        @Override
        public int hashCode() {
            int result = x;
            result = 31 * result + y;
            result = 31 * result + z;
            result = 31 * result + source.hashCode();
            return result;
        }
        
        @Override
        public String toString() {
            return String.format("TileKey{x=%d, y=%d, z=%d, source='%s'}", x, y, z, source);
        }
    }
    
    private static class LRUCache<K, V> extends LinkedHashMap<K, V> {
        private final int maxSize;
        
        LRUCache(int maxSize) {
            super(maxSize, 0.75f, true);
            this.maxSize = maxSize;
        }
        
        @Override
        protected boolean removeEldestEntry(Map.Entry<K, V> eldest) {
            return size() > maxSize;
        }
    }
}
