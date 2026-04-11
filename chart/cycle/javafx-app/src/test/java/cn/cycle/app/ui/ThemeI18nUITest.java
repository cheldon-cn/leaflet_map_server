package cn.cycle.app.ui;

import cn.cycle.app.theme.ThemeManager;
import cn.cycle.app.i18n.I18nManager;
import org.junit.Test;
import static org.junit.Assert.*;
import java.util.Locale;

public class ThemeI18nUITest {
    
    @Test
    public void testThemeManagerAvailableThemes() {
        ThemeManager manager = ThemeManager.getInstance();
        
        String[] themes = manager.getAvailableThemes();
        assertNotNull(themes);
        assertTrue(themes.length >= 3);
    }
    
    @Test
    public void testThemeManagerDefaultTheme() {
        ThemeManager manager = ThemeManager.getInstance();
        
        String currentTheme = manager.getCurrentTheme();
        assertNotNull(currentTheme);
        assertTrue(currentTheme.length() > 0);
    }
    
    @Test
    public void testThemeManagerHasBuiltInThemes() {
        ThemeManager manager = ThemeManager.getInstance();
        
        assertTrue("Should have light theme", manager.hasTheme("light"));
        assertTrue("Should have dark theme", manager.hasTheme("dark"));
        assertTrue("Should have blue theme", manager.hasTheme("blue"));
    }
    
    @Test
    public void testI18nManagerDefaultLocale() {
        I18nManager manager = I18nManager.getInstance();
        
        Locale locale = manager.getLocale();
        assertNotNull(locale);
    }
    
    @Test
    public void testI18nManagerGet() {
        I18nManager manager = I18nManager.getInstance();
        
        String value = manager.get("app.title");
        assertNotNull(value);
    }
    
    @Test
    public void testI18nManagerGetWithArgs() {
        I18nManager manager = I18nManager.getInstance();
        
        String formatted = manager.get("test.format", "arg1", "arg2");
        assertNotNull(formatted);
    }
    
    @Test
    public void testI18nManagerGetOrDefault() {
        I18nManager manager = I18nManager.getInstance();
        
        String value = manager.getOrDefault("nonexistent.key", "default");
        assertEquals("default", value);
    }
    
    @Test
    public void testThemeChange() {
        ThemeManager manager = ThemeManager.getInstance();
        String originalTheme = manager.getCurrentTheme();
        
        manager.setTheme("dark");
        assertEquals("dark", manager.getCurrentTheme());
        
        manager.setTheme("light");
        assertEquals("light", manager.getCurrentTheme());
        
        manager.setTheme(originalTheme);
    }
    
    @Test
    public void testLocaleChange() {
        I18nManager manager = I18nManager.getInstance();
        Locale originalLocale = manager.getLocale();
        
        manager.setLocale(Locale.CHINESE);
        assertEquals(Locale.CHINESE, manager.getLocale());
        
        manager.setLocale(Locale.ENGLISH);
        assertEquals(Locale.ENGLISH, manager.getLocale());
        
        manager.setLocale(originalLocale);
    }
}
