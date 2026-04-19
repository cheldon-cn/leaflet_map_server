package cn.cycle.echart.app;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class AppConfigTest {

    private AppConfig config;

    @Before
    public void setUp() {
        config = new AppConfig();
    }

    @Test
    public void testGetString() {
        String appName = config.getString("app.name");
        assertNotNull(appName);
        assertEquals("E-Chart Display and Alarm Application", appName);
    }

    @Test
    public void testGetStringWithDefault() {
        String value = config.getString("nonexistent.key", "default");
        assertEquals("default", value);
    }

    @Test
    public void testGetInt() {
        int cacheSize = config.getInt("chart.cacheSize");
        assertEquals(100, cacheSize);
    }

    @Test
    public void testGetIntWithDefault() {
        int value = config.getInt("nonexistent.key", 50);
        assertEquals(50, value);
    }

    @Test
    public void testGetBoolean() {
        boolean autoSave = config.getBoolean("workspace.autoSave");
        assertTrue(autoSave);
    }

    @Test
    public void testGetBooleanWithDefault() {
        boolean value = config.getBoolean("nonexistent.key", true);
        assertTrue(value);
    }

    @Test
    public void testSetString() {
        config.setString("test.key", "testValue");
        assertEquals("testValue", config.getString("test.key"));
    }

    @Test
    public void testSetInt() {
        config.setInt("test.int", 42);
        assertEquals(42, config.getInt("test.int"));
    }

    @Test
    public void testSetBoolean() {
        config.setBoolean("test.bool", false);
        assertFalse(config.getBoolean("test.bool"));
    }

    @Test
    public void testGetTheme() {
        String theme = config.getTheme();
        assertNotNull(theme);
    }

    @Test
    public void testSetTheme() {
        config.setTheme("dark");
        assertEquals("dark", config.getTheme());
    }

    @Test
    public void testGetLocale() {
        String locale = config.getLocale();
        assertNotNull(locale);
    }

    @Test
    public void testGetAutoSaveInterval() {
        int interval = config.getAutoSaveInterval();
        assertTrue(interval > 0);
    }

    @Test
    public void testGetChartCacheSize() {
        int size = config.getChartCacheSize();
        assertTrue(size > 0);
    }

    @Test
    public void testIsAlarmSoundEnabled() {
        boolean enabled = config.isAlarmSoundEnabled();
        assertNotNull(Boolean.valueOf(enabled));
    }

    @Test
    public void testGetAisUpdateInterval() {
        int interval = config.getAisUpdateInterval();
        assertTrue(interval > 0);
    }

    @Test
    public void testGetWindowWidth() {
        int width = config.getWindowWidth();
        assertTrue(width > 0);
    }

    @Test
    public void testGetWindowHeight() {
        int height = config.getWindowHeight();
        assertTrue(height > 0);
    }

    @Test
    public void testSetWindowSettings() {
        config.setWindowSettings(1920, 1080, false);
        assertEquals(1920, config.getWindowWidth());
        assertEquals(1080, config.getWindowHeight());
        assertFalse(config.isWindowMaximized());
    }
}
