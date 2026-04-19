package cn.cycle.echart.ui.render;

import javafx.scene.image.Image;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Objects;

/**
 * 瓦片缓存策略。
 * 
 * <p>实现LRU缓存策略，用于缓存渲染瓦片以提高性能。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class TileCache {

    private final int maxSize;
    private final Map<String, Tile> cache;
    private long hitCount;
    private long missCount;

    public TileCache(int maxSize) {
        this.maxSize = maxSize;
        this.cache = new LinkedHashMap<String, Tile>(maxSize, 0.75f, true) {
            @Override
            protected boolean removeEldestEntry(Map.Entry<String, Tile> eldest) {
                return size() > TileCache.this.maxSize;
            }
        };
        this.hitCount = 0;
        this.missCount = 0;
    }

    public TileCache() {
        this(256);
    }

    public synchronized Tile get(String key) {
        Objects.requireNonNull(key, "key cannot be null");
        
        Tile tile = cache.get(key);
        if (tile != null) {
            hitCount++;
            return tile;
        }
        
        missCount++;
        return null;
    }

    public synchronized void put(String key, Tile tile) {
        Objects.requireNonNull(key, "key cannot be null");
        Objects.requireNonNull(tile, "tile cannot be null");
        
        cache.put(key, tile);
    }

    public synchronized void remove(String key) {
        Objects.requireNonNull(key, "key cannot be null");
        cache.remove(key);
    }

    public synchronized void clear() {
        cache.clear();
        hitCount = 0;
        missCount = 0;
    }

    public synchronized int size() {
        return cache.size();
    }

    public synchronized boolean containsKey(String key) {
        Objects.requireNonNull(key, "key cannot be null");
        return cache.containsKey(key);
    }

    public double getHitRate() {
        long total = hitCount + missCount;
        if (total == 0) {
            return 0.0;
        }
        return (double) hitCount / total;
    }

    public long getHitCount() {
        return hitCount;
    }

    public long getMissCount() {
        return missCount;
    }

    public int getMaxSize() {
        return maxSize;
    }

    public static class Tile {
        private final String key;
        private final Image image;
        private final int x;
        private final int y;
        private final int zoom;
        private final long timestamp;

        public Tile(String key, Image image, int x, int y, int zoom) {
            this.key = key;
            this.image = image;
            this.x = x;
            this.y = y;
            this.zoom = zoom;
            this.timestamp = System.currentTimeMillis();
        }

        public String getKey() {
            return key;
        }

        public Image getImage() {
            return image;
        }

        public int getX() {
            return x;
        }

        public int getY() {
            return y;
        }

        public int getZoom() {
            return zoom;
        }

        public long getTimestamp() {
            return timestamp;
        }

        public long getAge() {
            return System.currentTimeMillis() - timestamp;
        }
    }

    public static String generateKey(int x, int y, int zoom) {
        return String.format("%d/%d/%d", zoom, x, y);
    }
}
