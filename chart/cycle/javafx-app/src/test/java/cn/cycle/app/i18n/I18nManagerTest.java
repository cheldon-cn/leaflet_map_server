package cn.cycle.app.i18n;

import org.junit.Test;
import static org.junit.Assert.*;
import java.util.Locale;

public class I18nManagerTest {
    
    @Test
    public void testGetInstance() {
        I18nManager manager1 = I18nManager.getInstance();
        I18nManager manager2 = I18nManager.getInstance();
        
        assertSame(manager1, manager2);
    }
    
    @Test
    public void testDefaultLocale() {
        I18nManager manager = I18nManager.getInstance();
        
        assertNotNull(manager.getLocale());
    }
    
    @Test
    public void testLocaleChange() {
        I18nManager manager = I18nManager.getInstance();
        Locale originalLocale = manager.getLocale();
        
        manager.setLocale(Locale.CHINESE);
        assertEquals(Locale.CHINESE, manager.getLocale());
        
        manager.setLocale(Locale.ENGLISH);
        assertEquals(Locale.ENGLISH, manager.getLocale());
        
        if (originalLocale != null) {
            manager.setLocale(originalLocale);
        }
    }
    
    @Test
    public void testGetString() {
        I18nManager manager = I18nManager.getInstance();
        
        String value = manager.get("app.title");
        assertNotNull(value);
    }
    
    @Test
    public void testMissingKey() {
        I18nManager manager = I18nManager.getInstance();
        
        String value = manager.get("nonexistent.key");
        assertEquals("nonexistent.key", value);
    }
    
    @Test
    public void testGetWithArgs() {
        I18nManager manager = I18nManager.getInstance();
        
        String formatted = manager.get("test.format", "arg1", "arg2");
        assertNotNull(formatted);
    }
    
    @Test
    public void testGetOrDefault() {
        I18nManager manager = I18nManager.getInstance();
        
        String value = manager.getOrDefault("nonexistent.key", "default");
        assertEquals("default", value);
    }
    
    @Test
    public void testHasKey() {
        I18nManager manager = I18nManager.getInstance();
        
        assertFalse(manager.hasKey("nonexistent.key"));
    }
    
    @Test
    public void testClearCache() {
        I18nManager manager = I18nManager.getInstance();
        
        manager.get("test.key");
        manager.clearCache();
    }
}
