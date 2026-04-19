package cn.cycle.echart.i18n;

import org.junit.Before;
import org.junit.Test;

import java.util.Locale;

import static org.junit.Assert.*;

public class I18nManagerTest {

    private I18nManager i18n;

    @Before
    public void setUp() {
        i18n = I18nManager.getInstance();
        i18n.setLocale(Locale.ENGLISH);
    }

    @Test
    public void testGetInstance() {
        I18nManager instance1 = I18nManager.getInstance();
        I18nManager instance2 = I18nManager.getInstance();
        
        assertSame(instance1, instance2);
    }

    @Test
    public void testGetMessage() {
        String title = i18n.getMessage("app.title");
        
        assertNotNull(title);
        assertFalse(title.isEmpty());
    }

    @Test
    public void testGetMessageWithArgs() {
        String greeting = i18n.getMessage("app.greeting", "World");
        
        assertTrue(greeting.contains("World"));
    }

    @Test
    public void testGetMessageNotFound() {
        String message = i18n.getMessage("nonexistent.key");
        
        assertEquals("nonexistent.key", message);
    }

    @Test
    public void testHasMessage() {
        assertTrue(i18n.hasMessage("app.title"));
        assertFalse(i18n.hasMessage("nonexistent.key"));
    }

    @Test
    public void testSetLocale() {
        i18n.setLocale(Locale.SIMPLIFIED_CHINESE);
        
        assertEquals(Locale.SIMPLIFIED_CHINESE, i18n.getLocale());
    }

    @Test
    public void testLocaleChangeListener() {
        final boolean[] notified = {false};
        
        LocaleProvider.LocaleChangeListener listener = (oldLocale, newLocale) -> {
            notified[0] = true;
        };
        
        i18n.addLocaleChangeListener(listener);
        i18n.setLocale(Locale.JAPANESE);
        
        assertTrue(notified[0]);
        
        i18n.removeLocaleChangeListener(listener);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSetNullLocale() {
        i18n.setLocale(null);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testGetNullMessage() {
        i18n.getMessage(null);
    }
}
