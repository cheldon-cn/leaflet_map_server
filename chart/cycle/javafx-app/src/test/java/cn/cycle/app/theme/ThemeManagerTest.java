package cn.cycle.app.theme;

import org.junit.Test;
import static org.junit.Assert.*;

public class ThemeManagerTest {
    
    @Test
    public void testGetInstance() {
        ThemeManager manager1 = ThemeManager.getInstance();
        ThemeManager manager2 = ThemeManager.getInstance();
        
        assertSame(manager1, manager2);
    }
    
    @Test
    public void testDefaultTheme() {
        ThemeManager manager = ThemeManager.getInstance();
        
        assertNotNull(manager.getCurrentTheme());
    }
    
    @Test
    public void testAvailableThemes() {
        ThemeManager manager = ThemeManager.getInstance();
        
        assertNotNull(manager.getAvailableThemes());
        assertTrue(manager.getAvailableThemes().length > 0);
    }
    
    @Test
    public void testThemeChange() {
        ThemeManager manager = ThemeManager.getInstance();
        String originalTheme = manager.getCurrentTheme();
        
        for (String theme : manager.getAvailableThemes()) {
            manager.setTheme(theme);
            assertEquals(theme, manager.getCurrentTheme());
        }
        
        if (originalTheme != null) {
            manager.setTheme(originalTheme);
        }
    }
    
    @Test
    public void testHasTheme() {
        ThemeManager manager = ThemeManager.getInstance();
        
        assertTrue(manager.hasTheme("light"));
        assertTrue(manager.hasTheme("dark"));
        assertTrue(manager.hasTheme("blue"));
        assertFalse(manager.hasTheme("nonexistent"));
    }
    
    @Test
    public void testRegisterTheme() {
        ThemeManager manager = ThemeManager.getInstance();
        
        manager.registerTheme("test", "/css/test.css");
        assertTrue(manager.hasTheme("test"));
    }
}
