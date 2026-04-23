package cn.cycle.echart.ui.service;

import java.io.File;
import java.util.LinkedHashMap;
import java.util.Map;

public class ChartCacheService {

    private static final int DEFAULT_MAX_CACHE_SIZE = 5;
    
    private static ChartCacheService instance;
    
    private final int maxCacheSize;
    private final Map<String, CacheEntry> cache;
    
    public static class CacheEntry {
        private final byte[] pixels;
        private final int width;
        private final int height;
        private final long timestamp;
        private final String filePath;
        
        public CacheEntry(byte[] pixels, int width, int height, String filePath) {
            this.pixels = pixels;
            this.width = width;
            this.height = height;
            this.filePath = filePath;
            this.timestamp = System.currentTimeMillis();
        }
        
        public byte[] getPixels() {
            return pixels;
        }
        
        public int getWidth() {
            return width;
        }
        
        public int getHeight() {
            return height;
        }
        
        public long getTimestamp() {
            return timestamp;
        }
        
        public String getFilePath() {
            return filePath;
        }
    }
    
    private ChartCacheService() {
        this(DEFAULT_MAX_CACHE_SIZE);
    }
    
    private ChartCacheService(int maxCacheSize) {
        this.maxCacheSize = maxCacheSize;
        this.cache = new LinkedHashMap<String, CacheEntry>(maxCacheSize, 0.75f, true) {
            private static final long serialVersionUID = 1L;
            
            @Override
            protected boolean removeEldestEntry(Map.Entry<String, CacheEntry> eldest) {
                return size() > maxCacheSize;
            }
        };
    }
    
    public static synchronized ChartCacheService getInstance() {
        if (instance == null) {
            instance = new ChartCacheService();
        }
        return instance;
    }
    
    public CacheEntry get(String filePath) {
        synchronized (cache) {
            return cache.get(filePath);
        }
    }
    
    public void put(String filePath, byte[] pixels, int width, int height) {
        synchronized (cache) {
            cache.put(filePath, new CacheEntry(pixels, width, height, filePath));
        }
    }
    
    public boolean contains(String filePath) {
        synchronized (cache) {
            return cache.containsKey(filePath);
        }
    }
    
    public void remove(String filePath) {
        synchronized (cache) {
            cache.remove(filePath);
        }
    }
    
    public void clear() {
        synchronized (cache) {
            cache.clear();
        }
    }
    
    public int size() {
        synchronized (cache) {
            return cache.size();
        }
    }
    
    public int getMaxCacheSize() {
        return maxCacheSize;
    }
    
    public String generateCacheKey(File file, int width, int height) {
        return file.getAbsolutePath() + "_" + width + "x" + height;
    }
    
    public String generateCacheKey(String filePath, int width, int height) {
        return filePath + "_" + width + "x" + height;
    }
}
