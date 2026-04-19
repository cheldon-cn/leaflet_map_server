package cn.cycle.echart.theme;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class ThemeManagerTest {

    private ThemeManager manager;

    @Before
    public void setUp() {
        manager = new ThemeManager();
    }

    @Test
    public void testGetAvailableThemes() {
        assertEquals(2, manager.getAvailableThemes().size());
    }

    @Test
    public void testGetTheme() {
        Theme light = manager.getTheme("light");
        assertNotNull(light);
        assertEquals("light", light.getName());
        
        Theme dark = manager.getTheme("dark");
        assertNotNull(dark);
        assertEquals("dark", dark.getName());
    }

    @Test
    public void testGetCurrentTheme() {
        assertNotNull(manager.getCurrentTheme());
    }

    @Test
    public void testSetCurrentTheme() {
        Theme dark = manager.getTheme("dark");
        manager.setCurrentTheme(dark);
        
        assertEquals(dark, manager.getCurrentTheme());
    }

    @Test
    public void testSetCurrentThemeByName() {
        manager.setCurrentTheme("dark");
        
        assertEquals("dark", manager.getCurrentTheme().getName());
    }

    @Test
    public void testHasTheme() {
        assertTrue(manager.hasTheme("light"));
        assertTrue(manager.hasTheme("dark"));
        assertFalse(manager.hasTheme("nonexistent"));
    }

    @Test
    public void testGetDefaultTheme() {
        assertNotNull(manager.getDefaultTheme());
    }

    @Test
    public void testResetToDefault() {
        manager.setCurrentTheme("dark");
        manager.resetToDefault();
        
        assertEquals("light", manager.getCurrentTheme().getName());
    }

    @Test
    public void testThemeChangeListener() {
        final boolean[] changed = {false};
        
        manager.addThemeChangeListener((oldTheme, newTheme) -> {
            changed[0] = true;
        });
        
        manager.setCurrentTheme("dark");
        
        assertTrue(changed[0]);
    }
}
