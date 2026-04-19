package cn.cycle.echart.theme;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class DarkThemeTest {

    private DarkTheme theme;

    @Before
    public void setUp() {
        theme = new DarkTheme();
    }

    @Test
    public void testGetName() {
        assertEquals("dark", theme.getName());
    }

    @Test
    public void testGetDisplayName() {
        assertEquals("深色主题", theme.getDisplayName());
    }

    @Test
    public void testIsDark() {
        assertTrue(theme.isDark());
    }

    @Test
    public void testGetColors() {
        assertNotNull(theme.getColors());
        assertFalse(theme.getColors().isEmpty());
    }

    @Test
    public void testGetColor() {
        assertEquals("#1E1E1E", theme.getColor("background"));
        assertEquals("#FFFFFF", theme.getColor("text.primary"));
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
        assertEquals("/themes/dark.css", theme.getStyleSheet());
    }
}
