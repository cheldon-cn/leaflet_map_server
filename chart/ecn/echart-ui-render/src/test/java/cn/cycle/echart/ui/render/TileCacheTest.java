package cn.cycle.echart.ui.render;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class TileCacheTest {

    private TileCache cache;

    @Before
    public void setUp() {
        cache = new TileCache(3);
    }

    @Test
    public void testPutAndGet() {
        TileCache.Tile tile = new TileCache.Tile("0/0/0", null, 0, 0, 0);
        cache.put("0/0/0", tile);
        
        TileCache.Tile retrieved = cache.get("0/0/0");
        assertNotNull(retrieved);
        assertEquals("0/0/0", retrieved.getKey());
    }

    @Test
    public void testGetNonExistent() {
        TileCache.Tile tile = cache.get("nonexistent");
        assertNull(tile);
    }

    @Test
    public void testLRUEviction() {
        TileCache.Tile tile1 = new TileCache.Tile("1", null, 0, 0, 0);
        TileCache.Tile tile2 = new TileCache.Tile("2", null, 0, 0, 0);
        TileCache.Tile tile3 = new TileCache.Tile("3", null, 0, 0, 0);
        TileCache.Tile tile4 = new TileCache.Tile("4", null, 0, 0, 0);
        
        cache.put("1", tile1);
        cache.put("2", tile2);
        cache.put("3", tile3);
        cache.put("4", tile4);
        
        assertNull(cache.get("1"));
        assertNotNull(cache.get("2"));
        assertNotNull(cache.get("3"));
        assertNotNull(cache.get("4"));
    }

    @Test
    public void testRemove() {
        TileCache.Tile tile = new TileCache.Tile("test", null, 0, 0, 0);
        cache.put("test", tile);
        
        assertNotNull(cache.get("test"));
        
        cache.remove("test");
        assertNull(cache.get("test"));
    }

    @Test
    public void testClear() {
        TileCache.Tile tile1 = new TileCache.Tile("1", null, 0, 0, 0);
        TileCache.Tile tile2 = new TileCache.Tile("2", null, 0, 0, 0);
        
        cache.put("1", tile1);
        cache.put("2", tile2);
        
        assertEquals(2, cache.size());
        
        cache.clear();
        assertEquals(0, cache.size());
    }

    @Test
    public void testHitRate() {
        TileCache.Tile tile = new TileCache.Tile("test", null, 0, 0, 0);
        cache.put("test", tile);
        
        cache.get("test");
        cache.get("nonexistent");
        
        assertEquals(0.5, cache.getHitRate(), 0.001);
    }

    @Test
    public void testGenerateKey() {
        String key = TileCache.generateKey(10, 20, 5);
        assertEquals("5/10/20", key);
    }

    @Test
    public void testTileProperties() {
        TileCache.Tile tile = new TileCache.Tile("test", null, 100, 200, 10);
        
        assertEquals("test", tile.getKey());
        assertEquals(100, tile.getX());
        assertEquals(200, tile.getY());
        assertEquals(10, tile.getZoom());
        assertTrue(tile.getAge() >= 0);
    }
}
