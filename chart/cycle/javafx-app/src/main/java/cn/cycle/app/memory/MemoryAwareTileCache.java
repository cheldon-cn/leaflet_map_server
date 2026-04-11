package cn.cycle.app.memory;

import cn.cycle.app.render.TileCache;
import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import javafx.scene.image.Image;

import java.lang.management.ManagementFactory;
import java.lang.management.MemoryMXBean;
import java.lang.management.MemoryUsage;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class MemoryAwareTileCache extends AbstractLifecycleComponent {
    
    private static final long DEFAULT_MAX_MEMORY_BYTES = 256 * 1024 * 1024;
    private static final double MEMORY_THRESHOLD = 0.8;
    private static final long CHECK_INTERVAL_MS = 5000;
    
    private final long maxMemoryBytes;
    private long currentMemoryUsage = 0;
    private final LinkedHashMap<TileCache.TileKey, CacheEntry> cache;
    private final MemoryMXBean memoryBean;
    private final ScheduledExecutorService scheduler;
    
    public MemoryAwareTileCache() {
        this(DEFAULT_MAX_MEMORY_BYTES);
    }
    
    public MemoryAwareTileCache(long maxMemoryBytes) {
        this.maxMemoryBytes = maxMemoryBytes;
        this.cache = new LinkedHashMap<>(16, 0.75f, true);
        this.memoryBean = ManagementFactory.getMemoryMXBean();
        this.scheduler = Executors.newSingleThreadScheduledExecutor();
    }
    
    public Image get(TileCache.TileKey key) {
        synchronized (cache) {
            CacheEntry entry = cache.get(key);
            if (entry != null) {
                entry.lastAccessTime = System.currentTimeMillis();
                return entry.image;
            }
        }
        return null;
    }
    
    public void put(TileCache.TileKey key, Image image) {
        if (key == null || image == null) {
            return;
        }
        
        long imageSize = estimateImageSize(image);
        
        synchronized (cache) {
            while (currentMemoryUsage + imageSize > maxMemoryBytes && !cache.isEmpty()) {
                evictOldest();
            }
            
            CacheEntry entry = new CacheEntry(image, imageSize);
            cache.put(key, entry);
            currentMemoryUsage += imageSize;
        }
    }
    
    public boolean contains(TileCache.TileKey key) {
        synchronized (cache) {
            return cache.containsKey(key);
        }
    }
    
    public void remove(TileCache.TileKey key) {
        synchronized (cache) {
            CacheEntry entry = cache.remove(key);
            if (entry != null) {
                currentMemoryUsage -= entry.size;
            }
        }
    }
    
    public void clear() {
        synchronized (cache) {
            cache.clear();
            currentMemoryUsage = 0;
        }
    }
    
    public int size() {
        synchronized (cache) {
            return cache.size();
        }
    }
    
    public long getMemoryUsage() {
        return currentMemoryUsage;
    }
    
    public long getMaxMemory() {
        return maxMemoryBytes;
    }
    
    public double getMemoryUsageRatio() {
        return (double) currentMemoryUsage / maxMemoryBytes;
    }
    
    private void evictOldest() {
        if (cache.isEmpty()) {
            return;
        }
        
        Map.Entry<TileCache.TileKey, CacheEntry> oldest = null;
        for (Map.Entry<TileCache.TileKey, CacheEntry> entry : cache.entrySet()) {
            oldest = entry;
            break;
        }
        
        if (oldest != null) {
            cache.remove(oldest.getKey());
            currentMemoryUsage -= oldest.getValue().size;
        }
    }
    
    private long estimateImageSize(Image image) {
        if (image == null) {
            return 0;
        }
        int width = (int) image.getWidth();
        int height = (int) image.getHeight();
        return (long) width * height * 4;
    }
    
    private void checkMemoryPressure() {
        MemoryUsage heapUsage = memoryBean.getHeapMemoryUsage();
        double usedRatio = (double) heapUsage.getUsed() / heapUsage.getMax();
        
        if (usedRatio > MEMORY_THRESHOLD) {
            synchronized (cache) {
                int entriesToEvict = cache.size() / 4;
                for (int i = 0; i < entriesToEvict && !cache.isEmpty(); i++) {
                    evictOldest();
                }
            }
        }
    }
    
    @Override
    protected void doInitialize() {
        scheduler.scheduleAtFixedRate(
            this::checkMemoryPressure,
            CHECK_INTERVAL_MS,
            CHECK_INTERVAL_MS,
            TimeUnit.MILLISECONDS
        );
    }
    
    @Override
    protected void doDispose() {
        scheduler.shutdown();
        clear();
    }
    
    private static class CacheEntry {
        final Image image;
        final long size;
        long lastAccessTime;
        
        CacheEntry(Image image, long size) {
            this.image = image;
            this.size = size;
            this.lastAccessTime = System.currentTimeMillis();
        }
    }
}
