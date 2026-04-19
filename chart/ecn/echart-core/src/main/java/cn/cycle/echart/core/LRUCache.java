package cn.cycle.echart.core;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Objects;

/**
 * LRU（最近最少使用）缓存实现。
 * 
 * <p>提供固定大小的缓存，当缓存满时自动移除最近最少使用的条目。</p>
 * 
 * <h2>使用示例</h2>
 * <pre>{@code
 * LRUCache<String, Tile> cache = new LRUCache<>(100);
 * cache.put("tile-1", tile1);
 * Tile tile = cache.get("tile-1");
 * }</pre>
 * 
 * @param <K> 键类型
 * @param <V> 值类型
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class LRUCache<K, V> {

    private final LinkedHashMap<K, V> cache;
    private final int maxSize;
    private long hitCount;
    private long missCount;

    /**
     * 创建LRU缓存。
     * 
     * @param maxSize 最大容量，必须大于0
     * @throws IllegalArgumentException 如果maxSize小于等于0
     */
    public LRUCache(int maxSize) {
        if (maxSize <= 0) {
            throw new IllegalArgumentException("maxSize must be positive");
        }
        this.maxSize = maxSize;
        this.cache = new LinkedHashMap<K, V>(maxSize, 0.75f, true) {
            private static final long serialVersionUID = 1L;

            @Override
            protected boolean removeEldestEntry(Map.Entry<K, V> eldest) {
                return size() > LRUCache.this.maxSize;
            }
        };
        this.hitCount = 0;
        this.missCount = 0;
    }

    /**
     * 放入缓存。
     * 
     * @param key 键，不能为null
     * @param value 值，不能为null
     * @throws IllegalArgumentException 如果参数为null
     */
    public synchronized void put(K key, V value) {
        Objects.requireNonNull(key, "key cannot be null");
        Objects.requireNonNull(value, "value cannot be null");
        cache.put(key, value);
    }

    /**
     * 获取缓存。
     * 
     * @param key 键，不能为null
     * @return 值，如果不存在返回null
     * @throws IllegalArgumentException 如果key为null
     */
    public synchronized V get(K key) {
        Objects.requireNonNull(key, "key cannot be null");
        V value = cache.get(key);
        if (value != null) {
            hitCount++;
        } else {
            missCount++;
        }
        return value;
    }

    /**
     * 检查是否包含键。
     * 
     * @param key 键，不能为null
     * @return 如果包含返回true
     * @throws IllegalArgumentException 如果key为null
     */
    public synchronized boolean containsKey(K key) {
        Objects.requireNonNull(key, "key cannot be null");
        return cache.containsKey(key);
    }

    /**
     * 移除缓存。
     * 
     * @param key 键，不能为null
     * @return 被移除的值，如果不存在返回null
     * @throws IllegalArgumentException 如果key为null
     */
    public synchronized V remove(K key) {
        Objects.requireNonNull(key, "key cannot be null");
        return cache.remove(key);
    }

    /**
     * 清空缓存。
     */
    public synchronized void clear() {
        cache.clear();
        hitCount = 0;
        missCount = 0;
    }

    /**
     * 获取缓存大小。
     * 
     * @return 当前缓存条目数
     */
    public synchronized int size() {
        return cache.size();
    }

    /**
     * 获取最大容量。
     * 
     * @return 最大容量
     */
    public int getMaxSize() {
        return maxSize;
    }

    /**
     * 检查缓存是否为空。
     * 
     * @return 如果为空返回true
     */
    public synchronized boolean isEmpty() {
        return cache.isEmpty();
    }

    /**
     * 获取命中次数。
     * 
     * @return 命中次数
     */
    public long getHitCount() {
        return hitCount;
    }

    /**
     * 获取未命中次数。
     * 
     * @return 未命中次数
     */
    public long getMissCount() {
        return missCount;
    }

    /**
     * 获取命中率。
     * 
     * @return 命中率（0.0-1.0）
     */
    public synchronized double getHitRate() {
        long total = hitCount + missCount;
        if (total == 0) {
            return 0.0;
        }
        return (double) hitCount / total;
    }

    @Override
    public synchronized String toString() {
        return "LRUCache{" +
               "size=" + size() +
               ", maxSize=" + maxSize +
               ", hitRate=" + String.format("%.2f%%", getHitRate() * 100) +
               '}';
    }
}
