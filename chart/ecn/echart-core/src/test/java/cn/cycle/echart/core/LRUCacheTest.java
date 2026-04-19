package cn.cycle.echart.core;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class LRUCacheTest {

    private LRUCache<String, String> cache;

    @Before
    public void setUp() {
        cache = new LRUCache<>(3);
    }

    @Test
    public void testPutAndGet() {
        cache.put("key1", "value1");
        assertEquals("value1", cache.get("key1"));
    }

    @Test
    public void testEviction() {
        cache.put("key1", "value1");
        cache.put("key2", "value2");
        cache.put("key3", "value3");
        cache.put("key4", "value4");
        
        assertNull(cache.get("key1"));
        assertEquals("value2", cache.get("key2"));
        assertEquals("value3", cache.get("key3"));
        assertEquals("value4", cache.get("key4"));
    }

    @Test
    public void testLRUOrder() {
        cache.put("key1", "value1");
        cache.put("key2", "value2");
        cache.put("key3", "value3");
        
        cache.get("key1");
        
        cache.put("key4", "value4");
        
        assertNotNull(cache.get("key1"));
        assertNull(cache.get("key2"));
        assertNotNull(cache.get("key3"));
        assertNotNull(cache.get("key4"));
    }

    @Test
    public void testContainsKey() {
        cache.put("key1", "value1");
        assertTrue(cache.containsKey("key1"));
        assertFalse(cache.containsKey("key2"));
    }

    @Test
    public void testRemove() {
        cache.put("key1", "value1");
        assertEquals("value1", cache.remove("key1"));
        assertNull(cache.get("key1"));
    }

    @Test
    public void testClear() {
        cache.put("key1", "value1");
        cache.put("key2", "value2");
        cache.clear();
        
        assertEquals(0, cache.size());
        assertTrue(cache.isEmpty());
    }

    @Test
    public void testSize() {
        assertEquals(0, cache.size());
        
        cache.put("key1", "value1");
        assertEquals(1, cache.size());
        
        cache.put("key2", "value2");
        assertEquals(2, cache.size());
    }

    @Test
    public void testHitRate() {
        cache.put("key1", "value1");
        
        cache.get("key1");
        cache.get("key1");
        cache.get("key2");
        
        assertEquals(2, cache.getHitCount());
        assertEquals(1, cache.getMissCount());
        assertEquals(0.666, cache.getHitRate(), 0.01);
    }

    @Test
    public void testMaxSize() {
        assertEquals(3, cache.getMaxSize());
    }

    @Test(expected = IllegalArgumentException.class)
    public void testZeroMaxSize() {
        new LRUCache<String, String>(0);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testNegativeMaxSize() {
        new LRUCache<String, String>(-1);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testPutNullKey() {
        cache.put(null, "value");
    }

    @Test(expected = IllegalArgumentException.class)
    public void testPutNullValue() {
        cache.put("key", null);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testGetNullKey() {
        cache.get(null);
    }
}
