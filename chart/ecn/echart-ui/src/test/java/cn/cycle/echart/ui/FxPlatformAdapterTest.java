package cn.cycle.echart.ui;

import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.*;

public class FxPlatformAdapterTest {

    private FxPlatformAdapter adapter;

    @Before
    public void setUp() {
        adapter = new FxPlatformAdapter();
    }

    @Test
    public void testGetPlatformName() {
        assertEquals("JavaFX", adapter.getPlatformName());
    }

    @Test
    public void testIsUiThread() {
        assertFalse(adapter.isUiThread());
    }

    @Test
    public void testGetScreenDpi() {
        double dpi = adapter.getScreenDpi();
        assertTrue(dpi > 0);
    }

    @Test
    public void testGetScreenScale() {
        double scale = adapter.getScreenScale();
        assertTrue(scale > 0);
    }

    @Test
    public void testGetConfigDirectory() {
        String configDir = adapter.getConfigDirectory();
        assertNotNull(configDir);
        assertTrue(configDir.contains(".echart"));
    }

    @Test
    public void testGetLogDirectory() {
        String logDir = adapter.getLogDirectory();
        assertNotNull(logDir);
        assertTrue(logDir.contains(".echart"));
    }

    @Test
    public void testGetCacheDirectory() {
        String cacheDir = adapter.getCacheDirectory();
        assertNotNull(cacheDir);
        assertTrue(cacheDir.contains(".echart"));
    }
}
