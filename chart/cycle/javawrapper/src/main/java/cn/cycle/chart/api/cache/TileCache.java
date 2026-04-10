package cn.cycle.chart.api.cache;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class TileCache {

    private final LinkedHashMap<String, CacheEntry> cache;
    private final int maxSize;
    private final long maxAge;
    private final ReadWriteLock lock;
    private long hits;
    private long misses;

    private static class CacheEntry {
        final byte[] data;
        final long timestamp;
        
        CacheEntry(byte[] data) {
            this.data = data;
            this.timestamp = System.currentTimeMillis();
        }
        
        boolean isExpired(long maxAge) {
            return maxAge > 0 && (System.currentTimeMillis() - timestamp) > maxAge;
        }
    }

    public TileCache() {
        this(1000, 30 * 60 * 1000);
    }

    public TileCache(int maxSize, long maxAge) {
        this.maxSize = maxSize;
        this.maxAge = maxAge;
        this.lock = new ReentrantReadWriteLock();
        this.hits = 0;
        this.misses = 0;
        this.cache = new LinkedHashMap<String, CacheEntry>(16, 0.75f, true) {
            @Override
            protected boolean removeEldestEntry(Map.Entry<String, CacheEntry> eldest) {
                return size() > TileCache.this.maxSize;
            }
        };
    }

    public byte[] get(TileKey key) {
        if (key == null) {
            return null;
        }
        return get(key.getKey());
    }

    public byte[] get(String key) {
        lock.readLock().lock();
        try {
            CacheEntry entry = cache.get(key);
            if (entry != null && !entry.isExpired(maxAge)) {
                hits++;
                return entry.data;
            }
            misses++;
            return null;
        } finally {
            lock.readLock().unlock();
        }
    }

    public void put(TileKey key, byte[] data) {
        if (key == null || data == null) {
            return;
        }
        put(key.getKey(), data);
    }

    public void put(String key, byte[] data) {
        if (key == null || data == null) {
            return;
        }
        lock.writeLock().lock();
        try {
            cache.put(key, new CacheEntry(data));
        } finally {
            lock.writeLock().unlock();
        }
    }

    public void remove(TileKey key) {
        if (key != null) {
            remove(key.getKey());
        }
    }

    public void remove(String key) {
        if (key == null) {
            return;
        }
        lock.writeLock().lock();
        try {
            cache.remove(key);
        } finally {
            lock.writeLock().unlock();
        }
    }

    public void clear() {
        lock.writeLock().lock();
        try {
            cache.clear();
            hits = 0;
            misses = 0;
        } finally {
            lock.writeLock().unlock();
        }
    }

    public void clearExpired() {
        lock.writeLock().lock();
        try {
            cache.entrySet().removeIf(entry -> entry.getValue().isExpired(maxAge));
        } finally {
            lock.writeLock().unlock();
        }
    }

    public int size() {
        lock.readLock().lock();
        try {
            return cache.size();
        } finally {
            lock.readLock().unlock();
        }
    }

    public boolean contains(TileKey key) {
        if (key == null) {
            return false;
        }
        return contains(key.getKey());
    }

    public boolean contains(String key) {
        if (key == null) {
            return false;
        }
        lock.readLock().lock();
        try {
            CacheEntry entry = cache.get(key);
            return entry != null && !entry.isExpired(maxAge);
        } finally {
            lock.readLock().unlock();
        }
    }

    public double getHitRate() {
        long total = hits + misses;
        return total > 0 ? (double) hits / total : 0.0;
    }

    public long getHits() {
        return hits;
    }

    public long getMisses() {
        return misses;
    }

    public int getMaxSize() {
        return maxSize;
    }

    public long getMaxAge() {
        return maxAge;
    }
}
