package cn.cycle.app.integration;

import cn.cycle.app.i18n.I18nManager;
import cn.cycle.app.theme.ThemeManager;
import org.junit.Test;
import static org.junit.Assert.*;
import java.util.Locale;

public class ThemeI18nIntegrationTest {
    
    @Test
    public void testThemeManagerSingleton() {
        ThemeManager manager1 = ThemeManager.getInstance();
        ThemeManager manager2 = ThemeManager.getInstance();
        
        assertSame(manager1, manager2);
    }
    
    @Test
    public void testI18nManagerSingleton() {
        I18nManager manager1 = I18nManager.getInstance();
        I18nManager manager2 = I18nManager.getInstance();
        
        assertSame(manager1, manager2);
    }
    
    @Test
    public void testThemeRegistration() {
        ThemeManager manager = ThemeManager.getInstance();
        
        int initialCount = manager.getAvailableThemes().length;
        
        manager.registerTheme("custom", "/css/custom.css");
        
        assertTrue(manager.hasTheme("custom"));
        assertEquals(initialCount + 1, manager.getAvailableThemes().length);
    }
    
    @Test
    public void testLocaleSwitch() {
        I18nManager manager = I18nManager.getInstance();
        Locale originalLocale = manager.getLocale();
        
        manager.setLocale(Locale.CHINESE);
        assertEquals(Locale.CHINESE, manager.getLocale());
        
        manager.setLocale(Locale.ENGLISH);
        assertEquals(Locale.ENGLISH, manager.getLocale());
        
        manager.setLocale(Locale.JAPANESE);
        assertEquals(Locale.JAPANESE, manager.getLocale());
        
        manager.setLocale(originalLocale);
    }
    
    @Test
    public void testThemeAndLocaleIndependence() {
        ThemeManager themeManager = ThemeManager.getInstance();
        I18nManager i18nManager = I18nManager.getInstance();
        
        String originalTheme = themeManager.getCurrentTheme();
        Locale originalLocale = i18nManager.getLocale();
        
        themeManager.setTheme("dark");
        i18nManager.setLocale(Locale.CHINESE);
        
        assertEquals("dark", themeManager.getCurrentTheme());
        assertEquals(Locale.CHINESE, i18nManager.getLocale());
        
        themeManager.setTheme("light");
        assertEquals(Locale.CHINESE, i18nManager.getLocale());
        
        i18nManager.setLocale(Locale.ENGLISH);
        assertEquals("light", themeManager.getCurrentTheme());
        
        themeManager.setTheme(originalTheme);
        i18nManager.setLocale(originalLocale);
    }
    
    @Test
    public void testI18nCache() {
        I18nManager manager = I18nManager.getInstance();
        
        String value1 = manager.get("test.key");
        String value2 = manager.get("test.key");
        
        assertEquals(value1, value2);
        
        manager.clearCache();
        
        String value3 = manager.get("test.key");
        assertEquals(value1, value3);
    }
}
