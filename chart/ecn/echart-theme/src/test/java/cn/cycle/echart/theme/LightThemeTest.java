package cn.cycle.echart.theme;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class LightThemeTest {

    private LightTheme theme;

    @Before
    public void setUp() {
        theme = new LightTheme();
    }

    @Test
    public void testGetName() {
        assertEquals("light", theme.getName());
    }

    @Test
    public void testGetDisplayName() {
        assertEquals("浅色主题", theme.getDisplayName());
    }

    @Test
    public void testIsDark() {
        assertFalse(theme.isDark());
    }

    @Test
    public void testGetColors() {
        assertNotNull(theme.getColors());
        assertFalse(theme.getColors().isEmpty());
    }

    @Test
    public void testGetColor() {
        assertEquals("#FFFFFF", theme.getColor("background"));
        assertEquals("#212121", theme.getColor("text.primary"));
    }

    @Test
    public void testGetFonts() {
        assertNotNull(theme.getFonts());
        assertFalse(theme.getFonts().isEmpty());
    }

    @Test
    public void testGetFont() {
        assertNotNull(theme.getFont("font.primary"));
    }

    @Test
    public void testGetStyleSheet() {
        assertEquals("/themes/light.css", theme.getStyleSheet());
    }
}
